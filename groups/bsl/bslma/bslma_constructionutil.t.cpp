// bslma_constructionutil.t.cpp                                       -*-C++-*-
#include <bslma_constructionutil.h>

#include <bslma_autodestructor.h>
#include <bslma_default.h>
#include <bslma_testallocator.h>

#include <bslmf_enableif.h>
#include <bslmf_isconvertible.h>
#include <bslmf_ispair.h>
#include <bslmf_movableref.h>

#include <bsls_bsltestutil.h>
#include <bsls_objectbuffer.h>

#include <utility>
#include <stdio.h>      // 'printf'
#include <stdlib.h>     // 'atoi'
#include <string.h>

using namespace BloombergLP;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// This component provides primitive operations to construct and destroy
// objects, abstracting the fact that the class constructors may or may not
// take an optional allocator argument of type 'bslma::Allocator *'.  These
// primitives allow one to write parameterized code (e.g., containers) in a
// manner that is independent of whether or not the template parameters take
// optional allocators.  In addition, the primitives use the most efficient
// implementation (e.g., bit-wise copy) whenever possible.
//
// The general concerns of this component are the proper detection of traits
// (using 'bslma::Allocator', using bit-wise copy) and the correct selection of
// the implementation.  Some of these concerns are addressed by the compilation
// (detecting the wrong traits will lead to compilation failure) and some
// others are addressed by runtime detection of values after evaluation.  A
// general mechanism used is to construct an object into a buffer previously
// initialized to some garbage value (usually 92).  For bit-wise copy, we use
// a fussy type which will modify its (internal and/or class-static) state upon
// invocation of the copy constructor, but not when copying bit-wise.
//-----------------------------------------------------------------------------
// [ 3] construct(T *dst, *a);                -- default constructor
// [ 4] construct(T *dst, const T& src, *a);  -- copy constructor
// [ 5] construct(T *dst, T&& src, *a);       -- move constructor
// [ 6] construct(T *dst, A[1--N]..., *a);    -- value constructor
// [ 7] destructiveMove(T *dst, ALLOCATOR *a, TARGET_TYPE *src);
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 8] USAGE EXAMPLE
// [ 2] TEST APPARATUS

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
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define PP(X) printf(#X " = %p\n", (void*)(X));
                                          // Print ptr identifier and value.

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

typedef bslma::ConstructionUtil Util;
typedef bslma::DestructionUtil  DestructionUtil;
typedef bslmf::MovableRefUtil   MoveUtil;

const int MOVED_FROM_VAL = 0x01d;

//=============================================================================
//                             CLASSES FOR TESTING
//-----------------------------------------------------------------------------

                             // =================
                             // class my_ClassDef
                             // =================

struct my_ClassDef {
    // Data members that give my_ClassX size and alignment.

    // DATA (exceptionally public, only in test driver)
    int               d_value;
    int              *d_data_p;
    bslma::Allocator *d_allocator_p;
};

// In optimized builds, some compilers will elide some of the operations in the
// destructors of the test classes defined below.  In order to force the
// compiler to retain all of the code in the destructors, we provide the
// following function that can be used to (conditionally) print out the state
// of a 'my_ClassDef' data member.  If the destructor calls this function as
// its last operation, then all values set in the destructor have visible
// side-effects, but non-verbose test runs do not have to be burdened with
// additional output.

static bool forceDestructorCall = false;

void dumpClassDefState(const my_ClassDef& def)
{
    if (forceDestructorCall) {
        printf("%p: %d %p %p\n",
               &def, def.d_value, def.d_data_p, def.d_allocator_p);
    }
}

                             // ===============
                             // class my_Class1
                             // ===============

class my_Class1 {
    // Class that doesn't take allocators.

    // DATA
    my_ClassDef d_def;

  public:
    // CREATORS
    explicit
    my_Class1(int v = 0)
    {
        d_def.d_value = v;
        d_def.d_allocator_p = 0;
    }

    my_Class1(const my_Class1& rhs)
    {
        d_def.d_value = rhs.d_def.d_value;
        d_def.d_allocator_p = 0;
    }

    my_Class1(bslmf::MovableRef<my_Class1> rhs)
    {
        my_Class1& lvalue = rhs;
        d_def.d_value = lvalue.d_def.d_value;
        lvalue.d_def.d_value = MOVED_FROM_VAL;
        d_def.d_allocator_p = 0;
    }

    ~my_Class1()
    {
        ASSERT(d_def.d_value != 91);
        d_def.d_value = 91;
        d_def.d_allocator_p = 0;
        dumpClassDefState(d_def);
    }

    my_Class1& operator=(const my_Class1& rhs)
    {
        d_def.d_value = rhs.d_def.d_value;
        return *this;
    }

    // ACCESSORS
    int value() const { return d_def.d_value; }
};

                             // ===============
                             // class my_Class2
                             // ===============

class my_Class2 {
    // Class that takes allocators.

    // DATA
    my_ClassDef d_def;

  public:
    // CREATORS
    explicit
    my_Class2(bslma::Allocator *a = 0)
    {
        d_def.d_value = 0;
        d_def.d_allocator_p = a;
    }

    explicit
    my_Class2(int v, bslma::Allocator *a = 0)
    {
        d_def.d_value = v;
        d_def.d_allocator_p = a;
    }

    my_Class2(const my_Class2& rhs, bslma::Allocator *a = 0)
    {
        d_def.d_value = rhs.d_def.d_value;
        d_def.d_allocator_p = a;
    }

    my_Class2(bslmf::MovableRef<my_Class2> rhs, bslma::Allocator *a = 0)
    {
        my_Class2& lvalue = rhs;
        d_def.d_value = lvalue.d_def.d_value;
        lvalue.d_def.d_value = MOVED_FROM_VAL;
        if (a) {
            d_def.d_allocator_p = a;
        }
        else {
            d_def.d_allocator_p = lvalue.d_def.d_allocator_p;
        }
    }

    ~my_Class2()
    {
        ASSERT(d_def.d_value != 92);
        d_def.d_value = 92;
        d_def.d_allocator_p = 0;
        dumpClassDefState(d_def);
    }

    // MANIPULATORS
    my_Class2& operator=(const my_Class2& rhs)
    {
        d_def.d_value = rhs.d_def.d_value;
        // do not touch allocator!
        return *this;
    }

    // ACCESSORS
    int value() const { return d_def.d_value; }
};

// TRAITS
namespace BloombergLP {
namespace bslma {

template <>
struct UsesBslmaAllocator<my_Class2> : bsl::true_type {};

}  // close namespace bslma
}  // close enterprise namespace

                                 // ==========
                                 // my_Class2a
                                 // ==========

class my_Class2a {
    // This 'class' behaves the same as 'my_Class2' (allocator-aware type that
    // never actually allocates memory) except that it uses the
    // 'allocator_arg_t' idiom for passing an allocator to constructors.

    // DATA
    my_Class2 d_data;

  public:
    // CREATORS
    my_Class2a()
    : d_data()
    {
    }

    my_Class2a(bsl::allocator_arg_t, bslma::Allocator *a)
    : d_data(a)
    {
    }

    explicit
    my_Class2a(int v)
    : d_data(v)
    {
    }

    my_Class2a(bsl::allocator_arg_t, bslma::Allocator *a, int v)
    : d_data(v, a)
    {
    }

    my_Class2a(const my_Class2a& rhs)
    : d_data(rhs.d_data)
    {
    }

    my_Class2a(bsl::allocator_arg_t  ,
               bslma::Allocator     *a,
               const my_Class2a&     rhs)
    : d_data(rhs.d_data, a)
    {
    }

    my_Class2a(bslmf::MovableRef<my_Class2a> rhs)                   // IMPLICIT
    : d_data(MoveUtil::move(MoveUtil::access(rhs).d_data))
    {
    }

    my_Class2a(bsl::allocator_arg_t,
               bslma::Allocator              *a,
               bslmf::MovableRef<my_Class2a>  rhs)
    : d_data(MoveUtil::move(MoveUtil::access(rhs).d_data), a)
    {
    }

    // MANIPULATORS
    my_Class2a& operator=(const my_Class2a& rhs)
    {
        d_data.operator=(rhs.d_data);
        return *this;
    }

    my_Class2a& operator=(bslmf::MovableRef<my_Class2a> rhs)
    {
        d_data.operator=(MoveUtil::move(MoveUtil::access(rhs).d_data));
        return *this;
    }

    // ACCESSORS
    int value() const { return d_data.value(); }
};

// TRAITS
namespace BloombergLP {

namespace bslma {
template <> struct UsesBslmaAllocator<my_Class2a> : bsl::true_type {};
}  // close namespace bslma

namespace bslmf {
template <> struct UsesAllocatorArgT<my_Class2a> : bsl::true_type {};

}  // close namespace bslmf
}  // close enterprise namespace

                             // ===============
                             // class my_Class3
                             // ===============

class my_Class3 {
    // This 'class' takes allocators similarly to 'my_Class2', but does not
    // have an explicit move constructor (moves call the corresponding copy
    // operation).

    // DATA
    my_ClassDef d_def;

  public:
    // CREATORS
    explicit
    my_Class3(bslma::Allocator *a = 0)
    {
        d_def.d_value = 0;
        d_def.d_allocator_p = a;
    }

    explicit
    my_Class3(int v, bslma::Allocator *a = 0)
    {
        d_def.d_value = v;
        d_def.d_allocator_p = a;
    }

    my_Class3(const my_Class3& rhs, bslma::Allocator *a = 0)
    {
        d_def.d_value = rhs.d_def.d_value;
        d_def.d_allocator_p = a;
    }

    ~my_Class3()
    {
        ASSERT(d_def.d_value != 93);
        d_def.d_value = 93;
        d_def.d_allocator_p = 0;
    }

    // MANIPULATORS
    my_Class3& operator=(const my_Class3& rhs)
    {
        d_def.d_value = rhs.d_def.d_value;
        // Do not touch allocator!
        return *this;
    }

    // ACCESSORS
    int value() const { return d_def.d_value; }
};

// TRAITS
namespace BloombergLP {
namespace bslma {

template <>
struct UsesBslmaAllocator<my_Class3> : bsl::true_type {};

}  // close namespace bslma
}  // close enterprise namespace

                             // ===================
                             // class my_ClassFussy
                             // ===================

class my_ClassFussy {
    // Class that doesn't take allocators.

    // DATA
    my_ClassDef d_def;

  public:
    // CLASS DATA
    static int s_conversionConstructorInvocations;
    static int s_destructorInvocations;

    // CLASS METHODS
    static void *operator new(std::size_t size)
        // Should never be invoked.
    {
        BSLS_ASSERT_OPT(0);

        return ::operator new(size);
    }

    static void *operator new(std::size_t /* size */, void *ptr)
        // Should never be invoked.
    {
        BSLS_ASSERT_OPT(0);

        return ptr;
    }

    static void operator delete(void * /* ptr */)
        // Should never be invoked.
    {
        BSLS_ASSERT_OPT(0);
    }

    // CREATORS
    my_ClassFussy()
    {
        // Should never be invoked by 'ConstructionUtil'.

        BSLS_ASSERT_OPT(0);
    }

    // deliberately not explicit
    my_ClassFussy(int v)
    {
        ++s_conversionConstructorInvocations;

        d_def.d_value       = v;
        d_def.d_allocator_p = 0;
    }

    my_ClassFussy(const my_ClassFussy& /* rhs */)
    {
        // Should never be invoked by 'ConstructionUtil'.

        BSLS_ASSERT_OPT(0);
    }

    my_ClassFussy(bslmf::MovableRef<my_ClassFussy> /* rhs */)
    {
        // Should never be invoked by 'ConstructionUtil'.

        BSLS_ASSERT_OPT(0);
    }

    ~my_ClassFussy()
    {
        // Should never be invoked by 'ConstructionUtil'.
        ++s_destructorInvocations;

        // 'dumpClassDefState' is not called here because a default-constructed
        // 'my_ClassFussy' object may leave 'd_def' uninitialized.
    }

    // MANIPULATORS
    my_ClassFussy& operator=(const my_ClassFussy& /* rhs */)
    {
        // Should never be invoked by 'ConstructionUtil'.

        BSLS_ASSERT_OPT(0);

        return *this;
    }
};

// CLASS DATA
int my_ClassFussy::s_conversionConstructorInvocations = 0;
int my_ClassFussy::s_destructorInvocations            = 0;

// TRAITS
namespace bsl {

template <>
struct is_trivially_copyable<my_ClassFussy> : true_type {};

template <>
struct is_trivially_default_constructible<my_ClassFussy> : true_type {};

}  // close namespace bsl

                                 // =========
                                 // my_Class4
                                 // =========

class my_Class4 {
    // Class that takes allocators, and that actually allocates (for use in
    // exception testing).

    // DATA
    my_ClassDef d_def;

  public:
    // CREATORS
    my_Class4(bslma::Allocator *a = 0)
    {
        d_def.d_allocator_p = bslma::Default::allocator(a);
        d_def.d_data_p = (int*)(d_def.d_allocator_p)->allocate(sizeof(int));
        d_def.d_value = 0;
        *d_def.d_data_p = d_def.d_value;
    }

    my_Class4(int v, bslma::Allocator *a = 0)
    {
        d_def.d_allocator_p = bslma::Default::allocator(a);
        d_def.d_data_p = (int*)(d_def.d_allocator_p)->allocate(sizeof(int));
        d_def.d_value = v;
        *d_def.d_data_p = d_def.d_value;
    }

    my_Class4(const my_Class4& rhs, bslma::Allocator *a = 0)
    {
        d_def.d_allocator_p = bslma::Default::allocator(a);
        d_def.d_data_p = (int*)(d_def.d_allocator_p)->allocate(sizeof(int));
        d_def.d_value = rhs.d_def.d_value;
        *d_def.d_data_p = d_def.d_value;
    }

    my_Class4(bslmf::MovableRef<my_Class4> rhs, bslma::Allocator *a = 0)
    {
        my_Class4& lvalue = rhs;
        if (a) {
            d_def.d_allocator_p = a;
        }
        else {
            d_def.d_allocator_p = lvalue.d_def.d_allocator_p;
        }
        d_def.d_data_p  = (int*)(d_def.d_allocator_p)->allocate(sizeof(int));
        d_def.d_value   = lvalue.d_def.d_value;
        *d_def.d_data_p = d_def.d_value;
        lvalue.d_def.d_value   = MOVED_FROM_VAL;
        *lvalue.d_def.d_data_p = lvalue.d_def.d_value;
    }

    ~my_Class4()
    {
        ASSERT(d_def.d_value != 94);
        ASSERT(*d_def.d_data_p == d_def.d_value);
        *d_def.d_data_p = 94;
        d_def.d_value = 94;
        (d_def.d_allocator_p)->deallocate(d_def.d_data_p);
        d_def.d_data_p = 0;
        d_def.d_allocator_p = 0;
        dumpClassDefState(d_def);
    }

    // MANIPULATORS
    my_Class4& operator=(const my_Class4& rhs)
    {
        int *tmp = (int*)(d_def.d_allocator_p)->allocate(sizeof(int));
        *tmp = rhs.d_def.d_value;
        // commit point:
        (d_def.d_allocator_p)->deallocate(d_def.d_data_p);
        d_def.d_data_p = tmp;
        d_def.d_value = rhs.d_def.d_value;
        return *this;
    }

    // ACCESSORS
    int value() const { return d_def.d_value; }
};

// TRAITS
namespace BloombergLP {
namespace bslma {

template <>
struct UsesBslmaAllocator<my_Class4> : bsl::true_type {};

}  // close namespace bslma
}  // close enterprise namespace

                             // ====================
                             // class my_ClassFussy2
                             // ====================

class my_ClassFussy2 {
    // This class does not take allocators, is bitwise movable, and does not
    // provide a move constructor.

    // DATA
    my_ClassDef d_def;

  public:
    // CLASS DATA
    static int s_copyConstructorInvocations;
    static int s_conversionConstructorInvocations;
    static int s_destructorInvocations;

    // CLASS METHODS
    static void *operator new(std::size_t size)
        // Should never be invoked.
    {
        BSLS_ASSERT_OPT(0);

        return ::operator new(size);
    }

    static void *operator new(std::size_t /* size */, void *ptr)
        // Should never be invoked.
    {
        BSLS_ASSERT_OPT(0);

        return ptr;
    }

    static void operator delete(void * /* ptr */)
        // Should never be invoked.
    {
        BSLS_ASSERT_OPT(0);
    }

    // CREATORS
    my_ClassFussy2()
    {
        // Should never be invoked by 'ConstructionUtil'.

        BSLS_ASSERT_OPT(0);
    }

    // deliberately not explicit
    my_ClassFussy2(int v)
    {
        ++s_conversionConstructorInvocations;

        d_def.d_value       = v;
        d_def.d_allocator_p = 0;
    }

    my_ClassFussy2(const my_ClassFussy2& rhs)
    {
        ++s_copyConstructorInvocations;

        d_def.d_value       = rhs.d_def.d_value;
        d_def.d_allocator_p = rhs.d_def.d_allocator_p;
    }

    ~my_ClassFussy2()
    {
        // Should never be invoked by 'ConstructionUtil'.
        ++s_destructorInvocations;

        // 'dumpClassDefState' is not called here because a default-constructed
        // 'my_ClassFussy2' object may leave 'd_def' uninitialized.
    }

    // MANIPULATORS
    my_ClassFussy2& operator=(const my_ClassFussy2& /* rhs */)
    {
        // Should never be invoked by 'ConstructionUtil'.

        BSLS_ASSERT_OPT(0);

        return *this;
    }
};

// CLASS DATA
int my_ClassFussy2::s_copyConstructorInvocations       = 0;
int my_ClassFussy2::s_conversionConstructorInvocations = 0;
int my_ClassFussy2::s_destructorInvocations            = 0;

// TRAITS
namespace bsl {

template <>
struct is_trivially_default_constructible<my_ClassFussy2> : true_type {};

}  // close namespace bsl

namespace BloombergLP {
namespace bslmf {

template <>
struct IsBitwiseMoveable<my_ClassFussy2> : bsl::true_type {};

}  // close namespace bslmf
}  // close enterprise namespace

                             // =============
                             // class my_Pair
                             // =============

template <class T1, class T2>
struct my_Pair {
    // Test pair type without allocators.

    // TYPES
    typedef T1 first_type;
    typedef T2 second_type;

    // DATA (public for pair types)
    T1 first;
    T2 second;

    // CREATORS
    my_Pair()
    {
    }

    my_Pair(const T1& f, const T2& s)
    : first(f)
    , second(s)
    {
    }

    my_Pair(const my_Pair& other)
    : first(other.first)
    , second(other.second)
    {
    }

    template <class U1, class U2>
    my_Pair(const my_Pair<U1, U2>& other)
    : first(other.first)
    , second(other.second)
    {
    }

    my_Pair(bslmf::MovableRef<my_Pair> other)
    : first(bslmf::MovableRefUtil::move(other.first))
    , second(bslmf::MovableRefUtil::move(other.second))
    {
    }

    template <typename U1, typename U2>
    my_Pair(bslmf::MovableRef<my_Pair<U1, U2> > other)
    : first(bslmf::MovableRefUtil::move(
            bslmf::MovableRefUtil::access(other).first))
    , second(bslmf::MovableRefUtil::move(
             bslmf::MovableRefUtil::access(other).second))
    {
    }

    my_Pair& operator=(const my_Pair& rhs)
    {
        first = rhs.first;
        second = rhs.second;
        return *this;
    }
};

                               // ==============
                               // class my_PairA
                               // ==============

template <class T1, class T2>
struct my_PairA {
    // Test pair type with mixed allocator and non-allocator.
    // Only T2 must use allocators.  We assume that the treatment of T1 and T2
    // in the component is symmetric, and do not bother with the symmetric test
    // pair type.

    // TYPES
    typedef T1 first_type;
    typedef T2 second_type;

    // DATA (public for pair types)
    T1 first;
    T2 second;

    // CREATORS
    explicit
    my_PairA(bslma::Allocator *a = 0)
    : first()
    , second(a)
    {
    }

    my_PairA(const T1& f, const T2& s, bslma::Allocator *a = 0)
    : first(f)
    , second(s, a)
    {
    }

    my_PairA(const my_PairA& other, bslma::Allocator *a = 0)
    : first(other.first)
    , second(other.second, a)
    {
    }

    template <class U1, class U2>
    my_PairA(const my_PairA<U1, U2>& other, bslma::Allocator *a = 0)
    : first(other.first)
    , second(other.second, a)
    {
    }

    my_PairA(bslmf::MovableRef<my_PairA> other, bslma::Allocator *a = 0)
    : first(bslmf::MovableRefUtil::move(other.first))
    , second(bslmf::MovableRefUtil::move(other.second), a)
    {
    }

    template <typename U1, typename U2>
    my_PairA(bslmf::MovableRef<my_PairA<U1, U2> >  other,
             bslma::Allocator                     *a = 0)
    : first(bslmf::MovableRefUtil::move(
            bslmf::MovableRefUtil::access(other).first))
    , second(bslmf::MovableRefUtil::move(
                 bslmf::MovableRefUtil::access(other).second),
             a)
    {
    }

    my_PairA& operator=(const my_PairA& rhs)
    {
        first = rhs.first;
        second = rhs.second;
        return *this;
    }
};

namespace BloombergLP {
namespace bslma {

template <class T1, class T2>
struct UsesBslmaAllocator<my_PairA<T1, T2> > : bsl::true_type {};

}  // close namespace bslma
}  // close enterprise namespace

                              // ===============
                              // class my_PairAA
                              // ===============

template <class T1, class T2>
struct my_PairAA {
    // Test pair type with allocators.
    // Both T1 and T2 must use allocators.

    // TYPES
    typedef T1 first_type;
    typedef T2 second_type;

    // DATA (public for pair types)
    T1 first;
    T2 second;

    // CREATORS
    explicit
    my_PairAA(bslma::Allocator *a = 0)
    : first(a)
    , second(a)
    {
    }

    my_PairAA(const T1& f, const T2& s, bslma::Allocator *a = 0)
    : first(f, a)
    , second(s, a)
    {
    }

    my_PairAA(const my_PairAA& other, bslma::Allocator *a = 0)
    : first(other.first, a)
    , second(other.second, a)
    {
    }

    template <class U1, class U2>
    my_PairAA(const my_PairAA<U1, U2>& other, bslma::Allocator *a = 0)
    : first(other.first, a)
    , second(other.second, a)
    {
    }

    my_PairAA(bslmf::MovableRef<my_PairAA> other, bslma::Allocator *a = 0)
    : first(bslmf::MovableRefUtil::move(
                bslmf::MovableRefUtil::access(other).first),
            a)
    , second(bslmf::MovableRefUtil::move(
                 bslmf::MovableRefUtil::access(other).second),
             a)
    {
    }

    template <typename U1, typename U2>
    my_PairAA(bslmf::MovableRef<my_PairAA<U1, U2> >  other,
              bslma::Allocator                      *a = 0)
    : first(bslmf::MovableRefUtil::move(
                bslmf::MovableRefUtil::access(other).first),
            a)
    , second(bslmf::MovableRefUtil::move(
                 bslmf::MovableRefUtil::access(other).second),
             a)
    {
    }

    my_PairAA& operator=(const my_PairAA& rhs)
    {
        first = rhs.first;
        second = rhs.second;
        return *this;
    }
};

namespace BloombergLP {
namespace bslma {

template <class T1, class T2>
struct  UsesBslmaAllocator<my_PairAA<T1, T2> > : bsl::true_type  {};

}  // close namespace bslma
}  // close enterprise namespace

                              // ===============
                              // class my_PairBB
                              // ===============

template <class T1, class T2>
struct my_PairBB {
    // Test pair type with the 'IsPair' trait.  This type is used to test that
    // this trait is NOT used by the construction utility to separately
    // construct pair's elements (without calling any of the pair's
    // constructors).

    // TYPES
    typedef T1 first_type;
    typedef T2 second_type;

    // DATA (public for pair types)
    T1 first;
    T2 second;

    // CREATORS
    my_PairBB()
    {
    }

    my_PairBB(const T1& f, const T2& s)
    : first(f)
    , second(s)
    {
    }

    my_PairBB(const my_PairBB& other)
    : first(other.first)
    , second(other.second)
    {
    }

    template <class U1, class U2>
    my_PairBB(const my_PairBB<U1, U2>& other)
    : first(other.first)
    , second(other.second)
    {
    }

    my_PairBB(bslmf::MovableRef<my_PairBB> other)
    : first(bslmf::MovableRefUtil::move(
                bslmf::MovableRefUtil::access(other).first))
    , second(bslmf::MovableRefUtil::move(
                bslmf::MovableRefUtil::access(other).second))
    {
    }

    template <typename U1, typename U2>
    my_PairBB(bslmf::MovableRef<my_PairBB<U1, U2> > other)
    : first(bslmf::MovableRefUtil::move(
                bslmf::MovableRefUtil::access(other).first))
    , second(bslmf::MovableRefUtil::move(
                 bslmf::MovableRefUtil::access(other).second))
    {
    }

    my_PairBB& operator=(const my_PairBB& rhs)
    {
        first  = rhs.first;
        second = rhs.second;
        return *this;
    }
};

namespace BloombergLP {
namespace bslmf {

template <class T1, class T2>
struct IsPair<my_PairBB<T1, T2> > : bsl::true_type {};

}  // close namespace bslmf
}  // close enterprise namespace

                              // ===============
                              // macros TEST_OP*
                              // ===============

#define TEST_OP(typeNum, op, expVal, expAlloc) {                              \
    typedef my_Class ## typeNum Type;                                         \
    static const int EXP_VAL = (expVal);                                      \
    bslma::Allocator *const EXP_ALLOC = (expAlloc);                           \
    my_ClassDef rawBuf;                                                       \
    memset(&rawBuf, 92, sizeof(rawBuf));                                      \
    Type *objPtr = (Type*) &rawBuf;                                           \
    pre(&rawBuf);                                                             \
    Util:: op ;                                                               \
    post(&rawBuf);                                                            \
    ASSERT(EXP_VAL   == rawBuf.d_value);                                      \
    ASSERT(EXP_ALLOC == rawBuf.d_allocator_p);                                \
  }
    // This macro evaluates the specified 'op' expression in the namespace
    // under test, involving the address 'objPtr' of an object of type
    // 'my_ClassN' (where 'N' stands for the specified 'typeNum'), and verifies
    // that the 'd_value' and 'd_allocator_p' members store the specified
    // 'expVal' and 'expAlloc' values after 'op' has been evaluated.

#define TEST_PAIR(op, expVal0, expA0, expVal1, expA1) {                       \
    static const int EXP_VAL0 = (expVal0);                                    \
    bslma::Allocator *const EXP_ALLOC0 = (expA0);                             \
    static const int EXP_VAL1 = (expVal1);                                    \
    bslma::Allocator *const EXP_ALLOC1 = (expA1);                             \
    my_ClassDef rawBuf[2];                                                    \
    memset(rawBuf, 92, sizeof(rawBuf));                                       \
    Type *objPtr = (Type*) rawBuf;                                            \
    pre(rawBuf);                                                              \
    Util:: op ;                                                               \
    post(rawBuf);                                                             \
    ASSERT(EXP_VAL0   == rawBuf[0].d_value);                                  \
    ASSERT(EXP_ALLOC0 == rawBuf[0].d_allocator_p);                            \
    ASSERT(EXP_VAL1   == rawBuf[1].d_value);                                  \
    ASSERT(EXP_ALLOC1 == rawBuf[1].d_allocator_p);                            \
  }
    // This macro evaluates the specified 'op' expression in the namespace
    // under test, involving the address 'objPtr' of an object of the locally
    // defined 'Type', and verifies that the 'd_value' and 'd_allocator_p'
    // members store the specified 'expVal0' and 'expAlloc0' values for the
    // first member and 'expVal0' and 'expAlloc0' values for the second member
    // after 'op' has been evaluated.

#define TEST_PAIROP(typeNum0, typeNum1, op, expVal0, expA0, expVal1, expA1) { \
    typedef my_Pair_ ## typeNum0 ## _ ## typeNum1 Type;                       \
    TEST_PAIR(op, expVal0, expA0, expVal1, expA1);                            \
  }
    // This macro evaluates the specified 'op' expression in the namespace
    // under test, involving the address 'objPtr' of an object of type
    // 'my_Pair_N0_N1' (where 'N0' and 'N1' stand for the specified 'typeNum0
    // and 'typeNum1').  See the 'TEST_PAIR' macro above for details.

#define TEST_PAIRAOP(typeNum0, typeNum1, op, expVal0, expA0, expVal1, expA1)  \
  {                                                                           \
    typedef my_PairA_ ## typeNum0 ## _ ## typeNum1 Type;                      \
    TEST_PAIR(op, expVal0, expA0, expVal1, expA1);                            \
  }
    // This macro evaluates the specified 'op' expression in the namespace
    // under test, involving the address 'objPtr' of an object of type
    // 'my_Pair_N0_N1' (where 'N0' and 'N1' stand for the specified 'typeNum0
    // and 'typeNum1').  See the 'TEST_PAIR' macro above for details.

#define TEST_PAIRAAOP(typeNum0, typeNum1, op, expVal0, expA0, expVal1, expA1) \
  {                                                                           \
    typedef my_PairAA_ ## typeNum0 ## _ ## typeNum1 Type;                     \
    TEST_PAIR(op, expVal0, expA0, expVal1, expA1);                            \
  }
    // This macro evaluates the specified 'op' expression in the namespace
    // under test, involving the address 'objPtr' of an object of type
    // 'my_PairAA_N0_N1' (where 'N0' and 'N1' stand for the specified 'typeNum0
    // and 'typeNum1').  See the 'TEST_PAIR' macro above for details.

#define TEST_PAIRBBOP(typeNum0, typeNum1, op, expVal0, expA0, expVal1, expA1) \
  {                                                                           \
    typedef my_PairBB_ ## typeNum0 ## _ ## typeNum1 Type;                     \
    TEST_PAIR(op, expVal0, expA0, expVal1, expA1);                            \
  }
    // This macro evaluates the specified 'op' expression in the namespace
    // under test, involving the address 'objPtr' of an object of type
    // 'my_PairBB_N0_N1' (where 'N0' and 'N1' stand for the specified 'typeNum0
    // and 'typeNum1').  See the 'TEST_PAIR' macro above for details.

                              // ===============
                              // macros TEST_MV*
                              // ===============

#define ASSERT_IS_MOVED_FROM(x)                                               \
  {                                                                           \
    ASSERT(MOVED_FROM_VAL == x.value());                                      \
  }
    // This macro reads the value of the specified 'x' and asserts that it is
    // 'MOVED_FROM_VAL' for both C++03 & C++11 environments.

#define TEST_MV(typeNum, op, expVal, expAlloc) {                              \
    bslma::TestAllocator fromA;                                               \
    my_Class ## typeNum fromObj(expVal);                                      \
    TEST_OP(typeNum, op, expVal, expAlloc);                                   \
    ASSERT_IS_MOVED_FROM(fromObj);                                            \
  }
    // This macro evaluates the specified 'op' expression in the namespace
    // under test, involving the address 'objPtr' of an object of type
    // 'my_ClassN' (where 'N' stands for the specified 'typeNum') and the
    // modifiable object 'fromObj' of type 'my_ClassN' initialized to
    // 'expVal', and verifies that the 'd_value' and 'd_allocator_p' members
    // store the specified 'expVal' and 'expAlloc' values and 'fromObj' is in
    // a moved-from state after 'op' has been evaluated.

#define TEST_PAIRMV(typeNum0, typeNum1, op, expVal0, expA0, expVal1, expA1) { \
    typedef my_PairA_ ## typeNum0 ## _ ## typeNum1 Type;                      \
    Type fromObj(expVal0, expVal1);                                           \
    TEST_PAIR(op, expVal0, expA0, expVal1, expA1);                            \
    ASSERT_IS_MOVED_FROM(fromObj.first);                                      \
  }
    // This macro evaluates the specified 'op' expression in the namespace
    // under test, involving the address 'objPtr' of an object of type
    // 'my_Pair_N0_N1' (where 'N0' and 'N1' stand for the specified 'typeNum0
    // and 'typeNum1') and a modifiable object 'fromObj' of type
    // 'my_Pair_N0_N1' initialized to '{ expVal0, expVal1 }, and verifies that
    // 'fromObj.first' and 'fromObj.second' are in a moved-from state after
    // 'op' has been evaluated.  See the 'TEST_PAIR' macro above for details.

                         // ==========================
                         // debug breakpoints pre/post
                         // ==========================

void pre(const my_ClassDef *p)
    // Do nothing.  This function can be taken advantage of to debug the above
    // macros by setting a breakpoint to examine state prior to executing the
    // main operation under test.
{
    (void) p;  // remove unused variable warning
}

void post(const my_ClassDef *p)
    // Do nothing.  This function can be taken advantage of to debug the above
    // macros by setting a breakpoint to examine state after executing the
    // main operation under test.
{
    (void) p;  // remove unused variable warning
}

                       // =============================
                       // class ConstructTestArgNoAlloc
                       // =============================

template <int ID>
class ConstructTestArgNoAlloc : public my_ClassDef {
    // This very simple 'struct' is used purely to disambiguate types in
    // passing parameters to 'construct' due to the fact that
    // 'ConstructTestArgNoAlloc<ID1>' is a different type than
    // 'ConstructTestArgNoAlloc<ID2>' if ID1 != ID2.  This class does not take
    // an optional allocator.

  public:
    // CREATORS
    ConstructTestArgNoAlloc(int value = -1);
        // Create an object having the specified 'value'.
};

// CREATORS
template <int ID>
ConstructTestArgNoAlloc<ID>::ConstructTestArgNoAlloc(int value)
{
    d_value = value;
    d_allocator_p = 0;
}

                       // ==============================
                       // class ConstructTestTypeNoAlloc
                       // ==============================

class ConstructTestTypeNoAlloc {
    // This 'struct' provides a test class capable of holding up to 14
    // parameters of types 'ConstructTestArgNoAlloc[1--14]'.  By default, a
    // 'ConstructTestTypeNoAlloc' is constructed with nil ('N1') values, but
    // instances can be constructed with actual values (e.g., for creating
    // expected values).  A 'ConstructTestTypeNoAlloc' can be invoked with up
    // to 14 parameters, via member functions 'testFunc[1--14]'.  These
    // functions are also called by the overloaded member 'operator()' of the
    // same signatures, and similar global functions 'testFunc[1--14]'.  All
    // invocations support the above 'ConstructTestSlotsNoAlloc' mechanism.
    //
    // This 'struct' intentionally does *not* take an allocator.

    // PRIVATE TYPES
    typedef ConstructTestArgNoAlloc<1>  Arg1;
    typedef ConstructTestArgNoAlloc<2>  Arg2;
    typedef ConstructTestArgNoAlloc<3>  Arg3;
    typedef ConstructTestArgNoAlloc<4>  Arg4;
    typedef ConstructTestArgNoAlloc<5>  Arg5;
    typedef ConstructTestArgNoAlloc<6>  Arg6;
    typedef ConstructTestArgNoAlloc<7>  Arg7;
    typedef ConstructTestArgNoAlloc<8>  Arg8;
    typedef ConstructTestArgNoAlloc<9>  Arg9;
    typedef ConstructTestArgNoAlloc<10> Arg10;
    typedef ConstructTestArgNoAlloc<11> Arg11;
    typedef ConstructTestArgNoAlloc<12> Arg12;
    typedef ConstructTestArgNoAlloc<13> Arg13;
    typedef ConstructTestArgNoAlloc<14> Arg14;
        // Argument types for shortcut.

    enum {
        N1 = -1   // default value for all private data
    };

  public:
    // DATA (exceptionally public, only within a test driver)
    Arg1  d_a1;
    Arg2  d_a2;
    Arg3  d_a3;
    Arg4  d_a4;
    Arg5  d_a5;
    Arg6  d_a6;
    Arg7  d_a7;
    Arg8  d_a8;
    Arg9  d_a9;
    Arg10 d_a10;
    Arg11 d_a11;
    Arg12 d_a12;
    Arg13 d_a13;
    Arg14 d_a14;

    // CREATORS (exceptionally in-line, only within a test driver)
    ConstructTestTypeNoAlloc(
                Arg1  a1  = N1, Arg2  a2  = N1, Arg3  a3  = N1,
                Arg4  a4  = N1, Arg5  a5  = N1, Arg6  a6  = N1, Arg7  a7  = N1,
                Arg8  a8  = N1, Arg9  a9  = N1, Arg10 a10 = N1, Arg11 a11 = N1,
                Arg12 a12 = N1, Arg13 a13 = N1, Arg14 a14 = N1)
    : d_a1(a1), d_a2(a2), d_a3(a3), d_a4(a4), d_a5(a5), d_a6(a6), d_a7(a7)
    , d_a8(a8), d_a9(a9), d_a10(a10), d_a11(a11), d_a12(a12), d_a13(a13)
    , d_a14(a14)
    {
    }
};

// FREE OPERATORS
bool operator==(const ConstructTestTypeNoAlloc& lhs,
                const ConstructTestTypeNoAlloc& rhs)
{
    return lhs.d_a1.d_value  == rhs.d_a1.d_value &&
           lhs.d_a2.d_value  == rhs.d_a2.d_value &&
           lhs.d_a3.d_value  == rhs.d_a3.d_value &&
           lhs.d_a4.d_value  == rhs.d_a4.d_value &&
           lhs.d_a5.d_value  == rhs.d_a5.d_value &&
           lhs.d_a6.d_value  == rhs.d_a6.d_value &&
           lhs.d_a7.d_value  == rhs.d_a7.d_value &&
           lhs.d_a8.d_value  == rhs.d_a8.d_value &&
           lhs.d_a9.d_value  == rhs.d_a9.d_value &&
           lhs.d_a10.d_value == rhs.d_a10.d_value &&
           lhs.d_a11.d_value == rhs.d_a11.d_value &&
           lhs.d_a12.d_value == rhs.d_a12.d_value &&
           lhs.d_a13.d_value == rhs.d_a13.d_value &&
           lhs.d_a14.d_value == rhs.d_a14.d_value;
}

                       // ===========================
                       // class ConstructTestArgAlloc
                       // ===========================

template <int ID>
class ConstructTestArgAlloc : public my_ClassDef {
    // This class is used to disambiguate types in passing parameters due to
    // the fact that 'ConstructTestArgNoAlloc<ID1>' is a different type than
    // 'ConstructTestArgNoAlloc<ID2>' is ID1 != ID2.  This class is used for
    // testing proper forwarding of memory allocator.

  public:
    // CREATORS
    explicit
    ConstructTestArgAlloc(int value = -1, bslma::Allocator *allocator = 0);
        // Create an object having the specified 'value'.  Use the specified
        // 'allocator' to supply memory.  If 'allocator' is 0, use the
        // currently installed default allocator.

    ConstructTestArgAlloc(const ConstructTestArgAlloc&  original,
                          bslma::Allocator             *allocator = 0);
        // Create an object having the same value as the specified 'original'.
        // Use the specified 'allocator' to supply memory.  If 'allocator' is
        // 0, use the currently installed default allocator.
};

// TRAITS
namespace BloombergLP {
namespace bslma {

template <int ID>
struct UsesBslmaAllocator<ConstructTestArgAlloc<ID> > : bsl::true_type {};

}  // close namespace bslma
}  // close enterprise namespace

// CREATORS
template <int ID>
ConstructTestArgAlloc<ID>::ConstructTestArgAlloc(int               value,
                                                 bslma::Allocator *allocator)
{
    d_value = value;
    d_allocator_p = allocator;
}

template <int ID>
ConstructTestArgAlloc<ID>::ConstructTestArgAlloc(
                                       const ConstructTestArgAlloc&  original,
                                       bslma::Allocator             *allocator)
{
    d_value = original.d_value;
    d_allocator_p = allocator;
}

                       // ============================
                       // class ConstructTestTypeAlloc
                       // ============================

class ConstructTestTypeAlloc {
    // This class provides a test class capable of holding up to 14
    // parameters of types 'ConstructTestArgAlloc[1--14]'.  By
    // default, a 'ConstructTestTypeAlloc' is constructed with nil ('N1')
    // values, but instances can be constructed with actual values (e.g., for
    // creating expected values).
    // This class intentionally *does* take an allocator.

    // PRIVATE TYPES
    typedef ConstructTestArgAlloc<1>  Arg1;
    typedef ConstructTestArgAlloc<2>  Arg2;
    typedef ConstructTestArgAlloc<3>  Arg3;
    typedef ConstructTestArgAlloc<4>  Arg4;
    typedef ConstructTestArgAlloc<5>  Arg5;
    typedef ConstructTestArgAlloc<6>  Arg6;
    typedef ConstructTestArgAlloc<7>  Arg7;
    typedef ConstructTestArgAlloc<8>  Arg8;
    typedef ConstructTestArgAlloc<9>  Arg9;
    typedef ConstructTestArgAlloc<10> Arg10;
    typedef ConstructTestArgAlloc<11> Arg11;
    typedef ConstructTestArgAlloc<12> Arg12;
    typedef ConstructTestArgAlloc<13> Arg13;
    typedef ConstructTestArgAlloc<14> Arg14;
        // Argument types for shortcut.

    enum {
        N1 = -1   // default value for all private data
    };

  public:
    // DATA (exceptionally public, only within a test driver)
    Arg1  d_a1;
    Arg2  d_a2;
    Arg3  d_a3;
    Arg4  d_a4;
    Arg5  d_a5;
    Arg6  d_a6;
    Arg7  d_a7;
    Arg8  d_a8;
    Arg9  d_a9;
    Arg10 d_a10;
    Arg11 d_a11;
    Arg12 d_a12;
    Arg13 d_a13;
    Arg14 d_a14;

    // CREATORS (exceptionally in-line, only within a test driver)
    ConstructTestTypeAlloc(Arg1  a1, bslma::Allocator *allocator = 0)
    : d_a1(a1, allocator)
    {
    }

    ConstructTestTypeAlloc(Arg1  a1, Arg2  a2, bslma::Allocator *allocator = 0)
    : d_a1(a1, allocator), d_a2(a2, allocator)
    {
    }

    ConstructTestTypeAlloc(Arg1  a1, Arg2  a2, Arg3  a3,
                           bslma::Allocator *allocator = 0)
    : d_a1(a1, allocator), d_a2(a2, allocator), d_a3(a3, allocator)
    {
    }

    ConstructTestTypeAlloc(Arg1  a1, Arg2  a2, Arg3  a3, Arg4  a4,
                           bslma::Allocator *allocator = 0)
    : d_a1(a1, allocator), d_a2(a2, allocator), d_a3(a3, allocator)
    , d_a4(a4, allocator)
    {
    }

    ConstructTestTypeAlloc(Arg1  a1, Arg2  a2, Arg3  a3, Arg4  a4, Arg5  a5,
                           bslma::Allocator *allocator = 0)
    : d_a1(a1, allocator), d_a2(a2, allocator), d_a3(a3, allocator)
    , d_a4(a4, allocator), d_a5(a5, allocator)
    {
    }

    ConstructTestTypeAlloc(Arg1  a1, Arg2  a2, Arg3  a3, Arg4  a4, Arg5  a5,
                           Arg6  a6, bslma::Allocator *allocator = 0)
    : d_a1(a1, allocator), d_a2(a2, allocator), d_a3(a3, allocator)
    , d_a4(a4, allocator), d_a5(a5, allocator), d_a6(a6, allocator)
    {
    }

    ConstructTestTypeAlloc(Arg1  a1, Arg2  a2, Arg3  a3, Arg4  a4, Arg5  a5,
                           Arg6  a6, Arg7  a7, bslma::Allocator *allocator = 0)
    : d_a1(a1, allocator), d_a2(a2, allocator), d_a3(a3, allocator)
    , d_a4(a4, allocator), d_a5(a5, allocator), d_a6(a6, allocator)
    , d_a7(a7, allocator)
    {
    }

    ConstructTestTypeAlloc(Arg1  a1, Arg2  a2, Arg3  a3, Arg4  a4, Arg5  a5,
                           Arg6  a6, Arg7  a7, Arg8  a8,
                           bslma::Allocator *allocator = 0)
    : d_a1(a1, allocator), d_a2(a2, allocator), d_a3(a3, allocator)
    , d_a4(a4, allocator), d_a5(a5, allocator), d_a6(a6, allocator)
    , d_a7(a7, allocator), d_a8(a8, allocator)
    {
    }

    ConstructTestTypeAlloc(Arg1  a1, Arg2  a2, Arg3  a3, Arg4  a4, Arg5  a5,
                           Arg6  a6, Arg7  a7, Arg8  a8, Arg9  a9,
                           bslma::Allocator *allocator = 0)
    : d_a1(a1, allocator), d_a2(a2, allocator), d_a3(a3, allocator)
    , d_a4(a4, allocator), d_a5(a5, allocator), d_a6(a6, allocator)
    , d_a7(a7, allocator), d_a8(a8, allocator), d_a9(a9, allocator)
    {
    }

    ConstructTestTypeAlloc(Arg1  a1, Arg2  a2, Arg3  a3, Arg4  a4, Arg5  a5,
                           Arg6  a6, Arg7  a7, Arg8  a8, Arg9  a9, Arg10 a10,
                           bslma::Allocator *allocator = 0)
    : d_a1 (a1,  allocator), d_a2 (a2,  allocator), d_a3 (a3,  allocator)
    , d_a4 (a4,  allocator), d_a5 (a5,  allocator), d_a6 (a6,  allocator)
    , d_a7 (a7,  allocator), d_a8 (a8,  allocator), d_a9 (a9,  allocator)
    , d_a10(a10, allocator)
    {
    }

    ConstructTestTypeAlloc(Arg1  a1, Arg2  a2, Arg3  a3, Arg4  a4, Arg5  a5,
                           Arg6  a6, Arg7  a7, Arg8  a8, Arg9  a9, Arg10 a10,
                           Arg11 a11, bslma::Allocator *allocator = 0)
    : d_a1 (a1,  allocator), d_a2 (a2,  allocator), d_a3 (a3,  allocator)
    , d_a4 (a4,  allocator), d_a5 (a5,  allocator), d_a6 (a6,  allocator)
    , d_a7 (a7,  allocator), d_a8 (a8,  allocator), d_a9 (a9,  allocator)
    , d_a10(a10, allocator), d_a11(a11, allocator)
    {
    }

    ConstructTestTypeAlloc(Arg1  a1, Arg2  a2, Arg3  a3, Arg4  a4, Arg5  a5,
                           Arg6  a6, Arg7  a7, Arg8  a8, Arg9  a9, Arg10 a10,
                           Arg11 a11, Arg12 a12,
                           bslma::Allocator *allocator = 0)
    : d_a1 (a1,  allocator), d_a2 (a2,  allocator), d_a3 (a3,  allocator)
    , d_a4 (a4,  allocator), d_a5 (a5,  allocator), d_a6 (a6,  allocator)
    , d_a7 (a7,  allocator), d_a8 (a8,  allocator), d_a9 (a9,  allocator)
    , d_a10(a10, allocator), d_a11(a11, allocator), d_a12(a12, allocator)
    {
    }

    ConstructTestTypeAlloc(Arg1  a1,  Arg2  a2,  Arg3  a3, Arg4  a4, Arg5  a5,
                           Arg6  a6,  Arg7  a7,  Arg8  a8, Arg9  a9, Arg10 a10,
                           Arg11 a11, Arg12 a12, Arg13 a13,
                           bslma::Allocator *allocator = 0)
    : d_a1 (a1,  allocator), d_a2 (a2,  allocator), d_a3 (a3,  allocator)
    , d_a4 (a4,  allocator), d_a5 (a5,  allocator), d_a6 (a6,  allocator)
    , d_a7 (a7,  allocator), d_a8 (a8,  allocator), d_a9 (a9,  allocator)
    , d_a10(a10, allocator), d_a11(a11, allocator), d_a12(a12, allocator)
    , d_a13(a13, allocator)
    {
    }

    ConstructTestTypeAlloc(Arg1  a1,  Arg2  a2,  Arg3  a3, Arg4  a4, Arg5  a5,
                           Arg6  a6,  Arg7  a7,  Arg8  a8, Arg9  a9, Arg10 a10,
                           Arg11 a11, Arg12 a12, Arg13 a13, Arg14 a14,
                           bslma::Allocator *allocator = 0)
    : d_a1 (a1,  allocator), d_a2 (a2,  allocator), d_a3 (a3,  allocator)
    , d_a4 (a4,  allocator), d_a5 (a5,  allocator), d_a6 (a6,  allocator)
    , d_a7 (a7,  allocator), d_a8 (a8,  allocator), d_a9 (a9,  allocator)
    , d_a10(a10, allocator), d_a11(a11, allocator), d_a12(a12, allocator)
    , d_a13(a13, allocator), d_a14(a14, allocator)
    {
    }
};

// TRAITS
namespace BloombergLP {
namespace bslma {

template <>
struct UsesBslmaAllocator<ConstructTestTypeAlloc> : bsl::true_type {};

}  // close namespace bslma
}  // close enterprise namespace

// FREE OPERATORS
bool operator==(const ConstructTestTypeAlloc& lhs,
                const ConstructTestTypeAlloc& rhs)
{
    return lhs.d_a1.d_value  == rhs.d_a1.d_value &&
           lhs.d_a2.d_value  == rhs.d_a2.d_value &&
           lhs.d_a3.d_value  == rhs.d_a3.d_value &&
           lhs.d_a4.d_value  == rhs.d_a4.d_value &&
           lhs.d_a5.d_value  == rhs.d_a5.d_value &&
           lhs.d_a6.d_value  == rhs.d_a6.d_value &&
           lhs.d_a7.d_value  == rhs.d_a7.d_value &&
           lhs.d_a8.d_value  == rhs.d_a8.d_value &&
           lhs.d_a9.d_value  == rhs.d_a9.d_value &&
           lhs.d_a10.d_value == rhs.d_a10.d_value &&
           lhs.d_a11.d_value == rhs.d_a11.d_value &&
           lhs.d_a12.d_value == rhs.d_a12.d_value &&
           lhs.d_a13.d_value == rhs.d_a13.d_value &&
           lhs.d_a14.d_value == rhs.d_a14.d_value;
}

                           // ======================
                           // macros TEST_CONSTRUCT*
                           // ======================

#define TEST_CONSTRUCT(op, expArgs)                                           \
  {                                                                           \
    ConstructTestTypeNoAlloc EXP expArgs;                                     \
    bsls::ObjectBuffer<ConstructTestTypeNoAlloc> rawBuf;                      \
    ConstructTestTypeNoAlloc *objPtr = rawBuf.address();                      \
    ConstructTestTypeNoAlloc& mX = *objPtr;                                   \
    const ConstructTestTypeNoAlloc& X = mX;                                   \
    memset(&mX, 92, sizeof mX);                                               \
    Util:: op ;                                                               \
    ASSERT(EXP == X);                                                         \
  }

#define TEST_CONSTRUCTA(op, expArgs,                                          \
                 a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14) \
  {                                                                           \
    ConstructTestTypeAlloc EXP expArgs;                                       \
    bsls::ObjectBuffer<ConstructTestTypeAlloc> rawBuf;                        \
    ConstructTestTypeAlloc *objPtr = rawBuf.address();                        \
    ConstructTestTypeAlloc& mX = *objPtr;                                     \
    const ConstructTestTypeAlloc& X = *objPtr;                                \
    memset(&mX, 92, sizeof mX);                                               \
    Util:: op ;                                                               \
    ASSERT(EXP == X);                                                         \
    ASSERT(a1  == X.d_a1.d_allocator_p);                                      \
    ASSERT(a2  == X.d_a2.d_allocator_p);                                      \
    ASSERT(a3  == X.d_a3.d_allocator_p);                                      \
    ASSERT(a4  == X.d_a4.d_allocator_p);                                      \
    ASSERT(a5  == X.d_a5.d_allocator_p);                                      \
    ASSERT(a6  == X.d_a6.d_allocator_p);                                      \
    ASSERT(a7  == X.d_a7.d_allocator_p);                                      \
    ASSERT(a8  == X.d_a8.d_allocator_p);                                      \
    ASSERT(a9  == X.d_a9.d_allocator_p);                                      \
    ASSERT(a10 == X.d_a10.d_allocator_p);                                     \
    ASSERT(a11 == X.d_a11.d_allocator_p);                                     \
    ASSERT(a12 == X.d_a12.d_allocator_p);                                     \
    ASSERT(a13 == X.d_a13.d_allocator_p);                                     \
    ASSERT(a14 == X.d_a14.d_allocator_p);                                     \
  }

//=============================================================================
//               GLOBAL TYPEDEFS AND CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------
typedef my_Pair<my_Class1, my_Class1> my_Pair_1_1;
typedef my_Pair<my_Class1, my_Class2> my_Pair_1_2;
typedef my_Pair<my_Class2, my_Class1> my_Pair_2_1;
typedef my_Pair<my_Class2, my_Class2> my_Pair_2_2;
typedef my_Pair<my_Class4, my_Class4> my_Pair_4_4;

typedef my_PairA<my_Class1, my_Class2> my_PairA_1_2;
typedef my_PairA<my_Class2, my_Class2> my_PairA_2_2;

typedef my_PairAA<my_Class2, my_Class2> my_PairAA_2_2;

typedef my_PairAA<my_Class4, my_Class4> my_PairAA_4_4;

typedef my_PairBB<my_Class4, my_Class4> my_PairBB_4_4;

const my_Class1      V1(1);
const my_Class2      V2(2);
const my_Class2a     V2A(0x2a);
const my_Class3      V3(3);
const my_Class4      V4(4);
const my_ClassFussy  VF(5);
const my_ClassFussy2 VF2(6);

const my_Pair_1_1 PV1V1(V1, V1);
const my_Pair_1_2 PV1V2(V1, V2);
const my_Pair_2_1 PV2V1(V2, V1);
const my_Pair_2_2 PV2V2(V2, V2);
const my_Pair_4_4 PV4V4(V4, V4);

const my_PairA_1_2 PAV1V2(V1, V2);
const my_PairA_2_2 PAV2V2(V2, V2);

const my_PairAA_2_2 PAAV2V2(V2, V2);

const my_PairAA_4_4 PAAV4V4(V4, V4);

const my_PairBB_4_4 PBBV4V4(V4, V4);

ConstructTestArgNoAlloc<1>  VNA1(1);
ConstructTestArgNoAlloc<2>  VNA2(2);
ConstructTestArgNoAlloc<3>  VNA3(3);
ConstructTestArgNoAlloc<4>  VNA4(4);
ConstructTestArgNoAlloc<5>  VNA5(5);
ConstructTestArgNoAlloc<6>  VNA6(6);
ConstructTestArgNoAlloc<7>  VNA7(7);
ConstructTestArgNoAlloc<8>  VNA8(8);
ConstructTestArgNoAlloc<9>  VNA9(9);
ConstructTestArgNoAlloc<10> VNA10(10);
ConstructTestArgNoAlloc<11> VNA11(11);
ConstructTestArgNoAlloc<12> VNA12(12);
ConstructTestArgNoAlloc<13> VNA13(13);
ConstructTestArgNoAlloc<14> VNA14(14);

ConstructTestArgAlloc<1>    VA1(1);  // leave default allocator, on purpose
ConstructTestArgAlloc<2>    VA2(2);
ConstructTestArgAlloc<3>    VA3(3);
ConstructTestArgAlloc<4>    VA4(4);
ConstructTestArgAlloc<5>    VA5(5);
ConstructTestArgAlloc<6>    VA6(6);
ConstructTestArgAlloc<7>    VA7(7);
ConstructTestArgAlloc<8>    VA8(8);
ConstructTestArgAlloc<9>    VA9(9);
ConstructTestArgAlloc<10>   VA10(10);
ConstructTestArgAlloc<11>   VA11(11);
ConstructTestArgAlloc<12>   VA12(12);
ConstructTestArgAlloc<13>   VA13(13);
ConstructTestArgAlloc<14>   VA14(14);

//=============================================================================
//                              USAGE EXAMPLE
//-----------------------------------------------------------------------------

///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Using 'bslma::ConstructionUtil' to Implement a Container
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// This example demonstrates the intended use of 'bslma::ConstructionUtil' to
// implement a simple container class that uses the 'bslma::Allocator' protocol
// for memory management.
//
// First, because allocation and construction are done in two separate steps,
// we need to define a proctor type that will deallocate the allocated memory
// in case the constructor throws an exception:
//..
    template <class TYPE>
    class MyContainerProctor {
        // This class implements a proctor to release memory allocated during
        // the construction of a 'MyContainer' object if the constructor for
        // the container's data element throws an exception.  Such a proctor
        // should be 'release'd once the element is safely constructed.

        // DATA
        bslma::Allocator *d_allocator_p;
        TYPE             *d_address_p;    // proctored memory

      private:
        // NOT IMPLEMENTED
        MyContainerProctor(const MyContainerProctor&);             // = delete
        MyContainerProctor& operator=(const MyContainerProctor&);  // = delete

      public:
        // CREATORS
        MyContainerProctor(bslma::Allocator *allocator, TYPE *address)
            // Create a proctor that conditionally manages the memory at the
            // specified 'address', and that uses the specified 'allocator' to
            // deallocate the block of memory (if not released -- see
            // 'release') upon destruction.  The behavior is undefined unless
            // 'allocator' is non-zero and supplied the memory at 'address'.
        : d_allocator_p(allocator)
        , d_address_p(address)
        {
        }

        ~MyContainerProctor()
            // Destroy this proctor, and deallocate the block of memory it
            // manages (if any) by invoking the 'deallocate' method of the
            // allocator that was supplied at construction of this proctor.  If
            // no memory is currently being managed, this method has no effect.
        {
            if (d_address_p) {
                d_allocator_p->deallocate(d_address_p);
            }
        }

        // MANIPULATORS
            // Release from management the block of memory currently managed by
            // this proctor.  If no memory is currently being managed, this
            // method has no effect.
        void release()
        {
            d_address_p = 0;
        }
    };
//..
// Then, we create a container class that holds a single element and uses
// 'bslma' allocators:
//..
//  #include <bslma_constructionutil.h>
//
    template <class TYPE>
    class MyContainer {
        // This class provides a container that always holds exactly one
        // element, dynamically allocated using the specified 'bslma'
        // allocator.

        // DATA
        TYPE             *d_value_p;
        bslma::Allocator *d_allocator_p;

      public:
        // TRAITS
        BSLMF_NESTED_TRAIT_DECLARATION(MyContainer, bslma::UsesBslmaAllocator);

        // CREATORS
        MyContainer(bslma::Allocator *basicAllocator = 0);
            // Create a container with a default-constructed element.
            // Optionally specify a 'basicAllocator' used to supply memory.  If
            // 'basicAllocator' is 0, the currently installed default allocator
            // is used.

        template <class OTHER>
        explicit MyContainer(
            BSLS_COMPILERFEATURES_FORWARD_REF(OTHER) value,
            typename bsl::enable_if<bsl::is_convertible<OTHER, TYPE>::value,
                                    void *>::type * = 0)
            // Create a container with an element constructed by
            // (perfectly) forwarding the specified 'value' and that uses the
            // currently installed default allocator to supply memory.  Note
            // that this constructor participates in overload resolution only
            // if 'OTHER' is implicitly convertible to 'TYPE'.
        : d_allocator_p(bslma::Default::defaultAllocator())
        {
            d_value_p =
                static_cast<TYPE *>(d_allocator_p->allocate(sizeof(TYPE)));

            MyContainerProctor<TYPE> proctor(d_allocator_p, d_value_p);

            // Call 'construct' by forwarding 'value'.

            bslma::ConstructionUtil::construct(
                d_value_p,
                d_allocator_p,
                BSLS_COMPILERFEATURES_FORWARD(OTHER, value));
            proctor.release();
        }

        template <class OTHER>
        explicit MyContainer(
            BSLS_COMPILERFEATURES_FORWARD_REF(OTHER)  value,
            bslma::Allocator                         *basicAllocator);
            // Create a container with an element constructed by
            // (perfectly) forwarding the specified 'value' and that uses the
            // specified 'basicAllocator' to supply memory.  If
            // 'basicAllocator' is 0, the currently installed default allocator
            // is used.  Note that this constructor participates in overload
            // resolution only if 'OTHER' is implicitly convertible to 'TYPE'.

        MyContainer(const MyContainer&  original,
                    bslma::Allocator   *basicAllocator = 0);
            // Create a container having the same value as the specified
            // 'original' object.  Optionally specify a 'basicAllocator' used
            // to supply memory.  If 'basicAllocator' is 0, the currently
            // installed default allocator is used.

        ~MyContainer();
            // Destroy this object.

        // MANIPULATORS
        TYPE& front()
            // Return a non-'const' reference to the element contained in this
            // object.
        {
            return *d_value_p;
        }

        // ACCESSORS
        const TYPE& front() const
            // Return a 'const' reference to the element contained in this
            // object.
        {
            return *d_value_p;
        }

        bslma::Allocator *allocator() const
            // Return the allocator used by this object to supply memory.
        {
            return d_allocator_p;
        }

        // etc.
    };
//..
// Next, we implement the constructors that allocate memory and construct a
// 'TYPE' object in the allocated memory.  We perform the allocation using the
// 'allocate' method of 'bslma::Allocator' and the construction using the
// 'construct' method of 'ConstructionUtil' that provides the correct semantics
// for passing the allocator to the constructed object when appropriate:
//..
    template <class TYPE>
    MyContainer<TYPE>::MyContainer(bslma::Allocator *basicAllocator)
    : d_allocator_p(bslma::Default::allocator(basicAllocator))
    {
        d_value_p = static_cast<TYPE *>(d_allocator_p->allocate(sizeof(TYPE)));
        MyContainerProctor<TYPE> proctor(d_allocator_p, d_value_p);

        // Call 'construct' with no constructor arguments (aside from the
        // allocator).

        bslma::ConstructionUtil::construct(d_value_p, d_allocator_p);
        proctor.release();
    }

    template <class TYPE>
    template <class OTHER>
    MyContainer<TYPE>::MyContainer(
                      BSLS_COMPILERFEATURES_FORWARD_REF(OTHER)  value,
                      bslma::Allocator                         *basicAllocator)
    : d_allocator_p(bslma::Default::allocator(basicAllocator))
    {
        d_value_p = static_cast<TYPE *>(d_allocator_p->allocate(sizeof(TYPE)));
        MyContainerProctor<TYPE> proctor(d_allocator_p, d_value_p);

        // Call 'construct' by forwarding 'value'.

        bslma::ConstructionUtil::construct(
            d_value_p,
            d_allocator_p,
            BSLS_COMPILERFEATURES_FORWARD(OTHER, value));
        proctor.release();
    }
//..
// Then, we define the copy constructor for 'MyContainer'.  Note that we don't
// propagate the allocator from the 'original' container, but use
// 'basicAllocator' instead:
//..
    template <class TYPE>
    MyContainer<TYPE>::MyContainer(const MyContainer&  original,
                                   bslma::Allocator   *basicAllocator)
    : d_allocator_p(bslma::Default::allocator(basicAllocator))
    {
        d_value_p = static_cast<TYPE *>(d_allocator_p->allocate(sizeof(TYPE)));
        MyContainerProctor<TYPE> proctor(d_allocator_p, d_value_p);

        // Call 'construct' so as to copy-construct the element contained by
        // 'original'.

        bslma::ConstructionUtil::construct(d_value_p,
                                           d_allocator_p,
                                           *original.d_value_p);
        proctor.release();
    }
//..
// Now, the destructor destroys the object and deallocates the memory used to
// hold the element using the allocator:
//..
    template <class TYPE>
    MyContainer<TYPE>::~MyContainer()
    {
        d_value_p->~TYPE();
        d_allocator_p->deallocate(d_value_p);
    }
//..
// Finally, we perform a simple test of 'MyContainer', instantiating it with
// element type 'int':
//..
    int usageExample1()
    {
        bslma::TestAllocator testAlloc;
        MyContainer<int> C1(123, &testAlloc);
        ASSERT(C1.allocator() == &testAlloc);
        ASSERT(C1.front()     == 123);

        MyContainer<int> C2(C1);
        ASSERT(C2.allocator() == bslma::Default::defaultAllocator());
        ASSERT(C2.front()     == 123);

        return 0;
    }
//..
///Example 2: 'bslma' Allocator Propagation
///- - - - - - - - - - - - - - - - - - - -
// This example demonstrates that 'MyContainer' does indeed propagate the
// allocator to its contained element.
//
// First, we create a representative element class, 'MyType', that allocates
// memory using the 'bslma' allocator protocol:
//..
//  #include <bslma_default.h>
//
    class MyType {

        // DATA
        // ...
        bslma::Allocator *d_allocator_p;

      public:
        // TRAITS
        BSLMF_NESTED_TRAIT_DECLARATION(MyType, bslma::UsesBslmaAllocator);

        // CREATORS
        explicit MyType(bslma::Allocator *basicAllocator = 0)
            // Create a 'MyType' object having the default value.  Optionally
            // specify a 'basicAllocator' used to supply memory.  If
            // 'basicAllocator' is 0, the currently installed default allocator
            // is used.
        : d_allocator_p(bslma::Default::allocator(basicAllocator))
        {
            // ...
        }

        MyType(const MyType&/*original*/, bslma::Allocator *basicAllocator = 0)
            // Create a 'MyType' object having the same value as the specified
            // 'original' object.  Optionally specify a 'basicAllocator' used
            // to supply memory.  If 'basicAllocator' is 0, the currently
            // installed default allocator is used.
        : d_allocator_p(bslma::Default::allocator(basicAllocator))
        {
            // ...
        }

        // ...

        // ACCESSORS
        bslma::Allocator *allocator() const
            // Return the allocator used by this object to supply memory.
        {
            return d_allocator_p;
        }

        // ...
    };
//..
// Finally, we instantiate 'MyContainer' using 'MyType' and verify that, when
// we provide the address of an allocator to the constructor of the container,
// the same address is passed to the constructor of the contained element.  We
// also verify that, when the container is copy-constructed without supplying
// an allocator, the copy uses the default allocator, not the allocator from
// the original object.  Moreover, we verify that the element stored in the
// copy also uses the default allocator:
//..
//  #include <bslmf_issame.h>
//
    int usageExample2()
    {
        bslma::TestAllocator testAlloc;
        MyContainer<MyType> C1(&testAlloc);
        ASSERT(C1.allocator()         == &testAlloc);
        ASSERT(C1.front().allocator() == &testAlloc);

        MyContainer<MyType> C2(C1);
        ASSERT(C2.allocator()         != C1.allocator());
        ASSERT(C2.allocator()         == bslma::Default::defaultAllocator());
        ASSERT(C2.front().allocator() != &testAlloc);
        ASSERT(C2.front().allocator() == bslma::Default::defaultAllocator());

        return 0;
    }
//..

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int                 test = argc > 1 ? atoi(argv[1]) : 0;
    bool             verbose = argc > 2;
    bool         veryVerbose = argc > 3;
    bool     veryVeryVerbose = argc > 4;
    bool veryVeryVeryVerbose = argc > 5;

    forceDestructorCall = veryVeryVerbose;

    setbuf(stdout, NULL);    // Use unbuffered output

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:  // Zero is always the leading case.
      case 8: {
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

        usageExample1();
        usageExample2();

      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TESTING 'destructiveMove'
        //
        // Concerns:
        //: 1 That the move constructor properly forwards the allocator when
        //:   appropriate.
        //:
        //: 2 That the move constructor uses 'memcpy' when appropriate.
        //
        // Plan:
        //   The test plan is identical to 'copyConstruct', except that we
        //   operate the move from a temporary copy created with the (already
        //   tested) 'copyConstruct' so as not to affect the constants of this
        //   test driver.  We are also careful (for the exception testing) that
        //   this temporary is not destroyed if it has been moved successfully.
        //   Finally, we verify that the original object has been destroyed,
        //   unless it was bitwise-movable.
        //
        // Testing:
        //   destructiveMove(T *dst, T *src, *a);
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING 'destructiveMove'"
                            "\n=========================\n");

        bslma::TestAllocator testAllocator(veryVeryVeryVerbose);
        bslma::TestAllocator *const TA = &testAllocator;
        int                  dummyAllocator;  // dummy, non-'bslma' allocator
        int                  *const XA = &dummyAllocator;

        if (verbose) printf("Value and allocator testing.\n");

        // my_Class #  Operation                            Val Alloc
        // ==========  ==================================== === =====
        {
            my_ClassDef rawBuf;
            my_Class1 *srcPtr = (my_Class1 *)&rawBuf;
            Util::construct(srcPtr, TA, V1);
            TEST_OP(1, destructiveMove(objPtr, TA, srcPtr),  1, 0);
            ASSERT(91 == rawBuf.d_value);
            ASSERT(0  == rawBuf.d_allocator_p);
        }
        {
            my_ClassDef rawBuf;
            my_Class2 *srcPtr = (my_Class2 *)&rawBuf;
            Util::construct(srcPtr, TA, V2);
            TEST_OP(2, destructiveMove(objPtr, TA, srcPtr),  2, TA);
            ASSERT(92 == rawBuf.d_value);
            ASSERT(0  == rawBuf.d_allocator_p);
        }
        {
            my_ClassDef rawBuf;
            my_Class2a *srcPtr = (my_Class2a *)&rawBuf;
            Util::construct(srcPtr, TA, V2A);
            TEST_OP(2a, destructiveMove(objPtr, TA, srcPtr), 0x2a, TA);
            ASSERT(92 == rawBuf.d_value);
            ASSERT(0  == rawBuf.d_allocator_p);
        }
        {
            my_ClassDef rawBuf;
            my_Class3 *srcPtr = (my_Class3 *)&rawBuf;
            Util::construct(srcPtr, TA, V3);
            TEST_OP(3, destructiveMove(objPtr, TA, srcPtr),  3, TA);
            ASSERT(93 == rawBuf.d_value);
            ASSERT(0  == rawBuf.d_allocator_p);
        }
        {
            my_ClassDef rawBuf;
            my_Class1 *srcPtr = (my_Class1 *)&rawBuf;
            Util::construct(srcPtr, TA, V1);
            TEST_OP(1, destructiveMove(objPtr, XA, srcPtr),  1, 0);
            ASSERT(91 == rawBuf.d_value);
            ASSERT(0  == rawBuf.d_allocator_p);
        }
        {
            my_ClassDef rawBuf;
            my_Class2 *srcPtr = (my_Class2 *)&rawBuf;
            Util::construct(srcPtr, TA, V2);
            // Must use 'TA' so that behavior is the same in C++98 mode (copy,
            // uses default allocator) and C++11 mode (move, copies '*srcPtr'
            // allocator).
            TEST_OP(2, destructiveMove(objPtr, TA, srcPtr),  2, TA);
            ASSERT(92 == rawBuf.d_value);
            ASSERT(0  == rawBuf.d_allocator_p);
        }
        {
            my_ClassDef rawBuf;
            my_Class2a *srcPtr = (my_Class2a *)&rawBuf;
            Util::construct(srcPtr, TA, V2A);
            // Must use 'TA' so that behavior is the same in C++98 mode (copy,
            // uses default allocator) and C++11 mode (move, copies '*srcPtr'
            // allocator).
            TEST_OP(2a, destructiveMove(objPtr, TA, srcPtr), 0x2a, TA);
            ASSERT(92 == rawBuf.d_value);
            ASSERT(0  == rawBuf.d_allocator_p);
        }
        {
            my_ClassDef rawBuf;
            my_Class3 *srcPtr = (my_Class3 *)&rawBuf;
            Util::construct(srcPtr, TA, V3);
            // Must use 'TA' so that behavior is the same in C++98 mode (copy,
            // uses default allocator) and C++11 mode (move, copies '*srcPtr'
            // allocator).
            TEST_OP(3, destructiveMove(objPtr, TA, srcPtr),  3, TA);
            ASSERT(93 == rawBuf.d_value);
            ASSERT(0  == rawBuf.d_allocator_p);
        }

        if (verbose) printf("Exception testing.\n");

        if (verbose) printf("\t...pair with allocators\n");

        BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
            my_ClassDef rawBuf[4];
            my_PairAA_4_4 *srcPtr = (my_PairAA_4_4 *)&rawBuf[0];
            Util::construct(srcPtr, TA, PAAV4V4);
            bslma::AutoDestructor<my_PairAA_4_4> guard(srcPtr, 1);
            my_PairAA_4_4 *objPtr = (my_PairAA_4_4 *)&rawBuf[2];
            Util::destructiveMove(objPtr, TA, srcPtr);
            guard.release();
            ASSERT(94 == rawBuf[0].d_value);
            ASSERT(0  == rawBuf[0].d_allocator_p);
            ASSERT(94 == rawBuf[1].d_value);
            ASSERT(0  == rawBuf[1].d_allocator_p);
            ASSERT(4  == rawBuf[2].d_value);
            ASSERT(TA == rawBuf[2].d_allocator_p);
            ASSERT(4  == rawBuf[3].d_value);
            ASSERT(TA == rawBuf[3].d_allocator_p);
            objPtr->~my_PairAA_4_4();
        } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

        if (verbose) printf("Trait selection testing.\n");
        {
            my_ClassDef rawBuf[2];
            const int DI = my_ClassFussy::s_destructorInvocations;
            my_ClassFussy *srcPtr = (my_ClassFussy *) &rawBuf[0];
            Util::construct(srcPtr, XA, VF);
            my_ClassFussy *objPtr = (my_ClassFussy *) &rawBuf[1];
            Util::destructiveMove(objPtr, XA, srcPtr);
            ASSERT(DI == my_ClassFussy::s_destructorInvocations);
            ASSERT(5  == rawBuf[0].d_value);
            ASSERT(0  == rawBuf[0].d_allocator_p);
            ASSERT(5  == rawBuf[1].d_value);
            ASSERT(0  == rawBuf[1].d_allocator_p);
            if (veryVerbose) {
                P_(rawBuf[0].d_value); PP(rawBuf[0].d_allocator_p);
                P_(rawBuf[1].d_value); PP(rawBuf[1].d_allocator_p);
            }
            objPtr->~my_ClassFussy();
        }
        {
            my_ClassDef rawBuf[2];
            const int DI  = my_ClassFussy2::s_destructorInvocations;
            my_ClassFussy2 *srcPtr = (my_ClassFussy2 *) &rawBuf[0];
            Util::construct(srcPtr, XA, VF2);
            const int CCI = my_ClassFussy2::s_copyConstructorInvocations;
            my_ClassFussy2 *objPtr = (my_ClassFussy2 *) &rawBuf[1];
            Util::destructiveMove(objPtr, XA, srcPtr);
            ASSERT(DI  == my_ClassFussy2::s_destructorInvocations);
            ASSERT(CCI == my_ClassFussy2::s_copyConstructorInvocations);
            ASSERT(6   == rawBuf[0].d_value);
            ASSERT(0   == rawBuf[0].d_allocator_p);
            ASSERT(6   == rawBuf[1].d_value);
            ASSERT(0   == rawBuf[1].d_allocator_p);
            if (veryVerbose) {
                P_(rawBuf[0].d_value); PP(rawBuf[0].d_allocator_p);
                P_(rawBuf[1].d_value); PP(rawBuf[1].d_allocator_p);
            }
            objPtr->~my_ClassFussy2();
        }

      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING construct
        //
        // Concerns:
        //  o That arguments are forwarded in the proper order and
        //    number (typos could easily make a10 become the 11th argument to
        //    the constructor).
        //  o That allocators are forwarded appropriately according to the
        //    traits and to the type ('bslma::Allocator *' or 'void *').
        //  o That even though
        //
        // Plan: Construct an object in some uninitialized buffer, and verify
        //   that the value and allocator is as expected.  In order to
        //   ascertain the proper forwarding of the arguments, use different
        //   types and values.
        //
        // Testing:
        //   construct(T *dst, A[1--N]..., *a);
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING construct"
                            "\n=================\n");

        bslma::Allocator     *const DA = bslma::Default::allocator();
        bslma::TestAllocator testAllocator(veryVeryVeryVerbose);
        bslma::TestAllocator *const TA = &testAllocator;
        int                  dummyAllocator;  // dummy, non-'bslma' allocator
        int                  *const XA = &dummyAllocator;

        if (verbose) printf("TEST_CONSTRUCT (without allocators).\n");

        // OP  = construct(&ConstructTestArgNoAlloc, VNA[1--N], TA)
        // EXP = ConstructTestArgNoAlloc(VNA[1--N])
        // ---   -------------------------------------------------
        TEST_CONSTRUCT(                                                  // OP
                       construct(objPtr, TA, VNA1),
                       (VNA1)                                            // EXP
                      );

        TEST_CONSTRUCT(                                                  // OP
                       construct(objPtr, TA, VNA1, VNA2),
                       (VNA1, VNA2)                                      // EXP
                      );

        TEST_CONSTRUCT(                                                  // OP
                       construct(objPtr, TA, VNA1, VNA2, VNA3),
                       (VNA1, VNA2, VNA3)                                // EXP
                      );

        TEST_CONSTRUCT(                                                  // OP
                       construct(objPtr, TA, VNA1, VNA2, VNA3, VNA4),
                       (VNA1, VNA2, VNA3, VNA4)                          // EXP
                      );

        TEST_CONSTRUCT(construct(objPtr, TA, VNA1, VNA2, VNA3, VNA4, VNA5
                                         ),                              // OP
                       (VNA1, VNA2, VNA3, VNA4, VNA5)                    // EXP
                      );

        TEST_CONSTRUCT(construct(objPtr, TA, VNA1, VNA2, VNA3, VNA4, VNA5,
                                             VNA6),                      // OP
                       (VNA1, VNA2, VNA3, VNA4, VNA5, VNA6)              // EXP
                      );

        TEST_CONSTRUCT(construct(objPtr, TA, VNA1, VNA2, VNA3, VNA4, VNA5,
                                             VNA6, VNA7),                // OP
                       (VNA1, VNA2, VNA3, VNA4, VNA5, VNA6, VNA7)        // EXP
                      );

        TEST_CONSTRUCT(construct(objPtr, TA, VNA1, VNA2, VNA3, VNA4, VNA5,
                                             VNA6, VNA7, VNA8),          // OP
                       (VNA1, VNA2, VNA3, VNA4, VNA5, VNA6, VNA7, VNA8)  // EXP
                      );

        TEST_CONSTRUCT(construct(objPtr, TA, VNA1, VNA2, VNA3, VNA4, VNA5,
                                             VNA6, VNA7, VNA8, VNA9),    // OP
                       (VNA1, VNA2, VNA3, VNA4, VNA5, VNA6, VNA7, VNA8,
                        VNA9)                                            // EXP
                      );

        TEST_CONSTRUCT(construct(objPtr, TA, VNA1, VNA2, VNA3, VNA4, VNA5,
                                             VNA6, VNA7, VNA8, VNA9, VNA10
                                         ),                              // OP
                       (VNA1, VNA2, VNA3, VNA4, VNA5, VNA6, VNA7,
                        VNA8, VNA9, VNA10)                               // EXP
                      );

        TEST_CONSTRUCT(construct(objPtr, TA, VNA1, VNA2, VNA3, VNA4, VNA5,
                                             VNA6, VNA7, VNA8, VNA9, VNA10,
                                             VNA11),                     // OP
                       (VNA1, VNA2, VNA3, VNA4, VNA5, VNA6, VNA7, VNA8,
                        VNA9, VNA10, VNA11)                              // EXP
                      );

        TEST_CONSTRUCT(construct(objPtr, TA, VNA1, VNA2, VNA3, VNA4, VNA5,
                                             VNA6, VNA7, VNA8, VNA9, VNA10,
                                             VNA11, VNA12),              // OP
                       (VNA1, VNA2, VNA3, VNA4, VNA5, VNA6, VNA7, VNA8,
                        VNA9, VNA10, VNA11, VNA12)                       // EXP
                      );

        TEST_CONSTRUCT(construct(objPtr, TA, VNA1, VNA2, VNA3, VNA4, VNA5,
                                             VNA6, VNA7, VNA8, VNA9, VNA10,
                                             VNA11, VNA12, VNA13),       // OP
                       (VNA1, VNA2, VNA3, VNA4, VNA5, VNA6, VNA7, VNA8,
                        VNA9, VNA10, VNA11, VNA12, VNA13)                // EXP
                      );

        TEST_CONSTRUCT(construct(objPtr, TA, VNA1, VNA2, VNA3, VNA4, VNA5,
                                             VNA6, VNA7, VNA8, VNA9, VNA10,
                                             VNA11, VNA12, VNA13, VNA14),// OP
                       (VNA1, VNA2, VNA3, VNA4, VNA5, VNA6, VNA7, VNA8,
                        VNA9, VNA10, VNA11, VNA12, VNA13, VNA14)         // EXP
                      );

        if (verbose) printf("TEST_CONSTRUCTA (with bslma::Allocator*).\n");

        // OP  = construct(&ConstructTestArgAlloc, VA[1--N], TA)
        // EXP = ConstructTestArgAlloc(VA[1--N])
        // ---   -------------------------------------------------
        TEST_CONSTRUCTA(                                                  // OP
                        construct(objPtr, TA, VA1),
                        (VA1),                                         // EXP
                        TA, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);    // ALLOC

        TEST_CONSTRUCTA(                                                  // OP
                        construct(objPtr, TA, VA1, VA2),
                        (VA1, VA2),                                    // EXP
                        TA, TA, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);   // ALLOC

        TEST_CONSTRUCTA(                                                  // OP
                        construct(objPtr, TA, VA1, VA2, VA3),
                        (VA1, VA2, VA3),                               // EXP
                        TA, TA, TA, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);  // ALLOC

        TEST_CONSTRUCTA(                                                  // OP
                        construct(objPtr, TA, VA1, VA2, VA3, VA4),
                        (VA1, VA2, VA3, VA4),                          // EXP
                        TA, TA, TA, TA, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0); // ALLOC

        TEST_CONSTRUCTA(                                                  // OP
                        construct(objPtr, TA, VA1, VA2, VA3, VA4, VA5),
                        (VA1, VA2, VA3, VA4, VA5),                     // EXP
                        TA, TA, TA, TA, TA, 0, 0, 0, 0, 0, 0, 0, 0, 0);// ALLOC

        TEST_CONSTRUCTA(construct(objPtr, TA, VA1, VA2, VA3, VA4, VA5,
                                              VA6),                    // OP
                        (VA1, VA2, VA3, VA4, VA5, VA6),                // EXP
                        TA, TA, TA, TA, TA, TA,
                                              0, 0, 0, 0, 0, 0, 0, 0); // ALLOC

        TEST_CONSTRUCTA(construct(objPtr, TA, VA1, VA2, VA3, VA4, VA5,
                                              VA6, VA7),                // OP
                        (VA1, VA2, VA3, VA4, VA5, VA6, VA7),            // EXP
                        TA, TA, TA, TA, TA, TA, TA,
                                                 0, 0, 0, 0, 0, 0, 0); // ALLOC

        TEST_CONSTRUCTA(construct(objPtr, TA, VA1, VA2, VA3, VA4, VA5,
                                              VA6, VA7, VA8),          // OP
                        (VA1, VA2, VA3, VA4, VA5, VA6, VA7, VA8),      // EXP
                        TA, TA, TA, TA, TA, TA, TA, TA,
                                                    0, 0, 0, 0, 0, 0); // ALLOC

        TEST_CONSTRUCTA(construct(objPtr, TA, VA1, VA2, VA3, VA4, VA5,
                                              VA6, VA7, VA8, VA9),     // OP
                        (VA1, VA2, VA3, VA4, VA5, VA6, VA7, VA8,
                         VA9),                                         // EXP
                        TA, TA, TA, TA, TA, TA, TA, TA, TA,
                                                       0, 0, 0, 0, 0); // ALLOC

        TEST_CONSTRUCTA(construct(objPtr, TA, VA1, VA2, VA3, VA4, VA5,
                                              VA6, VA7, VA8, VA9, VA10
                                           ),                          // OP
                        (VA1, VA2, VA3, VA4, VA5, VA6, VA7,
                         VA8, VA9, VA10),                              // EXP
                        TA, TA, TA, TA, TA, TA, TA, TA, TA, TA,
                                                          0, 0, 0, 0); // ALLOC

        TEST_CONSTRUCTA(construct(objPtr, TA, VA1, VA2, VA3, VA4, VA5,
                                              VA6, VA7, VA8, VA9, VA10,
                                              VA11),                   // OP
                        (VA1, VA2, VA3, VA4, VA5, VA6, VA7, VA8,
                         VA9, VA10, VA11),                             // EXP
                        TA, TA, TA, TA, TA, TA, TA, TA, TA, TA, TA,
                                                             0, 0, 0); // ALLOC

        TEST_CONSTRUCTA(construct(objPtr, TA, VA1, VA2, VA3, VA4, VA5,
                                              VA6, VA7, VA8, VA9, VA10,
                                              VA11, VA12),             // OP
                        (VA1, VA2, VA3, VA4, VA5, VA6, VA7, VA8,
                         VA9, VA10, VA11, VA12),                       // EXP
                        TA, TA, TA, TA, TA, TA, TA, TA, TA, TA, TA, TA,
                                                                0, 0); // ALLOC

        TEST_CONSTRUCTA(construct(objPtr, TA, VA1, VA2, VA3, VA4, VA5,
                                              VA6, VA7, VA8, VA9, VA10,
                                              VA11, VA12, VA13),       // OP
                        (VA1, VA2, VA3, VA4, VA5, VA6, VA7, VA8,
                         VA9, VA10, VA11, VA12, VA13),                 // EXP
                        TA, TA, TA, TA, TA, TA, TA, TA, TA, TA, TA, TA, TA,
                                                                   0); // ALLOC
        TEST_CONSTRUCTA(construct(objPtr, TA, VA1, VA2, VA3, VA4, VA5,
                                              VA6, VA7, VA8, VA9, VA10,
                                              VA11, VA12, VA13, VA14), // OP
                        (VA1, VA2, VA3, VA4, VA5, VA6, VA7, VA8,
                         VA9, VA10, VA11, VA12, VA13, VA14),           // EXP
                        TA, TA, TA, TA, TA, TA, TA, TA, TA, TA, TA, TA, TA,
                                                                  TA); // ALLOC

        if (verbose) printf("TEST_CONSTRUCTA (with void *).\n");

        // OP  = construct(&ConstructTestArgAlloc, VA[1--N], XA)
        // EXP = ConstructTestArgAlloc(VA[1--N])
        // ---   -------------------------------------------------
        TEST_CONSTRUCTA(                                                  // OP
                        construct(objPtr, XA, VA1),
                        (VA1),                                         // EXP
                        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);     // ALLOC

        TEST_CONSTRUCTA(                                                  // OP
                        construct(objPtr, XA, VA1, VA2),
                        (VA1, VA2),                                    // EXP
                        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);     // ALLOC

        TEST_CONSTRUCTA(                                                  // OP
                        construct(objPtr, XA, VA1, VA2, VA3),
                        (VA1, VA2, VA3),                               // EXP
                        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);     // ALLOC

        TEST_CONSTRUCTA(                                                  // OP
                        construct(objPtr, XA, VA1, VA2, VA3, VA4),
                        (VA1, VA2, VA3, VA4),                          // EXP
                        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);     // ALLOC

        TEST_CONSTRUCTA(                                                  // OP
                        construct(objPtr, XA, VA1, VA2, VA3, VA4, VA5),
                        (VA1, VA2, VA3, VA4, VA5),                     // EXP
                        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);     // ALLOC

        TEST_CONSTRUCTA(construct(objPtr, XA, VA1, VA2, VA3, VA4, VA5,
                                              VA6),                    // OP
                        (VA1, VA2, VA3, VA4, VA5, VA6),                // EXP
                        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);     // ALLOC

        TEST_CONSTRUCTA(construct(objPtr, XA, VA1, VA2, VA3, VA4, VA5,
                                              VA6, VA7),               // OP
                        (VA1, VA2, VA3, VA4, VA5, VA6, VA7),           // EXP
                        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);     // ALLOC

        TEST_CONSTRUCTA(construct(objPtr, XA, VA1, VA2, VA3, VA4, VA5,
                                              VA6, VA7, VA8),          // OP
                        (VA1, VA2, VA3, VA4, VA5, VA6, VA7, VA8),      // EXP
                        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);     // ALLOC

        TEST_CONSTRUCTA(construct(objPtr, XA, VA1, VA2, VA3, VA4, VA5,
                                              VA6, VA7, VA8, VA9),     // OP
                        (VA1, VA2, VA3, VA4, VA5, VA6, VA7, VA8,
                         VA9),                                         // EXP
                        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);     // ALLOC

        TEST_CONSTRUCTA(construct(objPtr, XA, VA1, VA2, VA3, VA4, VA5,
                                              VA6, VA7, VA8, VA9, VA10
                                         ),                            // OP
                        (VA1, VA2, VA3, VA4, VA5, VA6, VA7,
                         VA8, VA9, VA10),                              // EXP
                        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);     // ALLOC

        TEST_CONSTRUCTA(construct(objPtr, XA, VA1, VA2, VA3, VA4, VA5,
                                              VA6, VA7, VA8, VA9, VA10,
                                              VA11),                   // OP
                        (VA1, VA2, VA3, VA4, VA5, VA6, VA7, VA8,
                         VA9, VA10, VA11),                             // EXP
                        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);     // ALLOC

        TEST_CONSTRUCTA(construct(objPtr, XA, VA1, VA2, VA3, VA4, VA5,
                                              VA6, VA7, VA8, VA9, VA10,
                                              VA11, VA12),             // OP
                        (VA1, VA2, VA3, VA4, VA5, VA6, VA7, VA8,
                         VA9, VA10, VA11, VA12),                       // EXP
                        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);     // ALLOC

        TEST_CONSTRUCTA(construct(objPtr, XA, VA1, VA2, VA3, VA4, VA5,
                                              VA6, VA7, VA8, VA9, VA10,
                                              VA11, VA12, VA13),       // OP
                        (VA1, VA2, VA3, VA4, VA5, VA6, VA7, VA8,
                         VA9, VA10, VA11, VA12, VA13),                 // EXP
                        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);     // ALLOC

        TEST_CONSTRUCTA(construct(objPtr, XA, VA1, VA2, VA3, VA4, VA5,
                                              VA6, VA7, VA8, VA9, VA10,
                                              VA11, VA12, VA13, VA14), // OP
                        (VA1, VA2, VA3, VA4, VA5, VA6, VA7, VA8,
                         VA9, VA10, VA11, VA12, VA13, VA14),           // EXP
                        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);     // ALLOC

        if (verbose) printf("Exception testing\n");

        if (verbose) printf("\t...constructing pair with allocators\n");

        BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
            my_ClassDef rawBuf[2];
            my_PairAA_4_4 *objPtr = (my_PairAA_4_4 *)rawBuf;
            Util::construct(objPtr, TA, PAAV4V4);
            ASSERT(4  == rawBuf[0].d_value);
            ASSERT(TA == rawBuf[0].d_allocator_p);
            ASSERT(4  == rawBuf[1].d_value);
            ASSERT(TA == rawBuf[1].d_allocator_p);
            objPtr->~my_PairAA_4_4();
        } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

        BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
            my_ClassDef rawBuf[2];
            my_PairAA_4_4 *objPtr = (my_PairAA_4_4 *)rawBuf;
            Util::construct(objPtr, TA, V4, V4);
            ASSERT(4  == rawBuf[0].d_value);
            ASSERT(TA == rawBuf[0].d_allocator_p);
            ASSERT(4  == rawBuf[1].d_value);
            ASSERT(TA == rawBuf[1].d_allocator_p);
            objPtr->~my_PairAA_4_4();
        } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

        if (verbose) printf("\t...constructing pair with IsPair\n");

        // Testing that 'TA' is not passed to the pair constructor and the
        // default allocator is used instead.
        const bsls::Types::Int64 NUM_ALLOC1 = testAllocator.numAllocations();
        BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
            my_ClassDef rawBuf[2];
            my_PairBB_4_4 *objPtr = (my_PairBB_4_4 *)rawBuf;
            Util::construct(objPtr, TA, PBBV4V4);
            ASSERT(4  == rawBuf[0].d_value);
            ASSERT(DA == rawBuf[0].d_allocator_p);
            ASSERT(4  == rawBuf[1].d_value);
            ASSERT(DA == rawBuf[1].d_allocator_p);
            objPtr->~my_PairBB_4_4();
        } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
        ASSERT(NUM_ALLOC1 == testAllocator.numAllocations());

        const bsls::Types::Int64 NUM_ALLOC2 = testAllocator.numAllocations();
        BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
            my_ClassDef rawBuf[2];
            my_PairBB_4_4 *objPtr = (my_PairBB_4_4 *)rawBuf;
            Util::construct(objPtr, TA, V4, V4);
            ASSERT(4  == rawBuf[0].d_value);
            ASSERT(DA == rawBuf[0].d_allocator_p);
            ASSERT(4  == rawBuf[1].d_value);
            ASSERT(DA == rawBuf[1].d_allocator_p);
            objPtr->~my_PairBB_4_4();
        } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
        ASSERT(NUM_ALLOC2 == testAllocator.numAllocations());

        if (verbose) printf("Trait selection testing.\n");
        {
            my_ClassDef rawBuf;
            my_ClassFussy *objPtr = (my_ClassFussy *) &rawBuf;
            memset(&rawBuf, 92, sizeof rawBuf);
            Util::construct(objPtr, (bslma::Allocator*)TA, VF);
            ASSERT(5 == rawBuf.d_value);
            ASSERT(0 == rawBuf.d_allocator_p);
            if (veryVerbose) { P_(rawBuf.d_value); PP(rawBuf.d_allocator_p); }
        }

        {
            my_ClassDef rawBuf;
            my_ClassFussy *objPtr = (my_ClassFussy *) &rawBuf;
            memset(&rawBuf, 92, sizeof rawBuf);
            const int CVI = my_ClassFussy::s_conversionConstructorInvocations;
            const int VF  = 3;
            Util::construct(objPtr, (bslma::Allocator*)TA, VF);
            ASSERT(CVI < my_ClassFussy::s_conversionConstructorInvocations);
            ASSERT(3 == rawBuf.d_value);
            ASSERT(0 == rawBuf.d_allocator_p);
            if (veryVerbose) { P_(rawBuf.d_value); PP(rawBuf.d_allocator_p); }
        }

      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING moveConstruct
        //
        // Concerns:
        //   o That the move constructor properly forwards the allocator
        //     when appropriate.
        //   o That the move constructor uses memcpy when appropriate.
        //   o That the move constructor leaves the moved-from object in an
        //     appropriate state.
        //
        // Plan: Construct a copy of pre-initialized objects into an
        //   uninitialized buffer using move construction, passing allocators
        //   of both types 'bslma::Allocator *' and 'void *' types, and verify
        //   that the values and allocator of the copy are as expected.  Using
        //   a fussy type that has the BitwiseCopyable trait, ensure that the
        //   copy constructor and move constructor is not invoked.
        //
        // Testing:
        //   moveConstruct(T *dst, T& src, *a);
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING moveConstruct"
                            "\n=====================\n");

        bslma::Allocator     *const DA = bslma::Default::allocator();
        bslma::TestAllocator testAllocator(veryVeryVerbose);
        bslma::TestAllocator *const TA = &testAllocator;
        int                  dummyAllocator;  // dummy, non-'bslma' allocator
        int                  *const XA = &dummyAllocator;

        // my_Class                                               Expected
        //      #  Operation                                      Val Alloc
        //      =  ============================================== === =====
        TEST_MV(1, construct(objPtr, TA, MoveUtil::move(fromObj)),  1, 0);
        TEST_MV(2, construct(objPtr, TA, MoveUtil::move(fromObj)),  2, TA);
        TEST_MV(1, construct(objPtr, XA, MoveUtil::move(fromObj)),  1, 0);
        TEST_MV(2, construct(objPtr, XA, MoveUtil::move(fromObj)),  2, 0);

        if (verbose) printf("Exception testing.\n");

        if (verbose) printf("\t...pair with allocators\n");

        BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
            my_ClassDef rawBuf[2];
            my_PairAA_4_4 *objPtr = (my_PairAA_4_4 *)rawBuf;
            my_PairAA_4_4 fromObj(PAAV4V4);
            Util::construct(objPtr, TA, MoveUtil::move(fromObj));
            ASSERT(4  == rawBuf[0].d_value);
            ASSERT(TA == rawBuf[0].d_allocator_p);
            ASSERT(4  == rawBuf[1].d_value);
            ASSERT(TA == rawBuf[1].d_allocator_p);
            ASSERT_IS_MOVED_FROM(fromObj.first);
            ASSERT_IS_MOVED_FROM(fromObj.second);
            objPtr->~my_PairAA_4_4();

            memset(rawBuf, 91, sizeof(rawBuf));
            fromObj = PAAV4V4;
            Util::construct(objPtr, XA, MoveUtil::move(fromObj));
            ASSERT(4  == rawBuf[0].d_value);
            ASSERT(DA == rawBuf[0].d_allocator_p);
            ASSERT(4  == rawBuf[1].d_value);
            ASSERT(DA == rawBuf[1].d_allocator_p);
            ASSERT_IS_MOVED_FROM(fromObj.first);
            ASSERT_IS_MOVED_FROM(fromObj.second);
            objPtr->~my_PairAA_4_4();
        } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

        if (verbose) printf("\t...constructing pair with IsPair\n");

        // Testing that 'TA' is not passed to the pair constructor and the
        // default allocator is used instead.
        const bsls::Types::Int64 NUM_ALLOC1 = testAllocator.numAllocations();
        BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
            my_ClassDef rawBuf[2];
            my_PairBB_4_4 *objPtr = (my_PairBB_4_4 *)rawBuf;;
            my_PairBB_4_4 fromObj(PBBV4V4);
            Util::construct(objPtr, TA, MoveUtil::move(fromObj));
            ASSERT(4  == rawBuf[0].d_value);
            ASSERT(DA == rawBuf[0].d_allocator_p);
            ASSERT(4  == rawBuf[1].d_value);
            ASSERT(DA == rawBuf[1].d_allocator_p);
            ASSERT_IS_MOVED_FROM(fromObj.first);
            ASSERT_IS_MOVED_FROM(fromObj.second);
            objPtr->~my_PairBB_4_4();

            fromObj = PBBV4V4;
            Util::construct(objPtr, XA, MoveUtil::move(fromObj));
            ASSERT(4  == rawBuf[0].d_value);
            ASSERT(DA == rawBuf[0].d_allocator_p);
            ASSERT(4  == rawBuf[1].d_value);
            ASSERT(DA == rawBuf[1].d_allocator_p);
            ASSERT_IS_MOVED_FROM(fromObj.first);
            ASSERT_IS_MOVED_FROM(fromObj.second);
            objPtr->~my_PairBB_4_4();
        } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
        ASSERT(NUM_ALLOC1 == testAllocator.numAllocations());

        if (verbose) printf("Trait selection testing.\n");
        {
            my_ClassDef rawBuf;
            my_ClassFussy *objPtr = (my_ClassFussy *) &rawBuf;
            my_ClassFussy fromObj(3);
            memset(&rawBuf, 92, sizeof rawBuf);
            Util::construct(objPtr, XA, MoveUtil::move(fromObj));
            ASSERT(3 == rawBuf.d_value);
            ASSERT(0 == rawBuf.d_allocator_p);
            if (veryVerbose) { P_(rawBuf.d_value); PP(rawBuf.d_allocator_p); }
        }

      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING copyConstruct
        //
        // Concerns:
        //   o That the copy constructor properly forwards the allocator
        //     when appropriate.
        //   o That the copy constructor uses memcpy when appropriate.
        //
        // Plan: Construct a copy of pre-initialized objects into an
        //   uninitialized buffer, passing allocators of both types
        //   'bslma::Allocator *' and 'void *' types, and verify that the
        //   values and allocator of the copy are as expected.  Using a fussy
        //   type that has the BitwiseCopyable trait, ensure that the copy
        //   constructor is not invoked.
        //
        // Testing:
        //   copyConstruct(T *dst, const T& src, *a);
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING copyConstruct"
                            "\n=====================\n");

        bslma::Allocator     *const DA = bslma::Default::allocator();
        bslma::TestAllocator testAllocator(veryVeryVeryVerbose);
        bslma::TestAllocator *const TA = &testAllocator;
        int                  dummyAllocator;  // dummy, non-'bslma' allocator
        int                  *const XA = &dummyAllocator;

        // my_Class                               Expected
        //      #  Operation                      Val Alloc
        //      =  ============================== === =====
        TEST_OP(1, construct(objPtr, TA, V1),  1, 0);
        TEST_OP(2, construct(objPtr, TA, V2),  2, TA);
        TEST_OP(1, construct(objPtr, XA, V1),  1, 0);
        TEST_OP(2, construct(objPtr, XA, V2),  2, 0);

        if (verbose) printf("Exception testing.\n");

        if (verbose) printf("\t...pair with allocators\n");

        BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
            my_ClassDef rawBuf[2];
            my_PairAA_4_4 *objPtr = (my_PairAA_4_4 *)rawBuf;
            Util::construct(objPtr, TA, PAAV4V4);
            ASSERT(4  == rawBuf[0].d_value);
            ASSERT(TA == rawBuf[0].d_allocator_p);
            ASSERT(4  == rawBuf[1].d_value);
            ASSERT(TA == rawBuf[1].d_allocator_p);
            objPtr->~my_PairAA_4_4();
        } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

        if (verbose) printf("\t...constructing pair with IsPair\n");

        // Testing that 'TA' is not passed to the pair constructor and the
        // default allocator is used instead.
        const bsls::Types::Int64 NUM_ALLOC1 = testAllocator.numAllocations();
        BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
            my_ClassDef rawBuf[2];
            my_PairBB_4_4 *objPtr = (my_PairBB_4_4 *)rawBuf;
            Util::construct(objPtr, TA, PBBV4V4);
            ASSERT(4  == rawBuf[0].d_value);
            ASSERT(DA == rawBuf[0].d_allocator_p);
            ASSERT(4  == rawBuf[1].d_value);
            ASSERT(DA == rawBuf[1].d_allocator_p);
            objPtr->~my_PairBB_4_4();
        } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
        ASSERT(NUM_ALLOC1 == testAllocator.numAllocations());

        if (verbose) printf("Trait selection testing.\n");
        {
            my_ClassDef rawBuf;
            my_ClassFussy *objPtr = (my_ClassFussy *) &rawBuf;
            memset(&rawBuf, 92, sizeof rawBuf);
            Util::construct(objPtr, XA, VF);
            ASSERT(5 == rawBuf.d_value);
            ASSERT(0 == rawBuf.d_allocator_p);
            if (veryVerbose) { P_(rawBuf.d_value); PP(rawBuf.d_allocator_p); }
        }

      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING defaultConstruct
        //
        // Concerns:
        //   o That the default constructor properly forwards the allocator
        //     when appropriate.
        //   o That a pair type properly forwards to both its members, in an
        //     exception-safe manner.
        //   o That the default constructor uses memset(..,0,..) when
        //     appropriate.
        //
        // Plan: Construct an object into a default state into an uninitialized
        //   buffer, passing allocators of both types 'bslma::Allocator *' and
        //   'void *', and verify that the value and allocator of the object
        //   are as expected.  Using a fussy type that has the
        //   TrivialDefaultConstructor trait (even though the default
        //   constructor is not really trivial), ensure that this constructor
        //   is not invoked.
        //
        // Testing:
        //   defaultConstruct(T *dst, *a);
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING defaultConstruct"
                            "\n========================\n");

        bslma::Allocator     *const DA = bslma::Default::allocator();
        bslma::TestAllocator testAllocator(veryVeryVeryVerbose);
        bslma::TestAllocator *const TA = &testAllocator;
        int                  dummyAllocator;  // dummy, non-'bslma' allocator
        int                  *const XA = &dummyAllocator;

        if (verbose) printf("Value and allocator testing.\n");

        //   my_Class                             Expected
        //      #      Operation                 Val Alloc
        //      == ============================= === =====
        TEST_OP(1, construct(objPtr, TA),  0, 0);
        TEST_OP(2, construct(objPtr, TA),  0, TA);
        TEST_OP(1, construct(objPtr, XA),  0, 0);
        TEST_OP(2, construct(objPtr, XA),  0, 0);

        // my_Pair                                        first    second
        //           #_#   Operation                    Val Alloc Val Alloc
        //          == == ============================= === ===== === =====
        TEST_PAIROP(1, 1, construct(objPtr, TA),  0, 0,     0,  0);
        TEST_PAIROP(1, 2, construct(objPtr, TA),  0, 0,     0,  0);
        TEST_PAIROP(2, 1, construct(objPtr, TA),  0, 0,     0,  0);
        TEST_PAIROP(2, 2, construct(objPtr, TA),  0, 0,     0,  0);

        TEST_PAIROP(1, 1, construct(objPtr, XA),  0, 0,     0,  0);
        TEST_PAIROP(1, 2, construct(objPtr, XA),  0, 0,     0,  0);
        TEST_PAIROP(2, 1, construct(objPtr, XA),  0, 0,     0,  0);
        TEST_PAIROP(2, 2, construct(objPtr, XA),  0, 0,     0,  0);

        // my_PairA                                        first    second
        //            #_#   Operation                    Val Alloc Val Alloc
        //           == == ============================= === ===== === =====
        TEST_PAIRAOP(1, 2, construct(objPtr, TA),  0,  0,    0, TA);
        TEST_PAIRAOP(2, 2, construct(objPtr, TA),  0,  0,    0, TA);

        TEST_PAIRAOP(1, 2, construct(objPtr, XA),  0,  0,    0, 0);
        TEST_PAIRAOP(2, 2, construct(objPtr, XA),  0,  0,    0, 0);

        // my_PairAA                                       first    second
        //            #_#   Operation                    Val Alloc Val Alloc
        //           == == ============================= === ===== === =====
        TEST_PAIRAAOP(2, 2, construct(objPtr, TA),  0, TA,    0, TA);
        TEST_PAIRAAOP(2, 2, construct(objPtr, XA),  0,  0,    0, 0);

        if (verbose) printf("Exception testing.\n");

        if (verbose) printf("\t...constructing pair with allocators\n");

        BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
            my_ClassDef rawBuf[2];
            my_PairAA_4_4 *objPtr = (my_PairAA_4_4 *)rawBuf;
            Util::construct(objPtr, TA);
            ASSERT(0  == rawBuf[0].d_value);
            ASSERT(TA == rawBuf[0].d_allocator_p);
            ASSERT(0  == rawBuf[1].d_value);
            ASSERT(TA == rawBuf[1].d_allocator_p);
            objPtr->~my_PairAA_4_4();
        } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

        if (verbose) printf("\t...constructing pair with IsPair\n");

        // Testing that 'TA' is not passed to the pair constructor and the
        // default allocator is used instead.
        const bsls::Types::Int64 NUM_ALLOC1 = testAllocator.numAllocations();
        BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
            my_ClassDef rawBuf[2];
            my_PairBB_4_4 *objPtr = (my_PairBB_4_4 *)rawBuf;
            Util::construct(objPtr, TA);
            ASSERT(0  == rawBuf[0].d_value);
            ASSERT(DA == rawBuf[0].d_allocator_p);
            ASSERT(0  == rawBuf[1].d_value);
            ASSERT(DA == rawBuf[1].d_allocator_p);
            objPtr->~my_PairBB_4_4();
        } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
        ASSERT(NUM_ALLOC1 == testAllocator.numAllocations());

        if (verbose) printf("Trait selection testing.\n");
        {
            my_ClassDef rawBuf;
            my_ClassFussy *objPtr = (my_ClassFussy *) &rawBuf;
            memset(&rawBuf, 92, sizeof rawBuf);
            Util::construct(objPtr, XA);
            ASSERT(0 == rawBuf.d_value);
            ASSERT(0 == rawBuf.d_allocator_p);
            if (veryVerbose) { P_(rawBuf.d_value); PP(rawBuf.d_allocator_p); }
        }

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING TEST APPARATUS
        //
        // Concerns:  The test apparatus must work properly.
        //
        // Plan: Simply test the values of each constant defined.
        //
        // Testing:
        //    TEST APPARATUS
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING TEST APPARATUS"
                            "\n======================\n");

        ASSERT(1 == V1.value());
        ASSERT(2 == V2.value());

        ASSERT(1 == PV1V1.first.value());   ASSERT(1 == PV1V1.second.value());
        ASSERT(1 == PV1V2.first.value());   ASSERT(2 == PV1V2.second.value());
        ASSERT(2 == PV2V1.first.value());   ASSERT(1 == PV2V1.second.value());
        ASSERT(2 == PV2V2.first.value());   ASSERT(2 == PV2V2.second.value());

        ASSERT(1 == PAV1V2.first.value());  ASSERT(2 == PAV1V2.second.value());
        ASSERT(2 == PAV2V2.first.value());  ASSERT(2 == PAV2V2.second.value());

        ASSERT(2 == PAAV2V2.first.value());
                                           ASSERT(2 == PAAV2V2.second.value());

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //
        // Concerns:
        //: 1 That the templates can be instantiated without errors.
        //
        // Plan:
        //   Simply instantiate the templates in very simple examples,
        //   constructing or destroying an object stored in some buffer.  No
        //   thorough testing is performed, beyond simply asserting the call
        //   was forwarded properly by examining the value of the buffer
        //   storing the object.
        //
        // Testing:
        //   BREATHING
        // --------------------------------------------------------------------

        if (verbose) printf("\nBREATHING TEST"
                            "\n==============\n");

        my_Class1 v1(1);        ASSERT(1 == v1.value());
        my_Class2 v2(2);        ASSERT(2 == v2.value());

        my_ClassDef rawBuf;
        bslma::TestAllocator testAllocator(veryVeryVeryVerbose);
        bslma::Allocator *const theAlloc = &testAllocator;

        // 'defaultConstruct' invokes default constructor, with defaulted
        // arguments, even if type does not take an allocator.

        memset(&rawBuf, 92, sizeof(rawBuf));
        Util::construct((my_Class1*) &rawBuf, theAlloc);
        ASSERT(0 == rawBuf.d_allocator_p);
        ASSERT(0 == rawBuf.d_value);

        // 'copyConstruct' invokes copy constructor, even if type does not take
        // an allocator.

        memset(&rawBuf, 92, sizeof(rawBuf));
        Util::construct((my_Class1*) &rawBuf, theAlloc, v1);
        ASSERT(0 == rawBuf.d_allocator_p);
        ASSERT(1 == rawBuf.d_value);

        // 'copyConstruct' invokes copy constructor, passing the allocator if
        // type takes an allocator.

        memset(&rawBuf, 92, sizeof(rawBuf));
        Util::construct((my_Class2*) &rawBuf, theAlloc, v2);
        ASSERT(theAlloc == rawBuf.d_allocator_p);
        ASSERT(2 == rawBuf.d_value);

        // 'construct' invokes constructor, even if type does not take an
        // allocator.

        memset(&rawBuf, 92, sizeof(rawBuf));
        Util::construct((my_Class1*) &rawBuf, theAlloc, 3);
        ASSERT(0 == rawBuf.d_allocator_p);
        ASSERT(3 == rawBuf.d_value);

        // 'destroy' invokes destructor ... with no particular constraints.

        memset(&rawBuf, 92, sizeof(rawBuf));
        DestructionUtil::destroy((my_Class1*) &rawBuf);
        ASSERT(0  == rawBuf.d_allocator_p);
        ASSERT(91 == rawBuf.d_value);

        // 'construct' invokes constructor, passing the allocator if type takes
        // an allocator.

        memset(&rawBuf, 92, sizeof(rawBuf));
        Util::construct((my_Class2*) &rawBuf, theAlloc, 4);
        ASSERT(theAlloc == rawBuf.d_allocator_p);
        ASSERT(4 == rawBuf.d_value);

        // 'destroy' invokes destructor ... with no particular constraints.

        memset(&rawBuf, 92, sizeof(rawBuf));
        DestructionUtil::destroy((my_Class2*) &rawBuf);
        ASSERT(0  == rawBuf.d_allocator_p);
        ASSERT(92 == rawBuf.d_value);

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
