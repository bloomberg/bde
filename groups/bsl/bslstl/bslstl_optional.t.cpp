// bslstl_optional.t.cpp                                           -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include "bslstl_optional.h"
#include <bsls_bsltestutil.h>
#include <bslstl_string.h>

#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>
#include <bslma_testallocatormonitor.h>

// A list of disabled tests :
//
//BSLSTL_OPTIONAL_TEST_BAD_VALUE
//      Tests in this group check that value category and cv qualification
//      of the return value from value() member function is correct.
//
//BSLSTL_OPTIONAL_TEST_BAD_EQUAL_NONOPT
//      Tests in this group check that assignments is not possible if the
//      'value_type' is not both assignable and constructible from the source
//      type.
//
//BSLSTL_OPTIONAL_TEST_BAD_IL_EMPLACE
//      Tests in this group check that emplace is not possible for const
//      optional types.
//
//BSLSTL_OPTIONAL_TEST_BAD_EQUAL_CONST
//      Tests in this group check that assignments is not possible to
//      an optional of const type or to a const qualified optional

using namespace BloombergLP;
using namespace bsl;
// ============================================================================
//                     STANDARD BDE ASSERT TEST FUNCTION
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

#ifdef BSLS_COMPILERFEATURES_SUPPORT_OPERATOR_EXPLICIT
#define ASSERT_IF_BOOL_CONVERSION(X) ASSERT(X)
#else
#define ASSERT_IF_BOOL_CONVERSION(X) ASSERT(X)
#endif //BSLS_COMPILERFEATURES_SUPPORT_OPERATOR_EXPLICIT
// ============================================================================
//                  NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT_SAFE_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(EXPR)
#define ASSERT_SAFE_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(EXPR)
#define ASSERT_PASS(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS(EXPR)
#define ASSERT_FAIL(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL(EXPR)
#define ASSERT_OPT_PASS(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS(EXPR)
#define ASSERT_OPT_FAIL(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL(EXPR)

#define ASSERT_SAFE_PASS_RAW(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS_RAW(EXPR)
#define ASSERT_SAFE_FAIL_RAW(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL_RAW(EXPR)
#define ASSERT_PASS_RAW(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS_RAW(EXPR)
#define ASSERT_FAIL_RAW(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL_RAW(EXPR)
#define ASSERT_OPT_PASS_RAW(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(EXPR)
#define ASSERT_OPT_FAIL_RAW(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(EXPR)


// ============================================================================
//                       GLOBAL TEST VALUES
// ----------------------------------------------------------------------------

static bool             verbose;
static bool         veryVerbose;
static bool     veryVeryVerbose;
static bool veryVeryVeryVerbose;

using namespace BloombergLP;
using namespace bsl;

typedef bslmf::MovableRefUtil MovUtl;

const int MOVED_FROM_VAL = 0x01d;

//=============================================================================
//                  CLASSES FOR TESTING USAGE EXAMPLES
//-----------------------------------------------------------------------------

                             // =================
                             // class MyClassDef
                             // =================

struct MyClassDef {
    // Data members that give MyClassX size and alignment.  This class is a
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

                             // ===============
                             // class MyClass1
                             // ===============

class MyClass1 {
    // This 'class' is a simple type that does not take allocators.  Its
    // implementation owns a 'MyClassDef' aggregate, but uses only the
    // 'd_value' data member, to support the 'value' attribute.  The
    // 'd_allocator_p' pointer is always initialized to a null pointer, while
    // the 'd_data_p' pointer is never initialized


 public:

    // DATA
    MyClassDef d_def;

    static int copyConstructorInvocations;
    static int moveConstructorInvocations;
    static int destructorInvocations;

    // CREATORS
   //explicit
    MyClass1(int v = 0) {
        d_def.d_value = v;
        d_def.d_allocator_p = 0;
    }
    MyClass1(const MyClass1& rhs) {
        d_def.d_value = rhs.d_def.d_value;
        d_def.d_allocator_p = 0;
        ++copyConstructorInvocations;
    }

    MyClass1(bslmf::MovableRef<MyClass1> other) {
        MyClass1& otherRef = MovUtl::access(other);
        d_def.d_value = otherRef.d_def.d_value;
        otherRef.d_def.d_value = MOVED_FROM_VAL;
        d_def.d_allocator_p = 0;
        ++moveConstructorInvocations;
    }

    ~MyClass1() {
        ASSERT(d_def.d_value != 91);
        d_def.d_value = 91;
        d_def.d_allocator_p = 0;
        dumpClassDefState(d_def);
        ++destructorInvocations;
    }

    MyClass1& operator=(const MyClass1& rhs) {
        d_def.d_value = rhs.d_def.d_value;
        return *this;
    }

    MyClass1& operator=(bslmf::MovableRef<MyClass1> rhs) {
        MyClass1& otherRef = MovUtl::access(rhs);
        d_def.d_value = otherRef.d_def.d_value;
        otherRef.d_def.d_value = MOVED_FROM_VAL;
        return *this;
    }

    MyClass1& operator=(int rhs) {
        d_def.d_value = rhs;
        return *this;
    }
    // ACCESSORS
    int value() const { return d_def.d_value; }
};
bool operator==(const MyClass1& lhs,
                const MyClass1& rhs)
{
 return (lhs.value()==rhs.value());
}
// CLASS DATA
int MyClass1::copyConstructorInvocations       = 0;
int MyClass1::moveConstructorInvocations       = 0;
int MyClass1::destructorInvocations            = 0;

                             // ===============
                             // class MyClass1a
                             // ===============

class MyClass1a {
    // This 'class' is a simple type that does not take allocators.  Its
    // implementation owns a 'MyClass1' aggregate, but uses only the
    // 'd_value' data member, to support the 'value' attribute.  The
    // 'd_allocator_p' pointer is always initialized to a null pointer.
    // The class is both constructable and asignable from MyClass1.
  public:
    MyClass1 d_data;
    static int copyConstructorInvocations;
    static int moveConstructorInvocations;

    // CREATORS

    explicit
    MyClass1a(int v)  : d_data(v) {}

    MyClass1a(const MyClass1& v)
    : d_data(v) {}

    MyClass1a(bslmf::MovableRef<MyClass1> v)
    : d_data(MovUtl::move(MovUtl::access(v))) {}
#ifndef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
    MyClass1a(bslmf::MovableRef<const MyClass1> v)
    : d_data(MovUtl::access(v)) {}
#endif //#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES

    MyClass1a(const MyClass1a& rhs) : d_data(rhs.d_data)
    {
        ++copyConstructorInvocations;
    }

    MyClass1a(bslmf::MovableRef<MyClass1a> rhs)
    : d_data(MovUtl::move(MovUtl::access(rhs).d_data))
    {
        ++moveConstructorInvocations;
    }
#ifndef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
    MyClass1a(bslmf::MovableRef<const MyClass1a> rhs)
    : d_data(MovUtl::access(rhs).d_data)
    {
        ++moveConstructorInvocations;
    }
#endif //#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES

  // MANIPULATORS
    MyClass1a& operator=(const MyClass1a& rhs) {
        d_data.operator=(rhs.d_data);
        return *this;
    }

    MyClass1a& operator=(bslmf::MovableRef<MyClass1a> rhs) {
        d_data.operator=(MovUtl::move(MovUtl::access(rhs).d_data));
        return *this;
    }
#ifndef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
    MyClass1a& operator=(bslmf::MovableRef<const MyClass1a> rhs) {
        d_data.operator=(MovUtl::access(rhs).d_data);
        return *this;
    }
#endif //#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES

    // ACCESSORS
    int value() const { return d_data.value(); }
};
// CLASS DATA
int MyClass1a::copyConstructorInvocations       = 0;
int MyClass1a::moveConstructorInvocations       = 0;
bool operator==(const MyClass1a& lhs,
                const MyClass1a& rhs)
{
 return (lhs.value()==rhs.value());
}
                             // ===============
                             // class MyClass1b
                             // ===============

class MyClass1b {
    // This 'class' is a simple type that does not take allocators.  Its
    // implementation owns a 'MyClass1' aggregate, but uses only the
    // 'd_value' data member, to support the 'value' attribute.  The
    // 'd_allocator_p' pointer is always initialized to a null pointer.
    // The class is assignable from MyClass1, but not constructable from
    // MyClass1
  public:
    MyClass1 d_data;

    // CREATORS
    MyClass1b() : d_data() { }

    explicit
    MyClass1b(int v)  : d_data(v) {}

    MyClass1b(const MyClass1b& rhs) : d_data(rhs.d_data) {}

    MyClass1b(bslmf::MovableRef<MyClass1b> rhs)
    : d_data(MovUtl::move(MovUtl::access(rhs).d_data)) {}

    // MANIPULATORS
    MyClass1b& operator=(const MyClass1b& rhs) {
    d_data.operator=(rhs.d_data);
        return *this;
    }

    MyClass1b& operator=(bslmf::MovableRef<MyClass1b> rhs) {
        d_data.operator=(MovUtl::move(MovUtl::access(rhs).d_data));
        return *this;
    }

    MyClass1b& operator=(const MyClass1& rhs) {
        d_data.operator=(rhs);
        return *this;
    }

    MyClass1b& operator=(bslmf::MovableRef<MyClass1> rhs) {
        d_data.operator=(MovUtl::move(MovUtl::access(rhs)));
        return *this;
    }

    // ACCESSORS
    int value() const { return d_data.value(); }
};
bool operator==(const MyClass1b& lhs,
                const MyClass1b& rhs)
{
    return (lhs.value()==rhs.value());
}
                             // ================
                             // class MyClass1c
                             // ================

class MyClass1c {
    // This 'class' is a simple type that does not take allocators.  Its
    // implementation owns a 'MyClass1' aggregate, but uses only the
    // 'd_value' data member, to support the 'value' attribute.  The
    // 'd_allocator_p' pointer is always initialized to a null pointer.
    // The class is constructable from MyClass1, but not assignable from
    // MyClass1
  public:
    MyClass1 d_data;

    // CREATORS
    MyClass1c() : d_data() { }

    MyClass1c(int v)  : d_data(v) {}

    explicit
    MyClass1c(const MyClass1& v)
    : d_data(v) {}

    explicit
    MyClass1c(bslmf::MovableRef<MyClass1> v)
    : d_data(MovUtl::move(v)) {}

    MyClass1c(const MyClass1c& rhs) : d_data(rhs.d_data) {}

    MyClass1c(bslmf::MovableRef<MyClass1c> rhs)
    : d_data(MovUtl::move(MovUtl::access(rhs).d_data)) {}

    // MANIPULATORS
    MyClass1c& operator=(const MyClass1c& rhs) {
    d_data.operator=(rhs.d_data);
    return *this;
    }

    MyClass1c& operator=(bslmf::MovableRef<MyClass1c> rhs) {
    d_data.operator=(MovUtl::move(MovUtl::access(rhs).d_data));
    return *this;
    }

    // ACCESSORS
    int value() const { return d_data.value(); }
};
bool operator==(const MyClass1c& lhs,
                const MyClass1c& rhs)
{
    return (lhs.value()==rhs.value());
}

                             // ===============
                             // class MyClass2
                             // ===============

class MyClass2 {
    // This 'class' supports the 'bslma::UsesBslmaAllocator' trait, providing
    // an allocator-aware version of every constructor.  While it holds an
    // allocator and has the expected allocator propagation properties of a
    // 'bslma::Allocator'-aware type, it does not actually allocate any memory.
    // In many ways, this is similar to a 'std::string' object that never grows
    // beyond the small string optimization.  The 'd_data_p' member of the
    // wrapper 'MyClassDef' implementation type is never initialized, nor
    // used.  A signal value, 'MOVED_FROM_VAL', is used to detect an object in
    // a moved-from state.

 public:

    // DATA
    MyClassDef d_def;

    static int copyConstructorInvocations;
    static int moveConstructorInvocations;
    static int destructorInvocations;

    // CREATORS
    explicit
    MyClass2(bslma::Allocator *a = 0) {
        d_def.d_value = 0;
        d_def.d_allocator_p = a;
    }

    MyClass2(int v, bslma::Allocator *a = 0) {
        d_def.d_value = v;
        d_def.d_allocator_p = a;
    }
    MyClass2(const MyClass2& rhs, bslma::Allocator *a = 0) {
        d_def.d_value = rhs.d_def.d_value;
        d_def.d_allocator_p = a;
        copyConstructorInvocations++;
    }

    MyClass2(bslmf::MovableRef<MyClass2> other, bslma::Allocator *a = 0) {

        MyClass2& otherRef = MovUtl::access(other);
        d_def.d_value = otherRef.d_def.d_value;
        otherRef.d_def.d_value = MOVED_FROM_VAL;
        if (a) {
            d_def.d_allocator_p = a;
        }
        else {
            d_def.d_allocator_p = otherRef.d_def.d_allocator_p;
        }
        moveConstructorInvocations++;
    }
#ifndef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
    MyClass2(bslmf::MovableRef<const MyClass2> other,
             bslma::Allocator *a = 0) {

        const MyClass2& otherRef = MovUtl::access(other);
        d_def.d_value = otherRef.d_def.d_value;
        if (a) {
            d_def.d_allocator_p = a;
        }
        else {
            d_def.d_allocator_p = otherRef.d_def.d_allocator_p;
        }
        moveConstructorInvocations++;
    }
#endif //#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES

    MyClass2(const MyClass1& rhs, bslma::Allocator *a = 0) {
        d_def.d_value = rhs.d_def.d_value;
        d_def.d_allocator_p = a;
    }

    MyClass2(bslmf::MovableRef<MyClass1> other, bslma::Allocator *a = 0) {

        MyClass1& otherRef = MovUtl::access(other);
        d_def.d_value = otherRef.d_def.d_value;
        otherRef.d_def.d_value = MOVED_FROM_VAL;
        d_def.d_allocator_p = a;
    }
#ifndef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
    MyClass2(bslmf::MovableRef<const MyClass1> other,
             bslma::Allocator *a = 0) {

        const MyClass1& otherRef = MovUtl::access(other);
        d_def.d_value = otherRef.d_def.d_value;
        d_def.d_allocator_p = a;
    }
#endif //#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES

    ~MyClass2() {
        ASSERT(d_def.d_value != 92);
        d_def.d_value = 92;
        d_def.d_allocator_p = 0;
        dumpClassDefState(d_def);
        ++destructorInvocations;
    }

    // MANIPULATORS
    MyClass2& operator=(const MyClass2& rhs) {
        d_def.d_value = rhs.d_def.d_value;
        // do not touch allocator!
        return *this;
    }

    MyClass2& operator=(bslmf::MovableRef<MyClass2> rhs) {
        MyClass2& otherRef = MovUtl::access(rhs);
        d_def.d_value = otherRef.d_def.d_value;
        otherRef.d_def.d_value = MOVED_FROM_VAL;
        // do not touch allocator!
        return *this;
    }

#ifndef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
    MyClass2& operator=(bslmf::MovableRef<const MyClass2> rhs) {
        const MyClass2& otherRef = MovUtl::access(rhs);
        d_def.d_value = otherRef.d_def.d_value;
        // do not touch allocator!
        return *this;
    }
#endif //#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES

    MyClass2& operator=(int rhs) {
        d_def.d_value = rhs;
        // do not touch allocator!
        return *this;
    }

    // ACCESSORS
    int value() const { return d_def.d_value; }
};
// CLASS DATA
int MyClass2::copyConstructorInvocations       = 0;
int MyClass2::moveConstructorInvocations       = 0;
int MyClass2::destructorInvocations            = 0;

bool operator==(const MyClass2& lhs,
                const MyClass2& rhs)
{
    return (lhs.value()==rhs.value());
}

bool operator==(const int& lhs,
                const MyClass2& rhs)
{
    return (lhs==rhs.value());
}
bool operator==(const MyClass2& lhs,
                const int& rhs)
{
    return (lhs.value()==rhs);
}
bool operator!=(const MyClass2& lhs,
                const MyClass2& rhs)
{
    return !(lhs ==rhs);
}

bool operator!=(const int& lhs,
                const MyClass2& rhs)
{
    return !(lhs ==rhs);
}
bool operator!=(const MyClass2& lhs,
                const int& rhs)
{
    return !(lhs ==rhs);
}
bool operator<(const MyClass2& lhs,
                const MyClass2& rhs)
{
    return (lhs.value()<rhs.value());
}

bool operator<(const int& lhs,
                const MyClass2& rhs)
{
    return (lhs<rhs.value());
}
bool operator<(const MyClass2& lhs,
                const int& rhs)
{
    return (lhs.value()<rhs);
}
bool operator>(const MyClass2& lhs,
                const MyClass2& rhs)
{
    return (lhs.value()>rhs.value());
}

bool operator>(const int& lhs,
                const MyClass2& rhs)
{
    return (lhs>rhs.value());
}
bool operator>(const MyClass2& lhs,
                const int& rhs)
{
    return (lhs.value()>rhs);
}
bool operator<=(const MyClass2& lhs,
                const MyClass2& rhs)
{
    return (lhs.value()<=rhs.value());
}

bool operator<=(const int& lhs,
                const MyClass2& rhs)
{
    return (lhs<=rhs.value());
}
bool operator<=(const MyClass2& lhs,
                const int& rhs)
{
    return (lhs.value()<=rhs);
}
bool operator>=(const MyClass2& lhs,
                const MyClass2& rhs)
{
    return (lhs.value()>=rhs.value());
}

bool operator>=(const int& lhs,
                const MyClass2& rhs)
{
    return (lhs>=rhs.value());
}
bool operator>=(const MyClass2& lhs,
                const int& rhs)
{
    return (lhs.value()>=rhs);
}

// TRAITS
namespace BloombergLP {
namespace bslma {

template <>
struct UsesBslmaAllocator<MyClass2> : bsl::true_type { };

}  // close namespace bslma
}  // close enterprise namespace

                                 // ==========
                                 // MyClass2a
                                 // ==========

class MyClass2a {
    // This 'class' behaves the same as 'MyClass2' (allocator-aware type that
    // never actually allocates memory) except that it uses the
    // 'allocator_arg_t' idiom for passing an allocator to constructors.
    // This class is constructable and assignable from MyClass2
  public:
    MyClass2 d_data;

    static int copyConstructorInvocations;
    static int moveConstructorInvocations;

    // CREATORS
    MyClass2a() : d_data() { }

    MyClass2a(bsl::allocator_arg_t, bslma::Allocator *a) : d_data(a) {}

    explicit
    MyClass2a(int v)  : d_data(v) {}

    MyClass2a(const MyClass2& rhs) : d_data(rhs) {}

    MyClass2a(bslmf::MovableRef<MyClass2> rhs)
      : d_data(MovUtl::move(MovUtl::access(rhs))) {}

#ifndef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
    MyClass2a(bslmf::MovableRef<const MyClass2> rhs)
      : d_data(MovUtl::access(rhs)) {}
#endif //#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES

    MyClass2a(bsl::allocator_arg_t, bslma::Allocator *a, int v)
        : d_data(v, a) {}

    MyClass2a(bsl::allocator_arg_t, bslma::Allocator *a,const MyClass2& v)
            : d_data(v, a) {}

    MyClass2a(bsl::allocator_arg_t, bslma::Allocator *a,
              bslmf::MovableRef<MyClass2> v)
            : d_data(MovUtl::move(MovUtl::access(v)), a) {}

#ifndef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
    MyClass2a(bsl::allocator_arg_t, bslma::Allocator *a,
              bslmf::MovableRef<const MyClass2> v)
            : d_data(MovUtl::access(v), a) {}
#endif //#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES

    MyClass2a(const MyClass2a& rhs) : d_data(rhs.d_data)
    {
        ++copyConstructorInvocations;
    }

    MyClass2a(bsl::allocator_arg_t  ,
              bslma::Allocator     *a,
              const MyClass2a&     rhs)
        : d_data(rhs.d_data, a)
    {
        ++copyConstructorInvocations;
    }

    MyClass2a(bslmf::MovableRef<MyClass2a> rhs)
        : d_data(MovUtl::move(MovUtl::access(rhs).d_data))
    {
        ++moveConstructorInvocations;
    }

#ifndef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
    MyClass2a(bslmf::MovableRef<const MyClass2a> rhs)
        : d_data(MovUtl::access(rhs).d_data)
    {
        ++copyConstructorInvocations;
    }
#endif //#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES

    MyClass2a(bsl::allocator_arg_t,
              bslma::Allocator              *a,
              bslmf::MovableRef<MyClass2a>  rhs)
        : d_data(MovUtl::move(MovUtl::access(rhs).d_data), a)
    {
        ++moveConstructorInvocations;
    }

#ifndef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
    MyClass2a(bsl::allocator_arg_t,
              bslma::Allocator              *a,
              bslmf::MovableRef<const MyClass2a>  rhs)
        : d_data(MovUtl::access(rhs).d_data, a)
    {
        ++copyConstructorInvocations;
    }

#endif //#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
    // MANIPULATORS
    MyClass2a& operator=(const MyClass2a& rhs) {
        d_data.operator=(rhs.d_data);
        return *this;
    }

    MyClass2a& operator=(bslmf::MovableRef<MyClass2a> rhs) {
        d_data.operator=(MovUtl::move(MovUtl::access(rhs).d_data));
        return *this;
    }

#ifndef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
    MyClass2a& operator=(bslmf::MovableRef<const MyClass2a> rhs) {
        d_data.operator=(MovUtl::access(rhs).d_data);
        return *this;
    }

#endif //#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
    MyClass2a& operator=(int rhs) {
        d_data.operator=(rhs);
        return *this;
    }
    // ACCESSORS
    int value() const { return d_data.value(); }
};

bool operator==(const MyClass2a& lhs,
                const MyClass2a& rhs)
{
    return (lhs.value()==rhs.value());
}
int MyClass2a::copyConstructorInvocations       = 0;
int MyClass2a::moveConstructorInvocations       = 0;
// TRAITS
namespace BloombergLP {

namespace bslma {
template <> struct UsesBslmaAllocator<MyClass2a> : bsl::true_type { };
}  // close namespace bslma

namespace bslmf {
template <> struct UsesAllocatorArgT<MyClass2a> : bsl::true_type { };
}  // close namespace bslmf

}  // close enterprise namespace

                        // ==========
                        // MyClass2b
                        // ==========

class MyClass2b {
    // This 'class' behaves the same as 'MyClass2' (allocator-aware type that
    // never actually allocates memory) except that it uses the
    // 'allocator_arg_t' idiom for passing an allocator to constructors.
    // This class is assignable from MyClass2, but not constructible from
    // MyClass2
  public:
    MyClass2 d_data;

    // CREATORS
    MyClass2b() : d_data() { }

    MyClass2b(bsl::allocator_arg_t, bslma::Allocator *a) : d_data(a) {}

    MyClass2b(const MyClass2b& rhs) : d_data(rhs.d_data) {}

    MyClass2b(bsl::allocator_arg_t  ,
                bslma::Allocator     *a,
                const MyClass2b&     rhs)
    : d_data(rhs.d_data, a) {}

    MyClass2b(bslmf::MovableRef<MyClass2b> rhs)
    : d_data(MovUtl::move(MovUtl::access(rhs).d_data)) {}

    MyClass2b(bsl::allocator_arg_t,
                bslma::Allocator              *a,
                bslmf::MovableRef<MyClass2b>  rhs)
    : d_data(MovUtl::move(MovUtl::access(rhs).d_data), a) {}

    // MANIPULATORS
    MyClass2b& operator=(const MyClass2b& rhs) {
        d_data.operator=(rhs.d_data);
        return *this;
    }

    MyClass2b& operator=(bslmf::MovableRef<MyClass2b> rhs) {
        d_data.operator=(MovUtl::move(MovUtl::access(rhs).d_data));
        return *this;
    }

    MyClass2b& operator=(const MyClass2& rhs) {
       d_data.operator=(rhs);
       return *this;
    }

    MyClass2b& operator=(bslmf::MovableRef<MyClass2> rhs) {
       d_data.operator=(MovUtl::move(MovUtl::access(rhs)));
       return *this;
    }
    // ACCESSORS
    int value() const { return d_data.value(); }
};
bool operator==(const MyClass2b& lhs,
                const MyClass2b& rhs)
{
    return (lhs.value()==rhs.value());
}

// TRAITS
namespace BloombergLP {

namespace bslma {
template <> struct UsesBslmaAllocator<MyClass2b> : bsl::true_type { };
}  // close namespace bslma

namespace bslmf {
template <> struct UsesAllocatorArgT<MyClass2b> : bsl::true_type { };
}  // close namespace bslmf

}  // close enterprise namespace


                        // ==========
                        // MyClass2c
                        // ==========

class MyClass2c {
  // This 'class' behaves the same as 'MyClass2' (allocator-aware type that
  // never actually allocates memory) except that it uses the
  // 'allocator_arg_t' idiom for passing an allocator to constructors.
  // This class is constructable from MyClass2, but not assignable from
  // MyClass2

  public:
    MyClass2 d_data;

    // CREATORS
    MyClass2c() : d_data() { }

    MyClass2c(bsl::allocator_arg_t, bslma::Allocator *a) : d_data(a) {}

    explicit
    MyClass2c(int v)  : d_data(v) {}

    MyClass2c(bsl::allocator_arg_t, bslma::Allocator *a, int v)
    : d_data(v, a) {}

    MyClass2c(bsl::allocator_arg_t, bslma::Allocator *a,const MyClass2& v)
    : d_data(v, a) {}

    MyClass2c(bsl::allocator_arg_t, bslma::Allocator *a,
    bslmf::MovableRef<MyClass2> v)
    : d_data(MovUtl::move(v), a) {}


    MyClass2c(const MyClass2c& rhs) : d_data(rhs.d_data) {}

    MyClass2c(bsl::allocator_arg_t  ,
    bslma::Allocator     *a,
    const MyClass2c&     rhs)
    : d_data(rhs.d_data, a) {}

    MyClass2c(bslmf::MovableRef<MyClass2c> rhs)
    : d_data(MovUtl::move(MovUtl::access(rhs).d_data)) {}

    MyClass2c(bsl::allocator_arg_t,
              bslma::Allocator              *a,
              bslmf::MovableRef<MyClass2c>  rhs)
    : d_data(MovUtl::move(MovUtl::access(rhs).d_data), a) {}

    // MANIPULATORS
    MyClass2c& operator=(const MyClass2c& rhs) {
        d_data.operator=(rhs.d_data);
        return *this;
    }

    MyClass2c& operator=(bslmf::MovableRef<MyClass2c> rhs) {
        d_data.operator=(MovUtl::move(MovUtl::access(rhs).d_data));
        return *this;
    }

    MyClass2c& operator=(int rhs) {
        d_data.operator=(rhs);
        return *this;
    }
    // ACCESSORS
    int value() const { return d_data.value(); }
};
bool operator==(const MyClass2c& lhs,
                const MyClass2c& rhs)
{
    return (lhs.value()==rhs.value());
}
// TRAITS
namespace BloombergLP {

namespace bslma {
template <> struct UsesBslmaAllocator<MyClass2c> : bsl::true_type { };
}  // close namespace bslma

namespace bslmf {
template <> struct UsesAllocatorArgT<MyClass2c> : bsl::true_type { };
}  // close namespace bslmf

}  // close enterprise namespace

                        // ===============
                        // class MyClass3
                        // ===============

class MyClass3 {
    // This 'class' takes allocators similarly to 'MyClass2'. In addition,
    // it has overloads taking const rvalues.

  public:

      // DATA
      MyClassDef d_def;

      // CREATORS
      explicit
      MyClass3(bslma::Allocator *a = 0) {
          d_def.d_value = 0;
          d_def.d_allocator_p = a;
      }
      MyClass3(int v, bslma::Allocator *a = 0) {
          d_def.d_value = v;
          d_def.d_allocator_p = a;
      }
      MyClass3(const MyClass3& rhs, bslma::Allocator *a = 0) {
          d_def.d_value = rhs.d_def.d_value;
          d_def.d_allocator_p = a;
      }

      MyClass3(const MyClass2& rhs, bslma::Allocator *a = 0) {
          d_def.d_value = rhs.d_def.d_value;
          d_def.d_allocator_p = a;
      }

      MyClass3(bslmf::MovableRef<MyClass3> other, bslma::Allocator *a = 0) {

          MyClass3& otherRef = MovUtl::access(other);
          d_def.d_value = otherRef.d_def.d_value + 10;
          otherRef.d_def.d_value = MOVED_FROM_VAL;
          if (a) {
              d_def.d_allocator_p = a;
          }
          else {
              d_def.d_allocator_p = otherRef.d_def.d_allocator_p;
          }
      }
#ifdef BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS
      MyClass3(const MyClass3&& other, bslma::Allocator *a = 0) {
          d_def.d_value = other.d_def.d_value + 20;
          if (a) {
              d_def.d_allocator_p = a;
          }
          else {
              d_def.d_allocator_p = other.d_def.d_allocator_p;
          }
      }
#endif
      ~MyClass3() {
          ASSERT(d_def.d_value != 93);
          d_def.d_value = 93;
          d_def.d_allocator_p = 0;
          dumpClassDefState(d_def);
      }

      // MANIPULATORS
      MyClass3& operator=(const MyClass3& rhs) {
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
struct UsesBslmaAllocator<MyClass3> : bsl::true_type { };

}  // close namespace bslma
}  // close enterprise namespace

                      // ===============
                      // class MyClass4
                      // ===============

class MyClass4 {

    // This 'class' is the same as MyClass3, except it doesn't use
    // allocators.

public:

    // DATA
    MyClassDef d_def;

    // CREATORS
    explicit
    MyClass4() {
        d_def.d_value = 0;
        d_def.d_allocator_p = 0;
    }
    MyClass4(int v) {
        d_def.d_value = v;
        d_def.d_allocator_p = 0;
    }
    MyClass4(const MyClass4& rhs) {
        d_def.d_value = rhs.d_def.d_value;
        d_def.d_allocator_p = 0;
    }

    MyClass4(const MyClass2& rhs) {
        d_def.d_value = rhs.d_def.d_value;
        d_def.d_allocator_p = 0;
    }

    MyClass4(bslmf::MovableRef<MyClass4> other) {
        MyClass4& otherRef = MovUtl::access(other);
        d_def.d_value = otherRef.d_def.d_value + 10;
        otherRef.d_def.d_value = MOVED_FROM_VAL;
    }
#ifdef BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS
    MyClass4(const MyClass4&& other) {
        d_def.d_value = other.d_def.d_value + 20;
    }
#endif //BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS
    ~MyClass4() {
        ASSERT(d_def.d_value != 93);
        d_def.d_value = 93;
        d_def.d_allocator_p = 0;
        dumpClassDefState(d_def);
    }

    // MANIPULATORS
    MyClass4& operator=(const MyClass4& rhs) {
        d_def.d_value = rhs.d_def.d_value;
        return *this;
    }

    // ACCESSORS
    int value() const { return d_def.d_value; }
};

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
    int  d_copied_count;
        // if true, indicates this object was created from an rvalue

    // CREATORS
    ConstructTestArg(int value = -1);
        // Create an object having the specified 'value'.

    ConstructTestArg(const ConstructTestArg& other);
    ConstructTestArg(bslmf::MovableRef<ConstructTestArg>  other);
};

// CREATORS
template <int ID>
ConstructTestArg<ID>::ConstructTestArg(int value)
    : d_value(value), d_copied_count(0)
{
}
template <int ID>
ConstructTestArg<ID>::ConstructTestArg(const ConstructTestArg& other)
    : d_value(other.d_value), d_copied_count(other.d_copied_count + 1)
{
}
template <int ID>
ConstructTestArg<ID>::ConstructTestArg(
                                     bslmf::MovableRef<ConstructTestArg> other)
    : d_value(MovUtl::access(other).d_value),
      d_copied_count(MovUtl::access(other).d_copied_count)
{}
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
    static int copyConstructorInvocations;
    static int moveConstructorInvocations;


    int d_ilsum; // sum of initializer_list argument

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

    ConstructTestTypeNoAlloc() : d_ilsum(0){};

    explicit
    ConstructTestTypeNoAlloc(const Arg1 &  a1)
        : d_ilsum(0), d_a1(a1) {}
    explicit
    ConstructTestTypeNoAlloc(bslmf::MovableRef<Arg1>  a1)
        : d_ilsum(0), d_a1(MovUtl::move(a1)) {}

    template <class ARG1,  class ARG2>
    explicit
    ConstructTestTypeNoAlloc(
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2)
       : d_ilsum(0),
         d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1)),
         d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2))
    {}

    template <class ARG1,  class ARG2,  class ARG3>
        explicit
        ConstructTestTypeNoAlloc(
                                 BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                                 BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                                 BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3)
       : d_ilsum(0),
         d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1)),
         d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2)),
         d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3))
    {}
    template <class ARG1,  class ARG2,  class ARG3,
                          class ARG4>
    explicit
    ConstructTestTypeNoAlloc(
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)  a4)
    : d_ilsum(0),
      d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1)),
      d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2)),
      d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3)),
      d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4))
    {}
    template <class ARG1,  class ARG2,  class ARG3,
                          class ARG4,  class ARG5>
    explicit
    ConstructTestTypeNoAlloc(
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)  a4,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG5)  a5)
       : d_ilsum(0),
         d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1)),
         d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2)),
         d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3)),
         d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4)),
         d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5))
    {}
    template <class ARG1,  class ARG2,  class ARG3,
                          class ARG4,  class ARG5,  class ARG6>
    explicit
    ConstructTestTypeNoAlloc(
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)  a4,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG5)  a5,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG6)  a6)
       : d_ilsum(0),
         d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1)),
         d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2)),
         d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3)),
         d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4)),
         d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5)),
         d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6))
    {}
    template <class ARG1,  class ARG2,  class ARG3,
                          class ARG4,  class ARG5,  class ARG6,
                          class ARG7>
    explicit
    ConstructTestTypeNoAlloc(
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)  a4,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG5)  a5,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG6)  a6,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG7)  a7)
       : d_ilsum(0),
         d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1)),
         d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2)),
         d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3)),
         d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4)),
         d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5)),
         d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6)),
         d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7))
    {}
    template <class ARG1,  class ARG2,  class ARG3,
                          class ARG4,  class ARG5,  class ARG6,
                          class ARG7,  class ARG8>
    explicit
    ConstructTestTypeNoAlloc(
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)  a4,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG5)  a5,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG6)  a6,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG7)  a7,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG8)  a8)
       : d_ilsum(0),
         d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1)),
         d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2)),
         d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3)),
         d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4)),
         d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5)),
         d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6)),
         d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7)),
         d_a8(BSLS_COMPILERFEATURES_FORWARD(ARG8, a8))
    {}
    template <class ARG1,  class ARG2,  class ARG3,
                          class ARG4,  class ARG5,  class ARG6,
                          class ARG7,  class ARG8,  class ARG9>
    explicit
    ConstructTestTypeNoAlloc(
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)  a4,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG5)  a5,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG6)  a6,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG7)  a7,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG8)  a8,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG9)  a9)
       : d_ilsum(0),
         d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1)),
         d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2)),
         d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3)),
         d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4)),
         d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5)),
         d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6)),
         d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7)),
         d_a8(BSLS_COMPILERFEATURES_FORWARD(ARG8, a8)),
         d_a9(BSLS_COMPILERFEATURES_FORWARD(ARG9, a9))
    {}
    template <class ARG1,  class ARG2,  class ARG3,
                          class ARG4,  class ARG5,  class ARG6,
                          class ARG7,  class ARG8,  class ARG9,
                          class ARG10>
    explicit
    ConstructTestTypeNoAlloc(
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)  a4,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG5)  a5,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG6)  a6,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG7)  a7,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG8)  a8,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG9)  a9,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG10) a10)
     : d_ilsum(0),
       d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1)),
       d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2)),
       d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3)),
       d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4)),
       d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5)),
       d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6)),
       d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7)),
       d_a8(BSLS_COMPILERFEATURES_FORWARD(ARG8, a8)),
       d_a9(BSLS_COMPILERFEATURES_FORWARD(ARG9, a9)),
       d_a10(BSLS_COMPILERFEATURES_FORWARD(ARG10, a10))
    {}
    template <class ARG1,  class ARG2,  class ARG3,
                          class ARG4,  class ARG5,  class ARG6,
                          class ARG7,  class ARG8,  class ARG9,
                          class ARG10, class ARG11>
    explicit
    ConstructTestTypeNoAlloc(
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)  a4,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG5)  a5,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG6)  a6,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG7)  a7,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG8)  a8,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG9)  a9,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG10) a10,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG11) a11)
       : d_ilsum(0),
         d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1)),
         d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2)),
         d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3)),
         d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4)),
         d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5)),
         d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6)),
         d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7)),
         d_a8(BSLS_COMPILERFEATURES_FORWARD(ARG8, a8)),
         d_a9(BSLS_COMPILERFEATURES_FORWARD(ARG9, a9)),
         d_a10(BSLS_COMPILERFEATURES_FORWARD(ARG10, a10)),
         d_a11(BSLS_COMPILERFEATURES_FORWARD(ARG11, a11))
    {}

    template <class ARG1,  class ARG2,  class ARG3,
                      class ARG4,  class ARG5,  class ARG6,
                      class ARG7,  class ARG8,  class ARG9,
                      class ARG10, class ARG11, class ARG12>
    explicit
    ConstructTestTypeNoAlloc(
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)  a4,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG5)  a5,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG6)  a6,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG7)  a7,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG8)  a8,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG9)  a9,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG10) a10,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG11) a11,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG12) a12)
       : d_ilsum(0),
         d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1)),
         d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2)),
         d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3)),
         d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4)),
         d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5)),
         d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6)),
         d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7)),
         d_a8(BSLS_COMPILERFEATURES_FORWARD(ARG8, a8)),
         d_a9(BSLS_COMPILERFEATURES_FORWARD(ARG9, a9)),
         d_a10(BSLS_COMPILERFEATURES_FORWARD(ARG10, a10)),
         d_a11(BSLS_COMPILERFEATURES_FORWARD(ARG11, a11)),
         d_a12(BSLS_COMPILERFEATURES_FORWARD(ARG12, a12))
    {}

    template <class ARG1,  class ARG2,  class ARG3,
                  class ARG4,  class ARG5,  class ARG6,
                  class ARG7,  class ARG8,  class ARG9,
                  class ARG10, class ARG11, class ARG12,
                  class ARG13>
    explicit
    ConstructTestTypeNoAlloc(
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)  a4,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG5)  a5,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG6)  a6,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG7)  a7,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG8)  a8,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG9)  a9,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG10) a10,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG11) a11,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG12) a12,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG13) a13)
        : d_ilsum(0),
          d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1)),
          d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2)),
          d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3)),
          d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4)),
          d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5)),
          d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6)),
          d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7)),
          d_a8(BSLS_COMPILERFEATURES_FORWARD(ARG8, a8)),
          d_a9(BSLS_COMPILERFEATURES_FORWARD(ARG9, a9)),
          d_a10(BSLS_COMPILERFEATURES_FORWARD(ARG10, a10)),
          d_a11(BSLS_COMPILERFEATURES_FORWARD(ARG11, a11)),
          d_a12(BSLS_COMPILERFEATURES_FORWARD(ARG12, a12)),
          d_a13(BSLS_COMPILERFEATURES_FORWARD(ARG13, a13)) {}


    template <class ARG1,  class ARG2,  class ARG3,
                  class ARG4,  class ARG5,  class ARG6,
                  class ARG7,  class ARG8,  class ARG9,
                  class ARG10, class ARG11, class ARG12,
                  class ARG13, class ARG14>
    explicit
    ConstructTestTypeNoAlloc(
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)  a4,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG5)  a5,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG6)  a6,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG7)  a7,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG8)  a8,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG9)  a9,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG10) a10,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG11) a11,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG12) a12,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG13) a13,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG14) a14)
        :d_ilsum(0),
          d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1)),
          d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2)),
          d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3)),
          d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4)),
          d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5)),
          d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6)),
          d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7)),
          d_a8(BSLS_COMPILERFEATURES_FORWARD(ARG8, a8)),
          d_a9(BSLS_COMPILERFEATURES_FORWARD(ARG9, a9)),
          d_a10(BSLS_COMPILERFEATURES_FORWARD(ARG10, a10)),
          d_a11(BSLS_COMPILERFEATURES_FORWARD(ARG11, a11)),
          d_a12(BSLS_COMPILERFEATURES_FORWARD(ARG12, a12)),
          d_a13(BSLS_COMPILERFEATURES_FORWARD(ARG13, a13)),
          d_a14(BSLS_COMPILERFEATURES_FORWARD(ARG14, a14)) {}

    ConstructTestTypeNoAlloc(const ConstructTestTypeNoAlloc &other)
        : d_ilsum(0),
          d_a1(other.d_a1), d_a2(other.d_a2), d_a3(other.d_a3),
          d_a4(other.d_a4), d_a5(other.d_a5), d_a6(other.d_a6),
          d_a7(other.d_a7), d_a8(other.d_a8), d_a9(other.d_a9),
          d_a10(other.d_a10), d_a11(other.d_a11), d_a12(other.d_a12),
          d_a13(other.d_a13), d_a14(other.d_a14)
    {
        ++copyConstructorInvocations;
    }

    ConstructTestTypeNoAlloc(
                           bslmf::MovableRef<ConstructTestTypeNoAlloc> &other)
        : d_ilsum(0),
          d_a1(MovUtl::access(other).d_a1), d_a2(MovUtl::access(other).d_a2),
          d_a3(MovUtl::access(other).d_a3), d_a4(MovUtl::access(other).d_a4),
          d_a5(MovUtl::access(other).d_a5), d_a6(MovUtl::access(other).d_a6),
          d_a7(MovUtl::access(other).d_a7), d_a8(MovUtl::access(other).d_a8),
          d_a9(MovUtl::access(other).d_a9),d_a10(MovUtl::access(other).d_a10),
          d_a11(MovUtl::access(other).d_a11), d_a12(MovUtl::access(other).d_a12),
          d_a13(MovUtl::access(other).d_a13), d_a14(MovUtl::access(other).d_a14)
    {
        ++moveConstructorInvocations;
    }

#ifdef BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS
    ConstructTestTypeNoAlloc(std::initializer_list<int> il)
        : d_ilsum(0)
    {
      for (int i : il) d_ilsum+=i;
    }

    template <class ARG1>
    explicit
    ConstructTestTypeNoAlloc(std::initializer_list<int> il,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1)
       : d_ilsum(0),
         d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1))
    {
      for (int i : il) d_ilsum+=i;
    }

    template <class ARG1,  class ARG2>
    explicit
    ConstructTestTypeNoAlloc(std::initializer_list<int> il,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2)
       : d_ilsum(0),
         d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1)),
         d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2))
    {
      for (int i : il) d_ilsum+=i;
    }


    template <class ARG1,  class ARG2,  class ARG3>
        explicit
        ConstructTestTypeNoAlloc(std::initializer_list<int> il,
                                 BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                                 BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                                 BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3)
       : d_ilsum(0),
         d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1)),
         d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2)),
         d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3))
    {
      for (int i : il) d_ilsum+=i;
    }

    template <class ARG1,  class ARG2,  class ARG3,
                          class ARG4>
    explicit
    ConstructTestTypeNoAlloc(std::initializer_list<int> il,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)  a4)
    : d_ilsum(0),
      d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1)),
      d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2)),
      d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3)),
      d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4))
    {
      for (int i : il) d_ilsum+=i;
    }

    template <class ARG1,  class ARG2,  class ARG3,
                          class ARG4,  class ARG5>
    explicit
    ConstructTestTypeNoAlloc(std::initializer_list<int> il,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)  a4,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG5)  a5)
       : d_ilsum(0),
         d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1)),
         d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2)),
         d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3)),
         d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4)),
         d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5))
    {
      for (int i : il) d_ilsum+=i;
    }

    template <class ARG1,  class ARG2,  class ARG3,
                          class ARG4,  class ARG5,  class ARG6>
    explicit
    ConstructTestTypeNoAlloc(std::initializer_list<int> il,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)  a4,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG5)  a5,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG6)  a6)
       : d_ilsum(0),
         d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1)),
         d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2)),
         d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3)),
         d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4)),
         d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5)),
         d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6))
    {
      for (int i : il) d_ilsum+=i;
    }

    template <class ARG1,  class ARG2,  class ARG3,
                          class ARG4,  class ARG5,  class ARG6,
                          class ARG7>
    explicit
    ConstructTestTypeNoAlloc(std::initializer_list<int> il,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)  a4,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG5)  a5,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG6)  a6,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG7)  a7)
       : d_ilsum(0),
         d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1)),
         d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2)),
         d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3)),
         d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4)),
         d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5)),
         d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6)),
         d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7))
    {
      for (int i : il) d_ilsum+=i;
    }

    template <class ARG1,  class ARG2,  class ARG3,
                          class ARG4,  class ARG5,  class ARG6,
                          class ARG7,  class ARG8>
    explicit
    ConstructTestTypeNoAlloc(std::initializer_list<int> il,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)  a4,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG5)  a5,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG6)  a6,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG7)  a7,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG8)  a8)
       : d_ilsum(0),
         d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1)),
         d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2)),
         d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3)),
         d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4)),
         d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5)),
         d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6)),
         d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7)),
         d_a8(BSLS_COMPILERFEATURES_FORWARD(ARG8, a8))
    {
      for (int i : il) d_ilsum+=i;
    }

    template <class ARG1,  class ARG2,  class ARG3,
                          class ARG4,  class ARG5,  class ARG6,
                          class ARG7,  class ARG8,  class ARG9>
    explicit
    ConstructTestTypeNoAlloc(std::initializer_list<int> il,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)  a4,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG5)  a5,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG6)  a6,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG7)  a7,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG8)  a8,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG9)  a9)
       : d_ilsum(0),
         d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1)),
         d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2)),
         d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3)),
         d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4)),
         d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5)),
         d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6)),
         d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7)),
         d_a8(BSLS_COMPILERFEATURES_FORWARD(ARG8, a8)),
         d_a9(BSLS_COMPILERFEATURES_FORWARD(ARG9, a9))
    {
      for (int i : il) d_ilsum+=i;
    }

    template <class ARG1,  class ARG2,  class ARG3,
                          class ARG4,  class ARG5,  class ARG6,
                          class ARG7,  class ARG8,  class ARG9,
                          class ARG10>
    explicit
    ConstructTestTypeNoAlloc(std::initializer_list<int> il,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)  a4,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG5)  a5,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG6)  a6,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG7)  a7,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG8)  a8,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG9)  a9,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG10) a10)
     : d_ilsum(0),
       d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1)),
       d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2)),
       d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3)),
       d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4)),
       d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5)),
       d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6)),
       d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7)),
       d_a8(BSLS_COMPILERFEATURES_FORWARD(ARG8, a8)),
       d_a9(BSLS_COMPILERFEATURES_FORWARD(ARG9, a9)),
       d_a10(BSLS_COMPILERFEATURES_FORWARD(ARG10, a10))
    {
      for (int i : il) d_ilsum+=i;
    }

    template <class ARG1,  class ARG2,  class ARG3,
                          class ARG4,  class ARG5,  class ARG6,
                          class ARG7,  class ARG8,  class ARG9,
                          class ARG10, class ARG11>
    explicit
    ConstructTestTypeNoAlloc(std::initializer_list<int> il,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)  a4,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG5)  a5,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG6)  a6,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG7)  a7,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG8)  a8,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG9)  a9,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG10) a10,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG11) a11)
       : d_ilsum(0),
         d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1)),
         d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2)),
         d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3)),
         d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4)),
         d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5)),
         d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6)),
         d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7)),
         d_a8(BSLS_COMPILERFEATURES_FORWARD(ARG8, a8)),
         d_a9(BSLS_COMPILERFEATURES_FORWARD(ARG9, a9)),
         d_a10(BSLS_COMPILERFEATURES_FORWARD(ARG10, a10)),
         d_a11(BSLS_COMPILERFEATURES_FORWARD(ARG11, a11))
    {
      for (int i : il) d_ilsum+=i;
    }

    template <class ARG1,  class ARG2,  class ARG3,
                      class ARG4,  class ARG5,  class ARG6,
                      class ARG7,  class ARG8,  class ARG9,
                      class ARG10, class ARG11, class ARG12>
    explicit
    ConstructTestTypeNoAlloc(std::initializer_list<int> il,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)  a4,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG5)  a5,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG6)  a6,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG7)  a7,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG8)  a8,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG9)  a9,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG10) a10,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG11) a11,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG12) a12)
       : d_ilsum(0),
         d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1)),
         d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2)),
         d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3)),
         d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4)),
         d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5)),
         d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6)),
         d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7)),
         d_a8(BSLS_COMPILERFEATURES_FORWARD(ARG8, a8)),
         d_a9(BSLS_COMPILERFEATURES_FORWARD(ARG9, a9)),
         d_a10(BSLS_COMPILERFEATURES_FORWARD(ARG10, a10)),
         d_a11(BSLS_COMPILERFEATURES_FORWARD(ARG11, a11)),
         d_a12(BSLS_COMPILERFEATURES_FORWARD(ARG12, a12))
    {
      for (int i : il) d_ilsum+=i;
    }

    template <class ARG1,  class ARG2,  class ARG3,
                  class ARG4,  class ARG5,  class ARG6,
                  class ARG7,  class ARG8,  class ARG9,
                  class ARG10, class ARG11, class ARG12,
                  class ARG13>
    explicit
    ConstructTestTypeNoAlloc(std::initializer_list<int> il,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)  a4,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG5)  a5,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG6)  a6,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG7)  a7,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG8)  a8,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG9)  a9,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG10) a10,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG11) a11,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG12) a12,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG13) a13)
        : d_ilsum(0),
          d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1)),
          d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2)),
          d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3)),
          d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4)),
          d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5)),
          d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6)),
          d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7)),
          d_a8(BSLS_COMPILERFEATURES_FORWARD(ARG8, a8)),
          d_a9(BSLS_COMPILERFEATURES_FORWARD(ARG9, a9)),
          d_a10(BSLS_COMPILERFEATURES_FORWARD(ARG10, a10)),
          d_a11(BSLS_COMPILERFEATURES_FORWARD(ARG11, a11)),
          d_a12(BSLS_COMPILERFEATURES_FORWARD(ARG12, a12)),
          d_a13(BSLS_COMPILERFEATURES_FORWARD(ARG13, a13))
    {
      for (int i : il) d_ilsum+=i;
    }


    template <class ARG1,  class ARG2,  class ARG3,
                  class ARG4,  class ARG5,  class ARG6,
                  class ARG7,  class ARG8,  class ARG9,
                  class ARG10, class ARG11, class ARG12,
                  class ARG13, class ARG14>
    explicit
    ConstructTestTypeNoAlloc(std::initializer_list<int> il,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)  a4,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG5)  a5,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG6)  a6,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG7)  a7,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG8)  a8,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG9)  a9,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG10) a10,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG11) a11,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG12) a12,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG13) a13,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG14) a14)
        :d_ilsum(0),
          d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1)),
          d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2)),
          d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3)),
          d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4)),
          d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5)),
          d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6)),
          d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7)),
          d_a8(BSLS_COMPILERFEATURES_FORWARD(ARG8, a8)),
          d_a9(BSLS_COMPILERFEATURES_FORWARD(ARG9, a9)),
          d_a10(BSLS_COMPILERFEATURES_FORWARD(ARG10, a10)),
          d_a11(BSLS_COMPILERFEATURES_FORWARD(ARG11, a11)),
          d_a12(BSLS_COMPILERFEATURES_FORWARD(ARG12, a12)),
          d_a13(BSLS_COMPILERFEATURES_FORWARD(ARG13, a13)),
          d_a14(BSLS_COMPILERFEATURES_FORWARD(ARG14, a14))
    {
      for (int i : il) d_ilsum+=i;
    }
#endif //BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS

};
int ConstructTestTypeNoAlloc::copyConstructorInvocations       = 0;
int ConstructTestTypeNoAlloc::moveConstructorInvocations       = 0;

// FREE OPERATORS
bool operator==(const ConstructTestTypeNoAlloc& lhs,
                const ConstructTestTypeNoAlloc& rhs)
{
    return lhs.d_ilsum  == rhs.d_ilsum &&
           lhs.d_a1.d_value  == rhs.d_a1.d_value &&
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
           lhs.d_a14.d_value == rhs.d_a14.d_value ;
}
bool createdAlike(const ConstructTestTypeNoAlloc& lhs,
                  const ConstructTestTypeNoAlloc& rhs)
{
    return lhs.d_a1.d_copied_count  == rhs.d_a1.d_copied_count &&
           lhs.d_a2.d_copied_count  == rhs.d_a2.d_copied_count &&
           lhs.d_a3.d_copied_count  == rhs.d_a3.d_copied_count &&
           lhs.d_a4.d_copied_count  == rhs.d_a4.d_copied_count &&
           lhs.d_a5.d_copied_count  == rhs.d_a5.d_copied_count &&
           lhs.d_a6.d_copied_count  == rhs.d_a6.d_copied_count &&
           lhs.d_a7.d_copied_count  == rhs.d_a7.d_copied_count &&
           lhs.d_a8.d_copied_count  == rhs.d_a8.d_copied_count &&
           lhs.d_a9.d_copied_count  == rhs.d_a9.d_copied_count &&
           lhs.d_a10.d_copied_count == rhs.d_a10.d_copied_count &&
           lhs.d_a11.d_copied_count == rhs.d_a11.d_copied_count &&
           lhs.d_a12.d_copied_count == rhs.d_a12.d_copied_count &&
           lhs.d_a13.d_copied_count == rhs.d_a13.d_copied_count &&
           lhs.d_a14.d_copied_count == rhs.d_a14.d_copied_count;
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
    static int copyConstructorInvocations;
    static int moveConstructorInvocations;

    int d_ilsum; // sum of initializer_list argument

    bslma::Allocator *d_allocator_p;
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
        : d_ilsum(0), d_allocator_p(allocator) {}
    ConstructTestTypeAlloc(const ConstructTestTypeAlloc&  other,
                           bslma::Allocator              *allocator = 0)
        : d_ilsum(0), d_allocator_p(allocator)
        , d_a1 (other.d_a1),  d_a2 (other.d_a2),  d_a3 (other.d_a3)
        , d_a4 (other.d_a4),  d_a5 (other.d_a5),  d_a6 (other.d_a6)
        , d_a7 (other.d_a7),  d_a8 (other.d_a8),  d_a9 (other.d_a9)
        , d_a10(other.d_a10), d_a11(other.d_a11), d_a12(other.d_a12)
        , d_a13(other.d_a13), d_a14(other.d_a14)
    {
        ++copyConstructorInvocations;
    }

    ConstructTestTypeAlloc(bslmf::MovableRef<ConstructTestTypeAlloc>  other,
                           bslma::Allocator              *allocator = 0)
        : d_ilsum(0), d_allocator_p(allocator),
          d_a1 (MovUtl::move(MovUtl::access(other).d_a1)),
          d_a2 (MovUtl::move(MovUtl::access(other).d_a2)),
          d_a3 (MovUtl::move(MovUtl::access(other).d_a3)),
          d_a4 (MovUtl::move(MovUtl::access(other).d_a4)),
          d_a5 (MovUtl::move(MovUtl::access(other).d_a5)),
          d_a6 (MovUtl::move(MovUtl::access(other).d_a6)),
          d_a7 (MovUtl::move(MovUtl::access(other).d_a7)),
          d_a8 (MovUtl::move(MovUtl::access(other).d_a8)),
          d_a9 (MovUtl::move(MovUtl::access(other).d_a9)),
          d_a10(MovUtl::move(MovUtl::access(other).d_a10)),
          d_a11(MovUtl::move(MovUtl::access(other).d_a11)),
          d_a12(MovUtl::move(MovUtl::access(other).d_a12)),
          d_a13(MovUtl::move(MovUtl::access(other).d_a13)),
          d_a14(MovUtl::move(MovUtl::access(other).d_a14))
    {
        ++moveConstructorInvocations;
    }

    explicit
    ConstructTestTypeAlloc(const Arg1 &  a1, bslma::Allocator *allocator = 0)
        : d_ilsum(0), d_allocator_p(allocator)
        , d_a1(a1) {}
    explicit
    ConstructTestTypeAlloc(bslmf::MovableRef<Arg1>  a1,
                           bslma::Allocator *allocator = 0)
        : d_ilsum(0), d_allocator_p(allocator)
        , d_a1(MovUtl::move(a1)) {}

    template <class ARG1>
    explicit
    ConstructTestTypeAlloc(BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                           const Arg2 &                             a2,
                           bslma::Allocator             *allocator = 0)
        : d_ilsum(0), d_allocator_p(allocator),
          d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1)),
          d_a2(a2) {}

    template <class ARG1>
    explicit
    ConstructTestTypeAlloc(BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                           bslmf::MovableRef<Arg2>                  a2,
                           bslma::Allocator *allocator              = 0)
        : d_ilsum(0), d_allocator_p(allocator),
          d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1)),
          d_a2(MovUtl::move(a2)) {}

    template <class ARG1,  class ARG2>
    ConstructTestTypeAlloc(BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                           const Arg3                              &a3,
                           bslma::Allocator *allocator              = 0)
        : d_ilsum(0), d_allocator_p(allocator),
          d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1)),
          d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2)),
          d_a3(a3){}

    template <class ARG1,  class ARG2>
    ConstructTestTypeAlloc(BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                           bslmf::MovableRef<Arg3>                  a3,
                           bslma::Allocator *allocator = 0)
        : d_ilsum(0), d_allocator_p(allocator),
          d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1)),
          d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2)),
          d_a3(MovUtl::move(a3)){}

    template <class ARG1,  class ARG2,  class ARG3>
    ConstructTestTypeAlloc(BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
                           const Arg4                              &a4,
                           bslma::Allocator *allocator = 0)
        : d_ilsum(0), d_allocator_p(allocator),
          d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1)),
          d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2)),
          d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3)),
          d_a4(a4){}
    template <class ARG1,  class ARG2,  class ARG3>
    ConstructTestTypeAlloc(BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
                           bslmf::MovableRef<Arg4>                  a4,
                           bslma::Allocator *allocator = 0)
        : d_ilsum(0), d_allocator_p(allocator),
          d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1)),
          d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2)),
          d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3)),
          d_a4(MovUtl::move(a4)){}

    template <class ARG1,  class ARG2,  class ARG3,
              class ARG4>
    ConstructTestTypeAlloc(BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)  a4,
                           const Arg5                              &a5,
                           bslma::Allocator *allocator = 0)
        : d_ilsum(0), d_allocator_p(allocator),
          d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1)),
          d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2)),
          d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3)),
          d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4)),
          d_a5(a5){}
    template <class ARG1,  class ARG2,  class ARG3,
              class ARG4>
    ConstructTestTypeAlloc(BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)  a4,
                           bslmf::MovableRef<Arg5>                  a5,
                           bslma::Allocator *allocator = 0)
        : d_ilsum(0), d_allocator_p(allocator),
          d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1)),
          d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2)),
          d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3)),
          d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4)),
          d_a5(MovUtl::move(a5)){}

    template <class ARG1,  class ARG2,  class ARG3,
              class ARG4,  class ARG5>
    ConstructTestTypeAlloc(BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)  a4,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG5)  a5,
                           const Arg6                              &a6,
                           bslma::Allocator *allocator = 0)
        : d_ilsum(0), d_allocator_p(allocator),
          d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1)),
          d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2)),
          d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3)),
          d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4)),
          d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5)),
          d_a6(a6){}
    template <class ARG1,  class ARG2,  class ARG3,
              class ARG4,  class ARG5>
    ConstructTestTypeAlloc(BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)  a4,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG5)  a5,
                           bslmf::MovableRef<Arg6>                  a6,
                           bslma::Allocator *allocator = 0)
        : d_ilsum(0), d_allocator_p(allocator),
          d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1)),
          d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2)),
          d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3)),
          d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4)),
          d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5)),
          d_a6(MovUtl::move(a6)){}

    template <class ARG1,  class ARG2,  class ARG3,
              class ARG4,  class ARG5,  class ARG6>
    ConstructTestTypeAlloc(BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)  a4,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG5)  a5,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG6)  a6,
                           const Arg7                              &a7,
                           bslma::Allocator *allocator = 0)
        : d_ilsum(0), d_allocator_p(allocator),
          d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1)),
          d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2)),
          d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3)),
          d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4)),
          d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5)),
          d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6)),
          d_a7(a7){}
    template <class ARG1,  class ARG2,  class ARG3,
              class ARG4,  class ARG5,  class ARG6>
    ConstructTestTypeAlloc(BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)  a4,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG5)  a5,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG6)  a6,
                           bslmf::MovableRef<Arg7>                  a7,
                           bslma::Allocator *allocator = 0)
        : d_ilsum(0), d_allocator_p(allocator),
          d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1)),
          d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2)),
          d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3)),
          d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4)),
          d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5)),
          d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6)),
          d_a7(MovUtl::move(a7)){}

    template <class ARG1,  class ARG2,  class ARG3,
              class ARG4,  class ARG5,  class ARG6,
              class ARG7>
    ConstructTestTypeAlloc(BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)  a4,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG5)  a5,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG6)  a6,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG7)  a7,
                           const Arg8                              &a8,
                           bslma::Allocator *allocator = 0)
        : d_ilsum(0), d_allocator_p(allocator),
          d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1)),
          d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2)),
          d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3)),
          d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4)),
          d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5)),
          d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6)),
          d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7)),
          d_a8(a8){}
    template <class ARG1,  class ARG2,  class ARG3,
              class ARG4,  class ARG5,  class ARG6,
              class ARG7>
    ConstructTestTypeAlloc(BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)  a4,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG5)  a5,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG6)  a6,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG7)  a7,
                           bslmf::MovableRef<Arg8>                  a8,
                           bslma::Allocator *allocator = 0)
        : d_ilsum(0), d_allocator_p(allocator),
          d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1)),
          d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2)),
          d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3)),
          d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4)),
          d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5)),
          d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6)),
          d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7)),
          d_a8(MovUtl::move(a8)){}

    template <class ARG1,  class ARG2,  class ARG3,
              class ARG4,  class ARG5,  class ARG6,
              class ARG7,  class ARG8>
    ConstructTestTypeAlloc(BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)  a4,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG5)  a5,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG6)  a6,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG7)  a7,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG8)  a8,
                           const Arg9                              &a9,
                           bslma::Allocator *allocator = 0)
        : d_ilsum(0), d_allocator_p(allocator),
          d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1)),
          d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2)),
          d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3)),
          d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4)),
          d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5)),
          d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6)),
          d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7)),
          d_a8(BSLS_COMPILERFEATURES_FORWARD(ARG8, a8)),
          d_a9(a9){}
    template <class ARG1,  class ARG2,  class ARG3,
              class ARG4,  class ARG5,  class ARG6,
              class ARG7,  class ARG8>
    ConstructTestTypeAlloc(BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)  a4,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG5)  a5,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG6)  a6,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG7)  a7,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG8)  a8,
                           bslmf::MovableRef<Arg9>                  a9,
                           bslma::Allocator *allocator = 0)
        : d_ilsum(0), d_allocator_p(allocator),
          d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1)),
          d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2)),
          d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3)),
          d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4)),
          d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5)),
          d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6)),
          d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7)),
          d_a8(BSLS_COMPILERFEATURES_FORWARD(ARG8, a8)),
          d_a9(MovUtl::move(a9)){}

    template <class ARG1,  class ARG2,  class ARG3,
              class ARG4,  class ARG5,  class ARG6,
              class ARG7,  class ARG8,  class ARG9>
    ConstructTestTypeAlloc(BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)  a4,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG5)  a5,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG6)  a6,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG7)  a7,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG8)  a8,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG9)  a9,
                           const Arg10                             &a10,
                           bslma::Allocator *allocator = 0)
        : d_ilsum(0), d_allocator_p(allocator),
          d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1)),
          d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2)),
          d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3)),
          d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4)),
          d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5)),
          d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6)),
          d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7)),
          d_a8(BSLS_COMPILERFEATURES_FORWARD(ARG8, a8)),
          d_a9(BSLS_COMPILERFEATURES_FORWARD(ARG9, a9)),
          d_a10(a10){}
    template <class ARG1,  class ARG2,  class ARG3,
              class ARG4,  class ARG5,  class ARG6,
              class ARG7,  class ARG8,  class ARG9>
    ConstructTestTypeAlloc(BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)  a4,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG5)  a5,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG6)  a6,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG7)  a7,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG8)  a8,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG9)  a9,
                           bslmf::MovableRef<Arg10>                 a10,
                           bslma::Allocator *allocator = 0)
        : d_ilsum(0), d_allocator_p(allocator),
          d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1)),
          d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2)),
          d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3)),
          d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4)),
          d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5)),
          d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6)),
          d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7)),
          d_a8(BSLS_COMPILERFEATURES_FORWARD(ARG8, a8)),
          d_a9(BSLS_COMPILERFEATURES_FORWARD(ARG9, a9)),
          d_a10(MovUtl::move(a10)){}

    template <class ARG1,  class ARG2,  class ARG3,
              class ARG4,  class ARG5,  class ARG6,
              class ARG7,  class ARG8,  class ARG9,
              class ARG10>
    ConstructTestTypeAlloc(BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)  a4,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG5)  a5,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG6)  a6,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG7)  a7,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG8)  a8,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG9)  a9,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG10) a10,
                           const Arg11                             &a11,
                           bslma::Allocator *allocator = 0)
        : d_ilsum(0), d_allocator_p(allocator),
          d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1)),
          d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2)),
          d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3)),
          d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4)),
          d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5)),
          d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6)),
          d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7)),
          d_a8(BSLS_COMPILERFEATURES_FORWARD(ARG8, a8)),
          d_a9(BSLS_COMPILERFEATURES_FORWARD(ARG9, a9)),
          d_a10(BSLS_COMPILERFEATURES_FORWARD(ARG10, a10)),
          d_a11(a11){}
    template <class ARG1,  class ARG2,  class ARG3,
              class ARG4,  class ARG5,  class ARG6,
              class ARG7,  class ARG8,  class ARG9,
              class ARG10>
    ConstructTestTypeAlloc(BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)  a4,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG5)  a5,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG6)  a6,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG7)  a7,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG8)  a8,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG9)  a9,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG10) a10,
                           bslmf::MovableRef<Arg11>                 a11,
                           bslma::Allocator *allocator = 0)
        : d_ilsum(0), d_allocator_p(allocator),
          d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1)),
          d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2)),
          d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3)),
          d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4)),
          d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5)),
          d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6)),
          d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7)),
          d_a8(BSLS_COMPILERFEATURES_FORWARD(ARG8, a8)),
          d_a9(BSLS_COMPILERFEATURES_FORWARD(ARG9, a9)),
          d_a10(BSLS_COMPILERFEATURES_FORWARD(ARG10, a10)),
          d_a11(MovUtl::move(a11)){}

    template <class ARG1,  class ARG2,  class ARG3,
              class ARG4,  class ARG5,  class ARG6,
              class ARG7,  class ARG8,  class ARG9,
              class ARG10, class ARG11>
    ConstructTestTypeAlloc(BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)  a4,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG5)  a5,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG6)  a6,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG7)  a7,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG8)  a8,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG9)  a9,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG10) a10,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG11) a11,
                           const Arg12                             &a12,
                           bslma::Allocator *allocator = 0)
        : d_ilsum(0), d_allocator_p(allocator),
          d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1)),
          d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2)),
          d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3)),
          d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4)),
          d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5)),
          d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6)),
          d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7)),
          d_a8(BSLS_COMPILERFEATURES_FORWARD(ARG8, a8)),
          d_a9(BSLS_COMPILERFEATURES_FORWARD(ARG9, a9)),
          d_a10(BSLS_COMPILERFEATURES_FORWARD(ARG10, a10)),
          d_a11(BSLS_COMPILERFEATURES_FORWARD(ARG11, a11)),
          d_a12(a12){}
    template <class ARG1,  class ARG2,  class ARG3,
              class ARG4,  class ARG5,  class ARG6,
              class ARG7,  class ARG8,  class ARG9,
              class ARG10, class ARG11>
    ConstructTestTypeAlloc(BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)  a4,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG5)  a5,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG6)  a6,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG7)  a7,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG8)  a8,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG9)  a9,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG10) a10,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG11) a11,
                           bslmf::MovableRef<Arg12>                 a12,
                           bslma::Allocator *allocator = 0)
        : d_ilsum(0), d_allocator_p(allocator),
          d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1)),
          d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2)),
          d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3)),
          d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4)),
          d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5)),
          d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6)),
          d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7)),
          d_a8(BSLS_COMPILERFEATURES_FORWARD(ARG8, a8)),
          d_a9(BSLS_COMPILERFEATURES_FORWARD(ARG9, a9)),
          d_a10(BSLS_COMPILERFEATURES_FORWARD(ARG10, a10)),
          d_a11(BSLS_COMPILERFEATURES_FORWARD(ARG11, a11)),
          d_a12(MovUtl::move(a12)){}

    template <class ARG1,  class ARG2,  class ARG3,
              class ARG4,  class ARG5,  class ARG6,
              class ARG7,  class ARG8,  class ARG9,
              class ARG10, class ARG11, class ARG12>
    ConstructTestTypeAlloc(BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)  a4,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG5)  a5,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG6)  a6,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG7)  a7,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG8)  a8,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG9)  a9,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG10) a10,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG11) a11,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG12) a12,
                           const Arg13                             &a13,
                           bslma::Allocator *allocator = 0)
        : d_ilsum(0), d_allocator_p(allocator),
          d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1)),
          d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2)),
          d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3)),
          d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4)),
          d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5)),
          d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6)),
          d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7)),
          d_a8(BSLS_COMPILERFEATURES_FORWARD(ARG8, a8)),
          d_a9(BSLS_COMPILERFEATURES_FORWARD(ARG9, a9)),
          d_a10(BSLS_COMPILERFEATURES_FORWARD(ARG10, a10)),
          d_a11(BSLS_COMPILERFEATURES_FORWARD(ARG11, a11)),
          d_a12(BSLS_COMPILERFEATURES_FORWARD(ARG12, a12)),
          d_a13(a13){}
    template <class ARG1,  class ARG2,  class ARG3,
              class ARG4,  class ARG5,  class ARG6,
              class ARG7,  class ARG8,  class ARG9,
              class ARG10, class ARG11, class ARG12>
    ConstructTestTypeAlloc(BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)  a4,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG5)  a5,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG6)  a6,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG7)  a7,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG8)  a8,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG9)  a9,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG10) a10,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG11) a11,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG12) a12,
                           bslmf::MovableRef<Arg13>                 a13,
                           bslma::Allocator *allocator = 0)
        : d_ilsum(0), d_allocator_p(allocator),
          d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1)),
          d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2)),
          d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3)),
          d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4)),
          d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5)),
          d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6)),
          d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7)),
          d_a8(BSLS_COMPILERFEATURES_FORWARD(ARG8, a8)),
          d_a9(BSLS_COMPILERFEATURES_FORWARD(ARG9, a9)),
          d_a10(BSLS_COMPILERFEATURES_FORWARD(ARG10, a10)),
          d_a11(BSLS_COMPILERFEATURES_FORWARD(ARG11, a11)),
          d_a12(BSLS_COMPILERFEATURES_FORWARD(ARG12, a12)),
          d_a13(MovUtl::move(a13)){}

    template <class ARG1,  class ARG2,  class ARG3,
                      class ARG4,  class ARG5,  class ARG6,
                      class ARG7,  class ARG8,  class ARG9,
                      class ARG10, class ARG11, class ARG12,
                      class ARG13>
    ConstructTestTypeAlloc(BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)  a4,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG5)  a5,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG6)  a6,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG7)  a7,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG8)  a8,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG9)  a9,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG10) a10,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG11) a11,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG12) a12,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG13) a13,
                           const Arg14                             &a14,
                           bslma::Allocator *allocator = 0)
        : d_ilsum(0), d_allocator_p(allocator),
          d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1)),
          d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2)),
          d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3)),
          d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4)),
          d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5)),
          d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6)),
          d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7)),
          d_a8(BSLS_COMPILERFEATURES_FORWARD(ARG8, a8)),
          d_a9(BSLS_COMPILERFEATURES_FORWARD(ARG9, a9)),
          d_a10(BSLS_COMPILERFEATURES_FORWARD(ARG10, a10)),
          d_a11(BSLS_COMPILERFEATURES_FORWARD(ARG11, a11)),
          d_a12(BSLS_COMPILERFEATURES_FORWARD(ARG12, a12)),
          d_a13(BSLS_COMPILERFEATURES_FORWARD(ARG13, a13)),
          d_a14(a14) {}
    template <class ARG1,  class ARG2,  class ARG3,
                      class ARG4,  class ARG5,  class ARG6,
                      class ARG7,  class ARG8,  class ARG9,
                      class ARG10, class ARG11, class ARG12,
                      class ARG13>
    ConstructTestTypeAlloc(BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)  a4,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG5)  a5,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG6)  a6,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG7)  a7,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG8)  a8,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG9)  a9,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG10) a10,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG11) a11,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG12) a12,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG13) a13,
                           bslmf::MovableRef<Arg14>                 a14,
                           bslma::Allocator *allocator = 0)
        : d_ilsum(0), d_allocator_p(allocator),
          d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1)),
          d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2)),
          d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3)),
          d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4)),
          d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5)),
          d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6)),
          d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7)),
          d_a8(BSLS_COMPILERFEATURES_FORWARD(ARG8, a8)),
          d_a9(BSLS_COMPILERFEATURES_FORWARD(ARG9, a9)),
          d_a10(BSLS_COMPILERFEATURES_FORWARD(ARG10, a10)),
          d_a11(BSLS_COMPILERFEATURES_FORWARD(ARG11, a11)),
          d_a12(BSLS_COMPILERFEATURES_FORWARD(ARG12, a12)),
          d_a13(BSLS_COMPILERFEATURES_FORWARD(ARG13, a13)),
          d_a14(MovUtl::move(a14)){}

#ifdef BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS
    explicit
    ConstructTestTypeAlloc(std::initializer_list<int> il,
                           bslma::Allocator *allocator = 0)
        : d_ilsum(0), d_allocator_p(allocator)
    {
      for (int i : il) d_ilsum+=i;
    }
    explicit
    ConstructTestTypeAlloc(std::initializer_list<int> il,
                           const Arg1 &  a1,
                           bslma::Allocator *allocator = 0)
        : d_ilsum(0), d_allocator_p(allocator)
        , d_a1(a1)
    {
      for (int i : il) d_ilsum+=i;
    }
    explicit
    ConstructTestTypeAlloc(std::initializer_list<int> il,
                           bslmf::MovableRef<Arg1>  a1,
                           bslma::Allocator *allocator = 0)
            : d_ilsum(0), d_allocator_p(allocator)
            , d_a1(MovUtl::move(a1))
    {
      for (int i : il) d_ilsum+=i;
    }

    template <class ARG1>
    explicit
    ConstructTestTypeAlloc(std::initializer_list<int> il,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                           const Arg2 &                             a2,
                           bslma::Allocator             *allocator = 0)
        : d_ilsum(0), d_allocator_p(allocator),
          d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1)),
          d_a2(a2)
    {
      for (int i : il) d_ilsum+=i;
    }

    template <class ARG1>
    explicit
    ConstructTestTypeAlloc(std::initializer_list<int> il,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                           bslmf::MovableRef<Arg2>                  a2,
                           bslma::Allocator *allocator              = 0)
            : d_ilsum(0), d_allocator_p(allocator),
              d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1)),
              d_a2(MovUtl::move(a2))
    {
      for (int i : il) d_ilsum+=i;
    }


    template <class ARG1,  class ARG2>
    ConstructTestTypeAlloc(std::initializer_list<int> il,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                           const Arg3                              &a3,
                           bslma::Allocator *allocator              = 0)
        : d_ilsum(0), d_allocator_p(allocator),
          d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1)),
          d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2)),
          d_a3(a3)
    {
      for (int i : il) d_ilsum+=i;
    }

    template <class ARG1,  class ARG2>
    ConstructTestTypeAlloc(std::initializer_list<int> il,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                           bslmf::MovableRef<Arg3>                  a3,
                           bslma::Allocator *allocator = 0)
        : d_ilsum(0), d_allocator_p(allocator),
          d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1)),
          d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2)),
          d_a3(MovUtl::move(a3))
    {
      for (int i : il) d_ilsum+=i;
    }

    template <class ARG1,  class ARG2,  class ARG3>
    ConstructTestTypeAlloc(std::initializer_list<int> il,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
                           const Arg4                              &a4,
                           bslma::Allocator *allocator = 0)
        : d_ilsum(0), d_allocator_p(allocator),
          d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1)),
          d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2)),
          d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3)),
          d_a4(a4)
    {
      for (int i : il) d_ilsum+=i;
    }

    template <class ARG1,  class ARG2,  class ARG3>
    ConstructTestTypeAlloc(std::initializer_list<int> il,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
                           bslmf::MovableRef<Arg4>                  a4,
                           bslma::Allocator *allocator = 0)
        : d_ilsum(0), d_allocator_p(allocator),
          d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1)),
          d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2)),
          d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3)),
          d_a4(MovUtl::move(a4))
    {
      for (int i : il) d_ilsum+=i;
    }

    template <class ARG1,  class ARG2,  class ARG3,
              class ARG4>
    ConstructTestTypeAlloc(std::initializer_list<int> il,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)  a4,
                           const Arg5                              &a5,
                           bslma::Allocator *allocator = 0)
        : d_ilsum(0), d_allocator_p(allocator),
          d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1)),
          d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2)),
          d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3)),
          d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4)),
          d_a5(a5)
    {
      for (int i : il) d_ilsum+=i;
    }
    template <class ARG1,  class ARG2,  class ARG3,
              class ARG4>
    ConstructTestTypeAlloc(std::initializer_list<int> il,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)  a4,
                           bslmf::MovableRef<Arg5>                  a5,
                           bslma::Allocator *allocator = 0)
        : d_ilsum(0), d_allocator_p(allocator),
          d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1)),
          d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2)),
          d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3)),
          d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4)),
          d_a5(MovUtl::move(a5))
    {
      for (int i : il) d_ilsum+=i;
    }

    template <class ARG1,  class ARG2,  class ARG3,
              class ARG4,  class ARG5>
    ConstructTestTypeAlloc(std::initializer_list<int> il,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)  a4,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG5)  a5,
                           const Arg6                              &a6,
                           bslma::Allocator *allocator = 0)
        : d_ilsum(0), d_allocator_p(allocator),
          d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1)),
          d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2)),
          d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3)),
          d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4)),
          d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5)),
          d_a6(a6)
    {
      for (int i : il) d_ilsum+=i;
    }
    template <class ARG1,  class ARG2,  class ARG3,
              class ARG4,  class ARG5>
    ConstructTestTypeAlloc(std::initializer_list<int> il,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)  a4,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG5)  a5,
                           bslmf::MovableRef<Arg6>                  a6,
                           bslma::Allocator *allocator = 0)
        : d_ilsum(0), d_allocator_p(allocator),
          d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1)),
          d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2)),
          d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3)),
          d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4)),
          d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5)),
          d_a6(MovUtl::move(a6))
    {
      for (int i : il) d_ilsum+=i;
    }
    template <class ARG1,  class ARG2,  class ARG3,
              class ARG4,  class ARG5,  class ARG6>
    ConstructTestTypeAlloc(std::initializer_list<int> il,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)  a4,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG5)  a5,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG6)  a6,
                           const Arg7                              &a7,
                           bslma::Allocator *allocator = 0)
        : d_ilsum(0), d_allocator_p(allocator),
          d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1)),
          d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2)),
          d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3)),
          d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4)),
          d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5)),
          d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6)),
          d_a7(a7)
    {
      for (int i : il) d_ilsum+=i;
    }
    template <class ARG1,  class ARG2,  class ARG3,
              class ARG4,  class ARG5,  class ARG6>
    ConstructTestTypeAlloc(std::initializer_list<int> il,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)  a4,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG5)  a5,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG6)  a6,
                           bslmf::MovableRef<Arg7>                  a7,
                           bslma::Allocator *allocator = 0)
        : d_ilsum(0), d_allocator_p(allocator),
          d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1)),
          d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2)),
          d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3)),
          d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4)),
          d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5)),
          d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6)),
          d_a7(MovUtl::move(a7))
    {
      for (int i : il) d_ilsum+=i;
    }

    template <class ARG1,  class ARG2,  class ARG3,
              class ARG4,  class ARG5,  class ARG6,
              class ARG7>
    ConstructTestTypeAlloc(std::initializer_list<int> il,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)  a4,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG5)  a5,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG6)  a6,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG7)  a7,
                           const Arg8                              &a8,
                           bslma::Allocator *allocator = 0)
        : d_ilsum(0), d_allocator_p(allocator),
          d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1)),
          d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2)),
          d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3)),
          d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4)),
          d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5)),
          d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6)),
          d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7)),
          d_a8(a8)
    {
      for (int i : il) d_ilsum+=i;
    }
    template <class ARG1,  class ARG2,  class ARG3,
              class ARG4,  class ARG5,  class ARG6,
              class ARG7>
    ConstructTestTypeAlloc(std::initializer_list<int> il,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)  a4,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG5)  a5,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG6)  a6,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG7)  a7,
                           bslmf::MovableRef<Arg8>                  a8,
                           bslma::Allocator *allocator = 0)
        : d_ilsum(0), d_allocator_p(allocator),
          d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1)),
          d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2)),
          d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3)),
          d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4)),
          d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5)),
          d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6)),
          d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7)),
          d_a8(MovUtl::move(a8))
    {
      for (int i : il) d_ilsum+=i;
    }

    template <class ARG1,  class ARG2,  class ARG3,
              class ARG4,  class ARG5,  class ARG6,
              class ARG7,  class ARG8>
    ConstructTestTypeAlloc(std::initializer_list<int> il,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)  a4,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG5)  a5,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG6)  a6,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG7)  a7,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG8)  a8,
                           const Arg9                              &a9,
                           bslma::Allocator *allocator = 0)
        : d_ilsum(0), d_allocator_p(allocator),
          d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1)),
          d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2)),
          d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3)),
          d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4)),
          d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5)),
          d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6)),
          d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7)),
          d_a8(BSLS_COMPILERFEATURES_FORWARD(ARG8, a8)),
          d_a9(a9)
    {
      for (int i : il) d_ilsum+=i;
    }
    template <class ARG1,  class ARG2,  class ARG3,
              class ARG4,  class ARG5,  class ARG6,
              class ARG7,  class ARG8>
    ConstructTestTypeAlloc(std::initializer_list<int> il,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)  a4,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG5)  a5,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG6)  a6,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG7)  a7,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG8)  a8,
                           bslmf::MovableRef<Arg9>                  a9,
                           bslma::Allocator *allocator = 0)
        : d_ilsum(0), d_allocator_p(allocator),
          d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1)),
          d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2)),
          d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3)),
          d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4)),
          d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5)),
          d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6)),
          d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7)),
          d_a8(BSLS_COMPILERFEATURES_FORWARD(ARG8, a8)),
          d_a9(MovUtl::move(a9))
    {
      for (int i : il) d_ilsum+=i;
    }

    template <class ARG1,  class ARG2,  class ARG3,
              class ARG4,  class ARG5,  class ARG6,
              class ARG7,  class ARG8,  class ARG9>
    ConstructTestTypeAlloc(std::initializer_list<int> il,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)  a4,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG5)  a5,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG6)  a6,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG7)  a7,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG8)  a8,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG9)  a9,
                           const Arg10                             &a10,
                           bslma::Allocator *allocator = 0)
        : d_ilsum(0), d_allocator_p(allocator),
          d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1)),
          d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2)),
          d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3)),
          d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4)),
          d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5)),
          d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6)),
          d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7)),
          d_a8(BSLS_COMPILERFEATURES_FORWARD(ARG8, a8)),
          d_a9(BSLS_COMPILERFEATURES_FORWARD(ARG9, a9)),
          d_a10(a10)
    {
      for (int i : il) d_ilsum+=i;
    }
    template <class ARG1,  class ARG2,  class ARG3,
              class ARG4,  class ARG5,  class ARG6,
              class ARG7,  class ARG8,  class ARG9>
    ConstructTestTypeAlloc(std::initializer_list<int> il,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)  a4,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG5)  a5,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG6)  a6,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG7)  a7,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG8)  a8,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG9)  a9,
                           bslmf::MovableRef<Arg10>                 a10,
                           bslma::Allocator *allocator = 0)
        : d_ilsum(0), d_allocator_p(allocator),
          d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1)),
          d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2)),
          d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3)),
          d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4)),
          d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5)),
          d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6)),
          d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7)),
          d_a8(BSLS_COMPILERFEATURES_FORWARD(ARG8, a8)),
          d_a9(BSLS_COMPILERFEATURES_FORWARD(ARG9, a9)),
          d_a10(MovUtl::move(a10))
    {
      for (int i : il) d_ilsum+=i;
    }

    template <class ARG1,  class ARG2,  class ARG3,
              class ARG4,  class ARG5,  class ARG6,
              class ARG7,  class ARG8,  class ARG9,
              class ARG10>
    ConstructTestTypeAlloc(std::initializer_list<int> il,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)  a4,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG5)  a5,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG6)  a6,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG7)  a7,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG8)  a8,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG9)  a9,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG10) a10,
                           const Arg11                             &a11,
                           bslma::Allocator *allocator = 0)
        : d_ilsum(0), d_allocator_p(allocator),
          d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1)),
          d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2)),
          d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3)),
          d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4)),
          d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5)),
          d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6)),
          d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7)),
          d_a8(BSLS_COMPILERFEATURES_FORWARD(ARG8, a8)),
          d_a9(BSLS_COMPILERFEATURES_FORWARD(ARG9, a9)),
          d_a10(BSLS_COMPILERFEATURES_FORWARD(ARG10, a10)),
          d_a11(a11)
    {
      for (int i : il) d_ilsum+=i;
    }
    template <class ARG1,  class ARG2,  class ARG3,
              class ARG4,  class ARG5,  class ARG6,
              class ARG7,  class ARG8,  class ARG9,
              class ARG10>
    ConstructTestTypeAlloc(std::initializer_list<int> il,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)  a4,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG5)  a5,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG6)  a6,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG7)  a7,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG8)  a8,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG9)  a9,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG10) a10,
                           bslmf::MovableRef<Arg11>                 a11,
                           bslma::Allocator *allocator = 0)
        : d_ilsum(0), d_allocator_p(allocator),
          d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1)),
          d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2)),
          d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3)),
          d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4)),
          d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5)),
          d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6)),
          d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7)),
          d_a8(BSLS_COMPILERFEATURES_FORWARD(ARG8, a8)),
          d_a9(BSLS_COMPILERFEATURES_FORWARD(ARG9, a9)),
          d_a10(BSLS_COMPILERFEATURES_FORWARD(ARG10, a10)),
          d_a11(MovUtl::move(a11))
    {
      for (int i : il) d_ilsum+=i;
    }

    template <class ARG1,  class ARG2,  class ARG3,
              class ARG4,  class ARG5,  class ARG6,
              class ARG7,  class ARG8,  class ARG9,
              class ARG10, class ARG11>
    ConstructTestTypeAlloc(std::initializer_list<int> il,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)  a4,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG5)  a5,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG6)  a6,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG7)  a7,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG8)  a8,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG9)  a9,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG10) a10,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG11) a11,
                           const Arg12                             &a12,
                           bslma::Allocator *allocator = 0)
        : d_ilsum(0), d_allocator_p(allocator),
          d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1)),
          d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2)),
          d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3)),
          d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4)),
          d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5)),
          d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6)),
          d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7)),
          d_a8(BSLS_COMPILERFEATURES_FORWARD(ARG8, a8)),
          d_a9(BSLS_COMPILERFEATURES_FORWARD(ARG9, a9)),
          d_a10(BSLS_COMPILERFEATURES_FORWARD(ARG10, a10)),
          d_a11(BSLS_COMPILERFEATURES_FORWARD(ARG11, a11)),
          d_a12(a12)
    {
      for (int i : il) d_ilsum+=i;
    }
    template <class ARG1,  class ARG2,  class ARG3,
              class ARG4,  class ARG5,  class ARG6,
              class ARG7,  class ARG8,  class ARG9,
              class ARG10, class ARG11>
    ConstructTestTypeAlloc(std::initializer_list<int> il,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)  a4,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG5)  a5,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG6)  a6,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG7)  a7,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG8)  a8,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG9)  a9,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG10) a10,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG11) a11,
                           bslmf::MovableRef<Arg12>                 a12,
                           bslma::Allocator *allocator = 0)
        : d_ilsum(0), d_allocator_p(allocator),
          d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1)),
          d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2)),
          d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3)),
          d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4)),
          d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5)),
          d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6)),
          d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7)),
          d_a8(BSLS_COMPILERFEATURES_FORWARD(ARG8, a8)),
          d_a9(BSLS_COMPILERFEATURES_FORWARD(ARG9, a9)),
          d_a10(BSLS_COMPILERFEATURES_FORWARD(ARG10, a10)),
          d_a11(BSLS_COMPILERFEATURES_FORWARD(ARG11, a11)),
          d_a12(MovUtl::move(a12))
    {
      for (int i : il) d_ilsum+=i;
    }

    template <class ARG1,  class ARG2,  class ARG3,
              class ARG4,  class ARG5,  class ARG6,
              class ARG7,  class ARG8,  class ARG9,
              class ARG10, class ARG11, class ARG12>
    ConstructTestTypeAlloc(std::initializer_list<int> il,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)  a4,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG5)  a5,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG6)  a6,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG7)  a7,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG8)  a8,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG9)  a9,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG10) a10,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG11) a11,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG12) a12,
                           const Arg13                             &a13,
                           bslma::Allocator *allocator = 0)
        : d_ilsum(0), d_allocator_p(allocator),
          d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1)),
          d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2)),
          d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3)),
          d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4)),
          d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5)),
          d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6)),
          d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7)),
          d_a8(BSLS_COMPILERFEATURES_FORWARD(ARG8, a8)),
          d_a9(BSLS_COMPILERFEATURES_FORWARD(ARG9, a9)),
          d_a10(BSLS_COMPILERFEATURES_FORWARD(ARG10, a10)),
          d_a11(BSLS_COMPILERFEATURES_FORWARD(ARG11, a11)),
          d_a12(BSLS_COMPILERFEATURES_FORWARD(ARG12, a12)),
          d_a13(a13)
    {
      for (int i : il) d_ilsum+=i;
    }
    template <class ARG1,  class ARG2,  class ARG3,
              class ARG4,  class ARG5,  class ARG6,
              class ARG7,  class ARG8,  class ARG9,
              class ARG10, class ARG11, class ARG12>
    ConstructTestTypeAlloc(std::initializer_list<int> il,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)  a4,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG5)  a5,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG6)  a6,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG7)  a7,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG8)  a8,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG9)  a9,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG10) a10,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG11) a11,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG12) a12,
                           bslmf::MovableRef<Arg13>                 a13,
                           bslma::Allocator *allocator = 0)
        : d_ilsum(0), d_allocator_p(allocator),
          d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1)),
          d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2)),
          d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3)),
          d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4)),
          d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5)),
          d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6)),
          d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7)),
          d_a8(BSLS_COMPILERFEATURES_FORWARD(ARG8, a8)),
          d_a9(BSLS_COMPILERFEATURES_FORWARD(ARG9, a9)),
          d_a10(BSLS_COMPILERFEATURES_FORWARD(ARG10, a10)),
          d_a11(BSLS_COMPILERFEATURES_FORWARD(ARG11, a11)),
          d_a12(BSLS_COMPILERFEATURES_FORWARD(ARG12, a12)),
          d_a13(MovUtl::move(a13))
    {
      for (int i : il) d_ilsum+=i;
    }

    template <class ARG1,  class ARG2,  class ARG3,
                      class ARG4,  class ARG5,  class ARG6,
                      class ARG7,  class ARG8,  class ARG9,
                      class ARG10, class ARG11, class ARG12,
                      class ARG13>
    ConstructTestTypeAlloc(std::initializer_list<int> il,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)  a4,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG5)  a5,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG6)  a6,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG7)  a7,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG8)  a8,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG9)  a9,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG10) a10,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG11) a11,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG12) a12,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG13) a13,
                           const Arg14                             &a14,
                           bslma::Allocator *allocator = 0)
        : d_ilsum(0), d_allocator_p(allocator),
          d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1)),
          d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2)),
          d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3)),
          d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4)),
          d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5)),
          d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6)),
          d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7)),
          d_a8(BSLS_COMPILERFEATURES_FORWARD(ARG8, a8)),
          d_a9(BSLS_COMPILERFEATURES_FORWARD(ARG9, a9)),
          d_a10(BSLS_COMPILERFEATURES_FORWARD(ARG10, a10)),
          d_a11(BSLS_COMPILERFEATURES_FORWARD(ARG11, a11)),
          d_a12(BSLS_COMPILERFEATURES_FORWARD(ARG12, a12)),
          d_a13(BSLS_COMPILERFEATURES_FORWARD(ARG13, a13)),
          d_a14(a14)
    {
      for (int i : il) d_ilsum+=i;
    }
    template <class ARG1,  class ARG2,  class ARG3,
                      class ARG4,  class ARG5,  class ARG6,
                      class ARG7,  class ARG8,  class ARG9,
                      class ARG10, class ARG11, class ARG12,
                      class ARG13>
    ConstructTestTypeAlloc(std::initializer_list<int> il,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)  a4,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG5)  a5,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG6)  a6,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG7)  a7,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG8)  a8,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG9)  a9,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG10) a10,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG11) a11,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG12) a12,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG13) a13,
                           bslmf::MovableRef<Arg14>                 a14,
                           bslma::Allocator *allocator = 0)
        : d_ilsum(0), d_allocator_p(allocator),
          d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1)),
          d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2)),
          d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3)),
          d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4)),
          d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5)),
          d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6)),
          d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7)),
          d_a8(BSLS_COMPILERFEATURES_FORWARD(ARG8, a8)),
          d_a9(BSLS_COMPILERFEATURES_FORWARD(ARG9, a9)),
          d_a10(BSLS_COMPILERFEATURES_FORWARD(ARG10, a10)),
          d_a11(BSLS_COMPILERFEATURES_FORWARD(ARG11, a11)),
          d_a12(BSLS_COMPILERFEATURES_FORWARD(ARG12, a12)),
          d_a13(BSLS_COMPILERFEATURES_FORWARD(ARG13, a13)),
          d_a14(MovUtl::move(a14))
    {
      for (int i : il) d_ilsum+=i;
    }
#endif //BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS
};

// TRAITS
namespace BloombergLP {
namespace bslma {

template <>
struct UsesBslmaAllocator<ConstructTestTypeAlloc> : bsl::true_type { };

}  // close namespace bslma
}  // close enterprise namespace
int ConstructTestTypeAlloc::copyConstructorInvocations       = 0;
int ConstructTestTypeAlloc::moveConstructorInvocations       = 0;
// FREE OPERATORS
bool operator==(const ConstructTestTypeAlloc& lhs,
                const ConstructTestTypeAlloc& rhs)
{
    return lhs.d_ilsum  == rhs.d_ilsum &&
           lhs.d_a1.d_value  == rhs.d_a1.d_value &&
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
bool createdAlike(const ConstructTestTypeAlloc& lhs,
                  const ConstructTestTypeAlloc& rhs)
{
    return lhs.d_a1.d_copied_count  == rhs.d_a1.d_copied_count &&
           lhs.d_a2.d_copied_count  == rhs.d_a2.d_copied_count &&
           lhs.d_a3.d_copied_count  == rhs.d_a3.d_copied_count &&
           lhs.d_a4.d_copied_count  == rhs.d_a4.d_copied_count &&
           lhs.d_a5.d_copied_count  == rhs.d_a5.d_copied_count &&
           lhs.d_a6.d_copied_count  == rhs.d_a6.d_copied_count &&
           lhs.d_a7.d_copied_count  == rhs.d_a7.d_copied_count &&
           lhs.d_a8.d_copied_count  == rhs.d_a8.d_copied_count &&
           lhs.d_a9.d_copied_count  == rhs.d_a9.d_copied_count &&
           lhs.d_a10.d_copied_count == rhs.d_a10.d_copied_count &&
           lhs.d_a11.d_copied_count == rhs.d_a11.d_copied_count &&
           lhs.d_a12.d_copied_count == rhs.d_a12.d_copied_count &&
           lhs.d_a13.d_copied_count == rhs.d_a13.d_copied_count &&
           lhs.d_a14.d_copied_count == rhs.d_a14.d_copied_count;
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
    static int copyConstructorInvocations;
    static int moveConstructorInvocations;

    bslma::Allocator *d_allocator_p;
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
    ConstructTestTypeAllocArgT()
            : d_allocator_p(0){}

    ConstructTestTypeAllocArgT(ConstructTestTypeAllocArgT const& other)
    : d_allocator_p(0)
     , d_a1 (other.d_a1),  d_a2 (other.d_a2),  d_a3 (other.d_a3)
     , d_a4 (other.d_a4),  d_a5 (other.d_a5),  d_a6 (other.d_a6)
     , d_a7 (other.d_a7),  d_a8 (other.d_a8),  d_a9 (other.d_a9)
     , d_a10(other.d_a10), d_a11(other.d_a11), d_a12(other.d_a12)
     , d_a13(other.d_a13), d_a14(other.d_a14)
    {
       ++copyConstructorInvocations;
    }

    ConstructTestTypeAllocArgT(
              bslmf::MovableRef<ConstructTestTypeAllocArgT>  other)
       : d_allocator_p(MovUtl::access(other).d_allocator_p),
         d_a1 (MovUtl::move(MovUtl::access(other).d_a1)),
         d_a2 (MovUtl::move(MovUtl::access(other).d_a2)),
         d_a3 (MovUtl::move(MovUtl::access(other).d_a3)),
         d_a4 (MovUtl::move(MovUtl::access(other).d_a4)),
         d_a5 (MovUtl::move(MovUtl::access(other).d_a5)),
         d_a6 (MovUtl::move(MovUtl::access(other).d_a6)),
         d_a7 (MovUtl::move(MovUtl::access(other).d_a7)),
         d_a8 (MovUtl::move(MovUtl::access(other).d_a8)),
         d_a9 (MovUtl::move(MovUtl::access(other).d_a9)),
         d_a10(MovUtl::move(MovUtl::access(other).d_a10)),
         d_a11(MovUtl::move(MovUtl::access(other).d_a11)),
         d_a12(MovUtl::move(MovUtl::access(other).d_a12)),
         d_a13(MovUtl::move(MovUtl::access(other).d_a13)),
         d_a14(MovUtl::move(MovUtl::access(other).d_a14))
    {
       ++moveConstructorInvocations;
    }
    ConstructTestTypeAllocArgT(bsl::allocator_arg_t       ,
                               bslma::Allocator     *alloc,
                               ConstructTestTypeAllocArgT const& other)
    : d_allocator_p(alloc)
     , d_a1 (other.d_a1),  d_a2 (other.d_a2),  d_a3 (other.d_a3)
     , d_a4 (other.d_a4),  d_a5 (other.d_a5),  d_a6 (other.d_a6)
     , d_a7 (other.d_a7),  d_a8 (other.d_a8),  d_a9 (other.d_a9)
     , d_a10(other.d_a10), d_a11(other.d_a11), d_a12(other.d_a12)
     , d_a13(other.d_a13), d_a14(other.d_a14)
    {
       ++copyConstructorInvocations;
    }

    ConstructTestTypeAllocArgT(bsl::allocator_arg_t       ,
                          bslma::Allocator     *alloc,
                          bslmf::MovableRef<ConstructTestTypeAllocArgT>  other)
       : d_allocator_p(alloc),
         d_a1 (MovUtl::move(MovUtl::access(other).d_a1)),
         d_a2 (MovUtl::move(MovUtl::access(other).d_a2)),
         d_a3 (MovUtl::move(MovUtl::access(other).d_a3)),
         d_a4 (MovUtl::move(MovUtl::access(other).d_a4)),
         d_a5 (MovUtl::move(MovUtl::access(other).d_a5)),
         d_a6 (MovUtl::move(MovUtl::access(other).d_a6)),
         d_a7 (MovUtl::move(MovUtl::access(other).d_a7)),
         d_a8 (MovUtl::move(MovUtl::access(other).d_a8)),
         d_a9 (MovUtl::move(MovUtl::access(other).d_a9)),
         d_a10(MovUtl::move(MovUtl::access(other).d_a10)),
         d_a11(MovUtl::move(MovUtl::access(other).d_a11)),
         d_a12(MovUtl::move(MovUtl::access(other).d_a12)),
         d_a13(MovUtl::move(MovUtl::access(other).d_a13)),
         d_a14(MovUtl::move(MovUtl::access(other).d_a14))
    {
       ++moveConstructorInvocations;
    }

    ConstructTestTypeAllocArgT(bsl::allocator_arg_t       ,
                               bslma::Allocator     *alloc)
        : d_allocator_p(alloc){}

    ConstructTestTypeAllocArgT(bsl::allocator_arg_t       ,
                               bslma::Allocator     *alloc,
                               const Arg1           &a1)
        : d_allocator_p(alloc),
          d_a1(a1){}

    ConstructTestTypeAllocArgT(bsl::allocator_arg_t       ,
                               bslma::Allocator     *alloc,
                               bslmf::MovableRef<Arg1>  a1)
        : d_allocator_p(alloc),
          d_a1(MovUtl::move(a1)){}

    template <class ARG1,  class ARG2>
    ConstructTestTypeAllocArgT(bsl::allocator_arg_t       ,
                               bslma::Allocator     *alloc,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2)
        : d_allocator_p(alloc),
          d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1)),
          d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2)) {}

    template <class ARG1,  class ARG2,  class ARG3>
    ConstructTestTypeAllocArgT(bsl::allocator_arg_t       ,
                               bslma::Allocator     *alloc,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3)
        : d_allocator_p(alloc),
          d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1)),
          d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2)),
          d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3)){}

    template <class ARG1,  class ARG2,  class ARG3,
              class ARG4>
    ConstructTestTypeAllocArgT(bsl::allocator_arg_t       ,
                               bslma::Allocator     *alloc,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)  a4)
        : d_allocator_p(alloc),
          d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1)),
          d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2)),
          d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3)),
          d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4)) {}

    template <class ARG1,  class ARG2,  class ARG3,
              class ARG4,  class ARG5>
    ConstructTestTypeAllocArgT(bsl::allocator_arg_t       ,
                               bslma::Allocator     *alloc,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)  a4,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG5)  a5)
        : d_allocator_p(alloc),
          d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1)),
          d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2)),
          d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3)),
          d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4)),
          d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5)){}

    template <class ARG1,  class ARG2,  class ARG3,
              class ARG4,  class ARG5,  class ARG6>
    ConstructTestTypeAllocArgT(bsl::allocator_arg_t       ,
                               bslma::Allocator     *alloc,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)  a4,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG5)  a5,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG6)  a6)
        : d_allocator_p(alloc),
          d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1)),
          d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2)),
          d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3)),
          d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4)),
          d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5)),
          d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6)) {}

    template <class ARG1,  class ARG2,  class ARG3,
              class ARG4,  class ARG5,  class ARG6,
              class ARG7>
    ConstructTestTypeAllocArgT(bsl::allocator_arg_t       ,
                               bslma::Allocator     *alloc,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)  a4,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG5)  a5,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG6)  a6,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG7)  a7)
        : d_allocator_p(alloc),
          d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1)),
          d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2)),
          d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3)),
          d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4)),
          d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5)),
          d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6)),
          d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7)){}

    template <class ARG1,  class ARG2,  class ARG3,
              class ARG4,  class ARG5,  class ARG6,
              class ARG7,  class ARG8>
    ConstructTestTypeAllocArgT(bsl::allocator_arg_t       ,
                               bslma::Allocator     *alloc,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)  a4,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG5)  a5,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG6)  a6,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG7)  a7,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG8)  a8)
        : d_allocator_p(alloc),
          d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1)),
          d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2)),
          d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3)),
          d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4)),
          d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5)),
          d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6)),
          d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7)),
          d_a8(BSLS_COMPILERFEATURES_FORWARD(ARG8, a8)) {}

    template <class ARG1,  class ARG2,  class ARG3,
              class ARG4,  class ARG5,  class ARG6,
              class ARG7,  class ARG8,  class ARG9>
    ConstructTestTypeAllocArgT(bsl::allocator_arg_t       ,
                               bslma::Allocator     *alloc,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)  a4,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG5)  a5,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG6)  a6,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG7)  a7,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG8)  a8,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG9)  a9)
        : d_allocator_p(alloc),
          d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1)),
          d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2)),
          d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3)),
          d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4)),
          d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5)),
          d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6)),
          d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7)),
          d_a8(BSLS_COMPILERFEATURES_FORWARD(ARG8, a8)),
          d_a9(BSLS_COMPILERFEATURES_FORWARD(ARG9, a9)){}

    template <class ARG1,  class ARG2,  class ARG3,
              class ARG4,  class ARG5,  class ARG6,
              class ARG7,  class ARG8,  class ARG9,
              class ARG10>
    ConstructTestTypeAllocArgT(bsl::allocator_arg_t       ,
                               bslma::Allocator     *alloc,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)  a4,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG5)  a5,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG6)  a6,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG7)  a7,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG8)  a8,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG9)  a9,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG10) a10)
        : d_allocator_p(alloc),
          d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1)),
          d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2)),
          d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3)),
          d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4)),
          d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5)),
          d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6)),
          d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7)),
          d_a8(BSLS_COMPILERFEATURES_FORWARD(ARG8, a8)),
          d_a9(BSLS_COMPILERFEATURES_FORWARD(ARG9, a9)),
          d_a10(BSLS_COMPILERFEATURES_FORWARD(ARG10, a10)) {}

    template <class ARG1,  class ARG2,  class ARG3,
              class ARG4,  class ARG5,  class ARG6,
              class ARG7,  class ARG8,  class ARG9,
              class ARG10, class ARG11>
    ConstructTestTypeAllocArgT(bsl::allocator_arg_t       ,
                               bslma::Allocator     *alloc,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)  a4,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG5)  a5,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG6)  a6,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG7)  a7,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG8)  a8,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG9)  a9,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG10) a10,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG11) a11)
        : d_allocator_p(alloc),
          d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1)),
          d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2)),
          d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3)),
          d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4)),
          d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5)),
          d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6)),
          d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7)),
          d_a8(BSLS_COMPILERFEATURES_FORWARD(ARG8, a8)),
          d_a9(BSLS_COMPILERFEATURES_FORWARD(ARG9, a9)),
          d_a10(BSLS_COMPILERFEATURES_FORWARD(ARG10, a10)),
          d_a11(BSLS_COMPILERFEATURES_FORWARD(ARG11, a11)){}

    template <class ARG1,  class ARG2,  class ARG3,
              class ARG4,  class ARG5,  class ARG6,
              class ARG7,  class ARG8,  class ARG9,
              class ARG10, class ARG11, class ARG12>
    ConstructTestTypeAllocArgT(bsl::allocator_arg_t       ,
                               bslma::Allocator     *alloc,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)  a4,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG5)  a5,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG6)  a6,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG7)  a7,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG8)  a8,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG9)  a9,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG10) a10,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG11) a11,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG12) a12)
        : d_allocator_p(alloc),
          d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1)),
          d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2)),
          d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3)),
          d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4)),
          d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5)),
          d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6)),
          d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7)),
          d_a8(BSLS_COMPILERFEATURES_FORWARD(ARG8, a8)),
          d_a9(BSLS_COMPILERFEATURES_FORWARD(ARG9, a9)),
          d_a10(BSLS_COMPILERFEATURES_FORWARD(ARG10, a10)),
          d_a11(BSLS_COMPILERFEATURES_FORWARD(ARG11, a11)),
          d_a12(BSLS_COMPILERFEATURES_FORWARD(ARG12, a12)) {}

    template <class ARG1,  class ARG2,  class ARG3,
              class ARG4,  class ARG5,  class ARG6,
              class ARG7,  class ARG8,  class ARG9,
              class ARG10, class ARG11, class ARG12,
              class ARG13>
    ConstructTestTypeAllocArgT(bsl::allocator_arg_t       ,
                               bslma::Allocator     *alloc,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)  a4,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG5)  a5,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG6)  a6,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG7)  a7,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG8)  a8,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG9)  a9,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG10) a10,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG11) a11,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG12) a12,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG13) a13)
        : d_allocator_p(alloc),
          d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1)),
          d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2)),
          d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3)),
          d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4)),
          d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5)),
          d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6)),
          d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7)),
          d_a8(BSLS_COMPILERFEATURES_FORWARD(ARG8, a8)),
          d_a9(BSLS_COMPILERFEATURES_FORWARD(ARG9, a9)),
          d_a10(BSLS_COMPILERFEATURES_FORWARD(ARG10, a10)),
          d_a11(BSLS_COMPILERFEATURES_FORWARD(ARG11, a11)),
          d_a12(BSLS_COMPILERFEATURES_FORWARD(ARG12, a12)),
          d_a13(BSLS_COMPILERFEATURES_FORWARD(ARG13, a13)){}

    template <class ARG1,  class ARG2,  class ARG3,
              class ARG4,  class ARG5,  class ARG6,
              class ARG7,  class ARG8,  class ARG9,
              class ARG10, class ARG11, class ARG12,
              class ARG13, class ARG14>
    ConstructTestTypeAllocArgT(bsl::allocator_arg_t       ,
                               bslma::Allocator     *alloc,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)  a4,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG5)  a5,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG6)  a6,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG7)  a7,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG8)  a8,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG9)  a9,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG10) a10,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG11) a11,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG12) a12,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG13) a13,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG14) a14)
        : d_allocator_p(alloc),
          d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1)),
          d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2)),
          d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3)),
          d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4)),
          d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5)),
          d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6)),
          d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7)),
          d_a8(BSLS_COMPILERFEATURES_FORWARD(ARG8, a8)),
          d_a9(BSLS_COMPILERFEATURES_FORWARD(ARG9, a9)),
          d_a10(BSLS_COMPILERFEATURES_FORWARD(ARG10, a10)),
          d_a11(BSLS_COMPILERFEATURES_FORWARD(ARG11, a11)),
          d_a12(BSLS_COMPILERFEATURES_FORWARD(ARG12, a12)),
          d_a13(BSLS_COMPILERFEATURES_FORWARD(ARG13, a13)),
          d_a14(BSLS_COMPILERFEATURES_FORWARD(ARG14, a14)) {}


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
int ConstructTestTypeAllocArgT::copyConstructorInvocations       = 0;
int ConstructTestTypeAllocArgT::moveConstructorInvocations       = 0;

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
bool createdAlike(const ConstructTestTypeAllocArgT& lhs,
                  const ConstructTestTypeAllocArgT& rhs)
{
    return lhs.d_a1.d_copied_count  == rhs.d_a1.d_copied_count &&
           lhs.d_a2.d_copied_count  == rhs.d_a2.d_copied_count &&
           lhs.d_a3.d_copied_count  == rhs.d_a3.d_copied_count &&
           lhs.d_a4.d_copied_count  == rhs.d_a4.d_copied_count &&
           lhs.d_a5.d_copied_count  == rhs.d_a5.d_copied_count &&
           lhs.d_a6.d_copied_count  == rhs.d_a6.d_copied_count &&
           lhs.d_a7.d_copied_count  == rhs.d_a7.d_copied_count &&
           lhs.d_a8.d_copied_count  == rhs.d_a8.d_copied_count &&
           lhs.d_a9.d_copied_count  == rhs.d_a9.d_copied_count &&
           lhs.d_a10.d_copied_count == rhs.d_a10.d_copied_count &&
           lhs.d_a11.d_copied_count == rhs.d_a11.d_copied_count &&
           lhs.d_a12.d_copied_count == rhs.d_a12.d_copied_count &&
           lhs.d_a13.d_copied_count == rhs.d_a13.d_copied_count &&
           lhs.d_a14.d_copied_count == rhs.d_a14.d_copied_count;
}
                        // ================================
                        // class ConstructTestTypeAllocArgTIL
                        // ================================

class ConstructTestTypeAllocArgTIL {
    // This class provides a test class capable of holding up to 14 parameters
    // of types 'ConstructTestArg[1--14]'.  By default, a
    // 'ConstructTestTypeAllocArgTIL' is constructed with nil ('N1') values,
    // but instances can be constructed with actual values (e.g., for creating
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
    static int copyConstructorInvocations;
    static int moveConstructorInvocations;

    int d_ilsum;
    bslma::Allocator *d_allocator_p;
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
    ConstructTestTypeAllocArgTIL()
    : d_ilsum(0), d_allocator_p(0){}

    ConstructTestTypeAllocArgTIL(ConstructTestTypeAllocArgTIL const& other)
    : d_ilsum(0), d_allocator_p(0)
    , d_a1 (other.d_a1),  d_a2 (other.d_a2),  d_a3 (other.d_a3)
    , d_a4 (other.d_a4),  d_a5 (other.d_a5),  d_a6 (other.d_a6)
    , d_a7 (other.d_a7),  d_a8 (other.d_a8),  d_a9 (other.d_a9)
    , d_a10(other.d_a10), d_a11(other.d_a11), d_a12(other.d_a12)
    , d_a13(other.d_a13), d_a14(other.d_a14)
    {
        ++copyConstructorInvocations;
    }

    ConstructTestTypeAllocArgTIL(
    bslmf::MovableRef<ConstructTestTypeAllocArgTIL>  other)
    : d_ilsum(0),
    d_allocator_p(MovUtl::access(other).d_allocator_p),
    d_a1 (MovUtl::move(MovUtl::access(other).d_a1)),
    d_a2 (MovUtl::move(MovUtl::access(other).d_a2)),
    d_a3 (MovUtl::move(MovUtl::access(other).d_a3)),
    d_a4 (MovUtl::move(MovUtl::access(other).d_a4)),
    d_a5 (MovUtl::move(MovUtl::access(other).d_a5)),
    d_a6 (MovUtl::move(MovUtl::access(other).d_a6)),
    d_a7 (MovUtl::move(MovUtl::access(other).d_a7)),
    d_a8 (MovUtl::move(MovUtl::access(other).d_a8)),
    d_a9 (MovUtl::move(MovUtl::access(other).d_a9)),
    d_a10(MovUtl::move(MovUtl::access(other).d_a10)),
    d_a11(MovUtl::move(MovUtl::access(other).d_a11)),
    d_a12(MovUtl::move(MovUtl::access(other).d_a12)),
    d_a13(MovUtl::move(MovUtl::access(other).d_a13)),
    d_a14(MovUtl::move(MovUtl::access(other).d_a14))
    {
      ++moveConstructorInvocations;
    }
    ConstructTestTypeAllocArgTIL(bsl::allocator_arg_t       ,
            bslma::Allocator     *alloc,
            ConstructTestTypeAllocArgTIL const& other)
    : d_ilsum(0), d_allocator_p(alloc)
    , d_a1 (other.d_a1),  d_a2 (other.d_a2),  d_a3 (other.d_a3)
    , d_a4 (other.d_a4),  d_a5 (other.d_a5),  d_a6 (other.d_a6)
    , d_a7 (other.d_a7),  d_a8 (other.d_a8),  d_a9 (other.d_a9)
    , d_a10(other.d_a10), d_a11(other.d_a11), d_a12(other.d_a12)
    , d_a13(other.d_a13), d_a14(other.d_a14)
    {
    ++copyConstructorInvocations;
    }

    ConstructTestTypeAllocArgTIL(bsl::allocator_arg_t       ,
       bslma::Allocator     *alloc,
       bslmf::MovableRef<ConstructTestTypeAllocArgTIL>  other)
    : d_ilsum(0), d_allocator_p(alloc),
    d_a1 (MovUtl::move(MovUtl::access(other).d_a1)),
    d_a2 (MovUtl::move(MovUtl::access(other).d_a2)),
    d_a3 (MovUtl::move(MovUtl::access(other).d_a3)),
    d_a4 (MovUtl::move(MovUtl::access(other).d_a4)),
    d_a5 (MovUtl::move(MovUtl::access(other).d_a5)),
    d_a6 (MovUtl::move(MovUtl::access(other).d_a6)),
    d_a7 (MovUtl::move(MovUtl::access(other).d_a7)),
    d_a8 (MovUtl::move(MovUtl::access(other).d_a8)),
    d_a9 (MovUtl::move(MovUtl::access(other).d_a9)),
    d_a10(MovUtl::move(MovUtl::access(other).d_a10)),
    d_a11(MovUtl::move(MovUtl::access(other).d_a11)),
    d_a12(MovUtl::move(MovUtl::access(other).d_a12)),
    d_a13(MovUtl::move(MovUtl::access(other).d_a13)),
    d_a14(MovUtl::move(MovUtl::access(other).d_a14))
    {
    ++moveConstructorInvocations;
    }

    ConstructTestTypeAllocArgTIL(bsl::allocator_arg_t       ,
            bslma::Allocator     *alloc)
    : d_ilsum(0), d_allocator_p(alloc){}

#ifdef BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS
    ConstructTestTypeAllocArgTIL(bsl::allocator_arg_t       ,
                                bslma::Allocator     *alloc,
                                std::initializer_list<int> il)
         : d_ilsum(0), d_allocator_p(alloc)
     {
       for (int i : il) d_ilsum+=i;
     }

     template <class ARG1>
     ConstructTestTypeAllocArgTIL(bsl::allocator_arg_t       ,
                                bslma::Allocator     *alloc,
                                std::initializer_list<int> il,
                                BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1)
         : d_ilsum(0), d_allocator_p(alloc),
           d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1))
     {
       for (int i : il) d_ilsum+=i;
     }

     template <class ARG1,  class ARG2>
     ConstructTestTypeAllocArgTIL(bsl::allocator_arg_t       ,
                                bslma::Allocator     *alloc,
                                std::initializer_list<int> il,
                                BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                                BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2)
         : d_ilsum(0), d_allocator_p(alloc),
           d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1)),
           d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2))
     {
       for (int i : il) d_ilsum+=i;
     }

     template <class ARG1,  class ARG2,  class ARG3>
     ConstructTestTypeAllocArgTIL(bsl::allocator_arg_t       ,
                                bslma::Allocator     *alloc,
                                std::initializer_list<int> il,
                                BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                                BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                                BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3)
         : d_ilsum(0), d_allocator_p(alloc),
           d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1)),
           d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2)),
           d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3))
     {
       for (int i : il) d_ilsum+=i;
     }

     template <class ARG1,  class ARG2,  class ARG3,
               class ARG4>
     ConstructTestTypeAllocArgTIL(bsl::allocator_arg_t       ,
                                bslma::Allocator     *alloc,
                                std::initializer_list<int> il,
                                BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                                BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                                BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
                                BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)  a4)
         : d_ilsum(0), d_allocator_p(alloc),
           d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1)),
           d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2)),
           d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3)),
           d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4))
     {
       for (int i : il) d_ilsum+=i;
     }

     template <class ARG1,  class ARG2,  class ARG3,
               class ARG4,  class ARG5>
     ConstructTestTypeAllocArgTIL(bsl::allocator_arg_t       ,
                                bslma::Allocator     *alloc,
                                std::initializer_list<int> il,
                                BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                                BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                                BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
                                BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)  a4,
                                BSLS_COMPILERFEATURES_FORWARD_REF(ARG5)  a5)
         : d_ilsum(0), d_allocator_p(alloc),
           d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1)),
           d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2)),
           d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3)),
           d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4)),
           d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5))
     {
       for (int i : il) d_ilsum+=i;
     }

     template <class ARG1,  class ARG2,  class ARG3,
               class ARG4,  class ARG5,  class ARG6>
     ConstructTestTypeAllocArgTIL(bsl::allocator_arg_t       ,
                                bslma::Allocator     *alloc,
                                std::initializer_list<int> il,
                                BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                                BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                                BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
                                BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)  a4,
                                BSLS_COMPILERFEATURES_FORWARD_REF(ARG5)  a5,
                                BSLS_COMPILERFEATURES_FORWARD_REF(ARG6)  a6)
         : d_ilsum(0), d_allocator_p(alloc),
           d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1)),
           d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2)),
           d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3)),
           d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4)),
           d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5)),
           d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6))
     {
       for (int i : il) d_ilsum+=i;
     }

     template <class ARG1,  class ARG2,  class ARG3,
               class ARG4,  class ARG5,  class ARG6,
               class ARG7>
     ConstructTestTypeAllocArgTIL(bsl::allocator_arg_t       ,
                                bslma::Allocator     *alloc,
                                std::initializer_list<int> il,
                                BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                                BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                                BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
                                BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)  a4,
                                BSLS_COMPILERFEATURES_FORWARD_REF(ARG5)  a5,
                                BSLS_COMPILERFEATURES_FORWARD_REF(ARG6)  a6,
                                BSLS_COMPILERFEATURES_FORWARD_REF(ARG7)  a7)
         : d_ilsum(0), d_allocator_p(alloc),
           d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1)),
           d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2)),
           d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3)),
           d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4)),
           d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5)),
           d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6)),
           d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7))
     {
       for (int i : il) d_ilsum+=i;
     }

     template <class ARG1,  class ARG2,  class ARG3,
               class ARG4,  class ARG5,  class ARG6,
               class ARG7,  class ARG8>
     ConstructTestTypeAllocArgTIL(bsl::allocator_arg_t       ,
                                bslma::Allocator     *alloc,
                                std::initializer_list<int> il,
                                BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                                BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                                BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
                                BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)  a4,
                                BSLS_COMPILERFEATURES_FORWARD_REF(ARG5)  a5,
                                BSLS_COMPILERFEATURES_FORWARD_REF(ARG6)  a6,
                                BSLS_COMPILERFEATURES_FORWARD_REF(ARG7)  a7,
                                BSLS_COMPILERFEATURES_FORWARD_REF(ARG8)  a8)
         : d_ilsum(0), d_allocator_p(alloc),
           d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1)),
           d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2)),
           d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3)),
           d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4)),
           d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5)),
           d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6)),
           d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7)),
           d_a8(BSLS_COMPILERFEATURES_FORWARD(ARG8, a8))
     {
       for (int i : il) d_ilsum+=i;
     }

     template <class ARG1,  class ARG2,  class ARG3,
               class ARG4,  class ARG5,  class ARG6,
               class ARG7,  class ARG8,  class ARG9>
     ConstructTestTypeAllocArgTIL(bsl::allocator_arg_t       ,
                                bslma::Allocator     *alloc,
                                std::initializer_list<int> il,
                                BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                                BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                                BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
                                BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)  a4,
                                BSLS_COMPILERFEATURES_FORWARD_REF(ARG5)  a5,
                                BSLS_COMPILERFEATURES_FORWARD_REF(ARG6)  a6,
                                BSLS_COMPILERFEATURES_FORWARD_REF(ARG7)  a7,
                                BSLS_COMPILERFEATURES_FORWARD_REF(ARG8)  a8,
                                BSLS_COMPILERFEATURES_FORWARD_REF(ARG9)  a9)
         : d_ilsum(0), d_allocator_p(alloc),
           d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1)),
           d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2)),
           d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3)),
           d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4)),
           d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5)),
           d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6)),
           d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7)),
           d_a8(BSLS_COMPILERFEATURES_FORWARD(ARG8, a8)),
           d_a9(BSLS_COMPILERFEATURES_FORWARD(ARG9, a9))
     {
       for (int i : il) d_ilsum+=i;
     }

     template <class ARG1,  class ARG2,  class ARG3,
               class ARG4,  class ARG5,  class ARG6,
               class ARG7,  class ARG8,  class ARG9,
               class ARG10>
     ConstructTestTypeAllocArgTIL(bsl::allocator_arg_t       ,
                                bslma::Allocator     *alloc,
                                std::initializer_list<int> il,
                                BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                                BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                                BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
                                BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)  a4,
                                BSLS_COMPILERFEATURES_FORWARD_REF(ARG5)  a5,
                                BSLS_COMPILERFEATURES_FORWARD_REF(ARG6)  a6,
                                BSLS_COMPILERFEATURES_FORWARD_REF(ARG7)  a7,
                                BSLS_COMPILERFEATURES_FORWARD_REF(ARG8)  a8,
                                BSLS_COMPILERFEATURES_FORWARD_REF(ARG9)  a9,
                                BSLS_COMPILERFEATURES_FORWARD_REF(ARG10) a10)
         : d_ilsum(0), d_allocator_p(alloc),
           d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1)),
           d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2)),
           d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3)),
           d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4)),
           d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5)),
           d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6)),
           d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7)),
           d_a8(BSLS_COMPILERFEATURES_FORWARD(ARG8, a8)),
           d_a9(BSLS_COMPILERFEATURES_FORWARD(ARG9, a9)),
           d_a10(BSLS_COMPILERFEATURES_FORWARD(ARG10, a10))
     {
       for (int i : il) d_ilsum+=i;
     }

     template <class ARG1,  class ARG2,  class ARG3,
               class ARG4,  class ARG5,  class ARG6,
               class ARG7,  class ARG8,  class ARG9,
               class ARG10, class ARG11>
     ConstructTestTypeAllocArgTIL(bsl::allocator_arg_t       ,
                                bslma::Allocator     *alloc,
                                std::initializer_list<int> il,
                                BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                                BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                                BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
                                BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)  a4,
                                BSLS_COMPILERFEATURES_FORWARD_REF(ARG5)  a5,
                                BSLS_COMPILERFEATURES_FORWARD_REF(ARG6)  a6,
                                BSLS_COMPILERFEATURES_FORWARD_REF(ARG7)  a7,
                                BSLS_COMPILERFEATURES_FORWARD_REF(ARG8)  a8,
                                BSLS_COMPILERFEATURES_FORWARD_REF(ARG9)  a9,
                                BSLS_COMPILERFEATURES_FORWARD_REF(ARG10) a10,
                                BSLS_COMPILERFEATURES_FORWARD_REF(ARG11) a11)
         : d_ilsum(0), d_allocator_p(alloc),
           d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1)),
           d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2)),
           d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3)),
           d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4)),
           d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5)),
           d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6)),
           d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7)),
           d_a8(BSLS_COMPILERFEATURES_FORWARD(ARG8, a8)),
           d_a9(BSLS_COMPILERFEATURES_FORWARD(ARG9, a9)),
           d_a10(BSLS_COMPILERFEATURES_FORWARD(ARG10, a10)),
           d_a11(BSLS_COMPILERFEATURES_FORWARD(ARG11, a11))
     {
       for (int i : il) d_ilsum+=i;
     }

     template <class ARG1,  class ARG2,  class ARG3,
               class ARG4,  class ARG5,  class ARG6,
               class ARG7,  class ARG8,  class ARG9,
               class ARG10, class ARG11, class ARG12>
     ConstructTestTypeAllocArgTIL(bsl::allocator_arg_t       ,
                                bslma::Allocator     *alloc,
                                std::initializer_list<int> il,
                                BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                                BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                                BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
                                BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)  a4,
                                BSLS_COMPILERFEATURES_FORWARD_REF(ARG5)  a5,
                                BSLS_COMPILERFEATURES_FORWARD_REF(ARG6)  a6,
                                BSLS_COMPILERFEATURES_FORWARD_REF(ARG7)  a7,
                                BSLS_COMPILERFEATURES_FORWARD_REF(ARG8)  a8,
                                BSLS_COMPILERFEATURES_FORWARD_REF(ARG9)  a9,
                                BSLS_COMPILERFEATURES_FORWARD_REF(ARG10) a10,
                                BSLS_COMPILERFEATURES_FORWARD_REF(ARG11) a11,
                                BSLS_COMPILERFEATURES_FORWARD_REF(ARG12) a12)
         : d_ilsum(0), d_allocator_p(alloc),
           d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1)),
           d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2)),
           d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3)),
           d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4)),
           d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5)),
           d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6)),
           d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7)),
           d_a8(BSLS_COMPILERFEATURES_FORWARD(ARG8, a8)),
           d_a9(BSLS_COMPILERFEATURES_FORWARD(ARG9, a9)),
           d_a10(BSLS_COMPILERFEATURES_FORWARD(ARG10, a10)),
           d_a11(BSLS_COMPILERFEATURES_FORWARD(ARG11, a11)),
           d_a12(BSLS_COMPILERFEATURES_FORWARD(ARG12, a12))
     {
       for (int i : il) d_ilsum+=i;
     }

     template <class ARG1,  class ARG2,  class ARG3,
               class ARG4,  class ARG5,  class ARG6,
               class ARG7,  class ARG8,  class ARG9,
               class ARG10, class ARG11, class ARG12,
               class ARG13>
     ConstructTestTypeAllocArgTIL(bsl::allocator_arg_t       ,
                                bslma::Allocator     *alloc,
                                std::initializer_list<int> il,
                                BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                                BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                                BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
                                BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)  a4,
                                BSLS_COMPILERFEATURES_FORWARD_REF(ARG5)  a5,
                                BSLS_COMPILERFEATURES_FORWARD_REF(ARG6)  a6,
                                BSLS_COMPILERFEATURES_FORWARD_REF(ARG7)  a7,
                                BSLS_COMPILERFEATURES_FORWARD_REF(ARG8)  a8,
                                BSLS_COMPILERFEATURES_FORWARD_REF(ARG9)  a9,
                                BSLS_COMPILERFEATURES_FORWARD_REF(ARG10) a10,
                                BSLS_COMPILERFEATURES_FORWARD_REF(ARG11) a11,
                                BSLS_COMPILERFEATURES_FORWARD_REF(ARG12) a12,
                                BSLS_COMPILERFEATURES_FORWARD_REF(ARG13) a13)
         : d_ilsum(0), d_allocator_p(alloc),
           d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1)),
           d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2)),
           d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3)),
           d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4)),
           d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5)),
           d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6)),
           d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7)),
           d_a8(BSLS_COMPILERFEATURES_FORWARD(ARG8, a8)),
           d_a9(BSLS_COMPILERFEATURES_FORWARD(ARG9, a9)),
           d_a10(BSLS_COMPILERFEATURES_FORWARD(ARG10, a10)),
           d_a11(BSLS_COMPILERFEATURES_FORWARD(ARG11, a11)),
           d_a12(BSLS_COMPILERFEATURES_FORWARD(ARG12, a12)),
           d_a13(BSLS_COMPILERFEATURES_FORWARD(ARG13, a13))
     {
       for (int i : il) d_ilsum+=i;
     }

     template <class ARG1,  class ARG2,  class ARG3,
               class ARG4,  class ARG5,  class ARG6,
               class ARG7,  class ARG8,  class ARG9,
               class ARG10, class ARG11, class ARG12,
               class ARG13, class ARG14>
     ConstructTestTypeAllocArgTIL(bsl::allocator_arg_t       ,
                                bslma::Allocator     *alloc,
                                std::initializer_list<int> il,
                                BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                                BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                                BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
                                BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)  a4,
                                BSLS_COMPILERFEATURES_FORWARD_REF(ARG5)  a5,
                                BSLS_COMPILERFEATURES_FORWARD_REF(ARG6)  a6,
                                BSLS_COMPILERFEATURES_FORWARD_REF(ARG7)  a7,
                                BSLS_COMPILERFEATURES_FORWARD_REF(ARG8)  a8,
                                BSLS_COMPILERFEATURES_FORWARD_REF(ARG9)  a9,
                                BSLS_COMPILERFEATURES_FORWARD_REF(ARG10) a10,
                                BSLS_COMPILERFEATURES_FORWARD_REF(ARG11) a11,
                                BSLS_COMPILERFEATURES_FORWARD_REF(ARG12) a12,
                                BSLS_COMPILERFEATURES_FORWARD_REF(ARG13) a13,
                                BSLS_COMPILERFEATURES_FORWARD_REF(ARG14) a14)
         : d_ilsum(0), d_allocator_p(alloc),
           d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1)),
           d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2)),
           d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3)),
           d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4)),
           d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5)),
           d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6)),
           d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7)),
           d_a8(BSLS_COMPILERFEATURES_FORWARD(ARG8, a8)),
           d_a9(BSLS_COMPILERFEATURES_FORWARD(ARG9, a9)),
           d_a10(BSLS_COMPILERFEATURES_FORWARD(ARG10, a10)),
           d_a11(BSLS_COMPILERFEATURES_FORWARD(ARG11, a11)),
           d_a12(BSLS_COMPILERFEATURES_FORWARD(ARG12, a12)),
           d_a13(BSLS_COMPILERFEATURES_FORWARD(ARG13, a13)),
           d_a14(BSLS_COMPILERFEATURES_FORWARD(ARG14, a14))
     {
       for (int i : il) d_ilsum+=i;
     }
#endif // BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS
};

// TRAITS
namespace BloombergLP {
namespace bslma {

template <>
struct UsesBslmaAllocator<ConstructTestTypeAllocArgTIL> : bsl::true_type { };

}  // close namespace bslma

namespace bslmf {

template <>
struct UsesAllocatorArgT<ConstructTestTypeAllocArgTIL> : bsl::true_type { };

}  // close namespace bslmf
}  // close enterprise namespace
int ConstructTestTypeAllocArgTIL::copyConstructorInvocations       = 0;
int ConstructTestTypeAllocArgTIL::moveConstructorInvocations       = 0;

// FREE OPERATORS
bool operator==(const ConstructTestTypeAllocArgTIL& lhs,
             const ConstructTestTypeAllocArgTIL& rhs)
{
 return lhs.d_ilsum == rhs.d_ilsum &&
        lhs.d_a1.d_value  == rhs.d_a1.d_value &&
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
bool createdAlike(const ConstructTestTypeAllocArgTIL& lhs,
               const ConstructTestTypeAllocArgTIL& rhs)
{
 return lhs.d_a1.d_copied_count  == rhs.d_a1.d_copied_count &&
        lhs.d_a2.d_copied_count  == rhs.d_a2.d_copied_count &&
        lhs.d_a3.d_copied_count  == rhs.d_a3.d_copied_count &&
        lhs.d_a4.d_copied_count  == rhs.d_a4.d_copied_count &&
        lhs.d_a5.d_copied_count  == rhs.d_a5.d_copied_count &&
        lhs.d_a6.d_copied_count  == rhs.d_a6.d_copied_count &&
        lhs.d_a7.d_copied_count  == rhs.d_a7.d_copied_count &&
        lhs.d_a8.d_copied_count  == rhs.d_a8.d_copied_count &&
        lhs.d_a9.d_copied_count  == rhs.d_a9.d_copied_count &&
        lhs.d_a10.d_copied_count == rhs.d_a10.d_copied_count &&
        lhs.d_a11.d_copied_count == rhs.d_a11.d_copied_count &&
        lhs.d_a12.d_copied_count == rhs.d_a12.d_copied_count &&
        lhs.d_a13.d_copied_count == rhs.d_a13.d_copied_count &&
        lhs.d_a14.d_copied_count == rhs.d_a14.d_copied_count;
}

const MyClass1     V1(1);
const MyClass2     V2(2);
const MyClass3     V4(4);
const MyClass4     V5(5);
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

struct Swappable {

    // PUBLIC CLASS DATA
    static int s_swapCalled;

    // PUBLIC DATA
    int d_value;

    // CLASS METHODS
    static bool swapCalled()
    {
        return 0 != s_swapCalled;
    }

    static void swapReset()
    {
        s_swapCalled = 0;
    }

    // CREATORS
    explicit Swappable(int v)
    : d_value(v)
    {
    }
};

// FREE OPERATORS
bool operator==(const Swappable& lhs, const Swappable& rhs)
{
    return lhs.d_value == rhs.d_value;
}

// PUBLIC CLASS DATA
int Swappable::s_swapCalled = 0;

void swap(Swappable& a, Swappable& b)
{
    ++Swappable::s_swapCalled;

    BloombergLP::bslalg::SwapUtil::swap(&a.d_value, &b.d_value);
}

struct SwappableAA {

    // PUBLIC CLASS DATA
    static int s_swapCalled;

    // PUBLIC DATA
    MyClassDef d_def;

    // CLASS METHODS
    static bool swapCalled()
    {
        return 0 != s_swapCalled;
    }

    static void swapReset()
    {
        s_swapCalled = 0;
    }

    // CREATORS
    explicit SwappableAA(int v, bslma::Allocator *a = 0)
    {
        d_def.d_value = v;
        d_def.d_allocator_p = a;
    }

    SwappableAA(const SwappableAA& rhs, bslma::Allocator *a = 0) {
       d_def.d_value = rhs.d_def.d_value;
       d_def.d_allocator_p = a;
    }

    SwappableAA(bslmf::MovableRef<SwappableAA> other, bslma::Allocator *a = 0)
    {

        SwappableAA& otherRef = MovUtl::access(other);
        d_def.d_value = otherRef.d_def.d_value;
        otherRef.d_def.d_value = MOVED_FROM_VAL;
        if (a) {
            d_def.d_allocator_p = a;
        }
        else {
            d_def.d_allocator_p = otherRef.d_def.d_allocator_p;
        }
    }


};

// FREE OPERATORS
bool operator==(const SwappableAA& lhs, const SwappableAA& rhs)
{
    return lhs.d_def.d_value == rhs.d_def.d_value;
}

// PUBLIC CLASS DATA
int SwappableAA::s_swapCalled = 0;

void swap(SwappableAA& a, SwappableAA& b)
{
    ++SwappableAA::s_swapCalled;

    BloombergLP::bslalg::SwapUtil::swap(&a.d_def.d_value, &b.d_def.d_value);
}
// TRAITS
namespace BloombergLP {
namespace bslma {

template <>
struct UsesBslmaAllocator<SwappableAA> : bsl::true_type { };

}  // close namespace bslma
} // close namespace BloombergLP

// helper functions to determine the type of a reference
bool isConstRef(const MyClass1&)  { return true; }
bool isConstRef(MyClass1&)        { return false;}
#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
bool isConstRef(MyClass1&&)       { return false;}
bool isConstRef(const MyClass1&&) { return true; }
#endif //BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES

#ifdef BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS
bool isRvalueRef(const MyClass1&) { return false;}
bool isRvalueRef(MyClass1&)       { return false;}
bool isRvalueRef(MyClass1&&)        { return true; }
bool isRvalueRef(const MyClass1&&){ return true; }
#endif //BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS

bool isConstRef(const MyClass2&)  { return true; }
bool isConstRef(MyClass2&)        { return false;}
#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
bool isConstRef(MyClass2&&)       { return false;}
bool isConstRef(const MyClass2&&) { return true; }
#endif //BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES

#ifdef BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS
bool isRvalueRef(const MyClass2&) { return false;}
bool isRvalueRef(MyClass2&)       { return false;}
bool isRvalueRef(MyClass2&&)        { return true; }
bool isRvalueRef(const MyClass2&&){ return true; }
#endif // defined(BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS)

// helper functions to determine the constness of a pointer
template<typename T>
bool isConstPtr( T*) { return false;}
template<typename T>
bool isConstPtr(const T*) { return true;}
// ======================
// macros TEST_EMPLACE*
// ======================

#define TEST_EMPLACE(expArgs)                                                 \
{                                                                             \
  ConstructTestTypeNoAlloc EXP expArgs ;                                      \
  bsl::optional<ConstructTestTypeNoAlloc> obj;                                \
  int CCI = ConstructTestTypeNoAlloc::copyConstructorInvocations;             \
  int MCI = ConstructTestTypeNoAlloc::moveConstructorInvocations;             \
  obj . emplace expArgs;                                                      \
  ASSERT(EXP == obj .value());                                                \
  ASSERT(CCI == ConstructTestTypeNoAlloc::copyConstructorInvocations);        \
  ASSERT(MCI == ConstructTestTypeNoAlloc::moveConstructorInvocations);        \
  ASSERT(createdAlike(EXP,obj.value()) == true);                              \
}                                                                             \
{                                                                             \
  ConstructTestTypeAlloc EXP expArgs ;                                        \
  bslma::TestAllocator oa("other", veryVeryVeryVerbose);                      \
  bsl::optional<ConstructTestTypeAlloc> obj(bsl::allocator_arg, &oa);         \
  int CCI = ConstructTestTypeAlloc::copyConstructorInvocations;               \
  int MCI = ConstructTestTypeAlloc::moveConstructorInvocations;               \
  obj . emplace expArgs;                                                      \
  ASSERT(EXP == obj .value());                                                \
  ASSERT(&oa == obj .value().d_allocator_p);                                  \
  ASSERT(CCI == ConstructTestTypeAlloc::copyConstructorInvocations);          \
  ASSERT(MCI == ConstructTestTypeAlloc::moveConstructorInvocations);          \
  ASSERT(createdAlike(EXP,obj.value()) == true);                              \
}

#define TEST_EMPLACE_ARGT(expArgs, emplaceArgs)                               \
{                                                                             \
  bslma::TestAllocator oa("other", veryVeryVeryVerbose);                      \
  ConstructTestTypeAllocArgT EXP expArgs ;                                    \
  bsl::optional<ConstructTestTypeAllocArgT> obj(bsl::allocator_arg, &oa);     \
  int CCI = ConstructTestTypeAllocArgT::copyConstructorInvocations;           \
  int MCI = ConstructTestTypeAllocArgT::moveConstructorInvocations;           \
  obj . emplace emplaceArgs;                                                  \
  ASSERT(EXP == obj .value());                                                \
  ASSERT(&oa == obj .value().d_allocator_p);                                  \
  ASSERT(CCI == ConstructTestTypeAllocArgT::copyConstructorInvocations);      \
  ASSERT(MCI == ConstructTestTypeAllocArgT::moveConstructorInvocations);      \
  ASSERT(createdAlike(EXP,obj.value()) == true);                              \
}
#define TEST_EMPLACE_ARGTIL(expArgs, emplaceArgs)                             \
{                                                                             \
  bslma::TestAllocator oa("other", veryVeryVeryVerbose);                      \
  ConstructTestTypeAllocArgTIL EXP expArgs ;                                  \
  bsl::optional<ConstructTestTypeAllocArgTIL> obj(bsl::allocator_arg, &oa);   \
  int CCI = ConstructTestTypeAllocArgTIL::copyConstructorInvocations;         \
  int MCI = ConstructTestTypeAllocArgTIL::moveConstructorInvocations;         \
  obj . emplace emplaceArgs;                                                  \
  ASSERT(EXP == obj .value());                                                \
  ASSERT(&oa == obj .value().d_allocator_p);                                  \
  ASSERT(CCI == ConstructTestTypeAllocArgTIL::copyConstructorInvocations);    \
  ASSERT(MCI == ConstructTestTypeAllocArgTIL::moveConstructorInvocations);    \
  ASSERT(createdAlike(EXP,obj.value()) == true);                              \
}

#define TEST_EQUAL_EMPTY(obj, type)                                           \
{                                                                             \
  type sourceObj;                                                             \
  ASSERT(!sourceObj.has_value());                                             \
  obj = sourceObj;                                                            \
  ASSERT(!obj.has_value());                                                   \
  ASSERT(!sourceObj.has_value());                                             \
}
#define TEST_EQUAL_EMPTY_MOVE(obj, type)                                      \
{                                                                             \
  type sourceObj;                                                             \
  ASSERT(!sourceObj.has_value());                                             \
  obj = MovUtl::move(sourceObj);                                              \
  ASSERT(!obj.has_value());                                                   \
  ASSERT(!sourceObj.has_value());                                             \
}
#define TEST_EQUAL_ENGAGED(obj, otype, type, val)                             \
{                                                                             \
  otype sourceObj(type(val));                                                 \
 ASSERT(sourceObj.has_value());                                               \
  obj = sourceObj;                                                            \
  ASSERT(obj.has_value());                                                    \
  ASSERT(val == obj.value().value());                                         \
  ASSERT(sourceObj.has_value());                                              \
  ASSERT(val == sourceObj.value().value());                                   \
}
#define TEST_EQUAL_ENGAGED_MOVE(obj, otype, type, val, expVal)                \
{                                                                             \
  otype sourceObj(type(val));                                                 \
  ASSERT(sourceObj.has_value());                                              \
  obj = MovUtl::move(sourceObj);                                              \
  ASSERT(obj.has_value());                                                    \
  ASSERT(val == obj.value().value());                                         \
  ASSERT(sourceObj.has_value());                                              \
  ASSERT(expVal == sourceObj.value().value());                                \
}
#define TEST_EQUAL_EMPTY_A(obj, type)                                         \
{                                                                             \
  type sourceObj(bsl::allocator_arg, &ta);                                    \
  ASSERT(!sourceObj.has_value());                                             \
  obj = sourceObj;                                                            \
  ASSERT(!obj.has_value());                                                   \
  ASSERT(&oa == obj.get_allocator());                             \
  ASSERT(!sourceObj.has_value());                                             \
}
#define TEST_EQUAL_EMPTY_MOVE_A(obj, type)                                    \
{                                                                             \
  type sourceObj(bsl::allocator_arg, &ta);                                    \
  ASSERT(!sourceObj.has_value());                                             \
  obj = MovUtl::move(sourceObj);                                              \
  ASSERT(!obj.has_value());                                                   \
  ASSERT(&oa == obj.get_allocator());                             \
  ASSERT(!sourceObj.has_value());                                             \
}
#define TEST_EQUAL_ENGAGED_A(obj, otype, type, val)                           \
{                                                                             \
  otype sourceObj(bsl::allocator_arg, &ta, type(val));                        \
  ASSERT(sourceObj.has_value());                                              \
  obj = sourceObj;                                                            \
  ASSERT(obj.has_value());                                                    \
  ASSERT(val == obj.value().value());                                         \
  ASSERT(&oa == obj.get_allocator());                             \
  ASSERT(sourceObj.has_value());                                              \
  ASSERT(val == sourceObj.value().value());                                   \
}
#define TEST_EQUAL_ENGAGED_MOVE_A(obj, otype, type, val, expVal)              \
{                                                                             \
  otype sourceObj(bsl::allocator_arg, &ta, type(val));                        \
  ASSERT(sourceObj.has_value());                                              \
  obj = MovUtl::move(sourceObj);                                              \
  ASSERT(obj.has_value());                                                    \
  ASSERT(val == obj.value().value());                                         \
  ASSERT(&oa == obj.get_allocator());                             \
  ASSERT(sourceObj.has_value());                                              \
  ASSERT(expVal == sourceObj.value().value());                                \
}

#define TEST_COPY(valtype, optype, init, expArgs)                             \
{                                                                             \
  valtype EXP expArgs ;                                                       \
  int CCI = valtype::copyConstructorInvocations;                              \
  int MCI = valtype::moveConstructorInvocations;                              \
  optype obj init ;                                                           \
  ASSERT(EXP == obj.value());                                                 \
  ASSERT(CCI == ConstructTestTypeAllocArgTIL::copyConstructorInvocations);    \
  ASSERT(MCI == ConstructTestTypeAllocArgTIL::moveConstructorInvocations);    \
  ASSERT(createdAlike(EXP,obj.value()) == true);                              \
}
#define TEST_COPYA(valtype, optype, init, expArgs, alloc)                     \
{                                                                             \
  /* Expects allocator at end of argument list */                             \
  valtype EXP expArgs ;                                                       \
  int CCI = valtype::copyConstructorInvocations;                              \
  int MCI = valtype::moveConstructorInvocations;                              \
  optype obj init ;                                                           \
  ASSERT(EXP == obj.value());                                                 \
  ASSERT(CCI == ConstructTestTypeAllocArgTIL::copyConstructorInvocations);    \
  ASSERT(MCI == ConstructTestTypeAllocArgTIL::moveConstructorInvocations);    \
  ASSERT(alloc == obj.value().d_allocator_p);                                 \
}

#define TEST_MAKEOP(expArgs, alloc)                                           \
  {                                                                           \
    ConstructTestTypeNoAlloc EXP expArgs ;                                    \
    int CCI = ConstructTestTypeNoAlloc::copyConstructorInvocations;           \
    int MCI = ConstructTestTypeNoAlloc::moveConstructorInvocations;           \
    bsl::optional<ConstructTestTypeNoAlloc>  X =                              \
                       bsl::make_optional<ConstructTestTypeNoAlloc> expArgs;  \
    ASSERT(EXP == X.value());                                                 \
    ASSERT(CCI == ConstructTestTypeNoAlloc::copyConstructorInvocations);      \
    ASSERT(MCI == ConstructTestTypeNoAlloc::moveConstructorInvocations);      \
    ASSERT(createdAlike(EXP,X.value()) == true);                              \
  }                                                                           \
  {                                                                           \
    ConstructTestTypeAlloc EXP expArgs ;                                      \
    int CCI = ConstructTestTypeAlloc::copyConstructorInvocations;             \
    int MCI = ConstructTestTypeAlloc::moveConstructorInvocations;             \
    bsl::optional<ConstructTestTypeAlloc> X =                                 \
                      bsl::make_optional<ConstructTestTypeAlloc> expArgs ;    \
    ASSERT(EXP == X.value());                                                 \
    ASSERT(alloc == X.value().d_allocator_p);                                 \
    ASSERT(CCI == ConstructTestTypeAlloc::copyConstructorInvocations);        \
    ASSERT(MCI == ConstructTestTypeAlloc::moveConstructorInvocations);        \
    ASSERT(createdAlike(EXP,X.value()) == true);                              \
  }

#define TEST_ALLOCOP( op, expArgs, alloc)                                     \
  {                                                                           \
    ConstructTestTypeAlloc EXP expArgs ;                                      \
    int CCI = ConstructTestTypeAlloc::copyConstructorInvocations;             \
    int MCI = ConstructTestTypeAlloc::moveConstructorInvocations;             \
    bsl::optional<ConstructTestTypeAlloc> X =                                 \
                      bsl::alloc_optional<ConstructTestTypeAlloc> op;         \
    ASSERT(EXP == X.value());                                                 \
    ASSERT(alloc == X.value().d_allocator_p);                                 \
    ASSERT(CCI == ConstructTestTypeAlloc::copyConstructorInvocations);        \
    ASSERT(MCI == ConstructTestTypeAlloc::moveConstructorInvocations);        \
    ASSERT(createdAlike(EXP,X.value()) == true);                              \
  }

#define TEST_HASH_EMPTY(ValueType)                                            \
  {                                                                           \
      bsl::optional<ValueType> X;                                             \
      ASSERTV(!X.has_value());                                                \
      const size_t hashValue_1 = bslh::Hash<>()(X);                           \
      const size_t hashValue_2 = bslh::Hash<>()(false);                       \
      ASSERTV(hashValue_1, hashValue_2, hashValue_1 == hashValue_2);          \
  }
#define TEST_HASH_ENGAGED(ValueType, init)                                    \
  {                                                                           \
      bsl::optional<ValueType> X init;                                        \
      ValueType Y init;                                                       \
      ASSERTV(X.has_value());                                                 \
      ASSERTV(X.value() == Y);                                                \
      const size_t hashValue_1 = bslh::Hash<>()(X);                           \
      bslh::DefaultHashAlgorithm hasher;                                      \
      hashAppend(hasher, true);                                               \
      hashAppend(hasher, Y);                                                  \
      const size_t hashValue_2 = static_cast<size_t>(hasher.computeHash());   \
      ASSERTV(hashValue_1, hashValue_2, hashValue_1 == hashValue_2);          \
  }

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

void bslstl_optional_test1()
{

    // --------------------------------------------------------------------
    // BREATHING TEST
    //   This case exercises (but does not fully test) basic functionality.
    //
    // Concerns:
    //: 1 The class is sufficiently functional to enable comprehensive
    //:   testing in subsequent test cases.
    //
    // Plan:
    //: 1 Create a disengaged 'optional' of allocator aware type
    //: 2 Check that 'get_allocator' method returns the alloctor used
    //:   to construct the 'optional' object.
    //: 3 Test that 'has_value' on a disengaged 'optional' returns 'false'
    //: 4 Test that dissengaged 'optional' evaluates to 'false'
    //: 5 Emplace a value into the 'optional' object and check that
    //:   'value()' method returns the emplaced value.
    //: 6 Verify reset() method disengages an engaged 'optional.
    //: 7 Create an engaged 'optional' of non allocator aware type and verify
    //:   'value()' returns the expected value
    //: 8 Test that an engaged 'optional' evaluates to 'true'
    //
    // Testing:
    //   BREATHING TEST
    // --------------------------------------------------------------------

    if (verbose) printf("\nBREATHING TEST"
                        "\n==============\n");
    bslma::TestAllocator da("default", veryVeryVeryVerbose);
    bslma::TestAllocator ta("test", veryVeryVeryVerbose);
    bslma::DefaultAllocatorGuard dag(&da);

    bsl::optional<MyClass2> X(bsl::allocator_arg, &ta);
    ASSERT(X.get_allocator() == &ta);
    ASSERT(!X.has_value());
    ASSERT_IF_BOOL_CONVERSION(!X);

    X.emplace(V2);
    ASSERT(X.value().d_def.d_allocator_p == &ta);
    ASSERT(X.has_value());
    ASSERT(X.value() == V2);

    X.reset();
    ASSERT(!X.has_value());

    bsl::optional<int> Y = 5;
    ASSERT(Y.has_value());
    ASSERT(Y.value() == 5);
    ASSERT_IF_BOOL_CONVERSION(Y);

}
void bslstl_optional_test2()
{
    // --------------------------------------------------------------------
    // PRIMARY MANIPULATORS AND ACCESSORS
    //   This test will verify that the construction of a disengaged 'optional'
    //   is working as expected.  Also, we test that the basic accessors are
    //   working as expected.
    //
    // Concerns:
    //: 1 That the default constructor creates a disengaged object, as
    //:   determined by 'has_value' returning 'false'.
    //: 2 That the constructor taking 'nullopt_t' object creates a disengaged
    //:   object determined by 'has_value' returning 'false'.
    //: 3 If the 'value_type' of the 'optional' is allocator aware (AA), then
    //:   the 'get_allocator' method returns the default allocator when using
    //:   these constructors.
    //: 4 These constructors do not allocate any memory, whether or not the
    //:   'value_type' is AA.
    //: 5 That 'emplace' method taking a single argument of 'value_type' type
    //:   makes the 'optional' object engaged, as determined by 'has_value'
    //:   returning 'true', with a value of the argument, as determined by the
    //:   reference returned by 'value' method.
    //: 6 That for AA type, 'emplace' will create an object of 'value_type'
    //:   using the 'optional' object's allocator (which is always the default
    //:   allocator for now).
    //: 7 That an engaged 'optional' object converted to 'bool' evaluates to
    //:   'true', and a disengaged 'optional' object evaluates to 'false'.
    //: 8 That 'reset' method disengages an 'optional' object, as determined
    //:   by 'has_value' returning 'false'.
    //: 9 If an engaged 'optional' object is destroyed, the destructor of the
    //:   'value_type' is invoked.
    //
    // Plan:
    //: 1 Construct an 'optional' object of a non allocator aware type using
    //:   default construction and, for concern 1, verify that the 'optional'
    //:   object is disengaged.
    //: 2 Construct an 'optional' object of an allocator aware type using
    //:   default construction and, for concern 1, verify that the 'optional'
    //:   object is disengaged.
    //: 3 Construct an 'optional' object of a non allocator aware type using
    //:   the constructor that takes 'nullopt_t' argument and, for concern 2,
    //:   verify that the 'optional' object is disengaged.
    //: 4 Construct an 'optional' object of an allocator aware type using
    //:   using the constructor that takes 'nullopt_t' argument, and for
    //:   concern 2, verify that the 'optional' object is disengaged.
    //: 5 In step 2 and 4, for concern 3 verify that the 'get_allocator' method
    //:   returns the default allocator.
    //: 6 Call 'emplace' on a disengaged 'optional' of non allocator aware type
    //:   using an object of 'value_type' type. For concern 5, verify the
    //:   'get_value' returns 'true and that the 'value' method returns the
    //:   value of the source object.
    //: 7 Repeat step 6 for an allocator aware 'value_type'. Additionally, for
    //:   concern 6, check that the allocator of the object referenced by
    //:   the result of 'value' method is the default allocator.
    //: 8 For concern 7, in steps 1 and 2, verify the disengaged 'optional'
    //:   objects converted to 'bool' evaluate to false. Additionaly, in steps
    //:   6 and 7, verify the engaged 'optional' objects converted to 'bool'
    //:   evaluate to true.
    //: 9 Call reset on an engaged 'optional' object of non allocator aware
    //:   type. For concern 8, verify that 'get_value' returns 'false'. Repeat
    //:   the step for an 'optional' of allocator aware type.
    //:10 In a separate scope, create an engaged 'optional' of non allocator
    //:   aware type. For concern 9, when the scope ends, verify the destructor
    //:   of the 'value_type' has been called. Repeat the step for an
    //:   allocator aware type.
    //
    // Testing:
    //   typedef TYPE value_type;
    //   Optional();
    //   Optional(nullopt_t);
    //   template<class... ARGS>
    //   void emplace(BSLS_COMPILERFEATURES_FORWARD_REF(ARGS)...);
    //   void reset()
    //   bool has_value() const;
    //   TYPE& value()
    //   allocator_type get_allocator() const;
    //   operator bool()  const
    // --------------------------------------------------------------------

    if (verbose) printf(
                       "\nPRIMARY MANIPULATORS AND ACCESSORS"
                       "\n=================================="
                       "\n");

    if (veryVerbose) printf("\tUsing non allocator aware 'value_type'.\n");
    {
        typedef MyClass1                 ValueType;
        typedef bsl::optional<ValueType> Obj;

        {
            Obj mX;
            const Obj& X = mX;
            ASSERT(!X.has_value());
            ASSERT_IF_BOOL_CONVERSION(!X);
        }
        {
            Obj mX = Obj(nullopt);
            const Obj& X = mX;
            ASSERT(!X.has_value());
            ASSERT_IF_BOOL_CONVERSION(!X);

            mX.emplace(MyClass1(5));
            ASSERT(mX.has_value());
            ASSERT_IF_BOOL_CONVERSION(mX);
            ASSERT(mX.value() == MyClass1(5));

            mX.reset();
            ASSERT(!X.has_value());
            ASSERT_IF_BOOL_CONVERSION(!X);
        }
        int dI;
        {
            Obj mX = MyClass1(5);
            ASSERT(mX.has_value());
            ASSERT_IF_BOOL_CONVERSION(mX);
            dI = ValueType::destructorInvocations;
        }
        ASSERT(dI == ValueType::destructorInvocations -1);
    }

    if (veryVerbose) printf("\tUsing allocator aware type.\n");
    {
        bslma::TestAllocator da("default", veryVeryVeryVerbose);
        bslma::DefaultAllocatorGuard dag(&da);

        typedef MyClass2                        ValueType;
        typedef bsl::optional<ValueType>        Obj;

        {
            bslma::TestAllocatorMonitor dam(&da);

            Obj mX;
            const Obj& X = mX;
            ASSERT(!X.has_value());
            ASSERT_IF_BOOL_CONVERSION(!X);
            ASSERT(X.get_allocator() == &da);
            ASSERT(dam.isTotalSame());

        }
        {
            bslma::TestAllocatorMonitor dam(&da);

            Obj mX = Obj(nullopt);
            const Obj& X = mX;
            ASSERT(!X.has_value());
            ASSERT_IF_BOOL_CONVERSION(!X);
            ASSERT(X.get_allocator() == &da);
            ASSERT(dam.isTotalSame());

            mX.emplace(MyClass1(5));
            ASSERT(mX.has_value());
            ASSERT_IF_BOOL_CONVERSION(mX);
            ASSERT(mX.value() == MyClass1(5));
            ASSERT(X.value().d_def.d_allocator_p == &da);

            mX.reset();
            ASSERT(!X.has_value());
            ASSERT_IF_BOOL_CONVERSION(!X);
        }
        int dI;
        {
            Obj mX = MyClass1(5);
            ASSERT(mX.has_value());
            ASSERT_IF_BOOL_CONVERSION(mX);
            dI = ValueType::destructorInvocations;
        }
        ASSERT(dI == ValueType::destructorInvocations -1);
    }
}
void bslstl_optional_test3()
{
    // --------------------------------------------------------------------
    // PRIMARY ALLOCATOR EXTENDED DISENGAGED CONSTRUCTORS
    //   This test will verify that the construction of a disengaged 'optional'
    //   is working as expected.  Also, we test that the basic accessors are
    //   working as expected.
    //
    // Concerns:
    //: 1 That the default constructor creates a disengaged object, as
    //:   determined by 'has_value' returning 'false'.
    //: 2 That the constructor taking 'nullopt_t' object creates a disengaged
    //:   object determined by 'has_value' returning 'false'.
    //: 3 If the 'value_type' of the 'optional' is allocator aware (AA), then
    //:   the 'get_allocator' method returns the default allocator when using
    //:   these constructors.
    //: 4 These constructors do not allocate any memory, whether or not the
    //:   'value_type' is AA.
    //: 5 That 'emplace' method taking a single argument of 'value_type' type
    //:   makes the 'optional' object engaged, as determined by 'has_value'
    //:   returning 'true', with a value of the argument, as determined by the
    //:   reference returned by 'value' method.
    //: 6 That for AA type, 'emplace' will create an object of 'value_type'
    //:   using the 'optional' object's allocator (which is always the default
    //:   allocator for now).
    //: 7 That an engaged 'optional' object converted to 'bool' evaluates to
    //:   'true', and a disengaged 'optional' object evaluates to 'false'.
    //: 8 That 'reset' method disengages an 'optional' object, as determined
    //:   by 'has_value' returning 'false'.
    //: 9 If an engaged 'optional' object is destroyed, the destructor of the
    //:   'value_type' is invoked.
    //
    // Plan:
    //: 1 Construct an 'optional' object of a non allocator aware type using
    //:   default construction and, for concern 1, verify that the 'optional'
    //:   object is disengaged.
    //: 2 Construct an 'optional' object of an allocator aware type using
    //:   default construction and, for concern 1, verify that the 'optional'
    //:   object is disengaged.
    //: 3 Construct an 'optional' object of a non allocator aware type using
    //:   the constructor that takes 'nullopt_t' argument and, for concern 2,
    //:   verify that the 'optional' object is disengaged.
    //: 4 Construct an 'optional' object of an allocator aware type using
    //:   using the constructor that takes 'nullopt_t' argument, and for
    //:   concern 2, verify that the 'optional' object is disengaged.
    //: 5 In step 2 and 4, for concern 3 verify that the 'get_allocator' method
    //:   returns the default allocator.
    //: 6 Call 'emplace' on a disengaged 'optional' of non allocator aware type
    //:   using an object of 'value_type' type. For concern 5, verify the
    //:   'get_value' returns 'true and that the 'value' method returns the
    //:   value of the source object.
    //: 7 Repeat step 6 for an allocator aware 'value_type'. Additionally, for
    //:   concern 6, check that the allocator of the object referenced by
    //:   the result of 'value' method is the default allocator.
    //: 8 For concern 7, in steps 1 and 2, verify the disengaged 'optional'
    //:   objects converted to 'bool' evaluate to false. Additionaly, in steps
    //:   6 and 7, verify the engaged 'optional' objects converted to 'bool'
    //:   evaluate to true.
    //: 9 Call reset on an engaged 'optional' object of non allocator aware
    //:   type. For concern 8, verify that 'get_value' returns 'false'. Repeat
    //:   the step for an 'optional' of allocator aware type.
    //:10 In a separate scope, create an engaged 'optional' of non allocator
    //:   aware type. For concern 9, when the scope ends, verify the destructor
    //:   of the 'value_type' has been called. Repeat the step for an
    //:   allocator aware type.
    //
    // Testing:
    //   typedef TYPE value_type;
    //   Optional(bsl::allocator_arg_t, allocator_type);
    //   Optional(bsl::allocator_arg_t, allocator_type, nullopt_t);
    //   allocator_type get_allocator() const;
    // --------------------------------------------------------------------

    if (verbose) printf(
                       "\nPRIMARY ALLOCATOR EXTENDED DISENGAGED CONSTRUCTORS"
                       "\n=================================================="
                       "\n");

    if (veryVerbose) printf("\tUsing non allocator aware 'value_type'.\n");
    {
        typedef MyClass1                 ValueType;
        typedef bsl::optional<ValueType> Obj;

        {
            Obj mX;
            const Obj& X = mX;
            ASSERT(!X.has_value());
        }
        {
            Obj mX = Obj(nullopt);
            const Obj& X = mX;
            ASSERT(!X.has_value());

            mX.emplace(MyClass1(5));
            ASSERT(mX.has_value());
            ASSERT(mX.value() == MyClass1(5));

            mX.reset();
            ASSERT(!X.has_value());
        }
        int dI;
        {
            Obj mX = MyClass1(5);
            ASSERT(mX.has_value());
            dI = ValueType::destructorInvocations;
        }
        ASSERT(dI == ValueType::destructorInvocations -1);
    }

    if (veryVerbose) printf("\tUsing allocator aware type.\n");
    {
        bslma::TestAllocator da("default", veryVeryVeryVerbose);
        bslma::DefaultAllocatorGuard dag(&da);

        typedef MyClass2                        ValueType;
        typedef bsl::optional<ValueType>        Obj;

        {
            bslma::TestAllocatorMonitor dam(&da);

            Obj mX;
            const Obj& X = mX;
            ASSERT(!X.has_value());
            ASSERT(X.get_allocator() == &da);
            ASSERT(dam.isTotalSame());

        }
        {
            bslma::TestAllocatorMonitor dam(&da);

            Obj mX = Obj(nullopt);
            const Obj& X = mX;
            ASSERT(!X.has_value());
            ASSERT(X.get_allocator() == &da);
            ASSERT(dam.isTotalSame());

            mX.emplace(MyClass1(5));
            ASSERT(mX.has_value());
            ASSERT(mX.value() == MyClass1(5));
            ASSERT(X.value().d_def.d_allocator_p == &da);

            mX.reset();
            ASSERT(!X.has_value());
        }
        int dI;
        {
            Obj mX = MyClass1(5);
            ASSERT(mX.has_value());
            dI = ValueType::destructorInvocations;
        }
        ASSERT(dI == ValueType::destructorInvocations -1);
    }
}
void bslstl_optional_test4()
{
    // --------------------------------------------------------------------
    // TESTING CONVERSION TO BOOL
    //   This test will verify that the conversion to 'bool' works as expected.
    //   The test relies on reset and emplace member functions, as well as
    //   construction from a 'value_type'.
    //
    // Concerns:
    //: 1 A disengaged 'optional' when converted to 'bool' evaluates to 'false'
    //:   and returns 'false' from its 'has_value' method.
    //: 2 An engaged 'optional' when converted to 'bool' evaluates to 'true'
    //:   and returns 'false' from its 'has_value' method.
    //: 3 Both conversion to 'bool' and 'has_value' method can be performed/
    //:   invoked on a 'const' qualified 'optional' object.
    //: 4 All of the above concerns apply whether or not the 'optional'
    //:   object's 'value_type' is allocator aware.
    //
    // Plan:
    //: 1 Create a disengaged 'optional' of a non allocator aware type and,
    //:   for concern 1, verify that it evaluates to 'false' when converted to
    //:   'bool' and that 'has_value' method returns 'false'.
    //: 2 Emplace a value in the 'optional' object and, for concern 2, verify
    //:   that it evaluates to 'true' when converted to 'bool' and that the
    //:   'has_value' method returns 'true'.
    //: 3 Call 'reset' method and, for concern 1, verify that the 'optional'
    //:   object evaluates to 'false' when converted to 'bool' and that
    //:   'has_value' method returns 'false'.
    //: 4 Create an engaged 'optional' of a non allocator aware type and,
    //:   for concern 2, verify that it evaluates to 'true' when converted to
    //:   'bool' and that 'has_value' method returns 'true'.
    //: 5 In step 1, for concern 3, use a 'const' qualified reference to
    //:   test conversion to 'bool' and to invoke the 'has_value' method.
    //: 6 For concern 4, repeat steps 1-5 with an allocator aware 'value_type.
    //
    // Testing:
    //   operator bool() const;
    //   bool has_value() const;
    //
    //   void reset();
    //   optional(const T &);
    //   emplace(const T &);
    //
    // --------------------------------------------------------------------

    if (verbose) printf(
                       "\nTESTING CONVERSION TO BOOL"
                       "\n==========================\n");

    if (veryVerbose) printf("\tUsing 'bsl::optional<int>'.\n");
    {
        typedef int                            ValueType;
        typedef bsl::optional<ValueType> Obj;
        {
            Obj mX;
            const Obj& X = mX;
            ASSERT_IF_BOOL_CONVERSION(!X);
            ASSERT(false == X.has_value());

            mX.emplace(1);
            ASSERT_IF_BOOL_CONVERSION(mX);
            ASSERT(true == mX.has_value());

            mX.reset();
            ASSERT_IF_BOOL_CONVERSION(!mX);
            ASSERT(false == mX.has_value());
        }
        {
            Obj mX(2);
            ASSERT_IF_BOOL_CONVERSION(mX);
            ASSERT(true == mX.has_value());
        }
    }

    if (veryVerbose) printf("\tUsing MyClass2.\n");
    {
        bslma::TestAllocator da("default", veryVeryVeryVerbose);
        bslma::DefaultAllocatorGuard dag(&da);

        typedef MyClass2                 ValueType;
        typedef bsl::optional<ValueType> Obj;

        {
            Obj mX;
            const Obj& X = mX;
            ASSERT_IF_BOOL_CONVERSION(!X);
            ASSERT(false == X.has_value());

            mX.emplace(V2);
            ASSERT_IF_BOOL_CONVERSION(mX);
            ASSERT(true == mX.has_value());

            mX.reset();
            ASSERT_IF_BOOL_CONVERSION(!mX);
            ASSERT(false == mX.has_value());
        }
        {
            Obj mX(V2);
            ASSERT_IF_BOOL_CONVERSION(mX);
            ASSERT(true == mX.has_value());
        }
    }
}
void bslstl_optional_test5()
{
    // --------------------------------------------------------------------
    // TESTING 'reset' MEMBER FUNCTION
    //   This test will verify that the 'reset 'function works as expected.
    //   The test relies on constructors, 'has_value' and 'emplace' methods.
    //
    // Concerns:
    //: 1 Calling 'reset' on an disengaged 'optional' leaves it disengaged.
    //: 2 Calling 'reset' on an engaged 'optional' makes it disengaged and the
    //:   destructor of the 'value_type' is invoked.
    //: 3 All of the above concerns apply whether or not the 'optional'
    //:   object's 'value_type' is 'const'.
    //: 4 All of the above concerns apply whether or not the 'optional'
    //:   object's 'value_type' is allocator aware.
    //: 5 For an allocator aware 'value_type', calling 'reset' does not modify
    //:   the allocator.
    //
    // Plan:
    //: 1 Create disengaged 'optional' of a non-allocator aware 'value_type'.
    //:   For concern 1, call 'reset' on the created object and verify that it
    //:   is still disengaged.
    //: 2 Emplace a value in the 'optional' object. Call 'reset' on the test
    //:   object and verify that it has been disengaged and that the destructor
    //:   of the 'value_type' has been invoked.
    //: 3 Repeat steps 1-2 with an 'optional' object of a 'const' qualified
    //:   'value_type'. For concern 4, verify that 'reset' method behaves the
    //:   same as for non 'const' qualified 'value_type'.
    //: 4 Repeat steps 1-3 with an 'optional' object of an allocator aware
    //:   'value_type'. For concern 3, verify that 'reset' method behaves the
    //:   same.
    //: 5 In step 4, for concern 5, verify that 'get_allocator' method returns
    //:   the allocator used to construct the 'optional' type after each call
    //:   to 'reset.
    //
    // Testing:
    //   void reset();
    //
    // --------------------------------------------------------------------

    if (verbose) printf(
                       "\nTESTING 'reset' MEMBER FUNCTION "
                       "\n===============================\n");

    if (veryVerbose) printf("\tUsing non allocator aware 'value_type'.\n");
    {
        typedef MyClass1                 ValueType;
        typedef bsl::optional<ValueType> Obj;

        {
            Obj mX;

            mX.reset();
            ASSERT(false == mX.has_value());

            mX.reset();
            ASSERT(false == mX.has_value());

            mX.emplace(1);
            ASSERT(true == mX.has_value());

            int dI = ValueType::destructorInvocations;
            mX.reset();
            ASSERT(false == mX.has_value());
            ASSERT(dI == ValueType::destructorInvocations-1);
        }
    }
    if (veryVerbose) printf("\tUsing 'const' non allocator aware value "
                            "type.\n");
    {
        typedef MyClass1 ValueType;
        typedef bsl::optional<ValueType> Obj;

        {
            Obj mX;

            mX.reset();
            ASSERT(false == mX.has_value());

            mX.reset();
            ASSERT(false == mX.has_value());

            mX.emplace(1);
            ASSERT(true == mX.has_value());

            int dI = ValueType::destructorInvocations;
            mX.reset();
            ASSERT(false == mX.has_value());
            ASSERT(dI == ValueType::destructorInvocations-1);
        }
    }

    if (veryVerbose) printf("\tUsing allocator aware 'value_type'.\n");
    {
        bslma::TestAllocator da("default", veryVeryVeryVerbose);
        bslma::TestAllocator oa("other", veryVeryVeryVerbose);

        bslma::DefaultAllocatorGuard dag(&da);

        typedef MyClass2                 ValueType;
        typedef bsl::optional<ValueType> Obj;

        {
            Obj mX(bsl::allocator_arg,&oa);

            mX.reset();
            ASSERT(false == mX.has_value());
            ASSERT(mX.get_allocator() == &oa);

            mX.reset();
            ASSERT(false == mX.has_value());
            ASSERT(mX.get_allocator() == &oa);

            mX.emplace(3);
            ASSERT(true == mX.has_value());

            int dI = ValueType::destructorInvocations;
            mX.reset();
            ASSERT(false == mX.has_value());
            ASSERT(mX.get_allocator() == &oa);
            ASSERT(dI == ValueType::destructorInvocations-1);
        }
    }
    if (veryVerbose) printf("\tUsing 'const' allocator aware 'value_type'."
                            "\n");
    {
        bslma::TestAllocator da("default", veryVeryVeryVerbose);
        bslma::TestAllocator oa("other", veryVeryVeryVerbose);

        bslma::DefaultAllocatorGuard dag(&da);

        typedef const MyClass2  ValueType;
        typedef bsl::optional<ValueType> Obj;

        {
          Obj mX(bsl::allocator_arg,&oa);

          mX.reset();
          ASSERT(false == mX.has_value());
          ASSERT(mX.get_allocator() == &oa);

          mX.reset();
          ASSERT(false == mX.has_value());
          ASSERT(mX.get_allocator() == &oa);

          mX.emplace(3);
          ASSERT(true == mX.has_value());

          int dI = ValueType::destructorInvocations;
          mX.reset();
          ASSERT(false == mX.has_value());
          ASSERT(mX.get_allocator() == &oa);
          ASSERT(dI == ValueType::destructorInvocations-1);
        }
    }
}
void bslstl_optional_test6()
{
    // --------------------------------------------------------------------
    // TESTING 'value' METHOD
    //   This test will verify that the 'value' method works as expected.
    //   The test relies on constructors and 'emplace' method.
    //
    //   MSVC2015 has a bug which removes constness from temporaries in certain
    //   situations. For example :
    //
    //       bsl::string = CObj("s").value();
    //
    //   invokes non const qualified overload of 'value', despite the fact the
    //   temporary is of a const qualified 'bsl::optional'. In the following
    //   example, the constness is preserved:
    //
    //       Cobj temp = Cobj("s");
    //       bsl::string = MovUtl::move(temp).value();
    //
    //    The tests have been written to take this issue into account.
    //
    // Concerns:
    //: 1 Calling 'value()' on a disengaged 'optional' throws
    //:   'bad_optional_access' exception.
    //: 2 Calling 'value()' on a engaged 'optional' returns the reference
    //:   to the 'value_type' object.
    //: 3 The reference returned from 'value()' is 'const' qualified if the
    //:   'optional' object is 'const' qualified, or if the 'value_type' is
    //:   'const' qualified.
    //: 4 In C++11 and onwards, the returned reference is an rvalue reference
    //    if the 'optional' object is an rvalue.
    //: 5 It is possible to modify non constant 'optional' of non constant
    //:   'value_type' through the reference returned by 'value' method.
    //: 6 That 'value' method behaves the same for allocator aware types and
    //:   non allocator aware types.
    //
    // Plan:
    //: 1 Create a disengaged 'optional' of non allocator aware type. Call
    //:   the 'value' method and, for concern 1, check that the
    //:   'bad_optional_access' exception is thrown.
    //: 2 Emplace a value in the 'optional' object. Call the 'value' method
    //:   and, for concern 2, check that the returned object has the expected
    //:   value.
    //: 3 For concern 3, using 'isConstRef' verify the 'const' qualification
    //:   of a reference returned from 'value' on a non 'const' 'optional' of
    //:   non 'const' 'value_type', 'const' 'optional' of non 'const'
    //:   'value_type', and 'const' 'optional' of 'const' 'value_type'
    //: 4 If ref qualifiers are supported, for concern 3, using 'isRvalueRef'
    //:   verify that the reference retruned from 'value' is an rvalue
    //:   reference if the 'optional' object is an rvalue.
    //: 5 Modify the value of the 'optional' object through the reference
    //:   returned from the 'value' method. For concern 5, call 'value' method
    //:   check that the value of the 'optional' object has been modified.
    //: 6 Call 'reset' on the 'optional' object. For concern 1, call 'value'
    //:   method and check that the 'bad_optional_access' exception is thrown.
    //: 7 For concern 6, repeat steps 1-6 with an 'optional' object of an
    //:   allocator aware type.
    //: 8 For concern 1, in steps 1-7, make sure no unexpected exception is
    //:   thrown.
    //
    // Testing:
    //   TYPE& value() &;
    //   const TYPE& value() & const;
    //   TYPE&& value() &&;
    //   const TYPE&& value() && const;
    //
    // --------------------------------------------------------------------

    if (verbose) printf(
                       "\nTESTING 'value' METHOD"
                       "\n======================\n");

    if (veryVerbose) printf("\tUsing 'bsl::optional<int>'.\n");
    bool unexpected_exception_thrown = false;
    try
    {
        typedef MyClass1                 ValueType;
        typedef bsl::optional<ValueType> Obj;
        typedef bsl::optional<const ValueType> ObjC;
        {
            Obj mX; const Obj& X = mX;

            bool bad_optional_exception_caught = false;
            try{ mX.value();}
            catch (const bsl::bad_optional_access &)
            {
              bad_optional_exception_caught = true;
            }
            ASSERT(bad_optional_exception_caught);
            bad_optional_exception_caught = false;

            mX.emplace(3);
            ASSERT(mX.value().value() == 3);

            MyClass1& ri = mX.value();
            ri.d_def.d_value = 7;
            ASSERT(mX.value().value() == 7);

            ObjC cmX(3);
            ASSERT(!isConstRef(mX.value()));
            ASSERT(isConstRef(X.value()));
            ASSERT(isConstRef(cmX.value()));
#ifdef BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS
            ASSERT(!isRvalueRef(mX.value()));
            ASSERT(isRvalueRef(Obj(3).value()));
            ASSERT(isRvalueRef(ObjC(4).value()));
#endif // defined(BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS)

            mX.reset();
            try{ mX.value();}
            catch (bsl::bad_optional_access &)
            {
              bad_optional_exception_caught = true;
            }
            ASSERT(bad_optional_exception_caught);
            bad_optional_exception_caught = false;
        }
    } catch (...)
    {
        unexpected_exception_thrown = true;
    }
    ASSERT(unexpected_exception_thrown == false);
    unexpected_exception_thrown = false;
    if (veryVerbose) printf("\tUsing allocator aware 'value_type'.\n");
    try {
        bslma::TestAllocator da("default", veryVeryVeryVerbose);
        bslma::TestAllocator oa("other", veryVeryVeryVerbose);

        bslma::DefaultAllocatorGuard dag(&da);

        typedef MyClass2             ValueType;
        typedef bsl::optional<ValueType>        Obj;
        typedef bsl::optional<const ValueType>  ObjC;

        Obj mX(bsl::allocator_arg, &oa); const Obj& X = mX;

        bool bad_optional_exception_caught = false;
        try{ mX.value();}
        catch (bsl::bad_optional_access &)
        {
          bad_optional_exception_caught = true;
        }
        ASSERT(bad_optional_exception_caught);
        bad_optional_exception_caught = false;

        mX.emplace(4);
        ASSERT(mX.value().value() == 4);

        ObjC cmX(3);
        ASSERT(!isConstRef(mX.value()));
        ASSERT(isConstRef(X.value()));
        ASSERT(isConstRef(cmX.value()));
#ifdef BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS
        ASSERT(!isRvalueRef(mX.value()));
        ASSERT(isRvalueRef(Obj(3).value()));
        ASSERT(isRvalueRef(ObjC(4).value()));
#endif // defined(BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS)

        mX.reset();
        try{ mX.value();}
        catch (bsl::bad_optional_access &)
        {
          bad_optional_exception_caught = true;
        }
        ASSERT(bad_optional_exception_caught);
    }catch (...)
    {
        unexpected_exception_thrown = true;
    }
}
void bslstl_optional_test7()
{
    // --------------------------------------------------------------------
    // TESTING 'value_or' METHOD
    //   This test will verify that the 'value_or' method works as expected.
    //   The test relies on constructors and 'emplace' method.
    //
    // Concerns:
    //: 1 Calling 'value_or' on a disengaged 'optional' object returns the
    //:   argument value converted to the 'value_type' of the 'optional'
    //:   object.
    //: 2 Calling 'value_or' on an engaged 'optional' object returns the value
    //:   in the 'optional' object.
    //: 3 In C++11, when calling value_or on an engaged 'optional' rvalue, the
    //:   returned object is created by moving from the 'value_type' object in
    //:   'optional'.
    //: 4 It is possible to call 'value_or' on a constant 'optional' object.
    //: 6 If 'value_type' is allocator aware, the allocator of the object
    //:   returned from a single argument 'value_or' method is determined by
    //:   the copy/move/conversion constructor of the 'value_type'.
    //: 7 The allocator of the object returned from the allocator extended
    //:   'value_or' method is the allocator specified in the call to
    //:   'value_or'.
    //
    // Plan:
    //
    //: 1 Create a disengaged 'optional' of non allocator aware type.
    //:   For concern 1, call 'value_or' and check that the return object
    //:   matches the argument given to 'value_or'.
    //: 2 Emplace a value in the 'optional' object. Call 'value_or' and, for
    //:   concern 2, check that the returned value matches the value in the
    //:   'optional' object.
    //: 3 If ref qualifieres are supported, call 'value_or' on an engaged
    //:   'optional' rvalue. For concern 3, verify the resulting objects has
    //:   been created by move construction.
    //: 4 In step 1, for concern 4, call ' value_or' through a 'const'
    //:   reference to the  'optional' object.
    //: 6 Repeat steps 1-5 using an allocator aware 'value_type'. For concern
    //:   6, check that the allocator of the returned object is correct.
    //: 7 Repeat steps 1-5 using an allocator aware 'value_type' and the
    //:   allocator extended 'value_or' method. For concerns 7, check
    //:   that the allocator of the returned object is the one specified in the
    //:   call to 'value_or'.
    //
    // Testing:
    //  TYPE value_or(BSLS_COMPILERFEATURES_FORWARD_REF(ANY_TYPE)) const;
    //  TYPE value_or(bsl::allocator_arg_t,
    //                allocator_type,
    //                BSLS_COMPILERFEATURES_FORWARD_REF(ANY_TYPE)
    //                ) const;
    //
    // --------------------------------------------------------------------

    if (verbose) printf(
                       "\nTESTING 'value_or' METHOD"
                       "\n=========================\n");

    if (veryVerbose) printf("\tUsing non allocator aware type.\n");
    {
        typedef MyClass1                 ValueType;
        typedef bsl::optional<ValueType> Obj;
        {
            Obj mX; const Obj& X = mX;

            const ValueType &i1 = X.value_or(2);
            ASSERT(!X.has_value());
            ASSERT(i1.value() == 2);

            mX.emplace(3);
            ASSERT(mX.value().value() == 3);
            const ValueType &i2 = mX.value_or(i1);
            ASSERT(mX.value().value() == 3);
            ASSERT(i2.value() == 3);

#ifdef BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS
            Obj source(4);
            const ValueType &i3 = MovUtl::move(source).value_or(77);
            ASSERT(i3.value() == 4);
            ASSERT(source.value().value() == MOVED_FROM_VAL);
#endif  //defined(BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS)
        }
    }
    if (veryVerbose) printf("\tUsing allocator aware 'value_type'.\n");
    {
        bslma::TestAllocator da("default", veryVeryVeryVerbose);
        bslma::TestAllocator oa("other", veryVeryVeryVerbose);
        bslma::TestAllocator ta("third", veryVeryVeryVerbose);

        bslma::DefaultAllocatorGuard dag(&da);

        typedef MyClass2                 ValueType;
        typedef bsl::optional<ValueType> Obj;
        if (veryVerbose) printf("\tests of non allocator extended value_or'."
                                "\n");
        {
            Obj mX(bsl::allocator_arg, &oa); const Obj& X = mX;
            MyClass2 val(5, &ta);

            const ValueType &i1 = X.value_or(val);
            ASSERT(i1.value() == 5);
            // allocator is determined by the copy constructor of MyClass2
            // which will set the allocator to 0
            ASSERT(i1.d_def.d_allocator_p == 0);

            mX.emplace(88);
            const ValueType &i2 = mX.value_or(val);
            ASSERT(i2.value() == 88);
            ASSERT(i2.d_def.d_allocator_p == 0);

#ifdef BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS
            Obj source(4);
            const ValueType &i3 = MovUtl::move(source).value_or(77);
            ASSERT(i3 == 4);
            ASSERT(source.value().value() == MOVED_FROM_VAL);
#endif  //defined(BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS)
        }
#ifdef BSL_COMPILERFEATURES_GUARANTEED_COPY_ELISION
        if (veryVerbose) printf("\tallocator tests of allocator extended "
            "value_or'.\n");
        {
           Obj mX(bsl::allocator_arg, &oa); const Obj& X = mX;
           MyClass2 val(5, &da);

           const ValueType &i1 = X.value_or(bsl::allocator_arg, &ta, val);
           ASSERT(i1.value().value() == 5);
           ASSERT(i1.d_def.d_allocator_p == &ta);

           mX.emplace(8);
           const ValueType &i2 = mX.value_or(bsl::allocator_arg, &ta, val);
           ASSERT(i2.value().value() == 8);
           ASSERT(i2.d_def.d_allocator_p == &ta);

#ifdef BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS
            Obj source(4);
            const ValueType &i3 = MovUtl::move(source).value_or(
                                                bsl::allocator_arg, &ta, val);
            ASSERT(i3 == 4);
            ASSERT(source.value().value() == MOVED_FROM_VAL);
            ASSERT(i3.d_def.d_allocator_p == &ta);
#endif  //defined(BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS)
        }
#endif //BSL_COMPILERFEATURES_GUARANTEED_COPY_ELISION
    }
}
void bslstl_optional_test8()
{
    // --------------------------------------------------------------------
    // TESTING 'operator->' MEMBER FUNCTION
    //   This test will verify that the 'operator->' works as expected.
    //
    // Concerns:
    //: 1 Calling 'operator->' on an engaged 'optional' object returns a
    //:   pointer to the contained value object.
    //: 2 Returned pointer is 'const' qualified if the 'optional' object
    //:   is 'const' qualified, or if the 'value_type' is 'const' qualified.
    //: 3 All of the above concerns apply whether or not the 'value_type' is
    //:   allocator aware.
    //
    // Plan:
    //: 1 Create an engaged 'optional' of no allocator aware type. Using
    //:   'operator->', for concern 1 check the returned value matches the
    //:   expected value.
    //: 2 Assign a value to the 'optional' object through a call to
    //:   'operator->'. For concern 1, check the value of the 'optional' object
    //:   is as expected.
    //: 3 For concern 2, check that the pointer returned from 'operator->'
    //:   is not 'const' qualified if neither the 'optional' object, nor the
    //:   'value_type' are 'const' qualified.
    //: 4 For concern 2, check that the pointer returned from 'operator->'
    //:   is 'const' qualified if the 'optional' object is 'const' qualified.
    //: 5 For concern 2, check that the pointer returned from 'operator->'
    //:   is 'const' qualified if the 'value_type' is 'const' qualified.
    //; 6 Repeat steps 1-5 with an allocator aware 'value_type'. For concern 1
    //:   , check the allocator of the object retrieved from 'operator->'
    //:   matches the allocator returned from 'get_allocator' method.
    //
    // Testing:
    //   const T* operator->() const;
    //   T* operator->();
    //
    //   void value();
    //
    // --------------------------------------------------------------------

    if (verbose) printf(
                       "\nTESTING 'operator->' MEMBER FUNCTION"
                       "\n====================================\n");

    if (veryVerbose) printf("\tUsing 'MyClass1'.\n");
    {
        typedef MyClass1                            ValueType;
        typedef const MyClass1                      ConstValueType;
        typedef bsl::optional<ValueType> Obj;
        typedef const Obj CObj;
        typedef bsl::optional<ConstValueType> ObjC;
        typedef const CObj CObjC;

        {
            Obj mX(ValueType(4));
            CObj cobjX(ValueType(8));
            ObjC objcX(ValueType(9));
            CObjC cobjcX(ValueType(10));

            ASSERT(mX->d_def.d_value == 4);
            ASSERT(cobjX->d_def.d_value == 8);
            ASSERT(objcX->d_def.d_value == 9);
            ASSERT(cobjcX->d_def.d_value == 10);

            mX->d_def.d_value = 6;
            ASSERT(mX.value().d_def.d_value == 6);

            ASSERT(!isConstPtr(mX.operator->()));
            ASSERT(isConstPtr(cobjX.operator->()));
            ASSERT(isConstPtr(objcX.operator->()));
       }
    }
    if (veryVerbose) printf("\tUsing 'MyClass2'.\n");
    {
        bslma::TestAllocator da("default", veryVeryVeryVerbose);
        bslma::TestAllocator oa("other", veryVeryVeryVerbose);
        bslma::TestAllocator ta("third", veryVeryVeryVerbose);

        bslma::DefaultAllocatorGuard dag(&da);
        typedef MyClass2                            ValueType;
        typedef const MyClass2                      ConstValueType;
        typedef bsl::optional<ValueType> Obj;
        typedef const Obj CObj;
        typedef bsl::optional<ConstValueType> ObjC;
        typedef const CObj CObjC;

        {
            Obj mX(bsl::allocator_arg, &oa, V2);
            CObj cobjX(bsl::allocator_arg, &ta, V2);
            ObjC objcX(ValueType(9));
            CObjC cobjcX(ValueType(10));

            ASSERT(mX->d_def.d_value == 2);
            ASSERT(mX->d_def.d_allocator_p == &oa);
            ASSERT(cobjX->d_def.d_value == 2);
            ASSERT(cobjX->d_def.d_allocator_p == &ta);
            ASSERT(objcX->d_def.d_value == 9);
            ASSERT(objcX->d_def.d_allocator_p == &da);
            ASSERT(cobjcX->d_def.d_value == 10);

            mX->d_def.d_value = 6;
            ASSERT(mX.value().d_def.d_value == 6);

            ASSERT(!isConstPtr(mX.operator->()));
            ASSERT(isConstPtr(cobjX.operator->()));
            ASSERT(isConstPtr(objcX.operator->()));
       }
    }
}
void bslstl_optional_test9()
{
    // --------------------------------------------------------------------
    // TESTING 'operator*' FUNCTIONALITY
    //   This test will verify that the 'operator*' works as expected.
    //
    // Concerns:
    //: 1 Calling 'operator* 'on an engaged 'optional' returns a reference to
    //:   the contained value.
    //: 2 Returned reference is 'const' qualified if the 'optional' object
    //:   is 'const' qualified, or if the 'value_type' is 'const' qualified.
    //: 3 All of the above concerns apply whether or not the 'value_type' is
    //:   allocator aware.
    //
    // Plan:
    //: 1 Create an engaged 'optional' of a non allocator aware type. Using
    //:   'operator*', for concern 1, check the returned reference matches the
    //:   contained value.
    //: 2 Modify the value of the object obtained using 'operator*'. For
    //:   concern 1, check the value of the 'optional' has been modified.
    //: 3 For concern 2, check that the pointer returned from 'operator*'
    //:   is not 'const' qualified if neither the 'optional' object, nor the
    //:   'value_type' are 'const' qualified.
    //: 4 For concern 2, check that the pointer returned from 'operator*'
    //:   is 'const' qualified if the 'optional' object is 'const' qualified.
    //: 5 For concern 2, check that the pointer returned from 'operator*'
    //:   is 'const' qualified if the 'value_type' is 'const' qualified.
    //: 6 Repeat steps 1-4 with an 'optional' object of an allocator aware
    //:   'value_type'. For concern 1, verify that the allocator of the
    //:   returned reference object matches the allocator of the 'optional'
    //:   object.
    //
    //
    // Testing:
    //   const T* operator->() const;
    //   T* operator->();
    //
    //   void value();
    //
    // --------------------------------------------------------------------

    if (verbose) printf(
                       "\nTESTING operator* FUNCTIONALITY "
                       "\n===============================\n");

    if (veryVerbose) printf("\tUsing non allocator aware type.\n");
    {
        typedef MyClass1                            ValueType;
        typedef const MyClass1                      ConstValueType;
        typedef bsl::optional<ValueType> Obj;
        typedef const Obj CObj;
        typedef bsl::optional<ConstValueType> ObjC;
        typedef const CObj CObjC;

        {
            Obj mX(ValueType(4));
            CObj cobjX(ValueType(8));
            ObjC objcX(ValueType(9));
            CObjC cobjcX(ValueType(10));

            ASSERT((*mX).d_def.d_value == 4);
            ASSERT((*cobjX).d_def.d_value == 8);
            ASSERT((*objcX).d_def.d_value == 9);
            ASSERT((*cobjcX).d_def.d_value == 10);

            (*mX).d_def.d_value = 6;
            ASSERT(mX.value().d_def.d_value == 6);

            ASSERT(!isConstRef(mX.operator*()));
            ASSERT(isConstRef(cobjX.operator*()));
            ASSERT(isConstRef(objcX.operator*()));
       }
    }
    if (veryVerbose) printf("\tUsing allocator aware type.\n");
    {
       bslma::TestAllocator da("default", veryVeryVeryVerbose);
       bslma::TestAllocator oa("other", veryVeryVeryVerbose);
       bslma::TestAllocator ta("third", veryVeryVeryVerbose);

       bslma::DefaultAllocatorGuard dag(&da);
       typedef MyClass2                            ValueType;
       typedef const MyClass2                      ConstValueType;
       typedef bsl::optional<ValueType> Obj;
       typedef const Obj CObj;
       typedef bsl::optional<ConstValueType> ObjC;
       typedef const CObj CObjC;

       {
           Obj mX(bsl::allocator_arg, &oa, V2);
           CObj cobjX(bsl::allocator_arg, &ta, V2);
           ObjC objcX(ValueType(9));
           CObjC cobjcX(ValueType(10));

           ASSERT((*mX).d_def.d_value == 2);
           ASSERT((*mX).d_def.d_allocator_p == &oa);
           ASSERT((*cobjX).d_def.d_value == 2);
           ASSERT((*cobjX).d_def.d_allocator_p == &ta);
           ASSERT((*objcX).d_def.d_value == 9);
           ASSERT((*objcX).d_def.d_allocator_p == &da);
           ASSERT((*cobjcX).d_def.d_value == 10);

           (*mX).d_def.d_value = 6;
           ASSERT(mX.value().d_def.d_value == 6);

           ASSERT(!isConstRef(mX.operator*()));
           ASSERT(isConstRef(cobjX.operator*()));
           ASSERT(isConstRef(objcX.operator*()));
       }
   }
}
void bslstl_optional_test10()
{
    // --------------------------------------------------------------------
    // TESTING 'emplace' METHOD
    //   This test will verify that the 'emplace' method works as expected.
    //
    // Concerns:
    //: 1 Calling 'emplace' on a non-engaged 'optional' creates a 'value_type'
    //:   object by invoking the constructor with the 'emplace' arguments.
    //: 2 Calling 'emplace' on an engaged 'optional' replaces the 'value_type'
    //:   object with a new one created by invoking the constructor with the
    //:   'emplace' arguments.
    //: 3 Calling 'emplace' with no arguments creates a default constructed
    //:   'value_type' object.
    //: 4 If 'value_type' is allocator aware, 'emplace' invokes the allocator
    //:   extended constructor using the 'optional''s allocator.
    //: 5 If 'value_type' is allocator aware, calling 'emplace' does not modify
    //:    the allocator, even when called with an rvalue 'value_type' argument
    //: 6 'Emplace' can be used with a const qualified 'value_type'.
    //: 7 'Emplace' can not be called on a const qualified 'optional'.
    //: 8 There are no unnecessary 'value_type' copies created
    //: 9 Variadic arguments to 'emplace' method are correctly forwarded to the
    //:   constructor arguments.
    //
    // Plan:
    //: 1 Create a non engaged 'optional' object of non allocator aware value
    //:   type. Call 'emplace' and, for concern 1, verify that the 'optional'
    //:   object contains the expected value.
    //: 2 For concern 2, call 'emplace' method on the engaged 'optional' object
    //:   and verify the value of the 'optional' object has changed.
    //: 3 In step 1, for concern 3, use 'emplace' method that takes no
    //:   arguments and verify the object is default constructed.
    //: 4 For concern 6, repeat steps 1-3 using a const qualified 'value_type'.
    //: 5 For concern 2, repeat steps 1-4 using an allocator aware 'value_type'
    //:   and verify the allocator used for the constructed value is as
    //:   expected.
    //: 6 For concern 3, in step 5, call 'emplace' with and rvalue of value
    //:   type using a different allocator to the 'optional' object. Verify the
    //:   allocator has not propagated.
    //: 7 For concern 8, in steps 1-6, verify there are no additional copies of
    //:   the 'value_type' created.
    //: 8 Invoke 'emplace' method with varying number of arguments, some of
    //:   which are to be moved from. For concern 9, verify the arguments are
    //:   perfect forwarded to the constructor in the correct order.
    //: 9 In step 8, for concern 8, verify that the additional copies of the
    //:   arguments or 'value_type' have been created.
    //:10 For concern 7, verify that 'emplace' method can not be invoked on a
    //:   const qualified 'optional'. Note that this test requires compilation
    //:   errors and needs to be enabled and checked manually.
    //
    // Testing:
    //
    //   void emplace();
    //   void emplace(Args&&...);
    //
    //   void value();
    //
    // --------------------------------------------------------------------

    if (verbose) printf(
                       "\nTESTING 'emplace' METHOD"
                       "\n========================\n");

    if (veryVerbose) printf("\tUsing non allocator aware 'value_type'.\n");
    {
        typedef MyClass1                  ValueType;
        typedef bsl::optional<ValueType> Obj;

        {
            Obj mX;
            int CCI = ValueType::copyConstructorInvocations;
            int MCI = ValueType::moveConstructorInvocations;

            mX.emplace();
            ASSERT(CCI == ValueType::copyConstructorInvocations);
            ASSERT(MCI == ValueType::moveConstructorInvocations);
            ASSERT(mX.has_value());
            ASSERT(mX->d_def.d_value == 0 );

            ValueType other(3);
            CCI = ValueType::copyConstructorInvocations;
            MCI = ValueType::moveConstructorInvocations;
            mX.emplace(other);
            ASSERT(mX.has_value());
            ASSERT(mX->d_def.d_value == 3 );
            ASSERT(CCI == ValueType::copyConstructorInvocations -1 );
            ASSERT(MCI == ValueType::moveConstructorInvocations);

            ValueType third(4);
            CCI = ValueType::copyConstructorInvocations;
            MCI = ValueType::moveConstructorInvocations;
            mX.emplace(MovUtl::move(third));
            ASSERT(mX.has_value());
            ASSERT(mX->d_def.d_value == 4 );
            ASSERT(CCI == ValueType::copyConstructorInvocations );
            ASSERT(MCI == ValueType::moveConstructorInvocations -1);

            CCI = ValueType::copyConstructorInvocations;
            MCI = ValueType::moveConstructorInvocations;
            mX.emplace(6);
            ASSERT(mX.has_value());
            ASSERT(mX->d_def.d_value == 6 );
            ASSERT(CCI == MyClass1::copyConstructorInvocations);
            ASSERT(MCI == MyClass1::moveConstructorInvocations);
        }
    }
    if (veryVerbose) printf("\tUsing non allocator aware const qualified "
                          "'value_type'.\n");
    {
        typedef const MyClass1 ValueType;
        typedef bsl::optional<ValueType> Obj;

        {
            Obj mX;
            int CCI = ValueType::copyConstructorInvocations;
            int MCI = ValueType::moveConstructorInvocations;

            mX.emplace();
            ASSERT(CCI == ValueType::copyConstructorInvocations);
            ASSERT(MCI == ValueType::moveConstructorInvocations);
            ASSERT(mX.has_value());
            ASSERT(mX->d_def.d_value == 0 );

            ValueType other(3);
            CCI = ValueType::copyConstructorInvocations;
            MCI = ValueType::moveConstructorInvocations;
            mX.emplace(other);
            ASSERT(mX.has_value());
            ASSERT(mX->d_def.d_value == 3 );
            ASSERT(CCI == ValueType::copyConstructorInvocations -1 );
            ASSERT(MCI == ValueType::moveConstructorInvocations);

            // const qualified objects can't be moved from
            ValueType third(4);
            CCI = ValueType::copyConstructorInvocations;
            MCI = ValueType::moveConstructorInvocations;
            mX.emplace(MovUtl::move(third));
            ASSERT(mX.has_value());
            ASSERT(mX->d_def.d_value == 4 );
            ASSERT(CCI == ValueType::copyConstructorInvocations -1);
            ASSERT(MCI == ValueType::moveConstructorInvocations);

            CCI = ValueType::copyConstructorInvocations;
            MCI = ValueType::moveConstructorInvocations;
            mX.emplace(6);
            ASSERT(mX.has_value());
            ASSERT(mX->d_def.d_value == 6 );
            ASSERT(CCI == MyClass1::copyConstructorInvocations);
            ASSERT(MCI == MyClass1::moveConstructorInvocations);
        }
    }
    if (veryVerbose) printf("\tUsing allocator aware 'value_type'.\n");
    {
        bslma::TestAllocator da("default", veryVeryVeryVerbose);
        bslma::TestAllocator oa("other", veryVeryVeryVerbose);
        bslma::TestAllocator ta("third", veryVeryVeryVerbose);

        bslma::DefaultAllocatorGuard dag(&da);

       typedef MyClass2                 ValueType;
       typedef bsl::optional<ValueType> Obj;

       {
           Obj mX(bsl::allocator_arg, &oa);
           int CCI = ValueType::copyConstructorInvocations;
           int MCI = ValueType::moveConstructorInvocations;

           mX.emplace();
           ASSERT(CCI == ValueType::copyConstructorInvocations);
           ASSERT(MCI == ValueType::moveConstructorInvocations);
           ASSERT(mX.has_value());
           ASSERT(mX->d_def.d_value == 0 );
           ASSERT(mX.get_allocator() == &oa );

           ValueType other(3, &da);
           ASSERT(other.d_def.d_allocator_p == &da );
           CCI = ValueType::copyConstructorInvocations;
           MCI = ValueType::moveConstructorInvocations;
           mX.emplace(other);
           ASSERT(mX.has_value());
           ASSERT(mX->d_def.d_value == 3 );
           ASSERT(mX.get_allocator() == &oa );
           ASSERT(mX->d_def.d_allocator_p == &oa );
           ASSERT(CCI == ValueType::copyConstructorInvocations -1 );
           ASSERT(MCI == ValueType::moveConstructorInvocations);

           ValueType third(4, &da);
           CCI = ValueType::copyConstructorInvocations;
           MCI = ValueType::moveConstructorInvocations;
           mX.emplace(MovUtl::move(third));
           ASSERT(third.d_def.d_allocator_p == &da );
           ASSERT(mX.has_value());
           ASSERT(mX->d_def.d_value == 4 );
           ASSERT(CCI == ValueType::copyConstructorInvocations);
           ASSERT(MCI == ValueType::moveConstructorInvocations -1);
           ASSERT(mX.get_allocator() == &oa );
           ASSERT(mX->d_def.d_allocator_p == &oa );

           CCI = ValueType::copyConstructorInvocations;
           MCI = ValueType::moveConstructorInvocations;
           mX.emplace(6);
           ASSERT(mX.has_value());
           ASSERT(mX->d_def.d_value == 6 );
           ASSERT(mX.get_allocator() == &oa );
           ASSERT(mX->d_def.d_allocator_p == &oa );
           ASSERT(CCI == ValueType::copyConstructorInvocations);
           ASSERT(MCI == ValueType::moveConstructorInvocations);
       }
    }
    if (veryVerbose) printf("\tUsing allocator aware 'value_type'.\n");
    {
        bslma::TestAllocator da("default", veryVeryVeryVerbose);
        bslma::TestAllocator oa("other", veryVeryVeryVerbose);
        bslma::TestAllocator ta("third", veryVeryVeryVerbose);

        bslma::DefaultAllocatorGuard dag(&da);

       typedef MyClass2                 ValueType;
       typedef bsl::optional<ValueType> Obj;

       {
           Obj mX(bsl::allocator_arg, &oa);
           int CCI = ValueType::copyConstructorInvocations;
           int MCI = ValueType::moveConstructorInvocations;

           mX.emplace();
           ASSERT(CCI == ValueType::copyConstructorInvocations);
           ASSERT(MCI == ValueType::moveConstructorInvocations);
           ASSERT(mX.has_value());
           ASSERT(mX->d_def.d_value == 0 );
           ASSERT(mX.get_allocator() == &oa );

           ValueType other(3, &da);
           ASSERT(other.d_def.d_allocator_p == &da );
           CCI = ValueType::copyConstructorInvocations;
           MCI = ValueType::moveConstructorInvocations;
           mX.emplace(other);
           ASSERT(mX.has_value());
           ASSERT(mX->d_def.d_value == 3 );
           ASSERT(mX.get_allocator() == &oa );
           ASSERT(mX->d_def.d_allocator_p == &oa );
           ASSERT(CCI == ValueType::copyConstructorInvocations -1 );
           ASSERT(MCI == ValueType::moveConstructorInvocations);

           ValueType third(4, &da);
           CCI = ValueType::copyConstructorInvocations;
           MCI = ValueType::moveConstructorInvocations;
           mX.emplace(MovUtl::move(third));
           ASSERT(third.d_def.d_allocator_p == &da );
           ASSERT(mX.has_value());
           ASSERT(mX->d_def.d_value == 4 );

           ASSERT(CCI == ValueType::copyConstructorInvocations );
           ASSERT(MCI == ValueType::moveConstructorInvocations -1);
           ASSERT(mX.get_allocator() == &oa );
           ASSERT(mX->d_def.d_allocator_p == &oa );

           CCI = ValueType::copyConstructorInvocations;
           MCI = ValueType::moveConstructorInvocations;
           mX.emplace(6);
           ASSERT(mX.has_value());
           ASSERT(mX->d_def.d_value == 6 );
           ASSERT(mX.get_allocator() == &oa );
           ASSERT(mX->d_def.d_allocator_p == &oa );
           ASSERT(CCI == ValueType::copyConstructorInvocations);
           ASSERT(MCI == ValueType::moveConstructorInvocations);
       }
    }
    if (veryVerbose) printf("\tUsing allocator aware const qualified value "
                        "type.\n");
    {
        bslma::TestAllocator da("default", veryVeryVeryVerbose);
        bslma::TestAllocator oa("other", veryVeryVeryVerbose);
        bslma::TestAllocator ta("third", veryVeryVeryVerbose);

        bslma::DefaultAllocatorGuard dag(&da);

       typedef const MyClass2a          ValueType;
       typedef bsl::optional<ValueType> Obj;

       {
           Obj mX(bsl::allocator_arg, &oa);
           int CCI = ValueType::copyConstructorInvocations;
           int MCI = ValueType::moveConstructorInvocations;

           mX.emplace();
           ASSERT(mX.has_value());
           ASSERT(mX->d_data.d_def.d_value == 0 );
           ASSERT(mX.get_allocator() == &oa );
           ASSERT(CCI == ValueType::copyConstructorInvocations);
           ASSERT(MCI == ValueType::moveConstructorInvocations);

           ValueType other(bsl::allocator_arg, &da, 3);
           ASSERT(other.d_data.d_def.d_allocator_p == &da);
           CCI = ValueType::copyConstructorInvocations;
           MCI = ValueType::moveConstructorInvocations;
           mX.emplace(other);
           ASSERT(mX.has_value());
           ASSERT(mX->d_data.d_def.d_value == 3 );
           ASSERT(mX.get_allocator() == &oa);
           ASSERT(mX->d_data.d_def.d_allocator_p == &oa);
           ASSERT(CCI == ValueType::copyConstructorInvocations -1);
           ASSERT(MCI == ValueType::moveConstructorInvocations);

           ValueType third(bsl::allocator_arg, &da, 4);
           CCI = ValueType::copyConstructorInvocations;
           MCI = ValueType::moveConstructorInvocations;
           mX.emplace(MovUtl::move(third));
           ASSERT(third.d_data.d_def.d_allocator_p == &da);
           ASSERT(mX.has_value());
           ASSERT(mX->d_data.d_def.d_value == 4);
           // const objects can't be moved from
           ASSERT(CCI == ValueType::copyConstructorInvocations -1);
           ASSERT(MCI == ValueType::moveConstructorInvocations);
           ASSERT(mX.get_allocator() == &oa);
           ASSERT(mX->d_data.d_def.d_allocator_p == &oa);

           CCI = ValueType::copyConstructorInvocations;
           MCI = ValueType::moveConstructorInvocations;
           mX.emplace(6);
           ASSERT(mX.has_value());
           ASSERT(mX->d_data.d_def.d_value == 6);
           ASSERT(mX.get_allocator() == &oa);
           ASSERT(mX->d_data.d_def.d_allocator_p == &oa);
           ASSERT(CCI == ValueType::copyConstructorInvocations);
           ASSERT(MCI == ValueType::moveConstructorInvocations);
      }
    }
    if (veryVerbose) printf("\tTesting var args emplace .\n");
    {
        {
          ConstructTestTypeNoAlloc EXP;
          bsl::optional<ConstructTestTypeNoAlloc> obj;
          int CCI = ConstructTestTypeNoAlloc::copyConstructorInvocations;
          int MCI = ConstructTestTypeNoAlloc::moveConstructorInvocations;
          obj.emplace();
          ASSERT(EXP == obj.value());
          ASSERT(CCI == ConstructTestTypeNoAlloc::copyConstructorInvocations);
          ASSERT(MCI == ConstructTestTypeNoAlloc::moveConstructorInvocations);
          ASSERT(createdAlike(EXP,obj.value()) == true);
        }
        {
          ConstructTestTypeAlloc EXP;
          bslma::TestAllocator oa("other", veryVeryVeryVerbose);
          bsl::optional<ConstructTestTypeAlloc> obj(bsl::allocator_arg, &oa);
          int CCI = ConstructTestTypeAlloc::copyConstructorInvocations;
          int MCI = ConstructTestTypeAlloc::moveConstructorInvocations;
          obj.emplace();
          ASSERT(EXP == obj .value());
          ASSERT(&oa == obj .value().d_allocator_p);
          ASSERT(CCI == ConstructTestTypeAlloc::copyConstructorInvocations);
          ASSERT(MCI == ConstructTestTypeAlloc::moveConstructorInvocations);
          ASSERT(createdAlike(EXP,obj.value()) == true);
        }
        {
          ConstructTestTypeAllocArgT EXP;
          bslma::TestAllocator oa("other", veryVeryVeryVerbose);
          bsl::optional<ConstructTestTypeAllocArgT>
                               obj(bsl::allocator_arg, &oa);
          int CCI = ConstructTestTypeAllocArgT::copyConstructorInvocations;
          int MCI = ConstructTestTypeAllocArgT::moveConstructorInvocations;
          obj.emplace();
          ASSERT(EXP == obj .value());
          ASSERT(&oa == obj .value().d_allocator_p);
          ASSERT(CCI ==
                     ConstructTestTypeAllocArgT::copyConstructorInvocations);
          ASSERT(MCI ==
                     ConstructTestTypeAllocArgT::moveConstructorInvocations);
          ASSERT(createdAlike(EXP,obj.value()) == true);
        }

        TEST_EMPLACE((VA1));
        TEST_EMPLACE((MovUtl::move(VA1)));

        TEST_EMPLACE((MovUtl::move(VA1), VA2));
        TEST_EMPLACE((VA1, MovUtl::move(VA2)));

        TEST_EMPLACE((MovUtl::move(VA1), VA2, VA3));
        TEST_EMPLACE((VA1, MovUtl::move(VA2), VA3));
        TEST_EMPLACE((VA1, VA2, MovUtl::move(VA3)));

        TEST_EMPLACE((MovUtl::move(VA1), VA2,
                    MovUtl::move(VA3), VA4));
        TEST_EMPLACE((VA1, MovUtl::move(VA2),
                    VA3, MovUtl::move(VA4)));

        TEST_EMPLACE((MovUtl::move(VA1), VA2,
                    MovUtl::move(VA3), VA4,
                    MovUtl::move(VA5)));
        TEST_EMPLACE((VA1, MovUtl::move(VA2),
                    VA3, MovUtl::move(VA4),
                    VA5));
/*
        TEST_EMPLACE((MovUtl::move(VA1), VA2,
                    MovUtl::move(VA3), VA4,
                    MovUtl::move(VA5), VA6));
        TEST_EMPLACE((VA1, MovUtl::move(VA2),
                    VA3, MovUtl::move(VA4),
                    VA5, MovUtl::move(VA6)));

        TEST_EMPLACE((MovUtl::move(VA1), VA2,
                    MovUtl::move(VA3), VA4,
                    MovUtl::move(VA5), VA6,
                    MovUtl::move(VA7)));
        TEST_EMPLACE((VA1, MovUtl::move(VA2),
                    VA3, MovUtl::move(VA4),
                    VA5, MovUtl::move(VA6),
                    VA7));

        TEST_EMPLACE((MovUtl::move(VA1), VA2,
                    MovUtl::move(VA3), VA4,
                    MovUtl::move(VA5), VA6,
                    MovUtl::move(VA7), VA8));
        TEST_EMPLACE((VA1, MovUtl::move(VA2),
                    VA3, MovUtl::move(VA4),
                    VA5, MovUtl::move(VA6),VA6
                    VA7, MovUtl::move(VA8)));

        TEST_EMPLACE((MovUtl::move(VA1), VA2,
                    MovUtl::move(VA3), VA4,
                    MovUtl::move(VA5), VA6,
                    MovUtl::move(VA7), VA8,
                    MovUtl::move(VA9)));
        TEST_EMPLACE((VA1, MovUtl::move(VA2),
                    VA3, MovUtl::move(VA4),
                    VA5, MovUtl::move(VA6),
                    VA7, MovUtl::move(VA8),
                    VA9));

        TEST_EMPLACE((MovUtl::move(VA1), VA2,
                    MovUtl::move(VA3), VA4,
                    MovUtl::move(VA5), VA6,
                    MovUtl::move(VA7), VA8,
                    MovUtl::move(VA9), VA10));
        TEST_EMPLACE((VA1, MovUtl::move(VA2),
                    VA3, MovUtl::move(VA4),
                    VA5, MovUtl::move(VA6),
                    VA7, MovUtl::move(VA8),
                    VA9, MovUtl::move(VA10)));

        TEST_EMPLACE((MovUtl::move(VA1), VA2,
                    MovUtl::move(VA3), VA4,
                    MovUtl::move(VA5), VA6,
                    MovUtl::move(VA7), VA8,
                    MovUtl::move(VA9), VA10,
                    MovUtl::move(VA11)));
        TEST_EMPLACE((VA1, MovUtl::move(VA2),
                    VA3, MovUtl::move(VA4),
                    VA5, MovUtl::move(VA6),
                    VA7, MovUtl::move(VA8),
                    VA9, MovUtl::move(VA10),
                    VA11));

        TEST_EMPLACE((MovUtl::move(VA1), VA2,
                    MovUtl::move(VA3), VA4,
                    MovUtl::move(VA5), VA6,
                    MovUtl::move(VA7), VA8,
                    MovUtl::move(VA9), VA10,
                    MovUtl::move(VA11), VA12));
        TEST_EMPLACE((VA1, MovUtl::move(VA2),
                    VA3, MovUtl::move(VA4),
                    VA5, MovUtl::move(VA6),
                    VA7, MovUtl::move(VA8),
                    VA9, MovUtl::move(VA10),
                    VA11, MovUtl::move(VA12)));

        TEST_EMPLACE((MovUtl::move(VA1), VA2,
                    MovUtl::move(VA3), VA4,
                    MovUtl::move(VA5), VA6,
                    MovUtl::move(VA7), VA8,
                    MovUtl::move(VA9), VA10,
                    MovUtl::move(VA11), VA12,
                    MovUtl::move(VA13)));
        TEST_EMPLACE((VA1, MovUtl::move(VA2),
                    VA3, MovUtl::move(VA4),
                    VA5, MovUtl::move(VA6),
                    VA7, MovUtl::move(VA8),
                    VA9, MovUtl::move(VA10),
                    VA11, MovUtl::move(VA12),
                    VA13));

        TEST_EMPLACE_ARGT((bsl::allocator_arg, &oa),
                          ());

        TEST_EMPLACE_ARGT((bsl::allocator_arg, &oa,MovUtl::move(VA1)),
                          (MovUtl::move(VA1)));
        TEST_EMPLACE_ARGT((bsl::allocator_arg, &oa,VA1),
                          (VA1));

        TEST_EMPLACE_ARGT((bsl::allocator_arg, &oa,
                           MovUtl::move(VA1), VA2),
                          (MovUtl::move(VA1), VA2));
        TEST_EMPLACE_ARGT((bsl::allocator_arg, &oa,
                           VA1, MovUtl::move(VA2)),
                          (VA1, MovUtl::move(VA2)));

        TEST_EMPLACE_ARGT((bsl::allocator_arg, &oa,
                           MovUtl::move(VA1), VA2, MovUtl::move(VA3)),
                          (MovUtl::move(VA1), VA2, MovUtl::move(VA3)));
        TEST_EMPLACE_ARGT((bsl::allocator_arg, &oa,
                           VA1, MovUtl::move(VA2), VA3),
                          (VA1, MovUtl::move(VA2), VA3));

        TEST_EMPLACE_ARGT((bsl::allocator_arg, &oa,
                           MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4),
                          (MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4));
        TEST_EMPLACE_ARGT((bsl::allocator_arg, &oa,
                           VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4)),
                          (VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4)));

        TEST_EMPLACE_ARGT((bsl::allocator_arg, &oa,
                           MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                           MovUtl::move(VA5)),
                          (MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                           MovUtl::move(VA5)));
        TEST_EMPLACE_ARGT((bsl::allocator_arg, &oa,
                           VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                           VA5),
                          (VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                           VA5));

        TEST_EMPLACE_ARGT((bsl::allocator_arg, &oa,
                           MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                           MovUtl::move(VA5), VA6),
                          (MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                           MovUtl::move(VA5), VA6));
        TEST_EMPLACE_ARGT((bsl::allocator_arg, &oa,
                           VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                           VA5, MovUtl::move(VA6)),
                          (VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                           VA5, MovUtl::move(VA6)));

        TEST_EMPLACE_ARGT((bsl::allocator_arg, &oa,
                           MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                           MovUtl::move(VA5), VA6, MovUtl::move(VA7)),
                          (MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                           MovUtl::move(VA5), VA6, MovUtl::move(VA7)));
        TEST_EMPLACE_ARGT((bsl::allocator_arg, &oa,
                           VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                           VA5, MovUtl::move(VA6), VA7),
                          (VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                           VA5, MovUtl::move(VA6), VA7));

        TEST_EMPLACE_ARGT((bsl::allocator_arg, &oa,
                           MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                           MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8),
                          (MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                           MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8));
        TEST_EMPLACE_ARGT((bsl::allocator_arg, &oa,
                           VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                           VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8)),
                          (VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                           VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8)));

        TEST_EMPLACE_ARGT((bsl::allocator_arg, &oa,
                           MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                           MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                           MovUtl::move(VA9)),
                          (MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                           MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                           MovUtl::move(VA9)));
        TEST_EMPLACE_ARGT((bsl::allocator_arg, &oa,
                           VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                           VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                           VA9),
                          (VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                           VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                           VA9));

        TEST_EMPLACE_ARGT((bsl::allocator_arg, &oa,
                           MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                           MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                           MovUtl::move(VA9), VA10),
                          (MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                           MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                           MovUtl::move(VA9), VA10));
        TEST_EMPLACE_ARGT((bsl::allocator_arg, &oa,
                            VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                            VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                            VA9, MovUtl::move(VA10)),
                           (VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                            VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                            VA9, MovUtl::move(VA10)));


        TEST_EMPLACE_ARGT((bsl::allocator_arg, &oa,
                           MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                           MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                           MovUtl::move(VA9), VA10, MovUtl::move(VA11)),
                          (MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                           MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                           MovUtl::move(VA9), VA10, MovUtl::move(VA11)));
        TEST_EMPLACE_ARGT((bsl::allocator_arg, &oa,
                           VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                           VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                           VA9, MovUtl::move(VA10), VA11),
                          (VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                           VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                           VA9, MovUtl::move(VA10), VA11));

        TEST_EMPLACE_ARGT((bsl::allocator_arg, &oa,
                           MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                           MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                           MovUtl::move(VA9), VA10, MovUtl::move(VA11), VA12),
                          (MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                           MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                           MovUtl::move(VA9), VA10, MovUtl::move(VA11), VA12));

        TEST_EMPLACE_ARGT((bsl::allocator_arg, &oa,
                           VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                           VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                           VA9, MovUtl::move(VA10), VA11, MovUtl::move(VA12)),
                          (VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                           VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                           VA9, MovUtl::move(VA10), VA11, MovUtl::move(VA12)));

        TEST_EMPLACE_ARGT((bsl::allocator_arg, &oa,
                           MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                           MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                           MovUtl::move(VA9), VA10, MovUtl::move(VA11), VA12,
                           MovUtl::move(VA13)),
                          (MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                           MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                           MovUtl::move(VA9), VA10, MovUtl::move(VA11), VA12,
                           MovUtl::move(VA13)));
        TEST_EMPLACE_ARGT((bsl::allocator_arg, &oa,
                           VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                           VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                           VA9, MovUtl::move(VA10), VA11, MovUtl::move(VA12),
                           VA13),
                          (VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                           VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                           VA9, MovUtl::move(VA10), VA11, MovUtl::move(VA12),
                           VA13));
*/
    }
#ifdef BSLSTL_OPTIONAL_TEST_BAD__EMPLACE
    if (veryVerbose) printf("\tUsing 'ConstructTestTypeNoAlloc'.\n");
    {
        typedef ConstructTestTypeNoAlloc                  ValueType;
        typedef const ConstructTestTypeNoAlloc            ConstValueType;
        typedef const bsl::optional<ValueType> CObj;
        typedef const bsl::optional<ConstValueType> CObjC;
       {
            CObj bad1;
            bad1.emplace();  // this should not compile 1
            bad1.emplace( VA1); // this should not compile 2
            bad1.emplace( VA1, VA2); // this should not compile 3
            bad1.emplace( VA1, VA2, VA3); // this should not compile 4
            bad1.emplace( VA1, VA2, VA3, VA4);
                  // this should not compile 5

            CObjC bad2;
            bad2.emplace();  // this should not compile 6
            bad2.emplace( VA1); // this should not compile 7
            bad2.emplace( VA1, VA2); // this should not compile 8
            bad2.emplace( VA1, VA2, VA3); // this should not compile 9
            bad2.emplace( VA1, VA2, VA3, VA4);
                  // this should not compile 10

        }
    }
    if (veryVerbose) printf("\tUsing 'ConstructTestTypeAlloc'.\n");
    {
        typedef ConstructTestTypeAlloc                  ValueType;
        typedef const ConstructTestTypeAlloc            ConstValueType;
        typedef const bsl::optional<ValueType>          CObj;
        typedef const bsl::optional<ConstValueType>     CObjC;
        {
            CObj bad1;
            bad1.emplace();  // this should not compile 11
            bad1.emplace(VA1); // this should not compile 12
            bad1.emplace(VA1, VA2); // this should not compile 13
            bad1.emplace(VA1, VA2, VA3); // this should not compile 14
            bad1.emplace(VA1, VA2, VA3, VA4); // this should not compile 15

            CObjC bad2;
            bad2.emplace();  // this should not compile 16
            bad2.emplace(VA1); // this should not compile 17
            bad2.emplace(VA1, VA2); // this should not compile 18
            bad2.emplace(VA1, VA2, VA3); // this should not compile 19
            bad2.emplace(VA1, VA2, VA3, VA4); // this should not compile 20

        }
    }
#endif

}
void bslstl_optional_test11()
{
#ifdef BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS
    // --------------------------------------------------------------------
    // TESTING TESTING INITIALIZER LIST 'emplace' METHOD
    //   This test will verify that the initializer list 'emplace' method works
    //   as expected.
    //
    // Concerns:
    //: 1 Calling 'emplace' with only an initializer list detects the
    //:   initializer list.
    //: 2 Variadic arguments to 'emplace' method are correctly fowarded to the
    //:   constructor arguments.
    //: 3 If 'value_type' is allocator aware, 'emplace' invokes the allocator
    //:   extended constructor using the 'optional''s allocator.
    //: 4 There are no unnecessary argument type and 'value_type' copies
    //:   created
    //
    // Plan:
    //: 1 Create an 'optional' object of non allocator aware 'value_type'.
    //:   For concern 1, call 'emplace' method that takes just an initializer
    //:   list, and verify the object was constructed using an initializer list
    //:   constructor.
    //: 2 For concern 2, repeat steps 1 using varying number of arguments.
    //: 3 For concern 3, repeat steps 1-2 with an allocator aware 'value_type'
    //:   and verify the correct allocator is used
    //: 4 For concern 4, in stesp 1-3, verify no unnecessary copies of the
    //:   arguments and the 'value_type' have been created.
    //
    // Testing:
    //
    //   void emplace(std::initializer_list<U>, Args&&...);
    //
    //   void value();
    //
    // --------------------------------------------------------------------

    if (verbose) printf(
                       "\nTESTING INITIALIZER LIST 'emplace' METHOD"
                       "\n=========================================\n");
    if (veryVerbose) printf("\tUsing non allocator aware 'value_type'.\n");
    {
        TEST_EMPLACE(({1,2,3}));

        TEST_EMPLACE(({1,2,3}, VA1));
        TEST_EMPLACE(({1,2,3}, MovUtl::move(VA1)));

        TEST_EMPLACE(({1,2,3}, MovUtl::move(VA1), VA2));
        TEST_EMPLACE(({1,2,3}, VA1, MovUtl::move(VA2)));

        TEST_EMPLACE(({1,2,3}, MovUtl::move(VA1), VA2, VA3));
        TEST_EMPLACE(({1,2,3}, VA1, MovUtl::move(VA2), VA3));
        TEST_EMPLACE(({1,2,3}, VA1, VA2, MovUtl::move(VA3)));

        TEST_EMPLACE(({1,2,3}, MovUtl::move(VA1), VA2,
                    MovUtl::move(VA3), VA4));
        TEST_EMPLACE(({1,2,3}, VA1, MovUtl::move(VA2),
                    VA3, MovUtl::move(VA4)));

        TEST_EMPLACE(({1,2,3}, MovUtl::move(VA1), VA2,
                    MovUtl::move(VA3), VA4,
                    MovUtl::move(VA5)));
        TEST_EMPLACE(({1,2,3}, VA1, MovUtl::move(VA2),
                    VA3, MovUtl::move(VA4),
                    VA5));
/*
        TEST_EMPLACE(({1,2,3}, MovUtl::move(VA1), VA2,
                    MovUtl::move(VA3), VA4,
                    MovUtl::move(VA5), VA6));
        TEST_EMPLACE(({1,2,3}, VA1, MovUtl::move(VA2),
                    VA3, MovUtl::move(VA4),
                    VA5, MovUtl::move(VA6)));

        TEST_EMPLACE(({1,2,3}, MovUtl::move(VA1), VA2,
                    MovUtl::move(VA3), VA4,
                    MovUtl::move(VA5), VA6,
                    MovUtl::move(VA7)));
        TEST_EMPLACE(({1,2,3}, VA1, MovUtl::move(VA2),
                    VA3, MovUtl::move(VA4),
                    VA5, MovUtl::move(VA6),
                    VA7));

        TEST_EMPLACE(({1,2,3}, MovUtl::move(VA1), VA2,
                    MovUtl::move(VA3), VA4,
                    MovUtl::move(VA5), VA6,
                    MovUtl::move(VA7), VA8));
        TEST_EMPLACE(({1,2,3}, VA1, MovUtl::move(VA2),
                    VA3, MovUtl::move(VA4),
                    VA5, MovUtl::move(VA6),
                    VA7, MovUtl::move(VA8)));

        TEST_EMPLACE(({1,2,3}, MovUtl::move(VA1), VA2,
                    MovUtl::move(VA3), VA4,
                    MovUtl::move(VA5), VA6,
                    MovUtl::move(VA7), VA8,
                    MovUtl::move(VA9)));
        TEST_EMPLACE(({1,2,3}, VA1, MovUtl::move(VA2),
                    VA3, MovUtl::move(VA4),
                    VA5, MovUtl::move(VA6),
                    VA7, MovUtl::move(VA8),
                    VA9));

        TEST_EMPLACE(({1,2,3}, MovUtl::move(VA1), VA2,
                    MovUtl::move(VA3), VA4,
                    MovUtl::move(VA5), VA6,
                    MovUtl::move(VA7), VA8,
                    MovUtl::move(VA9), VA10));
        TEST_EMPLACE(({1,2,3}, VA1, MovUtl::move(VA2),
                    VA3, MovUtl::move(VA4),
                    VA5, MovUtl::move(VA6),
                    VA7, MovUtl::move(VA8),
                    VA9, MovUtl::move(VA10)));

        TEST_EMPLACE(({1,2,3}, MovUtl::move(VA1), VA2,
                    MovUtl::move(VA3), VA4,
                    MovUtl::move(VA5), VA6,
                    MovUtl::move(VA7), VA8,
                    MovUtl::move(VA9), VA10,
                    MovUtl::move(VA11)));
        TEST_EMPLACE(({1,2,3}, VA1, MovUtl::move(VA2),
                    VA3, MovUtl::move(VA4),
                    VA5, MovUtl::move(VA6),
                    VA7, MovUtl::move(VA8),
                    VA9, MovUtl::move(VA10),
                    VA11));

        TEST_EMPLACE(({1,2,3}, MovUtl::move(VA1), VA2,
                    MovUtl::move(VA3), VA4,
                    MovUtl::move(VA5), VA6,
                    MovUtl::move(VA7), VA8,
                    MovUtl::move(VA9), VA10,
                    MovUtl::move(VA11), VA12));
        TEST_EMPLACE(({1,2,3}, VA1, MovUtl::move(VA2),
                    VA3, MovUtl::move(VA4),
                    VA5, MovUtl::move(VA6),
                    VA7, MovUtl::move(VA8),
                    VA9, MovUtl::move(VA10),
                    VA11, MovUtl::move(VA12)));

        TEST_EMPLACE(({1,2,3}, MovUtl::move(VA1), VA2,
                    MovUtl::move(VA3), VA4,
                    MovUtl::move(VA5), VA6,
                    MovUtl::move(VA7), VA8,
                    MovUtl::move(VA9), VA10,
                    MovUtl::move(VA11), VA12,
                    MovUtl::move(VA13)));
        TEST_EMPLACE(({1,2,3}, VA1, MovUtl::move(VA2),
                    VA3, MovUtl::move(VA4),
                    VA5, MovUtl::move(VA6),
                    VA7, MovUtl::move(VA8),
                    VA9, MovUtl::move(VA10),
                    VA11, MovUtl::move(VA12),
                    VA13));
*/
    }
    if (veryVerbose) printf("\tUsing allocator aware 'value_type'.\n");
    {
        TEST_EMPLACE_ARGTIL((bsl::allocator_arg, &oa, {1,2,3},
                             MovUtl::move(VA1), VA2, MovUtl::move(VA3)),
                            ({1,2,3},
                             MovUtl::move(VA1), VA2, MovUtl::move(VA3)));
        TEST_EMPLACE_ARGTIL((bsl::allocator_arg, &oa, {1,2,3},
                             VA1, MovUtl::move(VA2), VA3),
                            ({1,2,3},
                             VA1, MovUtl::move(VA2), VA3));

        TEST_EMPLACE_ARGTIL((bsl::allocator_arg, &oa, {1,2,3}),
                             ({1,2,3}));

        TEST_EMPLACE_ARGTIL((bsl::allocator_arg, &oa, {1,2,3},
                              MovUtl::move(VA1)),
                             ({1,2,3},
                              MovUtl::move(VA1)));
        TEST_EMPLACE_ARGTIL((bsl::allocator_arg, &oa, {1,2,3},
                             VA1),
                            ({1,2,3},
                             VA1));

        TEST_EMPLACE_ARGTIL((bsl::allocator_arg, &oa, {1,2,3},
                             MovUtl::move(VA1), VA2),
                            ({1,2,3},
                             MovUtl::move(VA1), VA2));
        TEST_EMPLACE_ARGTIL((bsl::allocator_arg, &oa, {1,2,3},
                             VA1, MovUtl::move(VA2)),
                            ({1,2,3},
                             VA1, MovUtl::move(VA2)));

        TEST_EMPLACE_ARGTIL((bsl::allocator_arg, &oa, {1,2,3},
                             MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4),
                            ({1,2,3},
                             MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4));
        TEST_EMPLACE_ARGTIL((bsl::allocator_arg, &oa, {1,2,3},
                            VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4)),
                           ({1,2,3},
                            VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4)));

        TEST_EMPLACE_ARGTIL((bsl::allocator_arg, &oa, {1,2,3},
                             MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                             MovUtl::move(VA5)),
                            ({1,2,3},
                             MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                             MovUtl::move(VA5)));
        TEST_EMPLACE_ARGTIL((bsl::allocator_arg, &oa, {1,2,3},
                            VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                            VA5),
                           ({1,2,3},
                            VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                            VA5));
/*
        TEST_EMPLACE_ARGTIL((bsl::allocator_arg, &oa, {1,2,3},
                             MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                             MovUtl::move(VA5), VA6),
                            ({1,2,3},
                             MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                             MovUtl::move(VA5), VA6));
        TEST_EMPLACE_ARGTIL((bsl::allocator_arg, &oa, {1,2,3},
                            VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                            VA5, MovUtl::move(VA6)),
                           ({1,2,3},
                            VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                            VA5, MovUtl::move(VA6)));

        TEST_EMPLACE_ARGTIL((bsl::allocator_arg, &oa, {1,2,3},
                           MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                           MovUtl::move(VA5), VA6, MovUtl::move(VA7)),
                          ({1,2,3},
                           MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                           MovUtl::move(VA5), VA6, MovUtl::move(VA7)));
        TEST_EMPLACE_ARGTIL((bsl::allocator_arg, &oa, {1,2,3},
                            VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                            VA5, MovUtl::move(VA6), VA7),
                           ({1,2,3},
                            VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                            VA5, MovUtl::move(VA6), VA7));

        TEST_EMPLACE_ARGTIL((bsl::allocator_arg, &oa, {1,2,3},
                             MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                             MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8),
                            ({1,2,3},
                             MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                             MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8));
        TEST_EMPLACE_ARGTIL((bsl::allocator_arg, &oa, {1,2,3},
                            VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                            VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8)),
                           ({1,2,3},
                            VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                            VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8)));

        TEST_EMPLACE_ARGTIL((bsl::allocator_arg, &oa, {1,2,3},
                             MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                             MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                             MovUtl::move(VA9)),
                            ({1,2,3},
                             MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                             MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                             MovUtl::move(VA9)));
        TEST_EMPLACE_ARGTIL((bsl::allocator_arg, &oa, {1,2,3},
                            VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                            VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                            VA9),
                           ({1,2,3},
                            VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                            VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                            VA9));

        TEST_EMPLACE_ARGTIL((bsl::allocator_arg, &oa, {1,2,3},
                             MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                             MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                             MovUtl::move(VA9), VA10),
                            ({1,2,3},
                             MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                             MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                             MovUtl::move(VA9), VA10));
        TEST_EMPLACE_ARGTIL((bsl::allocator_arg, &oa, {1,2,3},
                            VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                            VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                            VA9, MovUtl::move(VA10)),
                           ({1,2,3},
                            VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                            VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                            VA9, MovUtl::move(VA10)));


        TEST_EMPLACE_ARGTIL((bsl::allocator_arg, &oa, {1,2,3},
                             MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                             MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                             MovUtl::move(VA9), VA10, MovUtl::move(VA11)),
                            ({1,2,3},
                             MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                             MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                             MovUtl::move(VA9), VA10, MovUtl::move(VA11)));
        TEST_EMPLACE_ARGTIL((bsl::allocator_arg, &oa, {1,2,3},
                            VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                            VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                            VA9, MovUtl::move(VA10), VA11),
                           ({1,2,3},
                            VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                            VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                            VA9, MovUtl::move(VA10), VA11));

        TEST_EMPLACE_ARGTIL((bsl::allocator_arg, &oa, {1,2,3},
                           MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                           MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                           MovUtl::move(VA9), VA10, MovUtl::move(VA11), VA12),
                          ({1,2,3},
                           MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                           MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                           MovUtl::move(VA9), VA10, MovUtl::move(VA11), VA12));
        TEST_EMPLACE_ARGTIL((bsl::allocator_arg, &oa, {1,2,3},
                           VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                           VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                           VA9, MovUtl::move(VA10), VA11, MovUtl::move(VA12)),
                          ({1,2,3},
                           VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                           VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                           VA9, MovUtl::move(VA10), VA11, MovUtl::move(VA12)));

        TEST_EMPLACE_ARGTIL((bsl::allocator_arg, &oa, {1,2,3},
                             MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                             MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                             MovUtl::move(VA9), VA10, MovUtl::move(VA11), VA12,
                             MovUtl::move(VA13)),
                            ({1,2,3},
                             MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                             MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                             MovUtl::move(VA9), VA10, MovUtl::move(VA11), VA12,
                             MovUtl::move(VA13)));
        TEST_EMPLACE_ARGTIL((bsl::allocator_arg, &oa, {1,2,3},
                            VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                            VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                            VA9, MovUtl::move(VA10), VA11, MovUtl::move(VA12),
                            VA13),
                           ({1,2,3},
                            VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                            VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                            VA9, MovUtl::move(VA10), VA11, MovUtl::move(VA12),
                            VA13));
*/
    }
#endif // BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS
}
void bslstl_optional_test12()
{
  // --------------------------------------------------------------------
  // TESTING operator(nullopt_t) MEMBER FUNCTION
  //   This test will verify that the operator=(nullopt_t) member function
  //   works as expected.
  //
  // Concerns:
  //: 1 Calling 'operator=(nullopt_t)' on a disengaged 'optional' leaves the
  //:   'optional' disengaged.
  //: 2 Calling 'operator=(nullopt_t)' on an engaged 'optional' makes the
  //:   'optional' disengaged.
  //: 3 For an allocator aware 'value_type', calling 'operator=(nullopt_t)'
  //:   does not modify the allocator.
  //: 4 'operator=(nullopt_t)' can be called on a non const qualified
  //:   'optional' of a const qualified 'value_type'.
  //
  //
  // Plan:
  //: 1 Create a disengaged 'optional' of a non allocator aware 'value_type'.
  //:   Call operator=(nullopt_t) and, for concern 1, verify that the 'ptional'
  //:   object is still disengaged
  //: 2 Emplace a value in the 'optional'. Call operator=(nullopt_t) and, for
  //:   concern 2, check that 'optional' is disengaged'
  //: 3 Repeat steps 1 and 2 with an allocator aware 'value_type', and for
  //:   concern 3, verify the allocator has not changed.
  //: 4 For concern 4, repeat steps 1 - 3, using a const qualified value
  //:   type.
  //
  // Testing:
  //
  //   operator=(nullopt_t)
  //
  // --------------------------------------------------------------------

    if (verbose) printf(
                       "\nTESTING operator(nullopt_t) MEMBER FUNCTION "
                       "\n===========================================\n");

    if (veryVerbose) printf("\tUsing 'int'.\n");
    {
        typedef int                  ValueType;
        typedef const int            ConstValueType;
        typedef bsl::optional<ValueType> Obj;
        typedef bsl::optional<ConstValueType> ObjC;
        {
          Obj mX;
          mX = bsl::nullopt;
          ASSERT(!mX.has_value());

          mX.emplace(3);
          mX = bsl::nullopt;
          ASSERT(!mX.has_value());

          ObjC mcX;
          mcX = bsl::nullopt;
          ASSERT(!mcX.has_value());

          mcX.emplace(3);
          mcX = bsl::nullopt;
          ASSERT(!mcX.has_value());
       }
    }
    if (veryVerbose) printf("\tUsing 'MyClass2'.\n");
    {
        typedef MyClass2                 ValueType;
        typedef const MyClass2           ConstValueType;
        typedef bsl::optional<ValueType> Obj;
        typedef bsl::optional<ConstValueType> ObjC;
        {
          Obj mX;
          mX = bsl::nullopt;
          ASSERT(!mX.has_value());

          mX.emplace(V2);
          mX = bsl::nullopt;
          ASSERT(!mX.has_value());

          ObjC mcX;
          mcX = bsl::nullopt;
          ASSERT(!mcX.has_value());

          mcX.emplace(5);
          mcX = bsl::nullopt;
          ASSERT(!mcX.has_value());
       }
    }
}

void bslstl_optional_test13()
{
    // --------------------------------------------------------------------
    // TESTING 'operator=(non_optional_type)' FUNCTIONALITY
    //   This test will verify that the 'operator=(rhs)' member function, where
    //   'rhs' is not an 'optional' type, works as expected.
    //
    // Concerns:
    //: 1 Calling 'operator=(rhs)', where 'rhs' is not of an 'optional' type,
    //:   or an engaged 'optional' assigns 'rhs' to the 'value_type' object.
    //: 2 Calling 'operator=(rhs)', where 'rhs' is not of an 'optional' type,
    //:   or a disengaged 'optional' creates a 'value_type' object initialized
    //:   with 'rhs'.
    //: 3 For allocator aware types, the assignment to a disengaged 'optional'
    //:   uses the stored allocator.
    //: 4 Assignment of rvalues uses move assignment/construction where
    //:   available.
    //: 5 'optional' object of const qualified 'value_type' can not be assigned
    //:   to.
    //
    // Plan:
    //: 1 Create an engaged 'optional' object of non allocator aware type.
    //:   For concern 1, check that assignment from 'value_type', from const
    //:   qualified 'value_type', and from type assignable to 'value_type'
    //:   results in an 'optional' object having the value (possible converted
    //:   from) rhs.
    //: 2 Repeat step 1 using rvalue 'rhs' and, for concern 4, check 'rhs' was
    //:   moved from.
    //: 3 Create a disengaged 'optional' object of non allocator aware type.
    //:   For concern 2, check that assignment from 'value_type', from 'const'
    //:   qualified 'value_type', and from type assignable to 'value_type'
    //:   results in an 'optional' object having the value (possible converted
    //:   from) rhs.
    //: 4 Repeat step 4 using rvalue 'rhs' and, for concern 4, check 'rhs' was
    //:   moved from.
    //: 5 Repeat steps 1-4 using an 'optional' object of allocator aware type.
    //:   For concern 3, check the allocator of the resulting 'optional'
    //:   object.
    //: 6 For concern 7, verify that an 'optional' object of const qualified
    //:   'value_type' can not be assigned to. Note that this test requires
    //:   compilation errors and needs to be enabled and checked manually.
    //
    // Testing:
    //
    //   operator=(BSLS_COMPILERFEATURES_FORWARD_REF(ANY_TYPE) other)
    //
    // --------------------------------------------------------------------

    if (verbose) printf(
                    "\nTESTING 'operator=(non_optional_type)' FUNCTIONALITY"
                    "\n===================================================="
                    "\n");

    if (veryVerbose) printf("\tUsing non allocator aware 'value_type'.\n");
    {
        typedef MyClass1a                  ValueType;
        typedef const ValueType            ConstValueType;
        typedef bsl::optional<ValueType> Obj;
        typedef bsl::optional<ConstValueType> ObjC;
        if (veryVeryVerbose) printf("\t\tChecking assignment to an engaged "
                            "'optional'.\n");
        {
            Obj mX = ValueType(0);
            ASSERT(mX.has_value());
            ValueType vi = ValueType(1);
            mX = vi;
            ASSERT(mX.value().value() == 1);
            ASSERT(vi.value() == 1);

            MyClass1 i = MyClass1(3);
            ASSERT(mX.has_value());
            mX = i;
            ASSERT(mX.value().value() == 3);
            ASSERT( i.value() == 3);

            ConstValueType cvi = ValueType(1);
            ASSERT(mX.has_value());
            mX = cvi;
            ASSERT(mX.value().value() == 1);

            const MyClass1 ci = MyClass1(3);
            ASSERT(mX.has_value());
            mX = ci;
            ASSERT(mX.value().value() == 3);

            ASSERT(mX.has_value());
            mX = MovUtl::move(vi);
            ASSERT(mX.value().value() == 1);
            ASSERT(vi.value() == MOVED_FROM_VAL);

            ASSERT(mX.has_value());
            mX = MovUtl::move(i);
            ASSERT(mX.value().value() == 3);
            ASSERT(i.value() == MOVED_FROM_VAL);

            ASSERT(mX.has_value());
            mX = MovUtl::move(cvi);
            ASSERT(mX.value().value() == 1);
            ASSERT(cvi.value() == 1);

            ASSERT(mX.has_value());
            mX = MovUtl::move(ci);
            ASSERT(mX.value().value() == 3);
            ASSERT(ci.value() == 3);
        }
        if (veryVeryVerbose) printf("\t\tChecking assignment to a disengaged"
                                    " 'optional'.\n");
        {
            Obj mX;
            ValueType vi = ValueType(1);
            ASSERT(!mX.has_value());
            mX = vi;
            ASSERT(mX.value().value() == 1);
            ASSERT(vi.value() == 1);

            mX.reset();
            ASSERT(!mX.has_value());
            MyClass1 i = MyClass1(3);
            mX = i;
            ASSERT(mX.value().value() == 3);
            ASSERT( i.value() == 3);

            ConstValueType cvi = ValueType(1);
            mX.reset();
            ASSERT(!mX.has_value());
            mX = cvi;
            ASSERT(mX.value().value() == 1);

            const MyClass1 ci = MyClass1(3);
            mX.reset();
            ASSERT(!mX.has_value());
            mX = ci;
            ASSERT(mX.value().value() == 3);

            mX.reset();
            ASSERT(!mX.has_value());
            mX = MovUtl::move(vi);
            ASSERT(mX.value().value() == 1);
            ASSERT(vi.value() == MOVED_FROM_VAL);

            mX.reset();
            ASSERT(!mX.has_value());
            mX = MovUtl::move(i);
            ASSERT(mX.value().value() == 3);
            ASSERT(i.value() == MOVED_FROM_VAL);

            mX.reset();
            ASSERT(!mX.has_value());
            mX = MovUtl::move(cvi);
            ASSERT(mX.value().value() == 1);
            ASSERT(cvi.value() == 1);

            mX.reset();
            ASSERT(!mX.has_value());
            mX = MovUtl::move(ci);
            ASSERT(mX.value().value() == 3);
            ASSERT(ci.value() == 3);
        }
        {
            ObjC mX = ValueType(0);
            ValueType vi = ValueType(1);
  #ifdef BSLSTL_OPTIONAL_TEST_BAD_EQUAL_CONST
            mX = vi; // this should not compile
  #endif //BSLSTL_OPTIONAL_TEST_BAD_EQUAL_CONST
        }
    }
    if (veryVerbose) printf("\tUsing allocator aware 'value_type'.\n");
    {
        bslma::TestAllocator da("default", veryVeryVeryVerbose);
        bslma::TestAllocator oa("other", veryVeryVeryVerbose);
        bslma::TestAllocator ta("third", veryVeryVeryVerbose);

        bslma::DefaultAllocatorGuard dag(&da);

        typedef MyClass2a                  ValueType;
        typedef const ValueType            ConstValueType;
        typedef bsl::optional<ValueType> Obj;
        typedef bsl::optional<ConstValueType> ObjC;
        if (veryVeryVerbose) printf("\t\tChecking assignment to an engaged "
                                    "'optional'.\n");
       {
            Obj mX(bsl::allocator_arg, &oa, ValueType(0));
            ASSERT(mX.has_value());
            ASSERT(mX.value().d_data.d_def.d_allocator_p == &oa);
            ASSERT(mX.get_allocator() == &oa);
            ValueType vi = ValueType(bsl::allocator_arg, &ta, 1);
            mX = vi;
            ASSERT(mX.value().value() == 1);
            ASSERT(mX.value().d_data.d_def.d_allocator_p == &oa);
            ASSERT(vi.value() == 1);
            ASSERT(vi.d_data.d_def.d_allocator_p == &ta);

            MyClass2 i = MyClass2(3, &da);
            ASSERT(mX.has_value());
            mX = i;
            ASSERT(mX.value().value() == 3);
            ASSERT(mX.value().d_data.d_def.d_allocator_p == &oa);
            ASSERT(mX.get_allocator() == &oa);
            ASSERT( i.value() == 3);

            ConstValueType cvi = ValueType(bsl::allocator_arg, &ta, 1);
            ASSERT(mX.has_value());
            mX = cvi;
            ASSERT(mX.value().value() == 1);
            ASSERT(mX.value().d_data.d_def.d_allocator_p == &oa);
            ASSERT(mX.get_allocator() == &oa);

            const MyClass2 ci = MyClass2(3, &da);
            ASSERT(mX.has_value());
            mX = ci;
            ASSERT(mX.value().value() == 3);
            ASSERT(mX.value().d_data.d_def.d_allocator_p == &oa);
            ASSERT(mX.get_allocator() == &oa);

            ASSERT(mX.has_value());
            mX = MovUtl::move(vi);
            ASSERT(mX.value().value() == 1);
            ASSERT(mX.value().d_data.d_def.d_allocator_p == &oa);
            ASSERT(mX.get_allocator() == &oa);
            ASSERT(vi.value() == MOVED_FROM_VAL);

            ASSERT(mX.has_value());
            mX = MovUtl::move(i);
            ASSERT(mX.value().value() == 3);
            ASSERT(mX.value().d_data.d_def.d_allocator_p == &oa);
            ASSERT(mX.get_allocator() == &oa);
            ASSERT(i.value() == MOVED_FROM_VAL);

            ASSERT(mX.has_value());
            mX = MovUtl::move(cvi);
            ASSERT(mX.value().value() == 1);
            ASSERT(mX.value().d_data.d_def.d_allocator_p == &oa);
            ASSERT(mX.get_allocator() == &oa);
            ASSERT(cvi.value() == 1);

            ASSERT(mX.has_value());
            mX = MovUtl::move(ci);
            ASSERT(mX.value().value() == 3);
            ASSERT(mX.value().d_data.d_def.d_allocator_p == &oa);
            ASSERT(mX.get_allocator() == &oa);
            ASSERT(ci.value() == 3);
        }
        if (veryVeryVerbose) printf("\t\tChecking assignment to a disengaged"
                                    " 'optional'.\n");
        {
            Obj mX(bsl::allocator_arg, &oa);
            ASSERT(mX.get_allocator() == &oa);
            ValueType vi = ValueType(bsl::allocator_arg, &ta, 1);
            ASSERT(!mX.has_value());
            mX = vi;
            ASSERT(mX.value().value() == 1);
            ASSERT(mX.value().d_data.d_def.d_allocator_p == &oa);
            ASSERT(mX.get_allocator() == &oa);
            ASSERT(vi.value() == 1);
            ASSERT(vi.d_data.d_def.d_allocator_p == &ta);

            mX.reset();
            ASSERT(!mX.has_value());
            MyClass2 i = MyClass2(3, &da);
            mX = i;
            ASSERT(mX.value().value() == 3);
            ASSERT(mX.value().d_data.d_def.d_allocator_p == &oa);
            ASSERT(mX.get_allocator() == &oa);
            ASSERT( i.value() == 3);

            ConstValueType cvi = ValueType(bsl::allocator_arg, &ta, 1);
            mX.reset();
            ASSERT(!mX.has_value());
            mX = cvi;
            ASSERT(mX.value().value() == 1);
            ASSERT(mX.value().d_data.d_def.d_allocator_p == &oa);
            ASSERT(mX.get_allocator() == &oa);

            const MyClass2 ci = MyClass2(3, &da);
            mX.reset();
            ASSERT(!mX.has_value());
            mX = ci;
            ASSERT(mX.value().value() == 3);
            ASSERT(mX.value().d_data.d_def.d_allocator_p == &oa);
            ASSERT(mX.get_allocator() == &oa);

            mX.reset();
            ASSERT(!mX.has_value());
            mX = MovUtl::move(vi);
            ASSERT(mX.value().value() == 1);
            ASSERT(mX.value().d_data.d_def.d_allocator_p == &oa);
            ASSERT(mX.get_allocator() == &oa);
            ASSERT(vi.value() == MOVED_FROM_VAL);

            mX.reset();
            ASSERT(!mX.has_value());
            mX = MovUtl::move(i);;
            ASSERT(mX.value().value() == 3);
            ASSERT(mX.value().d_data.d_def.d_allocator_p == &oa);
            ASSERT(mX.get_allocator() == &oa);
            ASSERT(vi.value() == MOVED_FROM_VAL);

            mX.reset();
            ASSERT(!mX.has_value());
            mX = MovUtl::move(cvi);
            ASSERT(mX.value().value() == 1);
            ASSERT(mX.value().d_data.d_def.d_allocator_p == &oa);
            ASSERT(mX.get_allocator() == &oa);
            ASSERT(cvi.value() == 1);

            mX.reset();
            ASSERT(!mX.has_value());
            mX = MovUtl::move(ci);
            ASSERT(mX.value().value() == 3);
            ASSERT(mX.value().d_data.d_def.d_allocator_p == &oa);
            ASSERT(mX.get_allocator() == &oa);
            ASSERT(ci.value() == 3);
        }
        {
            ObjC mX = ValueType(0);
            ValueType vi = ValueType(1);
#ifdef BSLSTL_OPTIONAL_TEST_BAD_EQUAL_CONST
            mX = vi; // this should not compile
#endif //BSLSTL_OPTIONAL_TEST_BAD_EQUAL_CONST
        }
    }
}
void bslstl_optional_test14()
{
    // --------------------------------------------------------------------
    // TESTING 'operator=(non_optional_type)' MEMBER FUNCTION
    //   This test will verify that the 'operator=(non_optional_type)' function
    //   works as expected.
    //
    // Concerns:
    //: 1 'operator=(rhs)', where rhs is not an 'optional' object, can not be
    //:    called if 'rhs' is of a type which is not assignable to
    //:    'value_type'.
    //: 2  'operator=(rhs)', where rhs is not an 'optional' object, can not be
    //:    called if 'value_type' is not constructable from 'rhs'.
    //
    // Plan:
    //
    //: 1 Create an 'optional' object of non allocator aware 'value_type'.
    //:   For concern 1, verify that lvalue and rvalue of type convertible,
    //:   but not assignable to 'value_type' can not be assigned to the
    //:   'optional' object. Note that this test requires compilation errors
    //:   and needs to be enabled and checked manually.
    //: 2 Create an 'optional' object of non allocator aware 'value_type'.
    //:   For concern 2, verify that lvalue and rvalue of type assignable,
    //:   but not convertible to 'value_type' can not be assigned to the
    //:   'optional' object. Note that this test requires compilation errors
    //:   and needs to be enabled and checked manually.
    //: 3 Repeat steps 1 and 2 with an 'optional' of allocator aware
    //:   'value_type'.
    //
    //
    // Testing:
    //
    //   operator=(BSLS_COMPILERFEATURES_FORWARD_REF(ANY_TYPE) other)
    //
    // --------------------------------------------------------------------

    if (verbose) printf(
                    "\nTESTING 'operator=(non_optional_type)' MEMBER FUNCTION"
                    "\n======================================================"
                    "\n");
#ifdef BSLSTL_OPTIONAL_TEST_BAD_EQUAL_NONOPT

    if (veryVerbose) printf("\tUsing 'MyClass1b'.\n");
    {
        typedef MyClass1b                  ValueType;
        typedef bsl::optional<ValueType> Obj;
        {
          Obj mX = ValueType(0);
          MyClass1 mc1 = MyClass1(2);

          mX = mc1;             //this should not compile 1/
          mX = MyClass1(0);    // this should not compile 2/
        }
    }
    if (veryVerbose) printf("\tUsing 'MyClass1c'.\n");
    {
       typedef MyClass1c                  ValueType;
       typedef bsl::optional<ValueType> Obj;
       {
         Obj mX = ValueType(0);
         MyClass1 mc1 = MyClass1(2);

         mX = mc1;             //this should not compile 3/
         mX = MyClass1(0);    // this should not compile 4/
       }
    }
    if (veryVerbose) printf("\tUsing 'MyClass2b'.\n");
    {
        typedef MyClass2b                  ValueType;
        typedef bsl::optional<ValueType> Obj;
        {
          Obj mX;
          MyClass2 mc1 = MyClass2(2);

          mX = mc1;             //this should not compile 1/
          mX = MyClass2(0);    // this should not compile 2/
        }
    }
    if (veryVerbose) printf("\tUsing 'MyClass2c'.\n");
    {
       typedef MyClass2c                  ValueType;
       typedef bsl::optional<ValueType> Obj;
       {
         Obj mX;
         MyClass2 mc1 = MyClass2(2);

         mX = mc1;             //this should not compile 3/
         mX = MyClass2(0);    // this should not compile 4/
       }
    }
#endif
}
void bslstl_optional_test15()
{
    // --------------------------------------------------------------------
    // TESTING 'operator=(optional_type)' MEMBER FUNCTION
    //   This test will verify that the 'operator=(rhs)', where 'rhs' is an
    //   'optional' type, works as expected.
    //
    // Concerns:
    //: 1 That 'operator=(rhs)', where 'rhs' is a disengaged 'optional' object,
    //:   makes the target 'optional' object disengaged.
    //: 2 That 'operator=(rhs)', invoked on an engaged 'optional' object and
    //:   where 'rhs' is an engaged 'optional' object, assigns the value of the
    //:   'rhs''optional' to the value of the target 'optional' object.
    //: 3 That 'operator=(rhs)', invoked on a disengaged 'optional' object and
    //:   where 'rhs' is an engaged 'optional' object, constructs a 'value_type'
    //:   object from the value of 'rhs''optional' object.
    //: 4 An assignment operator takign an 'optional' type will be used even if
    //:   'rhs' is a const qualified 'optional', or on 'optional' of a const
    //:   qualified 'value_type'.
    //: 5 For allocator aware types, the assignment from an 'optional' object
    //:   does not modify the allocator
    //: 6 Assignment from rvalues uses move assignment where available
    //: 7 Assignment to an 'optional' of const qualified 'value_type' is not
    //:   possible.
    //
    // Plan:
    //: 1 Create an engaged 'optional' of a non allocator aware 'value_type'.
    //:   Assign a disengaged 'optional' of the same type to it. For concern 1
    //:   check that the destination object is disengaged.
    //: 2 Emplace a value into the test object. Assign an engaged 'optional'
    //:   of the same type to it. For concern 2, check the value of the test
    //:   object is the same as that of the object assigned to it.
    //: 3 Assign a disengaged 'optional' of a 'value_type' convertible to test
    //:   object's 'value_type'. For concern 1, check that the destination
    //:   object is disengaged.
    //: 4 Emplace a value into the test object. Assign an engaged 'optional'
    //:   of a 'value_type' convertible to test object's 'value_type'. For
    //:   concern 2, check the value of the test object is the same as that of
    //:   the object assigned to it.
    //: 5 For concern 4, repeat steps 1-4 using a const qualified 'optional'
    //:   object as 'rhs', and using an 'optional' of const qualified value
    //:   type.
    //: 6 For concern 6, repeat steps 1-5 using an rvalue as 'rhs'. Verify the
    //:   object was moved from if rhs is a non const qualified of a non const
    //:   'value_type', and copied otherwise.
    //: 7 For concern 3, repeat steps 1-6 using a disengaged 'optional' as the
    //:   test object in each step by calling 'reset' before each assignment.
    //: 8 Repeat steps 1-7 using an 'optional' object of allocator aware type
    //:   as the test object. For concern 5, check that the test object's
    //:   allocator has not been modified.
    //: 9 For concern 7, verify that a const qualified 'optional' can not be
    //:   assigned to. Note taht this test requires compilation failures and
    //:   needs to be enabled and checked manually.
    //
    //   Repeat all of the above tests for a disengaged 'optional' as the
    //   destination. That is, call reset before each assignment.
    //
    // Testing:
    //
    //   optional& operator=(const optional&);
    //   optional& operator=( BloombergLP::bslmf::MovableRef<optional>);
    // --------------------------------------------------------------------

    if (verbose) printf(
                    "\nTESTING 'operator=(optional_type)' MEMBER FUNCTION"
                    "\n==================================================\n");

    if (veryVerbose) printf("\tUsing non allocator aware 'value_type'.\n");
    {
        typedef MyClass1a                  ValueType;
        typedef bsl::optional<ValueType>    Obj;
        typedef MyClass1                   OtherType;
        typedef bsl::optional<OtherType>    OtherObj;

#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
        typedef const ValueType             ConstValueType;
        typedef bsl::optional<ConstValueType> ObjC;
        typedef const OtherType             ConstOtherType;
        typedef bsl::optional<ConstOtherType> OtherObjC;

        typedef const Obj CObj;
        typedef const ObjC CObjC;
        typedef const OtherObj COtherObj;
        typedef const OtherObjC COtherObjC;
#endif //BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
        if (veryVeryVerbose) printf("\t\t Using an engaged 'optional' as the"
                                    " test object.\n");
        {
            Obj mX(ValueType(3));

            mX.emplace(2);
            TEST_EQUAL_EMPTY(mX, Obj);

            mX.emplace(2);
            TEST_EQUAL_EMPTY(mX, OtherObj);

#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
            // C++03 MovableRef isn't const friendly which will make these
            // tests fail

            mX.emplace(2);
            TEST_EQUAL_EMPTY(mX, CObj);

            mX.emplace(2);
            TEST_EQUAL_EMPTY(mX, COtherObj);

            mX.emplace(2);
            TEST_EQUAL_EMPTY(mX, ObjC);

            mX.emplace(2);
            TEST_EQUAL_EMPTY(mX, OtherObjC);

            mX.emplace(2);
            TEST_EQUAL_EMPTY(mX, CObjC);

            mX.emplace(2);
            TEST_EQUAL_EMPTY(mX, COtherObjC);
#endif //BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES

            mX.emplace(2);
            TEST_EQUAL_EMPTY_MOVE(mX, Obj);

            mX.emplace(2);
            TEST_EQUAL_EMPTY_MOVE(mX, OtherObj);

#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
            // C++03 MovableRef isn't const friendly which will make these
            // tests fail
            mX.emplace(2);
            TEST_EQUAL_EMPTY_MOVE(mX, CObj);

            mX.emplace(2);
            TEST_EQUAL_EMPTY_MOVE(mX, COtherObj);

            mX.emplace(2);
            TEST_EQUAL_EMPTY_MOVE(mX, ObjC);

            mX.emplace(2);
            TEST_EQUAL_EMPTY_MOVE(mX, OtherObjC);

            mX.emplace(2);
            TEST_EQUAL_EMPTY_MOVE(mX, CObjC);

            mX.emplace(2);
            TEST_EQUAL_EMPTY_MOVE(mX, COtherObjC);

#endif //BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES

            mX.emplace(2);
            TEST_EQUAL_ENGAGED(mX, Obj, ValueType, 7);

            mX.emplace(2);
            TEST_EQUAL_ENGAGED(mX, OtherObj, OtherType, 7);

#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
            // C++03 MovableRef isn't const friendly which will make these
            // tests fail

            mX.emplace(2);
            TEST_EQUAL_ENGAGED(mX, CObj, ValueType, 7);

            mX.emplace(2);
            TEST_EQUAL_ENGAGED(mX, ObjC, ValueType, 7);

            mX.emplace(2);
            TEST_EQUAL_ENGAGED(mX, CObjC, ValueType, 7);

            mX.emplace(2);
            TEST_EQUAL_ENGAGED(mX, COtherObj, OtherType, 7);

            mX.emplace(2);
            TEST_EQUAL_ENGAGED(mX, OtherObjC, OtherType, 7);

            mX.emplace(2);
            TEST_EQUAL_ENGAGED(mX, COtherObjC, OtherType,  7);
#endif //BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES

            mX.emplace(2);
            TEST_EQUAL_ENGAGED_MOVE(mX, Obj, ValueType, 7, MOVED_FROM_VAL);

            mX.emplace(2);
            TEST_EQUAL_ENGAGED_MOVE(mX, OtherObj, OtherType, 7,
                                    MOVED_FROM_VAL);

#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
            // C++03 MovableRef isn't const friendly which will make these
            // tests fail
            mX.emplace(2);
            TEST_EQUAL_ENGAGED_MOVE(mX, CObj, ValueType, 7, 7);

            mX.emplace(2);
            TEST_EQUAL_ENGAGED_MOVE(mX, ObjC, ValueType, 7, 7);

            mX.emplace(2);
            TEST_EQUAL_ENGAGED_MOVE(mX, CObjC, ValueType, 7, 7);


            mX.emplace(2);
            TEST_EQUAL_ENGAGED_MOVE(mX, COtherObj, OtherType, 7, 7);

            mX.emplace(2);
            TEST_EQUAL_ENGAGED_MOVE(mX, OtherObjC, OtherType, 7, 7);

            mX.emplace(2);
            TEST_EQUAL_ENGAGED_MOVE(mX, COtherObjC, OtherType,  7, 7);
#endif //BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES

        }
        if (veryVeryVerbose) printf("\t\tUsing a disengaged 'optional' as "
                                    "the test object.\n");
        {
            Obj mX(ValueType(3));

            mX.reset();
            TEST_EQUAL_EMPTY(mX, Obj);

            mX.reset();
            TEST_EQUAL_EMPTY(mX, OtherObj);

#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
            // C++03 MovableRef isn't const friendly which will make these
            // tests fail
            mX.reset();
            TEST_EQUAL_EMPTY(mX, CObj);

            mX.reset();
            TEST_EQUAL_EMPTY(mX, COtherObj);

            mX.reset();
            TEST_EQUAL_EMPTY(mX, ObjC);

            mX.reset();
            TEST_EQUAL_EMPTY(mX, OtherObjC);

            mX.reset();
            TEST_EQUAL_EMPTY(mX, CObjC);

            mX.reset();
            TEST_EQUAL_EMPTY(mX, COtherObjC);
#endif //BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES

            mX.reset();
            TEST_EQUAL_EMPTY_MOVE(mX, Obj);

            mX.reset();
            TEST_EQUAL_EMPTY_MOVE(mX, OtherObj);

#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
            // C++03 MovableRef isn't const friendly which will make these
            // tests fail
            mX.reset();
            TEST_EQUAL_EMPTY_MOVE(mX, CObj);

            mX.reset();
            TEST_EQUAL_EMPTY_MOVE(mX, COtherObj);

            mX.reset();
            TEST_EQUAL_EMPTY_MOVE(mX, ObjC);

            mX.reset();
            TEST_EQUAL_EMPTY_MOVE(mX, OtherObjC);

            mX.reset();
            TEST_EQUAL_EMPTY_MOVE(mX, CObjC);

            mX.reset();
            TEST_EQUAL_EMPTY_MOVE(mX, COtherObjC);
#endif //BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES

            mX.reset();
            TEST_EQUAL_ENGAGED(mX, Obj, ValueType, 7);

            mX.reset();
            TEST_EQUAL_ENGAGED(mX, OtherObj, OtherType, 7);

#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
            // C++03 MovableRef isn't const friendly which will make these
            // tests fail
            mX.reset();
            TEST_EQUAL_ENGAGED(mX, CObj, ValueType, 7);

            mX.reset();
            TEST_EQUAL_ENGAGED(mX, ObjC, ValueType, 7);

            mX.reset();
            TEST_EQUAL_ENGAGED(mX, CObjC, ValueType, 7);

            mX.reset();
            TEST_EQUAL_ENGAGED(mX, COtherObj, OtherType, 7);

            mX.reset();
            TEST_EQUAL_ENGAGED(mX, OtherObjC, OtherType, 7);

            mX.reset();
            TEST_EQUAL_ENGAGED(mX, COtherObjC, OtherType,  7);
#endif //BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES

            mX.reset();
            TEST_EQUAL_ENGAGED_MOVE(mX, Obj, ValueType, 7, MOVED_FROM_VAL);

            mX.reset();
            TEST_EQUAL_ENGAGED_MOVE(mX, OtherObj, OtherType, 7,
                                    MOVED_FROM_VAL);
#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
            // C++03 MovableRef isn't const friendly which will make these
            // tests fail
            mX.reset();
            TEST_EQUAL_ENGAGED_MOVE(mX, CObj, ValueType, 7, 7);

            mX.reset();
            TEST_EQUAL_ENGAGED_MOVE(mX, ObjC, ValueType, 7, 7);

            mX.reset();
            TEST_EQUAL_ENGAGED_MOVE(mX, CObjC, ValueType, 7, 7);


            mX.reset();
            TEST_EQUAL_ENGAGED_MOVE(mX, COtherObj, OtherType, 7, 7);

            mX.reset();
            TEST_EQUAL_ENGAGED_MOVE(mX, OtherObjC, OtherType, 7, 7);

            mX.reset();
            TEST_EQUAL_ENGAGED_MOVE(mX, COtherObjC, OtherType,  7, 7);
#endif //BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
        }
    }
    if (veryVerbose) printf("\tUsing allocator aware 'value_type'\n");
    {
        bslma::TestAllocator da("default", veryVeryVeryVerbose);
        bslma::TestAllocator oa("other", veryVeryVeryVerbose);
        bslma::TestAllocator ta("third", veryVeryVeryVerbose);

        typedef MyClass2a                  ValueType;
        typedef bsl::optional<ValueType>    Obj;

        typedef MyClass2                   OtherType;
        typedef bsl::optional<OtherType>    OtherObj;

#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
        typedef const ValueType             ConstValueType;
        typedef bsl::optional<ConstValueType> ObjC;
        typedef const OtherType             ConstOtherType;
        typedef bsl::optional<ConstOtherType> OtherObjC;

        typedef const Obj CObj;
        typedef const ObjC CObjC;
        typedef const OtherObj COtherObj;
        typedef const OtherObjC COtherObjC;
#endif //BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES

        if (veryVeryVerbose) printf("\t\tUsing an engaged 'optional' as the"
                                    " test object.\n");

        {
            Obj mX(bsl::allocator_arg, &oa, ValueType(3));

            mX.emplace(2);
            TEST_EQUAL_EMPTY_A(mX, Obj);

            mX.emplace(2);
            TEST_EQUAL_EMPTY_A(mX, OtherObj);

#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
            // C++03 MovableRef isn't const friendly which will make these
            // tests fail
            mX.emplace(2);
            TEST_EQUAL_EMPTY_A(mX, CObj);

            mX.emplace(2);
            TEST_EQUAL_EMPTY_A(mX, COtherObj);

            mX.emplace(2);
            TEST_EQUAL_EMPTY_A(mX, ObjC);

            mX.emplace(2);
            TEST_EQUAL_EMPTY_A(mX, OtherObjC);

            mX.emplace(2);
            TEST_EQUAL_EMPTY_A(mX, CObjC);

            mX.emplace(2);
            TEST_EQUAL_EMPTY_A(mX, COtherObjC);
#endif //BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES

            mX.emplace(2);
            TEST_EQUAL_EMPTY_MOVE_A(mX, Obj);

            mX.emplace(2);
            TEST_EQUAL_EMPTY_MOVE_A(mX, OtherObj);

#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
            // C++03 MovableRef isn't const friendly which will make these
            // tests fail
            mX.emplace(2);
            TEST_EQUAL_EMPTY_MOVE_A(mX, CObj);

            mX.emplace(2);
            TEST_EQUAL_EMPTY_MOVE_A(mX, COtherObj);

            mX.emplace(2);
            TEST_EQUAL_EMPTY_MOVE_A(mX, ObjC);

            mX.emplace(2);
            TEST_EQUAL_EMPTY_MOVE_A(mX, OtherObjC);

            mX.emplace(2);
            TEST_EQUAL_EMPTY_MOVE_A(mX, CObjC);

            mX.emplace(2);
            TEST_EQUAL_EMPTY_MOVE_A(mX, COtherObjC);
#endif //BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES

            mX.emplace(2);
            TEST_EQUAL_ENGAGED_A(mX, Obj, ValueType, 7);

            mX.emplace(2);
            TEST_EQUAL_ENGAGED_A(mX, OtherObj, OtherType, 7);
#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
            // C++03 MovableRef isn't const friendly which will make these
            // tests fail
            mX.emplace(2);
            TEST_EQUAL_ENGAGED_A(mX, CObj, ValueType, 7);

            mX.emplace(2);
            TEST_EQUAL_ENGAGED_A(mX, ObjC, ValueType, 7);

            mX.emplace(2);
            TEST_EQUAL_ENGAGED_A(mX, CObjC, ValueType, 7);

            mX.emplace(2);
            TEST_EQUAL_ENGAGED_A(mX, COtherObj, OtherType, 7);

            mX.emplace(2);
            TEST_EQUAL_ENGAGED_A(mX, OtherObjC, OtherType, 7);

            mX.emplace(2);
            TEST_EQUAL_ENGAGED_A(mX, COtherObjC, OtherType,  7);
#endif //BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES

            mX.emplace(2);
            TEST_EQUAL_ENGAGED_MOVE_A(mX, Obj, ValueType, 7, MOVED_FROM_VAL);

            mX.emplace(2);
            TEST_EQUAL_ENGAGED_MOVE_A(mX, OtherObj, OtherType, 7,
                                      MOVED_FROM_VAL);
#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
            // C++03 MovableRef isn't const friendly which will make these
            // tests fail
            mX.emplace(2);
            TEST_EQUAL_ENGAGED_MOVE_A(mX, CObj, ValueType, 7, 7);

            mX.emplace(2);
            TEST_EQUAL_ENGAGED_MOVE_A(mX, ObjC, ValueType, 7, 7);

            mX.emplace(2);
            TEST_EQUAL_ENGAGED_MOVE_A(mX, CObjC, ValueType, 7, 7);

            mX.emplace(2);
            TEST_EQUAL_ENGAGED_MOVE_A(mX, COtherObj, OtherType, 7, 7);

            mX.emplace(2);
            TEST_EQUAL_ENGAGED_MOVE_A(mX, OtherObjC, OtherType, 7, 7);

            mX.emplace(2);
            TEST_EQUAL_ENGAGED_MOVE_A(mX, COtherObjC, OtherType,  7, 7);
#endif //BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
        }
        if (veryVeryVerbose) printf("\t\tUsing a disengaged 'optional' as "
                                    "the test object.\n");
        {
            Obj mX(bsl::allocator_arg, &oa);

            mX.reset();
            TEST_EQUAL_EMPTY_A(mX, Obj);

            mX.reset();
            TEST_EQUAL_EMPTY_A(mX, OtherObj);

#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
            // C++03 MovableRef isn't const friendly which will make these
            // tests fail
            mX.reset();
            TEST_EQUAL_EMPTY_A(mX, CObj);

            mX.reset();
            TEST_EQUAL_EMPTY_A(mX, COtherObj);

            mX.reset();
            TEST_EQUAL_EMPTY_A(mX, ObjC);

            mX.reset();
            TEST_EQUAL_EMPTY_A(mX, CObjC);

            mX.reset();
            TEST_EQUAL_EMPTY_A(mX, COtherObjC);
#endif //BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES

            mX.reset();
            TEST_EQUAL_EMPTY_MOVE_A(mX, Obj);

            mX.reset();
            TEST_EQUAL_EMPTY_MOVE_A(mX, OtherObj);

#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
            // C++03 MovableRef isn't const friendly which will make these
            // tests fail
            mX.reset();
            TEST_EQUAL_EMPTY_MOVE_A(mX, CObj);

            mX.reset();
            TEST_EQUAL_EMPTY_MOVE_A(mX, COtherObj);

            mX.reset();
            TEST_EQUAL_EMPTY_MOVE_A(mX, ObjC);

            mX.reset();
            TEST_EQUAL_EMPTY_MOVE_A(mX, CObjC);

            mX.reset();
            TEST_EQUAL_EMPTY_MOVE_A(mX, COtherObjC);
#endif //BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES

            mX.reset();
            TEST_EQUAL_ENGAGED_A(mX, Obj, ValueType, 7);

            mX.reset();
            TEST_EQUAL_ENGAGED_A(mX, OtherObj, OtherType, 7);

#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
            // C++03 MovableRef isn't const friendly which will make these
            // tests fail
            mX.reset();
            TEST_EQUAL_ENGAGED_A(mX, CObj, ValueType, 7);

            mX.reset();
            TEST_EQUAL_ENGAGED_A(mX, ObjC, ValueType, 7);

            mX.reset();
            TEST_EQUAL_ENGAGED_A(mX, CObjC, ValueType, 7);

            mX.reset();
            TEST_EQUAL_ENGAGED_A(mX, COtherObj, OtherType, 7);

            mX.reset();
            TEST_EQUAL_ENGAGED_A(mX, OtherObjC, OtherType, 7);

            mX.reset();
            TEST_EQUAL_ENGAGED_A(mX, COtherObjC, OtherType,  7);

#endif //BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES

            mX.reset();
            TEST_EQUAL_ENGAGED_MOVE_A(mX, Obj, ValueType, 7, MOVED_FROM_VAL);

            mX.reset();
            TEST_EQUAL_ENGAGED_MOVE_A(mX, OtherObj, OtherType, 7,
                                      MOVED_FROM_VAL);

#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
            // C++03 MovableRef isn't const friendly which will make these
            // tests fail
            mX.reset();
            TEST_EQUAL_ENGAGED_MOVE_A(mX, CObj, ValueType, 7, 7);

            mX.reset();
            TEST_EQUAL_ENGAGED_MOVE_A(mX, ObjC, ValueType, 7, 7);

            mX.reset();
            TEST_EQUAL_ENGAGED_MOVE_A(mX, CObjC, ValueType, 7, 7);

            mX.reset();
            TEST_EQUAL_ENGAGED_MOVE_A(mX, COtherObj, OtherType, 7, 7);

            mX.reset();
            TEST_EQUAL_ENGAGED_MOVE_A(mX, OtherObjC, OtherType, 7, 7);

            mX.reset();
            TEST_EQUAL_ENGAGED_MOVE_A(mX, COtherObjC, OtherType,  7, 7);
#endif //BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
        }
    }
    {
        typedef MyClass1a                  ValueType;
        typedef const ValueType             ConstValueType;
        typedef bsl::optional<ConstValueType> ObjC;

        typedef MyClass1                   OtherType;

        ObjC mX = ValueType(0);
        OtherType i = OtherType(3);
#ifdef BSLSTL_OPTIONAL_TEST_BAD_EQUAL_CONST
        mX = i;
#endif //BSLSTL_OPTIONAL_TEST_BAD_EQUAL_CONST
    }
    {
        typedef MyClass2a                  ValueType;
        typedef const ValueType             ConstValueType;
        typedef bsl::optional<ConstValueType> ObjC;

        typedef MyClass2                    OtherType;

        ObjC mX = ValueType(0);
        OtherType i = OtherType(3);
#ifdef BSLSTL_OPTIONAL_TEST_BAD_EQUAL_CONST
        mX = i;
#endif //BSLSTL_OPTIONAL_TEST_BAD_EQUAL_CONST
     }
}

void bslstl_optional_test16()
{
    // --------------------------------------------------------------------
    // TESTING 'operator=(optional_type)' MEMBER FUNCTION
    //   This test will verify that the 'operator=(rhs)', where 'rhs' is an
    //   optional type, member function works as expected. These test require
    //   compilation failures and will not run by default.
    //
    // Concerns:
    //: 1 'operator=(rhs)', where 'rhs' is an 'optional' type whose
    //:   'value_type' is not assignable to the 'value_type' of the target
    //:    'optional', can not be called.
    //: 2 'operator=(rhs)', where 'rhs' is an 'optional' type whose
    //:   'value_type' is not convertible to target 'optional''s 'value_type',
    //:    can not be called.
    //
    // Plan:
    //: 1 Create an 'optional' object of non allocator aware 'value_type' as
    //:   the target object. For concern 1, verify that an 'optional' object of
    //:   'value_type' which is convertible, but not assignable to target
    //:   object 'value_type' can not be assigned to the target object. Note
    //:   that this test requires compilation errors and needs to be enabled
    //:   and checked manually.
    //: 2 For concern 2, verify that an 'optional' object of 'value_type' which
    //:   is assignable, but not convertible to target object's 'value_type'
    //:   can not be assigned to the target object. Note that this test
    //:   requires compilation errors and needs to be enabled and checked
    //:   manually.
    //: 3 Repeat steps 1 and 2 with an 'optional' of allocator aware
    //:   'value_type' as the target object.
    //
    //
    // Testing:
    //
    //   operator=(BSLS_COMPILERFEATURES_FORWARD_REF(ANY_TYPE) other)
    //
    // --------------------------------------------------------------------

    if (verbose) printf(
                    "\nTESTING 'operator=(optional_type)' MEMBER FUNCTION"
                    "\n==================================================\n");
#ifdef BSLSTL_OPTIONAL_TEST_BAD_EQUAL_OPT

    if (veryVerbose) printf("\tUsing 'MyClass1b'.\n");
    {
        typedef MyClass1b                  ValueType;
        typedef bsl::optional<ValueType> Obj;
        {
          Obj mX = ValueType(0);
          bsl::optional<MyClass1> mc1 = MyClass1(2);

          mX = mc1;             //this should not compile
          mX = MyClass1(0);    // this should not compile
        }
    }
    if (veryVerbose) printf("\tUsing 'MyClass1c'.\n");
    {
       typedef MyClass1c                  ValueType;
       typedef bsl::optional<ValueType> Obj;
       {
         Obj mX = ValueType(0);
         bsl::optional<MyClass1> mc1 = MyClass1(2);

         mX = mc1;             //this should not compile
         mX = MyClass1(0);    // this should not compile
       }
    }
    if (veryVerbose) printf("\tUsing 'MyClass2b'.\n");
    {
        typedef MyClass2b                  ValueType;
        typedef bsl::optional<ValueType> Obj;
        {
          Obj mX;
          bsl::optional<MyClass2> mc1 = MyClass2(2);

          mX = mc1;             //this should not compile
          mX = MyClass2(0);    // this should not compile
        }
    }
    if (veryVerbose) printf("\tUsing 'MyClass2c'.\n");
    {
       typedef MyClass2c                  ValueType;
       typedef bsl::optional<ValueType> Obj;
       {
         Obj mX;
         bsl::optional<MyClass2> mc1 = MyClass2(2);

         mX = mc1;             //this should not compile
         mX = MyClass2(0);    // this should not compile
       }
    }
#endif
}
void bslstl_optional_test17()
{
    // --------------------------------------------------------------------
    // TESTING COPY CONSTRUCTION
    //   This test will verify that the copy construction works as expected.
    //
    // Concerns:
    //: 1 Constructing an 'optional' from an engaged 'optional' of the same
    //    type creates an engaged 'optional' where the 'value_type' object is
    //:   copy constructed from the 'value_type' object of the original
    //:   'optional' object. The original is not modified.
    //: 2 Constructing an 'optional' from a disengaged 'optional' of the same
    //    type creates a disengaged 'optional'. The original is not modified.
    //: 3 If 'value_type' is allocator aware, and no allocator is provided, the
    //:   default allocator is used for the newly created 'optional'.
    //: 4 If allocator extended version of copy constructor is used, the
    //:   allocator passed to the constructor is the allocator of the newly
    //:   created 'optional'.
    //: 5 No unnecessary copies of the 'value_type' are created.
    //
    // Plan:
    //: 1 Create an engaged 'optional' of a non allocator aware 'value_type'.
    //:   Use the created object to copy initialize another 'optional'
    //:   object of the same type. For concern 1, check the value of the new
    //:   object and the value of the original object match.
    //: 2 Bind a 'const' reference to the 'original' object. Using the const
    //:   reference, copy initialize another 'optional' object of the same
    //:   type. For concern 1, check the value of the new object and the value
    //:   of the original object match.
    //: 3 Repeat steps 1 and 2 using a disengaged 'optional' object as the
    //:   source object. For concern 2, check that both the source and
    //:   destination object are disengaged.
    //: 4 For concern 3, repeat steps 1-3 using an allocator aware type, and
    //:   verify that the allocator of the newly created 'optional' object is
    //:   the default allocator.
    //: 5 For concern 4, repeat steps 1-3 using an allocator aware type and
    //:   an allocator extended copy constructo, and verify that the allocator
    //:   of the newly created 'optional' object is the allocator provided to
    //:   the copy constructor.
    //: 6 In steps 1-5, for concern 5, check that no unnecessary copies of the
    //:   'value_type' have been created.
    //
    // Testing:
    //
    //   optional(const optional& )
    //   optional(bsl::allocator_arg, allocator_type, const optional&)
    // --------------------------------------------------------------------

    if (verbose) printf(
                       "\nTESTING COPY CONSTRUCTION "
                       "\n=========================\n");

    if (veryVerbose) printf("\tUsing 'MyClass1'.\n");
    {
        typedef MyClass1                  ValueType;
        typedef bsl::optional<ValueType> Obj;

        {
            Obj source(ValueType(1));
            ASSERT(source.has_value());

            int CCI = ValueType::copyConstructorInvocations;
            int MCI = ValueType::moveConstructorInvocations;
            Obj dest = source;
            ASSERT(dest.has_value());
            ASSERT(dest.value() == source.value());
            ASSERT(dest.value().value() == 1);
            ASSERT(source.has_value());
            ASSERT(CCI == ValueType::copyConstructorInvocations -1);
            ASSERT(MCI == ValueType::moveConstructorInvocations);


            const Obj source2(ValueType(2));
            ASSERT(source2.has_value());
            CCI = ValueType::copyConstructorInvocations;
            MCI = ValueType::moveConstructorInvocations;
            Obj dest2 = source2;
            ASSERT(dest2.has_value());
            ASSERT(dest2.value() == source2.value());
            ASSERT(dest2.value().value() == 2);
            ASSERT(CCI == ValueType::copyConstructorInvocations-1);
            ASSERT(MCI == ValueType::moveConstructorInvocations);
        }
        {
            Obj source(nullopt);
            ASSERT(!source.has_value());

            Obj dest = source;
            ASSERT(!dest.has_value());
            ASSERT(!source.has_value());

            const Obj & csource = source;
            ASSERT(!csource.has_value());

            Obj dest2 = csource;
            ASSERT(!dest2.has_value());
        }
    }
    if (veryVerbose) printf("\tUsing 'MyClass2'.\n");
    {
        bslma::TestAllocator da("default", veryVeryVeryVerbose);
        bslma::TestAllocator oa("other", veryVeryVeryVerbose);
        bslma::TestAllocator ta("third", veryVeryVeryVerbose);

        bslma::DefaultAllocatorGuard dag(&da);

        typedef MyClass2                  ValueType;
        typedef bsl::optional<ValueType> Obj;

        {
          Obj source(bsl::allocator_arg, &oa, ValueType(1));
          ASSERT(source.has_value());
          ASSERT(&oa == source.get_allocator());

          int CCI = ValueType::copyConstructorInvocations;
          int MCI = ValueType::moveConstructorInvocations;
          Obj dest = source;
          ASSERT(dest.has_value());
          ASSERT(dest.value() == source.value());
          ASSERT(dest.value().value() == 1);
          ASSERT(&da == dest.get_allocator());
          ASSERT(source.has_value());
          ASSERT(&oa == source.get_allocator());
          ASSERT(CCI == ValueType::copyConstructorInvocations-1);
          ASSERT(MCI == ValueType::moveConstructorInvocations);

          const Obj & csource = source;
          ASSERT(csource.has_value());

          CCI = ValueType::copyConstructorInvocations;
          MCI = ValueType::moveConstructorInvocations;
          Obj dest2 = csource;
          ASSERT(dest2.has_value());
          ASSERT(dest2.value() == csource.value());
          ASSERT(&da == dest2.get_allocator());
          ASSERT(CCI == ValueType::copyConstructorInvocations-1);
          ASSERT(MCI == ValueType::moveConstructorInvocations);


          CCI = ValueType::copyConstructorInvocations;
          MCI = ValueType::moveConstructorInvocations;
          Obj dest3(bsl::allocator_arg, &ta, source);
          ASSERT(dest3.has_value());
          ASSERT(dest3.value() == source.value());
          ASSERT(dest3.value().value() == 1);
          ASSERT(&ta == dest3.get_allocator());
          ASSERT(source.has_value());
          ASSERT(&oa == source.get_allocator());
          ASSERT(CCI == ValueType::copyConstructorInvocations-1);
          ASSERT(MCI == ValueType::moveConstructorInvocations);

          CCI = ValueType::copyConstructorInvocations;
          MCI = ValueType::moveConstructorInvocations;
          Obj dest4(bsl::allocator_arg, &ta, csource);
          ASSERT(dest2.has_value());
          ASSERT(dest2.value() == source.value());
          ASSERT(&ta == dest4.get_allocator());
          ASSERT(CCI == ValueType::copyConstructorInvocations-1);
          ASSERT(MCI == ValueType::moveConstructorInvocations);
       }
       {
          Obj source(bsl::allocator_arg, &oa, nullopt);
          ASSERT(!source.has_value());
          ASSERT(&oa == source.get_allocator());

          Obj dest = source;
          ASSERT(!dest.has_value());
          ASSERT(&da == dest.get_allocator());
          ASSERT(!source.has_value());

          const Obj & csource = source;
          ASSERT(!csource.has_value());

          Obj dest2 = csource;
          ASSERT(!dest2.has_value());
          ASSERT(&da == dest2.get_allocator());

          Obj dest3(bsl::allocator_arg, &ta, source);
          ASSERT(!dest3.has_value());
          ASSERT(&ta == dest3.get_allocator());

          Obj dest4(bsl::allocator_arg, &ta, csource);
          ASSERT(!dest4.has_value());
          ASSERT(&ta == dest4.get_allocator());
       }
    }
    if (veryVerbose) printf("\tUsing 'MyClass2a'.\n");
   {
       bslma::TestAllocator da("default", veryVeryVeryVerbose);
       bslma::TestAllocator oa("other", veryVeryVeryVerbose);
       bslma::TestAllocator ta("third", veryVeryVeryVerbose);

       bslma::DefaultAllocatorGuard dag(&da);

       typedef MyClass2a                  ValueType;
       typedef bsl::optional<ValueType> Obj;

       {
         Obj source(bsl::allocator_arg, &oa, ValueType(1));
         ASSERT(source.has_value());
         ASSERT(&oa == source.get_allocator());
         int CCI = ValueType::copyConstructorInvocations;
         int MCI = ValueType::moveConstructorInvocations;

         Obj dest = source;
         ASSERT(dest.has_value());
         ASSERT(dest.value() == source.value());
         ASSERT(dest.value().value() == 1);
         ASSERT(&da == dest.get_allocator());
         ASSERT(source.has_value());
         ASSERT(&oa == source.get_allocator());
         ASSERT(CCI == ValueType::copyConstructorInvocations-1);
         ASSERT(MCI == ValueType::moveConstructorInvocations);


         const Obj & csource = source;
         ASSERT(csource.has_value());

         CCI = ValueType::copyConstructorInvocations;
         MCI = ValueType::moveConstructorInvocations;
         Obj dest2 = csource;
         ASSERT(dest2.has_value());
         ASSERT(dest2.value() == csource.value());
         ASSERT(&da == dest2.get_allocator());
         ASSERT(CCI == ValueType::copyConstructorInvocations-1);
         ASSERT(MCI == ValueType::moveConstructorInvocations);

         CCI = ValueType::copyConstructorInvocations;
         MCI = ValueType::moveConstructorInvocations;
         Obj dest3(bsl::allocator_arg, &ta, source);
         ASSERT(dest3.has_value());
         ASSERT(dest3.value() == source.value());
         ASSERT(dest3.value().value() == 1);
         ASSERT(&ta == dest3.get_allocator());
         ASSERT(source.has_value());
         ASSERT(&oa == source.get_allocator());
         ASSERT(CCI == ValueType::copyConstructorInvocations-1);
         ASSERT(MCI == ValueType::moveConstructorInvocations);

         CCI = ValueType::copyConstructorInvocations;
         MCI = ValueType::moveConstructorInvocations;
         Obj dest4(bsl::allocator_arg, &ta, csource);
         ASSERT(dest2.has_value());
         ASSERT(dest2.value() == source.value());
         ASSERT(&ta == dest4.get_allocator());
         ASSERT(CCI == ValueType::copyConstructorInvocations-1);
         ASSERT(MCI == ValueType::moveConstructorInvocations);
       }
      {
         Obj source(bsl::allocator_arg, &oa, nullopt);
         ASSERT(!source.has_value());
         ASSERT(&oa == source.get_allocator());

         Obj dest = source;
         ASSERT(!dest.has_value());
         ASSERT(&da == dest.get_allocator());
         ASSERT(!source.has_value());

         const Obj & csource = source;
         ASSERT(!csource.has_value());

         Obj dest2 = csource;
         ASSERT(!dest2.has_value());
         ASSERT(&da == dest2.get_allocator());

         Obj dest3(bsl::allocator_arg, &ta, source);
         ASSERT(!dest3.has_value());
         ASSERT(&ta == dest3.get_allocator());

         Obj dest4(bsl::allocator_arg, &ta, csource);
         ASSERT(!dest4.has_value());
         ASSERT(&ta == dest4.get_allocator());
      }
   }
}

void bslstl_optional_test18()
{
  // --------------------------------------------------------------------
  // TESTING MOVE CONSTRUCTION
  //   This test will verify that the move construction works as expected.
  //
  // Concerns:
  //: 1 Move constructing an 'optional' from an engaged 'optional' of the same
  //:   type creates an engaged 'optional' by moving from the source object's
  //:   value. The original is left engaged, but with a value object in a
  //:   moved from state.
  //: 2 Move constructing an 'optional' from a disengaged 'optional' of the
  //:   same 'value_type' creates a disengaged 'optional'.
  //: 3 If no allocator is provided and the 'value_type' uses allocator, the
  //:   allocator from the moved from 'optional' is used for the newly created
  //:   'optional'.
  //: 4 If allocator extended version of move constructor is used, the
  //:   allocator passed to the constructor is the allocator of the newly
  //:   created 'optional'.
  //: 5 No unnecessary copies of the value object are created.
  //
  //
  // Plan:
  //: 1 Create an engaged 'optional' of non allocator aware 'value_type'.
  //:   Create another 'optional' of the same 'value_type' by move construction
  //:   from the original object. For concern 1, check the value of the newly
  //:   created object is as expected, that the source's 'value_type' object
  //:   has been moved from, and that the source object is still engaged.
  //: 2 Create an disengaged 'optional' of non allocator aware 'value_type'.
  //:   Create another 'optional' of the same 'value_type' by move construction
  //:   from the original object. For concern 2, check the newly created
  //:   'optional' is disengaged.
  //: 3 Repeat steps 1-2 with an 'optional' object of an allocator aware type.
  //:   For concern 3, check that the allocator from the source object is
  //:   propagated.
  //: 4 In step 3, use allocator extended move constructor. For concern 4,
  //:   check the target uses the allocator passed into the constructor.
  //: 5 In steps 1-4, for concern 5, check no unnecessary copies of the value
  //
  //
  // Testing:
  //
  //   optional(MovableRef<optional>)
  //   optional(bsl::allocator_arg, allocator_type, MovableRef<optional>)
  //
  // --------------------------------------------------------------------

    if (verbose) printf(
                       "\nTESTING MOVE CONSTRUCTION"
                       "\n=========================\n");

    if (veryVerbose) printf("\tUsing 'MyClass1'.\n");
    {
        typedef MyClass1                  ValueType;
        typedef bsl::optional<ValueType> Obj;

        {
            Obj source(ValueType(1));
            ASSERT(source.has_value());

            int CCI = ValueType::copyConstructorInvocations;
            int MCI = ValueType::moveConstructorInvocations;
            Obj dest = MovUtl::move(source);
            ASSERT(dest.has_value());
            ASSERT(dest.value().value() == 1);
            ASSERT(source.has_value());
            ASSERT(CCI == ValueType::copyConstructorInvocations);
            ASSERT(MCI == ValueType::moveConstructorInvocations-1);
            ASSERT(source.value().value() == MOVED_FROM_VAL);
        }
        {
            Obj source(nullopt);
            ASSERT(!source.has_value());

            Obj dest = MovUtl::move(source);
            ASSERT(!dest.has_value());
            ASSERT(!source.has_value());
        }
    }
    if (veryVerbose) printf("\tUsing 'MyClass2'.\n");
    {
        bslma::TestAllocator da("default", veryVeryVeryVerbose);
        bslma::TestAllocator oa("other", veryVeryVeryVerbose);
        bslma::TestAllocator ta("third", veryVeryVeryVerbose);

        bslma::DefaultAllocatorGuard dag(&da);

        typedef MyClass2                  ValueType;
        typedef bsl::optional<ValueType> Obj;

        {
          Obj source(bsl::allocator_arg, &oa, ValueType(1));
          ASSERT(source.has_value());
          ASSERT(source.get_allocator() == &oa);

          int CCI = ValueType::copyConstructorInvocations;
          int MCI = ValueType::moveConstructorInvocations;
          Obj dest = MovUtl::move(source);
          ASSERT(dest.has_value());
          ASSERT(dest.value().value() == 1);
          ASSERT(dest.get_allocator() == &oa);
          ASSERT(source.has_value());
          ASSERT(source.value().value() == MOVED_FROM_VAL);
          ASSERT(CCI == ValueType::copyConstructorInvocations);
          ASSERT(MCI == ValueType::moveConstructorInvocations-1);
          ASSERT(source.get_allocator() == &oa);

          Obj source3(bsl::allocator_arg, &oa, ValueType(3));
          ASSERT(source3.has_value());
          ASSERT(source3.get_allocator() == &oa);

          CCI = ValueType::copyConstructorInvocations;
          MCI = ValueType::moveConstructorInvocations;
          Obj dest3(bsl::allocator_arg, &ta, MovUtl::move(source3));
          ASSERT(dest3.has_value());
          ASSERT(dest3.value().value() == 3);
          ASSERT(dest3.get_allocator() == &ta);
          ASSERT(source3.has_value());
          ASSERT(source3.value().value() == MOVED_FROM_VAL);
          ASSERT(CCI == ValueType::copyConstructorInvocations);
          ASSERT(MCI == ValueType::moveConstructorInvocations-1);
          ASSERT(source3.get_allocator() == &oa);
       }
       {
          Obj source(bsl::allocator_arg, &oa, nullopt);
          ASSERT(!source.has_value());
          ASSERT(source.get_allocator() == &oa);

          Obj dest = MovUtl::move(source);
          ASSERT(!dest.has_value());
          ASSERT(dest.get_allocator() == &oa);
          ASSERT(!source.has_value());
          ASSERT(source.get_allocator() == &oa);

          Obj source3(bsl::allocator_arg, &oa, nullopt);
          ASSERT(!source3.has_value());
          ASSERT(source3.get_allocator() == &oa);

          Obj dest3(bsl::allocator_arg, &ta, MovUtl::move(source3));
          ASSERT(!dest3.has_value());
          ASSERT(dest3.get_allocator() == &ta);
       }
    }
    if (veryVerbose) printf("\tUsing 'MyClass2a'.\n");
    {
        bslma::TestAllocator da("default", veryVeryVeryVerbose);
        bslma::TestAllocator oa("other", veryVeryVeryVerbose);
        bslma::TestAllocator ta("third", veryVeryVeryVerbose);

        bslma::DefaultAllocatorGuard dag(&da);

        typedef MyClass2a                  ValueType;
        typedef bsl::optional<ValueType> Obj;

        {
          Obj source(bsl::allocator_arg, &oa, ValueType(1));
          ASSERT(source.has_value());
          ASSERT(source.get_allocator() == &oa);

          int CCI = ValueType::copyConstructorInvocations;
          int MCI = ValueType::moveConstructorInvocations;
          Obj dest = MovUtl::move(source);
          ASSERT(dest.has_value());
          ASSERT(dest.value().value() == 1);
          ASSERT(dest.get_allocator() == &oa);
          ASSERT(source.has_value());
          ASSERT(source.value().value() == MOVED_FROM_VAL);
          ASSERT(CCI == ValueType::copyConstructorInvocations);
          ASSERT(MCI == ValueType::moveConstructorInvocations-1);
          ASSERT(source.get_allocator() == &oa);

          Obj source3(bsl::allocator_arg, &oa, ValueType(3));
          ASSERT(source3.has_value());
          ASSERT(source3.get_allocator() == &oa);

          CCI = ValueType::copyConstructorInvocations;
          MCI = ValueType::moveConstructorInvocations;
          Obj dest3(bsl::allocator_arg, &ta, MovUtl::move(source3));
          ASSERT(dest3.has_value());
          ASSERT(dest3.value().value() == 3);
          ASSERT(dest3.get_allocator() == &ta);
          ASSERT(source3.has_value());
          ASSERT(source3.value().value() == MOVED_FROM_VAL);
          ASSERT(CCI == ValueType::copyConstructorInvocations);
          ASSERT(MCI == ValueType::moveConstructorInvocations-1);
          ASSERT(source3.get_allocator() == &oa);
       }
       {
          Obj source(bsl::allocator_arg, &oa, nullopt);
          ASSERT(!source.has_value());
          ASSERT(source.get_allocator() == &oa);

          Obj dest = MovUtl::move(source);
          ASSERT(!dest.has_value());
          ASSERT(dest.get_allocator() == &oa);
          ASSERT(!source.has_value());
          ASSERT(source.get_allocator() == &oa);

          Obj source3(bsl::allocator_arg, &oa, nullopt);
          ASSERT(!source3.has_value());
          ASSERT(source3.get_allocator() == &oa);

          Obj dest3(bsl::allocator_arg, &ta, MovUtl::move(source3));
          ASSERT(!dest3.has_value());
          ASSERT(dest3.get_allocator() == &ta);
       }
    }
}
void bslstl_optional_test19()
{
  // --------------------------------------------------------------------
  // TESTING COPY CONSTRUCTION FROM VALUE TYPE
  //   This test will verify that the copy construction from 'value_type' works
  //   as expected.
  //
  // Concerns:
  //: 1 Constructing an 'optional' from a 'value_type' object creates an
  //:   engaged 'optional' with the value of the source object.
  //: 2 Copy construction from a 'value_type' object does not modify the source
  //:   object.
  //: 3 If no allocator is provided and the 'value_type' uses allocator, the
  //:   default allocator is used for the newly created 'optional'.
  //: 4 If allocator extended version of copy constructor is used, the allocator
  //:   passed to the constructors is the allocator of the newly created
  //:   'optional' object.
  //: 5 No unnecessary copies of the 'value_type' are created.
  //
  // Plan:
  //
  //: 1 Create an object of non allocator aware type and use it as an
  //:   initialization object for an 'optional' of that 'value_type'. For
  //:   concern 1, check the constructed 'optional' object is engaged and
  //:   contains the value of the object used to initialise it.
  //: 2 Bind a const reference to the original object of 'value_type'. For
  //:   concern 2, use that reference to initialize an 'optional' object of
  //:   that 'value_type'. For concern 1, check the constructed 'optional' object
  //:   is engaged and contains the value of the object used to initialise it.
  //: 3 Repeat steps 1 and 2 using an allocator aware type as the 'value_type'.
  //:   For concern 3, check the allocator of the new 'optional' object is the
  //:   default allocator.
  //: 4 Repeat step 3, but use an allocator extended 'optional' constructor.
  //:   For concern 4, check the allocator of the new 'optional' object is the
  //:   allocator given to the constructor.
  //: 5 In steps 1-4, check that no unnecessary copies of the 'value_type' are
  //:   created.
  //
  // Testing:
  //
  //   optional(const TYPE&);
  //   optional(allocator_arg_t, allocator_type, const TYPE&);
  // --------------------------------------------------------------------

    if (verbose) printf(
                       "\nTESTING COPY CONSTRUCTION FROM VALUE TYPE "
                       "\n=========================================\n");

    if (veryVerbose) printf("\tUsing non allocator aware 'value_type'.\n");
    {
        typedef MyClass1                  ValueType;
        typedef bsl::optional<ValueType> Obj;

        {
            ValueType source(ValueType(1));

            int CCI = ValueType::copyConstructorInvocations;
            int MCI = ValueType::moveConstructorInvocations;
            Obj dest = source;
            ASSERT(dest.has_value());
            ASSERT(dest.value() == source);
            ASSERT(dest.value().value() == 1);
            ASSERT(CCI == ValueType::copyConstructorInvocations-1);
            ASSERT(MCI == ValueType::moveConstructorInvocations);

            const ValueType & csource = source;

            CCI = ValueType::copyConstructorInvocations;
            MCI = ValueType::moveConstructorInvocations;
            Obj dest2 = csource;
            ASSERT(dest2.has_value());
            ASSERT(dest2.value() == csource);
            ASSERT(dest2.value().value() == 1);
            ASSERT(CCI == ValueType::copyConstructorInvocations-1);
            ASSERT(MCI == ValueType::moveConstructorInvocations);
        }
    }
    if (veryVerbose) printf("\tUsing 'UsesBslmaAllocator' 'value_type'.\n");
    {
        bslma::TestAllocator da("default", veryVeryVeryVerbose);
        bslma::TestAllocator oa("other", veryVeryVeryVerbose);
        bslma::TestAllocator ta("third", veryVeryVeryVerbose);

        bslma::DefaultAllocatorGuard dag(&da);

        typedef MyClass2                  ValueType;
        typedef bsl::optional<ValueType> Obj;

        {
          ValueType source(1, &oa);

          ASSERT(1 == source.value());
          ASSERT(&oa == source.d_def.d_allocator_p);

          int CCI = ValueType::copyConstructorInvocations;
          int MCI = ValueType::moveConstructorInvocations;
          Obj dest = source;
          ASSERT(dest.has_value());
          ASSERT(dest.value() == source);
          ASSERT(dest.value().value() == 1);
          ASSERT(&da == dest.get_allocator());
          ASSERT(1 == source.value());
          ASSERT(&oa == source.d_def.d_allocator_p);
          ASSERT(CCI == ValueType::copyConstructorInvocations-1);
          ASSERT(MCI == ValueType::moveConstructorInvocations);

          const ValueType source2(2, &oa);
          CCI = ValueType::copyConstructorInvocations;
          MCI = ValueType::moveConstructorInvocations;
          Obj dest2 = source2;
          ASSERT(dest2.has_value());
          ASSERT(dest2.value() == source2);
          ASSERT(&da == dest2.get_allocator());
          ASSERT(CCI == ValueType::copyConstructorInvocations-1);
          ASSERT(MCI == ValueType::moveConstructorInvocations);

          ValueType source3(3, &oa);
          CCI = ValueType::copyConstructorInvocations;
          MCI = ValueType::moveConstructorInvocations;
          Obj dest3(bsl::allocator_arg, &ta, source3);
          ASSERT(dest3.has_value());
          ASSERT(dest3.value() == source3);
          ASSERT(dest3.value().value() == 3);
          ASSERT(&ta == dest3.get_allocator());
          ASSERT(&oa == source.d_def.d_allocator_p);
          ASSERT(CCI == ValueType::copyConstructorInvocations-1);
          ASSERT(MCI == ValueType::moveConstructorInvocations);

          const ValueType source4(4, &oa);
          CCI = ValueType::copyConstructorInvocations;
          MCI = ValueType::moveConstructorInvocations;
          Obj dest4(bsl::allocator_arg, &ta, source4);
          ASSERT(dest4.has_value());
          ASSERT(dest4.value() == source4);
          ASSERT(&ta == dest4.get_allocator());
          ASSERT(CCI == ValueType::copyConstructorInvocations-1);
          ASSERT(MCI == ValueType::moveConstructorInvocations);
       }
     }
    if (veryVerbose) printf("\tUsing 'UsesAllocatorArgT' 'value_type'.\n");
    {
        bslma::TestAllocator da("default", veryVeryVeryVerbose);
        bslma::TestAllocator oa("other", veryVeryVeryVerbose);
        bslma::TestAllocator ta("third", veryVeryVeryVerbose);

        bslma::DefaultAllocatorGuard dag(&da);

        typedef MyClass2a                  ValueType;
        typedef bsl::optional<ValueType> Obj;

        {
            ValueType source(bsl::allocator_arg, &oa, 1);

            ASSERT(1 == source.value());
            ASSERT(&oa == source.d_data.d_def.d_allocator_p);

            int CCI = ValueType::copyConstructorInvocations;
            int MCI = ValueType::moveConstructorInvocations;
            Obj dest = source;
            ASSERT(dest.has_value());
            ASSERT(dest.value() == source);
            ASSERT(dest.value().value() == 1);
            ASSERT(&da == dest.get_allocator());
            ASSERT(1 == source.value());
            ASSERT(&oa == source.d_data.d_def.d_allocator_p);
            ASSERT(CCI == ValueType::copyConstructorInvocations-1);
            ASSERT(MCI == ValueType::moveConstructorInvocations);

            const ValueType & csource = source;

            CCI = ValueType::copyConstructorInvocations;
            MCI = ValueType::moveConstructorInvocations;
            Obj dest2 = csource;
            ASSERT(dest2.has_value());
            ASSERT(dest2.value() == csource);
            ASSERT(&da == dest2.get_allocator());
            ASSERT(CCI == ValueType::copyConstructorInvocations-1);
            ASSERT(MCI == ValueType::moveConstructorInvocations);

            CCI = ValueType::copyConstructorInvocations;
            MCI = ValueType::moveConstructorInvocations;
            Obj dest3(bsl::allocator_arg, &ta, source);
            ASSERT(dest3.has_value());
            ASSERT(dest3.value() == source);
            ASSERT(dest3.value().value() == 1);
            ASSERT(&ta == dest3.get_allocator());
            ASSERT(1 == source.value());
            ASSERT(&oa == source.d_data.d_def.d_allocator_p);
            ASSERT(CCI == ValueType::copyConstructorInvocations-1);
            ASSERT(MCI == ValueType::moveConstructorInvocations);

            CCI = ValueType::copyConstructorInvocations;
            MCI = ValueType::moveConstructorInvocations;
            Obj dest4(bsl::allocator_arg, &ta, csource);
            ASSERT(dest2.has_value());
            ASSERT(dest2.value() == source);
            ASSERT(&ta == dest4.get_allocator());
            ASSERT(CCI == ValueType::copyConstructorInvocations-1);
            ASSERT(MCI == ValueType::moveConstructorInvocations);
         }
     }
}
void bslstl_optional_test20()
{
    // --------------------------------------------------------------------
    // TESTING MOVE CONSTRUCTION FROM VALUE TYPE
    //   This test will verify that the move construction from 'value_type'
    //   works as expected.
    //
    // Concerns:
    //: 1 Constructing an 'optional' from a 'value_type' object creates an
    //:   engaged 'optional' with the value of the source object.
    //: 2 Move construction from a 'value_type' object creates the 'value_type'
    //:   object by moving from the source object.
    //: 3 If no allocator is provided and the 'value_type' uses allocator, the
    //:   default allocator is used for the newly created 'optional'.
    //: 4 If allocator extended version of copy constructor is used, the
    //:   allocator passed to the constructors is the allocator of the newly
    //:   created 'optional' object.
    //: 5 No unnecessary copies of the 'value_type' are created.
    //
    // Plan:
    //
    //: 1 Create an object of non allocator aware type and use it as an
    //:   initialization object for an 'optional' of that 'value_type'. For
    //:   concern 1, check the constructed 'optional' object is engaged and
    //:   contains the value of the object used to initialise it.
    //: 2 For concern 2, check the source object has been moved from.
    //: 3 Repeat steps 1 and 2 using an allocator aware type as the
    //:   'value_type'. For concern 3, check the allocator of the new
    //:   'optional' object is the default allocator.
    //: 4 Repeat step 3, but use an allocator extended 'optional' constructor.
    //:   For concern 4, check the allocator of the new 'optional' object is
    //:   the allocator given to the constructor.
    //: 5 In steps 1-4, check that no unnecessary copies of the 'value_type'
    //:   are created.
    //
    //
    // Testing:
    //
    //   optional(MovableRef<TYPE>);
    //   optional(allocator_arg_t, allocator_type, MovableRef<TYPE>);
    //
    // --------------------------------------------------------------------

    if (verbose) printf(
                       "\nTESTING MOVE CONSTRUCTION FROM VALUE TYPE "
                       "\n=========================================\n");

    if (veryVerbose) printf("\tUsing non allocator aware 'value_type'.\n");
    {
        typedef MyClass1                  ValueType;
        typedef bsl::optional<ValueType> Obj;

        {
            ValueType source(ValueType(1));
            int CCI = ValueType::copyConstructorInvocations;
            int MCI = ValueType::moveConstructorInvocations;
            Obj dest = MovUtl::move(source);
            ASSERT(dest.has_value());
            ASSERT(dest.value().value() == 1);
            ASSERT(source.value() == MOVED_FROM_VAL);
            ASSERT(CCI == ValueType::copyConstructorInvocations);
            ASSERT(MCI == ValueType::moveConstructorInvocations-1);
        }
    }
    if (veryVerbose) printf("\tUsing 'UsesBslmaAllocator' 'value_type'.\n");
   {
        bslma::TestAllocator da("default", veryVeryVeryVerbose);
        bslma::TestAllocator oa("other", veryVeryVeryVerbose);
        bslma::TestAllocator ta("third", veryVeryVeryVerbose);

        bslma::DefaultAllocatorGuard dag(&da);

        typedef MyClass2                  ValueType;
        typedef bsl::optional<ValueType> Obj;

        {
          ValueType source(1, &oa);
          int CCI = ValueType::copyConstructorInvocations;
          int MCI = ValueType::moveConstructorInvocations;
          Obj dest = MovUtl::move(source);
          ASSERT(dest.has_value());
          ASSERT(dest.value().value() == 1);
          ASSERT(dest.get_allocator() == &da);
          ASSERT(source.value() == MOVED_FROM_VAL);
          ASSERT(CCI == ValueType::copyConstructorInvocations);
          ASSERT(MCI == ValueType::moveConstructorInvocations-1);
          ASSERT(source.d_def.d_allocator_p == &oa);

          ValueType source3(3, &oa);
          CCI = ValueType::copyConstructorInvocations;
          MCI = ValueType::moveConstructorInvocations;
          Obj dest3(bsl::allocator_arg, &ta, MovUtl::move(source3));
          ASSERT(dest3.has_value());
          ASSERT(dest3.value().value() == 3);
          ASSERT(dest3.get_allocator() == &ta);
          ASSERT(source3.value() == MOVED_FROM_VAL);
          ASSERT(CCI == ValueType::copyConstructorInvocations);
          ASSERT(MCI == ValueType::moveConstructorInvocations-1);
          ASSERT(source3.d_def.d_allocator_p == &oa);
       }
    }
    if (veryVerbose) printf("\tUsing 'UsesAllocatorArgT' 'value_type'.\n");
    {
        bslma::TestAllocator da("default", veryVeryVeryVerbose);
        bslma::TestAllocator oa("other", veryVeryVeryVerbose);
        bslma::TestAllocator ta("third", veryVeryVeryVerbose);

        bslma::DefaultAllocatorGuard dag(&da);

        typedef MyClass2a                  ValueType;
        typedef bsl::optional<ValueType> Obj;

        {
          ValueType source(bsl::allocator_arg, &oa, 1);

          int CCI = ValueType::copyConstructorInvocations;
          int MCI = ValueType::moveConstructorInvocations;
          Obj dest = MovUtl::move(source);
          ASSERT(dest.has_value());
          ASSERT(dest.value().value() == 1);
          ASSERT(dest.get_allocator() == &da);
          ASSERT(source.value() == MOVED_FROM_VAL);
          ASSERT(CCI == ValueType::copyConstructorInvocations);
          ASSERT(MCI == ValueType::moveConstructorInvocations-1);
          ASSERT(source.d_data.d_def.d_allocator_p == &oa);

          ValueType source3(bsl::allocator_arg, &oa, 3);
          CCI = ValueType::copyConstructorInvocations;
          MCI = ValueType::moveConstructorInvocations;
          Obj dest3(bsl::allocator_arg, &ta, MovUtl::move(source3));
          ASSERT(dest3.has_value());
          ASSERT(dest3.value().value() == 3);
          ASSERT(dest3.get_allocator() == &ta);
          ASSERT(source3.value() == MOVED_FROM_VAL);
          ASSERT(CCI == ValueType::copyConstructorInvocations);
          ASSERT(MCI == ValueType::moveConstructorInvocations-1);
          ASSERT(source3.d_data.d_def.d_allocator_p == &oa);
       }
    }
}
void bslstl_optional_test21()
{
    // --------------------------------------------------------------------
    // TESTING COPY CONVERSION FROM OPTIONAL TYPE
    //   This test will verify that the copy conversion from an 'optional'
    //   of different 'value_type' works as expected.
    //
    // Concerns:
    //: 1 Constructing an 'optional' from an engaged 'optional' of a different
    //:   type creates an engaged 'optional'.The 'value_type' object of the new
    //:   'optional' object is created by copy conversion from 'value_type'
    //:   object from the source 'optional' object.
    //: 2 Constructing an 'optional' from a disengaged 'optional' of a
    //:   different type creates a disengaged 'optional'.
    //: 3 Copy conversion does not modify the source object.
    //: 4 If no allocator is provided and the 'value_type' uses allocator, the
    //:   default allocator is used for the newly created 'optional'.
    //: 5 If allocator extended version of copy conversion is used, the
    //:   allocator passed to the constructors is the allocator of the newly
    //:   created 'optional' object.
    //: 6 No unnecessary copies of the 'value_type' are created.
    //
    //
    // Plan:
    //: 1 Initialize an 'optional' object of a non allocator aware type 'TYPE'
    //:   with an engaged 'optional' of a 'value_type' convertable to 'TYPE'.
    //:   For concern 1, check the newly create 'optional' is engaged, and that
    //:   its value is of expected value.
    //: 2 Initialize an 'optional' object of a non allocator aware type 'TYPE'
    //:   with a disengaged 'optional' of a 'value_type' convertable to 'TYPE'.
    //:   For concern 2, check the newly create 'optional' is disengaged.
    //: 3 In step 1, for concern 3, use const reference to the source value
    //:   object to initialize the destination 'optional' object
    //: 4 Repeat step 1-3 using an allocator aware 'TYPE'. For concern 4, check
    //:   that the allocator of the newly created 'optional' is the default
    //:   allocator
    //: 5 In step 4, use an allocator extended conversion constructor. For
    //:   concern 5, check the allocator of the newly create 'optional' is
    //:   the one passed into the constructor.
    //: 6 In step 1-5, check no unnecessary copies of the 'value_type' are
    //:   created.
    //
    // Testing:
    //
    //   optional(const optional<ANY_TYPE>& )
    //   optional(bsl::allocator_arg, allocator_type,
    //            const optional<ANY_TYPE>&)
    //
    // --------------------------------------------------------------------

    if (verbose) printf(
                       "\nTESTING COPY CONVERSION FROM OPTIONAL TYPE"
                       "\n==========================================\n");

    if (veryVerbose) printf("\tUsing non allocator aware type.\n");
    {
        typedef MyClass1                   SourceType;
        typedef MyClass1a                  ValueType;
        typedef bsl::optional<SourceType> SrcObj;
        typedef bsl::optional<ValueType> Obj;

        {
            SrcObj source(SourceType(1));
            ASSERT(source.has_value());

            int CCI = ValueType::copyConstructorInvocations;
            int MCI = ValueType::moveConstructorInvocations;
            Obj dest = source;
            ASSERT(dest.has_value());
            ASSERT(dest.value().value() == 1);
            ASSERT(source.has_value());
            ASSERT(source.value().value() == 1);
            ASSERT(CCI == ValueType::copyConstructorInvocations);
            ASSERT(MCI == ValueType::moveConstructorInvocations);

            const SrcObj source2(SourceType(2));;
            CCI = ValueType::copyConstructorInvocations;
            MCI = ValueType::moveConstructorInvocations;
            Obj dest2 = source2;
            ASSERT(dest2.has_value());
            ASSERT(dest2.value().value() == 2);
            ASSERT(CCI == ValueType::copyConstructorInvocations);
            ASSERT(MCI == ValueType::moveConstructorInvocations);
        }
        {
            SrcObj source(nullopt);
            ASSERT(!source.has_value());

            int CCI = ValueType::copyConstructorInvocations;
            int MCI = ValueType::moveConstructorInvocations;
            Obj dest = source;
            ASSERT(!dest.has_value());
            ASSERT(!source.has_value());
            ASSERT(CCI == ValueType::copyConstructorInvocations);
            ASSERT(MCI == ValueType::moveConstructorInvocations);

            const SrcObj & csource = source;
            ASSERT(!csource.has_value());

            CCI = ValueType::copyConstructorInvocations;
            MCI = ValueType::moveConstructorInvocations;
            Obj dest2 = csource;
            ASSERT(!dest2.has_value());
            ASSERT(CCI == ValueType::copyConstructorInvocations);
            ASSERT(MCI == ValueType::moveConstructorInvocations);
        }
    }
    if (veryVerbose) printf("\tUsing 'UsesBslmaAllocator' 'value_type'.\n");
    {
        bslma::TestAllocator da("default", veryVeryVeryVerbose);
        bslma::TestAllocator oa("other", veryVeryVeryVerbose);
        bslma::TestAllocator ta("third", veryVeryVeryVerbose);

        bslma::DefaultAllocatorGuard dag(&da);

        typedef MyClass1                   SourceType;
        typedef MyClass2                   ValueType;
        typedef bsl::optional<SourceType>   SrcObj;
        typedef bsl::optional<ValueType>    Obj;

        {
          SrcObj source(SourceType(1));
          ASSERT(source.has_value());

          int CCI = ValueType::copyConstructorInvocations;
          int MCI = ValueType::moveConstructorInvocations;
          Obj dest = source;
          ASSERT(dest.has_value());
          ASSERT(dest.value().value() == 1);
          ASSERT(dest.get_allocator() == &da);
          ASSERT(source.has_value());
          ASSERT(source.value().value() == 1);
          ASSERT(CCI == ValueType::copyConstructorInvocations);
          ASSERT(MCI == ValueType::moveConstructorInvocations);

          const SrcObj source2(SourceType(2));
          CCI = ValueType::copyConstructorInvocations;
          MCI = ValueType::moveConstructorInvocations;
          Obj dest2 = source2;
          ASSERT(dest2.has_value());
          ASSERT(dest2.value().value() == 2);
          ASSERT(dest2.value().d_def.d_allocator_p == &da);
          ASSERT(dest2.get_allocator() == &da);
          ASSERT(CCI == ValueType::copyConstructorInvocations);
          ASSERT(MCI == ValueType::moveConstructorInvocations);


          SrcObj source3(SourceType(3));
          CCI = ValueType::copyConstructorInvocations;
          MCI = ValueType::moveConstructorInvocations;
          Obj dest3(bsl::allocator_arg, &ta, source3);
          ASSERT(dest3.has_value());
          ASSERT(dest3.value().value() == 3);
          ASSERT(dest3.value().d_def.d_allocator_p == &ta);
          ASSERT(dest3.get_allocator() == &ta);
          ASSERT(source3.has_value());
          ASSERT(source3.value().value() == 3);
          ASSERT(CCI == ValueType::copyConstructorInvocations);
          ASSERT(MCI == ValueType::moveConstructorInvocations);

          const SrcObj source4(SourceType(4));
          CCI = ValueType::copyConstructorInvocations;
          MCI = ValueType::moveConstructorInvocations;
          Obj dest4(bsl::allocator_arg, &ta, source4);
          ASSERT(dest4.has_value());
          ASSERT(dest4.value().value() == 4);
          ASSERT(dest4.value().d_def.d_allocator_p == &ta);
          ASSERT(dest4.get_allocator() == &ta);
          ASSERT(CCI == ValueType::copyConstructorInvocations);
          ASSERT(MCI == ValueType::moveConstructorInvocations);
       }
       {
          SrcObj source(nullopt);
          ASSERT(!source.has_value());

          Obj dest = source;
          ASSERT(!dest.has_value());
          ASSERT(dest.get_allocator() == &da);
          ASSERT(!source.has_value());

          const Obj & csource = source;
          ASSERT(!csource.has_value());

          Obj dest2 = csource;
          ASSERT(!dest2.has_value());
          ASSERT(dest2.get_allocator() == &da);

          Obj dest3(bsl::allocator_arg, &ta, source);
          ASSERT(!dest3.has_value());
          ASSERT(dest3.get_allocator() == &ta);

          Obj dest4(bsl::allocator_arg, &ta, csource);
          ASSERT(!dest4.has_value());
          ASSERT(dest4.get_allocator() == &ta);
       }
    }
    if (veryVerbose) printf("\tUsing 'UsesAllocatorArgT' 'value_type'.\n");
    {
        bslma::TestAllocator da("default", veryVeryVeryVerbose);
        bslma::TestAllocator oa("other", veryVeryVeryVerbose);
        bslma::TestAllocator ta("third", veryVeryVeryVerbose);

        bslma::DefaultAllocatorGuard dag(&da);

        typedef MyClass2                   SourceType;
        typedef MyClass2a                  ValueType;
        typedef bsl::optional<SourceType>   SrcObj;
        typedef bsl::optional<ValueType>    Obj;

        {
            SrcObj source(bsl::allocator_arg, &oa, SourceType(1));
            ASSERT(source.has_value());
            ASSERT(&oa == source.get_allocator());

            int CCI = ValueType::copyConstructorInvocations;
            int MCI = ValueType::moveConstructorInvocations;
            Obj dest = source;
            ASSERT(dest.has_value());
            ASSERT(dest.value() == source.value());
            ASSERT(dest.value().value() == 1);
            ASSERT(&da == dest.get_allocator());
            ASSERT(source.has_value());
            ASSERT(&oa == source.get_allocator());
            ASSERT(CCI == ValueType::copyConstructorInvocations);
            ASSERT(MCI == ValueType::moveConstructorInvocations);

            const SrcObj & csource = source;
            ASSERT(csource.has_value());

            CCI = ValueType::copyConstructorInvocations;
            MCI = ValueType::moveConstructorInvocations;
            Obj dest2 = csource;
            ASSERT(dest2.has_value());
            ASSERT(dest2.value() == csource.value());
            ASSERT(&da == dest2.get_allocator());
            ASSERT(CCI == ValueType::copyConstructorInvocations);
            ASSERT(MCI == ValueType::moveConstructorInvocations);

            CCI = ValueType::copyConstructorInvocations;
            MCI = ValueType::moveConstructorInvocations;
            Obj dest3(bsl::allocator_arg, &ta, source);
            ASSERT(dest3.has_value());
            ASSERT(dest3.value() == source.value());
            ASSERT(dest3.value().value() == 1);
            ASSERT(&ta == dest3.get_allocator());
            ASSERT(source.has_value());
            ASSERT(&oa == source.get_allocator());
            ASSERT(CCI == ValueType::copyConstructorInvocations);
            ASSERT(MCI == ValueType::moveConstructorInvocations);

            CCI = ValueType::copyConstructorInvocations;
            MCI = ValueType::moveConstructorInvocations;
            Obj dest4(bsl::allocator_arg, &ta, csource);
            ASSERT(dest2.has_value());
            ASSERT(dest2.value() == source.value());
            ASSERT(&ta == dest4.get_allocator());
            ASSERT(CCI == ValueType::copyConstructorInvocations);
            ASSERT(MCI == ValueType::moveConstructorInvocations);
        }
        {
            SrcObj source(bsl::allocator_arg, &oa, nullopt);
            ASSERT(!source.has_value());
            ASSERT(&oa == source.get_allocator());

            int CCI = ValueType::copyConstructorInvocations;
            int MCI = ValueType::moveConstructorInvocations;
            Obj dest = source;
            ASSERT(!dest.has_value());
            ASSERT(&da == dest.get_allocator());
            ASSERT(!source.has_value());
            ASSERT(CCI == ValueType::copyConstructorInvocations);
            ASSERT(MCI == ValueType::moveConstructorInvocations);

            const SrcObj & csource = source;
            ASSERT(!csource.has_value());

            CCI = ValueType::copyConstructorInvocations;
            MCI = ValueType::moveConstructorInvocations;
            Obj dest2 = csource;
            ASSERT(!dest2.has_value());
            ASSERT(&da == dest2.get_allocator());
            ASSERT(CCI == ValueType::copyConstructorInvocations);
            ASSERT(MCI == ValueType::moveConstructorInvocations);

            CCI = ValueType::copyConstructorInvocations;
            MCI = ValueType::moveConstructorInvocations;
            Obj dest3(bsl::allocator_arg, &ta, source);
            ASSERT(!dest3.has_value());
            ASSERT(&ta == dest3.get_allocator());
            ASSERT(CCI == ValueType::copyConstructorInvocations);
            ASSERT(MCI == ValueType::moveConstructorInvocations);

            CCI = ValueType::copyConstructorInvocations;
            MCI = ValueType::moveConstructorInvocations;
            Obj dest4(bsl::allocator_arg, &ta, csource);
            ASSERT(!dest4.has_value());
            ASSERT(&ta == dest4.get_allocator());
            ASSERT(CCI == ValueType::copyConstructorInvocations);
            ASSERT(MCI == ValueType::moveConstructorInvocations);
      }
   }
}
void bslstl_optional_test22()
{
    // --------------------------------------------------------------------
    // TESTING MOVE CONVERSION FROM OPTIONAL TYPE
    //   This test will verify that the move construction from an 'optional' of
    //   different 'value_type' works as expected.
    //
    // Concerns:
    //: 1 Constructing an 'optional' from an rvalue engaged 'optional' of a
    //:   different type creates an engaged 'optional'. The 'value_type' object
    //:   of the new 'optional' object is created by move conversion from value
    //:   type object from the source 'optional' object.
    //: 2 Constructing an 'optional' from a disengaged 'optional' of a
    //:   different type creates a disengaged 'optional'.
    //: 4 If no allocator is provided and the 'value_type' uses allocator, the
    //:   default allocator is used for the newly created 'optional'.
    //: 5 If allocator extended version of copy conversion is used, the
    //:   allocator passed to the constructors is the allocator of the newly
    //:   created 'optional' object.
    //: 6 No unnecessary copies of the 'value_type' are created.
    //
    //
    // Plan:
    //: 1 Initialize an 'optional' object of a non allocator aware type 'TYPE'
    //:   with an engaged 'optional' of a 'value_type' convertable to 'TYPE'.
    //:   For concern 1, check the newly create 'optional' is engaged, that its
    //:   value is of expected value, and that the source object's 'value_type'
    //:   has been moved from.
    //: 2 Initialize an 'optional' object of a non allocator aware type 'TYPE'
    //:   with a disengaged 'optional' of a 'value_type' convertable to 'TYPE'.
    //:   For concern 2, check the newly create 'optional' is disengaged.
    //: 4 Repeat step 1-3 using an allocator aware 'TYPE'. For concern 4, check
    //:   that the allocator of the newly created 'optional' is the default
    //:   allocator
    //: 5 In step 4, use an allocator extended conversion constructor. For
    //:   concern 5, check the allocator of the newly create 'optional' is
    //:   the one passed into the constructor.
    //: 6 In step 1-5, check no unnecessary copies of the 'value_type' are
    //:   created.
    //
    //
    // Testing:
    //
    //   optional(MovableRef<optional <ANY_TYPE> >)
    //   optional(bsl::allocator_arg,
    //            allocator_type,
    //            MovableRef<optional <ANY_TYPE> >)
    //
    // --------------------------------------------------------------------

    if (verbose) printf(
                       "\nTESTING MOVE CONVERSION FROM OPTIONAL TYPE"
                       "\n==========================================\n");

    if (veryVerbose) printf("\tUsing a non allocator aware type.\n");
    {
        typedef MyClass1                  SourceType;
        typedef MyClass1a                  ValueType;
        typedef bsl::optional<SourceType> SrcObj;
        typedef bsl::optional<ValueType> Obj;

        {
            SrcObj source(SourceType(1));
            ASSERT(source.has_value());

            int CCI = ValueType::copyConstructorInvocations;
            int MCI = ValueType::moveConstructorInvocations;
            Obj dest = MovUtl::move(source);
            ASSERT(dest.has_value());
            ASSERT(dest.value().value() == 1);
            ASSERT(source.value().value() == MOVED_FROM_VAL);
            ASSERT(CCI == ValueType::copyConstructorInvocations);
            ASSERT(MCI == ValueType::moveConstructorInvocations);
        }
        {
            SrcObj source(nullopt);
            ASSERT(!source.has_value());

            int CCI = ValueType::copyConstructorInvocations;
            int MCI = ValueType::moveConstructorInvocations;
            Obj dest = MovUtl::move(source);
            ASSERT(!dest.has_value());
            ASSERT(!source.has_value());
            ASSERT(CCI == ValueType::copyConstructorInvocations);
            ASSERT(MCI == ValueType::moveConstructorInvocations);
        }
    }
    if (veryVerbose) printf("\tUsing 'UsesBslmaAllocator' 'value_type'.\n");
    {
        bslma::TestAllocator da("default", veryVeryVeryVerbose);
        bslma::TestAllocator oa("other", veryVeryVeryVerbose);
        bslma::TestAllocator ta("third", veryVeryVeryVerbose);

        bslma::DefaultAllocatorGuard dag(&da);

        typedef MyClass1                   SourceType;
        typedef MyClass2                   ValueType;
        typedef bsl::optional<SourceType>   SrcObj;
        typedef bsl::optional<ValueType>    Obj;

        {
          SrcObj source(SourceType(1));
          ASSERT(source.has_value());

          int CCI = ValueType::copyConstructorInvocations;
          int MCI = ValueType::moveConstructorInvocations;
          Obj dest = MovUtl::move(source);
          ASSERT(dest.has_value());
          ASSERT(dest.value().value() == 1);
          ASSERT(dest.get_allocator() == &da);
          ASSERT(source.value().value() == MOVED_FROM_VAL);
          ASSERT(CCI == ValueType::copyConstructorInvocations);
          ASSERT(MCI == ValueType::moveConstructorInvocations);

          SrcObj source3(SourceType(3));
          CCI = ValueType::copyConstructorInvocations;
          MCI = ValueType::moveConstructorInvocations;
          Obj dest3(bsl::allocator_arg, &ta, MovUtl::move(source3));
          ASSERT(dest3.has_value());
          ASSERT(dest3.value().value() == 3);
          ASSERT(dest3.value().d_def.d_allocator_p == &ta);
          ASSERT(dest3.get_allocator() == &ta);
          ASSERT(source3.value().value() == MOVED_FROM_VAL);
          ASSERT(CCI == ValueType::copyConstructorInvocations);
          ASSERT(MCI == ValueType::moveConstructorInvocations);
       }
       {
          SrcObj source(nullopt);
          ASSERT(!source.has_value());

          int CCI = ValueType::copyConstructorInvocations;
          int MCI = ValueType::moveConstructorInvocations;
          Obj dest = MovUtl::move(source);
          ASSERT(!dest.has_value());
          ASSERT(dest.get_allocator() == &da);
          ASSERT(!source.has_value());
          ASSERT(CCI == ValueType::copyConstructorInvocations);
          ASSERT(MCI == ValueType::moveConstructorInvocations);

          SrcObj source3(nullopt);
          CCI = ValueType::copyConstructorInvocations;
          MCI = ValueType::moveConstructorInvocations;
          Obj dest3(bsl::allocator_arg, &ta, MovUtl::move(source3));
          ASSERT(!dest3.has_value());
          ASSERT(dest3.get_allocator() == &ta);
          ASSERT(CCI == ValueType::copyConstructorInvocations);
          ASSERT(MCI == ValueType::moveConstructorInvocations);
       }
    }
    if (veryVerbose) printf("\tUsing 'UsesAllocatorArgT' 'value_type'.\n");
    {
        bslma::TestAllocator da("default", veryVeryVeryVerbose);
        bslma::TestAllocator oa("other", veryVeryVeryVerbose);
        bslma::TestAllocator ta("third", veryVeryVeryVerbose);

        bslma::DefaultAllocatorGuard dag(&da);

        typedef MyClass2                   SourceType;
        typedef MyClass2a                  ValueType;
        typedef bsl::optional<SourceType>   SrcObj;
        typedef bsl::optional<ValueType>    Obj;

        {
            SrcObj source(bsl::allocator_arg, &oa, SourceType(1));
            int CCI = ValueType::copyConstructorInvocations;
            int MCI = ValueType::moveConstructorInvocations;
            Obj dest = MovUtl::move(source);
            ASSERT(dest.has_value());
            ASSERT(dest.value().value() == 1);
            ASSERT(&da == dest.get_allocator());
            ASSERT(source.value().value() == MOVED_FROM_VAL);
            ASSERT(CCI == ValueType::copyConstructorInvocations);
            ASSERT(MCI == ValueType::moveConstructorInvocations);

            SrcObj source3(bsl::allocator_arg, &oa, SourceType(3));
            CCI = ValueType::copyConstructorInvocations;
            MCI = ValueType::moveConstructorInvocations;
            Obj dest3(bsl::allocator_arg, &ta, MovUtl::move(source3));
            ASSERT(dest3.has_value());
            ASSERT(dest3.value().value() == 3);
            ASSERT(&ta == dest3.get_allocator());
            ASSERT(source3.value().value() == MOVED_FROM_VAL);
            ASSERT(CCI == ValueType::copyConstructorInvocations);
            ASSERT(MCI == ValueType::moveConstructorInvocations);
        }
        {
            SrcObj source(bsl::allocator_arg, &oa, nullopt);
            Obj dest = MovUtl::move(source);
            ASSERT(&da == dest.get_allocator());
            ASSERT(!source.has_value());

            SrcObj source3(bsl::allocator_arg, &oa, nullopt);
            Obj dest3(bsl::allocator_arg, &ta, MovUtl::move(source3));
            ASSERT(!dest3.has_value());
            ASSERT(&ta == dest3.get_allocator());
      }
   }
}
void bslstl_optional_test23()
{
    // --------------------------------------------------------------------
    // TESTING COPY CONVERSION FROM NON OPTIONAL TYPE
    //   This test will verify that the copy conversion from non 'optional'
    //   type works as expected.
    //
    // Concerns:
    //: 1 Constructing an 'optional' of type TYPE from an object of type
    //:   convertible to TYPE creates and engaged 'optional with the
    //:   'value_type' object created by copy conversion from the source
    //:   object.
    //: 2 When constructing an 'optional' of type TYPE from an object of type
    //:   convertible to TYPE, source object is not modified.
    //: 3 If no allocator is provided and the 'value_type' uses allocator, the
    //:   default allocator is used for the newly created 'optional'.
    //: 4 If allocator extended version of copy conversion constructor is used,
    //:   the specified allocator is used for the new object.
    //: 5 No unnecessary instances of TYPE are created.
    //
    //
    // Plan:
    //: 1 Create an 'optional' of non allocator aware type 'TYPE' by conversion
    //:   from an object of a type convertible to TYPE. For concern 1, check
    //:   that the 'optional' object is engaged, and that it contains the
    //:   expected value.
    //: 2 In step 1, for concern 2, create the 'optional' object by conversion
    //:   from a const reference to an object of a type convertible to TYPE.
    //: 3 Repeat steps 1 and 2 using an allocator aware TYPE. For concern 3,
    //:   verify the allocator of the 'optional' object is the default
    //:   allocator.
    //: 4 Repeat steps 3, but use an allocator extended 'optional' constructor.
    //:   For concern 4, verify the allocator of the 'optional' object is the
    //:   allocator used in the constructor call.
    //: 5 For concern 5, in steps 1-4, check no unnecessary instances of 'TYPE'
    //:   have been created.
    //
    // Testing:
    //
    //   optional(const ANY_TYPE&);
    //   optional(allocator_arg_t, allocator_type, const ANY_TYPE&);
    //
    // --------------------------------------------------------------------

    if (verbose) printf(
                       "\nTESTING COPY CONVERSION FROM NON OPTIONAL TYPE"
                       "\n==============================================\n");

    if (veryVerbose) printf("\tUsing non allocator aware 'value_type'.\n");
    {
        typedef MyClass1                   SourceType;
        typedef MyClass1a                  ValueType;
        typedef bsl::optional<ValueType> Obj;

        {
            SourceType source(1);

            int CCI = ValueType::copyConstructorInvocations;
            int MCI = ValueType::moveConstructorInvocations;
            Obj dest = source;
            ASSERT(dest.has_value());
            ASSERT(dest.value() == source);
            ASSERT(dest.value().value() == 1);
            ASSERT(CCI == ValueType::copyConstructorInvocations);
            ASSERT(MCI == ValueType::moveConstructorInvocations);

            const SourceType & csource = source;
            CCI = ValueType::copyConstructorInvocations;
            MCI = ValueType::moveConstructorInvocations;
            Obj dest2 = csource;
            ASSERT(dest2.has_value());
            ASSERT(dest2.value() == csource);
            ASSERT(dest2.value().value() == 1);
            ASSERT(CCI == ValueType::copyConstructorInvocations);
            ASSERT(MCI == ValueType::moveConstructorInvocations);
        }
    }
    if (veryVerbose) printf("\tUsing 'UsesBslmaAllocator' 'value_type'.\n");
    {
        bslma::TestAllocator da("default", veryVeryVeryVerbose);
        bslma::TestAllocator oa("other", veryVeryVeryVerbose);
        bslma::TestAllocator ta("third", veryVeryVeryVerbose);

        bslma::DefaultAllocatorGuard dag(&da);

        typedef MyClass1                   SourceType;
        typedef MyClass2                   ValueType;
        typedef bsl::optional<ValueType> Obj;

        {
            SourceType source(1);
            int CCI = ValueType::copyConstructorInvocations;
            int MCI = ValueType::moveConstructorInvocations;
            Obj dest = source;
            ASSERT(dest.has_value());
            ASSERT(dest.value() == source);
            ASSERT(dest.value().value() == 1);
            ASSERT(&da == dest.get_allocator());
            ASSERT(CCI == ValueType::copyConstructorInvocations);
            ASSERT(MCI == ValueType::moveConstructorInvocations);

            const SourceType source2(2);
            CCI = ValueType::copyConstructorInvocations;
            MCI = ValueType::moveConstructorInvocations;
            Obj dest2 = source2;
            ASSERT(dest2.has_value());
            ASSERT(dest2.value() == source2);
            ASSERT(&da == dest2.get_allocator());
            ASSERT(CCI == ValueType::copyConstructorInvocations);
            ASSERT(MCI == ValueType::moveConstructorInvocations);

            const SourceType source3(3);
            CCI = ValueType::copyConstructorInvocations;
            MCI = ValueType::moveConstructorInvocations;
            Obj dest3(bsl::allocator_arg, &ta, source3);
            ASSERT(dest3.has_value());
            ASSERT(dest3.value() == source3);
            ASSERT(dest3.value().value() == 3);
            ASSERT(&ta == dest3.get_allocator());
            ASSERT(CCI == ValueType::copyConstructorInvocations);
            ASSERT(MCI == ValueType::moveConstructorInvocations);

            const SourceType source4(4);
            CCI = ValueType::copyConstructorInvocations;
            MCI = ValueType::moveConstructorInvocations;
            Obj dest4(bsl::allocator_arg, &ta, source4);
            ASSERT(dest4.has_value());
            ASSERT(dest4.value() == source4);
            ASSERT(&ta == dest4.get_allocator());
            ASSERT(CCI == ValueType::copyConstructorInvocations);
            ASSERT(MCI == ValueType::moveConstructorInvocations);
        }
    }
    if (veryVerbose) printf("\tUsing 'UsesAllocatorArgT' 'value_type'.\n");
    {
        bslma::TestAllocator da("default", veryVeryVeryVerbose);
        bslma::TestAllocator oa("other", veryVeryVeryVerbose);
        bslma::TestAllocator ta("third", veryVeryVeryVerbose);

        bslma::DefaultAllocatorGuard dag(&da);

        typedef MyClass2                   SourceType;
        typedef MyClass2a                  ValueType;
        typedef bsl::optional<ValueType> Obj;

        {
            SourceType source(1, &oa);
            ASSERT(1 == source.value());
            ASSERT(&oa == source.d_def.d_allocator_p);

            int CCI = ValueType::copyConstructorInvocations;
            int MCI = ValueType::moveConstructorInvocations;
            Obj dest = source;
            ASSERT(dest.has_value());
            ASSERT(dest.value().value() == 1);
            ASSERT(&da == dest.get_allocator());
            ASSERT(1 == source.value());
            ASSERT(&oa == source.d_def.d_allocator_p);
            ASSERT(CCI == ValueType::copyConstructorInvocations);
            ASSERT(MCI == ValueType::moveConstructorInvocations);

            const SourceType source2(2, &oa);
            CCI = ValueType::copyConstructorInvocations;
            MCI = ValueType::moveConstructorInvocations;
            Obj dest2 = source2;
            ASSERT(dest2.has_value());
            ASSERT(dest2.value().value() == 2);
            ASSERT(&da == dest2.get_allocator());
            ASSERT(CCI == ValueType::copyConstructorInvocations);
            ASSERT(MCI == ValueType::moveConstructorInvocations);

            SourceType source3(3, &oa);
            CCI = ValueType::copyConstructorInvocations;
            MCI = ValueType::moveConstructorInvocations;
            Obj dest3(bsl::allocator_arg, &ta, source3);
            ASSERT(dest3.has_value());
            ASSERT(dest3.value().value() == 3);
            ASSERT(&ta == dest3.get_allocator());
            ASSERT(3 == source3.value());
            ASSERT(&oa == source3.d_def.d_allocator_p);
            ASSERT(CCI == ValueType::copyConstructorInvocations);
            ASSERT(MCI == ValueType::moveConstructorInvocations);

            const SourceType source4(4, &oa);
            CCI = ValueType::copyConstructorInvocations;
            MCI = ValueType::moveConstructorInvocations;
            Obj dest4(bsl::allocator_arg, &ta, source4);
            ASSERT(dest4.has_value());
            ASSERT(dest4.value().value() == 4);
            ASSERT(&ta == dest4.get_allocator());
            ASSERT(CCI == ValueType::copyConstructorInvocations);
            ASSERT(MCI == ValueType::moveConstructorInvocations);
         }
     }
}
void bslstl_optional_test24()
{
    // --------------------------------------------------------------------
    // TESTING MOVE CONVERSION FROM NON OPTIONAL TYPE
    //   This test will verify that the move construction from OTHER type
    //   works as expected.
    //
    // Concerns:
    //: 1 Constructing an 'optional' of type TYPE from an rvalue of type
    //:   convertible to TYPE creates and engaged 'optional with the
    //:   'value_type' object of the same value as the source object.
    //: 2 When constructing an 'optional' of type TYPE from an rvalue of type
    //:   convertible to TYPE, source object is moved from.
    //: 3 If no allocator is provided and the 'value_type' uses allocator, the
    //:   default allocator is used for the newly created 'optional'.
    //: 4 If allocator extended version of copy conversion constructor is used,
    //:   the specified allocator is used for the new object.
    //: 5 No unnecessary instances of TYPE are created.
    //
    //
    // Plan:
    //: 1 Create an 'optional' of non allocator aware type 'TYPE' by conversion
    //:   from an rvalue of a type convertible to TYPE. For concern 1, check
    //:   that the 'optional' object is engaged, and that it contains the
    //:   expected value.
    //: 2 In step 1, for concern 2, check the source is in a moved from state.
    //: 3 Repeat steps 1 and 2 using an allocator aware TYPE. For concern 3,
    //:   verify the allocator of the 'optional' object is the default
    //:   allocator.
    //: 4 Repeat steps 3, but use an allocator extended 'optional' constructor.
    //:   For concern 4, verify the allocator of the 'optional' object is the
    //:   allocator used in the constructor call.
    //: 5 For concern 5, in steps 1-4, check no unnecessary instances of 'TYPE'
    //:   have been created.
    //
    //
    // Testing:
    //
    //   optional(MovableRef<TYPE>);
    //   optional(allocator_arg_t, allocator_type, MovableRef<TYPE>);
    //
    // --------------------------------------------------------------------
    if (verbose) printf(
                       "\nTESTING MOVE CONVERSION FROM NON OPTIONAL TYPE "
                       "\n=========================================\n");

    if (veryVerbose) printf("\tUsing non allocator aware 'value_type'.\n");
    {
        typedef MyClass1                   SourceType;
        typedef MyClass1a                  ValueType;
        typedef bsl::optional<ValueType>    Obj;

        {
            SourceType source(1);
            int CCI = ValueType::copyConstructorInvocations;
            int MCI = ValueType::moveConstructorInvocations;
            Obj dest = MovUtl::move(source);
            ASSERT(dest.has_value());
            ASSERT(dest.value().value() == 1);
            ASSERT(source.value() == MOVED_FROM_VAL);
            ASSERT(CCI == ValueType::copyConstructorInvocations);
            ASSERT(MCI == ValueType::moveConstructorInvocations);
        }
    }
    if (veryVerbose) printf("\tUsing 'UsesBslmaAllocator' 'value_type'.\n");
    {
        bslma::TestAllocator da("default", veryVeryVeryVerbose);
        bslma::TestAllocator oa("other", veryVeryVeryVerbose);
        bslma::TestAllocator ta("third", veryVeryVeryVerbose);

        bslma::DefaultAllocatorGuard dag(&da);

        typedef MyClass2                   SourceType;
        typedef MyClass2a                  ValueType;
        typedef bsl::optional<ValueType>    Obj;

        {
          SourceType source(1, &oa);
          int CCI = ValueType::copyConstructorInvocations;
          int MCI = ValueType::moveConstructorInvocations;
          Obj dest = MovUtl::move(source);
          ASSERT(dest.has_value());
          ASSERT(dest.value().value() == 1);
          ASSERT(dest.get_allocator() == &da);
          ASSERT(source.value() == MOVED_FROM_VAL);
          ASSERT(source.d_def.d_allocator_p == &oa);
          ASSERT(CCI == ValueType::copyConstructorInvocations);
          ASSERT(MCI == ValueType::moveConstructorInvocations);

          SourceType source3(3, &oa);
          CCI = ValueType::copyConstructorInvocations;
          MCI = ValueType::moveConstructorInvocations;
          Obj dest3(bsl::allocator_arg, &ta, MovUtl::move(source3));
          ASSERT(dest3.has_value());
          ASSERT(dest3.value().value() == 3);
          ASSERT(dest3.get_allocator() == &ta);
          ASSERT(source3.value() == MOVED_FROM_VAL);
          ASSERT(source3.d_def.d_allocator_p == &oa);
          ASSERT(CCI == ValueType::copyConstructorInvocations);
          ASSERT(MCI == ValueType::moveConstructorInvocations);
       }
    }
    if (veryVerbose) printf("\tUsing 'UsesAllocatorArgT' 'value_type'.\n");
    {
        bslma::TestAllocator da("default", veryVeryVeryVerbose);
        bslma::TestAllocator oa("other", veryVeryVeryVerbose);
        bslma::TestAllocator ta("third", veryVeryVeryVerbose);

        bslma::DefaultAllocatorGuard dag(&da);

        typedef MyClass2                   SourceType;
        typedef MyClass2a                  ValueType;
        typedef bsl::optional<ValueType> Obj;

        {
          SourceType source(1, &oa);
          int CCI = ValueType::copyConstructorInvocations;
          int MCI = ValueType::moveConstructorInvocations;
          Obj dest = MovUtl::move(source);
          ASSERT(dest.has_value());
          ASSERT(dest.value().value() == 1);
          ASSERT(dest.get_allocator() == &da);
          ASSERT(source.value() == MOVED_FROM_VAL);
          ASSERT(source.d_def.d_allocator_p == &oa);
          ASSERT(CCI == ValueType::copyConstructorInvocations);
          ASSERT(MCI == ValueType::moveConstructorInvocations);

          SourceType source3(3, &oa);
          CCI = ValueType::copyConstructorInvocations;
          MCI = ValueType::moveConstructorInvocations;
          Obj dest3(bsl::allocator_arg, &ta, MovUtl::move(source3));
          ASSERT(dest3.has_value());
          ASSERT(dest3.value().value() == 3);
          ASSERT(dest3.get_allocator() == &ta);
          ASSERT(source3.value() == MOVED_FROM_VAL);
          ASSERT(source3.d_def.d_allocator_p == &oa);
          ASSERT(CCI == ValueType::copyConstructorInvocations);
          ASSERT(MCI == ValueType::moveConstructorInvocations);
        }
    }
}
// helper function that invokes 'TEST_COPY' macro for various number of
// constructor variadic arguments, and for a combination of lvalues and rvalue
// such that each argument position is tested for perfect forwarding.
template <typename VALTYPE, typename OPTYPE>
void test_copy_helper()
{
    TEST_COPY(VALTYPE, OPTYPE, (bsl::in_place),
                 /* no ctor arg list */
                );

    TEST_COPY(VALTYPE, OPTYPE,
              (bsl::in_place, MovUtl::move(VA1)),
              (MovUtl::move(VA1))
             );
    TEST_COPY(VALTYPE, OPTYPE,
              (bsl::in_place, VA1),
              (VA1)
             );

    TEST_COPY(VALTYPE, OPTYPE,
              (bsl::in_place,
               MovUtl::move(VA1), VA2),
              (MovUtl::move(VA1), VA2)
             );
    TEST_COPY(VALTYPE, OPTYPE,
              (bsl::in_place,
               VA1, MovUtl::move(VA2)),
              (VA1, MovUtl::move(VA2))
             );

    TEST_COPY(VALTYPE, OPTYPE,
              (bsl::in_place,
               MovUtl::move(VA1), VA2, MovUtl::move(VA3)),
              (MovUtl::move(VA1), VA2, MovUtl::move(VA3))
             );
    TEST_COPY(VALTYPE, OPTYPE,
              (bsl::in_place,
               VA1, MovUtl::move(VA2), VA3),
              (VA1, MovUtl::move(VA2), VA3)
             );

    TEST_COPY(VALTYPE, OPTYPE,
              (bsl::in_place,
               MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4),
              (MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4)
             );
    TEST_COPY(VALTYPE, OPTYPE,
              (bsl::in_place,
               VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4)),
              (VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4))
             );

    TEST_COPY(VALTYPE, OPTYPE,
              (bsl::in_place,
               MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
               MovUtl::move(VA5)),
              (MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
               MovUtl::move(VA5))
             );
    TEST_COPY(VALTYPE, OPTYPE,
              (bsl::in_place,
               VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
               VA5),
              (VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
               VA5)
             );
/*
    TEST_COPY(VALTYPE, OPTYPE,
              (bsl::in_place,
               MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
               MovUtl::move(VA5), VA6),
              (MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
               MovUtl::move(VA5), VA6)
             );
    TEST_COPY(VALTYPE, OPTYPE,
              (bsl::in_place,
               VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
               VA5, MovUtl::move(VA6)),
              (VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
               VA5, MovUtl::move(VA6))
             );

    TEST_COPY(VALTYPE, OPTYPE,
              (bsl::in_place,
               MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
               MovUtl::move(VA5), VA6, MovUtl::move(VA7)),
              (MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
               MovUtl::move(VA5), VA6, MovUtl::move(VA7))
             );
    TEST_COPY(VALTYPE, OPTYPE,
              (bsl::in_place,
               VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
               VA5, MovUtl::move(VA6), VA7),
              (VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
               VA5, MovUtl::move(VA6), VA7)
              );

    TEST_COPY(VALTYPE, OPTYPE,
              (bsl::in_place,
               MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
               MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8),
              (MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
               MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8)
             );
    TEST_COPY(VALTYPE, OPTYPE,
              (bsl::in_place,
               VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
               VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8)),
              (VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
               VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8))
              );

    TEST_COPY(VALTYPE, OPTYPE,
              (bsl::in_place,
               MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
               MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
               MovUtl::move(VA9)),
              (MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
               MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
               MovUtl::move(VA9))
             );
    TEST_COPY(VALTYPE, OPTYPE,
              (bsl::in_place,
               VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
               VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
               VA9),
              (VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
               VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
               VA9)
              );

    TEST_COPY(VALTYPE, OPTYPE,
              (bsl::in_place,
               MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
               MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
               MovUtl::move(VA9), VA10),
              (MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
               MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
               MovUtl::move(VA9), VA10)
             );
    TEST_COPY(VALTYPE, OPTYPE,
              (bsl::in_place,
               VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
               VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
               VA9, MovUtl::move(VA10)),
              (VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
               VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
               VA9, MovUtl::move(VA10))
              );

    TEST_COPY(VALTYPE, OPTYPE,
              (bsl::in_place,
               MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
               MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
               MovUtl::move(VA9), VA10, MovUtl::move(VA11)),
              (MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
               MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
               MovUtl::move(VA9), VA10, MovUtl::move(VA11))
             );
    TEST_COPY(VALTYPE, OPTYPE,
              (bsl::in_place,
               VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
               VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
               VA9, MovUtl::move(VA10), VA11),
              (VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
               VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
               VA9, MovUtl::move(VA10), VA11)
              );

    TEST_COPY(VALTYPE, OPTYPE,
              (bsl::in_place,
               MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
               MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
               MovUtl::move(VA9), VA10, MovUtl::move(VA11), VA12),
              (MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
               MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
               MovUtl::move(VA9), VA10, MovUtl::move(VA11), VA12)
             );
    TEST_COPY(VALTYPE, OPTYPE,
              (bsl::in_place,
               VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
               VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
               VA9, MovUtl::move(VA10), VA11, MovUtl::move(VA12)),
              (VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
               VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
               VA9, MovUtl::move(VA10), VA11, MovUtl::move(VA12))
              );

    TEST_COPY(VALTYPE, OPTYPE,
              (bsl::in_place,
               MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
               MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
               MovUtl::move(VA9), VA10, MovUtl::move(VA11), VA12,
               MovUtl::move(VA13)),
              (MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
               MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
               MovUtl::move(VA9), VA10, MovUtl::move(VA11), VA12,
               MovUtl::move(VA13))
             );
    TEST_COPY(VALTYPE, OPTYPE,
              (bsl::in_place,
               VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
               VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
               VA9, MovUtl::move(VA10), VA11, MovUtl::move(VA12),
               VA13),
              (VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
               VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
               VA9, MovUtl::move(VA10), VA11, MovUtl::move(VA12),
               VA13)
              );

    TEST_COPY(VALTYPE, OPTYPE,
              (bsl::in_place,
               MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
               MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
               MovUtl::move(VA9), VA10, MovUtl::move(VA11), VA12,
               MovUtl::move(VA13), VA14),
              (MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
               MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
               MovUtl::move(VA9), VA10, MovUtl::move(VA11), VA12,
               MovUtl::move(VA13), VA14)
             );
    TEST_COPY(VALTYPE, OPTYPE,
              (bsl::in_place,
               VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
               VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
               VA9, MovUtl::move(VA10), VA11, MovUtl::move(VA12),
               VA13, MovUtl::move(VA14)),
              (VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
               VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
               VA9, MovUtl::move(VA10), VA11, MovUtl::move(VA12),
               VA13, MovUtl::move(VA14))
              );
*/
}
// helper function that invokes 'TEST_COPYA' macro for various number of
// constructor variadic arguments, and for a combination of lvalues and rvalue
// such that each argument position is tested for perfect forwarding. The
// 'optional' constructor invoked is a non allocator extended constructor.
template <typename VALTYPE, typename OPTYPE>
void test_copyad_helper()
{
    bslma::TestAllocator da("default", veryVeryVeryVerbose);
    bslma::TestAllocator oa("other", veryVeryVeryVerbose);

    bslma::DefaultAllocatorGuard dag(&da);
    // ---   -------------------------------------------------
    TEST_COPYA(VALTYPE, OPTYPE, (bsl::in_place),
               /* no ctor arg list */,
               &da);

    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::in_place, MovUtl::move(VA1)),
               (MovUtl::move(VA1)),
               &da);
    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::in_place, VA1),
               (VA1),
               &da);

    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::in_place,
                MovUtl::move(VA1), VA2),
               (MovUtl::move(VA1), VA2),
               &da);
    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::in_place,
                VA1, MovUtl::move(VA2)),
               (VA1, MovUtl::move(VA2)),
               &da);

    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::in_place,
                MovUtl::move(VA1), VA2, MovUtl::move(VA3)),
               (MovUtl::move(VA1), VA2, MovUtl::move(VA3)),
               &da);
    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::in_place,
                VA1, MovUtl::move(VA2), VA3),
               (VA1, MovUtl::move(VA2), VA3),
               &da);

    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::in_place,
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4),
               (MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4),
               &da);
    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::in_place,
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4)),
               (VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4)),
               &da);

    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::in_place,
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5)),
               (MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5)),
               &da);
    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::in_place,
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5),
               (VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5),
               &da);
/*
    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::in_place,
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6),
               (MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6),
               &da);
    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::in_place,
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6)),
               (VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6)),
               &da);

    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::in_place,
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7)),
               (MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7)),
               &da);
    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::in_place,
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7),
               (VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7),
               &da);

    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::in_place,
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8),
               (MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8),
               &da);
    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::in_place,
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8)),
               (VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8)),
               &da);

    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::in_place,
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                MovUtl::move(VA9)),
               (MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                MovUtl::move(VA9)),
               &da);
    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::in_place,
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                VA9),
               (VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                VA9),
               &da);

    TEST_COPYA(VALTYPE, OPTYPE,
              (bsl::in_place,
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                MovUtl::move(VA9), VA10),
               (MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                MovUtl::move(VA9), VA10),
               &da);
    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::in_place,
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                VA9, MovUtl::move(VA10)),
               (VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                VA9, MovUtl::move(VA10)),
               &da);

    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::in_place,
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                MovUtl::move(VA9), VA10, MovUtl::move(VA11)),
               (MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                MovUtl::move(VA9), VA10, MovUtl::move(VA11)),
               &da);
    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::in_place,
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                VA9, MovUtl::move(VA10), VA11),
               (VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                VA9, MovUtl::move(VA10), VA11),
               &da);

    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::in_place,
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                MovUtl::move(VA9), VA10, MovUtl::move(VA11), VA12),
               (MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                MovUtl::move(VA9), VA10, MovUtl::move(VA11), VA12),
               &da);
    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::in_place,
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                VA9, MovUtl::move(VA10), VA11, MovUtl::move(VA12)),
               (VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                VA9, MovUtl::move(VA10), VA11, MovUtl::move(VA12)),
               &da);

    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::in_place,
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                MovUtl::move(VA9), VA10, MovUtl::move(VA11), VA12,
                MovUtl::move(VA13)),
               (MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                MovUtl::move(VA9), VA10, MovUtl::move(VA11), VA12,
                MovUtl::move(VA13)),
               &da);
    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::in_place,
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                VA9, MovUtl::move(VA10), VA11, MovUtl::move(VA12),
                VA13),
               (VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                VA9, MovUtl::move(VA10), VA11, MovUtl::move(VA12),
                VA13),
               &da);

    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::in_place,
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                MovUtl::move(VA9), VA10, MovUtl::move(VA11), VA12,
                MovUtl::move(VA13), VA14),
               (MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                MovUtl::move(VA9), VA10, MovUtl::move(VA11), VA12,
                MovUtl::move(VA13), VA14),
               &da);
    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::in_place,
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                VA9, MovUtl::move(VA10), VA11, MovUtl::move(VA12),
                VA13, MovUtl::move(VA14)),
               (VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                VA9, MovUtl::move(VA10), VA11, MovUtl::move(VA12),
                VA13, MovUtl::move(VA14)),
               &da);
*/
}
// helper function that invokes 'TEST_COPYA' macro for various number of
// constructor variadic arguments, adjusted for 'UsesAllocatorArgT' type
// construction, and for a combination of lvalues and rvalue such that each
// argument position is tested for perfect forwarding. The 'optional'
// constructor invoked is a non allocator extended constructor.
template <typename VALTYPE, typename OPTYPE>
void test_copyad_argt_helper()
{
    bslma::TestAllocator da("default", veryVeryVeryVerbose);
    bslma::TestAllocator oa("other", veryVeryVeryVerbose);
    bslma::DefaultAllocatorGuard dag(&da);
    // ---   -------------------------------------------------
    TEST_COPYA(VALTYPE, OPTYPE, (bsl::in_place),
               (bsl::allocator_arg, &oa),
               &da);

    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::in_place, MovUtl::move(VA1)),
               (bsl::allocator_arg, &oa, MovUtl::move(VA1)),
               &da);
    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::in_place, VA1),
               (bsl::allocator_arg, &oa, VA1),
               &da);

    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::in_place,
                MovUtl::move(VA1), VA2),
               (bsl::allocator_arg, &oa, MovUtl::move(VA1), VA2),
               &da);
    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::in_place,
                VA1, MovUtl::move(VA2)),
               (bsl::allocator_arg, &oa, VA1, MovUtl::move(VA2)),
               &da);

    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::in_place,
                MovUtl::move(VA1), VA2, MovUtl::move(VA3)),
               (bsl::allocator_arg, &oa,
                MovUtl::move(VA1), VA2, MovUtl::move(VA3)),
               &da);
    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::in_place,
                VA1, MovUtl::move(VA2), VA3),
               (bsl::allocator_arg, &oa,
                VA1, MovUtl::move(VA2), VA3),
               &da);

    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::in_place,
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4),
               (bsl::allocator_arg, &oa,
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4),
               &da);
    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::in_place,
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4)),
               (bsl::allocator_arg, &oa,
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4)),
               &da);

    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::in_place,
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5)),
               (bsl::allocator_arg, &oa,
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5)),
               &da);
    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::in_place,
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5),
               (bsl::allocator_arg, &oa,
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5),
               &da);
/*
    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::in_place,
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6),
               (bsl::allocator_arg, &oa,
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6),
               &da);
    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::in_place,
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6)),
               (bsl::allocator_arg, &oa,
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6)),
               &da);

    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::in_place,
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7)),
               (bsl::allocator_arg, &oa,
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7)),
               &da);
    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::in_place,
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7),
               (bsl::allocator_arg, &oa,
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7),
               &da);

    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::in_place,
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8),
               (bsl::allocator_arg, &oa,
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8),
               &da);
    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::in_place,
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8)),
               (bsl::allocator_arg, &oa,
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8)),
               &da);

    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::in_place,
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                MovUtl::move(VA9)),
               (bsl::allocator_arg, &oa,
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                MovUtl::move(VA9)),
               &da);
    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::in_place,
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                VA9),
               (bsl::allocator_arg, &oa,
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                VA9),
               &da);

    TEST_COPYA(VALTYPE, OPTYPE,
              (bsl::in_place,
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                MovUtl::move(VA9), VA10),
               (bsl::allocator_arg, &oa,
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                MovUtl::move(VA9), VA10),
               &da);
    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::in_place,
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                VA9, MovUtl::move(VA10)),
               (bsl::allocator_arg, &oa,
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                VA9, MovUtl::move(VA10)),
               &da);

    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::in_place,
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                MovUtl::move(VA9), VA10, MovUtl::move(VA11)),
               (bsl::allocator_arg, &oa,
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                MovUtl::move(VA9), VA10, MovUtl::move(VA11)),
               &da);
    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::in_place,
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                VA9, MovUtl::move(VA10), VA11),
               (bsl::allocator_arg, &oa,
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                VA9, MovUtl::move(VA10), VA11),
               &da);

    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::in_place,
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                MovUtl::move(VA9), VA10, MovUtl::move(VA11), VA12),
               (bsl::allocator_arg, &oa,
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                MovUtl::move(VA9), VA10, MovUtl::move(VA11), VA12),
               &da);
    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::in_place,
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                VA9, MovUtl::move(VA10), VA11, MovUtl::move(VA12)),
               (bsl::allocator_arg, &oa,
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                VA9, MovUtl::move(VA10), VA11, MovUtl::move(VA12)),
               &da);

    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::in_place,
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                MovUtl::move(VA9), VA10, MovUtl::move(VA11), VA12,
                MovUtl::move(VA13)),
               (bsl::allocator_arg, &oa,
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                MovUtl::move(VA9), VA10, MovUtl::move(VA11), VA12,
                MovUtl::move(VA13)),
               &da);
    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::in_place,
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                VA9, MovUtl::move(VA10), VA11, MovUtl::move(VA12),
                VA13),
               (bsl::allocator_arg, &oa,
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                VA9, MovUtl::move(VA10), VA11, MovUtl::move(VA12),
                VA13),
               &da);

    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::in_place,
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                MovUtl::move(VA9), VA10, MovUtl::move(VA11), VA12,
                MovUtl::move(VA13), VA14),
               (bsl::allocator_arg, &oa,
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                MovUtl::move(VA9), VA10, MovUtl::move(VA11), VA12,
                MovUtl::move(VA13), VA14),
               &da);
    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::in_place,
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                VA9, MovUtl::move(VA10), VA11, MovUtl::move(VA12),
                VA13, MovUtl::move(VA14)),
               (bsl::allocator_arg, &oa,
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                VA9, MovUtl::move(VA10), VA11, MovUtl::move(VA12),
                VA13, MovUtl::move(VA14)),
               &da);
*/
}
// helper function that invokes 'TEST_COPYA' macro for various number of
// constructor variadic arguments, and for a combination of lvalues and rvalue
// such that each argument position is tested for perfect forwarding. The
// 'optional' constructor invoked is an allocator extended constructor.
template <typename VALTYPE, typename OPTYPE>
void test_copya_helper()
{
    bslma::TestAllocator da("default", veryVeryVeryVerbose);
    bslma::TestAllocator oa("other", veryVeryVeryVerbose);
    bslma::TestAllocator ta("third", veryVeryVeryVerbose);

    bslma::DefaultAllocatorGuard dag(&da);
    // ---   -------------------------------------------------

    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::allocator_arg, &oa, bsl::in_place),
               ,
               &oa);

    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::allocator_arg, &oa, bsl::in_place, MovUtl::move(VA1)),
               (MovUtl::move(VA1)),
               &oa);
    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::allocator_arg, &oa, bsl::in_place, VA1),
               (VA1),
               &oa);

    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::allocator_arg, &oa, bsl::in_place,
                MovUtl::move(VA1), VA2),
               (MovUtl::move(VA1), VA2),
               &oa);
    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::allocator_arg, &oa, bsl::in_place,
                VA1, MovUtl::move(VA2)),
               (VA1, MovUtl::move(VA2)),
               &oa);

    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::allocator_arg, &oa, bsl::in_place,
                MovUtl::move(VA1), VA2, MovUtl::move(VA3)),
               (MovUtl::move(VA1), VA2, MovUtl::move(VA3)),
               &oa);
    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::allocator_arg, &oa, bsl::in_place,
                VA1, MovUtl::move(VA2), VA3),
               (VA1, MovUtl::move(VA2), VA3),
               &oa);

    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::allocator_arg, &oa, bsl::in_place,
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4),
               (MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4),
               &oa);
    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::allocator_arg, &oa, bsl::in_place,
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4)),
               (VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4)),
               &oa);

    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::allocator_arg, &oa, bsl::in_place,
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5)),
               (MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5)),
               &oa);
    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::allocator_arg, &oa, bsl::in_place,
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5),
               (VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5),
               &oa);
/*
    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::allocator_arg, &oa, bsl::in_place,
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6),
               (MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6),
               &oa);
    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::allocator_arg, &oa, bsl::in_place,
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6)),
               (VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6)),
               &oa);

    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::allocator_arg, &oa, bsl::in_place,
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7)),
               (MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7)),
               &oa);
    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::allocator_arg, &oa, bsl::in_place,
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7),
               (VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7),
               &oa);

    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::allocator_arg, &oa, bsl::in_place,
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8),
               (MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8),
               &oa);
    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::allocator_arg, &oa, bsl::in_place,
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8)),
               (VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8)),
               &oa);

    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::allocator_arg, &oa, bsl::in_place,
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                MovUtl::move(VA9)),
               (MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                MovUtl::move(VA9)),
               &oa);
    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::allocator_arg, &oa, bsl::in_place,
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                VA9),
               (VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                VA9),
               &oa);

    TEST_COPYA(VALTYPE, OPTYPE,
              (bsl::allocator_arg, &oa, bsl::in_place,
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                MovUtl::move(VA9), VA10),
               (MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                MovUtl::move(VA9), VA10),
               &oa);
    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::allocator_arg, &oa, bsl::in_place,
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                VA9, MovUtl::move(VA10)),
               (VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                VA9, MovUtl::move(VA10)),
               &oa);

    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::allocator_arg, &oa, bsl::in_place,
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                MovUtl::move(VA9), VA10, MovUtl::move(VA11)),
               (MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                MovUtl::move(VA9), VA10, MovUtl::move(VA11)),
               &oa);
    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::allocator_arg, &oa, bsl::in_place,
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                VA9, MovUtl::move(VA10), VA11),
               (VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                VA9, MovUtl::move(VA10), VA11),
               &oa);

    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::allocator_arg, &oa, bsl::in_place,
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                MovUtl::move(VA9), VA10, MovUtl::move(VA11), VA12),
               (MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                MovUtl::move(VA9), VA10, MovUtl::move(VA11), VA12),
               &oa);
    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::allocator_arg, &oa, bsl::in_place,
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                VA9, MovUtl::move(VA10), VA11, MovUtl::move(VA12)),
               (VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                VA9, MovUtl::move(VA10), VA11, MovUtl::move(VA12)),
               &oa);

    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::allocator_arg, &oa, bsl::in_place,
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                MovUtl::move(VA9), VA10, MovUtl::move(VA11), VA12,
                MovUtl::move(VA13)),
               (MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                MovUtl::move(VA9), VA10, MovUtl::move(VA11), VA12,
                MovUtl::move(VA13)),
               &oa);
    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::allocator_arg, &oa, bsl::in_place,
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                VA9, MovUtl::move(VA10), VA11, MovUtl::move(VA12),
                VA13),
               (VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                VA9, MovUtl::move(VA10), VA11, MovUtl::move(VA12),
                VA13),
               &oa);

    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::allocator_arg, &oa, bsl::in_place,
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                MovUtl::move(VA9), VA10, MovUtl::move(VA11), VA12,
                MovUtl::move(VA13), VA14),
               (MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                MovUtl::move(VA9), VA10, MovUtl::move(VA11), VA12,
                MovUtl::move(VA13), VA14),
               &oa);
    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::allocator_arg, &oa, bsl::in_place,
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                VA9, MovUtl::move(VA10), VA11, MovUtl::move(VA12),
                VA13, MovUtl::move(VA14)),
               (VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                VA9, MovUtl::move(VA10), VA11, MovUtl::move(VA12),
                VA13, MovUtl::move(VA14)),
               &oa);
*/
}
// helper function that invokes 'TEST_COPYA' macro for various number of
// constructor variadic arguments, adjusted for 'UsesAllocatorArgT' type
// construction, and for a combination of lvalues and rvalue such that each
// argument position is tested for perfect forwarding. The 'optional'
// constructor invoked is an allocator extended constructor.
template <typename VALTYPE, typename OPTYPE>
void test_copya_argt_helper()
{
    bslma::TestAllocator da("default", veryVeryVeryVerbose);
    bslma::TestAllocator oa("other", veryVeryVeryVerbose);
    bslma::TestAllocator ta("third", veryVeryVeryVerbose);

    bslma::DefaultAllocatorGuard dag(&oa);
    // ---   -------------------------------------------------

    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::allocator_arg, &oa, bsl::in_place),
               (bsl::allocator_arg, &oa),
               &oa);

    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::allocator_arg, &oa, bsl::in_place, MovUtl::move(VA1)),
               (bsl::allocator_arg, &oa, MovUtl::move(VA1)),
               &oa);
    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::allocator_arg, &oa, bsl::in_place, VA1),
               (bsl::allocator_arg, &oa, VA1),
               &oa);

    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::allocator_arg, &oa, bsl::in_place,
                MovUtl::move(VA1), VA2),
               (bsl::allocator_arg, &oa, MovUtl::move(VA1), VA2),
               &oa);
    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::allocator_arg, &oa, bsl::in_place,
                VA1, MovUtl::move(VA2)),
               (bsl::allocator_arg, &oa, VA1, MovUtl::move(VA2)),
               &oa);

    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::allocator_arg, &oa, bsl::in_place,
                MovUtl::move(VA1), VA2, MovUtl::move(VA3)),
               (bsl::allocator_arg, &oa,
                MovUtl::move(VA1), VA2, MovUtl::move(VA3)),
               &oa);
    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::allocator_arg, &oa, bsl::in_place,
                VA1, MovUtl::move(VA2), VA3),
               (bsl::allocator_arg, &oa,
                VA1, MovUtl::move(VA2), VA3),
               &oa);

    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::allocator_arg, &oa, bsl::in_place,
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4),
               (bsl::allocator_arg, &oa,
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4),
               &oa);
    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::allocator_arg, &oa, bsl::in_place,
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4)),
               (bsl::allocator_arg, &oa,
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4)),
               &oa);

    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::allocator_arg, &oa, bsl::in_place,
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5)),
               (bsl::allocator_arg, &oa,
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5)),
               &oa);
    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::allocator_arg, &oa, bsl::in_place,
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5),
               (bsl::allocator_arg, &oa,
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5),
               &oa);
/*
    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::allocator_arg, &oa, bsl::in_place,
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6),
               (bsl::allocator_arg, &oa,
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6),
               &oa);
    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::allocator_arg, &oa, bsl::in_place,
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6)),
               (bsl::allocator_arg, &oa,
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6)),
               &oa);

    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::allocator_arg, &oa, bsl::in_place,
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7)),
               (bsl::allocator_arg, &oa,
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7)),
               &oa);
    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::allocator_arg, &oa, bsl::in_place,
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7),
               (bsl::allocator_arg, &oa,
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7),
               &oa);

    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::allocator_arg, &oa, bsl::in_place,
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8),
               (bsl::allocator_arg, &oa,
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8),
               &oa);
    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::allocator_arg, &oa, bsl::in_place,
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8)),
               (bsl::allocator_arg, &oa,
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8)),
               &oa);

    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::allocator_arg, &oa, bsl::in_place,
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                MovUtl::move(VA9)),
               (bsl::allocator_arg, &oa,
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                MovUtl::move(VA9)),
               &oa);
    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::allocator_arg, &oa, bsl::in_place,
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                VA9),
               (bsl::allocator_arg, &oa,
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                VA9),
               &oa);

    TEST_COPYA(VALTYPE, OPTYPE,
              (bsl::allocator_arg, &oa, bsl::in_place,
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                MovUtl::move(VA9), VA10),
               (bsl::allocator_arg, &oa,
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                MovUtl::move(VA9), VA10),
               &oa);
    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::allocator_arg, &oa, bsl::in_place,
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                VA9, MovUtl::move(VA10)),
               (bsl::allocator_arg, &oa,
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                VA9, MovUtl::move(VA10)),
               &oa);

    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::allocator_arg, &oa, bsl::in_place,
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                MovUtl::move(VA9), VA10, MovUtl::move(VA11)),
               (bsl::allocator_arg, &oa,
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                MovUtl::move(VA9), VA10, MovUtl::move(VA11)),
               &oa);
    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::allocator_arg, &oa, bsl::in_place,
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                VA9, MovUtl::move(VA10), VA11),
               (bsl::allocator_arg, &oa,
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                VA9, MovUtl::move(VA10), VA11),
               &oa);

    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::allocator_arg, &oa, bsl::in_place,
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                MovUtl::move(VA9), VA10, MovUtl::move(VA11), VA12),
               (bsl::allocator_arg, &oa,
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                MovUtl::move(VA9), VA10, MovUtl::move(VA11), VA12),
               &oa);
    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::allocator_arg, &oa, bsl::in_place,
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                VA9, MovUtl::move(VA10), VA11, MovUtl::move(VA12)),
               (bsl::allocator_arg, &oa,
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                VA9, MovUtl::move(VA10), VA11, MovUtl::move(VA12)),
               &oa);

    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::allocator_arg, &oa, bsl::in_place,
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                MovUtl::move(VA9), VA10, MovUtl::move(VA11), VA12,
                MovUtl::move(VA13)),
               (bsl::allocator_arg, &oa,
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                MovUtl::move(VA9), VA10, MovUtl::move(VA11), VA12,
                MovUtl::move(VA13)),
               &oa);
    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::allocator_arg, &oa, bsl::in_place,
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                VA9, MovUtl::move(VA10), VA11, MovUtl::move(VA12),
                VA13),
               (bsl::allocator_arg, &oa,
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                VA9, MovUtl::move(VA10), VA11, MovUtl::move(VA12),
                VA13),
               &oa);

    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::allocator_arg, &oa, bsl::in_place,
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                MovUtl::move(VA9), VA10, MovUtl::move(VA11), VA12,
                MovUtl::move(VA13), VA14),
               (bsl::allocator_arg, &oa,
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                MovUtl::move(VA9), VA10, MovUtl::move(VA11), VA12,
                MovUtl::move(VA13), VA14),
               &oa);
    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::allocator_arg, &oa, bsl::in_place,
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                VA9, MovUtl::move(VA10), VA11, MovUtl::move(VA12),
                VA13, MovUtl::move(VA14)),
               (bsl::allocator_arg, &oa,
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                VA9, MovUtl::move(VA10), VA11, MovUtl::move(VA12),
                VA13, MovUtl::move(VA14)),
               &oa);
*/
}
#ifdef BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS
// helper function that invokes 'TEST_COPY' macro for 'initializer_list' and
// various number of constructor variadic arguments, and for a combination
// of lvalues and rvalue such that each argument position is tested
// for perfect forwarding.
template <typename VALTYPE, typename OPTYPE>
void test_copyil_helper()
{
  TEST_COPY(VALTYPE, OPTYPE, (bsl::in_place, {1,2,3}),
            ({1,2,3})
           );

  TEST_COPY(VALTYPE, OPTYPE,
            (bsl::in_place, {1,2,3}, MovUtl::move(VA1)),
            ({1,2,3}, MovUtl::move(VA1))
           );
  TEST_COPY(VALTYPE, OPTYPE,
            (bsl::in_place, {1,2,3},  VA1),
            ({1,2,3}, VA1)
           );

  TEST_COPY(VALTYPE, OPTYPE,
            (bsl::in_place, {1,2,3},
             MovUtl::move(VA1), VA2),
            ({1,2,3}, MovUtl::move(VA1), VA2)
           );
  TEST_COPY(VALTYPE, OPTYPE,
            (bsl::in_place, {1,2,3},
             VA1, MovUtl::move(VA2)),
            ({1,2,3}, VA1, MovUtl::move(VA2))
           );

  TEST_COPY(VALTYPE, OPTYPE,
            (bsl::in_place, {1,2,3},
             MovUtl::move(VA1), VA2, MovUtl::move(VA3)),
            ({1,2,3}, MovUtl::move(VA1), VA2, MovUtl::move(VA3))
           );
  TEST_COPY(VALTYPE, OPTYPE,
            (bsl::in_place, {1,2,3},
             VA1, MovUtl::move(VA2), VA3),
            ({1,2,3}, VA1, MovUtl::move(VA2), VA3)
           );

  TEST_COPY(VALTYPE, OPTYPE,
            (bsl::in_place, {1,2,3},
             MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4),
            ({1,2,3}, MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4)
           );
  TEST_COPY(VALTYPE, OPTYPE,
            (bsl::in_place, {1,2,3},
             VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4)),
            ({1,2,3}, VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4))
           );

  TEST_COPY(VALTYPE, OPTYPE,
            (bsl::in_place, {1,2,3},
             MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
             MovUtl::move(VA5)),
            ({1,2,3}, MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
             MovUtl::move(VA5))
           );
  TEST_COPY(VALTYPE, OPTYPE,
            (bsl::in_place, {1,2,3},
             VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
             VA5),
            ({1,2,3}, VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
             VA5)
           );
/*
  TEST_COPY(VALTYPE, OPTYPE,
            (bsl::in_place, {1,2,3},
             MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
             MovUtl::move(VA5), VA6),
            ({1,2,3}, MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
             MovUtl::move(VA5), VA6)
           );
  TEST_COPY(VALTYPE, OPTYPE,
            (bsl::in_place, {1,2,3},
             VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
             VA5, MovUtl::move(VA6)),
            ({1,2,3}, VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
             VA5, MovUtl::move(VA6))
           );

  TEST_COPY(VALTYPE, OPTYPE,
            (bsl::in_place, {1,2,3},
             MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
             MovUtl::move(VA5), VA6, MovUtl::move(VA7)),
            ({1,2,3}, MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
             MovUtl::move(VA5), VA6, MovUtl::move(VA7))
           );
  TEST_COPY(VALTYPE, OPTYPE,
            (bsl::in_place, {1,2,3},
             VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
             VA5, MovUtl::move(VA6), VA7),
            ({1,2,3}, VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
             VA5, MovUtl::move(VA6), VA7)
            );

  TEST_COPY(VALTYPE, OPTYPE,
            (bsl::in_place, {1,2,3},
             MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
             MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8),
            ({1,2,3}, MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
             MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8)
           );
  TEST_COPY(VALTYPE, OPTYPE,
            (bsl::in_place, {1,2,3},
             VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
             VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8)),
            ({1,2,3}, VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
             VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8))
            );

  TEST_COPY(VALTYPE, OPTYPE,
            (bsl::in_place, {1,2,3},
             MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
             MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
             MovUtl::move(VA9)),
            ({1,2,3}, MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
             MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
             MovUtl::move(VA9))
           );
  TEST_COPY(VALTYPE, OPTYPE,
            (bsl::in_place, {1,2,3},
             VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
             VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
             VA9),
            ({1,2,3}, VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
             VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
             VA9)
            );

  TEST_COPY(VALTYPE, OPTYPE,
            (bsl::in_place, {1,2,3},
             MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
             MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
             MovUtl::move(VA9), VA10),
            ({1,2,3}, MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
             MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
             MovUtl::move(VA9), VA10)
           );
  TEST_COPY(VALTYPE, OPTYPE,
            (bsl::in_place, {1,2,3},
             VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
             VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
             VA9, MovUtl::move(VA10)),
            ({1,2,3}, VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
             VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
             VA9, MovUtl::move(VA10))
            );

  TEST_COPY(VALTYPE, OPTYPE,
            (bsl::in_place, {1,2,3},
             MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
             MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
             MovUtl::move(VA9), VA10, MovUtl::move(VA11)),
            ({1,2,3}, MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
             MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
             MovUtl::move(VA9), VA10, MovUtl::move(VA11))
           );
  TEST_COPY(VALTYPE, OPTYPE,
            (bsl::in_place, {1,2,3},
             VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
             VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
             VA9, MovUtl::move(VA10), VA11),
            ({1,2,3}, VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
             VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
             VA9, MovUtl::move(VA10), VA11)
            );

  TEST_COPY(VALTYPE, OPTYPE,
            (bsl::in_place, {1,2,3},
             MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
             MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
             MovUtl::move(VA9), VA10, MovUtl::move(VA11), VA12),
            ({1,2,3}, MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
             MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
             MovUtl::move(VA9), VA10, MovUtl::move(VA11), VA12)
           );
  TEST_COPY(VALTYPE, OPTYPE,
            (bsl::in_place, {1,2,3},
             VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
             VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
             VA9, MovUtl::move(VA10), VA11, MovUtl::move(VA12)),
            ({1,2,3}, VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
             VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
             VA9, MovUtl::move(VA10), VA11, MovUtl::move(VA12))
            );

  TEST_COPY(VALTYPE, OPTYPE,
            (bsl::in_place, {1,2,3},
             MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
             MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
             MovUtl::move(VA9), VA10, MovUtl::move(VA11), VA12,
             MovUtl::move(VA13)),
            ({1,2,3}, MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
             MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
             MovUtl::move(VA9), VA10, MovUtl::move(VA11), VA12,
             MovUtl::move(VA13))
           );
  TEST_COPY(VALTYPE, OPTYPE,
            (bsl::in_place, {1,2,3},
             VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
             VA5, MovUtl::move(VA6), VA7, MovUtl::move
             (VA8),
             VA9, MovUtl::move(VA10), VA11, MovUtl::move(VA12),
             VA13),
            ({1,2,3}, VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
             VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
             VA9, MovUtl::move(VA10), VA11, MovUtl::move(VA12),
             VA13)
            );
*/
}
// helper function that invokes 'TEST_COPYA' macro for 'initializer_list' and
// various number of constructor variadic arguments, and for a combination
// of lvalues and rvalue such that each argument position is tested
// for perfect forarding. The 'optional' constructor invoked is an allocator
// extended constructor.
template <typename VALTYPE, typename OPTYPE>
void test_copyila_helper()
{
    bslma::TestAllocator da("default", veryVeryVeryVerbose);
    bslma::TestAllocator oa("other", veryVeryVeryVerbose);
    bslma::TestAllocator ta("third", veryVeryVeryVerbose);

    bslma::DefaultAllocatorGuard dag(&da);
    // ---   -------------------------------------------------
    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::allocator_arg, &oa, bsl::in_place, {1,2,3}),
               ({1,2,3}),
               &oa);

    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::allocator_arg, &oa, bsl::in_place, {1,2,3},
                MovUtl::move(VA1)),
               ({1,2,3}, MovUtl::move(VA1)),
               &oa);
    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::allocator_arg, &oa, bsl::in_place, {1,2,3}, VA1),
               ({1,2,3},VA1),
               &oa);

    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::allocator_arg, &oa, bsl::in_place, {1,2,3},
                MovUtl::move(VA1), VA2),
               ({1,2,3}, MovUtl::move(VA1), VA2),
               &oa);
    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::allocator_arg, &oa, bsl::in_place, {1,2,3},
                VA1, MovUtl::move(VA2)),
               ({1,2,3}, VA1, MovUtl::move(VA2)),
               &oa);

    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::allocator_arg, &oa, bsl::in_place, {1,2,3},
                MovUtl::move(VA1), VA2, MovUtl::move(VA3)),
               ({1,2,3}, MovUtl::move(VA1), VA2, MovUtl::move(VA3)),
               &oa);
    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::allocator_arg, &oa, bsl::in_place, {1,2,3},
                VA1, MovUtl::move(VA2), VA3),
               ({1,2,3}, VA1, MovUtl::move(VA2), VA3),
               &oa);

    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::allocator_arg, &oa, bsl::in_place, {1,2,3},
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4),
               ({1,2,3}, MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4),
               &oa);
    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::allocator_arg, &oa, bsl::in_place, {1,2,3},
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4)),
               ({1,2,3}, VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4)),
               &oa);

    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::allocator_arg, &oa, bsl::in_place, {1,2,3},
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5)),
               ({1,2,3}, MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5)),
               &oa);
    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::allocator_arg, &oa, bsl::in_place, {1,2,3},
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5),
               ({1,2,3}, VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5),
               &oa);
/*
    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::allocator_arg, &oa, bsl::in_place, {1,2,3},
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6),
               ({1,2,3}, MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6),
               &oa);
    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::allocator_arg, &oa, bsl::in_place, {1,2,3},
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6)),
               ({1,2,3}, VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6)),
               &oa);

    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::allocator_arg, &oa, bsl::in_place, {1,2,3},
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7)),
               ({1,2,3}, MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7)),
               &oa);
    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::allocator_arg, &oa, bsl::in_place, {1,2,3},
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7),
               ({1,2,3}, VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7),
               &oa);

    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::allocator_arg, &oa, bsl::in_place, {1,2,3},
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8),
               ({1,2,3}, MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8),
               &oa);
    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::allocator_arg, &oa, bsl::in_place, {1,2,3},
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8)),
               ({1,2,3}, VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8)),
               &oa);

    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::allocator_arg, &oa, bsl::in_place, {1,2,3},
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                MovUtl::move(VA9)),
               ({1,2,3}, MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                MovUtl::move(VA9)),
               &oa);
    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::allocator_arg, &oa, bsl::in_place, {1,2,3},
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                VA9),
               ({1,2,3}, VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                VA9),
               &oa);

    TEST_COPYA(VALTYPE, OPTYPE,
              (bsl::allocator_arg, &oa, bsl::in_place, {1,2,3},
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                MovUtl::move(VA9), VA10),
               ({1,2,3}, MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                MovUtl::move(VA9), VA10),
               &oa);
    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::allocator_arg, &oa, bsl::in_place, {1,2,3},
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                VA9, MovUtl::move(VA10)),
               ({1,2,3}, VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                VA9, MovUtl::move(VA10)),
               &oa);

    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::allocator_arg, &oa, bsl::in_place, {1,2,3},
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                MovUtl::move(VA9), VA10, MovUtl::move(VA11)),
               ({1,2,3}, MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                MovUtl::move(VA9), VA10, MovUtl::move(VA11)),
               &oa);
    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::allocator_arg, &oa, bsl::in_place, {1,2,3},
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                VA9, MovUtl::move(VA10), VA11),
               ({1,2,3}, VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                VA9, MovUtl::move(VA10), VA11),
               &oa);

    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::allocator_arg, &oa, bsl::in_place, {1,2,3},
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                MovUtl::move(VA9), VA10, MovUtl::move(VA11), VA12),
               ({1,2,3}, MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                MovUtl::move(VA9), VA10, MovUtl::move(VA11), VA12),
               &oa);
    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::allocator_arg, &oa, bsl::in_place, {1,2,3},
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                VA9, MovUtl::move(VA10), VA11, MovUtl::move(VA12)),
               ({1,2,3}, VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                VA9, MovUtl::move(VA10), VA11, MovUtl::move(VA12)),
               &oa);

    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::allocator_arg, &oa, bsl::in_place, {1,2,3},
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                MovUtl::move(VA9), VA10, MovUtl::move(VA11), VA12,
                MovUtl::move(VA13)),
               ({1,2,3}, MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                MovUtl::move(VA9), VA10, MovUtl::move(VA11), VA12,
                MovUtl::move(VA13)),
               &oa);
    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::allocator_arg, &oa, bsl::in_place, {1,2,3},
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                VA9, MovUtl::move(VA10), VA11, MovUtl::move(VA12),
                VA13),
               ({1,2,3}, VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                VA9, MovUtl::move(VA10), VA11, MovUtl::move(VA12),
                VA13),
               &oa);
*/
}
// helper function that invokes 'TEST_COPYA' macro for 'initializer_list' and
// various number of constructor variadic arguments, modified to support
// initialization of a 'UsesAllocatorArgT' type, and for a combination
// of lvalues and rvalue such that each argument position is tested
// for perfect forwarding. The 'optional' constructor invoked is an allocator
// extended constructor.
template <typename VALTYPE, typename OPTYPE>
void test_copyila_argt_helper()
{
    bslma::TestAllocator da("default", veryVeryVeryVerbose);
    bslma::TestAllocator oa("other", veryVeryVeryVerbose);
    bslma::TestAllocator ta("third", veryVeryVeryVerbose);

    bslma::DefaultAllocatorGuard dag(&da);
    // ---   -------------------------------------------------
    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::allocator_arg, &oa, bsl::in_place, {1,2,3}),
               (bsl::allocator_arg, &oa, {1,2,3}),
               &oa);

    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::allocator_arg, &oa, bsl::in_place, {1,2,3},
                MovUtl::move(VA1)),
               (bsl::allocator_arg, &oa, {1,2,3}, MovUtl::move(VA1)),
               &oa);
    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::allocator_arg, &oa, bsl::in_place, {1,2,3}, VA1),
               (bsl::allocator_arg, &oa, {1,2,3}, VA1),
               &oa);

    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::allocator_arg, &oa, bsl::in_place, {1,2,3},
                MovUtl::move(VA1), VA2),
               (bsl::allocator_arg, &oa, {1,2,3}, MovUtl::move(VA1), VA2),
               &oa);
    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::allocator_arg, &oa, bsl::in_place, {1,2,3},
                VA1, MovUtl::move(VA2)),
               (bsl::allocator_arg, &oa, {1,2,3}, VA1, MovUtl::move(VA2)),
               &oa);

    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::allocator_arg, &oa, bsl::in_place, {1,2,3},
                MovUtl::move(VA1), VA2, MovUtl::move(VA3)),
               (bsl::allocator_arg, &oa, {1,2,3},
                MovUtl::move(VA1), VA2, MovUtl::move(VA3)),
               &oa);
    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::allocator_arg, &oa, bsl::in_place, {1,2,3},
                VA1, MovUtl::move(VA2), VA3),
               (bsl::allocator_arg, &oa, {1,2,3},
                VA1, MovUtl::move(VA2), VA3),
               &oa);

    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::allocator_arg, &oa, bsl::in_place, {1,2,3},
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4),
               (bsl::allocator_arg, &oa, {1,2,3},
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4),
               &oa);
    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::allocator_arg, &oa, bsl::in_place, {1,2,3},
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4)),
               (bsl::allocator_arg, &oa, {1,2,3},
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4)),
               &oa);

    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::allocator_arg, &oa, bsl::in_place, {1,2,3},
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5)),
               (bsl::allocator_arg, &oa, {1,2,3},
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5)),
               &oa);
    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::allocator_arg, &oa, bsl::in_place, {1,2,3},
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5),
               (bsl::allocator_arg, &oa, {1,2,3},
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5),
               &oa);
/*
    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::allocator_arg, &oa, bsl::in_place, {1,2,3},
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6),
               (bsl::allocator_arg, &oa, {1,2,3},
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6),
               &oa);
    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::allocator_arg, &oa, bsl::in_place, {1,2,3},
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6)),
               (bsl::allocator_arg, &oa, {1,2,3},
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6)),
               &oa);

    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::allocator_arg, &oa, bsl::in_place, {1,2,3},
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7)),
               (bsl::allocator_arg, &oa, {1,2,3},
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7)),
               &oa);
    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::allocator_arg, &oa, bsl::in_place, {1,2,3},
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7),
               (bsl::allocator_arg, &oa, {1,2,3},
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7),
               &oa);

    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::allocator_arg, &oa, bsl::in_place, {1,2,3},
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8),
               (bsl::allocator_arg, &oa, {1,2,3},
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8),
               &oa);
    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::allocator_arg, &oa, bsl::in_place, {1,2,3},
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8)),
               (bsl::allocator_arg, &oa, {1,2,3},
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8)),
               &oa);

    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::allocator_arg, &oa, bsl::in_place, {1,2,3},
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                MovUtl::move(VA9)),
               (bsl::allocator_arg, &oa, {1,2,3},
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                MovUtl::move(VA9)),
               &oa);
    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::allocator_arg, &oa, bsl::in_place, {1,2,3},
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                VA9),
               (bsl::allocator_arg, &oa, {1,2,3},
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                VA9),
               &oa);

    TEST_COPYA(VALTYPE, OPTYPE,
              (bsl::allocator_arg, &oa, bsl::in_place, {1,2,3},
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                MovUtl::move(VA9), VA10),
               (bsl::allocator_arg, &oa, {1,2,3},
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                MovUtl::move(VA9), VA10),
               &oa);
    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::allocator_arg, &oa, bsl::in_place, {1,2,3},
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                VA9, MovUtl::move(VA10)),
               (bsl::allocator_arg, &oa, {1,2,3},
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                VA9, MovUtl::move(VA10)),
               &oa);

    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::allocator_arg, &oa, bsl::in_place, {1,2,3},
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                MovUtl::move(VA9), VA10, MovUtl::move(VA11)),
               (bsl::allocator_arg, &oa, {1,2,3},
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                MovUtl::move(VA9), VA10, MovUtl::move(VA11)),
               &oa);
    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::allocator_arg, &oa, bsl::in_place, {1,2,3},
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                VA9, MovUtl::move(VA10), VA11),
               (bsl::allocator_arg, &oa, {1,2,3},
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                VA9, MovUtl::move(VA10), VA11),
               &oa);

    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::allocator_arg, &oa, bsl::in_place, {1,2,3},
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                MovUtl::move(VA9), VA10, MovUtl::move(VA11), VA12),
               (bsl::allocator_arg, &oa, {1,2,3},
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                MovUtl::move(VA9), VA10, MovUtl::move(VA11), VA12),
               &oa);
    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::allocator_arg, &oa, bsl::in_place, {1,2,3},
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                VA9, MovUtl::move(VA10), VA11, MovUtl::move(VA12)),
               (bsl::allocator_arg, &oa, {1,2,3},
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                VA9, MovUtl::move(VA10), VA11, MovUtl::move(VA12)),
               &oa);

    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::allocator_arg, &oa, bsl::in_place, {1,2,3},
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                MovUtl::move(VA9), VA10, MovUtl::move(VA11), VA12,
                MovUtl::move(VA13)),
               (bsl::allocator_arg, &oa, {1,2,3},
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                MovUtl::move(VA9), VA10, MovUtl::move(VA11), VA12,
                MovUtl::move(VA13)),
               &oa);
    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::allocator_arg, &oa, bsl::in_place, {1,2,3},
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                VA9, MovUtl::move(VA10), VA11, MovUtl::move(VA12),
                VA13),
               (bsl::allocator_arg, &oa, {1,2,3},
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                VA9, MovUtl::move(VA10), VA11, MovUtl::move(VA12),
                VA13),
               &oa);
*/
}
// helper function that invokes 'TEST_COPYA' macro for 'initializer_list' and
// various number of constructor variadic arguments, and for a combination
// of lvalues and rvalue such that each argument position is tested
// for perfect forarding. The 'optional' constructor invoked is a non allocator
// extended constructor.
template <typename VALTYPE, typename OPTYPE>
void test_copyilad_helper()
{
    bslma::TestAllocator da("default", veryVeryVeryVerbose);
    bslma::TestAllocator oa("other", veryVeryVeryVerbose);

    bslma::DefaultAllocatorGuard dag(&da);
    // ---   -------------------------------------------------

    TEST_COPYA(VALTYPE, OPTYPE, (bsl::in_place, {1,2,3}),
               ({1,2,3}),
               &da);

    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::in_place, {1,2,3}, MovUtl::move(VA1)),
               ({1,2,3}, MovUtl::move(VA1)),
               &da);
    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::in_place, {1,2,3}, VA1),
               ({1,2,3},VA1),
               &da);

    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::in_place, {1,2,3},
                MovUtl::move(VA1), VA2),
               ({1,2,3}, MovUtl::move(VA1), VA2),
               &da);
    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::in_place, {1,2,3},
                VA1, MovUtl::move(VA2)),
               ({1,2,3}, VA1, MovUtl::move(VA2)),
               &da);

    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::in_place, {1,2,3},
                MovUtl::move(VA1), VA2, MovUtl::move(VA3)),
               ({1,2,3}, MovUtl::move(VA1), VA2, MovUtl::move(VA3)),
               &da);
    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::in_place, {1,2,3},
                VA1, MovUtl::move(VA2), VA3),
               ({1,2,3}, VA1, MovUtl::move(VA2), VA3),
               &da);

    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::in_place, {1,2,3},
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4),
               ({1,2,3}, MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4),
               &da);
    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::in_place, {1,2,3},
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4)),
               ({1,2,3}, VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4)),
               &da);

    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::in_place, {1,2,3},
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5)),
               ({1,2,3}, MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5)),
               &da);
    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::in_place, {1,2,3},
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5),
               ({1,2,3}, VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5),
               &da);
/*
    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::in_place, {1,2,3},
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6),
               ({1,2,3}, MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6),
               &da);
    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::in_place, {1,2,3},
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6)),
               ({1,2,3}, VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6)),
               &da);

    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::in_place, {1,2,3},
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7)),
               ({1,2,3}, MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7)),
               &da);
    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::in_place, {1,2,3},
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7),
               ({1,2,3}, VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7),
               &da);

    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::in_place, {1,2,3},
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8),
               ({1,2,3}, MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8),
               &da);
    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::in_place, {1,2,3},
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8)),
               ({1,2,3}, VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8)),
               &da);

    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::in_place, {1,2,3},
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                MovUtl::move(VA9)),
               ({1,2,3}, MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                MovUtl::move(VA9)),
               &da);
    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::in_place, {1,2,3},
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                VA9),
               ({1,2,3}, VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                VA9),
               &da);

    TEST_COPYA(VALTYPE, OPTYPE,
              (bsl::in_place, {1,2,3},
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                MovUtl::move(VA9), VA10),
               ({1,2,3}, MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                MovUtl::move(VA9), VA10),
               &da);
    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::in_place, {1,2,3},
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                VA9, MovUtl::move(VA10)),
               ({1,2,3}, VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                VA9, MovUtl::move(VA10)),
               &da);

    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::in_place, {1,2,3},
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                MovUtl::move(VA9), VA10, MovUtl::move(VA11)),
               ({1,2,3}, MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                MovUtl::move(VA9), VA10, MovUtl::move(VA11)),
               &da);
    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::in_place, {1,2,3},
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                VA9, MovUtl::move(VA10), VA11),
               ({1,2,3}, VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                VA9, MovUtl::move(VA10), VA11),
               &da);

    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::in_place, {1,2,3},
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                MovUtl::move(VA9), VA10, MovUtl::move(VA11), VA12),
               ({1,2,3}, MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                MovUtl::move(VA9), VA10, MovUtl::move(VA11), VA12),
               &da);
    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::in_place, {1,2,3},
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                VA9, MovUtl::move(VA10), VA11, MovUtl::move(VA12)),
               ({1,2,3}, VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                VA9, MovUtl::move(VA10), VA11, MovUtl::move(VA12)),
               &da);

    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::in_place, {1,2,3},
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                MovUtl::move(VA9), VA10, MovUtl::move(VA11), VA12,
                MovUtl::move(VA13)),
               ({1,2,3}, MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                MovUtl::move(VA9), VA10, MovUtl::move(VA11), VA12,
                MovUtl::move(VA13)),
               &da);
    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::in_place, {1,2,3},
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                VA9, MovUtl::move(VA10), VA11, MovUtl::move(VA12),
                VA13),
               ({1,2,3}, VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                VA9, MovUtl::move(VA10), VA11, MovUtl::move(VA12),
                VA13),
               &da);
*/
}
// helper function that invokes 'TEST_COPYA' macro for 'initializer_list' and
// various number of constructor variadic arguments, modified to support
// initialization of a 'UsesAllocatorArgT' type, and for a combination
// of lvalues and rvalue such that each argument position is tested
// for perfect forwarding.  The 'optional' constructor invoked is a non
// allocator extended constructor.
template <typename VALTYPE, typename OPTYPE>
void test_copyilad_argt_helper()
{
    bslma::TestAllocator da("default", veryVeryVeryVerbose);
    bslma::TestAllocator oa("other", veryVeryVeryVerbose);

    bslma::DefaultAllocatorGuard dag(&da);
    // ---   -------------------------------------------------
    TEST_COPYA(VALTYPE, OPTYPE, (bsl::in_place, {1,2,3}),
               (bsl::allocator_arg, &oa, {1,2,3}),
               &da);

    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::in_place, {1,2,3}, MovUtl::move(VA1)),
               (bsl::allocator_arg, &oa, {1,2,3}, MovUtl::move(VA1)),
               &da);
    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::in_place, {1,2,3}, VA1),
               (bsl::allocator_arg, &oa, {1,2,3}, VA1),
               &da);

    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::in_place, {1,2,3},
                MovUtl::move(VA1), VA2),
               (bsl::allocator_arg, &oa, {1,2,3}, MovUtl::move(VA1), VA2),
               &da);
    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::in_place, {1,2,3},
                VA1, MovUtl::move(VA2)),
               (bsl::allocator_arg, &oa, {1,2,3}, VA1, MovUtl::move(VA2)),
               &da);

    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::in_place, {1,2,3},
                MovUtl::move(VA1), VA2, MovUtl::move(VA3)),
               (bsl::allocator_arg, &oa, {1,2,3},
                MovUtl::move(VA1), VA2, MovUtl::move(VA3)),
               &da);
    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::in_place, {1,2,3},
                VA1, MovUtl::move(VA2), VA3),
               (bsl::allocator_arg, &oa, {1,2,3},
                VA1, MovUtl::move(VA2), VA3),
               &da);

    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::in_place, {1,2,3},
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4),
               (bsl::allocator_arg, &oa, {1,2,3},
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4),
               &da);
    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::in_place, {1,2,3},
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4)),
               (bsl::allocator_arg, &oa, {1,2,3},
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4)),
               &da);

    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::in_place, {1,2,3},
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5)),
               (bsl::allocator_arg, &oa, {1,2,3},
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5)),
               &da);
    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::in_place, {1,2,3},
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5),
               (bsl::allocator_arg, &oa, {1,2,3},
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5),
               &da);
/*
    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::in_place, {1,2,3},
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6),
               (bsl::allocator_arg, &oa, {1,2,3},
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6),
               &da);
    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::in_place, {1,2,3},
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6)),
               (bsl::allocator_arg, &oa, {1,2,3},
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6)),
               &da);

    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::in_place, {1,2,3},
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7)),
               (bsl::allocator_arg, &oa, {1,2,3},
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7)),
               &da);
    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::in_place, {1,2,3},
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7),
               (bsl::allocator_arg, &oa, {1,2,3},
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7),
               &da);

    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::in_place, {1,2,3},
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8),
               (bsl::allocator_arg, &oa, {1,2,3},
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8),
               &da);
    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::in_place, {1,2,3},
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8)),
               (bsl::allocator_arg, &oa, {1,2,3},
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8)),
               &da);

    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::in_place, {1,2,3},
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                MovUtl::move(VA9)),
               (bsl::allocator_arg, &oa, {1,2,3},
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                MovUtl::move(VA9)),
               &da);
    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::in_place, {1,2,3},
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                VA9),
               (bsl::allocator_arg, &oa, {1,2,3},
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                VA9),
               &da);

    TEST_COPYA(VALTYPE, OPTYPE,
              (bsl::in_place, {1,2,3},
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                MovUtl::move(VA9), VA10),
               (bsl::allocator_arg, &oa, {1,2,3},
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                MovUtl::move(VA9), VA10),
               &da);
    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::in_place, {1,2,3},
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                VA9, MovUtl::move(VA10)),
               (bsl::allocator_arg, &oa, {1,2,3},
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                VA9, MovUtl::move(VA10)),
               &da);

    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::in_place, {1,2,3},
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                MovUtl::move(VA9), VA10, MovUtl::move(VA11)),
               (bsl::allocator_arg, &oa, {1,2,3},
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                MovUtl::move(VA9), VA10, MovUtl::move(VA11)),
               &da);
    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::in_place, {1,2,3},
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                VA9, MovUtl::move(VA10), VA11),
               (bsl::allocator_arg, &oa, {1,2,3},
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                VA9, MovUtl::move(VA10), VA11),
               &da);

    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::in_place, {1,2,3},
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                MovUtl::move(VA9), VA10, MovUtl::move(VA11), VA12),
               (bsl::allocator_arg, &oa, {1,2,3},
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                MovUtl::move(VA9), VA10, MovUtl::move(VA11), VA12),
               &da);
    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::in_place, {1,2,3},
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                VA9, MovUtl::move(VA10), VA11, MovUtl::move(VA12)),
               (bsl::allocator_arg, &oa, {1,2,3},
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                VA9, MovUtl::move(VA10), VA11, MovUtl::move(VA12)),
               &da);

    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::in_place, {1,2,3},
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                MovUtl::move(VA9), VA10, MovUtl::move(VA11), VA12,
                MovUtl::move(VA13)),
               (bsl::allocator_arg, &oa, {1,2,3},
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                MovUtl::move(VA9), VA10, MovUtl::move(VA11), VA12,
                MovUtl::move(VA13)),
               &da);
    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::in_place, {1,2,3},
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                VA9, MovUtl::move(VA10), VA11, MovUtl::move(VA12),
                VA13),
               (bsl::allocator_arg, &oa, {1,2,3},
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                VA9, MovUtl::move(VA10), VA11, MovUtl::move(VA12),
                VA13),
               &da);
*/
}
#endif //BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS
void bslstl_optional_test25()
{
    // ------------------------------------------------------------------------
    // TESTING hash_append FUNCTION
    //
    //  This test will verify that hashing functionality works with
    //  bsl::optional objects
    //
    // Concerns:
    //: 1 Hashing a disengaged 'optional' is equivalent to appending 'false'
    //:   to the hash.
    //:
    //: 2 Hashing an engaged 'optional' is equivalent to appending 'true' to
    //;    the hash followed by the value.
    //
    // Plan:
    //: 1 Create a disengaged 'optional' of non allocator aware type 'TYPE',
    //:   and, for concern 1, verify that calling 'hashAppend' with such an
    //:   object yields the same value as invoking 'hashAppend' with 'false'.
    //: 2 Create an engaged 'optional' of non allocator aware type 'TYPE',
    //:   and verify that calling 'hashAppend' with such an object yields the
    //:   same value as invoking 'hashAppend' with 'true'.
    //: 3 Repeat step 1 and 2 using an allocator aware TYPE.
    //
    // Testing:
    //   void hashAppend(HASHALG& hashAlg, optional<TYPE>& input);
    // ------------------------------------------------------------------------
    if (verbose) printf("\nTESTING hash_append FUNCTION"
                        "\n============================\n");

   if (veryVerbose) printf("\tUsing non allocator aware 'value_type'.\n");
   {
       typedef size_t                          ValueType;

       TEST_HASH_EMPTY(ValueType);
       TEST_HASH_ENGAGED(ValueType, (0));
       TEST_HASH_ENGAGED(ValueType, (3));
       TEST_HASH_ENGAGED(ValueType, (777));
       TEST_HASH_ENGAGED(ValueType, (1056));

       TEST_HASH_EMPTY(const ValueType);
       TEST_HASH_ENGAGED(const ValueType, (0));
       TEST_HASH_ENGAGED(const ValueType, (3));
       TEST_HASH_ENGAGED(const ValueType, (777));
       TEST_HASH_ENGAGED(const ValueType, (1056));

   }
   if (veryVerbose) printf("\tUsing allocator aware 'value_type'.\n");
   {
       typedef bsl::string                          ValueType;


       bslma::TestAllocator da("default", veryVeryVeryVerbose);

       bslma::DefaultAllocatorGuard dag(&da);

       TEST_HASH_EMPTY(ValueType);
       TEST_HASH_ENGAGED(ValueType, (""));
       TEST_HASH_ENGAGED(ValueType, ("short string"));
       TEST_HASH_ENGAGED(ValueType, ("1234567890abcdefghijklmnopqrstuvwxyz~!@#"
                                     "$%^&*()_+-=`{}|[]\\\t\n "));
       TEST_HASH_ENGAGED(ValueType, ("1234567890abcdefghijklmnopqrstuvwxyz~!@#"
                                     "1234567890abcdefghijklmnopqrstuvwxyz~!@#"
                                     "1234567890abcdefghijklmnopqrstuvwxyz~!@")
                                     );

       TEST_HASH_EMPTY(const ValueType);
       TEST_HASH_ENGAGED(const ValueType, (""));
       TEST_HASH_ENGAGED(const ValueType, ("short string"));
       TEST_HASH_ENGAGED(const ValueType,
                         ("1234567890abcdefghijklmnopqrstuvwxyz~!@#"
                          "$%^&*()_+-=`{}|[]\\\t\n "));
       TEST_HASH_ENGAGED(const ValueType,
                         ("1234567890abcdefghijklmnopqrstuvwxyz~!@#"
                          "1234567890abcdefghijklmnopqrstuvwxyz~!@#"
                          "1234567890abcdefghijklmnopqrstuvwxyz~!@"));


   }
}
void bslstl_optional_test26()
{
    // --------------------------------------------------------------------
    // TESTING 'in_place_t' CONSTRUCTOR
    //   This test will verify that the 'in_place_t' constructor works
    //   as expected.
    //
    // Concerns:
    //: 1 Calling 'in_place_t' constructor creates an engaged 'optional' whose
    //:   'value_type' object is created using the constructors arguments.
    //: 2 Arguments to the 'in_place_t' constructor are forwarded in correct
    //:   order to the 'value_type' object's constructor.
    //: 3 Arguments to the 'in_place_t' constructor are perfect forwarded to
    //:   the 'value_type' object's constructor, that is, rvalue argument is
    //:   forwarded as an rvalue, and lvalue argument is forwarded as an lvalue
    //:   to the 'value_type' object's constructor
    //: 4 If the type uses an allocator and no allocator is provided, the
    //:   'optional' object will use the default allocator.
    //: 5 If the type uses an allocator and the allocator extended 'in_place_t'
    //:   constructor is used, the 'optional' object will use the allocator
    //:   provided in the constructor.
    //: 6 No unnecessary copies of the 'value_type' are created
    //
    //
    // Plan:
    //: 1 Using a non allocator aware 'value_type' TYPE, create an object of
    //:   'TYPE' using a set of arguments 'ARGS'. Create an 'optional' object
    //:   of 'value_type' 'TYPE' by invoking the 'in_place_t' constructor with
    //:   the same arguments. For concern 1, check that the value of the
    //:   'value_type' object in the 'optional' object matches the value of the
    //:   'TYPE' object.
    //: 2 For concern 2, in step 1 use different number of constructor
    //:   arguments.
    //: 3 For concern 3, in step 1 and 2, use a mixture of rvalue and lvalue
    //:   arguments such that each argument position is tested with an
    //:   rvalue and an lvalue. Check that the rvalue arguments have been
    //:   moved frome.
    //: 4 Repeat steps 1-3 with an allocator aware TYPE. For concern 4, check
    //:   that the default allocator was used to construct the 'value_type'
    //:   object.
    //: 5 in step 4, use the allocator extended constructor. For concern 5,
    //:   check that the allocator used in the constructor call is the
    //:   allocator of the 'optional' object.
    //: 6 In steps 1-5, check no unnecessary copies of the TYPE object have
    //:   been created.
    //
    // Testing:
    //
    //   void optional(allocator_arg, allocator, Args&&...);
    //   void optional(in_place_t, Args&&...);
    //   void optional(allocator_arg, allocator, in_place_t, Args&&...);
    //
    // --------------------------------------------------------------------

    if (verbose) printf(
                       "\nTESTING 'in_place_t' CONSTRUCTOR "
                       "\n================================\n");

    if (veryVerbose) printf("\tUsing 'ConstructTestTypeNoAlloc'.\n");
    {
        typedef ConstructTestTypeNoAlloc                  ValueType;
        test_copy_helper<ValueType, bsl::optional<ValueType> >();
    }

    if (veryVerbose) printf("\tUsing 'ConstructTestTypeAlloc'.\n");
    {
        typedef ConstructTestTypeAlloc                  ValueType;
        test_copyad_helper<ValueType, bsl::optional<ValueType> >();
        test_copya_helper<ValueType, bsl::optional<ValueType> >();

    }
    if (veryVerbose) printf("\tUsing 'ConstructTestTypeAllocArgT'.\n");
    {
        typedef ConstructTestTypeAllocArgT                ValueType;
        test_copyad_argt_helper<ValueType, bsl::optional<ValueType> >();
        test_copya_argt_helper<ValueType, bsl::optional<ValueType> >();
    }
}

void bslstl_optional_test27()
{
#ifdef BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS
    // --------------------------------------------------------------------
    // TESTING 'initializer_list' 'in_place_t' CONSTRUCTOR
    //   This test will verify that the 'initializer_list' 'in_place_t'
    //   constructor works as expected.
    //
    // Concerns:
    //: 1 Calling 'initializer_list' 'in_place_t' constructor creates an
    //:   engaged 'optional' whose 'value_type' object is created using the
    //:   constructors arguments.
    //: 2 Arguments to the 'initializer_list' 'in_place_t' constructor are
    //:   forwarded in correct order to the 'value_type' object's constructor.
    //: 3 Arguments to the 'initializer_list'  'in_place_t' constructor are
    //:   perfect forwarded to the 'value_type' object's constructor, that is,
    //:   rvalue argument is forwarded as an rvalue, and lvalue argument is
    //:   forwarded as an lvalue to the 'value_type' object's constructor.
    //: 4 If the type uses an allocator and no allocator is provided, the
    //:   'optional' object will use the default alllocator.
    //: 5 If the type uses an allocator and the allocator extended 'in_place_t'
    //:   constructor is used, the 'optional' object will use the alllocator
    //:   provided in the constructor.
    //: 6 No unnecessary copies of the 'value_type' are created.
    //: 7 It is possible to constructor const 'optional' objects and 'optional'
    //:   objects of const 'value_type'.
    //
    //
    // Plan:
    //: 1 Using a non allocator aware 'value_type' TYPE, create an object of
    //:   'TYPE' using a set of arguments 'ARGS'. Create an 'optional' object
    //:   of 'value_type' 'TYPE' by invoking the 'in_place_t' constructor with
    //:   the same arguments. For concern 1, check that the value of the
    //:   'value_type' object in the 'optional' object matches the value of the
    //:   'TYPE' object.
    //: 2 For concern 2, in step 1 use different number of constructor
    //:   arguments.
    //: 3 For concern 2, in step 1 and 2, use a mixture of rvalue and lvalue
    //:   arguments such that each argument position is tested with an
    //:   rvalue and an lvalue. Check that the rvalue arguments have been
    //:   moved frome.
    //: 4 Repeat steps 1-3 with an allocator aware type. For concern 4, check
    //:   that the default allocator was used to construct the 'value_type'
    //:   object.
    //: 5 in step 4, use the allocator extended constructor. For concern 5,
    //:   check that the allocator used in the constructor call is the
    //:   alloctor of the 'optional' object.
    //: 6 In steps 1-5, check no unnecessary copies of the 'value_type' object
    //:   have been created.
    //: 7 For concern 7, repeat steps 1-6 using a const qualified 'optional'
    //:   type and an 'optional' of const qualified 'value_type'.
    //
    // Plan:
    //
    //
    // Testing:
    //
    //   void optional(in_place_t, std::initializer_list<U>, Args&&...);
    //   void optional(allocator_arg, allocator, in_place_t,
    //                 std::initializer_list<U>, Args&&...);
    // --------------------------------------------------------------------

    if (verbose) printf(
                       "\nTESTING 'initializer_list' 'in_place_t' CONSTRUCTOR"
                       "\n==================================================="
                       "\n");

    if (veryVerbose) printf("\tUsing 'ConstructTestTypeNoAlloc'.\n");
    {
        typedef ConstructTestTypeNoAlloc                  ValueType;
        test_copyil_helper<ValueType, bsl::optional<ValueType> >();
        test_copyil_helper<ValueType, const bsl::optional<ValueType> >();
        test_copyil_helper<ValueType, bsl::optional< const ValueType> >();
    }

    if (veryVerbose) printf("\tUsing 'ConstructTestTypeAlloc'.\n");
    {
        typedef ConstructTestTypeAlloc                  ValueType;
        test_copyilad_helper<ValueType, bsl::optional<ValueType> >();
        test_copyilad_helper<ValueType, const bsl::optional<ValueType> >();
        test_copyilad_helper<ValueType, bsl::optional< const ValueType> >();

        test_copyila_helper<ValueType, bsl::optional<ValueType> >();
        test_copyila_helper<ValueType, const bsl::optional<ValueType> >();
        test_copyila_helper<ValueType, bsl::optional< const ValueType> >();
    }
    if (veryVerbose) printf("\tUsing 'ConstructTestTypeAllocArgT'.\n");
    {
        typedef ConstructTestTypeAllocArgTIL                ValueType;
        test_copyilad_argt_helper<ValueType, bsl::optional<ValueType> >();
        test_copyilad_argt_helper<ValueType, const bsl::optional<ValueType> >();
        test_copyilad_argt_helper<ValueType, bsl::optional< const ValueType> >();

        test_copyila_argt_helper<ValueType, bsl::optional<ValueType> >();
        test_copyila_argt_helper<ValueType, const bsl::optional<ValueType> >();
        test_copyila_argt_helper<ValueType, bsl::optional< const ValueType> >();
    }
#endif // BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS
}
void bslstl_optional_test28()
{
    // --------------------------------------------------------------------
    // TESTING 'operator=' OVERLOAD RESOLUTION
    //   This test will verify that the 'operator=' overload
    //   resolution works as expected in the presence of constructors and
    //   assignment operators of 'value_type' taking an 'optional'.
    //   In these tests we do not care about the different allocation argument
    //   policies because the allocator propagation is tested in the
    //   functionality tests for 'operator=. Here, we only care that the
    //   correct overload is selected.
    //   Similarly, we do not care about the allocator state after assignment,
    //   nor do we care about the difference in behaviour when the source and
    //   destination 'optional's are engaged/disengaged. We deliberately always
    //   use engaged 'optional's as that directly forwards to the underlying
    //   'value_type' assignment.
    //
    // Concerns:
    //: 1 If 'value_type' 'TYPE' is assignable and constructible from an
    //:   'optional' type 'OPT_TYPE', the assignment of 'OPT_TYPE' to an
    //:   'optional<TYPE>' assigns the 'OPT_TYPE' object to the 'value_type'
    //:   object of 'optional<TYPE>'. The resulting 'optional' is always
    //:   engaged.
    //: 2 If 'value_type' 'TYPE' is not assignable and constructible from an
    //:   'optional' type 'OPT_TYPE', the assignment of an engaged 'OPT_TYPE'
    //:   to an 'optional<TYPE>' assigns the 'value_type' object of the
    //:   'OPT_TYPE' object to the 'value_type' object of 'optional<TYPE>'.
    //: 3 Move assign and copy assign have the same overload resolution.
    //: 4 assigning '{}' to an 'optional' is interpreted as assigning a default
    //:   constructed 'optional' of same type. The resulting 'optional' is
    //:   disengaged.
    //: 5 Assigning a 'nullopt_t' object to an 'optional' object is always
    //:   interpreted as assigning a disengaged 'optional' of same type.
    //: 6 'optional' types of non allocator aware 'value_type' and allocator
    //:   aware type have the same overload resolution.
    //:
    // Plan:
    //: 1 Using a non allocator aware TYPE, create a source object of
    //:   'OPT_TYPE', where 'OPT_TYPE' is 'optional<TYPE>'. Assign the source
    //:   object to an object of 'optional<OPT_TYPE>'. For concern 1, check the
    //:   resulting object is engaged and that its 'value_type' object matches
    //:   the source object.
    //: 2 Assign an engaged object of 'optional<OPT_TYPE>' to another object
    //:   of 'optional<OPT_TYPE>'. For concern 2, check the 'value_type' of
    //:   the destination object matches the 'value_type' of the source object.
    //: 3 For concern 3, in steps 1 and 2 use rvalues.
    //: 4 For concern 4, assign '{}' to an object of 'OPT_TYPE' and verify
    //:   the resulting 'OPT_TYPE' object is disengaged.
    //: 5 For concern 4, assign '{}' to an object of 'optional<OPT_TYPE>' and
    //:   verify the resulting 'optional<OPT_TYPE>' object is disengaged.
    //: 6 For concern 5, assign 'nullopt_t' object to an object of 'OPT_TYPE'
    //:   and verify the resulting 'OPT_TYPE' object is disengaged.
    //: 7 For concern 5, assign 'nullopt_t' object to an object of
    //:   'optional<OPT_TYPE>' and  verify the resulting 'optional<OPT_TYPE>'
    //:   object is disengaged.
    //: 8 For concern 6, repeat steps 1-7 with an allocator awar TYPE.
    //
    // Testing:
    //
    //   optional& operator= overload set
    //
    // --------------------------------------------------------------------

    if (verbose) printf(
                       "\nTESTING 'operator=' OVERLOAD RESOLUTION"
                       "\n=======================================\n");

    if (veryVerbose) printf("\tUsing non allocator aware 'value_type'.\n");
    {
        typedef int                             ValueType;
        typedef bsl::optional<ValueType>        OptV;
        typedef const OptV                      COptV;
        typedef bsl::optional<OptV>             Obj;
        typedef bsl::optional<COptV>            ObjC;

        OptV source = 4;
        Obj destination;
        destination = source;
        ASSERT(destination.has_value());
        ASSERT(destination.value().has_value());
        ASSERT(destination.value().value() == 4);

#ifdef BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS
        source = {};
        ASSERT(!source.has_value());
        destination = {};
        ASSERT(!destination.has_value());
#endif //BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS

        source = 5;
        destination = MovUtl::move(source);
        ASSERT(destination.has_value());
        ASSERT(destination.value().value() == 5);

        source = 8;
        ASSERT(source.has_value());
        ASSERT(source.value() == 8);
        destination = 9;
        ASSERT(destination.has_value());
        ASSERT(destination.value().value() == 9);

        COptV source2;
        destination = source2;
        ASSERT(destination.has_value());
        ASSERT(!destination.value().has_value());

#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
        // C++03 MovableRef isn't const friendly which will make this test fail
        destination = 4;
        destination = MovUtl::move(source2);
        ASSERT(destination.has_value());
        ASSERT(!destination.value().has_value());
#endif //BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES

        source = bsl::nullopt;
        ASSERT(!source.has_value());
        destination = bsl::nullopt;
        ASSERT(!destination.has_value());

        Obj source3;
        destination = source3;
        ASSERT(!destination.has_value());

        ObjC source4;
        destination = source4;
        ASSERT(!destination.has_value());

    }
    if (veryVerbose) printf("\tUsing allocator aware 'value_type'.\n");
    {
        typedef bsl::string                     ValueType;
        typedef bsl::optional<ValueType>        OptV;
        typedef const OptV                      COptV;

        typedef bsl::optional<COptV>            ObjC;
        typedef bsl::optional<OptV>             Obj;

        OptV source("test26");
        Obj destination;
        destination = source;
        ASSERT(destination.has_value());
        ASSERT(destination.value().has_value());
        ASSERT(destination.value().value() == "test26");

#ifdef BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS
        source = {};
        ASSERT(!source.has_value());
        destination = {};
        ASSERT(!destination.has_value());
#endif //BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS


        source = "another test string";
        destination = MovUtl::move(source);
        ASSERT(destination.has_value());
        ASSERT(destination.value().value() == "another test string");

        COptV source2("test26");
        destination = source2;
        ASSERT(destination.has_value());
        ASSERT(destination.value().has_value());
        ASSERT(destination.value().value() == "test26");

        source = "booboo";
        ASSERT(source.has_value());
        ASSERT(source.value() == "booboo");
        destination = "haha";
        ASSERT(destination.has_value());
        ASSERT(destination.value().value() == "haha");

#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
        // C++03 MovableRef isn't const friendly which will make this test fail
        destination = MovUtl::move(source2);
        ASSERT(destination.has_value());
        ASSERT(destination.value().has_value());
        ASSERT(destination.value().value() == "test26");
#endif //BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES

        source = bsl::nullopt;
        ASSERT(!source.has_value());
        destination = bsl::nullopt;
        ASSERT(!destination.has_value());

        Obj source3;
        destination = source3;
        ASSERT(!destination.has_value());

        ObjC source4;
        destination = source4;
        ASSERT(!destination.has_value());
    }
}

void bslstl_optional_test29()
{
    // --------------------------------------------------------------------
    // TESTING 'swap' METHOD
    // Concerns:
    //: 1 Swap of two disengaged objects is a no-op,
    //: 2 Swap of an engaged and a disengaged 'optional' moves the value
    //:   from the engaged object to another without calling swap for the
    //:   'value_type'.
    //: 3 Swap of two engaged objects calls swap for the 'value_type'.
    //: 4 Behaviour is the same for 'swap' member funciton, and 'swap' free
    //:   function
    //: 5 Behaviour is the same for allocator aware an non allocator aware
    //:   'value_type's.
    //
    // Plan:
    //: 1 Using a non allocator aware 'value_type', call 'swap' free
    //:   function with two disengaged 'optional' objects and, for concern 1,
    //:   verify 'value_type' 'swap' has not been called.
    //: 2 Using a non allocator aware 'value_type', call 'swap 'free
    //:   function with an engaged 'optional' object and a disengaged
    //:   'optional' object. For concern 2, verify the disengaged 'optional'
    //:   is now engaged with a value from the other object, and that the other
    //:   object is now disengaged. Verify that the 'value_type' 'swap' has not
    //:   been called.
    //: 3 Using a non allocator aware 'value_type', call swap free function
    //:   with two engaged 'optional' objects and, for concern 2, verify
    //:   'value_type' 'swap' has been called.
    //: 4 Repeat steps 1-3, and for concern 4, use the 'swap' member function.
    //: 5 Repeat steps 1-4, and for concern 5, use an allocator aware 'TYPE'.
    //
    //
    // Testing:
    //   void swap(optional<TYPE>& other);
    //   void swap(optional<TYPE>& lhs,optional<TYPE>& rhs);
    // --------------------------------------------------------------------

    if (verbose) printf("\nTESTING 'swap' METHOD"
                        "\n=====================\n");

    using bsl::swap;
    {

        bsl::optional<Swappable> a;
        bsl::optional<Swappable> b;

        Swappable::swapReset();
        swap(a, b);

        ASSERT(!Swappable::swapCalled());
        ASSERT(!a.has_value());
        ASSERT(!b.has_value());

        Swappable::swapReset();
        a.swap(b);

        ASSERT(!Swappable::swapCalled());
        ASSERT(!a.has_value());
        ASSERT(!b.has_value());
    }
    {
        Swappable obj1(1);
        Swappable obj2(2);

        const Swappable Zobj1(obj1);
        const Swappable Zobj2(obj2);

        bsl::optional<Swappable> a = obj1;
        bsl::optional<Swappable> b = obj2;
        ASSERT(a.value() == Zobj1);
        ASSERT(b.value() == Zobj2);

        Swappable::swapReset();
        ASSERT(!Swappable::swapCalled());
        swap(a, b);
        ASSERT( Swappable::swapCalled());

        ASSERT(b.value() == Zobj1);
        ASSERT(a.value() == Zobj2);

        Swappable::swapReset();
        ASSERT(!Swappable::swapCalled());
        a.swap(b);
        ASSERT( Swappable::swapCalled());

        ASSERT(a.value() == Zobj1);
        ASSERT(b.value() == Zobj2);
    }
    {
        bsl::optional<Swappable> nonNullObj(Swappable(10));
        bsl::optional<Swappable> nonNullObjCopy(nonNullObj);
        bsl::optional<Swappable> nullObj;

        Swappable::swapReset();
        swap(nonNullObj, nullObj);

        ASSERT(!SwappableAA::swapCalled());
        ASSERT(nonNullObjCopy == nullObj);
        ASSERT(!nonNullObj.has_value());

        Swappable::swapReset();
        nonNullObj.swap(nullObj);

        ASSERT(!Swappable::swapCalled());
        ASSERT(nonNullObjCopy == nonNullObj);
        ASSERT(!nullObj.has_value());
    }
    {

        bsl::optional<SwappableAA> a;
        bsl::optional<SwappableAA> b;

        SwappableAA::swapReset();
        swap(a, b);

        ASSERT(!SwappableAA::swapCalled());
        ASSERT(!a.has_value());
        ASSERT(!b.has_value());

        SwappableAA::swapReset();
        a.swap(b);

        ASSERT(!SwappableAA::swapCalled());
        ASSERT(!a.has_value());
        ASSERT(!b.has_value());
    }
    {
        SwappableAA obj1(1);
        SwappableAA obj2(2);

        const SwappableAA Zobj1(obj1);
        const SwappableAA Zobj2(obj2);

        bsl::optional<SwappableAA> a = obj1;
        bsl::optional<SwappableAA> b = obj2;
        ASSERT(a.value() == Zobj1);
        ASSERT(b.value() == Zobj2);

        SwappableAA::swapReset();
        ASSERT(!SwappableAA::swapCalled());
        swap(a, b);
        ASSERT( SwappableAA::swapCalled());

        ASSERT(b.value() == Zobj1);
        ASSERT(a.value() == Zobj2);

        SwappableAA::swapReset();
        ASSERT(!SwappableAA::swapCalled());
        a.swap(b);
        ASSERT( SwappableAA::swapCalled());

        ASSERT(a.value() == Zobj1);
        ASSERT(b.value() == Zobj2);
    }
    {
        bsl::optional<SwappableAA> nonNullObj(SwappableAA(10));
        bsl::optional<SwappableAA> nonNullObjCopy(nonNullObj);
        bsl::optional<SwappableAA> nullObj;

        SwappableAA::swapReset();
        swap(nonNullObj, nullObj);

        ASSERT(!SwappableAA::swapCalled());
        ASSERT(nonNullObjCopy == nullObj);
        ASSERT(!nonNullObj.has_value());

        SwappableAA::swapReset();
        nonNullObj.swap(nullObj);

        ASSERT(!SwappableAA::swapCalled());
        ASSERT(nonNullObjCopy == nonNullObj);
        ASSERT(!nullObj.has_value());
    }
}
void bslstl_optional_test30()
{
    // --------------------------------------------------------------------
    // TESTING RELATIONAL OPERATORS
    //
    // Concerns:
    //: 1 Two 'optional' objects can be compared if their 'value_type's are
    //:   comparable. The result depends on whether the objects are engaged
    //:   or not.
    //: 2 We can compare an 'optional' object of 'value_type' V and a non
    //:   'optional' object of type U if U and V are comparable types. The
    //:   result depends on whether the 'optional' object is engaged or not.
    //: 3 We can compare any 'optional' object with nulllopt_t. The result
    //:   depends on whether the 'optional' object is engaged or not.
    //
    // Plan:
    //: 1 For each relation operator, for concern 1, compare two 'optional'
    //:   objects of comparable valye types. Execute tests for a combination of
    //:   engaged and disengaged 'optional' objects.
    //: 2 For each relation operator, for concern 2, compare an 'optional'
    //:   object of type 'TYPE', and an object of type comparable to TYPE.
    //:   Execute tests for an engaged and disengaged 'optional' object.
    //: 3 For each relation operator, for concern 3, compare an 'optional'
    //:   object and a 'nullopt_t' object. Execute tests for an engaged and
    //:   disengaged 'optional' object.
    //
    // Testing:
    //   bool operator==(const optional<LHS>&, const optional<RHS>&)
    //   bool operator==(const optional<LHS>&, const RHS&)
    //   bool operator==(const LHS&, const optional<RHS>&)
    //   bool operator==(const optional<LHS>&, bsl::nullopt_t)
    //   bool operator==(bsl::nullopt_t,         const optional<RHS>&)
    //   bool operator!=(const optional<LHS>&, const optional<RHS>&)
    //   bool operator!=(const optional<LHS>&, const RHS&)
    //   bool operator!=(const LHS&, const optional<RHS>&)
    //   bool operator!=(const optional<LHS>&, bsl::nullopt_t)
    //   bool operator!=(bsl::nullopt_t,         const optional<RHS>&)
    //   bool operator<=(const optional<LHS>&, const optional<RHS>&)
    //   bool operator<=(const optional<LHS>&, const RHS&)
    //   bool operator<=(const LHS&, const optional<RHS>&)
    //   bool operator<=(const optional<LHS>&, bsl::nullopt_t)
    //   bool operator<=(bsl::nullopt_t,         const optional<RHS>&)
    //   bool operator>=(const optional<LHS>&, const optional<RHS>&)
    //   bool operator>=(const optional<LHS>&, const RHS&)
    //   bool operator>=(const LHS&, const optional<RHS>&)
    //   bool operator>=(const optional<LHS>&, bsl::nullopt_t)
    //   bool operator>=(bsl::nullopt_t,         const optional<RHS>&)
    //   bool operator< (const optional<LHS>&, const optional<RHS>&)
    //   bool operator< (const optional<LHS>&, const RHS&)
    //   bool operator< (const LHS&, const optional<RHS>&)
    //   bool operator< (const optional<LHS>&, bsl::nullopt_t)
    //   bool operator< (bsl::nullopt_t,         const optional<RHS>&)
    //   bool operator> (const optional<LHS>&, const optional<RHS>&)
    //   bool operator> (const optional<LHS>&, const RHS&)
    //   bool operator> (const LHS&, const optional<RHS>&)
    //   bool operator> (const optional<LHS>&, bsl::nullopt_t)
    //   bool operator> (bsl::nullopt_t,         const optional<RHS>&)


    if (verbose) printf("\nTESTING RELATIONAL OPERATORS"
                        "\n============================\n");
    if (veryVerbose) printf("\tComparison with an 'optional'.\n");
    {
        typedef bsl::optional<int>                    OptT;
        typedef bsl::optional<MyClass2>              OptV;

        OptT X;
        OptV Y;

        //comparing two disengaged optionals
        ASSERT(  X == Y  ); // If bool(x) != bool(y), false;
        ASSERT(!(X != Y) ); // If bool(x) != bool(y), true;
                            // otherwise, if bool(x) == false, false;
        ASSERT(!(X <  Y) ); // If !y, false;
        ASSERT(!(X >  Y) ); // If !x, false;
        ASSERT(  X <= Y  ); // If !x, true;
        ASSERT(  X >= Y  ); // If !y, true;

        //rhs disengaged, lhs engaged
        Y.emplace(3);
        ASSERT(!(X == Y) ); // If bool(x) != bool(y), false;
        ASSERT( (X != Y) ); // If bool(x) != bool(y), true;
                            // otherwise, if bool(x) == false, false;
        ASSERT( (X <  Y) ); // If !y, false; otherwise, if !x, true;
        ASSERT(!(X >  Y) ); // If !x, false;
        ASSERT(  X <= Y  ); // If !x, true;
        ASSERT(!(X >= Y) ); // If !y, true; otherwise, if !x, false;

        //rhs engaged, lhs disengaged
        X.emplace(5);
        Y.reset();
        ASSERT(!(X == Y) ); // If bool(x) != bool(y), false;
        ASSERT( (X != Y) ); // If bool(x) != bool(y), true;
                            // otherwise, if bool(x) == false, false;
        ASSERT(!(X <  Y) ); // If !y, false; otherwise, if !x, true;
        ASSERT( (X >  Y) ); // If !x, false; otherwise, if !y, true;
        ASSERT(!(X <= Y) ); // If !x, true; otherwise, if !y, false;
        ASSERT( (X >= Y) ); // If !y, true; otherwise, if !x, false;

        //both engaged, compare the values
        X.emplace(1);
        Y.emplace(3);
        ASSERT(!(X == Y) ); // If bool(x) != bool(y), false;
        ASSERT( (X != Y) ); // If bool(x) != bool(y), true;
                            // otherwise, if bool(x) == false, false;
        ASSERT( (X <  Y) ); // If !y, false; otherwise, if !x, true;
        ASSERT(!(X >  Y) ); // If !x, false; otherwise, if !y, true;
        ASSERT( (X <= Y) ); // If !x, true; otherwise, if !y, false;
        ASSERT(!(X >= Y) ); // If !y, true; otherwise, if !x, false;
    }
    if (veryVerbose) printf("\tComparison with a non 'optional' .\n");
    {
        typedef bsl::optional<MyClass2>              OptV;
        OptV X;
        int Y = 3;

        //comparison with a disengaged optional on rhs
        ASSERT(!(X == Y) ); // return bool(x) ? *x == v : false;
        ASSERT( (X != Y) ); // return bool(x) ? *x != v : true;
        ASSERT( (X <  Y) ); // return bool(x) ? *x < v : true;
        ASSERT(!(X >  Y) ); // return bool(x) ? *x > v : false;
        ASSERT( (X <= Y) ); // return bool(x) ? *x <= v : true;
        ASSERT(!(X >= Y) ); // return bool(x) ? *x >= v : false;

        //comparison with a disengaged optional on lhs
        ASSERT(!(Y == X) ); // return bool(x) ? v == *x : false;
        ASSERT( (Y != X) ); // return bool(x) ? v != *x : true;
        ASSERT(!(Y <  X) ); // return bool(x) ? v < *x : false;
        ASSERT( (Y >  X) ); // return bool(x) ? v > *x : true;
        ASSERT(!(Y <= X) ); // return bool(x) ? v <= *x : false;
        ASSERT( (Y >= X) ); // return bool(x) ? v >= *x : true;

        //comparison with an engaged optional on rhs
        X.emplace(7);
        ASSERT(!(X == Y) ); // If bool(x) != bool(y), false;
        ASSERT( (X != Y) ); // If bool(x) != bool(y), true;
                            // otherwise, if bool(x) == false, false;
        ASSERT(!(X <  Y) ); // If !y, false; otherwise, if !x, true;
        ASSERT( (X >  Y) ); // If !x, false;
        ASSERT(!(X <= Y) ); // If !x, true;
        ASSERT( (X >= Y) ); // If !y, true; otherwise, if !x, false;

        //comparison with an engaged optional on lhs
        ASSERT(!(Y == X) ); // If bool(x) != bool(y), false;
        ASSERT( (Y != X) ); // If bool(x) != bool(y), true;
                            // otherwise, if bool(x) == false, false;
        ASSERT( (Y <  X) ); // If !y, false; otherwise, if !x, true;
        ASSERT(!(Y >  X) ); // If !x, false;
        ASSERT( (Y <= X) ); // If !x, true;
        ASSERT(!(Y >= X) ); // If !y, true; otherwise, if !x, false;
    }
    if (veryVerbose) printf("\tComparison with a nullopt_t .\n");
    {
        bsl::optional<MyClass2> X;

        //comparison with a disengaged optional on rhs
        ASSERT( (X == bsl::nullopt) ); // !x
        ASSERT(!(X != bsl::nullopt) ); // bool(x)
        ASSERT(!(X <  bsl::nullopt) ); // false
        ASSERT(!(X >  bsl::nullopt) ); // bool(x)
        ASSERT( (X <= bsl::nullopt) ); // !x
        ASSERT( (X >= bsl::nullopt) ); // true

        //comparison with a disengaged optional on lhs
        ASSERT( (bsl::nullopt == X) ); // !x
        ASSERT(!(bsl::nullopt != X) ); // bool(x)
        ASSERT(!(bsl::nullopt <  X) ); // bool(x)
        ASSERT(!(bsl::nullopt >  X) ); // false
        ASSERT( (bsl::nullopt <= X) ); // true
        ASSERT( (bsl::nullopt >= X) ); // !x

        //comparison with an engaged optional on rhs
        X.emplace(7);
        ASSERT(!(X == bsl::nullopt) ); // !x
        ASSERT( (X != bsl::nullopt) ); // bool(x)
        ASSERT(!(X <  bsl::nullopt) ); // false
        ASSERT( (X >  bsl::nullopt) ); // bool(x)
        ASSERT(!(X <= bsl::nullopt) ); // !x
        ASSERT( (X >= bsl::nullopt) ); // true

        //comparison with an engaged optional on lhs
        ASSERT(!(bsl::nullopt == X) ); // !x
        ASSERT( (bsl::nullopt != X) ); // bool(x)
        ASSERT( (bsl::nullopt <  X) ); // bool(x)
        ASSERT(!(bsl::nullopt >  X) ); // false
        ASSERT( (bsl::nullopt <= X) ); // true
        ASSERT(!(bsl::nullopt >= X) ); // !x
    }
}
void bslstl_optional_test31()
{
    // --------------------------------------------------------------------
    // TESTING 'make_optional' FACILITY
    //
    //  In this test, we are ensuring that the 'optional' created using
    //  'make_optional' facility is created using the given arguments and
    //  without unnecessary copies. We are not worried about the allocator
    //  policy of the allocator aware type, as choosing the allocator policy
    //  when creating an 'optional' is tested in constructor tests.
    //
    //
    // Concerns:
    //: 1 Invoking 'make_optional' creates an 'optional' with the value of the
    //:   arguments.
    //: 2 Multiple arguments are correctly forwarded.
    //: 3 Arguments are perfectly forwarded.
    //: 4 If the 'value_type' is allocator aware, default allocator is used to
    //:   construct the 'optional' object.
    //: 5 No unnecessary objects of 'value_type' are created.
    //
    //
    // Plan:
    //: 1 Call 'make_optional' to create an 'optional' of non allocator aware
    //:   'value_type'. As an argument to 'make_optional' use an lvalue of
    //:   the 'value_type'. For concern 1, check the resulting 'optional' object
    //:   has the expected value.
    //: 2 Repeat step 1 using an rvalue of the 'value_type' as an argument to
    //:   'make_optional'. For concern 2, check the resulting 'optional' object
    //:   has been created using a move constructor.
    //: 3 Using various number of arguments to 'make_optional', and a
    //:   combination of lvalue an  rvalue for each argument poisiont, for
    //:   concern 2, check that each argument is correctly forwarded to the
    //:   'value_type' constructor. For concern 3, check that an lvalue argument
    //:   is forwarded as an lvalue to the constructor, and rvalue argument is
    //:   forwarded as an rvalue.
    //: 4 Repeat step 3 using an 'initializer_list' as the first argument.
    //: 5 Repeat steps 1-4 using an 'optional' of allocator aware type. For
    //:   concern 4, check that the default allocator is used.
    //: 6 In steps 1-5, for concern 5, verify no additional copies of the value
    //:   type are created.
    //
    // Testing:
    //      make_optional(T&&);
    //      make_optional(Args&&... args);
    //      make_optional(initializer_list<U> il, Args&&... args);

    if (verbose) printf("\nTESTING 'make_optional' FACILITY"
                        "\n================================\n");

    if (veryVerbose) printf("\tDeduced type make_optional.\n");
    {
        MyClass1 source(2);

        int CCI = MyClass1::copyConstructorInvocations;
        int MCI = MyClass1::moveConstructorInvocations;

        optional<MyClass1> obj = bsl::make_optional(source);
        ASSERT( 2 == obj.value().value());
        ASSERT( CCI == (MyClass1::copyConstructorInvocations -1));
        ASSERT( MCI == MyClass1::moveConstructorInvocations);

        CCI = MyClass1::copyConstructorInvocations;
        MCI = MyClass1::moveConstructorInvocations;

        optional<MyClass1> obj2 = bsl::make_optional(MovUtl::move(source));
        ASSERT( 2 == obj2.value().value());
        ASSERT( CCI == (MyClass1::copyConstructorInvocations));
        ASSERT( MCI == (MyClass1::moveConstructorInvocations  - 1));
    }
    {
        bslma::TestAllocator da("default", veryVeryVeryVerbose);
        bslma::DefaultAllocatorGuard dag(&da);
        MyClass2 source(2);

        int CCI = MyClass2::copyConstructorInvocations;
        int MCI = MyClass2::moveConstructorInvocations;

        optional<MyClass2> obj = bsl::make_optional(source);
        ASSERT( 2 == obj.value().value());
        ASSERT( &da == obj.value().d_def.d_allocator_p);
        ASSERT( CCI == (MyClass2::copyConstructorInvocations -1));
        ASSERT( MCI == MyClass2::moveConstructorInvocations);

        CCI = MyClass2::copyConstructorInvocations;
        MCI = MyClass2::moveConstructorInvocations;

        optional<MyClass2> obj2 = bsl::make_optional(MovUtl::move(source));
        ASSERT( 2 == obj2.value().value());
        ASSERT( &da == obj2.value().d_def.d_allocator_p);
        ASSERT( CCI == (MyClass2::copyConstructorInvocations));
        ASSERT( MCI == (MyClass2::moveConstructorInvocations  - 1));
    }
    {
        bslma::TestAllocator da("default", veryVeryVeryVerbose);
        bslma::DefaultAllocatorGuard dag(&da);
        MyClass2a source(2);

        int CCI = MyClass2a::copyConstructorInvocations;
        int MCI = MyClass2a::moveConstructorInvocations;

        optional<MyClass2a> obj = bsl::make_optional(source);
        ASSERT( 2 == obj.value().value());
        ASSERT( &da == obj.value().d_data.d_def.d_allocator_p);
        ASSERT( CCI == (MyClass2a::copyConstructorInvocations -1));
        ASSERT( MCI == MyClass2a::moveConstructorInvocations);

        CCI = MyClass2a::copyConstructorInvocations;
        MCI = MyClass2a::moveConstructorInvocations;

        optional<MyClass2a> obj2 = bsl::make_optional(MovUtl::move(source));
        ASSERT( 2 == obj2.value().value());
        ASSERT( &da == obj2.value().d_data.d_def.d_allocator_p);
        ASSERT( CCI == (MyClass2a::copyConstructorInvocations));
        ASSERT( MCI == (MyClass2a::moveConstructorInvocations  - 1));
    }
    if (veryVerbose) printf("\tvar arg make optional.\n");
    {
        bslma::TestAllocator da("default", veryVeryVeryVerbose);
        bslma::DefaultAllocatorGuard dag(&da);
        TEST_MAKEOP((VA1),&da);
        TEST_MAKEOP((MovUtl::move(VA1)),&da);

        TEST_MAKEOP((MovUtl::move(VA1), VA2),&da);
        TEST_MAKEOP((VA1, MovUtl::move(VA2)),&da);

        TEST_MAKEOP((MovUtl::move(VA1), VA2, VA3),&da);
        TEST_MAKEOP((VA1, MovUtl::move(VA2), VA3),&da);
        TEST_MAKEOP((VA1, VA2, MovUtl::move(VA3)),&da);

        TEST_MAKEOP((MovUtl::move(VA1), VA2,
                     MovUtl::move(VA3), VA4),&da);
        TEST_MAKEOP((VA1, MovUtl::move(VA2),
                     VA3, MovUtl::move(VA4)),&da);

        TEST_MAKEOP((MovUtl::move(VA1), VA2,
                     MovUtl::move(VA3), VA4,
                     MovUtl::move(VA5)),&da);
        TEST_MAKEOP((VA1, MovUtl::move(VA2),
                     VA3, MovUtl::move(VA4),
                     VA5),&da);
/*
        TEST_MAKEOP((MovUtl::move(VA1), VA2,
                     MovUtl::move(VA3), VA4,
                     MovUtl::move(VA5), VA6),&da);
        TEST_MAKEOP((VA1, MovUtl::move(VA2),
                     VA3, MovUtl::move(VA4),
                     VA5, MovUtl::move(VA6)),&da);

        TEST_MAKEOP((MovUtl::move(VA1), VA2,
                     MovUtl::move(VA3), VA4,
                     MovUtl::move(VA5), VA6,
                     MovUtl::move(VA7)),&da);
        TEST_MAKEOP((VA1, MovUtl::move(VA2),
                     VA3, MovUtl::move(VA4),
                     VA5, MovUtl::move(VA6),
                     VA7),&da);

        TEST_MAKEOP((MovUtl::move(VA1), VA2,
                     MovUtl::move(VA3), VA4,
                     MovUtl::move(VA5), VA6,
                     MovUtl::move(VA7), VA8),&da);
        TEST_MAKEOP((VA1, MovUtl::move(VA2),
                     VA3, MovUtl::move(VA4),
                     VA5, MovUtl::move(VA6),
                     VA7, MovUtl::move(VA8)),&da);

        TEST_MAKEOP((MovUtl::move(VA1), VA2,
                     MovUtl::move(VA3), VA4,
                     MovUtl::move(VA5), VA6,
                     MovUtl::move(VA7), VA8,
                     MovUtl::move(VA9)),&da);
        TEST_MAKEOP((VA1, MovUtl::move(VA2),
                     VA3, MovUtl::move(VA4),
                     VA5, MovUtl::move(VA6),
                     VA7, MovUtl::move(VA8),
                     VA9),&da);

        TEST_MAKEOP((MovUtl::move(VA1), VA2,
                     MovUtl::move(VA3), VA4,
                     MovUtl::move(VA5), VA6,
                     MovUtl::move(VA7), VA8,
                     MovUtl::move(VA9), VA10),&da);
        TEST_MAKEOP((VA1, MovUtl::move(VA2),
                     VA3, MovUtl::move(VA4),
                     VA5, MovUtl::move(VA6),
                     VA7, MovUtl::move(VA8),
                     VA9, MovUtl::move(VA10)),&da);

        TEST_MAKEOP((MovUtl::move(VA1), VA2,
                     MovUtl::move(VA3), VA4,
                     MovUtl::move(VA5), VA6,
                     MovUtl::move(VA7), VA8,
                     MovUtl::move(VA9), VA10,
                     MovUtl::move(VA11)),&da);
        TEST_MAKEOP((VA1, MovUtl::move(VA2),
                     VA3, MovUtl::move(VA4),
                     VA5, MovUtl::move(VA6),
                     VA7, MovUtl::move(VA8),
                     VA9, MovUtl::move(VA10),
                     VA11),&da);

        TEST_MAKEOP((MovUtl::move(VA1), VA2,
                     MovUtl::move(VA3), VA4,
                     MovUtl::move(VA5), VA6,
                     MovUtl::move(VA7), VA8,
                     MovUtl::move(VA9), VA10,
                     MovUtl::move(VA11), VA12),&da);
        TEST_MAKEOP((VA1, MovUtl::move(VA2),
                     VA3, MovUtl::move(VA4),
                     VA5, MovUtl::move(VA6),
                     VA7, MovUtl::move(VA8),
                     VA9, MovUtl::move(VA10),
                     VA11, MovUtl::move(VA12)),&da);

        TEST_MAKEOP((MovUtl::move(VA1), VA2,
                     MovUtl::move(VA3), VA4,
                     MovUtl::move(VA5), VA6,
                     MovUtl::move(VA7), VA8,
                     MovUtl::move(VA9), VA10,
                     MovUtl::move(VA11), VA12,
                     MovUtl::move(VA13)),&da);
        TEST_MAKEOP((VA1, MovUtl::move(VA2),
                     VA3, MovUtl::move(VA4),
                     VA5, MovUtl::move(VA6),
                     VA7, MovUtl::move(VA8),
                     VA9, MovUtl::move(VA10),
                     VA11, MovUtl::move(VA12),
                     VA13),&da);

        TEST_MAKEOP((MovUtl::move(VA1), VA2,
                     MovUtl::move(VA3), VA4,
                     MovUtl::move(VA5), VA6,
                     MovUtl::move(VA7), VA8,
                     MovUtl::move(VA9), VA10,
                     MovUtl::move(VA11), VA12,
                     MovUtl::move(VA13), VA14),&da);
        TEST_MAKEOP((VA1, MovUtl::move(VA2),
                     VA3, MovUtl::move(VA4),
                     VA5, MovUtl::move(VA6),
                     VA7, MovUtl::move(VA8),
                     VA9, MovUtl::move(VA10),
                     VA11, MovUtl::move(VA12),
                     VA13, MovUtl::move(VA14)),&da);
*/
    }

#ifdef BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS
    if (veryVerbose) printf("\tinitializer_list make optional.\n");
    {
       bslma::TestAllocator da("default", veryVeryVeryVerbose);
       bslma::DefaultAllocatorGuard dag(&da);
       TEST_MAKEOP(({1,2,3}),&da);

       TEST_MAKEOP(({1,2,3}, VA1),&da);
       TEST_MAKEOP(({1,2,3}, MovUtl::move(VA1)),&da);

       TEST_MAKEOP(({1,2,3}, MovUtl::move(VA1), VA2),&da);
       TEST_MAKEOP(({1,2,3}, VA1, MovUtl::move(VA2)),&da);

       TEST_MAKEOP(({1,2,3}, MovUtl::move(VA1), VA2, VA3),&da);
       TEST_MAKEOP(({1,2,3}, VA1, MovUtl::move(VA2), VA3),&da);
       TEST_MAKEOP(({1,2,3}, VA1, VA2, MovUtl::move(VA3)),&da);

       TEST_MAKEOP(({1,2,3}, MovUtl::move(VA1), VA2,
                    MovUtl::move(VA3), VA4),&da);
       TEST_MAKEOP(({1,2,3}, VA1, MovUtl::move(VA2),
                    VA3, MovUtl::move(VA4)),&da);

       TEST_MAKEOP(({1,2,3}, MovUtl::move(VA1), VA2,
                    MovUtl::move(VA3), VA4,
                    MovUtl::move(VA5)),&da);
       TEST_MAKEOP(({1,2,3}, VA1, MovUtl::move(VA2),
                    VA3, MovUtl::move(VA4),
                    VA5),&da);
/*
       TEST_MAKEOP(({1,2,3}, MovUtl::move(VA1), VA2,
                    MovUtl::move(VA3), VA4,
                    MovUtl::move(VA5), VA6),&da);
       TEST_MAKEOP(({1,2,3}, VA1, MovUtl::move(VA2),
                    VA3, MovUtl::move(VA4),
                    VA5, MovUtl::move(VA6)),&da);

       TEST_MAKEOP(({1,2,3}, MovUtl::move(VA1), VA2,
                    MovUtl::move(VA3), VA4,
                    MovUtl::move(VA5), VA6,
                    MovUtl::move(VA7)),&da);
       TEST_MAKEOP(({1,2,3}, VA1, MovUtl::move(VA2),
                    VA3, MovUtl::move(VA4),
                    VA5, MovUtl::move(VA6),
                    VA7),&da);

       TEST_MAKEOP(({1,2,3}, MovUtl::move(VA1), VA2,
                    MovUtl::move(VA3), VA4,
                    MovUtl::move(VA5), VA6,
                    MovUtl::move(VA7), VA8),&da);
       TEST_MAKEOP(({1,2,3}, VA1, MovUtl::move(VA2),
                    VA3, MovUtl::move(VA4),
                    VA5, MovUtl::move(VA6),
                    VA7, MovUtl::move(VA8)),&da);

       TEST_MAKEOP(({1,2,3}, MovUtl::move(VA1), VA2,
                    MovUtl::move(VA3), VA4,
                    MovUtl::move(VA5), VA6,
                    MovUtl::move(VA7), VA8,
                    MovUtl::move(VA9)),&da);
       TEST_MAKEOP(({1,2,3}, VA1, MovUtl::move(VA2),
                    VA3, MovUtl::move(VA4),
                    VA5, MovUtl::move(VA6),
                    VA7, MovUtl::move(VA8),
                    VA9),&da);

       TEST_MAKEOP(({1,2,3}, MovUtl::move(VA1), VA2,
                    MovUtl::move(VA3), VA4,
                    MovUtl::move(VA5), VA6,
                    MovUtl::move(VA7), VA8,
                    MovUtl::move(VA9), VA10),&da);
       TEST_MAKEOP(({1,2,3}, VA1, MovUtl::move(VA2),
                    VA3, MovUtl::move(VA4),
                    VA5, MovUtl::move(VA6),
                    VA7, MovUtl::move(VA8),
                    VA9, MovUtl::move(VA10)),&da);

       TEST_MAKEOP(({1,2,3}, MovUtl::move(VA1), VA2,
                    MovUtl::move(VA3), VA4,
                    MovUtl::move(VA5), VA6,
                    MovUtl::move(VA7), VA8,
                    MovUtl::move(VA9), VA10,
                    MovUtl::move(VA11)),&da);
       TEST_MAKEOP(({1,2,3}, VA1, MovUtl::move(VA2),
                    VA3, MovUtl::move(VA4),
                    VA5, MovUtl::move(VA6),
                    VA7, MovUtl::move(VA8),
                    VA9, MovUtl::move(VA10),
                    VA11),&da);

       TEST_MAKEOP(({1,2,3}, MovUtl::move(VA1), VA2,
                    MovUtl::move(VA3), VA4,
                    MovUtl::move(VA5), VA6,
                    MovUtl::move(VA7), VA8,
                    MovUtl::move(VA9), VA10,
                    MovUtl::move(VA11), VA12),&da);
       TEST_MAKEOP(({1,2,3}, VA1, MovUtl::move(VA2),
                    VA3, MovUtl::move(VA4),
                    VA5, MovUtl::move(VA6),
                    VA7, MovUtl::move(VA8),
                    VA9, MovUtl::move(VA10),
                    VA11, MovUtl::move(VA12)),&da);

       TEST_MAKEOP(({1,2,3}, MovUtl::move(VA1), VA2,
                    MovUtl::move(VA3), VA4,
                    MovUtl::move(VA5), VA6,
                    MovUtl::move(VA7), VA8,
                    MovUtl::move(VA9), VA10,
                    MovUtl::move(VA11), VA12,
                    MovUtl::move(VA13)),&da);
       TEST_MAKEOP(({1,2,3}, VA1, MovUtl::move(VA2),
                    VA3, MovUtl::move(VA4),
                    VA5, MovUtl::move(VA6),
                    VA7, MovUtl::move(VA8),
                    VA9, MovUtl::move(VA10),
                    VA11, MovUtl::move(VA12),
                    VA13),&da);
*/
    }
#endif // BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS
}
void bslstl_optional_test32()
{
    // --------------------------------------------------------------------
    // TESTING 'alloc_optional' FACILITY
    //
    //  In this test, we are ensuring that the 'optional' created using
    //  'alloc_optional' facility is created using the given arguments and
    //  without unnecessary copies. We are not worried about the allocator
    //  policy of the allocator aware type, as choosing the allocator policy
    //  when creating an 'optional' is tested in constructor tests.
    //
    //
    // Concerns:
    //: 1 Invoking 'alloc_optional' creates an 'optional' object whose
    //:   'value_type' object has been created using the arguments of
    //:   'alloc_optional'.
    //: 2 The allocator of the created 'optional' object is the allocator
    //:   passed in to the 'alloc_optional'.
    //: 3 Multiple arguments are correctly forwarded.
    //: 4 Arguments are perfectly forwarded.
    //: 5 No unecessary objects of'value_type' are created.
    //
    //
    // Plan:
    //: 1 Call 'make_optional' to create an 'optional' of an allocator aware
    //:   'value_type'. As an argument to 'make_optional' use an lvalue of
    //:   the 'value_type'. For concern 1, check the resulting 'optional' object
    //:   has the expected value.
    //: 2 In step 2, for concern 2, check the allocator of the newly created
    //:   'optional' object is the one passed into the 'alloc_optional'
    //: 3 Repeat steps 1 and 2 using an rvalue of the 'value_type' as an argument
    //:   to 'make_optional'. For concern 3, check the resulting 'optional'
    //:   object has been created using a move constructor.
    //: 4 Using various number of arguments to 'make_optional', and a
    //:   combination of lvalue an  rvalue for each argument poisition, for
    //:   concern 4, check that each argument is correctly forwarded to the
    //:   'value_type' constructor. For concern 3, check that an lvalue argument
    //:   is forwarded as an lvalue to the constructor, and rvalue argument is
    //:   forwarded as an rvalue.
    //: 5 Repeat step 4 using an 'initializer_list' as the first argument.
    //: 6 In steps 1-5, for concern 5, verify no additional copies of the value
    //:   type are created.
    //
    //
    // Testing:
    //      alloc_optional(const bsl::allocator<char>& a, T&&);
    //      alloc_optional(const bsl::allocator<char>>& a, Args&&... args);
    //      alloc_optional(const bsl::allocator<char><>& a,
    //                     initializer_list<U> il,
    //                     Args&&... args);
    //

    if (verbose) printf("\nTESTING 'alloc_optional' FACILITY"
                        "\n=================================\n");

    if (veryVerbose) printf("\tDeduced type alloc_optional.\n");
    {
        bslma::TestAllocator da("default", veryVeryVeryVerbose);
        bslma::TestAllocator oa("other", veryVeryVeryVerbose);
        bslma::DefaultAllocatorGuard dag(&da);
        MyClass2 source(2);

        int CCI = MyClass2::copyConstructorInvocations;
        int MCI = MyClass2::moveConstructorInvocations;

        optional<MyClass2> obj = bsl::alloc_optional(&oa, source);
        ASSERT( 2 == obj.value().value());
        ASSERT( &oa == obj.value().d_def.d_allocator_p);
        ASSERT( CCI == (MyClass2::copyConstructorInvocations -1));
        ASSERT( MCI == MyClass2::moveConstructorInvocations);

        CCI = MyClass2::copyConstructorInvocations;
        MCI = MyClass2::moveConstructorInvocations;

#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
        optional<MyClass2> obj2 = bsl::alloc_optional(&oa,
                                  MovUtl::move(source));
#else
        //type deduction does not work when using MovableRef
        optional<MyClass2> obj2 = bsl::alloc_optional<MyClass2>(&oa,
                                  MovUtl::move(source));
#endif //#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
        ASSERT( 2 == obj2.value().value());
        ASSERT( &oa == obj2.value().d_def.d_allocator_p);
        ASSERT( CCI == (MyClass2::copyConstructorInvocations));
        ASSERT( MCI == (MyClass2::moveConstructorInvocations  - 1));
    }
    if (veryVerbose) printf("\tvar arg alloc_optional.\n");
    {
        bslma::TestAllocator da("default", veryVeryVeryVerbose);
        bslma::TestAllocator oa("other", veryVeryVeryVerbose);
        bslma::DefaultAllocatorGuard dag(&da);

        TEST_ALLOCOP((&da, VA1),
                     (VA1, &da), &da);
        TEST_ALLOCOP((&da, MovUtl::move(VA1)),
                      (MovUtl::move(VA1), &da), &da);

        TEST_ALLOCOP((&da,MovUtl::move(VA1), VA2),
                     (MovUtl::move(VA1), VA2, &da),
                     &da);
        TEST_ALLOCOP((&da,VA1, MovUtl::move(VA2)),
                     (VA1, MovUtl::move(VA2), &da),
                     &da);

        TEST_ALLOCOP((&da,MovUtl::move(VA1), VA2, MovUtl::move(VA3)),
                     (MovUtl::move(VA1), VA2, MovUtl::move(VA3), &da),
                     &da);
        TEST_ALLOCOP((&da,VA1, MovUtl::move(VA2), VA3),
                     (VA1, MovUtl::move(VA2), VA3, &da),
                     &da);

        TEST_ALLOCOP((&da,MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4),
                     (MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                      &da),
                     &da);
        TEST_ALLOCOP((&da,VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4)),
                     (VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                      &da),
                     &da);

        TEST_ALLOCOP((&da,MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                      MovUtl::move(VA5)),
                     (MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                      MovUtl::move(VA5), &da),
                     &da);
        TEST_ALLOCOP((&da,VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                      VA5),
                     (VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                      VA5, &da),
                     &da);
/*
        TEST_ALLOCOP((&da,MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                      MovUtl::move(VA5), VA6),
                     (MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                      MovUtl::move(VA5), VA6, &da),
                     &da);
        TEST_ALLOCOP((&da,VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                      VA5, MovUtl::move(VA6)),
                     (VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                      VA5, MovUtl::move(VA6),&da),
                     &da);

        TEST_ALLOCOP((&da,MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                      MovUtl::move(VA5), VA6, MovUtl::move(VA7)),
                     (MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                      MovUtl::move(VA5), VA6, MovUtl::move(VA7), &da),
                     &da);
        TEST_ALLOCOP((&da,VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                      VA5, MovUtl::move(VA6), VA7),
                     (VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                      VA5, MovUtl::move(VA6), VA7, &da),
                     &da);

        TEST_ALLOCOP((&da,MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                      MovUtl::move(VA5), VA6, MovUtl::move(VA7)),
                     (MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                      MovUtl::move(VA5), VA6, MovUtl::move(VA7), &da),
                     &da);
        TEST_ALLOCOP((&da,VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                      VA5, MovUtl::move(VA6), VA7),
                     (VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                      VA5, MovUtl::move(VA6), VA7, &da),
                     &da);

        TEST_ALLOCOP((&da,MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                      MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8),
                     (MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                      MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                      &da),
                     &da);
        TEST_ALLOCOP((&da,VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                      VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8)),
                     (VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                      VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                      &da),
                     &da);

        TEST_ALLOCOP((&da,MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                      MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                      MovUtl::move(VA9)),
                     (MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                      MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                      MovUtl::move(VA9),&da),
                     &da);
        TEST_ALLOCOP((&da,VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                      VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                      VA9),
                     (VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                      VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                      VA9, &da),
                     &da);

        TEST_ALLOCOP((&da,MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                      MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                      MovUtl::move(VA9), VA10),
                     (MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                      MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                      MovUtl::move(VA9), VA10, &da),
                     &da);
        TEST_ALLOCOP((&da,VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                      VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                      VA9, MovUtl::move(VA10)),
                     (VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                      VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                      VA9, MovUtl::move(VA10), &da),
                     &da);

        TEST_ALLOCOP((&da,MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                      MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                      MovUtl::move(VA9), VA10, MovUtl::move(VA11)),
                     (MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                      MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                      MovUtl::move(VA9), VA10, MovUtl::move(VA11),&da),
                     &da);
        TEST_ALLOCOP((&da,VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                      VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                      VA9, MovUtl::move(VA10), VA11),
                     (VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                      VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                      VA9, MovUtl::move(VA10), VA11, &da),
                     &da);

        TEST_ALLOCOP((&da,MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                      MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                      MovUtl::move(VA9), VA10, MovUtl::move(VA11), VA12),
                     (MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                      MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                      MovUtl::move(VA9), VA10, MovUtl::move(VA11), VA12,
                      &da),
                     &da);
        TEST_ALLOCOP((&da,VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                      VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                      VA9, MovUtl::move(VA10), VA11, MovUtl::move(VA12)),
                     (VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                      VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                      VA9, MovUtl::move(VA10), VA11, MovUtl::move(VA12),
                      &da),
                     &da);

        TEST_ALLOCOP((&da,MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                      MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                      MovUtl::move(VA9), VA10, MovUtl::move(VA11), VA12,
                      MovUtl::move(VA13)),
                     (MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                      MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                      MovUtl::move(VA9), VA10, MovUtl::move(VA11), VA12,
                      MovUtl::move(VA13), &da),
                     &da);
        TEST_ALLOCOP((&da,VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                      VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                      VA9, MovUtl::move(VA10), VA11, MovUtl::move(VA12),
                      VA13),
                     (VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                      VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                      VA9, MovUtl::move(VA10), VA11, MovUtl::move(VA12),
                      VA13, &da),
                     &da);


        TEST_ALLOCOP((&da,MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                      MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                      MovUtl::move(VA9), VA10, MovUtl::move(VA11), VA12,
                      MovUtl::move(VA13), VA14),
                     (MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                      MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                      MovUtl::move(VA9), VA10, MovUtl::move(VA11), VA12,
                      MovUtl::move(VA13), VA14, &da),
                     &da);
        TEST_ALLOCOP((&da,VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                      VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                      VA9, MovUtl::move(VA10), VA11, MovUtl::move(VA12),
                      VA13, MovUtl::move(VA14)),
                     (VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                      VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                      VA9, MovUtl::move(VA10), VA11, MovUtl::move(VA12),
                      VA13, MovUtl::move(VA14)),
                     &da);
*/
    }

#ifdef BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS
    if (veryVerbose) printf("\tinitializer_list alloc_optional.\n");
    {
        bslma::TestAllocator da("default", veryVeryVeryVerbose);
        bslma::TestAllocator oa("other", veryVeryVeryVerbose);
        bslma::DefaultAllocatorGuard dag(&da);

        TEST_ALLOCOP((&da,{1,2,3}),
                     ({1,2,3}, &da), &da);

        TEST_ALLOCOP((&da,{1,2,3}, VA1),
                     ({1,2,3}, VA1, &da), &da);
        TEST_ALLOCOP((&da,{1,2,3}, MovUtl::move(VA1)),
                      ({1,2,3}, MovUtl::move(VA1), &da), &da);

        TEST_ALLOCOP((&da, {1,2,3}, MovUtl::move(VA1), VA2),
                     ({1,2,3}, MovUtl::move(VA1), VA2, &da),
                     &da);
        TEST_ALLOCOP((&da, {1,2,3}, VA1, MovUtl::move(VA2)),
                     ({1,2,3}, VA1, MovUtl::move(VA2), &da),
                     &da);

        TEST_ALLOCOP((&da, {1,2,3},
                      MovUtl::move(VA1), VA2, MovUtl::move(VA3)),
                     ({1,2,3},
                      MovUtl::move(VA1), VA2, MovUtl::move(VA3), &da),
                     &da);
        TEST_ALLOCOP((&da, {1,2,3},
                      VA1, MovUtl::move(VA2), VA3),
                     ({1,2,3},
                      VA1, MovUtl::move(VA2), VA3, &da),
                     &da);

        TEST_ALLOCOP((&da, {1,2,3},
                      MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4),
                     ({1,2,3},
                      MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                      &da),
                     &da);
        TEST_ALLOCOP((&da, {1,2,3},
                      VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4)),
                     ({1,2,3},
                      VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                      &da),
                     &da);

        TEST_ALLOCOP((&da, {1,2,3},
                      MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                      MovUtl::move(VA5)),
                     ({1,2,3},
                      MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                      MovUtl::move(VA5), &da),
                     &da);
        TEST_ALLOCOP((&da, {1,2,3},
                      VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                      VA5),
                     ({1,2,3},
                      VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                      VA5, &da),
                     &da);
/*
        TEST_ALLOCOP((&da, {1,2,3},
                      MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                      MovUtl::move(VA5), VA6),
                     ({1,2,3},
                      MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                      MovUtl::move(VA5), VA6, &da),
                     &da);
        TEST_ALLOCOP((&da, {1,2,3},
                      VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                      VA5, MovUtl::move(VA6)),
                     ({1,2,3},
                      VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                      VA5, MovUtl::move(VA6),&da),
                     &da);

        TEST_ALLOCOP((&da, {1,2,3},
                      MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                      MovUtl::move(VA5), VA6, MovUtl::move(VA7)),
                     ({1,2,3},
                      MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                      MovUtl::move(VA5), VA6, MovUtl::move(VA7), &da),
                     &da);
        TEST_ALLOCOP((&da, {1,2,3},
                      VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                      VA5, MovUtl::move(VA6), VA7),
                     ({1,2,3},
                      VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                      VA5, MovUtl::move(VA6), VA7, &da),
                     &da);

        TEST_ALLOCOP((&da, {1,2,3},
                      MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                      MovUtl::move(VA5), VA6, MovUtl::move(VA7)),
                     ({1,2,3},
                      MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                      MovUtl::move(VA5), VA6, MovUtl::move(VA7), &da),
                     &da);
        TEST_ALLOCOP((&da, {1,2,3},
                      VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                      VA5, MovUtl::move(VA6), VA7),
                     ({1,2,3},
                      VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                      VA5, MovUtl::move(VA6), VA7, &da),
                     &da);

        TEST_ALLOCOP((&da, {1,2,3},
                      MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                      MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8),
                     ({1,2,3},
                      MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                      MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                      &da),
                     &da);
        TEST_ALLOCOP((&da, {1,2,3},
                      VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                      VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8)),
                     ({1,2,3},
                      VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                      VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                      &da),
                     &da);

        TEST_ALLOCOP((&da, {1,2,3},
                      MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                      MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                      MovUtl::move(VA9)),
                     ({1,2,3},
                      MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                      MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                      MovUtl::move(VA9),&da),
                     &da);
        TEST_ALLOCOP((&da, {1,2,3},
                      VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                      VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                      VA9),
                     ({1,2,3},
                      VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                      VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                      VA9, &da),
                     &da);

        TEST_ALLOCOP((&da, {1,2,3},
                      MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                      MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                      MovUtl::move(VA9), VA10),
                     ({1,2,3},
                      MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                      MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                      MovUtl::move(VA9), VA10, &da),
                     &da);
        TEST_ALLOCOP((&da, {1,2,3},
                       VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                      VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                      VA9, MovUtl::move(VA10)),
                     ({1,2,3},
                      VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                      VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                      VA9, MovUtl::move(VA10), &da),
                     &da);

        TEST_ALLOCOP((&da, {1,2,3},
                      MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                      MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                      MovUtl::move(VA9), VA10, MovUtl::move(VA11)),
                     ({1,2,3},
                      MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                      MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                      MovUtl::move(VA9), VA10, MovUtl::move(VA11),&da),
                     &da);
        TEST_ALLOCOP((&da, {1,2,3},
                      VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                      VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                      VA9, MovUtl::move(VA10), VA11),
                     ({1,2,3},
                      VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                      VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                      VA9, MovUtl::move(VA10), VA11, &da),
                     &da);

        TEST_ALLOCOP((&da, {1,2,3},
                      MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                      MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                      MovUtl::move(VA9), VA10, MovUtl::move(VA11), VA12),
                     ({1,2,3},
                      MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                      MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                      MovUtl::move(VA9), VA10, MovUtl::move(VA11), VA12,
                      &da),
                     &da);
        TEST_ALLOCOP((&da, {1,2,3},
                      VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                      VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                      VA9, MovUtl::move(VA10), VA11, MovUtl::move(VA12)),
                     ({1,2,3},
                      VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                      VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                      VA9, MovUtl::move(VA10), VA11, MovUtl::move(VA12),
                      &da),
                     &da);

        TEST_ALLOCOP((&da, {1,2,3},
                      MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                      MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                      MovUtl::move(VA9), VA10, MovUtl::move(VA11), VA12,
                      MovUtl::move(VA13)),
                     ({1,2,3},
                      MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                      MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                      MovUtl::move(VA9), VA10, MovUtl::move(VA11), VA12,
                      MovUtl::move(VA13), &da),
                     &da);
        TEST_ALLOCOP((&da, {1,2,3},
                      VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                      VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                      VA9, MovUtl::move(VA10), VA11, MovUtl::move(VA12),
                      VA13),
                     ({1,2,3},
                      VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                      VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                      VA9, MovUtl::move(VA10), VA11, MovUtl::move(VA12),
                      VA13, &da),
                     &da);
*/
    }
#endif // BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS
}

#ifdef __cpp_lib_optional
void bslstl_optional_test33()
{
    // --------------------------------------------------------------------
    // TESTING relop with std::optional
    //
    //  In this test, we check that relationship operators work when used
    //  with bsl::optional and std::optional;
    //
    // Concerns:
    //: 1 We can compare a bsl::optional object of value type V and an
    //    std::optional object of type U if U and V are comparable types. The
    //    result depends on whether the optional objects is engaged or not.
    //
    // Plan:
    //: 1 Execute tests for int( doesn't use allocator) and MyClass2 (uses
    //    allocator).
    //  2 For each relation operator, execute a combination of comparing:
    //      - a bsl::optional and std::optional objects of same
    //        value types, with bsl::optional on left hand side
    //      - a bsl::optional and std::optional objects of same
    //        value types, with bsl::optional on right hand side
    //      - a bsl::optional and std::optional objects of different
    //        value types, with bsl::optional on left hand side
    //      - a bsl::optional and std::optional objects of different
    //        value types, with bsl::optional on right hand side
    //
    //   For each comparison, execute tests for both engaged and disengaged
    //   optional objects
    //
    // Testing:
    //   bool operator==(const optional<LHS>&, const optional<RHS>&)
    //   bool operator!=(const optional<LHS>&, const optional<RHS>&)
    //   bool operator<=(const optional<LHS>&, const optional<RHS>&)
    //   bool operator>=(const optional<LHS>&, const optional<RHS>&)
    //   bool operator< (const optional<LHS>&, const optional<RHS>&)
    //   bool operator> (const optional<LHS>&, const optional<RHS>&)


  if (verbose) printf("\nTESTING RELATIONAL OPERATORS"
                      "\n============================\n");
  {
      typedef std::optional<int>                    OptT;
      typedef bsl::optional<int>                    OptV;

      OptT X;
      OptV Y;

      //comparing two disengaged optionals
      ASSERT(  X == Y  ); // If bool(x) != bool(y), false;
      ASSERT(!(X != Y) ); // If bool(x) != bool(y), true;
                          // otherwise, if bool(x) == false, false;
      ASSERT(!(X <  Y) ); // If !y, false;
      ASSERT(!(X >  Y) ); // If !x, false;
      ASSERT(  X <= Y  ); // If !x, true;
      ASSERT(  X >= Y  ); // If !y, true;

      //rhs disengaged, lhs engaged
      Y.emplace(3);
      ASSERT(!(X == Y) ); // If bool(x) != bool(y), false;
      ASSERT( (X != Y) ); // If bool(x) != bool(y), true;
                          // otherwise, if bool(x) == false, false;
      ASSERT( (X <  Y) ); // If !y, false; otherwise, if !x, true;
      ASSERT(!(X >  Y) ); // If !x, false;
      ASSERT(  X <= Y  ); // If !x, true;
      ASSERT(!(X >= Y) ); // If !y, true; otherwise, if !x, false;

      //rhs engaged, lhs disengaged
      X.emplace(5);
      Y.reset();
      ASSERT(!(X == Y) ); // If bool(x) != bool(y), false;
      ASSERT( (X != Y) ); // If bool(x) != bool(y), true;
                          // otherwise, if bool(x) == false, false;
      ASSERT(!(X <  Y) ); // If !y, false; otherwise, if !x, true;
      ASSERT( (X >  Y) ); // If !x, false; otherwise, if !y, true;
      ASSERT(!(X <= Y) ); // If !x, true; otherwise, if !y, false;
      ASSERT( (X >= Y) ); // If !y, true; otherwise, if !x, false;

      //both engaged, compare the values
      X.emplace(1);
      Y.emplace(3);
      ASSERT(!(X == Y) ); // If bool(x) != bool(y), false;
      ASSERT( (X != Y) ); // If bool(x) != bool(y), true;
                          // otherwise, if bool(x) == false, false;
      ASSERT( (X <  Y) ); // If !y, false; otherwise, if !x, true;
      ASSERT(!(X >  Y) ); // If !x, false; otherwise, if !y, true;
      ASSERT( (X <= Y) ); // If !x, true; otherwise, if !y, false;
      ASSERT(!(X >= Y) ); // If !y, true; otherwise, if !x, false;
  }
  {
      typedef bsl::optional<int>                    OptT;
      typedef std::optional<int>                    OptV;

      OptT X;
      OptV Y;

      //comparing two disengaged optionals
      ASSERT(  X == Y  ); // If bool(x) != bool(y), false;
      ASSERT(!(X != Y) ); // If bool(x) != bool(y), true;
                          // otherwise, if bool(x) == false, false;
      ASSERT(!(X <  Y) ); // If !y, false;
      ASSERT(!(X >  Y) ); // If !x, false;
      ASSERT(  X <= Y  ); // If !x, true;
      ASSERT(  X >= Y  ); // If !y, true;

      //rhs disengaged, lhs engaged
      Y.emplace(3);
      ASSERT(!(X == Y) ); // If bool(x) != bool(y), false;
      ASSERT( (X != Y) ); // If bool(x) != bool(y), true;
                          // otherwise, if bool(x) == false, false;
      ASSERT( (X <  Y) ); // If !y, false; otherwise, if !x, true;
      ASSERT(!(X >  Y) ); // If !x, false;
      ASSERT(  X <= Y  ); // If !x, true;
      ASSERT(!(X >= Y) ); // If !y, true; otherwise, if !x, false;

      //rhs engaged, lhs disengaged
      X.emplace(5);
      Y.reset();
      ASSERT(!(X == Y) ); // If bool(x) != bool(y), false;
      ASSERT( (X != Y) ); // If bool(x) != bool(y), true;
                          // otherwise, if bool(x) == false, false;
      ASSERT(!(X <  Y) ); // If !y, false; otherwise, if !x, true;
      ASSERT( (X >  Y) ); // If !x, false; otherwise, if !y, true;
      ASSERT(!(X <= Y) ); // If !x, true; otherwise, if !y, false;
      ASSERT( (X >= Y) ); // If !y, true; otherwise, if !x, false;

      //both engaged, compare the values
      X.emplace(1);
      Y.emplace(3);
      ASSERT(!(X == Y) ); // If bool(x) != bool(y), false;
      ASSERT( (X != Y) ); // If bool(x) != bool(y), true;
                          // otherwise, if bool(x) == false, false;
      ASSERT( (X <  Y) ); // If !y, false; otherwise, if !x, true;
      ASSERT(!(X >  Y) ); // If !x, false; otherwise, if !y, true;
      ASSERT( (X <= Y) ); // If !x, true; otherwise, if !y, false;
      ASSERT(!(X >= Y) ); // If !y, true; otherwise, if !x, false;
  }
  {
      typedef std::optional<MyClass2>                    OptT;
      typedef bsl::optional<MyClass2>                    OptV;

      OptT X;
      OptV Y;

      //comparing two disengaged optionals
      ASSERT(  X == Y  ); // If bool(x) != bool(y), false;
      ASSERT(!(X != Y) ); // If bool(x) != bool(y), true;
                          // otherwise, if bool(x) == false, false;
      ASSERT(!(X <  Y) ); // If !y, false;
      ASSERT(!(X >  Y) ); // If !x, false;
      ASSERT(  X <= Y  ); // If !x, true;
      ASSERT(  X >= Y  ); // If !y, true;

      //rhs disengaged, lhs engaged
      Y.emplace(3);
      ASSERT(!(X == Y) ); // If bool(x) != bool(y), false;
      ASSERT( (X != Y) ); // If bool(x) != bool(y), true;
                          // otherwise, if bool(x) == false, false;
      ASSERT( (X <  Y) ); // If !y, false; otherwise, if !x, true;
      ASSERT(!(X >  Y) ); // If !x, false;
      ASSERT(  X <= Y  ); // If !x, true;
      ASSERT(!(X >= Y) ); // If !y, true; otherwise, if !x, false;

      //rhs engaged, lhs disengaged
      X.emplace(5);
      Y.reset();
      ASSERT(!(X == Y) ); // If bool(x) != bool(y), false;
      ASSERT( (X != Y) ); // If bool(x) != bool(y), true;
                          // otherwise, if bool(x) == false, false;
      ASSERT(!(X <  Y) ); // If !y, false; otherwise, if !x, true;
      ASSERT( (X >  Y) ); // If !x, false; otherwise, if !y, true;
      ASSERT(!(X <= Y) ); // If !x, true; otherwise, if !y, false;
      ASSERT( (X >= Y) ); // If !y, true; otherwise, if !x, false;

      //both engaged, compare the values
      X.emplace(1);
      Y.emplace(3);
      ASSERT(!(X == Y) ); // If bool(x) != bool(y), false;
      ASSERT( (X != Y) ); // If bool(x) != bool(y), true;
                          // otherwise, if bool(x) == false, false;
      ASSERT( (X <  Y) ); // If !y, false; otherwise, if !x, true;
      ASSERT(!(X >  Y) ); // If !x, false; otherwise, if !y, true;
      ASSERT( (X <= Y) ); // If !x, true; otherwise, if !y, false;
      ASSERT(!(X >= Y) ); // If !y, true; otherwise, if !x, false;
  }
  {
      typedef bsl::optional<MyClass2>                    OptT;
      typedef std::optional<MyClass2>                    OptV;

      OptT X;
      OptV Y;

      //comparing two disengaged optionals
      ASSERT(  X == Y  ); // If bool(x) != bool(y), false;
      ASSERT(!(X != Y) ); // If bool(x) != bool(y), true;
                          // otherwise, if bool(x) == false, false;
      ASSERT(!(X <  Y) ); // If !y, false;
      ASSERT(!(X >  Y) ); // If !x, false;
      ASSERT(  X <= Y  ); // If !x, true;
      ASSERT(  X >= Y  ); // If !y, true;

      //rhs disengaged, lhs engaged
      Y.emplace(3);
      ASSERT(!(X == Y) ); // If bool(x) != bool(y), false;
      ASSERT( (X != Y) ); // If bool(x) != bool(y), true;
                          // otherwise, if bool(x) == false, false;
      ASSERT( (X <  Y) ); // If !y, false; otherwise, if !x, true;
      ASSERT(!(X >  Y) ); // If !x, false;
      ASSERT(  X <= Y  ); // If !x, true;
      ASSERT(!(X >= Y) ); // If !y, true; otherwise, if !x, false;

      //rhs engaged, lhs disengaged
      X.emplace(5);
      Y.reset();
      ASSERT(!(X == Y) ); // If bool(x) != bool(y), false;
      ASSERT( (X != Y) ); // If bool(x) != bool(y), true;
                          // otherwise, if bool(x) == false, false;
      ASSERT(!(X <  Y) ); // If !y, false; otherwise, if !x, true;
      ASSERT( (X >  Y) ); // If !x, false; otherwise, if !y, true;
      ASSERT(!(X <= Y) ); // If !x, true; otherwise, if !y, false;
      ASSERT( (X >= Y) ); // If !y, true; otherwise, if !x, false;

      //both engaged, compare the values
      X.emplace(1);
      Y.emplace(3);
      ASSERT(!(X == Y) ); // If bool(x) != bool(y), false;
      ASSERT( (X != Y) ); // If bool(x) != bool(y), true;
                          // otherwise, if bool(x) == false, false;
      ASSERT( (X <  Y) ); // If !y, false; otherwise, if !x, true;
      ASSERT(!(X >  Y) ); // If !x, false; otherwise, if !y, true;
      ASSERT( (X <= Y) ); // If !x, true; otherwise, if !y, false;
      ASSERT(!(X >= Y) ); // If !y, true; otherwise, if !x, false;
  }
  {
      typedef std::optional<int>                          OptT;
      typedef bsl::optional<MyClass2>                    OptV;

      OptT X;
      OptV Y;

      //comparing two disengaged optionals
      ASSERT(  X == Y  ); // If bool(x) != bool(y), false;
      ASSERT(!(X != Y) ); // If bool(x) != bool(y), true;
                          // otherwise, if bool(x) == false, false;
      ASSERT(!(X <  Y) ); // If !y, false;
      ASSERT(!(X >  Y) ); // If !x, false;
      ASSERT(  X <= Y  ); // If !x, true;
      ASSERT(  X >= Y  ); // If !y, true;

      //rhs disengaged, lhs engaged
      Y.emplace(3);
      ASSERT(!(X == Y) ); // If bool(x) != bool(y), false;
      ASSERT( (X != Y) ); // If bool(x) != bool(y), true;
                          // otherwise, if bool(x) == false, false;
      ASSERT( (X <  Y) ); // If !y, false; otherwise, if !x, true;
      ASSERT(!(X >  Y) ); // If !x, false;
      ASSERT(  X <= Y  ); // If !x, true;
      ASSERT(!(X >= Y) ); // If !y, true; otherwise, if !x, false;

      //rhs engaged, lhs disengaged
      X.emplace(5);
      Y.reset();
      ASSERT(!(X == Y) ); // If bool(x) != bool(y), false;
      ASSERT( (X != Y) ); // If bool(x) != bool(y), true;
                          // otherwise, if bool(x) == false, false;
      ASSERT(!(X <  Y) ); // If !y, false; otherwise, if !x, true;
      ASSERT( (X >  Y) ); // If !x, false; otherwise, if !y, true;
      ASSERT(!(X <= Y) ); // If !x, true; otherwise, if !y, false;
      ASSERT( (X >= Y) ); // If !y, true; otherwise, if !x, false;

      //both engaged, compare the values
      X.emplace(1);
      Y.emplace(3);
      ASSERT(!(X == Y) ); // If bool(x) != bool(y), false;
      ASSERT( (X != Y) ); // If bool(x) != bool(y), true;
                          // otherwise, if bool(x) == false, false;
      ASSERT( (X <  Y) ); // If !y, false; otherwise, if !x, true;
      ASSERT(!(X >  Y) ); // If !x, false; otherwise, if !y, true;
      ASSERT( (X <= Y) ); // If !x, true; otherwise, if !y, false;
      ASSERT(!(X >= Y) ); // If !y, true; otherwise, if !x, false;
  }
  {
      typedef bsl::optional<int>                          OptT;
      typedef std::optional<MyClass2>                    OptV;

      OptT X;
      OptV Y;

      //comparing two disengaged optionals
      ASSERT(  X == Y  ); // If bool(x) != bool(y), false;
      ASSERT(!(X != Y) ); // If bool(x) != bool(y), true;
                          // otherwise, if bool(x) == false, false;
      ASSERT(!(X <  Y) ); // If !y, false;
      ASSERT(!(X >  Y) ); // If !x, false;
      ASSERT(  X <= Y  ); // If !x, true;
      ASSERT(  X >= Y  ); // If !y, true;

      //rhs disengaged, lhs engaged
      Y.emplace(3);
      ASSERT(!(X == Y) ); // If bool(x) != bool(y), false;
      ASSERT( (X != Y) ); // If bool(x) != bool(y), true;
                          // otherwise, if bool(x) == false, false;
      ASSERT( (X <  Y) ); // If !y, false; otherwise, if !x, true;
      ASSERT(!(X >  Y) ); // If !x, false;
      ASSERT(  X <= Y  ); // If !x, true;
      ASSERT(!(X >= Y) ); // If !y, true; otherwise, if !x, false;

      //rhs engaged, lhs disengaged
      X.emplace(5);
      Y.reset();
      ASSERT(!(X == Y) ); // If bool(x) != bool(y), false;
      ASSERT( (X != Y) ); // If bool(x) != bool(y), true;
                          // otherwise, if bool(x) == false, false;
      ASSERT(!(X <  Y) ); // If !y, false; otherwise, if !x, true;
      ASSERT( (X >  Y) ); // If !x, false; otherwise, if !y, true;
      ASSERT(!(X <= Y) ); // If !x, true; otherwise, if !y, false;
      ASSERT( (X >= Y) ); // If !y, true; otherwise, if !x, false;

      //both engaged, compare the values
      X.emplace(1);
      Y.emplace(3);
      ASSERT(!(X == Y) ); // If bool(x) != bool(y), false;
      ASSERT( (X != Y) ); // If bool(x) != bool(y), true;
                          // otherwise, if bool(x) == false, false;
      ASSERT( (X <  Y) ); // If !y, false; otherwise, if !x, true;
      ASSERT(!(X >  Y) ); // If !x, false; otherwise, if !y, true;
      ASSERT( (X <= Y) ); // If !x, true; otherwise, if !y, false;
      ASSERT(!(X >= Y) ); // If !y, true; otherwise, if !x, false;
  }
  {
    typedef std::optional<MyClass2>                    OptT;
    typedef bsl::optional<int>                          OptV;

    OptT X;
    OptV Y;

    //comparing two disengaged optionals
    ASSERT(  X == Y  ); // If bool(x) != bool(y), false;
    ASSERT(!(X != Y) ); // If bool(x) != bool(y), true;
                        // otherwise, if bool(x) == false, false;
    ASSERT(!(X <  Y) ); // If !y, false;
    ASSERT(!(X >  Y) ); // If !x, false;
    ASSERT(  X <= Y  ); // If !x, true;
    ASSERT(  X >= Y  ); // If !y, true;

    //rhs disengaged, lhs engaged
    Y.emplace(3);
    ASSERT(!(X == Y) ); // If bool(x) != bool(y), false;
    ASSERT( (X != Y) ); // If bool(x) != bool(y), true;
                        // otherwise, if bool(x) == false, false;
    ASSERT( (X <  Y) ); // If !y, false; otherwise, if !x, true;
    ASSERT(!(X >  Y) ); // If !x, false;
    ASSERT(  X <= Y  ); // If !x, true;
    ASSERT(!(X >= Y) ); // If !y, true; otherwise, if !x, false;

    //rhs engaged, lhs disengaged
    X.emplace(5);
    Y.reset();
    ASSERT(!(X == Y) ); // If bool(x) != bool(y), false;
    ASSERT( (X != Y) ); // If bool(x) != bool(y), true;
                        // otherwise, if bool(x) == false, false;
    ASSERT(!(X <  Y) ); // If !y, false; otherwise, if !x, true;
    ASSERT( (X >  Y) ); // If !x, false; otherwise, if !y, true;
    ASSERT(!(X <= Y) ); // If !x, true; otherwise, if !y, false;
    ASSERT( (X >= Y) ); // If !y, true; otherwise, if !x, false;

    //both engaged, compare the values
    X.emplace(1);
    Y.emplace(3);
    ASSERT(!(X == Y) ); // If bool(x) != bool(y), false;
    ASSERT( (X != Y) ); // If bool(x) != bool(y), true;
                        // otherwise, if bool(x) == false, false;
    ASSERT( (X <  Y) ); // If !y, false; otherwise, if !x, true;
    ASSERT(!(X >  Y) ); // If !x, false; otherwise, if !y, true;
    ASSERT( (X <= Y) ); // If !x, true; otherwise, if !y, false;
    ASSERT(!(X >= Y) ); // If !y, true; otherwise, if !x, false;
  }
  {
    typedef bsl::optional<MyClass2>                    OptT;
    typedef std::optional<int>                          OptV;

    OptT X;
    OptV Y;

    //comparing two disengaged optionals
    ASSERT(  X == Y  ); // If bool(x) != bool(y), false;
    ASSERT(!(X != Y) ); // If bool(x) != bool(y), true;
                        // otherwise, if bool(x) == false, false;
    ASSERT(!(X <  Y) ); // If !y, false;
    ASSERT(!(X >  Y) ); // If !x, false;
    ASSERT(  X <= Y  ); // If !x, true;
    ASSERT(  X >= Y  ); // If !y, true;

    //rhs disengaged, lhs engaged
    Y.emplace(3);
    ASSERT(!(X == Y) ); // If bool(x) != bool(y), false;
    ASSERT( (X != Y) ); // If bool(x) != bool(y), true;
                        // otherwise, if bool(x) == false, false;
    ASSERT( (X <  Y) ); // If !y, false; otherwise, if !x, true;
    ASSERT(!(X >  Y) ); // If !x, false;
    ASSERT(  X <= Y  ); // If !x, true;
    ASSERT(!(X >= Y) ); // If !y, true; otherwise, if !x, false;

    //rhs engaged, lhs disengaged
    X.emplace(5);
    Y.reset();
    ASSERT(!(X == Y) ); // If bool(x) != bool(y), false;
    ASSERT( (X != Y) ); // If bool(x) != bool(y), true;
                        // otherwise, if bool(x) == false, false;
    ASSERT(!(X <  Y) ); // If !y, false; otherwise, if !x, true;
    ASSERT( (X >  Y) ); // If !x, false; otherwise, if !y, true;
    ASSERT(!(X <= Y) ); // If !x, true; otherwise, if !y, false;
    ASSERT( (X >= Y) ); // If !y, true; otherwise, if !x, false;

    //both engaged, compare the values
    X.emplace(1);
    Y.emplace(3);
    ASSERT(!(X == Y) ); // If bool(x) != bool(y), false;
    ASSERT( (X != Y) ); // If bool(x) != bool(y), true;
                        // otherwise, if bool(x) == false, false;
    ASSERT( (X <  Y) ); // If !y, false; otherwise, if !x, true;
    ASSERT(!(X >  Y) ); // If !x, false; otherwise, if !y, true;
    ASSERT( (X <= Y) ); // If !x, true; otherwise, if !y, false;
    ASSERT(!(X >= Y) ); // If !y, true; otherwise, if !x, false;
  }
}
void bslstl_optional_test34()
{
    // --------------------------------------------------------------------
    // TESTING swap FACILITY between bsl::optional and std::optional for a
    // non AA type
    // Concerns:
    //   1. Swap of two disengaged objects is a no-op,
    //   2. Swap of an engaged and a disengaged optional moves the value
    //      from the engaged object to another without calling swap for the
    //      value type.
    //   3. Swap of two engaged objects calls swap for the value type.
    //
    // Plan:
    //   Conduct the test using 'Swappable' (doesn't use allocator) for 'TYPE'.
    //
    //   Swap two disengaged optional objects and verify swap has not been
    //   called.
    //
    //   Swap two engaged optional objects and verify swap has been called.
    //
    //   Swap an engaged and disengaged optional object. Check swap has not
    //   been called. Check the correct values of swapped optional objects.
    //
    //   Execute the tests for both swap member function and free
    //   function. Execute the tests for bsl::optional on lhs and on rhs
    //
    // Testing:
    //   void swap(std::optional<TYPE>& other);
    //   void swap(std::optional<TYPE>& lhs,bsl::optional<TYPE>& rhs);
    //   void swap(bsl::optional<TYPE>& lhs,std::optional<TYPE>& rhs);
    // --------------------------------------------------------------------

    if (verbose) printf("\nTESTING SWAP METHOD"
                        "\n===================\n");

    using bsl::swap;
    {

        bsl::optional<Swappable> a;
        std::optional<Swappable> b;

        Swappable::swapReset();
        swap(a, b);

        ASSERT(!Swappable::swapCalled());
        ASSERT(!a.has_value());
        ASSERT(!b.has_value());

        Swappable::swapReset();
        a.swap(b);

        ASSERT(!Swappable::swapCalled());
        ASSERT(!a.has_value());
        ASSERT(!b.has_value());

        Swappable::swapReset();
        swap(b, a);

        ASSERT(!Swappable::swapCalled());
        ASSERT(!a.has_value());
        ASSERT(!b.has_value());

        Swappable::swapReset();
        b.swap(a);

        ASSERT(!Swappable::swapCalled());
        ASSERT(!a.has_value());
        ASSERT(!b.has_value());
    }
    {
        Swappable obj1(1);
        Swappable obj2(2);

        const Swappable Zobj1(obj1);
        const Swappable Zobj2(obj2);

        bsl::optional<Swappable> a = obj1;
        std::optional<Swappable> b = obj2;
        ASSERT(a.value() == Zobj1);
        ASSERT(b.value() == Zobj2);

        Swappable::swapReset();
        ASSERT(!Swappable::swapCalled());
        swap(a, b);
        ASSERT( Swappable::swapCalled());

        ASSERT(b.value() == Zobj1);
        ASSERT(a.value() == Zobj2);

        Swappable::swapReset();
        ASSERT(!Swappable::swapCalled());
        a.swap(b);
        ASSERT( Swappable::swapCalled());

        ASSERT(a.value() == Zobj1);
        ASSERT(b.value() == Zobj2);

        Swappable::swapReset();
        ASSERT(!Swappable::swapCalled());
        swap(b, a);
        ASSERT( Swappable::swapCalled());

        ASSERT(b.value() == Zobj1);
        ASSERT(a.value() == Zobj2);

        Swappable::swapReset();
        ASSERT(!Swappable::swapCalled());
        b.swap(a);
        ASSERT( Swappable::swapCalled());

        ASSERT(a.value() == Zobj1);
        ASSERT(b.value() == Zobj2);
    }
    {
        bsl::optional<Swappable> nonNullObj(Swappable(10));
        bsl::optional<Swappable> nonNullObjCopy(nonNullObj);
        std::optional<Swappable> nullObj;

        Swappable::swapReset();
        swap(nonNullObj, nullObj);

        ASSERT(!SwappableAA::swapCalled());
        ASSERT(nonNullObjCopy == nullObj);
        ASSERT(!nonNullObj.has_value());

        Swappable::swapReset();
        nonNullObj.swap(nullObj);

        ASSERT(!Swappable::swapCalled());
        ASSERT(nonNullObjCopy == nonNullObj);
        ASSERT(!nullObj.has_value());

        Swappable::swapReset();
        swap(nullObj, nonNullObj);

        ASSERT(!SwappableAA::swapCalled());
        ASSERT(nonNullObjCopy == nullObj);
        ASSERT(!nonNullObj.has_value());

        Swappable::swapReset();
        nullObj.swap(nonNullObj);

        ASSERT(!Swappable::swapCalled());
        ASSERT(nonNullObjCopy == nonNullObj);
        ASSERT(!nullObj.has_value());
    }
}
#endif //__cpp_lib_optional
template <typename TYPE>
void bslstl_optional_value_type_deduce(const bsl::optional<TYPE>& )
{

}

template <typename TYPE>
void bslstl_optional_optional_type_deduce(const TYPE& )
{

}
void bslstl_optional_test35()
{
    // --------------------------------------------------------------------
    // TESTING type deduction
    //
    //  In this test, we check that type deduction with bsl::optional
    //  works.
    //
    // Concerns:
    //: 1 Invoking a template function which takes bsl::optional<TYPE> deduces
    //    the TYPE
    //: 2 Invoking a template function which takes <TYPE> with bsl::optional
    //    will be able to deduce bsl::optional<VALUE_TYPE>
    //
    // Plan:
    //: 1 Execute tests for int and bsl::string as TestType.
    //: 2 Call a template function which takes bsl::optinal<TYPE> with an
    //    object of type bsl::optional<TestType>
    //: 3 Call a template function which takes TYPE with an object of type
    //    bsl::optional<TestType>
    //
    //
    // Testing:
    //    Type deduction
    //

    bslstl_optional_value_type_deduce(bsl::optional<int>());
    bslstl_optional_value_type_deduce(bsl::optional<bsl::string>());
    bslstl_optional_optional_type_deduce(bsl::optional<int>());
    bslstl_optional_optional_type_deduce(bsl::optional<bsl::string>());

}
void bslstl_optional_test36()
{
    // --------------------------------------------------------------------
    // TESTING TRAITS AND TYPEDEFS
    //
    //
    // Concerns:
    //: 1 That 'optional<TYPE>::value_type' is 'TYPE'
    //: 2 That 'optional<const TYPE>::value_type' is 'const TYPE'
    //: 3 That 'BloombergLP::bslma::UsesBslmaAllocator<optional<TYPE>>' and
    //:   'BloombergLP::bslmf::UsesAllocatorArgT<optional<TYPE>>' are 'false'
    //:   if 'TYPE' is a non allocator aware type.
    //: 4 That 'BloombergLP::bslma::UsesBslmaAllocator<optional<TYPE>>' and
    //:   'BloombergLP::bslmf::UsesAllocatorArgT<optional<TYPE>>' are 'true'
    //:   if 'TYPE' is an allocator aware type.
    //
    // Plan:
    //: 1 Using an 'optional' of non allocator aware type, verify that
    //:   'value_type' matches the chosen 'value_type'.
    //: 2 For concern 2, repeat step 1 using a 'const' 'value_type'.
    //: 3 In step 1, for concern 2, verify that both 'UsesBslmaAllocator' and
    //:   'UsesAllocatorArgT' traits are false for the 'optional' type.
    //: 4 Repeat step 1 for a 'value_type' that is 'UsesBslmaAllocator', but
    //:   isn't 'UsesAllocatorArgT'. For concern 3, verify that both
    //:   'UsesBslmaAllocator' and 'UsesAllocatorArgT' traits are true for an
    //:   'optional' of that type.
    //: 4 Repeat steps 1 for a 'value_type' that is both 'UsesBslmaAllocator'
    //:   and 'UsesAllocatorArgT'. For concern 3, verify that both
    //:   'UsesBslmaAllocator' and 'UsesAllocatorArgT' traits are true for an
    //:   'optional' of that type.
    //
    // Testing:
    //      optional<TYPE>::value_type;
    //      BloombergLP::bslma::UsesBslmaAllocator<optional<TYPE>>
    //      BloombergLP::bslma::UsesBslmaAllocator<optional<TYPE>>
    if (verbose) printf("\nTTRAITS AND TYPEDEFS"
                        "\n====================\n");

    {
      typedef MyClass1                      ValueType;
      typedef const MyClass1                ConstValueType;
      typedef bsl::optional<ValueType>      Obj;
      typedef bsl::optional<ConstValueType> ObjC;
      ASSERT((bsl::is_same<Obj::value_type, ValueType>::value));
      ASSERT((bsl::is_same<ObjC::value_type, ConstValueType>::value));
      ASSERT(!(BloombergLP::bslma::UsesBslmaAllocator<Obj>::value));
      ASSERT(!(BloombergLP::bslmf::UsesAllocatorArgT<Obj>::value));
    }
    {
      typedef MyClass2                      ValueType;
      typedef bsl::optional<ValueType>      Obj;
      ASSERT((bsl::is_same<Obj::value_type, ValueType>::value));
      ASSERT((BloombergLP::bslma::UsesBslmaAllocator<Obj>::value));
      ASSERT((BloombergLP::bslmf::UsesAllocatorArgT<Obj>::value));
    }
    {
      typedef MyClass2a                      ValueType;
      typedef bsl::optional<ValueType>      Obj;
      ASSERT((bsl::is_same<Obj::value_type, ValueType>::value));
      ASSERT((BloombergLP::bslma::UsesBslmaAllocator<Obj>::value));
      ASSERT((BloombergLP::bslmf::UsesAllocatorArgT<Obj>::value));
    }

}

int main(int argc, char **argv)
{
    const int                 test = argc > 1 ? atoi(argv[1]) : 0;
    verbose = argc > 2;
    veryVerbose = argc > 3;
    veryVeryVerbose = argc > 4;
    veryVeryVeryVerbose = argc > 5;


    printf( "TEST  %s CASE %d \n", __FILE__, test );

    // CONCERN: 'BSLS_REVIEW' failures should lead to test failures.
    bsls::ReviewFailureHandlerGuard reviewGuard(&bsls::Review::failByAbort);

    bslma::TestAllocator defaultAllocator("default", veryVeryVeryVerbose);
    ASSERT(0 == bslma::Default::setDefaultAllocator(&defaultAllocator));

    // CONCERN: In no case does memory come from the global allocator.

    bslma::TestAllocator globalAllocator("global", veryVeryVeryVerbose);
    bslma::Default::setGlobalAllocator(&globalAllocator);

    switch (test) { case 0:
      case 35:
        bslstl_optional_test36();
        break;
      case 36:
        bslstl_optional_test36();
        break;
#ifdef __cpp_lib_optional
      case 34:
        bslstl_optional_test34();
        break;
      case 33:
        bslstl_optional_test33();
        break;
#endif // __cpp_lib_optional
      case 32:
        bslstl_optional_test32();
        break;
      case 31:
        bslstl_optional_test31();
        break;
      case 30:
        bslstl_optional_test30();
        break;
      case 29:
        bslstl_optional_test29();
        break;
      case 28:
        bslstl_optional_test28();
        break;
      case 27:
        bslstl_optional_test27();
        break;
      case 26:
        bslstl_optional_test26();
        break;
      case 25:
        bslstl_optional_test25();
        break;
      case 24:
        bslstl_optional_test24();
        break;
      case 23:
        bslstl_optional_test23();
        break;
      case 22:
        bslstl_optional_test22();
        break;
      case 21:
        bslstl_optional_test21();
        break;
      case 20:
        bslstl_optional_test20();
        break;
      case 19:
        bslstl_optional_test19();
        break;
      case 18:
        bslstl_optional_test18();
        break;
      case 17:
        bslstl_optional_test17();
        break;
      case 16:
        bslstl_optional_test16();
        break;
      case 15:
        bslstl_optional_test15();
        break;
      case 14:
        bslstl_optional_test14();
        break;
      case 13:
        bslstl_optional_test13();
        break;
      case 12:
        bslstl_optional_test12();
        break;
      case 11:
        bslstl_optional_test11();
        break;
      case 10:
        bslstl_optional_test10();
        break;
      case 9:
        bslstl_optional_test9();
        break;
     case 8:
        bslstl_optional_test8();
        break;
      case 7:
        bslstl_optional_test7();
        break;
      case 6:
        bslstl_optional_test6();
        break;
      case 5:
        bslstl_optional_test5();
        break;
      case 4:
        bslstl_optional_test4();
        break;
      case 3:
         bslstl_optional_test3();
         break;
      case 2:
        bslstl_optional_test2();
        break;
      case 1:
        bslstl_optional_test1();
        break;
      default: {
        printf( "WARNING: CASE `%d' NOT FOUND.\n", test);
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
      printf( "Error, non-zero test status = %d .\n", testStatus);
    }
    return testStatus;
}

// ----------------------------------------------------------------------------
// Copyright 2015 Bloomberg Finance L.P.
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
