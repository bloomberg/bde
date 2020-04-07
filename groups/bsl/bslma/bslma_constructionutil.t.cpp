// bslma_constructionutil.t.cpp                                       -*-C++-*-
#include <bslma_constructionutil.h>

#include <bslma_autodestructor.h>
#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>

#include <bslmf_enableif.h>
#include <bslmf_isconvertible.h>
#include <bslmf_ispair.h>
#include <bslmf_movableref.h>

#include <bsls_bsltestutil.h>
#include <bsls_nameof.h>
#include <bsls_objectbuffer.h>

#include <utility>
#include <stdio.h>      // 'printf'
#include <stdlib.h>     // 'atoi'
#include <string.h>

using namespace BloombergLP;
using bsls::NameOf;

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
// a fussy type that will modify its (internal and/or class-static) state upon
// invocation of the copy constructor, but not when copying bit-wise.
//-----------------------------------------------------------------------------
// [ 3] construct(T *dst, *a);
// [ 4] construct(T *dst, const T& src, *a);
// [ 5] void construct(TYPE *addr, Allocator *a, MovableRef<TYPE> orig);
// [ 5] void construct(TYPE *addr, void      *a, MovableRef<TYPE> orig);
// [ 6] construct(T *dst, A[1--N]..., *a);
// [ 7] destructiveMove(T *dst, ALLOCATOR *a, TARGET_TYPE *src);
// [ 8]
// [ 9]
// [10]
// [11]
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [12] USAGE EXAMPLE
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

// STATIC DATA
static bool verbose;
static bool veryVerbose;
static bool veryVeryVerbose;
static bool veryVeryVeryVerbose;

//=============================================================================
//                             CLASSES FOR TESTING
//-----------------------------------------------------------------------------

                             // =================
                             // class MyClassDef
                             // =================

struct MyClassDef {
    // Data members that give MyClassX size and alignment.

    // DATA (exceptionally public, only in test driver)
    int               d_value;
    int              *d_data_p;
    bslma::Allocator *d_allocator_p;
};

// In optimized builds, some compilers will elide some of the operations in the
// destructors of the test classes defined below.  In order to force the
// compiler to retain all of the code in the destructors, we provide the
// following function that can be used to (conditionally) print out the state
// of a 'MyClassDef' data member.  If the destructor calls this function as
// its last operation, then all values set in the destructor have visible
// side-effects, but non-verbose test runs do not have to be burdened with
// additional output.

static bool forceDestructorCall = false;

void dumpClassDefState(const MyClassDef& def)
{
    if (forceDestructorCall) {
        printf("%p: %d %p %p\n",
               &def, def.d_value, def.d_data_p, def.d_allocator_p);
    }
}

                             // =================
                             // class MySrcClass
                             // =================

class MySrcClass {
    // Class to be used as a 'convert from' type.

  public:
    // DATA
    MyClassDef d_def;

    // DATA
    static int copyConstructorInvocations;
    static int moveConstructorInvocations;

    // CREATORS
    explicit
    MySrcClass(int v = 0)
    {
        d_def.d_value = v;
        d_def.d_allocator_p = 0;
    }

    MySrcClass(const MySrcClass& rhs)
    {
        d_def.d_value = rhs.d_def.d_value;
        d_def.d_allocator_p = 0;
        ++copyConstructorInvocations;
    }

    MySrcClass(bslmf::MovableRef<MySrcClass> rhs)
    {
        MySrcClass& lvalue = rhs;
        d_def.d_value = lvalue.d_def.d_value;
        lvalue.d_def.d_value = MOVED_FROM_VAL;
        d_def.d_allocator_p = 0;
        ++moveConstructorInvocations;
    }

    ~MySrcClass()
    {
        ASSERT(d_def.d_value != 91);
        d_def.d_value = 91;
        d_def.d_allocator_p = 0;
        dumpClassDefState(d_def);
    }

    MySrcClass& operator=(const MySrcClass& rhs)
    {
        d_def.d_value = rhs.d_def.d_value;
        return *this;
    }

    // ACCESSORS
    int value() const { return d_def.d_value; }
};

// CLASS DATA
int MySrcClass::copyConstructorInvocations       = 0;
int MySrcClass::moveConstructorInvocations       = 0;

                             // ===============
                             // class MyClass1
                             // ===============

class MyClass1 {
    // Class that doesn't take allocators.

    // DATA
    MyClassDef d_def;

  public:

    // DATA
    static int copyConstructorInvocations;
    static int moveConstructorInvocations;

    // CREATORS
    explicit
    MyClass1(int v = 0)
    {
        d_def.d_value = v;
        d_def.d_allocator_p = 0;
    }

    MyClass1(const MyClass1& rhs)
    {
        d_def.d_value = rhs.d_def.d_value;
        d_def.d_allocator_p = 0;
        ++copyConstructorInvocations;
    }

    MyClass1(bslmf::MovableRef<MyClass1> rhs)
    {
        MyClass1& lvalue = rhs;
        d_def.d_value = lvalue.d_def.d_value;
        lvalue.d_def.d_value = MOVED_FROM_VAL;
        d_def.d_allocator_p = 0;
        ++moveConstructorInvocations;
    }

    MyClass1(const MySrcClass& rhs)
    {
        d_def.d_value = rhs.value();
        d_def.d_allocator_p = 0;
    }

    MyClass1(bslmf::MovableRef<MySrcClass> rhs)
    {
        MySrcClass& lvalue = rhs;
        d_def.d_value = lvalue.value();
        lvalue.d_def.d_value = MOVED_FROM_VAL;
        d_def.d_allocator_p = 0;
    }

    ~MyClass1()
    {
        ASSERT(d_def.d_value != 91);
        d_def.d_value = 91;
        d_def.d_allocator_p = 0;
        dumpClassDefState(d_def);
    }

    MyClass1& operator=(const MyClass1& rhs)
    {
        d_def.d_value = rhs.d_def.d_value;
        return *this;
    }

    // ACCESSORS
    int value() const { return d_def.d_value; }
};

// CLASS DATA
int MyClass1::copyConstructorInvocations       = 0;
int MyClass1::moveConstructorInvocations       = 0;

                             // ===============
                             // class MyClass2
                             // ===============

class MyClass2 {
    // Class that takes allocators.

    // DATA
    MyClassDef d_def;

  public:

    // DATA
    static int copyConstructorInvocations;
    static int moveConstructorInvocations;

    // CREATORS
    explicit
    MyClass2(bslma::Allocator *a = 0)
    {
        d_def.d_value = 0;
        d_def.d_allocator_p = a;
    }

    explicit
    MyClass2(int v, bslma::Allocator *a = 0)
    {
        d_def.d_value = v;
        d_def.d_allocator_p = a;
    }

    MyClass2(const MyClass2& rhs, bslma::Allocator *a = 0)
    {
        d_def.d_value = rhs.d_def.d_value;
        d_def.d_allocator_p = a;
        ++copyConstructorInvocations;
    }

    MyClass2(bslmf::MovableRef<MyClass2> rhs, bslma::Allocator *a = 0)
    {
        MyClass2& lvalue = rhs;
        d_def.d_value = lvalue.d_def.d_value;
        lvalue.d_def.d_value = MOVED_FROM_VAL;
        if (a) {
            d_def.d_allocator_p = a;
        }
        else {
            d_def.d_allocator_p = lvalue.d_def.d_allocator_p;
        }
        ++moveConstructorInvocations;
    }

    MyClass2(const MySrcClass& rhs, bslma::Allocator *a = 0)
    {
        d_def.d_value = rhs.d_def.d_value;
        d_def.d_allocator_p = a;
    }

    MyClass2(bslmf::MovableRef<MySrcClass> rhs, bslma::Allocator *a = 0)
    {
        MySrcClass& lvalue = rhs;
        d_def.d_value = lvalue.d_def.d_value;
        lvalue.d_def.d_value = MOVED_FROM_VAL;
        d_def.d_allocator_p = a;
    }

    ~MyClass2()
    {
        ASSERT(d_def.d_value != 92);
        d_def.d_value = 92;
        d_def.d_allocator_p = 0;
        dumpClassDefState(d_def);
    }

    // MANIPULATORS
    MyClass2& operator=(const MyClass2& rhs)
    {
        d_def.d_value = rhs.d_def.d_value;
        // do not touch allocator!
        return *this;
    }

    // ACCESSORS
    int value() const { return d_def.d_value; }
    bslma::Allocator *allocator() const { return d_def.d_allocator_p; }
};

// CLASS DATA
int MyClass2::copyConstructorInvocations       = 0;
int MyClass2::moveConstructorInvocations       = 0;

// TRAITS
namespace BloombergLP {
namespace bslma {

template <>
struct UsesBslmaAllocator<MyClass2> : bsl::true_type {};

}  // close package namespace
}  // close enterprise namespace

                                 // ==========
                                 // MyClass2a
                                 // ==========

class MyClass2a {
    // This 'class' behaves the same as 'MyClass2' (allocator-aware type that
    // never actually allocates memory) except that it uses the
    // 'allocator_arg_t' idiom for passing an allocator to constructors.

    // DATA
    MyClass2 d_data;

  public:

    // DATA
    static int copyConstructorInvocations;
    static int moveConstructorInvocations;

    // CREATORS
    MyClass2a()
    : d_data()
    {
    }

    MyClass2a(bsl::allocator_arg_t, bslma::Allocator *a)
    : d_data(a)
    {
    }

    explicit
    MyClass2a(int v)
    : d_data(v)
    {
    }

    MyClass2a(bsl::allocator_arg_t, bslma::Allocator *a, int v)
    : d_data(v, a)
    {
    }

    MyClass2a(const MyClass2a& rhs)
    : d_data(rhs.d_data)
    {
        ++copyConstructorInvocations;
    }

    MyClass2a(bsl::allocator_arg_t  ,
              bslma::Allocator     *a,
              const MyClass2a&      rhs)
    : d_data(rhs.d_data, a)
    {
        ++copyConstructorInvocations;
    }

    MyClass2a(bslmf::MovableRef<MyClass2a> rhs)                   // IMPLICIT
    : d_data(MoveUtil::move(MoveUtil::access(rhs).d_data))
    {
        ++moveConstructorInvocations;
    }

    MyClass2a(bsl::allocator_arg_t,
              bslma::Allocator              *a,
              bslmf::MovableRef<MyClass2a>   rhs)
    : d_data(MoveUtil::move(MoveUtil::access(rhs).d_data), a)
    {
        ++moveConstructorInvocations;
    }

    MyClass2a(bsl::allocator_arg_t  ,
              bslma::Allocator     *a,
              const MySrcClass&     rhs)
    : d_data(rhs, a)
    {
    }

    MyClass2a(bsl::allocator_arg_t,
              bslma::Allocator              *a,
              bslmf::MovableRef<MySrcClass>  rhs)
    : d_data(MoveUtil::move(rhs), a)
    {
    }

    // MANIPULATORS
    MyClass2a& operator=(const MyClass2a& rhs)
    {
        d_data.operator=(rhs.d_data);
        return *this;
    }

    MyClass2a& operator=(bslmf::MovableRef<MyClass2a> rhs)
    {
        d_data.operator=(MoveUtil::move(MoveUtil::access(rhs).d_data));
        return *this;
    }

    // ACCESSORS
    int value() const { return d_data.value(); }
    bslma::Allocator *allocator() const { return d_data.allocator(); }
};

// CLASS DATA
int MyClass2a::copyConstructorInvocations       = 0;
int MyClass2a::moveConstructorInvocations       = 0;

// TRAITS
namespace BloombergLP {
namespace bslmf {
template <> struct UsesAllocatorArgT<MyClass2a> : bsl::true_type {};
}  // close namespace bslmf

namespace bslma {
template <> struct UsesBslmaAllocator<MyClass2a> : bsl::true_type {};
}  // close package namespace
}  // close enterprise namespace


                             // ===============
                             // class MyClass3
                             // ===============

class MyClass3 {
    // This 'class' takes allocators similarly to 'MyClass2', but does not
    // have an explicit move constructor (moves call the corresponding copy
    // operation).

    // DATA
    MyClassDef d_def;

  public:

    // DATA
    static int copyConstructorInvocations;

    // CREATORS
    explicit
    MyClass3(bslma::Allocator *a = 0)
    {
        d_def.d_value = 0;
        d_def.d_allocator_p = a;
    }

    explicit
    MyClass3(int v, bslma::Allocator *a = 0)
    {
        d_def.d_value = v;
        d_def.d_allocator_p = a;
    }

    MyClass3(const MyClass3& rhs, bslma::Allocator *a = 0)
    {
        d_def.d_value = rhs.d_def.d_value;
        d_def.d_allocator_p = a;
        ++copyConstructorInvocations;
    }

    MyClass3(const MySrcClass& rhs, bslma::Allocator *a = 0)
    {
        d_def.d_value = rhs.d_def.d_value;
        d_def.d_allocator_p = a;
    }

    ~MyClass3()
    {
        ASSERT(d_def.d_value != 93);
        d_def.d_value = 93;
        d_def.d_allocator_p = 0;
        dumpClassDefState(d_def);
    }

    // MANIPULATORS
    MyClass3& operator=(const MyClass3& rhs)
    {
        d_def.d_value = rhs.d_def.d_value;
        // Do not touch allocator!
        return *this;
    }

    // ACCESSORS
    int value() const { return d_def.d_value; }
    bslma::Allocator *allocator() const { return d_def.d_allocator_p; }
};
// CLASS DATA
int MyClass3::copyConstructorInvocations       = 0;

// TRAITS
namespace BloombergLP {
namespace bslma {

template <>
struct UsesBslmaAllocator<MyClass3> : bsl::true_type {};

}  // close package namespace
}  // close enterprise namespace

                             // ===================
                             // class MyClassFussy
                             // ===================

class MyClassFussy {
    // Class that doesn't take allocators.

    // DATA
    MyClassDef d_def;

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
    MyClassFussy()
    {
        // Should never be invoked by 'ConstructionUtil'.

        BSLS_ASSERT_OPT(0);
    }

    // deliberately not explicit
    MyClassFussy(int v)
    {
        ++s_conversionConstructorInvocations;

        d_def.d_value       = v;
        d_def.d_allocator_p = 0;
    }

    MyClassFussy(const MyClassFussy& /* rhs */)
    {
        // Should never be invoked by 'ConstructionUtil'.

        BSLS_ASSERT_OPT(0);
    }

    MyClassFussy(bslmf::MovableRef<MyClassFussy> /* rhs */)
    {
        // Should never be invoked by 'ConstructionUtil'.

        BSLS_ASSERT_OPT(0);
    }

    ~MyClassFussy()
    {
        // Should never be invoked by 'ConstructionUtil'.
        ++s_destructorInvocations;

        // 'dumpClassDefState' is not called here because a default-constructed
        // 'MyClassFussy' object may leave 'd_def' uninitialized.
    }

    // MANIPULATORS
    MyClassFussy& operator=(const MyClassFussy& /* rhs */)
    {
        // Should never be invoked by 'ConstructionUtil'.

        BSLS_ASSERT_OPT(0);

        return *this;
    }
};

// CLASS DATA
int MyClassFussy::s_conversionConstructorInvocations = 0;
int MyClassFussy::s_destructorInvocations            = 0;

// TRAITS
namespace bsl {

template <>
struct is_trivially_copyable<MyClassFussy> : true_type {};

template <>
struct is_trivially_default_constructible<MyClassFussy> : true_type {};

}  // close namespace bsl

                                 // =========
                                 // MyClass4
                                 // =========

class MyClass4 {
    // Class that takes allocators, and that actually allocates (for use in
    // exception testing).

    // DATA
    MyClassDef d_def;

  public:
    // CREATORS
    MyClass4(bslma::Allocator *a = 0)
    {
        d_def.d_allocator_p = bslma::Default::allocator(a);
        d_def.d_data_p = (int*)(d_def.d_allocator_p)->allocate(sizeof(int));
        d_def.d_value = 0;
        *d_def.d_data_p = d_def.d_value;
    }

    MyClass4(int v, bslma::Allocator *a = 0)
    {
        d_def.d_allocator_p = bslma::Default::allocator(a);
        d_def.d_data_p = (int*)(d_def.d_allocator_p)->allocate(sizeof(int));
        d_def.d_value = v;
        *d_def.d_data_p = d_def.d_value;
    }

    MyClass4(const MyClass4& rhs, bslma::Allocator *a = 0)
    {
        d_def.d_allocator_p = bslma::Default::allocator(a);
        d_def.d_data_p = (int*)(d_def.d_allocator_p)->allocate(sizeof(int));
        d_def.d_value = rhs.d_def.d_value;
        *d_def.d_data_p = d_def.d_value;
    }

    MyClass4(bslmf::MovableRef<MyClass4> rhs, bslma::Allocator *a = 0)
    {
        MyClass4& lvalue = rhs;
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

    ~MyClass4()
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
    MyClass4& operator=(const MyClass4& rhs)
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
struct UsesBslmaAllocator<MyClass4> : bsl::true_type {};

}  // close package namespace
}  // close enterprise namespace

                             // ====================
                             // class MyClassFussy2
                             // ====================

class MyClassFussy2 {
    // This class does not take allocators, is bitwise movable, and does not
    // provide a move constructor.

    // DATA
    MyClassDef d_def;

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
    MyClassFussy2()
    {
        // Should never be invoked by 'ConstructionUtil'.

        BSLS_ASSERT_OPT(0);
    }

    // deliberately not explicit
    MyClassFussy2(int v)
    {
        ++s_conversionConstructorInvocations;

        d_def.d_value       = v;
        d_def.d_allocator_p = 0;
    }

    MyClassFussy2(const MyClassFussy2& rhs)
    {
        ++s_copyConstructorInvocations;

        d_def.d_value       = rhs.d_def.d_value;
        d_def.d_allocator_p = rhs.d_def.d_allocator_p;
    }

    ~MyClassFussy2()
    {
        // Should never be invoked by 'ConstructionUtil'.
        ++s_destructorInvocations;

        // 'dumpClassDefState' is not called here because a default-constructed
        // 'MyClassFussy2' object may leave 'd_def' uninitialized.
    }

    // MANIPULATORS
    MyClassFussy2& operator=(const MyClassFussy2& /* rhs */)
    {
        // Should never be invoked by 'ConstructionUtil'.

        BSLS_ASSERT_OPT(0);

        return *this;
    }
};

// CLASS DATA
int MyClassFussy2::s_copyConstructorInvocations       = 0;
int MyClassFussy2::s_conversionConstructorInvocations = 0;
int MyClassFussy2::s_destructorInvocations            = 0;

// TRAITS
namespace bsl {

template <>
struct is_trivially_default_constructible<MyClassFussy2> : true_type {};

}  // close namespace bsl

namespace BloombergLP {
namespace bslmf {

template <>
struct IsBitwiseMoveable<MyClassFussy2> : bsl::true_type {};

}  // close namespace bslmf
}  // close enterprise namespace

                             // =============
                             // class MyPair
                             // =============

template <class T1, class T2>
struct MyPair {
    // Test pair type without allocators.

    // TYPES
    typedef T1 first_type;
    typedef T2 second_type;

    // DATA (public for pair types)
    T1 first;
    T2 second;

    // CREATORS
    MyPair()
    {
    }

    MyPair(const T1& f, const T2& s)
    : first(f)
    , second(s)
    {
    }

    MyPair(const MyPair& other)
    : first(other.first)
    , second(other.second)
    {
    }

    template <class U1, class U2>
    MyPair(const MyPair<U1, U2>& other)
    : first(other.first)
    , second(other.second)
    {
    }

    MyPair(bslmf::MovableRef<MyPair> other)
    : first(bslmf::MovableRefUtil::move(other.first))
    , second(bslmf::MovableRefUtil::move(other.second))
    {
    }

    template <class U1, class U2>
    MyPair(bslmf::MovableRef<MyPair<U1, U2> > other)
    : first(bslmf::MovableRefUtil::move(
                                   bslmf::MovableRefUtil::access(other).first))
    , second(bslmf::MovableRefUtil::move(
                                  bslmf::MovableRefUtil::access(other).second))
    {
    }

    MyPair& operator=(const MyPair& rhs)
    {
        first = rhs.first;
        second = rhs.second;
        return *this;
    }
};

                               // ==============
                               // class MyPairA
                               // ==============

template <class T1, class T2>
struct MyPairA {
    // Test pair type with mixed allocator and non-allocator.  Only 'T2' must
    // use allocators.  We assume that the treatment of 'T1' and 'T2' in the
    // component is symmetric, and do not bother with the symmetric test pair
    // type.

    // TYPES
    typedef T1 first_type;
    typedef T2 second_type;

    // DATA (public for pair types)
    T1 first;
    T2 second;

    // CREATORS
    explicit
    MyPairA(bslma::Allocator *a = 0)
    : first()
    , second(a)
    {
    }

    MyPairA(const T1& f, const T2& s, bslma::Allocator *a = 0)
    : first(f)
    , second(s, a)
    {
    }

    MyPairA(const MyPairA& other, bslma::Allocator *a = 0)
    : first(other.first)
    , second(other.second, a)
    {
    }

    template <class U1, class U2>
    MyPairA(const MyPairA<U1, U2>& other, bslma::Allocator *a = 0)
    : first(other.first)
    , second(other.second, a)
    {
    }

    MyPairA(bslmf::MovableRef<MyPairA> other, bslma::Allocator *a = 0)
    : first(bslmf::MovableRefUtil::move(other.first))
    , second(bslmf::MovableRefUtil::move(other.second), a)
    {
    }

    template <class U1, class U2>
    MyPairA(bslmf::MovableRef<MyPairA<U1, U2> >  other,
            bslma::Allocator                    *a = 0)
    : first(bslmf::MovableRefUtil::move(
                                   bslmf::MovableRefUtil::access(other).first))
    , second(bslmf::MovableRefUtil::move(
                                  bslmf::MovableRefUtil::access(other).second),
                                  a)
    {
    }

    MyPairA& operator=(const MyPairA& rhs)
    {
        first = rhs.first;
        second = rhs.second;
        return *this;
    }
};

namespace BloombergLP {
namespace bslma {

template <class T1, class T2>
struct UsesBslmaAllocator<MyPairA<T1, T2> > : bsl::true_type {};

}  // close package namespace
}  // close enterprise namespace

                              // ===============
                              // class MyPairAA
                              // ===============

template <class T1, class T2>
struct MyPairAA {
    // Test pair type with allocators.  Both 'T1' and 'T2' must use allocators.

    // TYPES
    typedef T1 first_type;
    typedef T2 second_type;

    // DATA (public for pair types)
    T1 first;
    T2 second;

    // CREATORS
    explicit
    MyPairAA(bslma::Allocator *a = 0)
    : first(a)
    , second(a)
    {
    }

    MyPairAA(const T1& f, const T2& s, bslma::Allocator *a = 0)
    : first(f, a)
    , second(s, a)
    {
    }

    MyPairAA(const MyPairAA& other, bslma::Allocator *a = 0)
    : first(other.first, a)
    , second(other.second, a)
    {
    }

    template <class U1, class U2>
    MyPairAA(const MyPairAA<U1, U2>& other, bslma::Allocator *a = 0)
    : first(other.first, a)
    , second(other.second, a)
    {
    }

    MyPairAA(bslmf::MovableRef<MyPairAA> other, bslma::Allocator *a = 0)
    : first(bslmf::MovableRefUtil::move(
                bslmf::MovableRefUtil::access(other).first),
            a)
    , second(bslmf::MovableRefUtil::move(
                 bslmf::MovableRefUtil::access(other).second),
             a)
    {
    }

    template <class U1, class U2>
    MyPairAA(bslmf::MovableRef<MyPairAA<U1, U2> >  other,
             bslma::Allocator                     *a = 0)
    : first(bslmf::MovableRefUtil::move(
                bslmf::MovableRefUtil::access(other).first),
            a)
    , second(bslmf::MovableRefUtil::move(
                 bslmf::MovableRefUtil::access(other).second),
             a)
    {
    }

    MyPairAA& operator=(const MyPairAA& rhs)
    {
        first = rhs.first;
        second = rhs.second;
        return *this;
    }
};

namespace BloombergLP {
namespace bslma {

template <class T1, class T2>
struct  UsesBslmaAllocator<MyPairAA<T1, T2> > : bsl::true_type  {};

}  // close package namespace
}  // close enterprise namespace

                              // ===============
                              // class MyPairBB
                              // ===============

template <class T1, class T2>
struct MyPairBB {
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
    MyPairBB()
    {
    }

    MyPairBB(const T1& f, const T2& s)
    : first(f)
    , second(s)
    {
    }

    MyPairBB(const MyPairBB& other)
    : first(other.first)
    , second(other.second)
    {
    }

    template <class U1, class U2>
    MyPairBB(const MyPairBB<U1, U2>& other)
    : first(other.first)
    , second(other.second)
    {
    }

    MyPairBB(bslmf::MovableRef<MyPairBB> other)
    : first(bslmf::MovableRefUtil::move(
                                   bslmf::MovableRefUtil::access(other).first))
    , second(bslmf::MovableRefUtil::move(
                                  bslmf::MovableRefUtil::access(other).second))
    {
    }

    template <class U1, class U2>
    MyPairBB(bslmf::MovableRef<MyPairBB<U1, U2> > other)
    : first(bslmf::MovableRefUtil::move(
                                   bslmf::MovableRefUtil::access(other).first))
    , second(bslmf::MovableRefUtil::move(
                                  bslmf::MovableRefUtil::access(other).second))
    {
    }

    MyPairBB& operator=(const MyPairBB& rhs)
    {
        first  = rhs.first;
        second = rhs.second;
        return *this;
    }
};

namespace BloombergLP {
namespace bslmf {

template <class T1, class T2>
struct IsPair<MyPairBB<T1, T2> > : bsl::true_type {};

}  // close namespace bslmf
}  // close enterprise namespace

                              // ===============
                              // macros TEST_OP*
                              // ===============

#define TEST_OP(typeNum, op, expVal, expAlloc) {                              \
    typedef MyClass ## typeNum Type;                                          \
    static const int EXP_VAL = (expVal);                                      \
    bslma::Allocator *const EXP_ALLOC = (expAlloc);                           \
    MyClassDef rawBuf;                                                        \
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
    // 'MyClassN' (where 'N' stands for the specified 'typeNum'), and verifies
    // that the 'd_value' and 'd_allocator_p' members store the specified
    // 'expVal' and 'expAlloc' values after 'op' has been evaluated.

#define TEST_PAIR(op, expVal0, expA0, expVal1, expA1) {                       \
    static const int EXP_VAL0 = (expVal0);                                    \
    bslma::Allocator *const EXP_ALLOC0 = (expA0);                             \
    static const int EXP_VAL1 = (expVal1);                                    \
    bslma::Allocator *const EXP_ALLOC1 = (expA1);                             \
    MyClassDef rawBuf[2];                                                     \
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
    typedef MyPair_ ## typeNum0 ## _ ## typeNum1 Type;                        \
    TEST_PAIR(op, expVal0, expA0, expVal1, expA1);                            \
  }
    // This macro evaluates the specified 'op' expression in the namespace
    // under test, involving the address 'objPtr' of an object of type
    // 'MyPair_N0_N1' (where 'N0' and 'N1' stand for the specified 'typeNum0
    // and 'typeNum1').  See the 'TEST_PAIR' macro above for details.

#define TEST_PAIRAOP(typeNum0, typeNum1, op, expVal0, expA0, expVal1, expA1)  \
  {                                                                           \
    typedef MyPairA_ ## typeNum0 ## _ ## typeNum1 Type;                       \
    TEST_PAIR(op, expVal0, expA0, expVal1, expA1);                            \
  }
    // This macro evaluates the specified 'op' expression in the namespace
    // under test, involving the address 'objPtr' of an object of type
    // 'MyPair_N0_N1' (where 'N0' and 'N1' stand for the specified 'typeNum0
    // and 'typeNum1').  See the 'TEST_PAIR' macro above for details.

#define TEST_PAIRAAOP(typeNum0, typeNum1, op, expVal0, expA0, expVal1, expA1) \
  {                                                                           \
    typedef MyPairAA_ ## typeNum0 ## _ ## typeNum1 Type;                      \
    TEST_PAIR(op, expVal0, expA0, expVal1, expA1);                            \
  }
    // This macro evaluates the specified 'op' expression in the namespace
    // under test, involving the address 'objPtr' of an object of type
    // 'MyPairAA_N0_N1' (where 'N0' and 'N1' stand for the specified 'typeNum0
    // and 'typeNum1').  See the 'TEST_PAIR' macro above for details.

#define TEST_PAIRBBOP(typeNum0, typeNum1, op, expVal0, expA0, expVal1, expA1) \
  {                                                                           \
    typedef MyPairBB_ ## typeNum0 ## _ ## typeNum1 Type;                      \
    TEST_PAIR(op, expVal0, expA0, expVal1, expA1);                            \
  }
    // This macro evaluates the specified 'op' expression in the namespace
    // under test, involving the address 'objPtr' of an object of type
    // 'MyPairBB_N0_N1' (where 'N0' and 'N1' stand for the specified 'typeNum0
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
    MyClass ## typeNum fromObj(expVal);                                       \
    TEST_OP(typeNum, op, expVal, expAlloc);                                   \
    ASSERT_IS_MOVED_FROM(fromObj);                                            \
  }
    // This macro evaluates the specified 'op' expression in the namespace
    // under test, involving the address 'objPtr' of an object of type
    // 'MyClassN' (where 'N' stands for the specified 'typeNum') and the
    // modifiable object 'fromObj' of type 'MyClassN' initialized to
    // 'expVal', and verifies that the 'd_value' and 'd_allocator_p' members
    // store the specified 'expVal' and 'expAlloc' values and 'fromObj' is in
    // a moved-from state after 'op' has been evaluated.

#define TEST_PAIRMV(typeNum0, typeNum1, op, expVal0, expA0, expVal1, expA1) { \
    typedef MyPairA_ ## typeNum0 ## _ ## typeNum1 Type;                       \
    Type fromObj(expVal0, expVal1);                                           \
    TEST_PAIR(op, expVal0, expA0, expVal1, expA1);                            \
    ASSERT_IS_MOVED_FROM(fromObj.first);                                      \
  }
    // This macro evaluates the specified 'op' expression in the namespace
    // under test, involving the address 'objPtr' of an object of type
    // 'MyPair_N0_N1' (where 'N0' and 'N1' stand for the specified 'typeNum0
    // and 'typeNum1') and a modifiable object 'fromObj' of type
    // 'MyPair_N0_N1' initialized to '{ expVal0, expVal1 }, and verifies that
    // 'fromObj.first' and 'fromObj.second' are in a moved-from state after
    // 'op' has been evaluated.  See the 'TEST_PAIR' macro above for details.

                         // ==========================
                         // debug breakpoints pre/post
                         // ==========================

void pre(const MyClassDef *p)
    // Do nothing.  This function can be taken advantage of to debug the above
    // macros by setting a breakpoint to examine state prior to executing the
    // main operation under test.
{
    (void) p;  // remove unused variable warning
}

void post(const MyClassDef *p)
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
class ConstructTestArgNoAlloc : public MyClassDef {
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
class ConstructTestArgAlloc : public MyClassDef {
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

}  // close package namespace
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
    // This class provides a test class capable of holding up to 14 parameters
    // of types 'ConstructTestArgAlloc[1--14]'.  By default, a
    // 'ConstructTestTypeAlloc' is constructed with nil ('N1') values, but
    // instances can be constructed with actual values (e.g., for creating
    // expected values).  This class intentionally *does* take an allocator.

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

}  // close package namespace
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
    memset((void *)&mX, 92, sizeof mX);                                       \
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
    memset((void *)&mX, 92, sizeof mX);                                       \
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
typedef MyPair<MyClass1, MyClass1> MyPair_1_1;
typedef MyPair<MyClass1, MyClass2> MyPair_1_2;
typedef MyPair<MyClass2, MyClass1> MyPair_2_1;
typedef MyPair<MyClass2, MyClass2> MyPair_2_2;
typedef MyPair<MyClass4, MyClass4> MyPair_4_4;

typedef MyPairA<MyClass1, MyClass2> MyPairA_1_2;
typedef MyPairA<MyClass2, MyClass2> MyPairA_2_2;

typedef MyPairAA<MyClass2, MyClass2> MyPairAA_2_2;

typedef MyPairAA<MyClass4, MyClass4> MyPairAA_4_4;

typedef MyPairBB<MyClass4, MyClass4> MyPairBB_4_4;

const MyClass1      V1(1);
const MyClass2      V2(2);
const MyClass2a     V2A(0x2a);
const MyClass3      V3(3);
const MyClass4      V4(4);
const MyClassFussy  VF(5);
const MyClassFussy2 VF2(6);

const MyPair_1_1 PV1V1(V1, V1);
const MyPair_1_2 PV1V2(V1, V2);
const MyPair_2_1 PV2V1(V2, V1);
const MyPair_2_2 PV2V2(V2, V2);
const MyPair_4_4 PV4V4(V4, V4);

const MyPairA_1_2 PAV1V2(V1, V2);
const MyPairA_2_2 PAV2V2(V2, V2);

const MyPairAA_2_2 PAAV2V2(V2, V2);

const MyPairAA_4_4 PAAV4V4(V4, V4);

const MyPairBB_4_4 PBBV4V4(V4, V4);

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

                             // ====================
                             // Base Type Qualifiers
                             // ====================

// The following empty classes are used as qualifiers for the base type,
// allowing to modify behavior of the 'bslma::ConstructionUtil::construct'
// functions via different trait values for different base class
// specifications.

class TRIVIAL     {};  // trivially copyable type
class NON_TRIVIAL {};  // non-trivially copyable type
class BSLMA_ALLOC {};  // type using bslma allocator
class ARG_T_ALLOC {};  // tape using special argument with allocator

                             // ==================
                             // class BaseTestType
                             // ==================
template <class T>
class BaseTestType {
    // This class provides a mechanism (static variables) to establish exactly
    // which copy or move constructor is called.

  public:
    // CLASS DATA
    static int s_copyConstructorInvocations;
    static int s_moveConstructorInvocations;
    static int s_copyAllocConstructorInvocations;
    static int s_moveAllocConstructorInvocations;
    static int s_copyArgTConstructorInvocations;
    static int s_moveArgTConstructorInvocations;

    // DATA
    MyClassDef d_def;  // object's value

    // CREATORS
    BaseTestType()
        // Create an object having the null value.
    {
        d_def.d_value = 0;
        d_def.d_allocator_p = 0;
    }

    BaseTestType(int value, bslma::Allocator *alloc = 0)
        // Create an object that has the specified 'value' and that uses the
        // specified 'alloc' to supply memory.
    {
        d_def.d_value = value;
        d_def.d_allocator_p = alloc;
    }

    BaseTestType(const BaseTestType& original)
        // Create an object having the value of the specified 'original'
        // object.
    {
        ++s_copyConstructorInvocations;
        d_def.d_value = original.d_def.d_value;
        d_def.d_allocator_p = 0;
    }

    BaseTestType(const BaseTestType& original, bslma::Allocator *alloc)
        // Create an object that has the value of the specified 'original'
        // object and that uses the specified 'alloc' to supply memory.
    {
        ++s_copyAllocConstructorInvocations;
        d_def.d_value = original.d_def.d_value;
        d_def.d_allocator_p = alloc;
    }

    BaseTestType(const bsl::allocator_arg_t&,
                 bslma::Allocator            *alloc,
                 const BaseTestType&          original)
        // Following the 'allocator_arg_t' construction protocol create an
        // object having the same value as the specified 'original' object that
        // uses the specified 'alloc' to supply memory.
    {
        ++s_copyArgTConstructorInvocations;
        d_def.d_value = original.d_def.d_value;
        d_def.d_allocator_p = alloc;
    }


    BaseTestType(bslmf::MovableRef<BaseTestType> original)
        // Create an object having the same value as the specified
        // 'original' object by moving the contents of 'original' to the
        // newly-created object.
    {
        ++s_moveConstructorInvocations;
        BaseTestType& lvalue = original;
        d_def.d_value = lvalue.d_def.d_value;
        d_def.d_allocator_p = 0;
    }

    BaseTestType(bslmf::MovableRef<BaseTestType>  original,
                 bslma::Allocator                *alloc)
        // Create an object having the same value as the specified 'original'
        // object that uses the specified 'alloc' to supply memory by moving
        // the contents of 'original' to the newly-created object.
     {
        ++s_moveAllocConstructorInvocations;
        BaseTestType& lvalue = original;
        d_def.d_value = lvalue.d_def.d_value;
        d_def.d_allocator_p = alloc;
    }

    BaseTestType(const bsl::allocator_arg_t&,
                 bslma::Allocator                *alloc,
                 bslmf::MovableRef<BaseTestType> original)
        // Following the 'allocator_arg_t' construction protocol create an
        // object having the same value as the specified 'original' object that
        // uses the specified 'alloc' to supply memory by moving the contents
        // of 'original' to the newly-created object.
    {
        ++s_moveArgTConstructorInvocations;
        BaseTestType& lvalue = original;
        d_def.d_value = lvalue.d_def.d_value;
        d_def.d_allocator_p = alloc;
    }

    // ACCESSORS
    int value() const
        // Return the value of this object.
    {
        return d_def.d_value;
    }

    bslma::Allocator *allocator() const
        // Return the allocator used by this object to supply memory.
    {
        return d_def.d_allocator_p;
    }

};

// CLASS DATA
template <class T>
int BaseTestType<T>::s_copyConstructorInvocations = 0;
template <class T>
int BaseTestType<T>::s_moveConstructorInvocations = 0;
template <class T>
int BaseTestType<T>::s_copyAllocConstructorInvocations = 0;
template <class T>
int BaseTestType<T>::s_moveAllocConstructorInvocations = 0;
template <class T>
int BaseTestType<T>::s_copyArgTConstructorInvocations = 0;
template <class T>
int BaseTestType<T>::s_moveArgTConstructorInvocations = 0;

                          // =====================
                          // class TrivialTestType
                          // =====================

typedef BaseTestType<TRIVIAL>     TrivialTestType;

                         // ========================
                         // class NonTrivialTestType
                         // ========================

typedef BaseTestType<NON_TRIVIAL> NonTrivialTestType;

                         // ========================
                         // class BslmaAllocTestType
                         // ========================

typedef BaseTestType<BSLMA_ALLOC> BslmaAllocTestType;

                         // =======================
                         // class ArgTAllocTestType
                         // =======================

typedef BaseTestType<ARG_T_ALLOC> ArgTAllocTestType;

// TRAITS

namespace bsl {
template <>
struct is_trivially_copyable< TrivialTestType    > : true_type {};

template <>
struct is_trivially_copyable< NonTrivialTestType > : false_type {};

template <>
struct is_trivially_copyable< BslmaAllocTestType > : false_type {};

template <>
struct is_trivially_copyable< ArgTAllocTestType  > : false_type {};

}  // close namespace bsl

namespace BloombergLP {
namespace bslma {

template <>
struct UsesBslmaAllocator<BslmaAllocTestType> : bsl::true_type {};

template <>
struct UsesBslmaAllocator<ArgTAllocTestType> : bsl::true_type {};

}  // close namespace bslma

namespace bslmf {

template <>
struct UsesAllocatorArgT<ArgTAllocTestType> : bsl::true_type {};

}  // close namespace bslmf

}  // close enterprise namespace

                             // ==========================
                             // class MoveOnlyBaseTestType
                             // ==========================
template <class T>
class MoveOnlyBaseTestType {
    // This class, having deprecated copy constructors, provides a mechanism
    // (static variables) to establish exactly which copy or move constructor
    // is called.

  private:
    // NOT IMPLEMENTED
    MoveOnlyBaseTestType(const MoveOnlyBaseTestType&);             // = delete;
    MoveOnlyBaseTestType(const MoveOnlyBaseTestType&, bslma::Allocator *);
                                                                   // = delete;
    MoveOnlyBaseTestType(const bsl::allocator_arg_t&,
                         bslma::Allocator            *,
                         const MoveOnlyBaseTestType&);             // = delete;

  public:
    // CLASS DATA
    static int s_copyConstructorInvocations;
    static int s_moveConstructorInvocations;
    static int s_copyAllocConstructorInvocations;
    static int s_moveAllocConstructorInvocations;
    static int s_copyArgTConstructorInvocations;
    static int s_moveArgTConstructorInvocations;

    // DATA
    MyClassDef d_def;  // object's value

    // CREATORS
    MoveOnlyBaseTestType()
        // Create an object having the null value.
    {
        d_def.d_value = 0;
        d_def.d_allocator_p = 0;
    }

    MoveOnlyBaseTestType(int value, bslma::Allocator *alloc = 0)
        // Create an object that has the specified 'value' and that uses the
        // specified 'alloc' to supply memory.
    {
        d_def.d_value = value;
        d_def.d_allocator_p = alloc;
    }

    MoveOnlyBaseTestType(bslmf::MovableRef<MoveOnlyBaseTestType> original)
        // Create an object having the same value as the specified 'original'
        // object by moving the contents of 'original' to the newly-created
        // object.
    {
        ++s_moveConstructorInvocations;
        MoveOnlyBaseTestType& lvalue = original;
        d_def.d_value = lvalue.d_def.d_value;
        d_def.d_allocator_p = 0;
    }

    MoveOnlyBaseTestType(bslmf::MovableRef<MoveOnlyBaseTestType>  original,
                         bslma::Allocator                        *alloc)
        // Create an object having the same value as the specified 'original'
        // object that uses the specified 'alloc' to supply memory by moving
        // the contents of 'original' to the newly-created object.
    {
        ++s_moveAllocConstructorInvocations;
        MoveOnlyBaseTestType& lvalue = original;
        d_def.d_value = lvalue.d_def.d_value;
        d_def.d_allocator_p = alloc;
    }

    MoveOnlyBaseTestType(const bsl::allocator_arg_t&,
                         bslma::Allocator                        *alloc,
                         bslmf::MovableRef<MoveOnlyBaseTestType>  original)
        // Following the 'allocator_arg_t' construction protocol create an
        // object having the same value as the specified 'original' object that
        // uses the specified 'alloc' to supply memory by moving the contents
        // of 'original' to the newly-created object.
    {
        ++s_moveArgTConstructorInvocations;
        MoveOnlyBaseTestType& lvalue = original;
        d_def.d_value = lvalue.d_def.d_value;
        d_def.d_allocator_p = alloc;
    }

    // ACCESSORS
    int value() const
        // Return the value of this object.
    {
        return d_def.d_value;
    }

    bslma::Allocator *allocator() const
        // Return the allocator used by this object to supply memory.
    {
        return d_def.d_allocator_p;
    }
};

// CLASS DATA
template <class T>
int MoveOnlyBaseTestType<T>::s_copyConstructorInvocations = 0;
template <class T>
int MoveOnlyBaseTestType<T>::s_moveConstructorInvocations = 0;
template <class T>
int MoveOnlyBaseTestType<T>::s_copyAllocConstructorInvocations = 0;
template <class T>
int MoveOnlyBaseTestType<T>::s_moveAllocConstructorInvocations = 0;
template <class T>
int MoveOnlyBaseTestType<T>::s_copyArgTConstructorInvocations = 0;
template <class T>
int MoveOnlyBaseTestType<T>::s_moveArgTConstructorInvocations = 0;

                          // =============================
                          // class MoveOnlyTrivialTestType
                          // =============================

typedef MoveOnlyBaseTestType<TRIVIAL> MoveOnlyTrivialTestType;

                         // ================================
                         // class MoveOnlyNonTrivialTestType
                         // ================================

typedef MoveOnlyBaseTestType<NON_TRIVIAL> MoveOnlyNonTrivialTestType;

                         // ================================
                         // class MoveOnlyBslmaAllocTestType
                         // ================================

typedef MoveOnlyBaseTestType<BSLMA_ALLOC> MoveOnlyBslmaAllocTestType;

                         // ===============================
                         // class MoveOnlyArgTAllocTestType
                         // ===============================

typedef MoveOnlyBaseTestType<ARG_T_ALLOC> MoveOnlyArgTAllocTestType;


// TRAITS
namespace bsl {

template <>
struct is_trivially_copyable<MoveOnlyTrivialTestType> : true_type {};

template <>
struct is_trivially_copyable<MoveOnlyNonTrivialTestType> : false_type {};

template <>
struct is_trivially_copyable<MoveOnlyBslmaAllocTestType> : false_type {};

template <>
struct is_trivially_copyable<MoveOnlyArgTAllocTestType> : false_type {};

}  // close namespace bsl

namespace BloombergLP {
namespace bslma {

template <>
struct UsesBslmaAllocator<MoveOnlyBslmaAllocTestType> : bsl::true_type {};

template <>
struct UsesBslmaAllocator<MoveOnlyArgTAllocTestType> : bsl::true_type {};

}  // close namespace bslma

namespace bslmf {

template <>
struct UsesAllocatorArgT<MoveOnlyArgTAllocTestType> : bsl::true_type {};

}  // close namespace bslmf

}  // close enterprise namespace

                             // ==========================
                             // class CopyOnlyBaseTestType
                             // ==========================

template <class T>
class CopyOnlyBaseTestType {
    // This class, that does not support move constructors, provides a
    // mechanism (static variables) to establish exactly which copy or move
    // constructor is called.

  public:
    // CLASS DATA
    static int s_copyConstructorInvocations;
    static int s_moveConstructorInvocations;
    static int s_copyAllocConstructorInvocations;
    static int s_moveAllocConstructorInvocations;
    static int s_copyArgTConstructorInvocations;
    static int s_moveArgTConstructorInvocations;

    // DATA
    MyClassDef d_def;  // object's value

    // CREATORS
    CopyOnlyBaseTestType()
        // Create an object having the null value.
    {
        d_def.d_value = 0;
        d_def.d_allocator_p = 0;
    }

    CopyOnlyBaseTestType(int value, bslma::Allocator *alloc = 0)
        // Create an object that has the specified 'value' and that uses the
        // specified 'alloc' to supply memory.
    {
        d_def.d_value = value;
        d_def.d_allocator_p = alloc;
    }

    CopyOnlyBaseTestType(const CopyOnlyBaseTestType& original)
        // Create an object having the value of the specified 'original'
        // object.
    {
        ++s_copyConstructorInvocations;
        d_def.d_value = original.d_def.d_value;
        d_def.d_allocator_p = 0;
    }

    CopyOnlyBaseTestType(const CopyOnlyBaseTestType&  original,
                         bslma::Allocator            *alloc)
        // Create an object that has the value of the specified 'original'
        // object and that uses the specified 'alloc' to supply memory.
    {
        ++s_copyAllocConstructorInvocations;
        d_def.d_value = original.d_def.d_value;
        d_def.d_allocator_p = alloc;
    }

    CopyOnlyBaseTestType(const bsl::allocator_arg_t&,
                         bslma::Allocator            *alloc,
                         const CopyOnlyBaseTestType&  original)
        // Following the 'allocator_arg_t' construction protocol create an
        // object having the same value as the specified 'original' object that
        // uses the specified 'alloc' to supply memory.
    {
        ++s_copyArgTConstructorInvocations;
        d_def.d_value = original.d_def.d_value;
        d_def.d_allocator_p = alloc;
    }

    // ACCESSORS
    int value() const
        // Return the value of this object.
    {
        return d_def.d_value;
    }

    bslma::Allocator *allocator() const
        // Return the allocator used by this object to supply memory.
    {
        return d_def.d_allocator_p;
    }
};

// CLASS DATA
template <class T>
int CopyOnlyBaseTestType<T>::s_copyConstructorInvocations = 0;
template <class T>
int CopyOnlyBaseTestType<T>::s_moveConstructorInvocations = 0;
template <class T>
int CopyOnlyBaseTestType<T>::s_copyAllocConstructorInvocations = 0;
template <class T>
int CopyOnlyBaseTestType<T>::s_moveAllocConstructorInvocations = 0;
template <class T>
int CopyOnlyBaseTestType<T>::s_copyArgTConstructorInvocations = 0;
template <class T>
int CopyOnlyBaseTestType<T>::s_moveArgTConstructorInvocations = 0;

                          // =============================
                          // class CopyOnlyTrivialTestType
                          // =============================

typedef CopyOnlyBaseTestType<TRIVIAL>     CopyOnlyTrivialTestType;

                         // ================================
                         // class CopyOnlyNonTrivialTestType
                         // ================================

typedef CopyOnlyBaseTestType<NON_TRIVIAL> CopyOnlyNonTrivialTestType;

                         // ================================
                         // class CopyOnlyBslmaAllocTestType
                         // ================================

typedef CopyOnlyBaseTestType<BSLMA_ALLOC> CopyOnlyBslmaAllocTestType;

                         // ===============================
                         // class CopyOnlyArgTAllocTestType
                         // ===============================

typedef CopyOnlyBaseTestType<ARG_T_ALLOC> CopyOnlyArgTAllocTestType;


// TRAITS
namespace bsl {
template <>
struct is_trivially_copyable<CopyOnlyTrivialTestType> : true_type {};

template <>
struct is_trivially_copyable<CopyOnlyNonTrivialTestType> : false_type {};

template <>
struct is_trivially_copyable<CopyOnlyBslmaAllocTestType> : false_type {};

template <>
struct is_trivially_copyable<CopyOnlyArgTAllocTestType> : false_type {};

}  // close namespace bsl

namespace BloombergLP {
namespace bslma {

template <>
struct UsesBslmaAllocator<CopyOnlyBslmaAllocTestType> : bsl::true_type {};

template <>
struct UsesBslmaAllocator<CopyOnlyArgTAllocTestType> : bsl::true_type {};

}  // close namespace bslma

namespace bslmf {

template <>
struct UsesAllocatorArgT<CopyOnlyArgTAllocTestType> : bsl::true_type {};

}  // close namespace bslmf

}  // close enterprise namespace

//=============================================================================
//                            TEST DRIVER TEMPLATE
//-----------------------------------------------------------------------------

                            // ================
                            // class TestDriver
                            // ================

template <class TYPE>
struct TestDriver
    // This template class provides a namespace for testing
    // 'bslma::ConstructionUtil'.  The template parameter 'TYPE' specifies the
    // type to construct.
{

  public:
    // TYPES
    enum ExpectedAllocator {
        DEFAULT  = 0,
        OBJECT   = 1,
        SUPPLIED = 2
    };

    // CLASS METHODS
    static void testCase5Imp(int               expCopyInvocationsNum,
                             int               expMoveInvocationsNum,
                             int               expCopyAllocInvocationsNum,
                             int               expMoveAllocInvocationsNum,
                             int               expCopyArgTInvocationsNum,
                             int               expMoveArgTInvocationsNum,
                             ExpectedAllocator expAlloc,
                             bool              useAllocator
                             );
        // Implementation of the 'testCase5Alloc' and the 'testCase5NoAlloc'.

    static void testCase5NoAlloc(int               expCopyInvocationsNum,
                                 int               expMoveInvocationsNum,
                                 int               expCopyAllocInvocationsNum,
                                 int               expMoveAllocInvocationsNum,
                                 int               expCopyArgTInvocationsNum,
                                 int               expMoveArgTInvocationsNum,
                                 ExpectedAllocator expAlloc);
        // Test 'construct(TYPE *addr, void *a, MovableRef<TYPE> orig)';

    static void testCase5Alloc(int               expCopyInvocationsNum,
                               int               expMoveInvocationsNum,
                               int               expCopyAllocInvocationsNum,
                               int               expMoveAllocInvocationsNum,
                               int               expCopyArgTInvocationsNum,
                               int               expMoveArgTInvocationsNum,
                               ExpectedAllocator expAlloc);
        // Test 'construct(TYPE *addr, Allocator *a, MovableRef<TYPE> orig)';
};
                           //-----------
                           // TEST CASES
                           //-----------

template<class TYPE>
void TestDriver<TYPE>::testCase5Imp(
                                  int               expCopyInvocationsNum,
                                  int               expMoveInvocationsNum,
                                  int               expCopyAllocInvocationsNum,
                                  int               expMoveAllocInvocationsNum,
                                  int               expCopyArgTInvocationsNum,
                                  int               expMoveArgTInvocationsNum,
                                  ExpectedAllocator expAlloc,
                                  bool              useAllocator)
{
    if (veryVerbose) {
        T_ T_ P(bsls::NameOf<TYPE>());
    }

    bslma::TestAllocator  oa;
    TYPE                  fromObj(1, &oa);
    bslma::TestAllocator  sa;
    void                 *VOID_ALLOC     = 0;
    bslma::TestAllocator *SUPPLIED_ALLOC = &sa;
    bslma::TestAllocator *OBJECT_ALLOC   = &oa;

    bslma::TestAllocator *EXP_ALLOC      = 0;
    if (OBJECT == expAlloc) {
        EXP_ALLOC = OBJECT_ALLOC;
    } else if (SUPPLIED == expAlloc) {
        EXP_ALLOC = SUPPLIED_ALLOC;
    }

    MyClassDef rawBuf;
    memset(&rawBuf, 92, sizeof(rawBuf));
    TYPE *objPtr = (TYPE*) &rawBuf;

    // Preparation.

    TYPE::s_copyConstructorInvocations      = 0;
    TYPE::s_moveConstructorInvocations      = 0;
    TYPE::s_copyAllocConstructorInvocations = 0;
    TYPE::s_moveAllocConstructorInvocations = 0;
    TYPE::s_copyArgTConstructorInvocations  = 0;
    TYPE::s_moveArgTConstructorInvocations  = 0;

    // Construction.

    if (useAllocator) {
        Util::construct(objPtr, SUPPLIED_ALLOC, MoveUtil::move(fromObj));
    } else {
        Util::construct(objPtr, VOID_ALLOC,     MoveUtil::move(fromObj));
    }

    // Verification.

    ASSERTV(bsls::NameOf<TYPE>(), TYPE::s_copyConstructorInvocations,
            expCopyInvocationsNum == TYPE::s_copyConstructorInvocations);
    ASSERTV(bsls::NameOf<TYPE>(), TYPE::s_moveConstructorInvocations,
            expMoveInvocationsNum == TYPE::s_moveConstructorInvocations);
    ASSERTV(
        bsls::NameOf<TYPE>(), TYPE::s_copyAllocConstructorInvocations,
        expCopyAllocInvocationsNum == TYPE::s_copyAllocConstructorInvocations);
    ASSERTV(
        bsls::NameOf<TYPE>(), TYPE::s_moveAllocConstructorInvocations,
        expMoveAllocInvocationsNum == TYPE::s_moveAllocConstructorInvocations);
    ASSERTV(
        bsls::NameOf<TYPE>(), TYPE::s_copyArgTConstructorInvocations,
        expCopyArgTInvocationsNum == TYPE::s_copyArgTConstructorInvocations);
    ASSERTV(
        bsls::NameOf<TYPE>(), TYPE::s_moveAllocConstructorInvocations,
        expMoveArgTInvocationsNum == TYPE::s_moveArgTConstructorInvocations);

    ASSERTV(fromObj.value() == objPtr->value());
    ASSERTV(EXP_ALLOC       == objPtr->allocator());

    // Cleanup.

    TYPE::s_copyConstructorInvocations      = 0;
    TYPE::s_moveConstructorInvocations      = 0;
    TYPE::s_copyAllocConstructorInvocations = 0;
    TYPE::s_moveAllocConstructorInvocations = 0;
    TYPE::s_copyArgTConstructorInvocations  = 0;
    TYPE::s_moveArgTConstructorInvocations  = 0;
}

template<class TYPE>
void TestDriver<TYPE>::testCase5Alloc(
                                  int               expCopyInvocationsNum,
                                  int               expMoveInvocationsNum,
                                  int               expCopyAllocInvocationsNum,
                                  int               expMoveAllocInvocationsNum,
                                  int               expCopyArgTInvocationsNum,
                                  int               expMoveArgTInvocationsNum,
                                  ExpectedAllocator expAlloc)
{
    testCase5Imp(expCopyInvocationsNum,
                 expMoveInvocationsNum,
                 expCopyAllocInvocationsNum,
                 expMoveAllocInvocationsNum,
                 expCopyArgTInvocationsNum,
                 expMoveArgTInvocationsNum,
                 expAlloc,
                 true);
}

template<class TYPE>
void TestDriver<TYPE>::testCase5NoAlloc(
                                  int               expCopyInvocationsNum,
                                  int               expMoveInvocationsNum,
                                  int               expCopyAllocInvocationsNum,
                                  int               expMoveAllocInvocationsNum,
                                  int               expCopyArgTInvocationsNum,
                                  int               expMoveArgTInvocationsNum,
                                  ExpectedAllocator expAlloc)
{
    testCase5Imp(expCopyInvocationsNum,
                 expMoveInvocationsNum,
                 expCopyAllocInvocationsNum,
                 expMoveAllocInvocationsNum,
                 expCopyArgTInvocationsNum,
                 expMoveArgTInvocationsNum,
                 expAlloc,
                 false);
}

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
            // Create a container with an element constructed by (perfectly)
            // forwarding the specified 'value' and that uses the currently
            // installed default allocator to supply memory.  Note that this
            // constructor participates in overload resolution only if 'OTHER'
            // is implicitly convertible to 'TYPE'.
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
            // Create a container with an element constructed by (perfectly)
            // forwarding the specified 'value' and that uses the specified
            // 'basicAllocator' to supply memory.  If 'basicAllocator' is 0,
            // the currently installed default allocator is used.  Note that
            // this constructor participates in overload resolution only if
            // 'OTHER' is implicitly convertible to 'TYPE'.

        MyContainer(const MyContainer&  original,
                    bslma::Allocator   *basicAllocator = 0);
            // Create a container having the same value as the specified
            // 'original' object.  Optionally specify a 'basicAllocator' used
            // to supply memory.  If 'basicAllocator' is 0, the currently
            // installed default allocator is used.

        ~MyContainer();
            // Destroy this object.

        // MANIPULATORS
        MyContainer& operator=(const TYPE& rhs);
        MyContainer& operator=(const MyContainer& rhs);
            // Assign this object a new value

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

// The assignment needs to take the allocator into account.
//..
    template <class TYPE>
    MyContainer<TYPE>& MyContainer<TYPE>::operator=(const TYPE& rhs)
    {
        *d_value_p  = rhs;
        return *this;
    }
    template <class TYPE>
    MyContainer<TYPE>& MyContainer<TYPE>::operator=(const MyContainer& rhs)
    {
        *d_value_p  = *rhs.d_value_p;
        return *this;
    }

//..
// Finally, we perform a simple test of 'MyContainer', instantiating it with
// element type 'int':
//..
    int usageExample1()
    {
        bslma::TestAllocator testAlloc;
        MyContainer<int>     C1(123, &testAlloc);
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
///Example 3: Constructing into non-heap memory
///- - - - - - - - - - - - - - - - - - - - - -
// This example demonstrates the use of the 'make' function to implement a
// simple wrapper class that contains a single item that might or might not
// use the bslma allocator protocol.
//
// First we define wrapper class that hold an object and a functor and
// calls the functor (called the listener) each time the wrapped object is
// assigned to. We store the object directly as a member variable, instead of
// using an uninitialised buffer, to avoid the separate construction step :
//..
    template <class TYPE, class FUNC>
    class MyTriggeredWrapper {
        // PRIVATE DATA
        TYPE d_value;
        FUNC d_listener;

    public:
        // CREATORS
        MyTriggeredWrapper(const FUNC& f, bslma::Allocator *alloc = 0);
        MyTriggeredWrapper(const TYPE& v, const FUNC& f,
                           bslma::Allocator *alloc = 0);
        MyTriggeredWrapper(const MyTriggeredWrapper& other,
                           bslma::Allocator *alloc = 0);
        ~MyTriggeredWrapper() { }

        // MANIPULATORS
        MyTriggeredWrapper& operator=(const TYPE& rhs);
        MyTriggeredWrapper& operator=(const MyTriggeredWrapper& rhs);
            // Assign this object a new value and call the listner with the
            // new value after assignment.

        // ACCESSORS
        const TYPE& value() const { return d_value; }
        const FUNC& listener() const { return d_listener; }
    };
//..
// Next we define the constructors such that they initialize 'd_value' using
// the specified allocator if and only if 'TYPE' accepts an allocator. The
// 'bslma::ConstructUtil::make' family of functions encapsulate all of the
// metaprogramming that detects whether or not 'TYPE' uses an allocator and,
// if so, which construction protocol it uses (allocator at the front or at
// the back of the argument list), making all three constructors straight-
// forward:
//..
    template <class TYPE, class FUNC>
    MyTriggeredWrapper<TYPE, FUNC>::MyTriggeredWrapper(const FUNC&       f,
                                                       bslma::Allocator *alloc)
        : d_value(bslma::ConstructionUtil::make<TYPE>(alloc))
        , d_listener(f)
    {
    }

    template <class TYPE, class FUNC>
    MyTriggeredWrapper<TYPE, FUNC>::MyTriggeredWrapper(const TYPE&       v,
                                                       const FUNC&       f,
                                                       bslma::Allocator *alloc)
        : d_value(bslma::ConstructionUtil::make<TYPE>(alloc, v))
        , d_listener(f)
    {
    }

    template <class TYPE, class FUNC>
    MyTriggeredWrapper<TYPE, FUNC>::MyTriggeredWrapper(
                                              const MyTriggeredWrapper&  other,
                                              bslma::Allocator          *alloc)
        : d_value(bslma::ConstructionUtil::make<TYPE>(alloc, other.value()))
        , d_listener(other.d_listener)
    {
    }
//..
// Note that, in order for 'd_value' to be constructed with the correct
// allocator, the compiler must construct the result of 'make' directly into
// the the 'd_value' variable, an optimization formerly known prior to C++17
// as "copy elision".  This optimization is required by the C++17 standard and
// is optional in pre-2017 standards, but is implemented in all of the
// compilers for which this component is expected to be used at Bloomberg.
//
// Next, we implement the assignment operators, which call the listener:
//..
    template <class TYPE, class FUNC>
    MyTriggeredWrapper<TYPE, FUNC>&
    MyTriggeredWrapper<TYPE, FUNC>::operator=(const TYPE& rhs)
    {
        d_value = rhs;
        d_listener(d_value);
        return *this;
    }

    template <class TYPE, class FUNC>
    MyTriggeredWrapper<TYPE, FUNC>&
    MyTriggeredWrapper<TYPE, FUNC>::operator=(const MyTriggeredWrapper& rhs)
    {
        return operator=(rhs.value());
    }
//..
// Finally, we check our work by creating a listener for 'MyContainer<int>'
// that stores its last-seen value in a known location and creating a wrapper
// around 'MyContainer<int>' to test it.
//..
    int lastSeen = 0;
    void myListener(const MyContainer<int>& c) {
        lastSeen = c.front();
    }

    void usageExample3() {
        bslma::TestAllocator testAlloc;
        MyTriggeredWrapper<MyContainer<int>, void (*)(const MyContainer<int> &) >
                      wrappedContainer(myListener, &testAlloc);
        ASSERT(&testAlloc == wrappedContainer.value().allocator());

        wrappedContainer = MyContainer<int>(99);

        ASSERT(99 == lastSeen);
    }
//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int            test = argc > 1 ? atoi(argv[1]) : 0;
                verbose = argc > 2;
            veryVerbose = argc > 3;
        veryVeryVerbose = argc > 4;
    veryVeryVeryVerbose = argc > 5;

    forceDestructorCall = veryVeryVerbose;

    bslma::TestAllocator defaultAllocator("default", veryVeryVeryVerbose);
    bslma::DefaultAllocatorGuard dag(&defaultAllocator);

    bslma::TestAllocator globalAllocator("global", veryVeryVeryVerbose);
    bslma::Default::setGlobalAllocator(&globalAllocator);

    setbuf(stdout, NULL);    // Use unbuffered output

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:  // Zero is always the leading case.
      case 12: {
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
        usageExample3();

      } break;
      case 11: {
          // --------------------------------------------------------------------
          // TESTING 'make' FROM A DIFFERENT TYPE
          //
          // Concerns:
          //: 1 When constructing from an object of a different type, 'make'
          //:   moves from the source object if the source object is an
          //:   rvalue, and copies from the source object if the source object
          //:   is an lvalue.
          //: 2 That the 'allocator' is ignored if 'UsesBslmaAllocator' is
          //:   'false' for the type being constructed.
          //: 3 That the 'allocator' is passed to the constructor if it
          //:   is a pointer to a class derived from 'bslma::Allocator*' and
          //:   is 'UsesBslmaAllocator' is 'true' for the type being
          //:   constructed, either as the first argument (with
          //:   'bsl::allocator_arg') if 'UsesAllocatorArgT' is 'true' for
          //:   the type being tested; otherwise as the last argument.
          //: 4 That no unnecessary copies are created.
          //
          // Plan:
          //: 1 For concern 1, call the two-argument 'make' passing a test
          //:   allocator for the first argument and an lvalue object of a
          //:   different type as the second argument. Verify that the source
          //:   object is copied from. Repeat, passing an rvalue reference as
          //:   second argument and verify that the source object is moved
          //:   from.
          //: 2 For concern 2, perform step 1 using a target type for which
          //:   'UsesBslmaAllocator' is 'false'. Verify that the 'allocator'
          //:   is ignored.
          //: 3 For concern 3, perform step 1 using a target type for which
          //:   'UsesBslmaAllocator' is 'true' and a target type for which
          //:   'UsesAllocatorArgT' is 'true'. Using an 'allocator' which
          //:   is a pointer to a class derived from 'bslma::Allocator*',
          //:   check that the allocator is forwarded to the extended
          //:   constructor as described above.
          //: 4 For concern 4, perform steps 1-3 and verify that no unnecessary
          //:   copies of the source type and target type are made.
          //
          // Testing:
          //   make(bslma::Allocator                          *allocator,
          //        BSLS_COMPILERFEATURES_FORWARD_REF(ANY_TYPE) original)
          //   make(void                                      *allocator,
          //        BSLS_COMPILERFEATURES_FORWARD_REF(ANY_TYPE) original)
          // ------------------------------------------------------------------

          if (verbose) printf("\nTESTING 'make' FROM A DIFFERENT TYPE"
                              "\n====================================\n");

          bslma::TestAllocator testAllocator(veryVeryVeryVerbose);
          bslma::TestAllocator *const TA = &testAllocator;
          int                  dummyAllocator;  // not a 'bslma' allocator
          int                  *const XA = &dummyAllocator;
          if (veryVerbose) printf("Testing a non AA type with a non 'bslma'"
                                  "allocator \n");
          {
              MySrcClass src(1);
              int SCCI = MySrcClass::copyConstructorInvocations;
              int SMCI = MySrcClass::moveConstructorInvocations;
              int CCI = MyClass1::copyConstructorInvocations;
              int MCI = MyClass1::moveConstructorInvocations;

              MyClass1 dest1 = Util::make<MyClass1>(TA, src);
              ASSERTV(dest1.value(), 1 == dest1.value());
              ASSERT(CCI == MyClass1::copyConstructorInvocations);
              ASSERT(MCI == MyClass1::moveConstructorInvocations);
              ASSERTV(src.value(), 1 == src.value());
              ASSERT(SCCI == MySrcClass::copyConstructorInvocations);
              ASSERT(SMCI == MySrcClass::moveConstructorInvocations);

              MyClass1 dest2 = Util::make<MyClass1>(TA, MoveUtil::move(src));
              ASSERTV(dest2.value(), 1 == dest2.value());
              ASSERT(CCI == MyClass1::copyConstructorInvocations);
              ASSERT(MCI == MyClass1::moveConstructorInvocations);
              ASSERTV(src.value(), MOVED_FROM_VAL == src.value());
              ASSERT(SCCI == MySrcClass::copyConstructorInvocations);
              ASSERT(SMCI == MySrcClass::moveConstructorInvocations);
          }
          if (veryVerbose) printf("Testing a non AA type with a non 'bslma'"
                                  "allocator \n");
          {
              MySrcClass src(1);
              int SCCI = MySrcClass::copyConstructorInvocations;
              int SMCI = MySrcClass::moveConstructorInvocations;
              int CCI = MyClass1::copyConstructorInvocations;
              int MCI = MyClass1::moveConstructorInvocations;

              MyClass1 dest1 = Util::make<MyClass1>(XA, src);
              ASSERTV(dest1.value(), 1 == dest1.value());
              ASSERT(CCI == MyClass1::copyConstructorInvocations);
              ASSERT(MCI == MyClass1::moveConstructorInvocations);
              ASSERTV(src.value(), 1 == src.value());
              ASSERT(SCCI == MySrcClass::copyConstructorInvocations);
              ASSERT(SMCI == MySrcClass::moveConstructorInvocations);

              MyClass1 dest2 = Util::make<MyClass1>(XA, MoveUtil::move(src));
              ASSERTV(dest2.value(), 1 == dest2.value());
              ASSERT(CCI == MyClass1::copyConstructorInvocations);
              ASSERT(MCI == MyClass1::moveConstructorInvocations);
              ASSERTV(src.value(), MOVED_FROM_VAL == src.value());
              ASSERT(SCCI == MySrcClass::copyConstructorInvocations);
              ASSERT(SMCI == MySrcClass::moveConstructorInvocations);
          }
          if (veryVerbose) printf("Testing a 'UsesBslmaAllocator' type \n");
          {
              MySrcClass src(1);
              int SCCI = MySrcClass::copyConstructorInvocations;
              int SMCI = MySrcClass::moveConstructorInvocations;
              int CCI = MyClass2::copyConstructorInvocations;
              int MCI = MyClass2::moveConstructorInvocations;

              MyClass2 dest1 = Util::make<MyClass2>(TA, src);
              ASSERTV(dest1.value(), 1 == dest1.value());
              ASSERT(CCI == MyClass2::copyConstructorInvocations);
              ASSERT(MCI == MyClass2::moveConstructorInvocations);
              ASSERT(TA == dest1.allocator());
              ASSERTV(src.value(), 1 == src.value());
              ASSERT(SCCI == MySrcClass::copyConstructorInvocations);
              ASSERT(SMCI == MySrcClass::moveConstructorInvocations);

              MyClass2 dest2 = Util::make<MyClass2>(TA, MoveUtil::move(src));
              ASSERTV(dest2.value(), 1 == dest2.value());
              ASSERT(CCI == MyClass2::copyConstructorInvocations);
              ASSERT(MCI == MyClass2::moveConstructorInvocations);
              ASSERT(TA == dest2.allocator());
              ASSERTV(src.value(), MOVED_FROM_VAL == src.value());
              ASSERT(SCCI == MySrcClass::copyConstructorInvocations);
              ASSERT(SMCI == MySrcClass::moveConstructorInvocations);
          }
          if (veryVerbose) printf("Testing a 'UsesAllocatorArgT' type \n");
          {
              MySrcClass src(1);
              int SCCI = MySrcClass::copyConstructorInvocations;
              int SMCI = MySrcClass::moveConstructorInvocations;
              int CCI = MyClass2a::copyConstructorInvocations;
              int MCI = MyClass2a::moveConstructorInvocations;

              MyClass2a dest1 = Util::make<MyClass2a>(TA, src);
              ASSERTV(dest1.value(), 1 == dest1.value());
              ASSERT(CCI == MyClass2a::copyConstructorInvocations);
              ASSERT(MCI == MyClass2a::moveConstructorInvocations);
              ASSERT(TA == dest1.allocator());
              ASSERTV(src.value(), 1 == src.value());
              ASSERT(SCCI == MySrcClass::copyConstructorInvocations);
              ASSERT(SMCI == MySrcClass::moveConstructorInvocations);

              MyClass2a dest2 = Util::make<MyClass2a>(TA,
                                                        MoveUtil::move(src));
              ASSERTV(dest2.value(), 1 == dest2.value());
              ASSERT(CCI == MyClass2a::copyConstructorInvocations);
              ASSERT(MCI == MyClass2a::moveConstructorInvocations);
              ASSERT(TA == dest2.allocator());
              ASSERTV(src.value(), MOVED_FROM_VAL == src.value());
              ASSERT(SCCI == MySrcClass::copyConstructorInvocations);
              ASSERT(SMCI == MySrcClass::moveConstructorInvocations);
          }
          if (veryVerbose) printf("Testing a type that doesn't support move "
                                  "construction \n");
          {
              MySrcClass src(1);
              int SCCI = MySrcClass::copyConstructorInvocations;
              int SMCI = MySrcClass::moveConstructorInvocations;
              int CCI = MyClass3::copyConstructorInvocations;

              MyClass3 dest1 = Util::make<MyClass3>(TA, src);
              ASSERTV(dest1.value(), 1 == dest1.value());
              ASSERT(CCI == MyClass3::copyConstructorInvocations);
              ASSERT(TA == dest1.allocator());
              ASSERTV(src.value(), 1 == src.value());
              ASSERT(SCCI == MySrcClass::copyConstructorInvocations);
              ASSERT(SMCI == MySrcClass::moveConstructorInvocations);

              // MyClass3 doesn't support move conversion.
              MyClass3 dest2 = Util::make<MyClass3>(TA, MoveUtil::move(src));
              ASSERTV(dest2.value(), 1 == dest2.value());
              ASSERT(CCI == MyClass3::copyConstructorInvocations);
              ASSERT(TA == dest2.allocator());
              ASSERTV(src.value(), 1 == src.value());
              ASSERT(SCCI == MySrcClass::copyConstructorInvocations);
              ASSERT(SMCI == MySrcClass::moveConstructorInvocations);
          }
      } break;
      case 10: {
        // --------------------------------------------------------------------
        // TESTING 'make' FOR (EXTENDED) MOVE CONSTRUCTION
        //
        //: 1 When creating an object using 'make' method with an rvalue
        //:   of the same type, the resulting object is created by invoking
        //:   the (possibly extended) move constructor. If the type doesn't
        //:   support move construction, copy constructor is invoked instead.
        //: 2 That the 'allocator' is ignored if 'UsesBslmaAllocator' is
        //:   'false' for the type being constructed.
        //: 3 That the 'allocator' is passed to the constructor if it
        //:   is a pointer to a class derived from 'bslma::Allocator*' and
        //:   is 'UsesBslmaAllocator' is 'true' for the type being
        //:   constructed, either as the first argument (with
        //:   'bsl::allocator_arg') if 'UsesAllocatorArgT' is 'true' for
        //:   the type being tested; otherwise as the last argument.
        //: 4 That no unnecessary copies are created.
        //
        // Plan:
        //: 1 For concern 1, using a type that supports move construction,
        //:   call the two-argument 'make' passing a test allocator for the
        //:   first argument and an rvalue object of the same type as the
        //:   second argument. Verify that the source object is moved from.
        //: 2 For concern 2, perform step 1 using a target type for which
        //:   'UsesBslmaAllocator' is 'false'. Verify that the 'allocator'
        //:   is ignored.
        //: 3 For concern 3, perform step 1 using a target type for which
        //:   'UsesBslmaAllocator' is 'true' and a target type for which
        //:   'UsesAllocatorArgT' is 'true'. Using an 'allocator' which
        //:   is a pointer to a class derived from 'bslma::Allocator*',
        //:   check that the allocator is forwarded to the extended
        //:   constructor as described above.
        //: 3 For concern 1, perform step 1 using a target type that only
        //:   supports copy construction. Verify that the source object is
        //:   copied from.
        //: 4 For concern 4, perform steps 1-4 and verify that no unnecessary
        //:   copies of the target type are made.
        //
        // Testing:
        //   make (bslma::Allocator                          *allocator,
        //        BSLS_COMPILERFEATURES_FORWARD_REF(ANY_TYPE) original)
        //   make (void                                      *allocator,
        //        BSLS_COMPILERFEATURES_FORWARD_REF(ANY_TYPE) original)
        // --------------------------------------------------------------------

          if (verbose) printf("\nTESTING 'make' FOR (EXTENDED) MOVE "
                              "CONSTRUCTION"
                              "\n==================================="
                              "============\n");

          bslma::TestAllocator testAllocator(veryVeryVeryVerbose);
          bslma::TestAllocator *const TA = &testAllocator;
          int                  dummyAllocator;  // not a 'bslma' allocator
          int                  *const XA = &dummyAllocator;
          if (veryVerbose) printf("Testing a non AA type with a non 'bslma'"
                                  "allocator \n");
          {
              MyClass1 src = V1;
              int CCI = MyClass1::copyConstructorInvocations;
              int MCI = MyClass1::moveConstructorInvocations;
              MyClass1 dest = Util::make<MyClass1>(TA, MoveUtil::move(src));
              ASSERTV(dest.value(), V1.value() == dest.value());
              ASSERT(CCI == MyClass1::copyConstructorInvocations);
              ASSERT(MCI == MyClass1::moveConstructorInvocations -1);
          }
          if (veryVerbose) printf("Testing a non AA type with a 'bslma'"
                                  "allocator \n");
          {
              MyClass1 src = V1;
              int CCI = MyClass1::copyConstructorInvocations;
              int MCI = MyClass1::moveConstructorInvocations;
              MyClass1 dest = Util::make<MyClass1>(XA, MoveUtil::move(src));
              ASSERTV(dest.value(), V1.value() == dest.value());
              ASSERT(CCI == MyClass1::copyConstructorInvocations);
              ASSERT(MCI == MyClass1::moveConstructorInvocations -1);
          }
          if (veryVerbose) printf("Testing a 'UsesBslmaAllocator' type \n");
          {
              MyClass2 src = V2;
              int CCI = MyClass2::copyConstructorInvocations;
              int MCI = MyClass2::moveConstructorInvocations;
              MyClass2 dest = Util::make<MyClass2>(TA, MoveUtil::move(src));
              ASSERTV(dest.value(), V2.value() == dest.value());
              ASSERT(TA == dest.allocator());
              ASSERT(CCI == MyClass2::copyConstructorInvocations);
              ASSERT(MCI == MyClass2::moveConstructorInvocations -1);
          }
          if (veryVerbose) printf("Testing a 'UsesAllocatorArgT' type \n");
          {
              MyClass2a src = V2A;
              int CCI = MyClass2a::copyConstructorInvocations;
              int MCI = MyClass2a::moveConstructorInvocations;
              MyClass2a dest = Util::make<MyClass2a>(TA,
                                                       MoveUtil::move(src));
              ASSERTV(dest.value(), V2A.value() == dest.value());
              ASSERT(TA == dest.allocator());
              ASSERT(CCI == MyClass2a::copyConstructorInvocations);
              ASSERT(MCI == MyClass2a::moveConstructorInvocations -1);
          }
      } break;
     case 9: {
          // --------------------------------------------------------------------
          // TESTING 'make' FOR (EXTENDED) COPY CONSTRUCTION
          //
          //: 1 When creating an object using 'make' method with an lvalue
          //:   of the same type, the resulting object is created by invoking
          //:   the (possibly extended) copy constructor.
          //: 2 That the 'allocator' is ignored if 'UsesBslmaAllocator' is
          //:   'false' for the type being constructed.
          //: 3 That the 'allocator' is passed to the constructor if it
          //:   is a pointer to a class derived from 'bslma::Allocator*' and
          //:   is 'UsesBslmaAllocator' is 'true' for the type being
          //:   constructed, either as the first argument (with
          //:   'bsl::allocator_arg') if 'UsesAllocatorArgT' is 'true' for
          //:   the type being tested; otherwise as the last argument.
          //: 4 That no unnecessary copies are created.
          //
          // Plan:
          //: 1 For concern 1, call the two-argument 'make' passing a test
          //    allocator for the first argument and an lvalue object of the
          //    same type as the second argument. Verify that the source object
          //    is copied from.
          //: 2 For concern 2, perform step 1 using a target type for which
          //:   'UsesBslmaAllocator' is 'false'. Verify that the 'allocator'
          //:   is ignored.
          //: 3 For concern 3, perform step 1 using a target type for which
          //:   'UsesBslmaAllocator' is 'true' and a target type for which
          //:   'UsesAllocatorArgT' is 'true'. Using an 'allocator' which
          //:   is a pointer to a class derived from 'bslma::Allocator*',
          //:   check that the allocator is forwarded to the extended
          //:   constructor as described above.
          //: 4 For concern 4, perform steps 1-3 and verify that no unnecessary
          //:   copies of the target type are made.
          //
          //
          // Testing:
          //   make (bslma::Allocator                          *allocator,
          //        BSLS_COMPILERFEATURES_FORWARD_REF(ANY_TYPE) original)
          //   make (void                                      *allocator,
          //        BSLS_COMPILERFEATURES_FORWARD_REF(ANY_TYPE) original)
          // --------------------------------------------------------------------

          if (verbose) printf("\nTESTING 'make' FOR (EXTENDED) COPY "
                              "CONSTRUCTION"
                              "\n===================================="
                              "============\n");

          bslma::TestAllocator testAllocator(veryVeryVeryVerbose);
          bslma::TestAllocator *const TA = &testAllocator;
          int                  dummyAllocator;  // not a 'bslma' allocator
          int                  *const XA = &dummyAllocator;
          if (veryVerbose) printf("Testing a non AA type with a non 'bslma'"
                                  "allocator \n");
          {
              int CCI = MyClass1::copyConstructorInvocations;
              int MCI = MyClass1::moveConstructorInvocations;
              MyClass1 dest = Util::make<MyClass1>(TA, V1);
              ASSERTV(dest.value(), V1.value() == dest.value());
              ASSERT(CCI == MyClass1::copyConstructorInvocations -1);
              ASSERT(MCI == MyClass1::moveConstructorInvocations);
          }
          if (veryVerbose) printf("Testing a non AA type with a 'bslma'"
                                  "allocator \n");
          {
              int CCI = MyClass1::copyConstructorInvocations;
              int MCI = MyClass1::moveConstructorInvocations;
              MyClass1 dest = Util::make<MyClass1>(XA, V1);
              ASSERTV(dest.value(), V1.value() == dest.value());
              ASSERT(CCI == MyClass1::copyConstructorInvocations -1);
              ASSERT(MCI == MyClass1::moveConstructorInvocations);
          }
          if (veryVerbose) printf("Testing a 'UsesBslmaAllocator' type \n");
          {
              int CCI = MyClass2::copyConstructorInvocations;
              int MCI = MyClass2::moveConstructorInvocations;
              MyClass2 dest = Util::make<MyClass2>(TA, V2);
              ASSERTV(dest.value(), V2.value() == dest.value());
              ASSERT(TA == dest.allocator());
              ASSERT(CCI == MyClass2::copyConstructorInvocations -1);
              ASSERT(MCI == MyClass2::moveConstructorInvocations);
          }
          if (veryVerbose) printf("Testing a 'UsesAllocatorArgT' type \n");
          {
              int CCI = MyClass2a::copyConstructorInvocations;
              int MCI = MyClass2a::moveConstructorInvocations;
              MyClass2a dest = Util::make<MyClass2a>(TA, V2A);
              ASSERTV(dest.value(), V2A.value() == dest.value());
              ASSERT(TA == dest.allocator());
              ASSERT(CCI == MyClass2a::copyConstructorInvocations -1);
              ASSERT(MCI == MyClass2a::moveConstructorInvocations);
          }
      } break;
      case 8: {
          // --------------------------------------------------------------------
          // TESTING 'make' WITH DEFAULT CONSTRUCTION
          //
          // Concerns:
          //: 1 That 'make' with only an allocator argument will
          //:   default-construct a type that does not use allocators . The
          //:   allocator is ignored.
          //: 2 That 'make' with only a 'bslma::allocator' pointer argument
          //:   will pass that argument to the extended default constructor of
          //:   a type that uses the bslma allocator protocol.
          //: 3 That no unnecessary copies are created.
          //
          // Plan:
          //: 1 Construct an object using 'make' method taking only an
          //:   allocator. Verify that the target object is default
          //:   constructed.
          //: 2 For concern 2, perform step 1 using a target type for which
          //:   'UsesBslmaAllocator' is 'false'. Verify that the 'allocator'
          //:   is ignored.
          //: 3 For concern 3, perform step 1 using a target type for which
          //:   'UsesBslmaAllocator' is 'true' and a target type for which
          //:   'UsesAllocatorArgT' is 'true'. Using an 'allocator' which
          //:   is a pointer to a class derived from 'bslma::Allocator*',
          //:   check that the allocator is forwarded to the extended
          //:   constructor as described above.
          //: 4 For concern 4, perform steps 1-3 and verify that no unnecessary
          //:   copies of the target type are made.
          //
          // Testing:
          //   make(bslma::Allocator*)
          //   make(void*)
          // --------------------------------------------------------------------

          if (verbose) printf("\nTESTING 'make' WITH DEFAULT CONSTRUCTION"
                              "\n========================================\n");

          bslma::TestAllocator testAllocator(veryVeryVeryVerbose);
          bslma::TestAllocator *const TA = &testAllocator;
          int                  dummyAllocator;  // not a 'bslma' allocator
          int                  *const XA = &dummyAllocator;

          if (veryVerbose) printf("Testing a non AA type with a non 'bslma'"
                                  "allocator \n");
          {
              int CCI = MyClass1::copyConstructorInvocations;
              int MCI = MyClass1::moveConstructorInvocations;
              MyClass1 dest = Util::make<MyClass1>(XA);
              ASSERTV(dest.value(), 0 == dest.value());
              ASSERT(CCI == MyClass1::copyConstructorInvocations);
              ASSERT(MCI == MyClass1::moveConstructorInvocations);
          }
          if (veryVerbose) printf("Testing a non AA type with a 'bslma'"
                                  "allocator \n");
          {
              int CCI = MyClass1::copyConstructorInvocations;
              int MCI = MyClass1::moveConstructorInvocations;
              MyClass1 dest = Util::make<MyClass1>(TA);
              ASSERTV(dest.value(), 0 == dest.value());
              ASSERT(CCI == MyClass1::copyConstructorInvocations);
              ASSERT(MCI == MyClass1::moveConstructorInvocations);
          }
          if (veryVerbose) printf("Testing a 'UsesBslmaAllocator' type \n");
          {
              int CCI = MyClass2::copyConstructorInvocations;
              int MCI = MyClass2::moveConstructorInvocations;
              MyClass2 dest = Util::make<MyClass2>(TA);
              ASSERTV(dest.value(), 0 == dest.value());
              ASSERT(TA == dest.allocator());
              ASSERT(CCI == MyClass2::copyConstructorInvocations);
              ASSERT(MCI == MyClass2::moveConstructorInvocations);
          }
          if (veryVerbose) printf("Testing a 'UsesAllocatorArgT' type \n");
          {
              int CCI = MyClass2a::copyConstructorInvocations;
              int MCI = MyClass2a::moveConstructorInvocations;
              MyClass2a dest = Util::make<MyClass2a>(TA);
              ASSERTV(dest.value(), 0 == dest.value());
              ASSERT(TA == dest.allocator());
              ASSERT(CCI == MyClass2a::copyConstructorInvocations);
              ASSERT(MCI == MyClass2a::moveConstructorInvocations);
          }
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

        bslma::TestAllocator        testAllocator(veryVeryVeryVerbose);
        bslma::TestAllocator *const TA = &testAllocator;
        int                         dummyAllocator;  // not a 'bslma' allocator
        int                  *const XA = &dummyAllocator;

        if (verbose) printf("Value and allocator testing.\n");

        // MyClass #  Operation                            Val Alloc
        // ==========  ==================================== === =====
        {
            MyClassDef  rawBuf;
            MyClass1   *srcPtr = (MyClass1 *)&rawBuf;
            Util::construct(srcPtr, TA, V1);
            TEST_OP(1, destructiveMove(objPtr, TA, srcPtr),  1, 0);
            ASSERT(91 == rawBuf.d_value);
            ASSERT(0  == rawBuf.d_allocator_p);
        }
        {
            MyClassDef  rawBuf;
            MyClass2   *srcPtr = (MyClass2 *)&rawBuf;
            Util::construct(srcPtr, TA, V2);
            TEST_OP(2, destructiveMove(objPtr, TA, srcPtr),  2, TA);
            ASSERT(92 == rawBuf.d_value);
            ASSERT(0  == rawBuf.d_allocator_p);
        }
        {
            MyClassDef  rawBuf;
            MyClass2a  *srcPtr = (MyClass2a *)&rawBuf;
            Util::construct(srcPtr, TA, V2A);
            TEST_OP(2a, destructiveMove(objPtr, TA, srcPtr), 0x2a, TA);
            ASSERT(92 == rawBuf.d_value);
            ASSERT(0  == rawBuf.d_allocator_p);
        }
        {
            MyClassDef  rawBuf;
            MyClass3   *srcPtr = (MyClass3 *)&rawBuf;
            Util::construct(srcPtr, TA, V3);
            TEST_OP(3, destructiveMove(objPtr, TA, srcPtr),  3, TA);
            ASSERT(93 == rawBuf.d_value);
            ASSERT(0  == rawBuf.d_allocator_p);
        }
        {
            MyClassDef  rawBuf;
            MyClass1   *srcPtr = (MyClass1 *)&rawBuf;
            Util::construct(srcPtr, TA, V1);
            TEST_OP(1, destructiveMove(objPtr, XA, srcPtr),  1, 0);
            ASSERT(91 == rawBuf.d_value);
            ASSERT(0  == rawBuf.d_allocator_p);
        }
        {
            MyClassDef  rawBuf;
            MyClass2   *srcPtr = (MyClass2 *)&rawBuf;
            Util::construct(srcPtr, TA, V2);
            // Must use 'TA' so that behavior is the same in C++98 mode (copy,
            // uses default allocator) and C++11 mode (move, copies '*srcPtr'
            // allocator).
            TEST_OP(2, destructiveMove(objPtr, TA, srcPtr),  2, TA);
            ASSERT(92 == rawBuf.d_value);
            ASSERT(0  == rawBuf.d_allocator_p);
        }
        {
            MyClassDef  rawBuf;
            MyClass2a  *srcPtr = (MyClass2a *)&rawBuf;
            Util::construct(srcPtr, TA, V2A);
            // Must use 'TA' so that behavior is the same in C++98 mode (copy,
            // uses default allocator) and C++11 mode (move, copies '*srcPtr'
            // allocator).
            TEST_OP(2a, destructiveMove(objPtr, TA, srcPtr), 0x2a, TA);
            ASSERT(92 == rawBuf.d_value);
            ASSERT(0  == rawBuf.d_allocator_p);
        }
        {
            MyClassDef  rawBuf;
            MyClass3   *srcPtr = (MyClass3 *)&rawBuf;
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
            MyClassDef    rawBuf[4];
            MyPairAA_4_4 *srcPtr = (MyPairAA_4_4 *)&rawBuf[0];
            Util::construct(srcPtr, TA, PAAV4V4);
            bslma::AutoDestructor<MyPairAA_4_4> guard(srcPtr, 1);
            MyPairAA_4_4 *objPtr = (MyPairAA_4_4 *)&rawBuf[2];
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
            objPtr->~MyPairAA_4_4();
        } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

        if (verbose) printf("Trait selection testing.\n");
        {
            MyClassDef    rawBuf[2];
            const int      DI = MyClassFussy::s_destructorInvocations;
            MyClassFussy *srcPtr = (MyClassFussy *) &rawBuf[0];
            Util::construct(srcPtr, XA, VF);
            MyClassFussy *objPtr = (MyClassFussy *) &rawBuf[1];
            Util::destructiveMove(objPtr, XA, srcPtr);
            ASSERT(DI == MyClassFussy::s_destructorInvocations);
            ASSERT(5  == rawBuf[0].d_value);
            ASSERT(0  == rawBuf[0].d_allocator_p);
            ASSERT(5  == rawBuf[1].d_value);
            ASSERT(0  == rawBuf[1].d_allocator_p);
            if (veryVerbose) {
                P_(rawBuf[0].d_value); PP(rawBuf[0].d_allocator_p);
                P_(rawBuf[1].d_value); PP(rawBuf[1].d_allocator_p);
            }
            objPtr->~MyClassFussy();
        }
        {
            MyClassDef     rawBuf[2];
            const int       DI  = MyClassFussy2::s_destructorInvocations;
            MyClassFussy2 *srcPtr = (MyClassFussy2 *) &rawBuf[0];
            Util::construct(srcPtr, XA, VF2);
            const int       CCI = MyClassFussy2::s_copyConstructorInvocations;
            MyClassFussy2 *objPtr = (MyClassFussy2 *) &rawBuf[1];
            Util::destructiveMove(objPtr, XA, srcPtr);
            ASSERT(DI  == MyClassFussy2::s_destructorInvocations);
            ASSERT(CCI == MyClassFussy2::s_copyConstructorInvocations);
            ASSERT(6   == rawBuf[0].d_value);
            ASSERT(0   == rawBuf[0].d_allocator_p);
            ASSERT(6   == rawBuf[1].d_value);
            ASSERT(0   == rawBuf[1].d_allocator_p);
            if (veryVerbose) {
                P_(rawBuf[0].d_value); PP(rawBuf[0].d_allocator_p);
                P_(rawBuf[1].d_value); PP(rawBuf[1].d_allocator_p);
            }
            objPtr->~MyClassFussy2();
        }

      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING 'construct'
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

        if (verbose) printf("\nTESTING 'construct'"
                            "\n===================\n");

        bslma::Allocator     *const DA = bslma::Default::allocator();
        bslma::TestAllocator        testAllocator(veryVeryVeryVerbose);
        bslma::TestAllocator *const TA = &testAllocator;
        int                         dummyAllocator;  // not a 'bslma' allocator
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
            MyClassDef    rawBuf[2];
            MyPairAA_4_4 *objPtr = (MyPairAA_4_4 *)rawBuf;
            Util::construct(objPtr, TA, PAAV4V4);
            ASSERT(4  == rawBuf[0].d_value);
            ASSERT(TA == rawBuf[0].d_allocator_p);
            ASSERT(4  == rawBuf[1].d_value);
            ASSERT(TA == rawBuf[1].d_allocator_p);
            objPtr->~MyPairAA_4_4();
        } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

        BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
            MyClassDef    rawBuf[2];
            MyPairAA_4_4 *objPtr = (MyPairAA_4_4 *)rawBuf;
            Util::construct(objPtr, TA, V4, V4);
            ASSERT(4  == rawBuf[0].d_value);
            ASSERT(TA == rawBuf[0].d_allocator_p);
            ASSERT(4  == rawBuf[1].d_value);
            ASSERT(TA == rawBuf[1].d_allocator_p);
            objPtr->~MyPairAA_4_4();
        } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

        if (verbose) printf("\t...constructing pair with IsPair\n");

        // Testing that 'TA' is not passed to the pair constructor and the
        // default allocator is used instead.
        const bsls::Types::Int64 NUM_ALLOC1 = testAllocator.numAllocations();
        BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
            MyClassDef    rawBuf[2];
            MyPairBB_4_4 *objPtr = (MyPairBB_4_4 *)rawBuf;
            Util::construct(objPtr, TA, PBBV4V4);
            ASSERT(4  == rawBuf[0].d_value);
            ASSERT(DA == rawBuf[0].d_allocator_p);
            ASSERT(4  == rawBuf[1].d_value);
            ASSERT(DA == rawBuf[1].d_allocator_p);
            objPtr->~MyPairBB_4_4();
        } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
        ASSERT(NUM_ALLOC1 == testAllocator.numAllocations());

        const bsls::Types::Int64 NUM_ALLOC2 = testAllocator.numAllocations();
        BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
            MyClassDef    rawBuf[2];
            MyPairBB_4_4 *objPtr = (MyPairBB_4_4 *)rawBuf;
            Util::construct(objPtr, TA, V4, V4);
            ASSERT(4  == rawBuf[0].d_value);
            ASSERT(DA == rawBuf[0].d_allocator_p);
            ASSERT(4  == rawBuf[1].d_value);
            ASSERT(DA == rawBuf[1].d_allocator_p);
            objPtr->~MyPairBB_4_4();
        } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
        ASSERT(NUM_ALLOC2 == testAllocator.numAllocations());

        if (verbose) printf("Trait selection testing.\n");
        {
            MyClassDef    rawBuf;
            MyClassFussy *objPtr = (MyClassFussy *) &rawBuf;
            memset(&rawBuf, 92, sizeof rawBuf);
            Util::construct(objPtr, (bslma::Allocator*)TA, VF);
            ASSERT(5 == rawBuf.d_value);
            ASSERT(0 == rawBuf.d_allocator_p);
            if (veryVerbose) { P_(rawBuf.d_value); PP(rawBuf.d_allocator_p); }
        }

        {
            MyClassDef    rawBuf;
            MyClassFussy *objPtr = (MyClassFussy *) &rawBuf;
            memset(&rawBuf, 92, sizeof rawBuf);
            const int CVI = MyClassFussy::s_conversionConstructorInvocations;
            const int VF  = 3;
            Util::construct(objPtr, (bslma::Allocator*)TA, VF);
            ASSERT(CVI < MyClassFussy::s_conversionConstructorInvocations);
            ASSERT(3 == rawBuf.d_value);
            ASSERT(0 == rawBuf.d_allocator_p);
            if (veryVerbose) { P_(rawBuf.d_value); PP(rawBuf.d_allocator_p); }
        }

      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING MOVE CONSTRUCTION
        //
        // Concerns:
        //: 1 The 'construct' function with 'MovableRef' parameter properly
        //:   forwards the allocator to the object constructor when
        //:   appropriate.
        //:
        //: 2 The 'construct' function with 'MovableRef' parameter uses
        //:   'memcpy' instead of constructor call for bitwise copyable
        //:   objects.
        //
        // Plan:
        //: 1 Using special types, construct copies of pre-initialized objects
        //:   into uninitialized buffers using move construction, passing
        //:   allocators of both types 'bslma::Allocator *' and 'void *' types,
        //:   and verify that the value and allocator of the copy are as
        //:   expected.  Ensure that the expected overload of move
        //:   constructor (or copy constructor) is invoked.  (C-1,2)
        //
        // Testing:
        //   void construct(TYPE *addr, Allocator *a, MovableRef<TYPE> orig);
        //   void construct(TYPE *addr, void      *a, MovableRef<TYPE> orig);
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING MOVE CONSTRUCTION"
                            "\n=========================\n");

        typedef TestDriver<           TrivialTestType>     T;
        typedef TestDriver<        NonTrivialTestType>    NT;
        typedef TestDriver<        BslmaAllocTestType>   BAT;
        typedef TestDriver<         ArgTAllocTestType>   ATT;

        typedef TestDriver<   MoveOnlyTrivialTestType>   MOT;
        typedef TestDriver<MoveOnlyNonTrivialTestType>  MONT;
        typedef TestDriver<MoveOnlyBslmaAllocTestType> MOBAT;
        typedef TestDriver< MoveOnlyArgTAllocTestType> MOATT;

        typedef TestDriver<   CopyOnlyTrivialTestType>   COT;
        typedef TestDriver<CopyOnlyNonTrivialTestType>  CONT;
        typedef TestDriver<CopyOnlyBslmaAllocTestType> COBAT;
        typedef TestDriver< CopyOnlyArgTAllocTestType> COATT;

        // Legend
        // ------
        // CP   - copy constructor
        // MV   - move constructor
        // CP_B - copy constructor with bslma allocator
        // MV_B - move constructor with bslma allocator
        // CP_A - copy constructor with an argument tag
        // MV_A - move constructor with an argument tag

        if (verbose) printf("\tWithout allocators\n");

        //                      CP   MV   CP_B MV_B CP_A MV_A EXP_ALLOCATOR
        //                      ==   ==   ==== ==== ==== ==== ==============
            T::testCase5NoAlloc(0,   0,   0,   0,   0,   0,       T::OBJECT  );
           NT::testCase5NoAlloc(0,   1,   0,   0,   0,   0,      NT::DEFAULT );
          BAT::testCase5NoAlloc(0,   1,   0,   0,   0,   0,     BAT::DEFAULT );
          ATT::testCase5NoAlloc(0,   1,   0,   0,   0,   0,     ATT::DEFAULT );

          MOT::testCase5NoAlloc(0,   0,   0,   0,   0,   0,     MOT::OBJECT  );
         MONT::testCase5NoAlloc(0,   1,   0,   0,   0,   0,    MONT::DEFAULT );
        MOBAT::testCase5NoAlloc(0,   1,   0,   0,   0,   0,   MOBAT::DEFAULT );
        MOATT::testCase5NoAlloc(0,   1,   0,   0,   0,   0,   MOATT::DEFAULT );

          COT::testCase5NoAlloc(0,   0,   0,   0,   0,   0,     COT::OBJECT  );
         CONT::testCase5NoAlloc(1,   0,   0,   0,   0,   0,    CONT::DEFAULT );
        COBAT::testCase5NoAlloc(1,   0,   0,   0,   0,   0,   COBAT::DEFAULT );
        COATT::testCase5NoAlloc(1,   0,   0,   0,   0,   0,   COATT::DEFAULT );

        if (verbose) printf("\tWith allocators\n");

        //                      CP   MV   CP_B MV_B CP_A MV_A EXP_ALLOCATOR
        //                      ==   ==   ==== ==== ==== ==== ===============
            T::testCase5Alloc(  0,   0,   0,   0,   0,   0,       T::OBJECT  );
           NT::testCase5Alloc(  0,   1,   0,   0,   0,   0,      NT::DEFAULT );
          BAT::testCase5Alloc(  0,   0,   0,   1,   0,   0,     BAT::SUPPLIED);
          ATT::testCase5Alloc(  0,   0,   0,   0,   0,   1,     ATT::SUPPLIED);

          MOT::testCase5Alloc(  0,   0,   0,   0,   0,   0,     MOT::OBJECT  );
         MONT::testCase5Alloc(  0,   1,   0,   0,   0,   0,    MONT::DEFAULT );
        MOBAT::testCase5Alloc(  0,   0,   0,   1,   0,   0,   MOBAT::SUPPLIED);
        MOATT::testCase5Alloc(  0,   0,   0,   0,   0,   1,   MOATT::SUPPLIED);

          COT::testCase5Alloc(  0,   0,   0,   0,   0,   0,     COT::OBJECT  );
         CONT::testCase5Alloc(  1,   0,   0,   0,   0,   0,    CONT::DEFAULT );
        COBAT::testCase5Alloc(  0,   0,   1,   0,   0,   0,   COBAT::SUPPLIED);
        COATT::testCase5Alloc(  0,   0,   0,   0,   1,   0,   COATT::SUPPLIED);
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING 'copyConstruct'
        //
        // Concerns:
        //   o That the copy constructor properly forwards the allocator
        //     when appropriate.
        //   o That the copy constructor uses 'memcpy' when appropriate.
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

        if (verbose) printf("\nTESTING 'copyConstruct'"
                            "\n=======================\n");

        bslma::Allocator     *const DA = bslma::Default::allocator();
        bslma::TestAllocator        testAllocator(veryVeryVeryVerbose);
        bslma::TestAllocator *const TA = &testAllocator;
        int                         dummyAllocator; //not a 'bslma' allocator
        int                  *const XA = &dummyAllocator;

        // MyClass                               Expected
        //      #  Operation                      Val Alloc
        //      =  ============================== === =====
        TEST_OP(1, construct(objPtr, TA, V1),  1, 0);
        TEST_OP(2, construct(objPtr, TA, V2),  2, TA);
        TEST_OP(1, construct(objPtr, XA, V1),  1, 0);
        TEST_OP(2, construct(objPtr, XA, V2),  2, 0);

        if (verbose) printf("Exception testing.\n");

        if (verbose) printf("\t...pair with allocators\n");

        BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
            MyClassDef    rawBuf[2];
            MyPairAA_4_4 *objPtr = (MyPairAA_4_4 *)rawBuf;
            Util::construct(objPtr, TA, PAAV4V4);
            ASSERT(4  == rawBuf[0].d_value);
            ASSERT(TA == rawBuf[0].d_allocator_p);
            ASSERT(4  == rawBuf[1].d_value);
            ASSERT(TA == rawBuf[1].d_allocator_p);
            objPtr->~MyPairAA_4_4();
        } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

        if (verbose) printf("\t...constructing pair with IsPair\n");

        // Testing that 'TA' is not passed to the pair constructor and the
        // default allocator is used instead.
        const bsls::Types::Int64 NUM_ALLOC1 = testAllocator.numAllocations();
        BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
            MyClassDef    rawBuf[2];
            MyPairBB_4_4 *objPtr = (MyPairBB_4_4 *)rawBuf;
            Util::construct(objPtr, TA, PBBV4V4);
            ASSERT(4  == rawBuf[0].d_value);
            ASSERT(DA == rawBuf[0].d_allocator_p);
            ASSERT(4  == rawBuf[1].d_value);
            ASSERT(DA == rawBuf[1].d_allocator_p);
            objPtr->~MyPairBB_4_4();
        } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
        ASSERT(NUM_ALLOC1 == testAllocator.numAllocations());

        if (verbose) printf("Trait selection testing.\n");
        {
            MyClassDef    rawBuf;
            MyClassFussy *objPtr = (MyClassFussy *) &rawBuf;
            memset(&rawBuf, 92, sizeof rawBuf);
            Util::construct(objPtr, XA, VF);
            ASSERT(5 == rawBuf.d_value);
            ASSERT(0 == rawBuf.d_allocator_p);
            if (veryVerbose) { P_(rawBuf.d_value); PP(rawBuf.d_allocator_p); }
        }

      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING 'defaultConstruct'
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

        if (verbose) printf("\nTESTING 'defaultConstruct'"
                            "\n==========================\n");

        bslma::Allocator     *const DA = bslma::Default::allocator();
        bslma::TestAllocator        testAllocator(veryVeryVeryVerbose);
        bslma::TestAllocator *const TA = &testAllocator;
        int                         dummyAllocator;  // not a 'bslma' allocator
        int                  *const XA = &dummyAllocator;

        if (verbose) printf("Value and allocator testing.\n");

        //   MyClass                             Expected
        //      #      Operation                 Val Alloc
        //      == ============================= === =====
        TEST_OP(1, construct(objPtr, TA),  0, 0);
        TEST_OP(2, construct(objPtr, TA),  0, TA);
        TEST_OP(1, construct(objPtr, XA),  0, 0);
        TEST_OP(2, construct(objPtr, XA),  0, 0);

        // MyPair                                        first    second
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

        // MyPairA                                        first    second
        //            #_#   Operation                    Val Alloc Val Alloc
        //           == == ============================= === ===== === =====
        TEST_PAIRAOP(1, 2, construct(objPtr, TA),  0,  0,    0, TA);
        TEST_PAIRAOP(2, 2, construct(objPtr, TA),  0,  0,    0, TA);

        TEST_PAIRAOP(1, 2, construct(objPtr, XA),  0,  0,    0, 0);
        TEST_PAIRAOP(2, 2, construct(objPtr, XA),  0,  0,    0, 0);

        // MyPairAA                                       first    second
        //            #_#   Operation                    Val Alloc Val Alloc
        //           == == ============================= === ===== === =====
        TEST_PAIRAAOP(2, 2, construct(objPtr, TA),  0, TA,    0, TA);
        TEST_PAIRAAOP(2, 2, construct(objPtr, XA),  0,  0,    0, 0);

        if (verbose) printf("Exception testing.\n");

        if (verbose) printf("\t...constructing pair with allocators\n");

        BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
            MyClassDef    rawBuf[2];
            MyPairAA_4_4 *objPtr = (MyPairAA_4_4 *)rawBuf;
            Util::construct(objPtr, TA);
            ASSERT(0  == rawBuf[0].d_value);
            ASSERT(TA == rawBuf[0].d_allocator_p);
            ASSERT(0  == rawBuf[1].d_value);
            ASSERT(TA == rawBuf[1].d_allocator_p);
            objPtr->~MyPairAA_4_4();
        } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

        if (verbose) printf("\t...constructing pair with IsPair\n");

        // Testing that 'TA' is not passed to the pair constructor and the
        // default allocator is used instead.
        const bsls::Types::Int64 NUM_ALLOC1 = testAllocator.numAllocations();
        BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
            MyClassDef    rawBuf[2];
            MyPairBB_4_4 *objPtr = (MyPairBB_4_4 *)rawBuf;
            Util::construct(objPtr, TA);
            ASSERT(0  == rawBuf[0].d_value);
            ASSERT(DA == rawBuf[0].d_allocator_p);
            ASSERT(0  == rawBuf[1].d_value);
            ASSERT(DA == rawBuf[1].d_allocator_p);
            objPtr->~MyPairBB_4_4();
        } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
        ASSERT(NUM_ALLOC1 == testAllocator.numAllocations());

        if (verbose) printf("Trait selection testing.\n");
        {
            MyClassDef    rawBuf;
            MyClassFussy *objPtr = (MyClassFussy *) &rawBuf;
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
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) printf("\nBREATHING TEST"
                            "\n==============\n");

        MyClass1 v1(1);        ASSERT(1 == v1.value());
        MyClass2 v2(2);        ASSERT(2 == v2.value());

        MyClassDef             rawBuf;
        bslma::TestAllocator    testAllocator(veryVeryVeryVerbose);
        bslma::Allocator *const theAlloc = &testAllocator;

        // 'defaultConstruct' invokes default constructor, with defaulted
        // arguments, even if type does not take an allocator.

        memset(&rawBuf, 92, sizeof(rawBuf));
        Util::construct((MyClass1*) &rawBuf, theAlloc);
        ASSERT(0 == rawBuf.d_allocator_p);
        ASSERT(0 == rawBuf.d_value);

        // 'copyConstruct' invokes copy constructor, even if type does not take
        // an allocator.

        memset(&rawBuf, 92, sizeof(rawBuf));
        Util::construct((MyClass1*) &rawBuf, theAlloc, v1);
        ASSERT(0 == rawBuf.d_allocator_p);
        ASSERT(1 == rawBuf.d_value);

        // 'copyConstruct' invokes copy constructor, passing the allocator if
        // type takes an allocator.

        memset(&rawBuf, 92, sizeof(rawBuf));
        Util::construct((MyClass2*) &rawBuf, theAlloc, v2);
        ASSERT(theAlloc == rawBuf.d_allocator_p);
        ASSERT(2 == rawBuf.d_value);

        // 'construct' invokes constructor, even if type does not take an
        // allocator.

        memset(&rawBuf, 92, sizeof(rawBuf));
        Util::construct((MyClass1*) &rawBuf, theAlloc, 3);
        ASSERT(0 == rawBuf.d_allocator_p);
        ASSERT(3 == rawBuf.d_value);

        // 'destroy' invokes destructor ... with no particular constraints.

        memset(&rawBuf, 92, sizeof(rawBuf));
        DestructionUtil::destroy((MyClass1*) &rawBuf);
        ASSERT(0  == rawBuf.d_allocator_p);
        ASSERT(91 == rawBuf.d_value);

        // 'construct' invokes constructor, passing the allocator if type takes
        // an allocator.

        memset(&rawBuf, 92, sizeof(rawBuf));
        Util::construct((MyClass2*) &rawBuf, theAlloc, 4);
        ASSERT(theAlloc == rawBuf.d_allocator_p);
        ASSERT(4 == rawBuf.d_value);

        // 'destroy' invokes destructor ... with no particular constraints.

        memset(&rawBuf, 92, sizeof(rawBuf));
        DestructionUtil::destroy((MyClass2*) &rawBuf);
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
