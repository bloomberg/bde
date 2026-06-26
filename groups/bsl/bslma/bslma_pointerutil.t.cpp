// bslma_pointerutil.t.cpp                                            -*-C++-*-

#include <bslma_pointerutil.h>

#include <bsls_alignmentutil.h>
#include <bsls_assert.h>
#include <bsls_bsltestutil.h>
#include <bsls_compilerfeatures.h>

#include <new>          // placement `new`

#include <stdio.h>      // `printf`
#include <stdlib.h>     // `atoi`

// BDE_VERIFY pragma: -TP17     // `puts` instead of `printf`

using namespace BloombergLP;

// ============================================================================
//                             TEST PLAN
// ----------------------------------------------------------------------------
//                                   Overview
//                                   --------
// The component under test provides a utility `struct` with two static member
// function templates, `voidify` and `unqualify`, that are simple inline cast
// wrappers.  Each function is tested independently across a comprehensive set
// of type categories, including cv-qualified object types, `void`, function
// types, and implicit-decay scenarios.
//
// We define a set of test macros ('TEST_VOIDIFY_OBJECT_TYPE',
// 'TEST_UNQUALIFY_TYPE', etc.) that expand a common pattern of assertions for
// every type under test.  Macros are used rather than template helpers because
// the functions under test are themselves templates whose correctness depends
// on template-argument deduction and overload resolution at the call site.  A
// test helper template would introduce its own deduction context, potentially
// masking defects in the component's overload set.  By using macros, each
// expanded call site is a direct, non-dependent invocation --- the same code a
// user would write.  We do not test with cv-qualified pointer *objects* (e.g.,
// 'TYPE * const') because all overloads take their argument by value, so
// top-level cv-qualification on the pointer is stripped before overload
// resolution and has no observable effect.  Array types must be passed to
// these macros via a typedef because the macro expands 'typedef TYPE Tp;',
// which is ill-formed when 'TYPE' contains brackets (e.g., 'int[3]'); C/C++
// requires the extent after the declarator name ('typedef int Tp[3];'), a
// syntax that macro substitution cannot produce.
//
// ----------------------------------------------------------------------------
// [ 2] TYPE *unqualify(               TYPE *address);
// [ 2] TYPE *unqualify(const          TYPE *address);
// [ 2] TYPE *unqualify(      volatile TYPE *address);
// [ 2] TYPE *unqualify(const volatile TYPE *address);
// [ 3] void *voidify(TYPE *address);
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 6] USAGE EXAMPLE 1
// [ 7] USAGE EXAMPLE 2
// [ 4] CONCERN: IMPLICIT DECAY
// [ 5] CONCERN: CONSTEXPR

// ============================================================================
//                     STANDARD BSL ASSERT TEST FUNCTION
// ----------------------------------------------------------------------------

namespace {

int testStatus = 0;

void aSsErT(bool condition, const char *message, int line)
{
    if (condition) {
        printf("Error " __FILE__ "(%d): %s    (failed)\n", line, message);
        fflush(stdout);

        if (0 <= testStatus && testStatus <= 100) {
            ++testStatus;
        }
    }
}

// ============================================================================
//               STANDARD BSL TEST DRIVER MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT       BSLS_BSLTESTUTIL_ASSERT
#define ASSERTV      BSLS_BSLTESTUTIL_ASSERTV

#define Q            BSLS_BSLTESTUTIL_Q   // Quote identifier literally.
#define P            BSLS_BSLTESTUTIL_P   // Print identifier and value.
#define P_           BSLS_BSLTESTUTIL_P_  // P(X) without '\n'.
#define T_           BSLS_BSLTESTUTIL_T_  // Print a tab (w/o newline).
#define L_           BSLS_BSLTESTUTIL_L_  // current Line number

}  // close unnamed namespace

// ============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
// ============================================================================

typedef bslma::PointerUtil Util;

// INCOMPLETE TYPE (forward-declared only; never defined in this TU)
class IncompleteClass;

// COMPLETE CLASS TYPES
struct Base {
    int d_x;
};

struct Derived : Base {
    int d_y;
};

// UNION TYPE
union MyUnion {
    int    d_int;
    double d_double;
};

// ENUM TYPE
enum MyEnum { e_VALUE_A = 0, e_VALUE_B = 1 };

// FUNCTION DEFINITIONS mirroring the function type catalog.  Each function
// provides a distinct non-null address for the corresponding function type.

// Core set
void func0()                                   {}
int  func1(double)                             { return 0; }
int  func2(int, int)                           { return 0; }
void func3(const char*, int, double)           {}

// Core + ellipsis
void func0E(...)                               {}
int  func1E(double, ...)                       { return 0; }
int  func2E(int, int, ...)                     { return 0; }
void func3E(const char*, int, double, ...)     {}

// extern "C" linkage: mirrors core and ellipsis sets
extern "C" {
    void cfunc0()                              {}
    int  cfunc1(double)                        { return 0; }
    int  cfunc2(int, int)                      { return 0; }
    void cfunc3(const char*, int, double)      {}

    void cfunc0E(...)                           {}
    int  cfunc1E(double, ...)                   { return 0; }
    int  cfunc2E(int, int, ...)                 { return 0; }
    void cfunc3E(const char*, int, double, ...) {}
}

// noexcept: mirrors all of the above
#ifdef BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT_IN_FNC_TYPE
void func0_NE()                                noexcept {}
int  func1_NE(double)                          noexcept { return 0; }
int  func2_NE(int, int)                        noexcept { return 0; }
void func3_NE(const char*, int, double)        noexcept {}

void func0E_NE(...)                            noexcept {}
int  func1E_NE(double, ...)                    noexcept { return 0; }
int  func2E_NE(int, int, ...)                  noexcept { return 0; }
void func3E_NE(const char*, int, double, ...)  noexcept {}

extern "C" {
    void cfunc0_NE()                           noexcept {}
    int  cfunc1_NE(double)                     noexcept { return 0; }
    int  cfunc2_NE(int, int)                   noexcept { return 0; }
    void cfunc3_NE(const char*, int, double)   noexcept {}

    void cfunc0E_NE(...)                           noexcept {}
    int  cfunc1E_NE(double, ...)                   noexcept { return 0; }
    int  cfunc2E_NE(int, int, ...)                 noexcept { return 0; }
    void cfunc3E_NE(const char*, int, double, ...) noexcept {}
}
#endif

// Exotic function types: unusual return types and parameter counts
typedef void (*FP0)();
FP0   funcRetFP(int)                           { return &func0; }
void *funcRetVP(int)                           { return 0; }

// Function with 256 parameters, the recommended minimum in Annex B of the
// C++ Standard [implimits].
int funcMany(
    int, int, int, int, int, int, int, int, int, int, int, int, int,
    int, int, int, int, int, int, int, int, int, int, int, int, int,
    int, int, int, int, int, int, int, int, int, int, int, int, int,
    int, int, int, int, int, int, int, int, int, int, int, int, int,
    int, int, int, int, int, int, int, int, int, int, int, int, int,
    int, int, int, int, int, int, int, int, int, int, int, int, int,
    int, int, int, int, int, int, int, int, int, int, int, int, int,
    int, int, int, int, int, int, int, int, int, int, int, int, int,
    int, int, int, int, int, int, int, int, int, int, int, int, int,
    int, int, int, int, int, int, int, int, int, int, int, int, int,
    int, int, int, int, int, int, int, int, int, int, int, int, int,
    int, int, int, int, int, int, int, int, int, int, int, int, int,
    int, int, int, int, int, int, int, int, int, int, int, int, int,
    int, int, int, int, int, int, int, int, int, int, int, int, int,
    int, int, int, int, int, int, int, int, int, int, int, int, int,
    int, int, int, int, int, int, int, int, int, int, int, int, int,
    int, int, int, int, int, int, int, int, int, int, int, int, int,
    int, int, int, int, int, int, int, int, int, int, int, int, int,
    int, int, int, int, int, int, int, int, int, int, int, int, int,
    int, int, int) { return 0; }

// ============================================================================
//             HELPER MACROS FOR COMPREHENSIVE TYPE TESTING
// ============================================================================

// TEST_VOIDIFY_OBJECT_TYPE(TYPE, OBJ):
//   Test `voidify` with all 4 cv-qualifications of the given object `TYPE`,
//   for both a valid address and null.  `TYPE` must be a cv-unqualified
//   object type expressible as a simple type-id (no brackets); use a typedef
//   for array types before calling this macro.  `OBJ` must be an lvalue of
//   type `TYPE`.
#define TEST_VOIDIFY_OBJECT_TYPE(TYPE, OBJ)                                   \
    do {                                                                      \
        typedef TYPE             Tp;                                          \
        typedef const Tp         CTp;                                         \
        typedef volatile Tp      VTp;                                         \
        typedef const volatile Tp CVTp;                                       \
        void *expected = const_cast<void *>(                                  \
                             static_cast<const volatile void *>(&(OBJ)));     \
        /* --- plain --- */                                                   \
        {                                                                     \
            Tp *p = static_cast<Tp *>(expected);                              \
            void *result = Util::voidify(p);                                  \
            ASSERTV(L_, expected, result, expected == result);                \
            Tp *np = 0;                                                       \
            result = Util::voidify(np);                                       \
            ASSERTV(L_, result, (void *)0 == result);                         \
        }                                                                     \
        /* --- const --- */                                                   \
        {                                                                     \
            CTp *p = static_cast<CTp *>(expected);                            \
            void *result = Util::voidify(p);                                  \
            ASSERTV(L_, expected, result, expected == result);                \
            CTp *np = 0;                                                      \
            result = Util::voidify(np);                                       \
            ASSERTV(L_, result, (void *)0 == result);                         \
        }                                                                     \
        /* --- volatile --- */                                                \
        {                                                                     \
            VTp *p = static_cast<VTp *>(expected);                            \
            void *result = Util::voidify(p);                                  \
            ASSERTV(L_, expected, result, expected == result);                \
            VTp *np = 0;                                                      \
            result = Util::voidify(np);                                       \
            ASSERTV(L_, result, (void *)0 == result);                         \
        }                                                                     \
        /* --- const volatile --- */                                          \
        {                                                                     \
            CVTp *p = static_cast<CVTp *>(expected);                          \
            void *result = Util::voidify(p);                                  \
            ASSERTV(L_, expected, result, expected == result);                \
            CVTp *np = 0;                                                     \
            result = Util::voidify(np);                                       \
            ASSERTV(L_, result, (void *)0 == result);                         \
        }                                                                     \
    } while (false)

// TEST_VOIDIFY_VOID_CV(QUAL, ADDR):
//   Test `voidify` for a single cv-qualification of `void`.  `QUAL` must be
//   a cv-qualifier token (`const`, `volatile`, or `const volatile`) or
//   empty.  `ADDR` must be a non-null address.
#define TEST_VOIDIFY_VOID_CV(QUAL, ADDR)                                      \
    do {                                                                      \
        QUAL void *p = const_cast<QUAL void *>(                               \
                            static_cast<const volatile void *>(ADDR));        \
        void *exp = (void *)(ADDR);                                           \
        void *result = Util::voidify(p);                                      \
        ASSERTV(L_, exp, result, exp == result);                              \
        QUAL void *np = 0;                                                    \
        result = Util::voidify(np);                                           \
        ASSERTV(L_, result, (void *)0 == result);                             \
    } while (false)

// TEST_VOIDIFY_FUNC_TYPE(FTYPE, FPTR):
//   Test `voidify` for the given function type `FTYPE`, noting that function
//   types cannot be cv-qualified.  `FPTR` must be an expression of type
//   `FTYPE *` (i.e., a pointer to such a function).
#define TEST_VOIDIFY_FUNC_TYPE(FTYPE, FPTR)                                   \
    do {                                                                      \
        FTYPE *p = (FPTR);                                                    \
        void *exp    = (void *)p;                                             \
        void *result = Util::voidify(p);                                      \
        ASSERTV(L_, exp, result, exp == result);                              \
        FTYPE *np = 0;                                                        \
        result = Util::voidify(np);                                           \
        ASSERTV(L_, result, (void *)0 == result);                             \
    } while (false)

// TEST_UNQUALIFY_TYPE(TYPE, OBJ):
//   Test `unqualify` with all 4 cv-qualifications of the given object
//   `TYPE`, for both a valid address and null.  `TYPE` must be a
//   cv-unqualified object type expressible as a simple type-id (no
//   brackets); use a typedef for array types before calling this macro.
//   `OBJ` must be an lvalue of type `TYPE`.
#define TEST_UNQUALIFY_TYPE(TYPE, OBJ)                                        \
    do {                                                                      \
        typedef TYPE             Tp;                                          \
        typedef const Tp         CTp;                                         \
        typedef volatile Tp      VTp;                                         \
        typedef const volatile Tp CVTp;                                       \
        void *raw = const_cast<void *>(                                       \
                        static_cast<const volatile void *>(&(OBJ)));          \
        Tp *expected = static_cast<Tp *>(raw);                                \
        /* --- from plain (identity) --- */                                   \
        {                                                                     \
            Tp *result = Util::unqualify(expected);                           \
            ASSERTV(L_, expected, result, expected == result);                \
            Tp *np = 0;                                                       \
            result = Util::unqualify(np);                                     \
            ASSERTV(L_, result, (Tp *)0 == result);                           \
        }                                                                     \
        /* --- from const --- */                                              \
        {                                                                     \
            CTp *p = static_cast<CTp *>(raw);                                 \
            Tp *result = Util::unqualify(p);                                  \
            ASSERTV(L_, expected, result, expected == result);                \
            CTp *np = 0;                                                      \
            result = Util::unqualify(np);                                     \
            ASSERTV(L_, result, (Tp *)0 == result);                           \
        }                                                                     \
        /* --- from volatile --- */                                           \
        {                                                                     \
            VTp *p = static_cast<VTp *>(raw);                                 \
            Tp *result = Util::unqualify(p);                                  \
            ASSERTV(L_, expected, result, expected == result);                \
            VTp *np = 0;                                                      \
            result = Util::unqualify(np);                                     \
            ASSERTV(L_, result, (Tp *)0 == result);                           \
        }                                                                     \
        /* --- from const volatile --- */                                     \
        {                                                                     \
            CVTp *p = static_cast<CVTp *>(raw);                               \
            Tp *result = Util::unqualify(p);                                  \
            ASSERTV(L_, expected, result, expected == result);                \
            CVTp *np = 0;                                                     \
            result = Util::unqualify(np);                                     \
            ASSERTV(L_, result, (Tp *)0 == result);                           \
        }                                                                     \
    } while (false)

// TEST_UNQUALIFY_FUNC_TYPE(FTYPE, FPTR):
//   Test `unqualify` for the given function type `FTYPE`, noting that
//   function types cannot be cv-qualified.  Verifies that `unqualify` on a
//   function pointer returns the same pointer value, and also tests null.
//   `FPTR` must be an expression of type `FTYPE *`.
#define TEST_UNQUALIFY_FUNC_TYPE(FTYPE, FPTR)                                 \
    do {                                                                      \
        FTYPE *p = (FPTR);                                                    \
        FTYPE *result = Util::unqualify(p);                                   \
        ASSERTV(L_, p == result);                                             \
        FTYPE *np = 0;                                                        \
        result = Util::unqualify(np);                                         \
        ASSERTV(L_, (FTYPE *)0 == result);                                    \
    } while (false)

// ============================================================================
//                        FUNCTION TYPE CATALOG
// ============================================================================

// Core set: varying argument counts
typedef void Func0();
typedef int  Func1(double);
typedef int  Func2(int, int);
typedef void Func3(const char*, int, double);

// Core + C ellipsis
typedef void Func0E(...);
typedef int  Func1E(double, ...);
typedef int  Func2E(int, int, ...);
typedef void Func3E(const char*, int, double, ...);

// extern "C" linkage: mirrors core and ellipsis sets
extern "C" {
    typedef void CFunc0();
    typedef int  CFunc1(double);
    typedef int  CFunc2(int, int);
    typedef void CFunc3(const char*, int, double);

    typedef void CFunc0E(...);
    typedef int  CFunc1E(double, ...);
    typedef int  CFunc2E(int, int, ...);
    typedef void CFunc3E(const char*, int, double, ...);
}

// noexcept (C++17): mirrors all of the above
#ifdef BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT_IN_FNC_TYPE
typedef void Func0_NE()                              noexcept;
typedef int  Func1_NE(double)                        noexcept;
typedef int  Func2_NE(int, int)                      noexcept;
typedef void Func3_NE(const char*, int, double)      noexcept;

typedef void Func0E_NE(...)                           noexcept;
typedef int  Func1E_NE(double, ...)                   noexcept;
typedef int  Func2E_NE(int, int, ...)                 noexcept;
typedef void Func3E_NE(const char*, int, double, ...) noexcept;

extern "C" {
    typedef void CFunc0_NE()                         noexcept;
    typedef int  CFunc1_NE(double)                   noexcept;
    typedef int  CFunc2_NE(int, int)                 noexcept;
    typedef void CFunc3_NE(const char*, int, double) noexcept;

    typedef void CFunc0E_NE(...)                           noexcept;
    typedef int  CFunc1E_NE(double, ...)                   noexcept;
    typedef int  CFunc2E_NE(int, int, ...)                 noexcept;
    typedef void CFunc3E_NE(const char*, int, double, ...) noexcept;
}
#endif

// Exotic function types: unusual return types and parameter counts
typedef FP0   FuncRetFP(int);
typedef void *FuncRetVP(int);

// Function with 256 parameters, the recommended minimum in Annex B of the
// C++ Standard [implimits].
typedef int FuncMany(
    int, int, int, int, int, int, int, int, int, int, int, int, int,
    int, int, int, int, int, int, int, int, int, int, int, int, int,
    int, int, int, int, int, int, int, int, int, int, int, int, int,
    int, int, int, int, int, int, int, int, int, int, int, int, int,
    int, int, int, int, int, int, int, int, int, int, int, int, int,
    int, int, int, int, int, int, int, int, int, int, int, int, int,
    int, int, int, int, int, int, int, int, int, int, int, int, int,
    int, int, int, int, int, int, int, int, int, int, int, int, int,
    int, int, int, int, int, int, int, int, int, int, int, int, int,
    int, int, int, int, int, int, int, int, int, int, int, int, int,
    int, int, int, int, int, int, int, int, int, int, int, int, int,
    int, int, int, int, int, int, int, int, int, int, int, int, int,
    int, int, int, int, int, int, int, int, int, int, int, int, int,
    int, int, int, int, int, int, int, int, int, int, int, int, int,
    int, int, int, int, int, int, int, int, int, int, int, int, int,
    int, int, int, int, int, int, int, int, int, int, int, int, int,
    int, int, int, int, int, int, int, int, int, int, int, int, int,
    int, int, int, int, int, int, int, int, int, int, int, int, int,
    int, int, int, int, int, int, int, int, int, int, int, int, int,
    int, int, int);

// ============================================================================
//                             USAGE EXAMPLES
// ============================================================================

namespace usage {

///Example 1: Using `voidify` for Placement New
/// - - - - - - - - - - - - - - - - - - - - - -
// Suppose we are implementing a simplified `optional`-like container that
// holds a value of (template parameter) `TYPE` in a raw buffer.  `TYPE` may
// be const-qualified (e.g., `MyOptional<const int>`).
//
// First, we define the class template with an aligned buffer and a flag:
// ```
    template <class TYPE>
    class MyOptional {
        // DATA
        union {
            bsls::AlignmentUtil::MaxAlignedType d_align;
            char                                d_buf[sizeof(TYPE)];
        };
        bool d_hasValue;

      public:
        // CREATORS
        MyOptional() : d_hasValue(false) {}

        // MANIPULATORS

        /// Construct a `TYPE` object in this object's buffer having its
        /// default value, and return a reference to the newly created
        /// object.
        TYPE& emplace();

        // ACCESSORS
        bool        hasValue() const { return d_hasValue; }
        const TYPE& value()    const
        {
            return *reinterpret_cast<const TYPE *>(d_buf);
        }
    };
// ```
// Then we implement `emplace`.  We declare a pointer of the correct type to
// the return value so that we can efficiently return a reference to the object
// we are about to create.
// ```
    template <class TYPE>
    TYPE& MyOptional<TYPE>::emplace()
    {
        BSLS_ASSERT(!d_hasValue);

        TYPE *addr = reinterpret_cast<TYPE *>(d_buf);
// ```
// Now, we use `voidify` to provide the address at which to construct the new
// object.  A placement-new expression requires a `void *` operand, but `TYPE`
// may be cv-qualified and `static_cast` alone cannot produce `void *` from a
// pointer to a cv-qualified type.
// ```
        addr = ::new (bslma::PointerUtil::voidify(addr)) TYPE();
        d_hasValue = true;
        return *addr;
    }
// ```
// Finally, we can use `MyOptional` with a const-qualified type.  Without
// `voidify`, the placement new inside `emplace` would not compile:

///Example 2: Using `unqualify` for Generic Forwarding
///- - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose we are given a construction facility that constructs an object of
// (template parameter) `TYPE` at a specified address having its default
// value, and returns a pointer to the newly created object:
// ```
//  /// Construct an object of the specified (template parameter) `TYPE` at
//  /// the specified `address` having its default value, and return
//  /// `address`.
    template <class TYPE>
    TYPE *constructInPlace(TYPE *address);
// ```

}  // close namespace usage

// ============================================================================
//              ELIDED IMPLEMENTATION DETAILS FOR USAGE EXAMPLES
// ============================================================================

template <class TYPE>
TYPE *usage::constructInPlace(TYPE *address)
{
    ::new (bslma::PointerUtil::voidify(address)) TYPE();
    return address;
}

// ============================================================================
//                              MAIN PROGRAM
// ============================================================================

int main(int argc, char *argv[])
{
    const int                 test = argc > 1 ? atoi(argv[1]) : 0;
    const bool             verbose = argc > 2;
    const bool         veryVerbose = argc > 3;
    const bool     veryVeryVerbose = argc > 4;
    const bool veryVeryVeryVerbose = argc > 5;

    (void) veryVerbose;
    (void) veryVeryVerbose;
    (void) veryVeryVeryVerbose;

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:
      case 7: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE 2
        //
        // Concerns:
        // 1. That the usage example shown in the component-level
        //    documentation compiles and runs as described.
        //
        // Plan:
        // 1. Copy the usage example from the component header, changing
        //    `assert` to `ASSERT`, and execute it.  (C-1)
        //
        // Testing:
        //     USAGE EXAMPLE 2
        // --------------------------------------------------------------------

        if (verbose) puts("\nUSAGE EXAMPLE 2"
                           "\n===============");

        using namespace usage;

// When forwarding to such a facility from our own generic code, we can strip
// any cv-qualification with `unqualify` so that the downstream facility is
// always invoked with the unqualified type, avoiding a redundant template
// instantiation for each cv-variant of the same underlying type across the
// entire program.  Casting away `const` is well-defined when the underlying
// storage was not originally declared `const` -- as is typical for raw buffers
// managed by containers and allocators.  Observe that the `int *` returned by
// `constructInPlace` implicitly converts to `const int *` or `volatile int *`
// when assigned back, correctly preserving the original cv-qualification:
// ```
    union {
        bsls::AlignmentUtil::MaxAlignedType d_align;
        char                                d_buf[sizeof(int)];
    } u = {};
    int          *ip = reinterpret_cast<int *>(u.d_buf);
    const int    *cp = reinterpret_cast<const int *>(u.d_buf);
    volatile int *vp = reinterpret_cast<volatile int *>(u.d_buf);

    ip = constructInPlace(bslma::PointerUtil::unqualify(ip));
    ASSERT(0 == *ip);

    cp = constructInPlace(bslma::PointerUtil::unqualify(cp));
    ASSERT(0 == *cp);

    vp = constructInPlace(bslma::PointerUtil::unqualify(vp));
    ASSERT(0 == *vp);
// ```
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE 1
        //
        // Concerns:
        // 1. That the usage example shown in the component-level
        //    documentation compiles and runs as described.
        //
        // Plan:
        // 1. Copy the usage example from the component header, changing
        //    `assert` to `ASSERT`, and execute it.  (C-1)
        //
        // Testing:
        //     USAGE EXAMPLE 1
        // --------------------------------------------------------------------

        if (verbose) puts("\nUSAGE EXAMPLE 1"
                           "\n===============");

        using namespace usage;

// Finally, we can use `MyOptional` with a const-qualified type.  Without
// `voidify`, the placement new inside `emplace` would not compile:
// ```
    MyOptional<const int> opt;
    const int& ref = opt.emplace();
    ASSERT(opt.hasValue());
    ASSERT(0 == ref);
// ```

      } break;
      case 5: {
        // --------------------------------------------------------------------
        // CONCERN: CONSTEXPR
        //
        // Concerns:
        // 1. `unqualify` can be evaluated at compile time in C++14 and later
        //    (where `const_cast` is allowed in constant expressions).
        //
        // 2. `voidify` can be evaluated at compile time for non-function
        //    types in C++11 and later.
        //
        // 3. `voidify` for function types is not constexpr-evaluable (uses
        //    `reinterpret_cast`), but this is not tested here.
        //
        // Plan:
        // 1. Under `BSLS_COMPILERFEATURES_FULL_CPP11`, declare
        //    `constexpr` variables initialized by `unqualify` and `voidify`
        //    applied to pointers to objects with static storage duration.
        //    Verify the values at runtime.  (C-1,2)
        //
        // Testing:
        //     CONCERN: CONSTEXPR
        // --------------------------------------------------------------------

        if (verbose) puts("\nCONCERN: CONSTEXPR"
                           "\n==================");

#ifdef BSLS_COMPILERFEATURES_FULL_CPP11
        if (veryVerbose) puts("\tunqualify constexpr");
        {
            static constexpr int si = 42;

            constexpr int *p1 = Util::unqualify(&si);
            static_assert(&si == p1,
                          "unqualify failed to strip const from int");

            static constexpr const int sci = 99;
            constexpr int *p2 = Util::unqualify(&sci);
            static_assert(&sci == p2,
                          "unqualify changed address of const int");

            static int svi = 7;
            static volatile int svvi = 8;
            constexpr int *p3 = Util::unqualify(&svvi);
            static_assert(&svvi == p3,
                          "unqualify failed to strip volatile from int");
            (void) svi;
        }

        if (veryVerbose) puts("\tvoidify constexpr (non-function)");
        {
            static constexpr int si = 42;
            constexpr void *p1 = Util::voidify(&si);
            static_assert(static_cast<const void *>(&si) == p1,
                          "voidify failed for const int");

            static constexpr double sd = 3.14;
            constexpr void *p2 = Util::voidify(&sd);
            static_assert(static_cast<const void *>(&sd) == p2,
                          "voidify failed for const double");

            static const int sci = 99;
            constexpr void *p3 = Util::voidify(&sci);
            static_assert(static_cast<const void *>(&sci) == p3,
                          "voidify changed address of const int");

            static constexpr Base sb = {10};
            constexpr void *p4 = Util::voidify(&sb);
            static_assert(static_cast<const void *>(&sb) == p4,
                          "voidify failed for aggregate class type");

            // Confirm that `voidify` can be called with a string literal.
            // This is simply a syntax check as the assert condition is moot.
            static_assert(static_cast<const void *>("world") != nullptr,
                          "voidify returned null for string literal");
        }
#else
        if (veryVerbose) puts("\tSkipped (full C++11 not available)");
#endif

      } break;
      case 4: {
        // --------------------------------------------------------------------
        // CONCERN: IMPLICIT DECAY
        //
        // Concerns:
        // 1. An array name (which decays to a pointer to its first element)
        //    can be passed to `voidify` and produces the expected address.
        //
        // 2. A function name (which decays to a pointer to function) can be
        //    passed to `voidify` and produces the expected address.
        //
        // 3. A wide string literal decays correctly.
        //
        // Plan:
        // 1. Declare local arrays and pass their names to `voidify`.  Verify
        //    the returned address matches the first element.  (C-1)
        //
        // 2. Pass bare function names (with various signatures) to `voidify`.
        //    Verify the returned address equals the function pointer.  (C-2)
        //
        // 3. Pass a wide string literal to `voidify`.  (C-3)
        //
        // Testing:
        //     CONCERN: IMPLICIT DECAY
        // --------------------------------------------------------------------

        if (verbose) puts("\nCONCERN: IMPLICIT DECAY"
                           "\n=======================");

        if (veryVerbose) puts("\tArray-to-pointer decay");
        {
            int arr1d[10] = {};
            ASSERT(static_cast<void *>(+arr1d) == Util::voidify(arr1d));

            const char arr_c[5] = "abcd";
            ASSERT(static_cast<const void *>(+arr_c) == Util::voidify(arr_c));

            int arr2d[3][4] = {};
            ASSERT(static_cast<void *>(+arr2d) == Util::voidify(arr2d));
        }

        if (veryVerbose) puts("\tWide string literal decay");
        {
            ASSERT(0 != Util::voidify(L"hello"));
        }

        if (veryVerbose) puts("\tFunction-to-pointer decay");
        {
            ASSERT((void *)(+func0)  == Util::voidify(func0));
            ASSERT((void *)(+func1)  == Util::voidify(func1));
            ASSERT((void *)(+func2)  == Util::voidify(func2));
            ASSERT((void *)(+cfunc0) == Util::voidify(cfunc0));
        }

      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING `voidify`
        //
        // Concerns:
        // 1. For any pointer-to-TYPE 'p', 'voidify(p)' returns a 'void *'
        //    holding the same address value as 'p'.
        //
        // 2. For cv-qualified TYPE, the cv-qualification is properly stripped
        //    before converting to 'void *'.
        //
        // 3. 'voidify' returns null when given a null pointer.
        //
        // 4. The function works correctly for all categories of types:
        //    fundamental, void, class, incomplete, union, enum, pointer,
        //    pointer-to-member, array (known and unknown bound), and function
        //    types (with varying argument counts, C variadic, extern "C",
        //    and C++17 noexcept).
        //
        // Plan:
        // 1. For each object type in the test matrix, invoke
        //    'TEST_VOIDIFY_OBJECT_TYPE' which tests all 4 cv-qualifications
        //    of TYPE and null pointers.  (C-1..4)
        //
        // 2. For function types, invoke 'TEST_VOIDIFY_FUNC_TYPE'.  (C-4)
        //
        // Testing:
        //   void *voidify(TYPE *address);
        // --------------------------------------------------------------------

        if (verbose) puts("\nTESTING `voidify`"
                           "\n=================");

        if (veryVerbose) puts("\tFundamental types");
        {
            int    intObj    = 42;
            char   charObj   = 'x';
            double doubleObj = 3.14;
            bool   boolObj   = true;

            TEST_VOIDIFY_OBJECT_TYPE(int,    intObj);
            TEST_VOIDIFY_OBJECT_TYPE(char,   charObj);
            TEST_VOIDIFY_OBJECT_TYPE(double, doubleObj);
            TEST_VOIDIFY_OBJECT_TYPE(bool,   boolObj);
        }

        if (veryVerbose) puts("\tvoid types");
        {
            int storage = 0;
            void *addr = &storage;

            TEST_VOIDIFY_VOID_CV(              , addr);
            TEST_VOIDIFY_VOID_CV(const         , addr);
            TEST_VOIDIFY_VOID_CV(volatile      , addr);
            TEST_VOIDIFY_VOID_CV(const volatile, addr);
        }

        if (veryVerbose) puts("\tClass types");
        {
            Base    baseObj;
            baseObj.d_x = 0;
            Derived derivedObj;
            derivedObj.d_x = 0;
            derivedObj.d_y = 0;

            TEST_VOIDIFY_OBJECT_TYPE(Base,    baseObj);
            TEST_VOIDIFY_OBJECT_TYPE(Derived, derivedObj);
        }

        if (veryVerbose) puts("\tIncomplete type");
        {
            // Use raw storage; we only care about address values
            Base storage = {};
            IncompleteClass *ic =
                          reinterpret_cast<IncompleteClass *>(&storage);

            // plain
            ASSERT((void *)ic == Util::voidify(ic));
            IncompleteClass *np1 = 0;
            ASSERT((void *)0 == Util::voidify(np1));

            // const
            const IncompleteClass *cic = ic;
            ASSERT((void *)ic == Util::voidify(cic));
            const IncompleteClass *np2 = 0;
            ASSERT((void *)0 == Util::voidify(np2));

            // volatile
            volatile IncompleteClass *vic =
                const_cast<volatile IncompleteClass *>(
                    static_cast<const volatile IncompleteClass *>(ic));
            ASSERT((void *)ic == Util::voidify(vic));
            volatile IncompleteClass *np3 = 0;
            ASSERT((void *)0 == Util::voidify(np3));

            // const volatile
            const volatile IncompleteClass *cvic = ic;
            ASSERT((void *)ic == Util::voidify(cvic));
            const volatile IncompleteClass *np4 = 0;
            ASSERT((void *)0 == Util::voidify(np4));
        }

        if (veryVerbose) puts("\tUnion type");
        {
            MyUnion unionObj = {};
            TEST_VOIDIFY_OBJECT_TYPE(MyUnion, unionObj);
        }

        if (veryVerbose) puts("\tEnum type");
        {
            MyEnum enumObj = e_VALUE_A;
            TEST_VOIDIFY_OBJECT_TYPE(MyEnum, enumObj);
        }

        if (veryVerbose) puts("\tPointer types");
        {
            int i = 0;
            int *pi = &i;
            TEST_VOIDIFY_OBJECT_TYPE(int *, pi);

            const int *cpi = &i;
            TEST_VOIDIFY_OBJECT_TYPE(const int *, cpi);

            void *vptr = &i;
            TEST_VOIDIFY_OBJECT_TYPE(void *, vptr);

            // pointer-to-function
            void(*fptr)() = &func0;
            typedef void(*FP)();
            FP fpObj = fptr;
            TEST_VOIDIFY_OBJECT_TYPE(FP, fpObj);

            // pointer-to-array
            int localArr[5] = {};
            int (*arrPtr)[5] = &localArr;
            typedef int(*ArrP)[5];
            ArrP apObj = arrPtr;
            TEST_VOIDIFY_OBJECT_TYPE(ArrP, apObj);
        }

        if (veryVerbose) puts("\tPointer-to-member types");
        {
            int Base::*pdm = &Base::d_x;
            typedef int Base::*PDM;
            PDM pdmObj = pdm;
            TEST_VOIDIFY_OBJECT_TYPE(PDM, pdmObj);

            // pointer-to-member-function (use size/address of the PMF object)
            typedef void(Base::*PMF)();
            PMF pmfObj = 0;
            // Can only test that it compiles and works with null
            PMF * const pmfAddr = &pmfObj;
            ASSERT((void *)pmfAddr == Util::voidify(pmfAddr));
        }

        if (veryVerbose) puts("\tArray types (known bound)");
        {
            int arr10[10] = {};
            typedef int Arr10[10];
            TEST_VOIDIFY_OBJECT_TYPE(Arr10, arr10);

            int arr34[3][4] = {};
            typedef int Arr34[3][4];
            TEST_VOIDIFY_OBJECT_TYPE(Arr34, arr34);

            Base arrBase[2] = {};
            typedef Base ArrBase2[2];
            TEST_VOIDIFY_OBJECT_TYPE(ArrBase2, arrBase);
        }

        if (veryVerbose) puts("\tArray types (unknown bound)");
        {
            extern int externArr[];
            typedef int UBArr[];
            TEST_VOIDIFY_OBJECT_TYPE(UBArr, externArr);

            extern int externArr2d[][4];
            typedef int UBArr2d[][4];
            TEST_VOIDIFY_OBJECT_TYPE(UBArr2d, externArr2d);
        }

        if (veryVerbose) puts("\tFunction types (core)");
        {
            TEST_VOIDIFY_FUNC_TYPE(Func0, &func0);
            TEST_VOIDIFY_FUNC_TYPE(Func1, &func1);
            TEST_VOIDIFY_FUNC_TYPE(Func2, &func2);
            TEST_VOIDIFY_FUNC_TYPE(Func3, &func3);
        }

        if (veryVerbose) puts("\tFunction types (ellipsis)");
        {
            TEST_VOIDIFY_FUNC_TYPE(Func0E, &func0E);
            TEST_VOIDIFY_FUNC_TYPE(Func1E, &func1E);
            TEST_VOIDIFY_FUNC_TYPE(Func2E, &func2E);
            TEST_VOIDIFY_FUNC_TYPE(Func3E, &func3E);
        }

        if (veryVerbose) puts("\tFunction types (extern \"C\")");
        {
            TEST_VOIDIFY_FUNC_TYPE(CFunc0,  &cfunc0);
            TEST_VOIDIFY_FUNC_TYPE(CFunc1,  &cfunc1);
            TEST_VOIDIFY_FUNC_TYPE(CFunc2,  &cfunc2);
            TEST_VOIDIFY_FUNC_TYPE(CFunc3,  &cfunc3);
            TEST_VOIDIFY_FUNC_TYPE(CFunc0E, &cfunc0E);
            TEST_VOIDIFY_FUNC_TYPE(CFunc1E, &cfunc1E);
            TEST_VOIDIFY_FUNC_TYPE(CFunc2E, &cfunc2E);
            TEST_VOIDIFY_FUNC_TYPE(CFunc3E, &cfunc3E);
        }

#ifdef BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT_IN_FNC_TYPE
        if (veryVerbose) puts("\tFunction types (noexcept, C++17)");
        {
            TEST_VOIDIFY_FUNC_TYPE(Func0_NE,   &func0_NE);
            TEST_VOIDIFY_FUNC_TYPE(Func1_NE,   &func1_NE);
            TEST_VOIDIFY_FUNC_TYPE(Func2_NE,   &func2_NE);
            TEST_VOIDIFY_FUNC_TYPE(Func3_NE,   &func3_NE);

            TEST_VOIDIFY_FUNC_TYPE(Func0E_NE,  &func0E_NE);
            TEST_VOIDIFY_FUNC_TYPE(Func1E_NE,  &func1E_NE);
            TEST_VOIDIFY_FUNC_TYPE(Func2E_NE,  &func2E_NE);
            TEST_VOIDIFY_FUNC_TYPE(Func3E_NE,  &func3E_NE);

            TEST_VOIDIFY_FUNC_TYPE(CFunc0_NE,  &cfunc0_NE);
            TEST_VOIDIFY_FUNC_TYPE(CFunc1_NE,  &cfunc1_NE);
            TEST_VOIDIFY_FUNC_TYPE(CFunc2_NE,  &cfunc2_NE);
            TEST_VOIDIFY_FUNC_TYPE(CFunc3_NE,  &cfunc3_NE);

            TEST_VOIDIFY_FUNC_TYPE(CFunc0E_NE, &cfunc0E_NE);
            TEST_VOIDIFY_FUNC_TYPE(CFunc1E_NE, &cfunc1E_NE);
            TEST_VOIDIFY_FUNC_TYPE(CFunc2E_NE, &cfunc2E_NE);
            TEST_VOIDIFY_FUNC_TYPE(CFunc3E_NE, &cfunc3E_NE);
        }
#endif

        if (veryVerbose) puts("\tFunction types (exotic)");
        {
            TEST_VOIDIFY_FUNC_TYPE(FuncRetFP,  &funcRetFP);
            TEST_VOIDIFY_FUNC_TYPE(FuncRetVP,  &funcRetVP);
            TEST_VOIDIFY_FUNC_TYPE(FuncMany,   &funcMany);
        }

#ifdef BSLS_COMPILERFEATURES_FULL_CPP11
        if (veryVerbose) puts("\tCaptureless lambda");
        {
            void (*lp)(int) = [](int) {};
            ASSERT((void *)lp == Util::voidify(lp));
        }
#endif

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING `unqualify`
        //
        // Concerns:
        // 1. For a pointer to 'const TYPE', 'volatile TYPE', or
        //    'const volatile TYPE', 'unqualify' returns a 'TYPE *' with the
        //    same address value.
        //
        // 2. 'unqualify' returns null when given a null pointer.
        //
        // 3. The function works correctly for all categories of object types:
        //    fundamental, void, class, incomplete, union, enum, pointer,
        //    pointer-to-member, and array types.
        //
        // 4. For function types, 'unqualify' on a function pointer returns a
        //    function pointer with the same value.
        //
        // Plan:
        // 1. For each object type in the test matrix, invoke
        //    'TEST_UNQUALIFY_TYPE' which tests 'const', 'volatile', and
        //    'const volatile' qualifications and null pointers.  (C-1..3)
        //
        // 2. For each function type in the catalog, invoke
        //    'TEST_UNQUALIFY_FUNC_TYPE' which tests that 'unqualify' on a
        //    function pointer returns the original pointer.  (C-4)
        //
        // Testing:
        //   TYPE *unqualify(               TYPE *address);
        //   TYPE *unqualify(const          TYPE *address);
        //   TYPE *unqualify(      volatile TYPE *address);
        //   TYPE *unqualify(const volatile TYPE *address);
        // --------------------------------------------------------------------

        if (verbose) puts("\nTESTING `unqualify`"
                           "\n===================");

        if (veryVerbose) puts("\tFundamental types");
        {
            int    intObj    = 42;
            char   charObj   = 'x';
            double doubleObj = 3.14;
            bool   boolObj   = true;

            TEST_UNQUALIFY_TYPE(int,    intObj);
            TEST_UNQUALIFY_TYPE(char,   charObj);
            TEST_UNQUALIFY_TYPE(double, doubleObj);
            TEST_UNQUALIFY_TYPE(bool,   boolObj);
        }

        if (veryVerbose) puts("\tvoid");
        {
            int storage = 0;
            void *voidAddr = &storage;
            {
                const void *p = voidAddr;
                ASSERT(voidAddr == Util::unqualify(p));
                const void *np = 0;
                ASSERT((void *)0 == Util::unqualify(np));
            }
            {
                volatile void *p = voidAddr;
                ASSERT(voidAddr == Util::unqualify(p));
                volatile void *np = 0;
                ASSERT((void *)0 == Util::unqualify(np));
            }
            {
                const volatile void *p = voidAddr;
                ASSERT(voidAddr == Util::unqualify(p));
                const volatile void *np = 0;
                ASSERT((void *)0 == Util::unqualify(np));
            }
        }

        if (veryVerbose) puts("\tClass types");
        {
            Base    baseObj;
            baseObj.d_x = 0;
            Derived derivedObj;
            derivedObj.d_x = 0;
            derivedObj.d_y = 0;

            TEST_UNQUALIFY_TYPE(Base,    baseObj);
            TEST_UNQUALIFY_TYPE(Derived, derivedObj);
        }

        if (veryVerbose) puts("\tIncomplete type");
        {
            // Use a Base object as stand-in storage
            Base storage = {};
            IncompleteClass *ic =
                          reinterpret_cast<IncompleteClass *>(&storage);
            const IncompleteClass *cp = ic;
            ASSERT(ic == Util::unqualify(cp));
            volatile IncompleteClass *vp =
                          const_cast<volatile IncompleteClass *>(
                              static_cast<const volatile IncompleteClass *>(
                                  ic));
            ASSERT(ic == Util::unqualify(vp));
            const volatile IncompleteClass *cvp = ic;
            ASSERT(ic == Util::unqualify(cvp));
            const IncompleteClass *np = 0;
            ASSERT((IncompleteClass *)0 == Util::unqualify(np));
        }

        if (veryVerbose) puts("\tUnion");
        {
            MyUnion unionObj = {};
            TEST_UNQUALIFY_TYPE(MyUnion, unionObj);
        }

        if (veryVerbose) puts("\tEnum");
        {
            MyEnum enumObj = e_VALUE_A;
            TEST_UNQUALIFY_TYPE(MyEnum, enumObj);
        }

        if (veryVerbose) puts("\tPointer types");
        {
            int i = 0;
            int *pi = &i;
            TEST_UNQUALIFY_TYPE(int *, pi);

            const int *cpi = &i;
            TEST_UNQUALIFY_TYPE(const int *, cpi);

            void *vptr = &i;
            TEST_UNQUALIFY_TYPE(void *, vptr);

            typedef void(*FP)();
            FP fpObj = &func0;
            TEST_UNQUALIFY_TYPE(FP, fpObj);

            int localArr[5] = {};
            int (*arrPtr)[5] = &localArr;
            typedef int (*ArrP)[5];
            ArrP apObj = arrPtr;
            TEST_UNQUALIFY_TYPE(ArrP, apObj);
        }

        if (veryVerbose) puts("\tPointer-to-member types");
        {
            int Base::*pdm = &Base::d_x;
            typedef int Base::*PDM;
            PDM pdmObj = pdm;
            TEST_UNQUALIFY_TYPE(PDM, pdmObj);
        }

        if (veryVerbose) puts("\tArray types (known bound)");
        {
            int arr10[10] = {};
            typedef int Arr10[10];
            TEST_UNQUALIFY_TYPE(Arr10, arr10);

            int arr34[3][4] = {};
            typedef int Arr34[3][4];
            TEST_UNQUALIFY_TYPE(Arr34, arr34);

            Base arrBase[2] = {};
            typedef Base ArrBase2[2];
            TEST_UNQUALIFY_TYPE(ArrBase2, arrBase);
        }

        if (veryVerbose) puts("\tArray types (unknown bound)");
        {
            extern int externArr[];
            typedef int UBArr[];
            TEST_UNQUALIFY_TYPE(UBArr, externArr);

            extern int externArr2d[][4];
            typedef int UBArr2d[][4];
            TEST_UNQUALIFY_TYPE(UBArr2d, externArr2d);
        }

        if (veryVerbose) puts("\tFunction types (core)");
        {
            TEST_UNQUALIFY_FUNC_TYPE(Func0, &func0);
            TEST_UNQUALIFY_FUNC_TYPE(Func1, &func1);
            TEST_UNQUALIFY_FUNC_TYPE(Func2, &func2);
            TEST_UNQUALIFY_FUNC_TYPE(Func3, &func3);
        }

        if (veryVerbose) puts("\tFunction types (ellipsis)");
        {
            TEST_UNQUALIFY_FUNC_TYPE(Func0E, &func0E);
            TEST_UNQUALIFY_FUNC_TYPE(Func1E, &func1E);
            TEST_UNQUALIFY_FUNC_TYPE(Func2E, &func2E);
            TEST_UNQUALIFY_FUNC_TYPE(Func3E, &func3E);
        }

        if (veryVerbose) puts("\tFunction types (extern \"C\")");
        {
            TEST_UNQUALIFY_FUNC_TYPE(CFunc0,  &cfunc0);
            TEST_UNQUALIFY_FUNC_TYPE(CFunc1,  &cfunc1);
            TEST_UNQUALIFY_FUNC_TYPE(CFunc2,  &cfunc2);
            TEST_UNQUALIFY_FUNC_TYPE(CFunc3,  &cfunc3);
            TEST_UNQUALIFY_FUNC_TYPE(CFunc0E, &cfunc0E);
            TEST_UNQUALIFY_FUNC_TYPE(CFunc1E, &cfunc1E);
            TEST_UNQUALIFY_FUNC_TYPE(CFunc2E, &cfunc2E);
            TEST_UNQUALIFY_FUNC_TYPE(CFunc3E, &cfunc3E);
        }

#ifdef BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT_IN_FNC_TYPE
        if (veryVerbose) puts("\tFunction types (noexcept, C++17)");
        {
            TEST_UNQUALIFY_FUNC_TYPE(Func0_NE,   &func0_NE);
            TEST_UNQUALIFY_FUNC_TYPE(Func1_NE,   &func1_NE);
            TEST_UNQUALIFY_FUNC_TYPE(Func2_NE,   &func2_NE);
            TEST_UNQUALIFY_FUNC_TYPE(Func3_NE,   &func3_NE);

            TEST_UNQUALIFY_FUNC_TYPE(Func0E_NE,  &func0E_NE);
            TEST_UNQUALIFY_FUNC_TYPE(Func1E_NE,  &func1E_NE);
            TEST_UNQUALIFY_FUNC_TYPE(Func2E_NE,  &func2E_NE);
            TEST_UNQUALIFY_FUNC_TYPE(Func3E_NE,  &func3E_NE);

            TEST_UNQUALIFY_FUNC_TYPE(CFunc0_NE,  &cfunc0_NE);
            TEST_UNQUALIFY_FUNC_TYPE(CFunc1_NE,  &cfunc1_NE);
            TEST_UNQUALIFY_FUNC_TYPE(CFunc2_NE,  &cfunc2_NE);
            TEST_UNQUALIFY_FUNC_TYPE(CFunc3_NE,  &cfunc3_NE);

            TEST_UNQUALIFY_FUNC_TYPE(CFunc0E_NE, &cfunc0E_NE);
            TEST_UNQUALIFY_FUNC_TYPE(CFunc1E_NE, &cfunc1E_NE);
            TEST_UNQUALIFY_FUNC_TYPE(CFunc2E_NE, &cfunc2E_NE);
            TEST_UNQUALIFY_FUNC_TYPE(CFunc3E_NE, &cfunc3E_NE);
        }
#endif

        if (veryVerbose) puts("\tFunction types (exotic)");
        {
            TEST_UNQUALIFY_FUNC_TYPE(FuncRetFP,  &funcRetFP);
            TEST_UNQUALIFY_FUNC_TYPE(FuncRetVP,  &funcRetVP);
            TEST_UNQUALIFY_FUNC_TYPE(FuncMany,   &funcMany);
        }

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //   This case exercises (but does not fully test) basic
        //   functionality.
        //
        // Concerns:
        // 1. The class is sufficiently functional to enable comprehensive
        //    testing in subsequent test cases.
        //
        // Plan:
        // 1. Exercise `unqualify` and `voidify` with a representative type
        //    (`int`) to verify basic operation.  (C-1)
        //
        // Testing:
        //     BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) puts("\nBREATHING TEST"
                           "\n==============");

        int  intObj = 42;
        int *intPtr = &intObj;

        if (veryVerbose) puts("\tunqualify");
        {
            const int          *cp  = intPtr;
            volatile int       *vp  = intPtr;
            const volatile int *cvp = intPtr;

            ASSERT(intPtr == Util::unqualify(cp));
            ASSERT(intPtr == Util::unqualify(vp));
            ASSERT(intPtr == Util::unqualify(cvp));

            const int *np = 0;
            ASSERT(0 == Util::unqualify(np));
        }

        if (veryVerbose) puts("\tvoidify (object type)");
        {
            void *expected = intPtr;

            ASSERT(expected == Util::voidify(intPtr));

            const int          *cp  = intPtr;
            volatile int       *vp  = intPtr;
            const volatile int *cvp = intPtr;

            ASSERT(expected == Util::voidify(cp));
            ASSERT(expected == Util::voidify(vp));
            ASSERT(expected == Util::voidify(cvp));

            int *np = 0;
            ASSERT(0 == Util::voidify(np));
        }

        if (veryVerbose) puts("\tvoidify (function type)");
        {
            void (*fp)() = &func0;
            ASSERT((void *)fp == Util::voidify(fp));

            void (*nfp)() = 0;
            ASSERT(0 == Util::voidify(nfp));
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

// Provide storage for the extern array-of-unknown-bound tests
int externArr[]     = { 10, 20, 30 };
int externArr2d[][4] = { {1,2,3,4}, {5,6,7,8}, {9,10,11,12} };

// ----------------------------------------------------------------------------
// Copyright 2026 Bloomberg Finance L.P.
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
