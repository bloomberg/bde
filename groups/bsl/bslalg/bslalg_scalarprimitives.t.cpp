// bslalg_scalarprimitives.t.cpp                                      -*-C++-*-
#include <bslalg_scalarprimitives.h>

#include <bslalg_autoscalardestructor.h>
#include <bslalg_scalardestructionprimitives.h>

#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>
#include <bslma_testallocatorexception.h>
#include <bslma_usesbslmaallocator.h>

#include <bslmf_ispair.h>

#include <bsls_bsltestutil.h>
#include <bsls_objectbuffer.h>

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
// a fussy type that will modify its (internal and/or class-static) state upon
// invocation of the copy constructor, but not when copying bit-wise.
//-----------------------------------------------------------------------------
// [ 3] defaultConstruct(T *dst, *a);
// [ 4] copyConstruct(T *dst, const T& src, *a);
// [ 5] moveConstruct(T *dst, T& src, *a);
// [ 6] construct(T *dst, A[1--N]..., *a);
// [ 7] destructiveMove(T *dst, T *src, *a);
// [ 8] swap(T& lhs, T& rhs);
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
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
                                         // Print pointer identifier and value.

// Pragmas to silence format warnings, should be cleaned up before final commit
// BDE_VERIFY pragma: -AL01  // Strict aliasing concerns should be addressed
// BDE_VERIFY pragma: -CC01  // C style casts
// BDE_VERIFY pragma: -FD01  // Lots of functions need a clear contract
// BDE_VERIFY pragma: -IND01 // Indent issues
// BDE_VERIFY pragma: -IND03 // Text-alignment issues
// BDE_VERIFY pragma: -IND04 // Text-alignment issues

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

typedef BloombergLP::bslalg::ScalarPrimitives            Obj;
typedef BloombergLP::bslalg::ScalarDestructionPrimitives DestructionPrimitives;

typedef bslmf::MovableRefUtil MovUtl;

const int MOVED_FROM_VAL = 0x01d;

//=============================================================================
//                  CLASSES FOR TESTING USAGE EXAMPLES
//-----------------------------------------------------------------------------

                             // =================
                             // class my_ClassDef
                             // =================

struct my_ClassDef {
    // Data members that give my_ClassX size and alignment.  This class is a
    // simple aggregate, use to provide a common data layout to subsequent test
    // types.  There are no semantics associated with any of the members, in
    // particular the allocator pointer is not used directly by this aggregate
    // to allocate storage owned by this class.

    // DATA (exceptionally public, only in test driver)
    int                         d_value;
    int                        *d_data_p;
    bslma::Allocator           *d_allocator_p;
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
    // This 'class' is a simple type that does not take allocators.  Its
    // implementation owns a 'my_ClassDef' aggregate, but uses only the
    // 'd_value' data member, to support the 'value' attribute.  The
    // 'd_allocator_p' pointer is always initialized to a null pointer, while
    // the 'd_data_p' pointer is never initialized

    // DATA
    my_ClassDef d_def;

  public:
    // CREATORS
    explicit
    my_Class1(int v = 0) {
        d_def.d_value = v;
        d_def.d_allocator_p = 0;
    }
    my_Class1(const my_Class1& rhs) {
        d_def.d_value = rhs.d_def.d_value;
        d_def.d_allocator_p = 0;
    }

    my_Class1(bslmf::MovableRef<my_Class1> other) {                 // IMPLICIT
        my_Class1& otherRef = MovUtl::access(other);
        d_def.d_value = otherRef.d_def.d_value;
        otherRef.d_def.d_value = MOVED_FROM_VAL;
        d_def.d_allocator_p = 0;
    }

    ~my_Class1() {
        ASSERT(d_def.d_value != 91);
        d_def.d_value = 91;
        d_def.d_allocator_p = 0;
        dumpClassDefState(d_def);
    }

    my_Class1& operator=(const my_Class1& rhs) {
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
    // This 'class' supports the 'bslma::UsesBslmaAllocator' trait, providing
    // an allocator-aware version of every constructor.  While it holds an
    // allocator and has the expected allocator propagation properties of a
    // 'bslma::Allocator'-aware type, it does not actually allocate any memory.
    // In many ways, this is similar to a 'std::string' object that never grows
    // beyond the small string optimization.  The 'd_data_p' member of the
    // wrapper 'my_ClassDef' implementation type is never initialized, nor
    // used.  A signal value, 'MOVED_FROM_VAL', is used to detect an object in
    // a moved-from state.

    // DATA
    my_ClassDef d_def;

  public:
    // CREATORS
    explicit
    my_Class2(bslma::Allocator *a = 0) {
        d_def.d_value = 0;
        d_def.d_allocator_p = a;
    }
    explicit
    my_Class2(int v, bslma::Allocator *a = 0) {
        d_def.d_value = v;
        d_def.d_allocator_p = a;
    }
    my_Class2(const my_Class2& rhs, bslma::Allocator *a = 0) {
        d_def.d_value = rhs.d_def.d_value;
        d_def.d_allocator_p = a;
    }

    my_Class2(bslmf::MovableRef<my_Class2> other, bslma::Allocator *a = 0) {
                                                                    // IMPLICIT
        my_Class2& otherRef = MovUtl::access(other);
        d_def.d_value = otherRef.d_def.d_value;
        otherRef.d_def.d_value = MOVED_FROM_VAL;
        if (a) {
            d_def.d_allocator_p = a;
        }
        else {
            d_def.d_allocator_p = otherRef.d_def.d_allocator_p;
        }
    }

    ~my_Class2() {
        ASSERT(d_def.d_value != 92);
        d_def.d_value = 92;
        d_def.d_allocator_p = 0;
        dumpClassDefState(d_def);
    }

    // MANIPULATORS
    my_Class2& operator=(const my_Class2& rhs) {
        d_def.d_value = rhs.d_def.d_value;

        // do not touch allocator!

        return *this;
    }

    my_Class2& operator=(bslmf::MovableRef<my_Class2> rhs) {
        my_Class2& otherRef = MovUtl::access(rhs);
        d_def.d_value = otherRef.d_def.d_value;
        otherRef.d_def.d_value = MOVED_FROM_VAL;

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
struct UsesBslmaAllocator<my_Class2> : bsl::true_type { };

}  // close namespace bslma
}  // close enterprise namespace

                                 // ==========
                                 // my_Class2a
                                 // ==========

class my_Class2a {
    // This 'class' behaves the same as 'my_Class2' (allocator-aware type that
    // never actually allocates memory) except that it uses the
    // 'allocator_arg_t' idiom for passing an allocator to constructors.

    my_Class2 d_data;

  public:
    // CREATORS
    my_Class2a() : d_data() { }

    my_Class2a(bsl::allocator_arg_t, bslma::Allocator *a) : d_data(a) {}

    explicit
    my_Class2a(int v)  : d_data(v) {}

    my_Class2a(bsl::allocator_arg_t, bslma::Allocator *a, int v)
        : d_data(v, a) {}

    my_Class2a(const my_Class2a& rhs) : d_data(rhs.d_data) {}

    my_Class2a(bsl::allocator_arg_t  ,
               bslma::Allocator     *a,
               const my_Class2a&     rhs)
        : d_data(rhs.d_data, a) {}

    my_Class2a(bslmf::MovableRef<my_Class2a> rhs)                   // IMPLICIT
        : d_data(MovUtl::move(MovUtl::access(rhs).d_data)) {}

    my_Class2a(bsl::allocator_arg_t,
               bslma::Allocator              *a,
               bslmf::MovableRef<my_Class2a>  rhs)
        : d_data(MovUtl::move(MovUtl::access(rhs).d_data), a) {}

    // MANIPULATORS
    my_Class2a& operator=(const my_Class2a& rhs) {
        d_data.operator=(rhs.d_data);
        return *this;
    }

    my_Class2a& operator=(bslmf::MovableRef<my_Class2a> rhs) {
        d_data.operator=(MovUtl::move(MovUtl::access(rhs).d_data));
        return *this;
    }

    // ACCESSORS
    int value() const { return d_data.value(); }
};

// TRAITS
namespace BloombergLP {

namespace bslma {
template <> struct UsesBslmaAllocator<my_Class2a> : bsl::true_type { };
}  // close namespace bslma

namespace bslmf {
template <> struct UsesAllocatorArgT<my_Class2a> : bsl::true_type { };
}  // close namespace bslmf

}  // close enterprise namespace

                             // ===============
                             // class my_Class3
                             // ===============

class my_Class3 {
    // This 'class' takes allocators similarly to 'my_Class2', but does not
    // have an explicit move constructor (move calls the corresponding copy
    // operation).

    // DATA
    my_ClassDef d_def;

  public:
    // CREATORS
    explicit
    my_Class3(bslma::Allocator *a = 0) {
        d_def.d_value = 0;
        d_def.d_allocator_p = a;
    }
    explicit
    my_Class3(int v, bslma::Allocator *a = 0) {
        d_def.d_value = v;
        d_def.d_allocator_p = a;
    }
    my_Class3(const my_Class3& rhs, bslma::Allocator *a = 0) {
        d_def.d_value = rhs.d_def.d_value;
        d_def.d_allocator_p = a;
    }

    ~my_Class3() {
        ASSERT(d_def.d_value != 93);
        d_def.d_value = 93;
        d_def.d_allocator_p = 0;
        dumpClassDefState(d_def);
    }

    // MANIPULATORS
    my_Class3& operator=(const my_Class3& rhs) {
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
struct UsesBslmaAllocator<my_Class3> : bsl::true_type { };

}  // close namespace bslma
}  // close enterprise namespace

                             // ===================
                             // class my_ClassFussy
                             // ===================

class my_ClassFussy {
    // This 'class' does not take allocators and is trivially default
    // constructible, trivially destructible, and trivially copyable.  This
    // class keeps track of actual calls to its default and copy constructors
    // and its destructor.  'bslalg::ScalarPrimitives' should never invoke any
    // of those operations because it should use the trivial implementation,
    // instead (noop for default construction and destruction, 'memcpy' for
    // copy construction and move construction).

    // DATA
    my_ClassDef d_def;

  public:
    // CLASS DATA
    static int defaultConstructorInvocations;
    static int copyConstructorInvocations;
    static int moveConstructorInvocations;
    static int conversionConstructorInvocations;
    static int assignmentInvocations;
    static int destructorInvocations;

    // CLASS METHODS
    static void* operator new(std::size_t size)
        // Should never be invoked
    {
        BSLS_ASSERT_OPT(0);
        return ::operator new(size);
    }

    static void* operator new(std::size_t /* size */, void *ptr)
        // Should never be invoked
    {
        BSLS_ASSERT_OPT(0);
        return ptr;
    }

    static void operator delete(void * /* ptr */)
        // Should never be invoked
    {
        BSLS_ASSERT_OPT(0);
    }

    // CREATORS
    my_ClassFussy() {
        // Should never be invoked by bslalg_ScalarPrimitives.
        ++defaultConstructorInvocations;
    }
    // deliberately not explicit
    my_ClassFussy(int v) {                                          // IMPLICIT
        ++conversionConstructorInvocations;
        d_def.d_value = v;
        d_def.d_allocator_p = 0;
    }
    my_ClassFussy(const my_ClassFussy& /* rhs */) {
        // Should never be invoked by bslalg_ScalarPrimitives.
        ++copyConstructorInvocations;
    }
    my_ClassFussy(bslmf::MovableRef<my_ClassFussy> /* rhs */) {     // IMPLICIT
        // Should never be invoked by bslalg_ScalarPrimitives.
        ++moveConstructorInvocations;
    }
    ~my_ClassFussy() {
        // Should never be invoked by bslalg_ScalarPrimitives.
        ++destructorInvocations;

        // 'dumpClassDefState' is not called here, because a
        // default-constructed 'my_ClassFussy' object may leave 'd_def'
        // uninitialized.
    }

    // MANIPULATORS
    my_ClassFussy& operator=(const my_ClassFussy& /* rhs */) {
        // Should never be invoked by bslalg_ScalarPrimitives.
        ++assignmentInvocations;
        return *this;
    }
};

// CLASS DATA
int my_ClassFussy::defaultConstructorInvocations    = 0;
int my_ClassFussy::copyConstructorInvocations       = 0;
int my_ClassFussy::moveConstructorInvocations       = 0;
int my_ClassFussy::conversionConstructorInvocations = 0;
int my_ClassFussy::assignmentInvocations            = 0;
int my_ClassFussy::destructorInvocations            = 0;

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
    explicit
    my_Class4(bslma::Allocator *a = 0) {
        d_def.d_allocator_p = bslma::Default::allocator(a);
        d_def.d_data_p = (int*)(d_def.d_allocator_p)->allocate(sizeof(int));
        d_def.d_value = 0;
        *d_def.d_data_p = d_def.d_value;
    }

    explicit
    my_Class4(int v, bslma::Allocator *a = 0) {
        d_def.d_allocator_p = bslma::Default::allocator(a);
        d_def.d_data_p = (int*)(d_def.d_allocator_p)->allocate(sizeof(int));
        d_def.d_value = v;
        *d_def.d_data_p = d_def.d_value;
    }

    my_Class4(const my_Class4& rhs, bslma::Allocator *a = 0) {
        d_def.d_allocator_p = bslma::Default::allocator(a);
        d_def.d_data_p = (int*)(d_def.d_allocator_p)->allocate(sizeof(int));
        d_def.d_value = rhs.d_def.d_value;
        *d_def.d_data_p = d_def.d_value;
    }

    my_Class4(bslmf::MovableRef<my_Class4> other, bslma::Allocator *a = 0) {
                                                                    // IMPLICIT
        my_Class4& otherRef = MovUtl::access(other);
        if (a) {
            d_def.d_allocator_p = a;
        }
        else {
            d_def.d_allocator_p = otherRef.d_def.d_allocator_p;
        }
        d_def.d_data_p  = (int*)(d_def.d_allocator_p)->allocate(sizeof(int));
        d_def.d_value   = otherRef.d_def.d_value;
        *d_def.d_data_p = d_def.d_value;
        otherRef.d_def.d_value   = MOVED_FROM_VAL;
        *otherRef.d_def.d_data_p = otherRef.d_def.d_value;
    }

    ~my_Class4() {
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
    my_Class4& operator=(const my_Class4& rhs) {
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
struct UsesBslmaAllocator<my_Class4> : bsl::true_type { };

}  // close namespace bslma
}  // close enterprise namespace

                                 // =========
                                 // my_Class5
                                 // =========

class my_Class5 : public my_Class4 {
    // Class that takes allocators, and that actually allocates (for use in
    // exception testing).  This is the same as my_Class4, with the bitwise
    // moveable trait in addition.

  public:
    // CREATORS
    explicit
    my_Class5(bslma::Allocator *a = 0) : my_Class4(a) {}

    explicit
    my_Class5(int v, bslma::Allocator *a = 0)  : my_Class4(v, a) {}

    my_Class5(const my_Class4& rhs, bslma::Allocator *a = 0)        // IMPLICIT
        : my_Class4(rhs, a) {}

    my_Class5(const my_Class5& rhs, bslma::Allocator *a = 0)
        : my_Class4(rhs, a) {}

    my_Class5(bslmf::MovableRef<my_Class4> rhs, bslma::Allocator *a = 0)
        : my_Class4(MovUtl::move(rhs), a) {}                        // IMPLICIT

    my_Class5(bslmf::MovableRef<my_Class5> rhs, bslma::Allocator *a = 0)
        : my_Class4(MovUtl::move(rhs), a) {}                        // IMPLICIT

    // MANIPULATORS
    my_Class5& operator=(const my_Class5& rhs) {
        my_Class4::operator=(rhs);
        return *this;
    }
};

// TRAITS
namespace BloombergLP {

namespace bslma {
template <> struct UsesBslmaAllocator<my_Class5> : bsl::true_type { };
}  // close namespace bslma

namespace bslmf {
template <> struct IsBitwiseMoveable<my_Class5> : bsl::true_type { };
}  // close namespace bslmf

}  // close enterprise namespace

                             // =============
                             // class my_Pair
                             // =============
template <class T1, class T2>
struct my_Pair {
    // Test pair type without allocators.

    // TYPES
    typedef T1  first_type;
    typedef T2  second_type;

    // DATA (public for pair types)
    T1 first;
    T2 second;

    // CREATORS
    my_Pair() { };

    my_Pair(const T1& f, const T2& s) : first(f), second(s) { }

    my_Pair(const my_Pair& other) : first(other.first), second(other.second) {}

    template <class U1, class U2>
    my_Pair(const my_Pair<U1, U2>& other)                           // IMPLICIT
        : first(other.first), second(other.second) { }

    my_Pair(bslmf::MovableRef<my_Pair> other)                       // IMPLICIT
        : first(MovUtl::move(MovUtl::access(other).first))
        , second(MovUtl::move(MovUtl::access(other).second)) {}

    template <class U1, class U2>
    my_Pair(bslmf::MovableRef<my_Pair<U1, U2> > other)              // IMPLICIT
        : first(MovUtl::move(MovUtl::access(other).first))
        , second(MovUtl::move(MovUtl::access(other).second)) { }

    // MANIPULATORS
    my_Pair& operator=(const my_Pair& rhs) {
        first = rhs.first;
        second = rhs.second;
        return *this;
    }
};

namespace BloombergLP {
namespace bslma {

template <class T1, class T2>
struct UsesBslmaAllocator<my_Pair<T1, T2> > : bsl::false_type { };

}  // close namespace bslma
}  // close enterprise namespace

                               // ==============
                               // class my_PairA
                               // ==============

template <class T1, class T2>
struct my_PairA {
    // Test pair type with mixed allocator-aware and non-allocator-aware types.
    // Only 'T2' must use allocators.  We assume that the treatment of 'T1' and
    // 'T2' in the component is symmetric, and do not bother with the symmetric
    // test pair type.

    // TYPES
    typedef T1  first_type;
    typedef T2  second_type;

    // DATA (public for pair types)
    T1 first;
    T2 second;

    // CREATORS
    explicit
    my_PairA(bslma::Allocator *a = 0) : first(), second(a) {}

    my_PairA(const T1& f, const T2& s, bslma::Allocator *a = 0)
        : first(f), second(s, a) {}

    my_PairA(const my_PairA& other, bslma::Allocator *a = 0)
        : first(other.first), second(other.second, a) {}

    template <class U1, class U2>
    my_PairA(const my_PairA<U1, U2>& other, bslma::Allocator *a = 0)
        : first(other.first), second(other.second, a) {}            // IMPLICIT

    my_PairA(bslmf::MovableRef<my_PairA> other, bslma::Allocator *a = 0)
        : first(MovUtl::move(MovUtl::access(other).first))          // IMPLICIT
        , second(MovUtl::move(MovUtl::access(other).second), a) {}

    template <class U1, class U2>
    my_PairA(bslmf::MovableRef<my_PairA<U1, U2> >  other,
             bslma::Allocator                     *a = 0)           // IMPLICIT
        : first(MovUtl::move(MovUtl::access(other).first))
        , second(MovUtl::move(MovUtl::access(other).second), a) { }

    // MANIPULATORS
    my_PairA& operator=(const my_PairA& rhs) {
        first = rhs.first;
        second = rhs.second;
        return *this;
    }
};

namespace BloombergLP {
namespace bslma {

template <class T1, class T2>
struct UsesBslmaAllocator<my_PairA<T1, T2> > : bsl::true_type { };

}  // close namespace bslma
}  // close enterprise namespace

                              // ===============
                              // class my_PairAA
                              // ===============

template <class T1, class T2>
struct my_PairAA {
    // Test pair type with allocators.
    // Both 'T1' and 'T2' must use allocators.

    // TYPES
    typedef T1  first_type;
    typedef T2  second_type;

    // DATA (public for pair types)
    T1 first;
    T2 second;

    // CREATORS
    explicit
    my_PairAA(bslma::Allocator *a = 0) : first(a), second(a) {}

    my_PairAA(const T1& f, const T2& s, bslma::Allocator *a = 0)
        : first(f, a), second(s, a) {}

    my_PairAA(const my_PairAA& other, bslma::Allocator *a = 0)
        : first(other.first, a), second(other.second, a) {}

    template <class U1, class U2>
    my_PairAA(const my_PairAA<U1, U2>& other, bslma::Allocator *a = 0)
        : first(other.first, a), second(other.second, a) {}         // IMPLICIT

    my_PairAA(bslmf::MovableRef<my_PairAA> other, bslma::Allocator *a = 0)
        : first(MovUtl::move(MovUtl::access(other).first), a)       // IMPLICIT
        , second(MovUtl::move(MovUtl::access(other).second), a) {}

    template <class U1, class U2>
    my_PairAA(bslmf::MovableRef<my_PairAA<U1, U2> >  other,
              bslma::Allocator                      *a = 0)         // IMPLICIT
        : first(MovUtl::move(MovUtl::access(other).first), a)
        , second(MovUtl::move(MovUtl::access(other).second), a) { }

    // MANIPULATORS
    my_PairAA& operator=(const my_PairAA& rhs) {
        first = rhs.first;
        second = rhs.second;
        return *this;
    }
};

namespace BloombergLP {
namespace bslma {

template <class T1, class T2>
struct  UsesBslmaAllocator<my_PairAA<T1, T2> > : bsl::true_type  { };

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
    // contructors).

    // TYPES
    typedef T1  first_type;
    typedef T2  second_type;

    // DATA (public for pair types)
    T1 first;
    T2 second;

    // CREATORS
    my_PairBB() {}

    my_PairBB(const T1& f, const T2& s) : first(f), second(s) {}

    my_PairBB(const my_PairBB& other)
        : first(other.first), second(other.second) {}

    template <class U1, class U2>
    my_PairBB(const my_PairBB<U1, U2>& other)                       // IMPLICIT
        : first(other.first), second(other.second) {}

    my_PairBB(bslmf::MovableRef<my_PairBB> other)                   // IMPLICIT
        : first(MovUtl::move(MovUtl::access(other).first))
        , second(MovUtl::move(MovUtl::access(other).second)) {}

    template <class U1, class U2>
    my_PairBB(bslmf::MovableRef<my_PairBB<U1, U2> > other)          // IMPLICIT
        : first(MovUtl::move(MovUtl::access(other).first))
        , second(MovUtl::move(MovUtl::access(other).second)) { }

    // MANIPULATORS
    my_PairBB& operator=(const my_PairBB& rhs) {
        first = rhs.first;
        second = rhs.second;
        return *this;
    }
};

namespace BloombergLP {
namespace bslmf {

template <class T1, class T2>
struct IsPair<my_PairBB<T1, T2> > : bsl::true_type { };

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
    std::memset(&rawBuf, 92, sizeof(rawBuf));                                 \
    Type *objPtr = (Type*) &rawBuf;                                           \
    pre(&rawBuf);                                                             \
    Obj:: op ;                                                                \
    post(&rawBuf);                                                            \
    ASSERT(EXP_VAL == rawBuf.d_value);                                        \
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
    std::memset(rawBuf, 92, sizeof(rawBuf));                                  \
    Type *objPtr = (Type*) rawBuf;                                            \
    pre(rawBuf);                                                              \
    Obj:: op ;                                                                \
    post(rawBuf);                                                             \
    ASSERT(EXP_VAL0 == rawBuf[0].d_value);                                    \
    ASSERT(EXP_ALLOC0 == rawBuf[0].d_allocator_p);                            \
    ASSERT(EXP_VAL1 == rawBuf[1].d_value);                                    \
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

#define TEST_MV(typeNum, op, expVal, expAlloc) {                              \
    bslma::TestAllocator fromA;                                               \
    my_Class ## typeNum fromObj(expVal);                                      \
    TEST_OP(typeNum, op, expVal, expAlloc);                                   \
    ASSERT(isMovedFrom(fromObj));                                             \
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
    ASSERT(isMovedFrom(fromObj.first));                                       \
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

void pre(const my_ClassDef* p)
    // Do nothing.  This function can be taken advantage of to debug the above
    // macros by setting a breakpoint to examine state prior to executing the
    // main operation under test.
{
    (void) p;  // remove unused variable warning
}

void post(const my_ClassDef* p)
    // Do nothing.  This function can be taken advantage of to debug the above
    // macros by setting a breakpoint to examine state after executing the
    // main operation under test.
{
    (void) p;  // remove unused variable warning
}

                       // ======================
                       // class ConstructTestArg
                       // ======================

template <int ID>
class ConstructTestArg {
    // This very simple 'struct' is used purely to disambiguate types in
    // passing parameters to 'construct' due to the fact that
    // 'ConstructTestArg<ID1>' is a different type than 'ConstructTestArg<ID2>'
    // if 'ID1 != ID2'.  This class does not take an allocator.

  public:
    // PUBLIC DATA FOR TEST DRIVER ONLY
    const int d_value;

    // CREATORS
    ConstructTestArg(int value = -1);                               // IMPLICIT
        // Create an object having the specified 'value'.
};

// CREATORS
template <int ID>
ConstructTestArg<ID>::ConstructTestArg(int value)
    : d_value(value)
{
}

                       // ==============================
                       // class ConstructTestTypeNoAlloc
                       // ==============================

class ConstructTestTypeNoAlloc {
    // This 'struct' provides a test class capable of holding up to 14
    // parameters of types 'ConstructTestArg[1--14]'.  By default, a
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
    typedef ConstructTestArg<1>  Arg1;
    typedef ConstructTestArg<2>  Arg2;
    typedef ConstructTestArg<3>  Arg3;
    typedef ConstructTestArg<4>  Arg4;
    typedef ConstructTestArg<5>  Arg5;
    typedef ConstructTestArg<6>  Arg6;
    typedef ConstructTestArg<7>  Arg7;
    typedef ConstructTestArg<8>  Arg8;
    typedef ConstructTestArg<9>  Arg9;
    typedef ConstructTestArg<10> Arg10;
    typedef ConstructTestArg<11> Arg11;
    typedef ConstructTestArg<12> Arg12;
    typedef ConstructTestArg<13> Arg13;
    typedef ConstructTestArg<14> Arg14;
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
    explicit
    ConstructTestTypeNoAlloc(
                Arg1  a1  = N1, Arg2  a2  = N1, Arg3  a3  = N1,
                Arg4  a4  = N1, Arg5  a5  = N1, Arg6  a6  = N1, Arg7  a7  = N1,
                Arg8  a8  = N1, Arg9  a9  = N1, Arg10 a10 = N1, Arg11 a11 = N1,
                Arg12 a12 = N1, Arg13 a13 = N1, Arg14 a14 = N1)
        : d_a1(a1), d_a2(a2), d_a3(a3), d_a4(a4), d_a5(a5), d_a6(a6), d_a7(a7)
        , d_a8(a8), d_a9(a9), d_a10(a10), d_a11(a11), d_a12(a12), d_a13(a13)
        , d_a14(a14) {}
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

                       // =============================
                       // class ConstructTestTypeAlloc
                       // =============================

class ConstructTestTypeAlloc {
    // This class provides a test class capable of holding up to 14 parameters
    // of types 'ConstructTestArg[1--14]'.  By default, a
    // 'ConstructTestTypeAlloc' is constructed with nil ('N1') values, but
    // instances can be constructed with actual values (e.g., for creating
    // expected values).  This class intentionally *does* take an allocator.

    // PRIVATE TYPES
    typedef ConstructTestArg<1>  Arg1;
    typedef ConstructTestArg<2>  Arg2;
    typedef ConstructTestArg<3>  Arg3;
    typedef ConstructTestArg<4>  Arg4;
    typedef ConstructTestArg<5>  Arg5;
    typedef ConstructTestArg<6>  Arg6;
    typedef ConstructTestArg<7>  Arg7;
    typedef ConstructTestArg<8>  Arg8;
    typedef ConstructTestArg<9>  Arg9;
    typedef ConstructTestArg<10> Arg10;
    typedef ConstructTestArg<11> Arg11;
    typedef ConstructTestArg<12> Arg12;
    typedef ConstructTestArg<13> Arg13;
    typedef ConstructTestArg<14> Arg14;
        // Argument types for shortcut.

    enum {
        N1 = -1   // default value for all private data
    };

  public:
    // DATA (exceptionally public, only within a test driver)
    bslma::Allocator *d_allocator;
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
    explicit
    ConstructTestTypeAlloc(bslma::Allocator *allocator = 0)
        : d_allocator(allocator) {}
    ConstructTestTypeAlloc(const ConstructTestTypeAlloc&  other,
                           bslma::Allocator              *allocator = 0)
        : d_allocator(allocator)
        , d_a1 (other.d_a1),  d_a2 (other.d_a2),  d_a3 (other.d_a3)
        , d_a4 (other.d_a4),  d_a5 (other.d_a5),  d_a6 (other.d_a6)
        , d_a7 (other.d_a7),  d_a8 (other.d_a8),  d_a9 (other.d_a9)
        , d_a10(other.d_a10), d_a11(other.d_a11), d_a12(other.d_a12)
        , d_a13(other.d_a13), d_a14(other.d_a14) {}
    explicit
    ConstructTestTypeAlloc(Arg1  a1, bslma::Allocator *allocator = 0)
        : d_allocator(allocator)
        , d_a1(a1) {}
    ConstructTestTypeAlloc(Arg1  a1, Arg2  a2, bslma::Allocator *allocator = 0)
        : d_allocator(allocator)
        , d_a1(a1), d_a2(a2) {}
    ConstructTestTypeAlloc(Arg1  a1, Arg2  a2, Arg3  a3,
                           bslma::Allocator *allocator = 0)
        : d_allocator(allocator)
        , d_a1(a1), d_a2(a2), d_a3(a3) {}
    ConstructTestTypeAlloc(Arg1  a1, Arg2  a2, Arg3  a3, Arg4  a4,
                           bslma::Allocator *allocator = 0)
        : d_allocator(allocator)
        , d_a1(a1), d_a2(a2), d_a3(a3), d_a4(a4) {}
    ConstructTestTypeAlloc(Arg1  a1, Arg2  a2, Arg3  a3, Arg4  a4, Arg5  a5,
                           bslma::Allocator *allocator = 0)
        : d_allocator(allocator)
        , d_a1(a1), d_a2(a2), d_a3(a3), d_a4(a4), d_a5(a5) {}
    ConstructTestTypeAlloc(Arg1  a1, Arg2  a2, Arg3  a3, Arg4  a4, Arg5  a5,
                           Arg6  a6, bslma::Allocator *allocator = 0)
        : d_allocator(allocator)
        , d_a1(a1), d_a2(a2), d_a3(a3), d_a4(a4), d_a5(a5), d_a6(a6) {}
    ConstructTestTypeAlloc(Arg1  a1, Arg2  a2, Arg3  a3, Arg4  a4, Arg5  a5,
                           Arg6  a6, Arg7  a7, bslma::Allocator *allocator = 0)
        : d_allocator(allocator)
        , d_a1(a1), d_a2(a2), d_a3(a3), d_a4(a4), d_a5(a5), d_a6(a6)
        , d_a7(a7) {}
    ConstructTestTypeAlloc(Arg1  a1, Arg2  a2, Arg3  a3, Arg4  a4, Arg5  a5,
                           Arg6  a6, Arg7  a7, Arg8  a8,
                           bslma::Allocator *allocator = 0)
        : d_allocator(allocator)
        , d_a1(a1), d_a2(a2), d_a3(a3), d_a4(a4), d_a5(a5), d_a6(a6)
        , d_a7(a7), d_a8(a8) {}
    ConstructTestTypeAlloc(Arg1  a1, Arg2  a2, Arg3  a3, Arg4  a4, Arg5  a5,
                           Arg6  a6, Arg7  a7, Arg8  a8, Arg9  a9,
                           bslma::Allocator *allocator = 0)
        : d_allocator(allocator)
        , d_a1(a1), d_a2(a2), d_a3(a3), d_a4(a4), d_a5(a5), d_a6(a6)
        , d_a7(a7), d_a8(a8), d_a9(a9) {}
    ConstructTestTypeAlloc(Arg1  a1, Arg2  a2, Arg3  a3, Arg4  a4, Arg5  a5,
                           Arg6  a6, Arg7  a7, Arg8  a8, Arg9  a9, Arg10 a10,
                           bslma::Allocator *allocator = 0)
        : d_allocator(allocator)
        , d_a1 (a1), d_a2 (a2), d_a3 (a3), d_a4 (a4), d_a5 (a5), d_a6 (a6)
        , d_a7 (a7), d_a8 (a8), d_a9 (a9)
        , d_a10(a10) {}
    ConstructTestTypeAlloc(Arg1  a1, Arg2  a2, Arg3  a3, Arg4  a4, Arg5  a5,
                           Arg6  a6, Arg7  a7, Arg8  a8, Arg9  a9, Arg10 a10,
                           Arg11 a11, bslma::Allocator *allocator = 0)
        : d_allocator(allocator)
        , d_a1 (a1), d_a2 (a2), d_a3 (a3), d_a4 (a4), d_a5 (a5), d_a6 (a6)
        , d_a7 (a7), d_a8 (a8), d_a9 (a9)
        , d_a10(a10), d_a11(a11) {}
    ConstructTestTypeAlloc(Arg1  a1, Arg2  a2, Arg3  a3, Arg4  a4, Arg5  a5,
                           Arg6  a6, Arg7  a7, Arg8  a8, Arg9  a9, Arg10 a10,
                           Arg11 a11, Arg12 a12,
                           bslma::Allocator *allocator = 0)
        : d_allocator(allocator)
        , d_a1 (a1), d_a2 (a2), d_a3 (a3), d_a4 (a4 ), d_a5 (a5 ), d_a6 (a6 )
        , d_a7 (a7), d_a8 (a8), d_a9 (a9), d_a10(a10), d_a11(a11), d_a12(a12)
        {}
    ConstructTestTypeAlloc(Arg1  a1,  Arg2  a2,  Arg3  a3, Arg4  a4, Arg5  a5,
                           Arg6  a6,  Arg7  a7,  Arg8  a8, Arg9  a9, Arg10 a10,
                           Arg11 a11, Arg12 a12, Arg13 a13,
                           bslma::Allocator *allocator = 0)
        : d_allocator(allocator)
        , d_a1 (a1), d_a2 (a2), d_a3 (a3), d_a4 (a4 ), d_a5 (a5 ), d_a6 (a6 )
        , d_a7 (a7), d_a8 (a8), d_a9 (a9), d_a10(a10), d_a11(a11), d_a12(a12)
        , d_a13(a13) {}
    ConstructTestTypeAlloc(Arg1  a1,  Arg2  a2,  Arg3  a3, Arg4  a4, Arg5  a5,
                           Arg6  a6,  Arg7  a7,  Arg8  a8, Arg9  a9, Arg10 a10,
                           Arg11 a11, Arg12 a12, Arg13 a13, Arg14 a14,
                           bslma::Allocator *allocator = 0)
        : d_allocator(allocator)
        , d_a1 (a1), d_a2 (a2), d_a3 (a3), d_a4 (a4 ), d_a5 (a5 ), d_a6 (a6 )
        , d_a7 (a7), d_a8 (a8), d_a9 (a9), d_a10(a10), d_a11(a11), d_a12(a12)
        , d_a13(a13), d_a14(a14) {}
};

// TRAITS
namespace BloombergLP {
namespace bslma {

template <>
struct UsesBslmaAllocator<ConstructTestTypeAlloc> : bsl::true_type { };

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

                       // ================================
                       // class ConstructTestTypeAllocArgT
                       // ================================

class ConstructTestTypeAllocArgT {
    // This class provides a test class capable of holding up to 14 parameters
    // of types 'ConstructTestArg[1--14]'.  By default, a
    // 'ConstructTestTypeAllocArgT' is constructed with nil ('N1') values, but
    // instances can be constructed with actual values (e.g., for creating
    // expected values).  This class takes an allocator using the
    // 'allocator_arg_t' protocol.

    // PRIVATE TYPES
    typedef ConstructTestArg<1>  Arg1;
    typedef ConstructTestArg<2>  Arg2;
    typedef ConstructTestArg<3>  Arg3;
    typedef ConstructTestArg<4>  Arg4;
    typedef ConstructTestArg<5>  Arg5;
    typedef ConstructTestArg<6>  Arg6;
    typedef ConstructTestArg<7>  Arg7;
    typedef ConstructTestArg<8>  Arg8;
    typedef ConstructTestArg<9>  Arg9;
    typedef ConstructTestArg<10> Arg10;
    typedef ConstructTestArg<11> Arg11;
    typedef ConstructTestArg<12> Arg12;
    typedef ConstructTestArg<13> Arg13;
    typedef ConstructTestArg<14> Arg14;
        // Argument types for shortcut.

    enum {
        N1 = -1   // default value for all private data
    };

  public:
    // DATA (exceptionally public, only within a test driver)
    bslma::Allocator *d_allocator;
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
    explicit
    ConstructTestTypeAllocArgT(Arg1  a1  = N1, Arg2  a2 = N1,  Arg3  a3  = N1,
                               Arg4  a4  = N1, Arg5  a5 = N1,  Arg6  a6  = N1,
                               Arg7  a7  = N1, Arg8  a8 = N1,  Arg9  a9  = N1,
                               Arg10 a10 = N1, Arg11 a11 = N1, Arg12 a12 = N1,
                               Arg13 a13 = N1, Arg14 a14 = N1)
        : d_allocator(0)
        , d_a1 (a1 ), d_a2 (a2 ), d_a3 (a3), d_a4 (a4 ), d_a5 (a5 ), d_a6 (a6 )
        , d_a7 (a7 ), d_a8 (a8 ), d_a9 (a9), d_a10(a10), d_a11(a11), d_a12(a12)
        , d_a13(a13), d_a14(a14) {}

    ConstructTestTypeAllocArgT(bsl::allocator_arg_t       ,
                               bslma::Allocator     *alloc,
                               Arg1  a1  = N1, Arg2  a2 = N1,  Arg3  a3  = N1,
                               Arg4  a4  = N1, Arg5  a5 = N1,  Arg6  a6  = N1,
                               Arg7  a7  = N1, Arg8  a8 = N1,  Arg9  a9  = N1,
                               Arg10 a10 = N1, Arg11 a11 = N1, Arg12 a12 = N1,
                               Arg13 a13 = N1, Arg14 a14 = N1)
        : d_allocator(alloc)
        , d_a1 (a1 ), d_a2 (a2 ), d_a3 (a3), d_a4 (a4 ), d_a5 (a5 ), d_a6 (a6 )
        , d_a7 (a7 ), d_a8 (a8 ), d_a9 (a9), d_a10(a10), d_a11(a11), d_a12(a12)
        , d_a13(a13), d_a14(a14) {}
};

// TRAITS
namespace BloombergLP {
namespace bslma {

template <>
struct UsesBslmaAllocator<ConstructTestTypeAllocArgT> : bsl::true_type { };

}  // close namespace bslma

namespace bslmf {

template <>
struct UsesAllocatorArgT<ConstructTestTypeAllocArgT> : bsl::true_type { };

}  // close namespace bslmf
}  // close enterprise namespace

// FREE OPERATORS
bool operator==(const ConstructTestTypeAllocArgT& lhs,
                const ConstructTestTypeAllocArgT& rhs)
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
    ConstructTestTypeNoAlloc EXP expArgs ;                                    \
    bsls::ObjectBuffer<ConstructTestTypeNoAlloc> rawBuf;                      \
    ConstructTestTypeNoAlloc  *objPtr =  rawBuf.address();                    \
    ConstructTestTypeNoAlloc&  mX     = *objPtr;                              \
    const ConstructTestTypeNoAlloc& X =  mX;                                  \
    std::memset(&mX, 92, sizeof mX);                                          \
    Obj:: op ;                                                                \
    ASSERT(EXP == X);                                                         \
  }

#define TEST_CONSTRUCTA(op, expArgs, alloc)                                   \
  {                                                                           \
    /* Expects allocator at end of argument list */                           \
    ConstructTestTypeAlloc EXP expArgs ;                                      \
    bsls::ObjectBuffer<ConstructTestTypeAlloc> rawBuf;                        \
    ConstructTestTypeAlloc  *objPtr =  rawBuf.address();                      \
    ConstructTestTypeAlloc&  mX     = *objPtr;                                \
    const ConstructTestTypeAlloc& X = *objPtr;                                \
    std::memset(&mX, 92, sizeof mX);                                          \
    Obj:: op ;                                                                \
    ASSERT(EXP == X);                                                         \
    ASSERT(alloc == X.d_allocator);                                           \
  }                                                                           \
  {                                                                           \
    /* Expects allocator after 'allocator_arg_t' tag */                       \
    ConstructTestTypeAllocArgT EXP expArgs ;                                  \
    bsls::ObjectBuffer<ConstructTestTypeAllocArgT> rawBuf;                    \
    ConstructTestTypeAllocArgT  *objPtr =  rawBuf.address();                  \
    ConstructTestTypeAllocArgT&  mX     = *objPtr;                            \
    const ConstructTestTypeAllocArgT& X = *objPtr;                            \
    std::memset(&mX, 92, sizeof mX);                                          \
    Obj:: op ;                                                                \
    ASSERT(EXP == X);                                                         \
    ASSERT(alloc == X.d_allocator);                                           \
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

const my_Class1     V1(1);
const my_Class2     V2(2);
const my_Class2a    V2A(0x2a);
const my_Class3     V3(3);
const my_ClassFussy VF(3);
const my_Class4     V4(4);

const my_Pair_1_1 PV1V1(V1, V1);
const my_Pair_1_2 PV1V2(V1, V2);
const my_Pair_2_1 PV2V1(V2, V1);
const my_Pair_2_2 PV2V2(V2, V2);
const my_Pair_4_4 PV4V4(V4, V4);

const my_PairA_1_2  PAV1V2(V1, V2);
const my_PairA_2_2  PAV2V2(V2, V2);

const my_PairAA_2_2 PAAV2V2(V2, V2);

const my_PairAA_4_4 PAAV4V4(V4, V4);

const my_PairBB_4_4 PBBV4V4(V4, V4);

ConstructTestArg<1>    VA1(1);
ConstructTestArg<2>    VA2(2);
ConstructTestArg<3>    VA3(3);
ConstructTestArg<4>    VA4(4);
ConstructTestArg<5>    VA5(5);
ConstructTestArg<6>    VA6(6);
ConstructTestArg<7>    VA7(7);
ConstructTestArg<8>    VA8(8);
ConstructTestArg<9>    VA9(9);
ConstructTestArg<10>   VA10(10);
ConstructTestArg<11>   VA11(11);
ConstructTestArg<12>   VA12(12);
ConstructTestArg<13>   VA13(13);
ConstructTestArg<14>   VA14(14);

template <class TYPE>
inline bool isMovedFrom(const TYPE& x)
    // Return 'true' if the specified 'x' is in a moved-from state.  This
    // template is for classes with explicit move constructors whose 'value()'
    // member returns 'MOVED_FROM_VAL' if the class is in a moved-from state.
    // Classes without explicit move constructors should overload this function
    // to return true if 'x' holds the expected initial value.
{
    // For classes with explicit move constructors, the moved-from object holds
    // 'MOVED_FROM_VAL'.
    return x.value() == MOVED_FROM_VAL;
}

inline bool isMovedFrom(const my_Class3& x)
    // Return 'true' if the specified 'x' is in the moved-from state, which is
    // assumed to be the same as its initial value, which is, in turn, assumed
    // to be 'V3'.
{
    return x.value() == V3.value();
}


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
        // TESTING 'swap'
        //
        // Concerns:
        //   o That swapping a value with itself does not destroy the value.
        //   o That swap is an involution (swapping twice is a no-op).
        //   o That values of mutually convertible types can be swapped.
        //   o That values are swapped properly, but not allocators.
        //   o That swap uses memcpy whenever appropriate.
        //   o That swap is exception-safe.
        //
        // Plan:  Simply create pairs of objects of different types, and swap
        //   them.  Verify that the resulting values are as expected, that
        //   exceptions do not leak memory.  Using a fussy type that has the
        //   bitwise moveable trait, ensure that neither copy construction nor
        //   assignments are invoked.
        //
        // Testing:
        //   swap(T& lhs, T& rhs);
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING 'swap'"
                            "\n==============\n");

        bslma::TestAllocator               testAllocator(veryVeryVeryVerbose);
        bslma::TestAllocator *const TA  = &testAllocator;
        bslma::TestAllocator               testAllocator2(veryVeryVeryVerbose);
        bslma::TestAllocator *const TA1 = &testAllocator;
        bslma::TestAllocator *const TA2 = &testAllocator2;

        int              dummyAllocator;  // Dummy, non-bslma allocator
        int *const XA = &dummyAllocator;

        (void) TA;
        (void) XA;

        if (verbose) printf("Value and allocator testing.\n");
        {
            my_Class1 obj1(1), obj2(2);
            Obj::swap(obj1, obj2);
            ASSERT(2 == obj1.value());
            ASSERT(1 == obj2.value());
            Obj::swap(obj1, obj1);
            ASSERT(2 == obj1.value());
        }
        {
            my_ClassDef  rawBuf[2];
            my_Class2   *objPtr = (my_Class2 *)&rawBuf[0];
            Obj::construct(&objPtr[0], 1, TA1);
            Obj::construct(&objPtr[1], 2, TA2);
            Obj::swap(objPtr[0], objPtr[1]);  // should not swap allocators
            ASSERT(2   == rawBuf[0].d_value);
            ASSERT(TA1 == rawBuf[0].d_allocator_p);
            ASSERT(1   == rawBuf[1].d_value);
            ASSERT(TA2 == rawBuf[1].d_allocator_p);
            Obj::swap(objPtr[0], objPtr[0]);
            ASSERT(2   == rawBuf[0].d_value);
            DestructionPrimitives::destroy(&objPtr[0]);
            DestructionPrimitives::destroy(&objPtr[1]);
        }
        {
            my_ClassDef  rawBuf[2];
            my_Class2a  *objPtr = (my_Class2a *)&rawBuf[0];
            Obj::construct(&objPtr[0], 1, TA1);
            Obj::construct(&objPtr[1], 2, TA2);
            Obj::swap(objPtr[0], objPtr[1]);  // should not swap allocators
            ASSERT(2   == rawBuf[0].d_value);
            ASSERT(TA1 == rawBuf[0].d_allocator_p);
            ASSERT(1   == rawBuf[1].d_value);
            ASSERT(TA2 == rawBuf[1].d_allocator_p);
            Obj::swap(objPtr[0], objPtr[0]);
            ASSERT(2   == rawBuf[0].d_value);
            DestructionPrimitives::destroy(&objPtr[0]);
            DestructionPrimitives::destroy(&objPtr[1]);
        }
        {
            my_ClassDef  rawBuf[2];
            my_Class4   *objPtr = (my_Class4 *)&rawBuf[0];
            Obj::construct(&objPtr[0], 1, TA1);
            Obj::construct(&objPtr[1], 2, TA2);
            Obj::swap(objPtr[0], objPtr[1]); // should not use bitwise moveable
            ASSERT(2   == rawBuf[0].d_value);
            ASSERT(TA1 == rawBuf[0].d_allocator_p);
            ASSERT(1   == rawBuf[1].d_value);
            ASSERT(TA2 == rawBuf[1].d_allocator_p);
            Obj::swap(objPtr[0], objPtr[0]);
            ASSERT(2   == rawBuf[0].d_value);
            DestructionPrimitives::destroy(&objPtr[0]);
            DestructionPrimitives::destroy(&objPtr[1]);
        }
        {
            my_ClassDef  rawBuf[2];
            my_Class5   *objPtr = (my_Class5 *)&rawBuf[0];
            Obj::construct(&objPtr[0], 1, TA1);
            Obj::construct(&objPtr[1], 2, TA2);
            Obj::swap(objPtr[0], objPtr[1]); // should not use bitwise moveable
            ASSERT(2   == rawBuf[0].d_value);
            ASSERT(TA1 == rawBuf[0].d_allocator_p);
            ASSERT(1   == rawBuf[1].d_value);
            ASSERT(TA2 == rawBuf[1].d_allocator_p);
            Obj::swap(objPtr[0], objPtr[0]);
            ASSERT(2   == rawBuf[0].d_value);
            DestructionPrimitives::destroy(&objPtr[0]);
            DestructionPrimitives::destroy(&objPtr[1]);
        }

        if (verbose) printf("Heterogeneous swap testing.\n");
        {
            my_ClassDef  rawBuf[2];
            my_Class4   *objPtr4 = (my_Class4 *)&rawBuf[0];
            my_Class5   *objPtr5 = (my_Class5 *)&rawBuf[1];
            Obj::construct(objPtr4, 1, TA1);
            Obj::construct(objPtr5, 2, TA2);
            Obj::swap(*objPtr4, *objPtr5); // should not use bitwise moveable
            ASSERT(2   == rawBuf[0].d_value);
            ASSERT(TA1 == rawBuf[0].d_allocator_p);
            ASSERT(1   == rawBuf[1].d_value);
            ASSERT(TA2 == rawBuf[1].d_allocator_p);
            Obj::swap(*objPtr5, *objPtr4); // should not use bitwise moveable
            ASSERT(1   == rawBuf[0].d_value);
            ASSERT(TA1 == rawBuf[0].d_allocator_p);
            ASSERT(2   == rawBuf[1].d_value);
            ASSERT(TA2 == rawBuf[1].d_allocator_p);
            DestructionPrimitives::destroy(objPtr4);
            DestructionPrimitives::destroy(objPtr5);
        }

        if (verbose) printf("Exception testing.\n");

        BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
            my_ClassDef  rawBuf[2];
            my_Class4   *objPtr = (my_Class4 *)&rawBuf[0];
            Obj::construct(&objPtr[0], 1, TA);
            bslalg::AutoScalarDestructor<my_Class4> guard1(&objPtr[0]);
            Obj::construct(&objPtr[1], 2, TA);
            bslalg::AutoScalarDestructor<my_Class4> guard2(&objPtr[1]);
            Obj::swap(objPtr[0], objPtr[1]);
            guard1.release();
            guard2.release();
            ASSERT(2 == rawBuf[0].d_value);
            ASSERT(1 == rawBuf[1].d_value);
            DestructionPrimitives::destroy(&objPtr[0]);
            DestructionPrimitives::destroy(&objPtr[1]);
        } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

        if (verbose) printf("Trait selection testing.\n");
        {
            my_ClassDef    rawBuf[2];
            my_ClassFussy *objPtr = (my_ClassFussy *) &rawBuf[0];
            Obj::construct(&objPtr[0], 1, XA);
            Obj::construct(&objPtr[1], 2, XA);
            const int CCI = my_ClassFussy::copyConstructorInvocations;
            const int AI  = my_ClassFussy::assignmentInvocations;
            Obj::swap(objPtr[0], objPtr[1]);
            ASSERT(CCI == my_ClassFussy::copyConstructorInvocations);
            ASSERT(AI  == my_ClassFussy::assignmentInvocations);
            ASSERT(2 == rawBuf[0].d_value);
            ASSERT(1 == rawBuf[1].d_value);
            if (veryVerbose) {
                P_(rawBuf[0].d_value); PP(rawBuf[0].d_allocator_p);
                P_(rawBuf[1].d_value); PP(rawBuf[1].d_allocator_p);
            }
            Obj::swap(objPtr[0], objPtr[0]);
            ASSERT(2   == rawBuf[0].d_value);
            DestructionPrimitives::destroy(&objPtr[0]);
            DestructionPrimitives::destroy(&objPtr[1]);
        }

      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TESTING 'destructiveMove'
        //
        // Concerns:
        //   o That the move constructor properly forwards the allocator
        //     when appropriate.
        //   o That the copy constructor uses memcpy when appropriate.
        //
        // Plan: The test plan is identical to copyConstruct, except that we
        //   operate the move from a temporary copy created with the (already
        //   tested) 'copyConstruct' so as not to affect the constants of this
        //   test driver.  We are also careful (for the exception testing) that
        //   this temporary is not destroyed if it has been moved successfully.
        //   Finally, we verify that the original object has been destroyed.
        //
        // Testing:
        //   destructiveMove(T *dst, T *src, *a);
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING 'destructiveMove'"
                            "\n=========================\n");

        bslma::TestAllocator testAllocator(veryVeryVeryVerbose);
        bslma::TestAllocator *const TA = &testAllocator;
        int                  dummyAllocator;  // Dummy, non-bslma allocator
        int                  *const XA = &dummyAllocator;

        if (verbose) printf("Value and allocator testing.\n");

        // my_Class #  Operation                            Val Alloc
        // ==========  ==================================== === =====
        {
            my_ClassDef rawBuf;
            my_Class1 *srcPtr = (my_Class1 *)&rawBuf;
            Obj::copyConstruct(srcPtr, V1, TA);
            TEST_OP(1, destructiveMove(objPtr, srcPtr, TA),  1, 0);
            ASSERTV(rawBuf.d_value, 91 == rawBuf.d_value);
            ASSERT(0  == rawBuf.d_allocator_p);
        }
        {
            my_ClassDef rawBuf;
            my_Class2 *srcPtr = (my_Class2 *)&rawBuf;
            Obj::copyConstruct(srcPtr, V2, TA);
            TEST_OP(2, destructiveMove(objPtr, srcPtr, TA),  2, TA);
            ASSERTV(rawBuf.d_value, 92 == rawBuf.d_value);
            ASSERT(0  == rawBuf.d_allocator_p);
        }
        {
            my_ClassDef rawBuf;
            my_Class2a *srcPtr = (my_Class2a *)&rawBuf;
            Obj::copyConstruct(srcPtr, V2A, TA);
            TEST_OP(2a, destructiveMove(objPtr, srcPtr, TA), 0x2a, TA);
            ASSERTV(rawBuf.d_value, 92 == rawBuf.d_value);
            ASSERT(0  == rawBuf.d_allocator_p);
        }
        {
            my_ClassDef rawBuf;
            my_Class3 *srcPtr = (my_Class3 *)&rawBuf;
            Obj::copyConstruct(srcPtr, V3, TA);
            TEST_OP(3, destructiveMove(objPtr, srcPtr, TA),  3, TA);
            ASSERTV(rawBuf.d_value, 93 == rawBuf.d_value);
            ASSERT(0  == rawBuf.d_allocator_p);
        }
        {
            my_ClassDef rawBuf;
            my_Class1 *srcPtr = (my_Class1 *)&rawBuf;
            Obj::copyConstruct(srcPtr, V1, TA);
            TEST_OP(1, destructiveMove(objPtr, srcPtr, XA),  1, 0);
            ASSERTV(rawBuf.d_value, 91 == rawBuf.d_value);
            ASSERT(0  == rawBuf.d_allocator_p);
        }
        {
            my_ClassDef rawBuf;
            my_Class2 *srcPtr = (my_Class2 *)&rawBuf;
            Obj::copyConstruct(srcPtr, V2, TA);
            // Must use 'TA' so that behavior is the same in C++98 mode (copy,
            // uses default allocator) and C++11 mode (move, copies '*srcPtr'
            // allocator).
            TEST_OP(2, destructiveMove(objPtr, srcPtr, TA),  2, TA);
            ASSERTV(rawBuf.d_value, 92 == rawBuf.d_value);
            ASSERT(0  == rawBuf.d_allocator_p);
        }
        {
            my_ClassDef rawBuf;
            my_Class2a *srcPtr = (my_Class2a *)&rawBuf;
            Obj::copyConstruct(srcPtr, V2A, TA);
            // Must use 'TA' so that behavior is the same in C++98 mode (copy,
            // uses default allocator) and C++11 mode (move, copies '*srcPtr'
            // allocator).
            TEST_OP(2a, destructiveMove(objPtr, srcPtr, TA), 0x2a, TA);
            ASSERTV(rawBuf.d_value, 92 == rawBuf.d_value);
            ASSERT(0  == rawBuf.d_allocator_p);
        }
        {
            my_ClassDef rawBuf;
            my_Class3 *srcPtr = (my_Class3 *)&rawBuf;
            Obj::copyConstruct(srcPtr, V3, TA);
            // Must use 'TA' so that behavior is the same in C++98 mode (copy,
            // uses default allocator) and C++11 mode (move, copies '*srcPtr'
            // allocator).
            TEST_OP(3, destructiveMove(objPtr, srcPtr, TA),  3, TA);
            ASSERTV(rawBuf.d_value, 93 == rawBuf.d_value);
            ASSERT(0  == rawBuf.d_allocator_p);
        }

        if (verbose) printf("Exception testing.\n");

        if (verbose) printf("\t...pair with allocators\n");

        BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
            my_ClassDef rawBuf[4];
            my_PairAA_4_4 *srcPtr = (my_PairAA_4_4 *)&rawBuf[0];
            Obj::copyConstruct(srcPtr, PAAV4V4, TA);
            bslalg::AutoScalarDestructor<my_PairAA_4_4> guard(srcPtr);
            my_PairAA_4_4 *objPtr = (my_PairAA_4_4 *)&rawBuf[2];
            Obj::destructiveMove(objPtr, srcPtr, TA);
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
            my_ClassFussy *srcPtr = (my_ClassFussy *) &rawBuf[0];
            Obj::copyConstruct(srcPtr, VF, XA);
            my_ClassFussy *objPtr = (my_ClassFussy *) &rawBuf[1];
            const int CCI = my_ClassFussy::copyConstructorInvocations;
            const int DI  = my_ClassFussy::destructorInvocations;
            Obj::destructiveMove(objPtr, srcPtr, XA);
            ASSERT(CCI == my_ClassFussy::copyConstructorInvocations);
            ASSERT(DI  == my_ClassFussy::destructorInvocations);
            ASSERT(3   == rawBuf[0].d_value);
            ASSERT(0   == rawBuf[0].d_allocator_p);
            ASSERT(3   == rawBuf[1].d_value);
            ASSERT(0   == rawBuf[1].d_allocator_p);
            if (veryVerbose) {
                P_(rawBuf[0].d_value); PP(rawBuf[0].d_allocator_p);
                P_(rawBuf[1].d_value); PP(rawBuf[1].d_allocator_p);
            }
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
        //    traits and to the type (bslma::Allocator* or void*).
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
        bslma::TestAllocator testAllocator(veryVeryVeryVerbose);
        bslma::TestAllocator *const TA = &testAllocator;
        int                  dummyAllocator;  // Dummy, non-bslma allocator
        int                  *const XA = &dummyAllocator;

        if (verbose) printf("TEST_CONSTRUCT (without allocators).\n");

        // OP  = construct(&ConstructTestArg, VA[1--N], TA)
        // EXP = ConstructTestArg(VA[1--N])
        // ---   -------------------------------------------------
        TEST_CONSTRUCT(construct(objPtr, TA),                            // OP
                       /* no ctor arg list */                            // EXP
                      );

        TEST_CONSTRUCT(construct(objPtr, VA1, TA),                       // OP
                       (VA1)                                             // EXP
                      );

        TEST_CONSTRUCT(construct(objPtr, VA1, VA2, TA),                  // OP
                       (VA1, VA2)                                        // EXP
                      );

        TEST_CONSTRUCT(construct(objPtr, VA1, VA2, VA3, TA),             // OP
                       (VA1, VA2, VA3)                                   // EXP
                      );

        TEST_CONSTRUCT(construct(objPtr, VA1, VA2, VA3, VA4, TA),        // OP
                       (VA1, VA2, VA3, VA4)                              // EXP
                      );

        TEST_CONSTRUCT(construct(objPtr, VA1, VA2, VA3, VA4, VA5,
                                         TA),                            // OP
                       (VA1, VA2, VA3, VA4, VA5)                         // EXP
                      );

        TEST_CONSTRUCT(construct(objPtr, VA1, VA2, VA3, VA4, VA5,
                                         VA6, TA),                       // OP
                       (VA1, VA2, VA3, VA4, VA5, VA6)                    // EXP
                      );

        TEST_CONSTRUCT(construct(objPtr, VA1, VA2, VA3, VA4, VA5,
                                         VA6, VA7, TA),                  // OP
                       (VA1, VA2, VA3, VA4, VA5, VA6, VA7)               // EXP
                      );

        TEST_CONSTRUCT(construct(objPtr, VA1, VA2, VA3, VA4, VA5,
                                         VA6, VA7, VA8, TA),             // OP
                       (VA1, VA2, VA3, VA4, VA5, VA6, VA7, VA8)          // EXP
                      );

        TEST_CONSTRUCT(construct(objPtr, VA1, VA2, VA3, VA4, VA5,
                                         VA6, VA7, VA8, VA9, TA),        // OP
                       (VA1, VA2, VA3, VA4, VA5, VA6, VA7, VA8,
                        VA9)                                             // EXP
                      );

        TEST_CONSTRUCT(construct(objPtr, VA1, VA2, VA3, VA4, VA5,
                                         VA6, VA7, VA8, VA9, VA10,
                                         TA),                            // OP
                       (VA1, VA2, VA3, VA4, VA5, VA6, VA7,
                        VA8, VA9, VA10)                                  // EXP
                      );

        TEST_CONSTRUCT(construct(objPtr, VA1, VA2, VA3, VA4, VA5,
                                         VA6, VA7, VA8, VA9, VA10,
                                         VA11, TA),                      // OP
                       (VA1, VA2, VA3, VA4, VA5, VA6, VA7, VA8,
                        VA9, VA10, VA11)                                 // EXP
                      );

        TEST_CONSTRUCT(construct(objPtr, VA1, VA2, VA3, VA4, VA5,
                                         VA6, VA7, VA8, VA9, VA10,
                                         VA11, VA12, TA),                // OP
                       (VA1, VA2, VA3, VA4, VA5, VA6, VA7, VA8,
                        VA9, VA10, VA11, VA12)                           // EXP
                      );

        TEST_CONSTRUCT(construct(objPtr, VA1, VA2, VA3, VA4, VA5,
                                         VA6, VA7, VA8, VA9, VA10,
                                         VA11, VA12, VA13, TA),          // OP
                       (VA1, VA2, VA3, VA4, VA5, VA6, VA7, VA8,
                        VA9, VA10, VA11, VA12, VA13)                     // EXP
                      );

        TEST_CONSTRUCT(construct(objPtr, VA1, VA2, VA3, VA4, VA5,
                                         VA6, VA7, VA8, VA9, VA10,
                                         VA11, VA12, VA13, VA14, TA),    // OP
                       (VA1, VA2, VA3, VA4, VA5, VA6, VA7, VA8,
                        VA9, VA10, VA11, VA12, VA13, VA14)               // EXP
                      );

        if (verbose) printf("TEST_CONSTRUCTA (with bslma::Allocator*).\n");

        // OP  = construct(&ConstructTestArg, VA[1--N], TA)
        // EXP = ConstructTestArg(VA[1--N])
        // ---   -------------------------------------------------
        TEST_CONSTRUCTA(construct(objPtr, TA),                         // OP
                        /* no ctor arg list */,                        // EXP
                        TA);                                           // ALLOC

        TEST_CONSTRUCTA(construct(objPtr, VA1, TA),                    // OP
                        (VA1),                                         // EXP
                        TA);                                           // ALLOC

        TEST_CONSTRUCTA(construct(objPtr, VA1, VA2, TA),               // OP
                        (VA1, VA2),                                    // EXP
                        TA);                                           // ALLOC

        TEST_CONSTRUCTA(construct(objPtr, VA1, VA2, VA3, TA),          // OP
                        (VA1, VA2, VA3),                               // EXP
                        TA);                                           // ALLOC

        TEST_CONSTRUCTA(construct(objPtr, VA1, VA2, VA3, VA4, TA),     // OP
                        (VA1, VA2, VA3, VA4),                          // EXP
                        TA);                                           // ALLOC

        TEST_CONSTRUCTA(construct(objPtr, VA1, VA2, VA3, VA4, VA5, TA),// OP
                        (VA1, VA2, VA3, VA4, VA5),                     // EXP
                        TA);                                           // ALLOC

        TEST_CONSTRUCTA(construct(objPtr, VA1, VA2, VA3, VA4, VA5,
                                         VA6, TA),                     // OP
                        (VA1, VA2, VA3, VA4, VA5, VA6),                // EXP
                        TA);                                           // ALLOC

        TEST_CONSTRUCTA(construct(objPtr, VA1, VA2, VA3, VA4, VA5,
                                          VA6, VA7, TA),                // OP
                        (VA1, VA2, VA3, VA4, VA5, VA6, VA7),            // EXP
                        TA);                                           // ALLOC

        TEST_CONSTRUCTA(construct(objPtr, VA1, VA2, VA3, VA4, VA5,
                                         VA6, VA7, VA8, TA),           // OP
                        (VA1, VA2, VA3, VA4, VA5, VA6, VA7, VA8),      // EXP
                        TA);                                           // ALLOC

        TEST_CONSTRUCTA(construct(objPtr, VA1, VA2, VA3, VA4, VA5,
                                         VA6, VA7, VA8, VA9, TA),      // OP
                        (VA1, VA2, VA3, VA4, VA5, VA6, VA7, VA8,
                         VA9),                                         // EXP
                        TA);                                           // ALLOC

        TEST_CONSTRUCTA(construct(objPtr, VA1, VA2, VA3, VA4, VA5,
                                         VA6, VA7, VA8, VA9, VA10,
                                         TA),                          // OP
                        (VA1, VA2, VA3, VA4, VA5, VA6, VA7,
                         VA8, VA9, VA10),                              // EXP
                        TA);                                           // ALLOC

        TEST_CONSTRUCTA(construct(objPtr, VA1, VA2, VA3, VA4, VA5,
                                         VA6, VA7, VA8, VA9, VA10,
                                         VA11, TA),                    // OP
                        (VA1, VA2, VA3, VA4, VA5, VA6, VA7, VA8,
                         VA9, VA10, VA11),                             // EXP
                        TA);                                           // ALLOC

        TEST_CONSTRUCTA(construct(objPtr, VA1, VA2, VA3, VA4, VA5,
                                         VA6, VA7, VA8, VA9, VA10,
                                         VA11, VA12, TA),              // OP
                        (VA1, VA2, VA3, VA4, VA5, VA6, VA7, VA8,
                         VA9, VA10, VA11, VA12),                       // EXP
                        TA);                                           // ALLOC

        TEST_CONSTRUCTA(construct(objPtr, VA1, VA2, VA3, VA4, VA5,
                                         VA6, VA7, VA8, VA9, VA10,
                                         VA11, VA12, VA13, TA),        // OP
                        (VA1, VA2, VA3, VA4, VA5, VA6, VA7, VA8,
                         VA9, VA10, VA11, VA12, VA13),                 // EXP
                        TA);                                           // ALLOC
        TEST_CONSTRUCTA(construct(objPtr, VA1, VA2, VA3, VA4, VA5,
                                         VA6, VA7, VA8, VA9, VA10,
                                         VA11, VA12, VA13, VA14, TA),  // OP
                        (VA1, VA2, VA3, VA4, VA5, VA6, VA7, VA8,
                         VA9, VA10, VA11, VA12, VA13, VA14),           // EXP
                        TA);                                           // ALLOC

        if (verbose) printf("TEST_CONSTRUCTA (with void *).\n");

        // OP  = construct(&ConstructTestArg, VA[1--N], XA)
        // EXP = ConstructTestArg(VA[1--N])
        // ---   -------------------------------------------------
        TEST_CONSTRUCTA(construct(objPtr, XA),                         // OP
                        /* no ctor arg list */,                        // EXP
                        0);                                            // ALLOC

        TEST_CONSTRUCTA(construct(objPtr, VA1, XA),                    // OP
                        (VA1),                                         // EXP
                        0);                                            // ALLOC

        TEST_CONSTRUCTA(construct(objPtr, VA1, VA2, XA),               // OP
                        (VA1, VA2),                                    // EXP
                        0);                                            // ALLOC

        TEST_CONSTRUCTA(construct(objPtr, VA1, VA2, VA3, XA),          // OP
                        (VA1, VA2, VA3),                               // EXP
                        0);                                            // ALLOC

        TEST_CONSTRUCTA(construct(objPtr, VA1, VA2, VA3, VA4, XA),     // OP
                        (VA1, VA2, VA3, VA4),                          // EXP
                        0);                                            // ALLOC

        TEST_CONSTRUCTA(construct(objPtr, VA1, VA2, VA3, VA4, VA5, XA),// OP
                        (VA1, VA2, VA3, VA4, VA5),                     // EXP
                        0);                                            // ALLOC

        TEST_CONSTRUCTA(construct(objPtr, VA1, VA2, VA3, VA4, VA5,
                                         VA6, XA),                     // OP
                        (VA1, VA2, VA3, VA4, VA5, VA6),                // EXP
                        0);                                            // ALLOC

        TEST_CONSTRUCTA(construct(objPtr, VA1, VA2, VA3, VA4, VA5,
                                         VA6, VA7, XA),                // OP
                        (VA1, VA2, VA3, VA4, VA5, VA6, VA7),           // EXP
                        0);                                            // ALLOC

        TEST_CONSTRUCTA(construct(objPtr, VA1, VA2, VA3, VA4, VA5,
                                         VA6, VA7, VA8, XA),           // OP
                        (VA1, VA2, VA3, VA4, VA5, VA6, VA7, VA8),      // EXP
                        0);                                            // ALLOC

        TEST_CONSTRUCTA(construct(objPtr, VA1, VA2, VA3, VA4, VA5,
                                         VA6, VA7, VA8, VA9, XA),      // OP
                        (VA1, VA2, VA3, VA4, VA5, VA6, VA7, VA8,
                         VA9),                                         // EXP
                        0);                                            // ALLOC

        TEST_CONSTRUCTA(construct(objPtr, VA1, VA2, VA3, VA4, VA5,
                                         VA6, VA7, VA8, VA9, VA10,
                                         XA),                          // OP
                        (VA1, VA2, VA3, VA4, VA5, VA6, VA7,
                         VA8, VA9, VA10),                              // EXP
                        0);                                            // ALLOC

        TEST_CONSTRUCTA(construct(objPtr, VA1, VA2, VA3, VA4, VA5,
                                         VA6, VA7, VA8, VA9, VA10,
                                         VA11, XA),                    // OP
                        (VA1, VA2, VA3, VA4, VA5, VA6, VA7, VA8,
                         VA9, VA10, VA11),                             // EXP
                        0);                                            // ALLOC

        TEST_CONSTRUCTA(construct(objPtr, VA1, VA2, VA3, VA4, VA5,
                                         VA6, VA7, VA8, VA9, VA10,
                                         VA11, VA12, XA),              // OP
                        (VA1, VA2, VA3, VA4, VA5, VA6, VA7, VA8,
                         VA9, VA10, VA11, VA12),                       // EXP
                        0);                                            // ALLOC

        TEST_CONSTRUCTA(construct(objPtr, VA1, VA2, VA3, VA4, VA5,
                                         VA6, VA7, VA8, VA9, VA10,
                                         VA11, VA12, VA13, XA),        // OP
                        (VA1, VA2, VA3, VA4, VA5, VA6, VA7, VA8,
                         VA9, VA10, VA11, VA12, VA13),                 // EXP
                        0);                                            // ALLOC

        TEST_CONSTRUCTA(construct(objPtr, VA1, VA2, VA3, VA4, VA5,
                                         VA6, VA7, VA8, VA9, VA10,
                                         VA11, VA12, VA13, VA14, XA),  // OP
                        (VA1, VA2, VA3, VA4, VA5, VA6, VA7, VA8,
                         VA9, VA10, VA11, VA12, VA13, VA14),           // EXP
                        0);                                            // ALLOC

        if (verbose) printf("Exception testing\n");

        if (verbose) printf("\t...constructing pair with allocators\n");

        BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
            my_ClassDef rawBuf[2];
            my_PairAA_4_4 *objPtr = (my_PairAA_4_4 *)rawBuf;
            Obj::construct(objPtr, PAAV4V4, TA);
            ASSERT(4  == rawBuf[0].d_value);
            ASSERT(TA == rawBuf[0].d_allocator_p);
            ASSERT(4  == rawBuf[1].d_value);
            ASSERT(TA == rawBuf[1].d_allocator_p);
            objPtr->~my_PairAA_4_4();
        } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

        BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
            my_ClassDef rawBuf[2];
            my_PairAA_4_4 *objPtr = (my_PairAA_4_4 *)rawBuf;
            Obj::construct(objPtr, V4, V4, TA);
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
            Obj::construct(objPtr, PBBV4V4, TA);
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
            Obj::construct(objPtr, V4, V4, TA);
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
            std::memset(&rawBuf, 92, sizeof rawBuf);
            const int CCI = my_ClassFussy::copyConstructorInvocations;
            Obj::construct(objPtr, VF, (bslma::Allocator*)TA);
            ASSERT(CCI == my_ClassFussy::copyConstructorInvocations);
            ASSERT(3 == rawBuf.d_value);
            ASSERT(0 == rawBuf.d_allocator_p);
            if (veryVerbose) { P_(rawBuf.d_value); PP(rawBuf.d_allocator_p); }
        }

        {
            my_ClassDef rawBuf;
            my_ClassFussy *objPtr = (my_ClassFussy *) &rawBuf;
            std::memset(&rawBuf, 92, sizeof rawBuf);
            const int CCI = my_ClassFussy::copyConstructorInvocations;
            const int CVI = my_ClassFussy::conversionConstructorInvocations;
            const int VF  = 3;
            Obj::construct(objPtr, VF, (bslma::Allocator*)TA);
            ASSERT(CCI == my_ClassFussy::copyConstructorInvocations);
            ASSERT(CVI <  my_ClassFussy::conversionConstructorInvocations);
            ASSERT(3 == rawBuf.d_value);
            ASSERT(0 == rawBuf.d_allocator_p);
            if (veryVerbose) { P_(rawBuf.d_value); PP(rawBuf.d_allocator_p); }
        }

      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING 'moveConstruct'
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

        if (verbose) printf("\nTESTING 'moveConstruct'"
                            "\n=======================\n");

        bslma::Allocator     *const DA = bslma::Default::allocator();
        bslma::TestAllocator testAllocator(veryVeryVerbose);
        bslma::TestAllocator *const TA = &testAllocator;
        int                  dummyAllocator;  // Dummy, non-bslma allocator
        int                  *const XA = &dummyAllocator;

        // my_Class                                   Expected
        //      #  Operation                          Val Alloc
        //      =  ================================== === =====
        TEST_MV(1, moveConstruct(objPtr, fromObj, TA),  1, 0);
        TEST_MV(2, moveConstruct(objPtr, fromObj, TA),  2, TA  );
        TEST_MV(1, moveConstruct(objPtr, fromObj, XA),  1, 0);
        TEST_MV(2, moveConstruct(objPtr, fromObj, XA),  2, 0);

        if (verbose) printf("Exception testing.\n");

        if (verbose) printf("\t...pair with allocators\n");

        BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
            my_ClassDef rawBuf[2];
            my_PairAA_4_4 *objPtr = (my_PairAA_4_4 *)rawBuf;
            my_PairAA_4_4 fromObj(PAAV4V4);
            Obj::moveConstruct(objPtr, fromObj, TA);
            ASSERT(4  == rawBuf[0].d_value);
            ASSERT(TA == rawBuf[0].d_allocator_p);
            ASSERT(4  == rawBuf[1].d_value);
            ASSERT(TA == rawBuf[1].d_allocator_p);
            ASSERT(isMovedFrom(fromObj.first));
            ASSERT(isMovedFrom(fromObj.second));
            objPtr->~my_PairAA_4_4();

            std::memset(rawBuf, 91, sizeof(rawBuf));
            fromObj = PAAV4V4;
            Obj::moveConstruct(objPtr, fromObj, XA);
            ASSERT(4  == rawBuf[0].d_value);
            ASSERT(DA == rawBuf[0].d_allocator_p);
            ASSERT(4  == rawBuf[1].d_value);
            ASSERT(DA == rawBuf[1].d_allocator_p);
            ASSERT(isMovedFrom(fromObj.first));
            ASSERT(isMovedFrom(fromObj.second));
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
            Obj::moveConstruct(objPtr, fromObj, TA);
            ASSERT(4  == rawBuf[0].d_value);
            ASSERT(DA == rawBuf[0].d_allocator_p);
            ASSERT(4  == rawBuf[1].d_value);
            ASSERT(DA == rawBuf[1].d_allocator_p);
            ASSERT(isMovedFrom(fromObj.first));
            ASSERT(isMovedFrom(fromObj.second));
            objPtr->~my_PairBB_4_4();

            fromObj = PBBV4V4;
            Obj::moveConstruct(objPtr, fromObj, XA);
            ASSERT(4  == rawBuf[0].d_value);
            ASSERT(DA == rawBuf[0].d_allocator_p);
            ASSERT(4  == rawBuf[1].d_value);
            ASSERT(DA == rawBuf[1].d_allocator_p);
            ASSERT(isMovedFrom(fromObj.first));
            ASSERT(isMovedFrom(fromObj.second));
            objPtr->~my_PairBB_4_4();
        } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
        ASSERT(NUM_ALLOC1 == testAllocator.numAllocations());

        if (verbose) printf("Trait selection testing.\n");
        {
            my_ClassDef rawBuf;
            my_ClassFussy *objPtr = (my_ClassFussy *) &rawBuf;
            my_ClassFussy fromObj(3);
            std::memset(&rawBuf, 92, sizeof rawBuf);
            const int CCI = my_ClassFussy::moveConstructorInvocations;
            Obj::moveConstruct(objPtr, fromObj, XA);
            ASSERT(CCI == my_ClassFussy::moveConstructorInvocations);
            ASSERT(3 == rawBuf.d_value);
            ASSERT(0 == rawBuf.d_allocator_p);
            if (veryVerbose) { P_(rawBuf.d_value); PP(rawBuf.d_allocator_p); }
        }

      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING 'copyConstruct'
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

        if (verbose) printf("\nTESTING 'copyConstruct'"
                            "\n=======================\n");

        bslma::Allocator     *const DA = bslma::Default::allocator();
        bslma::TestAllocator  testAllocator(veryVeryVeryVerbose);
        bslma::TestAllocator *const TA = &testAllocator;
        int                   dummyAllocator;  // Dummy, non-bslma allocator
        int                  *const XA = &dummyAllocator;

        // my_Class                               Expected
        //      #  Operation                      Val Alloc
        //      =  ============================== === =====
        TEST_OP(1, copyConstruct(objPtr, V1, TA),  1, 0);
        TEST_OP(2, copyConstruct(objPtr, V2, TA),  2, TA);
        TEST_OP(3, copyConstruct(objPtr, V3, TA),  3, TA);
        TEST_OP(1, copyConstruct(objPtr, V1, XA),  1, 0);
        TEST_OP(2, copyConstruct(objPtr, V2, XA),  2, 0);
        TEST_OP(3, copyConstruct(objPtr, V3, XA),  3, 0);

        if (verbose) printf("Exception testing.\n");

        if (verbose) printf("\t...pair with allocators\n");

        BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
            my_ClassDef rawBuf[2];
            my_PairAA_4_4 *objPtr = (my_PairAA_4_4 *)rawBuf;
            Obj::copyConstruct(objPtr, PAAV4V4, TA);
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
            Obj::copyConstruct(objPtr, PBBV4V4, TA);
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
            std::memset(&rawBuf, 92, sizeof rawBuf);
            const int CCI = my_ClassFussy::copyConstructorInvocations;
            Obj::copyConstruct(objPtr, VF, XA);
            ASSERT(CCI == my_ClassFussy::copyConstructorInvocations);
            ASSERT(3 == rawBuf.d_value);
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
        bslma::TestAllocator testAllocator(veryVeryVeryVerbose);
        bslma::TestAllocator *const TA = &testAllocator;
        int                  dummyAllocator;  // dummy, non-bslma allocator
        int                  *const XA = &dummyAllocator;

        if (verbose) printf("Value and allocator testing.\n");

        //   my_Class                             Expected
        //      #      Operation                 Val Alloc
        //      == ============================= === =====
        TEST_OP(1, defaultConstruct(objPtr, TA),  0, 0);
        TEST_OP(2, defaultConstruct(objPtr, TA),  0, TA);
        TEST_OP(1, defaultConstruct(objPtr, XA),  0, 0);
        TEST_OP(2, defaultConstruct(objPtr, XA),  0, 0);

        // my_Pair                                        first    second
        //           #_#   Operation                    Val Alloc Val Alloc
        //          == == ============================= === ===== === =====
        TEST_PAIROP(1, 1, defaultConstruct(objPtr, TA),  0, 0,     0,  0);
        TEST_PAIROP(1, 2, defaultConstruct(objPtr, TA),  0, 0,     0,  0);
        TEST_PAIROP(2, 1, defaultConstruct(objPtr, TA),  0, 0,     0,  0);
        TEST_PAIROP(2, 2, defaultConstruct(objPtr, TA),  0, 0,     0,  0);

        TEST_PAIROP(1, 1, defaultConstruct(objPtr, XA),  0, 0,     0,  0);
        TEST_PAIROP(1, 2, defaultConstruct(objPtr, XA),  0, 0,     0,  0);
        TEST_PAIROP(2, 1, defaultConstruct(objPtr, XA),  0, 0,     0,  0);
        TEST_PAIROP(2, 2, defaultConstruct(objPtr, XA),  0, 0,     0,  0);

        // my_PairA                                        first    second
        //            #_#   Operation                    Val Alloc Val Alloc
        //           == == ============================= === ===== === =====
        TEST_PAIRAOP(1, 2, defaultConstruct(objPtr, TA),  0,  0,    0, TA);
        TEST_PAIRAOP(2, 2, defaultConstruct(objPtr, TA),  0,  0,    0, TA);

        TEST_PAIRAOP(1, 2, defaultConstruct(objPtr, XA),  0,  0,    0, 0);
        TEST_PAIRAOP(2, 2, defaultConstruct(objPtr, XA),  0,  0,    0, 0);

        // my_PairAA                                       first    second
        //            #_#   Operation                    Val Alloc Val Alloc
        //           == == ============================= === ===== === =====
        TEST_PAIRAAOP(2, 2, defaultConstruct(objPtr, TA),  0, TA,    0, TA);
        TEST_PAIRAAOP(2, 2, defaultConstruct(objPtr, XA),  0,  0,    0, 0);

        if (verbose) printf("Exception testing.\n");

        if (verbose) printf("\t...constructing pair with allocators\n");

        BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
            my_ClassDef rawBuf[2];
            my_PairAA_4_4 *objPtr = (my_PairAA_4_4 *)rawBuf;
            Obj::defaultConstruct(objPtr, TA);
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
            Obj::defaultConstruct(objPtr, TA);
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
            std::memset(&rawBuf, 92, sizeof rawBuf);
            const int DCI = my_ClassFussy::defaultConstructorInvocations;
            Obj::defaultConstruct(objPtr, XA);
            ASSERT(DCI == my_ClassFussy::defaultConstructorInvocations);
            ASSERT(0 == rawBuf.d_value);
            ASSERT(0 == rawBuf.d_allocator_p);
            if (veryVerbose) { P_(rawBuf.d_value); PP(rawBuf.d_allocator_p); }
        }

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING TEST APPARATUS
        //
        // Concerns:
        //: 1 The test apparatus must work properly.
        //
        // Plan:
        //: 1 Simply test the values of each constant defined.
        //
        // Testing:
        //   TEST APPARATUS
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING TEST APPARATUS"
                            "\n======================\n");

        ASSERT(1 == V1.value());
        ASSERT(2 == V2.value());
        ASSERT(3 == V3.value());

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
        //: 1 Simply instantiate the templates in very simple examples,
        //:   constructing or destroying an object stored in some buffer.  No
        //:   thorough testing is performed, beyond simply asserting the call
        //:   was forwarded properly by examining the value of the buffer
        //:   storing the object.
        //
        // Testing:
        //   BREATHING TEST
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

        std::memset(&rawBuf, 92, sizeof(rawBuf));
        bslalg::ScalarPrimitives::defaultConstruct((my_Class1*) &rawBuf,
                                                   theAlloc);
        ASSERT(0 == rawBuf.d_allocator_p);
        ASSERT(0 == rawBuf.d_value);

        // 'copyConstruct' invokes copy constructor, even if type does not take
        // an allocator.

        std::memset(&rawBuf, 92, sizeof(rawBuf));
        bslalg::ScalarPrimitives::copyConstruct((my_Class1*) &rawBuf,
                                                v1, theAlloc);
        ASSERT(0 == rawBuf.d_allocator_p);
        ASSERT(1 == rawBuf.d_value);

        // 'copyConstruct' invokes copy constructor, passing the allocator if
        // type takes an allocator.

        std::memset(&rawBuf, 92, sizeof(rawBuf));
        bslalg::ScalarPrimitives::copyConstruct((my_Class2*) &rawBuf,
                                                v2, theAlloc);
        ASSERT(theAlloc == rawBuf.d_allocator_p);
        ASSERT(2 == rawBuf.d_value);

        // 'construct' invokes constructor, even if type does not take an
        // allocator.

        std::memset(&rawBuf, 92, sizeof(rawBuf));
        bslalg::ScalarPrimitives::construct((my_Class1*) &rawBuf,
                                            3, theAlloc);
        ASSERT(0 == rawBuf.d_allocator_p);
        ASSERT(3 == rawBuf.d_value);

        // 'destroy' invokes destructor ... with no particular constraints.

        std::memset(&rawBuf, 92, sizeof(rawBuf));
        DestructionPrimitives::destroy((my_Class1*) &rawBuf);
        ASSERT(0  == rawBuf.d_allocator_p);
        ASSERT(91 == rawBuf.d_value);

        // 'construct' invokes constructor, passing the allocator if type takes
        // an allocator.

        std::memset(&rawBuf, 92, sizeof(rawBuf));
        bslalg::ScalarPrimitives::construct((my_Class2*) &rawBuf,
                                            4, theAlloc);
        ASSERT(theAlloc == rawBuf.d_allocator_p);
        ASSERT(4 == rawBuf.d_value);

        // 'destroy' invokes destructor ... with no particular constraints.

        std::memset(&rawBuf, 92, sizeof(rawBuf));
        DestructionPrimitives::destroy((my_Class2*) &rawBuf);
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
