// bsls_compilerfeatures.t.cpp                                        -*-C++-*-
#include <bsls_compilerfeatures.h>

#include <bsls_bsltestutil.h>
#include <bsls_buildtarget.h>
#include <bsls_platform.h>

#include <exception>    // testing exception specifications
#include <limits>       // testing hexfloat literals

#include <stdio.h>      // 'printf', 'puts', 'fwrite'
#include <stdlib.h>     // 'atoi'
#include <string.h>     // 'memcmp'

#ifdef BSLS_COMPILERFEATURES_SUPPORT_THREE_WAY_COMPARISON
    #include <compare>
#endif

#if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
    #include <initializer_list>
#endif

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                            * Overview *
// Testing available C++ language features by trying to compile code that uses
// them.  For example, if 'BSLS_COMPILERFEATURES_SUPPORT_STATIC_ASSERT' is
// defined, then we try to compile code that uses 'static_assert'.  Many of
// the tests are purely compile-time; if the code compiles, then the test
// succeeds.  Due to the limitations of the testing framework, there is no way
// to turn compile-time failures into runtime failures.  Note that we don't
// intend to test the correctness of the implementation of C++ features, but
// just the fact that features are supported.  However, if a feature
// is known to be buggy or incomplete in some implementations such that it is
// not useful to us, the corresponding macro should be turned off for that
// platform.  For these cases, this test driver may contain a minimal test of
// that feature that ensures that the Bloomberg use of the feature is supported
// on any platform that defines that macro.
//-----------------------------------------------------------------------------
// [32] BSLS_COMPILERFEATURES_GUARANTEED_COPY_ELISION
// [24] BSLS_COMPILERFEATURES_INITIALIZER_LIST_LEAKS_ON_EXCEPTIONS
// [33] BSLS_COMPILERFEATURES_PP_LINE_IS_ON_FIRST
// [ 1] BSLS_COMPILERFEATURES_SUPPORT_ALIAS_TEMPLATES
// [19] BSLS_COMPILERFEATURES_SUPPORT_ALIGNAS
// [25] BSLS_COMPILERFEATURES_SUPPORT_ATTRIBUTE_NORETURN
// [26] BSLS_COMPILERFEATURES_SUPPORT_ATTRIBUTE_NODISCARD
// [27] BSLS_COMPILERFEATURES_SUPPORT_ATTRIBUTE_FALLTHROUGH
// [28] BSLS_COMPILERFEATURES_SUPPORT_ATTRIBUTE_MAYBE_UNUSED
// [ 2] BSLS_COMPILERFEATURES_SUPPORT_CONSTEXPR
// [ 3] BSLS_COMPILERFEATURES_SUPPORT_CONSTEXPR_CPP14
// [ 4] BSLS_COMPILERFEATURES_SUPPORT_CONSTEXPR_CPP17
// [34] BSLS_COMPILERFEATURES_SUPPORT_CTAD
// [ 5] BSLS_COMPILERFEATURES_SUPPORT_DECLTYPE
// [  ] BSLS_COMPILERFEATURES_SUPPORT_DEFAULT_TEMPLATE_ARGS
// [ 6] BSLS_COMPILERFEATURES_SUPPORT_DEFAULTED_FUNCTIONS
// [ 7] BSLS_COMPILERFEATURES_SUPPORT_DELETED_FUNCTIONS
// [  ] BSLS_COMPILERFEATURES_SUPPORT_ENUM_CLASS
// [ 8] BSLS_COMPILERFEATURES_SUPPORT_EXTERN_TEMPLATE
// [ 9] BSLS_COMPILERFEATURES_SUPPORT_FINAL
// [10] BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS
// [23] BSLS_COMPILERFEATURES_SUPPORT_HAS_INCLUDE
// [35] BSLS_COMPILERFEATURES_SUPPORT_HEXFLOAT_LITERALS
// [11] BSLS_COMPILERFEATURES_SUPPORT_INCLUDE_NEXT
// [  ] BSLS_COMPILERFEATURES_SUPPORT_INLINE_NAMESPACE
// [  ] BSLS_COMPILERFEATURES_SUPPORT_INLINE_VARIABLES
// [12] BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT
// [31] BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT_IN_FNC_TYPE
// [31] BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT_TYPES -- deprecated
// [13] BSLS_COMPILERFEATURES_SUPPORT_NULLPTR
// [14] BSLS_COMPILERFEATURES_SUPPORT_OPERATOR_EXPLICIT
// [15] BSLS_COMPILERFEATURES_SUPPORT_OVERRIDE
// [30] BSLS_COMPILERFEATURES_SUPPORT_RAW_STRINGS
// [20] BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS
// [16] BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
// [17] BSLS_COMPILERFEATURES_SUPPORT_STATIC_ASSERT
// [36] BSLS_COMPILERFEATURES_SUPPORT_THREE_WAY_COMPARISON
// [29] BSLS_COMPILERFEATURES_SUPPORT_THROW_SPECIFICATIONS
// [  ] BSLS_COMPILERFEATURES_SUPPORT_TRAITS_HEADER
// [21] BSLS_COMPILERFEATURES_SUPPORT_UNICODE_CHAR_TYPES
// [22] BSLS_COMPILERFEATURES_SUPPORT_UTF8_CHAR_TYPE
// [  ] BSLS_COMPILERFEATURES_SUPPORT_VARIABLE_TEMPLATES
// [18] BSLS_COMPILERFEATURES_SUPPORT_VARIADIC_TEMPLATES
// [  ] BSLS_COMPILERFEATURES_FORWARD_REF
// [  ] BSLS_COMPILERFEATURES_FORWARD
// ----------------------------------------------------------------------------
// [37] USAGE EXAMPLE

#ifdef BDE_VERIFY
// Suppress some pedantic bde_verify checks in this test driver
#pragma bde_verify -FD01   // Function must have contract
#pragma bde_verify -MN01   // Class data members must be private
#pragma bde_verify -MN03   // Constant member names must begin w/s_ or k_
#pragma bde_verify -FE01   // Exception type is not derived from std::exception
#pragma bde_verify -FABC01 // Function not in alphanumeric order
#pragma bde_verify -TY02   // Template parameter uses single-letter name
#pragma bde_verify -FD03   // Parameter is not documented in function contract
#pragma bde_verify -IND01  // Possibly mis-indented line
#endif

using namespace BloombergLP;

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

#define STRINGIFY2(...) "" #__VA_ARGS__
#define STRINGIFY(a) STRINGIFY2(a)

//=============================================================================
//                'u_HAS_CPP_ATTRIBUTE(attribute_token)'
//-----------------------------------------------------------------------------

// We define 'u_HAS_CPP_ATTRIBUTE(attribute_token)' to either retrieve the
// value of '__has_cpp_attribute(attribute_token)' on platforms that support
// it, or zero on platforms that don't.  This simplifies otherwise very long
// preprocessor conditions that would need to check for the presence of
// '__has_cpp_attribute'.

#if (__cplusplus >= 201103L) && defined(__has_cpp_attribute)
    #define u_HAS_CPP_ATTRIBUTE(attribute_token)                              \
                                           __has_cpp_attribute(attribute_token)
#else
    #define u_HAS_CPP_ATTRIBUTE(attribute_token) 0
        // We cannot check for specific attribute support on this platform.
#endif

//=============================================================================
//                  NON-STANDARD MACROS FOR READABILITY
//-----------------------------------------------------------------------------

// Due to long macro names printing test case titles can easily run out of the
// 79 characters line limit.  Wrapper renders the code irregular and harder to
// read.  The 'VERBOSE_PUTS(str)' macro makes the very frequently repeated
// 'if (verbose) puts("string-literal")' less "busy" looking, and shorter.  The
// 'MACRO_TEST_TITLE(suffix, line)' macro makes long titles readable by
// eliminating the need to specify the "BSLS_COMPILERFEATURES" prefix.

#ifdef VERBOSE_PUTS
  #undef VERBOSE_PUTS
#endif
#define VERBOSE_PUTS(str) do { if (verbose) puts(str); } while (false)
    // Print the specified 'str' string literal or C-string using the 'stdio.h'
    // 'puts' function unless 'false == verbose'.

#ifdef VERYVERBOSE_PUTS
  #undef VERYVERBOSE_PUTS
#endif
#define VERYVERBOSE_PUTS(str) do { if (veryVerbose) puts(str); } while (false)
    // Print the specified 'str' string literal or C-string using the 'stdio.h'
    // 'puts' function unless 'false == veryVerbose'.

#ifdef VERBOSE_P
  #undef VERBOSE_P
#endif
#define VERBOSE_P(expr) do { if (verbose) { P(expr); } } while (false)
    // Print the specified 'expr' using 'P' unless 'false == verbose'.

#ifdef VERYVERBOSE_P
  #undef VERYVERBOSE_P
#endif
#define VERYVERBOSE_P(expr) do { if (veryVerbose) { P(expr); } } while (false)
    // Print the specified 'expr' using 'P' unless 'false == veryVerbose'.

#ifdef MACRO_TEST_TITLE
  #undef MACRO_TEST_TITLE
#endif
#define MACRO_TEST_TITLE(suffix, line)                                        \
    VERBOSE_PUTS("\nBSLS_COMPILERFEATURES" suffix                             \
                 "\n=====================" line)
    // Print the standard test case title for the specified macro 'suffix'
    // string literal for a macro that starts with "BSLS_COMPILERFEATURES".
    // Use the specified 'line' string literal as underline for the suffix
    // portion.  The behavior is ugly unless 'line' consists of '=' characters
    // only, and has the same length as 'suffix'.

//=============================================================================
//              SUPPORTING FUNCTIONS AND TYPES USED FOR TESTING
//-----------------------------------------------------------------------------

#if defined(BSLS_COMPILERFEATURES_SUPPORT_EXTERN_TEMPLATE)
namespace externTemplateTesting {
// Note that 'extern template' declarations cannot have internal linkage, so
// are not allowed in unnamed namespaces, even for testing the feature.

// define class template
template <class TYPE>
class ExternTemplateClass {};

// don't instantiate in this translation unit
extern template class ExternTemplateClass<char>;

// instantiate in this translation unit
template class ExternTemplateClass<char>;

}  // close namespace externTemplateTesting
#endif  // BSLS_COMPILERFEATURES_SUPPORT_EXTERN_TEMPLATE

namespace {

// "Runtime" is-same-type.
template <class TYPE>
bool isSameType(const TYPE&, const TYPE&) {
    return true;
}

template <class LHSTYPE, class RHSTYPE>
bool isSameType(const LHSTYPE&, const RHSTYPE&) {
    return false;
}

// "Compile time" is-same-type.
template <class U, class V>
struct is_same {
    static const bool value = false;
};

template <class T>
struct is_same<T,T> {
    static const bool value = true;
};

#if defined(BSLS_COMPILERFEATURES_SUPPORT_ALIAS_TEMPLATES)

template <class TYPE, class OTHER>
struct alias_base {};

using my_own_int = int;
using alias_nontemplate = alias_base<int, char>;
template <class TYPE> using alias_template1 = alias_base<TYPE, int>;
template <class TYPE> using alias_template2 = alias_base<char, TYPE>;

#endif  // BSLS_COMPILERFEATURES_SUPPORT_ALIAS_TEMPLATES

#if defined(BSLS_COMPILERFEATURES_SUPPORT_CONSTEXPR)

int x; // not constant
struct A {
    constexpr explicit A(bool b) : d_m(b?42:x) { }
    int d_m;
};

class OracleMiscompile {
    // This class and out-of-line constructor demonstrate a known problem when
    // trying to support 'constexpr' with Oracle studio CC 5.12.4
    unsigned d_data[2];

  public:
    // CREATOR
    constexpr OracleMiscompile();

    // ACCESSOR
    const unsigned *data() {
        return d_data;
    }
};

constexpr OracleMiscompile::OracleMiscompile() : d_data() {}

template <class TYPE>
struct aggregate_base {};

template <class TYPE>
struct aggregate_derived : aggregate_base<TYPE> {};

#ifdef BSLS_PLATFORM_PRAGMA_GCC_DIAGNOSTIC_CLANG
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wunneeded-internal-declaration"
#endif
void test_dependent_constexpr_aggregate() {
    // The following line is a regression that will not compile with Oracle CC
    // 12.5/6, and is a significant problem for type traits.
    constexpr aggregate_derived<bool> X{};    (void)X;

    (void)&test_dependent_constexpr_aggregate;
}
#ifdef BSLS_PLATFORM_PRAGMA_GCC_DIAGNOSTIC_CLANG
    #pragma GCC diagnostic pop
#endif

#endif // BSLS_COMPILERFEATURES_SUPPORT_CONSTEXPR

#if defined(BSLS_COMPILERFEATURES_SUPPORT_DECLTYPE)

// libc++ defines 'decltype' as a function-like macro when it is not provided
// by the compiler.  Our test should not be fooled by this macro.
#ifdef decltype
    #undef decltype
#endif

#ifdef BSLS_PLATFORM_HAS_PRAGMA_GCC_DIAGNOSTIC
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wunused-function"
#endif

char testFuncForDecltype(int);

#ifdef BSLS_PLATFORM_HAS_PRAGMA_GCC_DIAGNOSTIC
    #pragma GCC diagnostic pop
#endif

template <class T, class U>
auto my_max(const T& t, const U& u) -> decltype(t > u ? t : u) {
    return t > u ? t : u;
}

#endif  // BSLS_COMPILERFEATURES_SUPPORT_DECLTYPE

#if defined(BSLS_COMPILERFEATURES_SUPPORT_DEFAULTED_FUNCTIONS)
struct ClassWithDefaultOps {
    explicit ClassWithDefaultOps(int value) : d_value(value) {}

    ClassWithDefaultOps()                                       = default;
    ClassWithDefaultOps(const ClassWithDefaultOps &)            = default;
    ClassWithDefaultOps& operator=(const ClassWithDefaultOps &) = default;

    ClassWithDefaultOps(ClassWithDefaultOps &&)                 = default;
    ClassWithDefaultOps & operator=(ClassWithDefaultOps &&)     = default;

    ~ClassWithDefaultOps()                                      = default;

    int d_value;
};
#endif  //BSLS_COMPILERFEATURES_SUPPORT_DEFAULTED_FUNCTIONS

#if defined(BSLS_COMPILERFEATURES_SUPPORT_DEFAULT_TEMPLATE_ARGS)
// This test is distilled from 'shared_ptr' use of the feature that has been
// shown to crash on at least one buggy compiler.

typedef decltype(nullptr) nullptr_t;  // avoids dependencies

template <class ELEMENT>
struct Meta {
    typedef ELEMENT type;
};

template <class ELEMENT>
struct SmartPtrWithSfinaeConstructor {
    // CREATORS
    template <class ANY_TYPE, typename Meta<ANY_TYPE>::type * = nullptr>
    SmartPtrWithSfinaeConstructor(ANY_TYPE *ptr);                   // IMPLICIT

    SmartPtrWithSfinaeConstructor(nullptr_t) {}                     // IMPLICIT
};

void test_default_template_args() {
    SmartPtrWithSfinaeConstructor<int> x = 0;   // Oracle CC 12.4 asserts.
    (void)x;
    (void)&test_default_template_args;  // Portably silence unused warnings.
}
#endif  //BSLS_COMPILERFEATURES_SUPPORT_DEFAULT_TEMPLATE_ARGS

#if defined(BSLS_COMPILERFEATURES_SUPPORT_DELETED_FUNCTIONS)
struct ClassWithDeletedOps {
    ClassWithDeletedOps()                                   = delete;

    ClassWithDeletedOps(const ClassWithDeletedOps &)        = delete;
    ClassWithDeletedOps& operator=(ClassWithDeletedOps &)   = delete;

    ClassWithDeletedOps(ClassWithDeletedOps &&)             = delete;
    ClassWithDeletedOps & operator=(ClassWithDeletedOps &&) = delete;

    ~ClassWithDeletedOps()                                  = delete;
};
#endif  //BSLS_COMPILERFEATURES_SUPPORT_DELETED_FUNCTIONS

#if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
namespace initializerFeatureTest {
// The following code demonstrates a bug with Oracle CC 12.4, where the
// 'initializer_list' method dominates another single-argument method in
// overload resolution, despite not having a suitable conversion to the
// required 'initializer_list' instantiation, and so rejecting a valid call.
// The 'use' function is invoked directly from the test case in 'main'.

struct object {
    object() {}
};

template <class T, class U>
struct couple {
    couple(const T&, const U&) {}
};

template <class T, class U>
struct coupling {
    typedef couple<T, U> value_type;

    void use(const value_type&) {}
    void use(std::initializer_list<value_type>) {}
};

}  // close namespace initializerFeatureTest
#endif

#if defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
void noexceptTest1()    noexcept                               {}
void noexceptTest2()    noexcept(true)                         {}
void noexceptTest3()    noexcept(noexcept(noexceptTest1()))    {}
void notNoexceptTest1() noexcept(false)                        {}
void notNoexceptTest2() noexcept(noexcept(notNoexceptTest1())) {}
#endif  // BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT

#if defined(BSLS_COMPILERFEATURES_SUPPORT_NULLPTR)
void OverloadForNullptr(int) {
    (void)static_cast<void(*)(int)>(&OverloadForNullptr);
    // Portably silence unused function warnings.
}
void OverloadForNullptr(void *) {}
#endif  // BSLS_COMPILERFEATURES_SUPPORT_NULLPTR

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
// Check support for the perfect forwarding idiom
template <class TYPE>
struct my_remove_reference {
    typedef TYPE type;
};

template <class TYPE>
struct my_remove_reference<TYPE&> {
    typedef TYPE type;
};

template <class TYPE>
struct my_remove_reference<TYPE&&> {
    typedef TYPE type;
};

template <class TYPE>
TYPE&& my_forward(typename my_remove_reference<TYPE>::type& t) {
    return static_cast<TYPE&&>(t);
}

template <class TYPE>
TYPE&& my_forward(typename my_remove_reference<TYPE>::type&& t) {
    return static_cast<TYPE&&>(t);
}

template <class TYPE>
typename my_remove_reference<TYPE>::type&& my_move(TYPE&& t) {
    return static_cast<typename my_remove_reference<TYPE>::type&&>(t);
}

template <class TYPE, class ARG>
TYPE my_factory(ARG&& arg) {
    return my_move(TYPE(my_forward<ARG>(arg)));
}

struct RvalueArg {};

struct RvalueTest {
    RvalueTest(RvalueArg const &) {}                                // IMPLICIT
};

// Check for support for move-constructors declared with a 'typedef'.  This is
// known to expose a bug on some compilers that causes issues for our
// compatibility with emulated C++03 move.

template <class TYPE>
struct my_movable_ref_helper {
    // The class template 'MovableRef_Helper' just defines a nested type 'type'
    // that is used by an alias template.  Using this indirection the template
    // argument of the alias template is prevented from being deduced.
    using type = TYPE&&;
        // The type 'type' defined to be an r-value reference to the argument
        // type of 'MovableRef_Helper'.
};

template <class TYPE>
using my_movable_ref = typename my_movable_ref_helper<TYPE>::type;
    // The alias template 'MovableRef<TYPE>' yields an r-value reference of
    // type 'TYPE&&'.

template <class TYPE>
struct TemplateType {
    TemplateType();
    TemplateType(const TemplateType&);

    template <class OTHER>
    TemplateType(my_movable_ref<TemplateType<OTHER> >);

    template <class OTHER>
    TemplateType(my_movable_ref<OTHER>);

    TemplateType& operator=(const TemplateType&);

    template <class OTHER>
    TemplateType& operator=(my_movable_ref<TemplateType<OTHER> >);

    template <class OTHER>
    TemplateType& operator=(my_movable_ref<OTHER>);
};

template <class TYPE>
TemplateType<TYPE>::TemplateType() {}

template <class TYPE>
TemplateType<TYPE>::TemplateType(const TemplateType&) {}

template <class TYPE>
template <class OTHER>
TemplateType<TYPE>::TemplateType(my_movable_ref<TemplateType<OTHER> >) {}

template <class TYPE>
template <class OTHER>
TemplateType<TYPE>::TemplateType(my_movable_ref<OTHER>) {}

template <class TYPE>
TemplateType<TYPE>&
TemplateType<TYPE>::operator=(const TemplateType&) {
    return *this;
}

template <class TYPE>
template <class OTHER>
TemplateType<TYPE>&
TemplateType<TYPE>::operator=(my_movable_ref<TemplateType<OTHER> >) {
    return *this;
}

template <class TYPE>
template <class OTHER>
TemplateType<TYPE>&
TemplateType<TYPE>::operator=(my_movable_ref<OTHER>) {
    return *this;
}

template <class TYPE>
TYPE make_rvalue() {
    return TYPE();
}

// Further test for deduction in the presence of a movable-ref alias template
struct Utility {
    template <class TYPE>
    static void sink(TYPE *, my_movable_ref<TYPE>) {}
};

template <class TYPE>
struct Wrapper {
    void test() {
        TYPE x = TYPE();
        Utility::sink(&x, TYPE());
    }
};

#if defined(BSLS_COMPILERFEATURES_SUPPORT_ALIAS_TEMPLATES)

// The following test exposes a bug specific to the Oracle Developer Studio
// 12.6 beta compiler CC when reference-collapsing lvalue-references with
// rvalue-references provided in a non-deducing context.  This test also relies
// on alias templates to set up the error condition, but it prevents us
// migrating code from C++03 -> C++11 with our move-emulation library.

template <class TYPE>
struct RValueType { using type = TYPE &&; };

template <class TYPE>
using RValueeRef = typename RValueType<TYPE>::type;

template <class>
struct AClassTemplate {};

void showRefCollapsingBug() {
    AClassTemplate< RValueeRef<int>& > X;
    (void)X;                        // silence 'unused'
    (void)&showRefCollapsingBug;    // silence 'never called'
}
#endif  // BSLS_COMPILERFEATURES_SUPPORT_ALIAS_TEMPLATES
#endif  // BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES

#if defined(BSLS_COMPILERFEATURES_SUPPORT_VARIADIC_TEMPLATES)
template <class... TYPES>
struct PackSize;

template <class HEAD, class... TAIL>
struct PackSize<HEAD, TAIL...> {
    enum { VALUE = 1 + PackSize<TAIL...>::VALUE };
};

template <class TYPE>
struct PackSize<TYPE> {
    enum { VALUE = 1 };
};

// This is a second test to highlight why variadic templates do not work
// sufficiently well to be supported on the Sun CC 12.4 compiler (and any
// others that suffer similar bugs).

template <class TYPE>
void func(TYPE*, const TYPE&) {
    // This function should deduce 'TYPE' from the pointer, and bind to a
    // reference of any argument convertible to that type.
}

template <class TYPE, class ARG1, class... TAIL>
void func(TYPE *, const ARG1&, const TAIL&...) {
    // This function should deduce 'TYPE' from the pointer, perfectly match the
    // second argument.
}

void test_func() {
    int x = 0;
    func(&x, 0);       // This line will be ambiguous on buggy compilers
    (void)&test_func;  // Portably silence unused function warnings.
}
#endif  // BSLS_COMPILERFEATURES_SUPPORT_VARIADIC_TEMPLATES

#if defined(BSLS_COMPILERFEATURES_SUPPORT_CONSTEXPR)
// Note that the tests below also rely on the use of rvalue-references and the
// 'decltype' operator.  This has not been a problem on any tested platform
// that also supports at least the C++11 level of 'constexpr'.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_CONSTEXPR_CPP14)
constexpr int relaxedConstExprFunc(bool b)
    // Return a different integer value depending on the specified 'b' boolean
    // value.  This function demonstrates relaxed 'constexpr' requirements
    // since it has multiple statements, including mutating of a local variable
    // and multiple return statements.
{
    int i = -1;
    if (b) {
        i = 42;
        return i;                                                     // RETURN
    }
    else {
        i = 0;
        return i;                                                     // RETURN
    }
}
#endif // BSLS_COMPILERFEATURES_SUPPORT_CONSTEXPR_CPP14

#if defined(BSLS_COMPILERFEATURES_SUPPORT_CONSTEXPR_CPP17)
constexpr int moreRelaxedConstExprFunc(bool b)
    // Return the result of a invoking a locally defined lambda, with a lambda
    // that cannot be evaluated at compile time used on the false path.
{
    if (b) {
        return []{
                   return 42;
               }();                                                   // RETURN
    }
    else {
        return []{
                   static int i = 17;
                   return i;
               }();                                                   // RETURN
    }
}
#endif // BSLS_COMPILERFEATURES_SUPPORT_CONSTEXPR_CPP17

struct FalseType {
    // A type to represent 'false'.  Notice that its size is different from
    // that of 'TrueType'.

    // PUBLIC DATA
    char d_dummy;
};

struct TrueType {
    // A type to represent 'true'.  Notice that its size is different from that
    // of 'FalseType'.

    // PUBLIC DATA
    char d_dummy[13];
};

template <int VALUE>
struct TypeN {
    // This class template turns a compile time constant into a distinct type.
};

struct Sniffer {
    // This class provides a means to detect if the expression
    // 'TARGET().call(true)' is valid, and usable in SFINAE constraints.

    template <class TARGET>
    static FalseType test(...);
        // The "catch all" overload that gets selected when
        // 'TARGET{}.call(true)' is not a compile time constant.

    template <class TARGET>
    static TrueType test(TARGET *, TypeN < TARGET{}.call(true) > * = 0);
        // The overload that gets selected when 'TARGET{}.call(true)' is a
        // valid compile time constant ('constexpr').
};

struct ConstexprConst {
    // This class uses expression SFINAE to detect if the expression
    // 'const TARGET{}.call(true)' is a valid compile time constant expression.

    template<class TYPE>
    static TYPE&& declval();
        // Return an rvalue-reference to the (template parameter) 'TYPE'.
        // Note that this function is never defined, and should be called only
        // from unevaluated contexts, such as 'decltype' and 'sizeof'.

    template <class TARGET>
    static TrueType test(...);
        // The "catch all" overload that gets selected when
        // 'const TARGET{}.call(true)' is not a valid compile time constant
        // expression.  Notice that since in 'Feature14' the function 'call' is
        // not defined explicitly 'const', therefore this catch all function
        // returns 'TrueType', because in C++14 'constexpr' does not make
        // member functions implicitly 'const'.

    template <class TARGET,
              class = decltype(declval<const TARGET>().call(true))>
    static FalseType test(TARGET *);
        // The overload that gets selected when 'TARGET{}.call(true)' is a
        // valid expression.
};

struct Feature11 {
    // This literal type is for testing C++11 'constexpr' support.

    // CREATORS
    constexpr Feature11();
        // Create a, possibly 'constexpr', 'Feature11' object.

    // MANIPULATORS
    constexpr int call(bool) const;
        // Return an integer usable in constant expressions.
};

constexpr Feature11::Feature11() {}

constexpr int Feature11::call(bool) const {
    return 42;
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_CONSTEXPR_CPP14)
struct Feature14 {
    // This literal type is for testing C++14 'constexpr' support.

    // PUBLIC DATA
    int d_value;

    // CREATORS
    constexpr Feature14();
        // Create a, possibly 'constexpr', 'Feature14' object.

    constexpr int call(bool b);
        // Return, a possibly 'constexpr' integer value that depends on the
        // specified 'b' flag.  This method is "complex", cannot be 'constexpr'
        // in C++11, only in C++14 and onwards.
};

constexpr Feature14::Feature14() : d_value(-1) {}

constexpr int Feature14::call(bool b) {
    if (b) {
        d_value = 42;
        return d_value;                                               // RETURN
    }
    else {
        d_value = 21;
        return d_value;                                               // RETURN
    }
}
#endif // BSLS_COMPILERFEATURES_SUPPORT_CONSTEXPR_CPP14

#if defined(BSLS_COMPILERFEATURES_SUPPORT_CONSTEXPR_CPP17)
struct Feature17 {
    // This literal type is for testing C++17 'constexpr' support.

    // PUBLIC DATA
    int d_value;

    // CREATORS
    constexpr Feature17();
        // Create a, possibly 'constexpr', 'Feature17' object.

    constexpr int call(bool b);
        // Return, a possibly 'constexpr' integer value that depends on the
        // specified 'b' flag.  This method is "complex", cannot be 'constexpr'
        // in C++11, only in C++17 and onwards.
};

constexpr Feature17::Feature17() : d_value(-1) {}

constexpr int Feature17::call(bool b) {
    if (b) {
        d_value = []{
                      return 42;                                      // RETURN
                  }();
        return d_value;                                               // RETURN
    }
    else {
        d_value = []{
                      return 21;                                      // RETURN
                  }();
        return d_value;                                               // RETURN
    }
}
#endif // BSLS_COMPILERFEATURES_SUPPORT_CONSTEXPR_CPP17
#endif // BSLS_COMPILERFEATURES_SUPPORT_CONSTEXPR

}  // close unnamed namespace

#if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
namespace test_case_24 {

class MyType {
  public:
    static int s_liveCount;

  private:
    static void addToLiveCount() {
        if (s_liveCount > 5) {
#ifdef BDE_BUILD_TARGET_EXC
            throw s_liveCount;
#else
            ASSERT(!"Exceptions not supported");
#endif
        }
        else {
            ++s_liveCount;
        }
    }

  public:
    MyType() { addToLiveCount(); }
    MyType(int) { addToLiveCount(); }                               // IMPLICIT
    MyType(const MyType&) { addToLiveCount(); }                     // IMPLICIT
    ~MyType() { --s_liveCount; }
};

struct MyWrapper {
    MyWrapper(std::initializer_list<MyType>) {}                     // IMPLICIT
};

int MyType::s_liveCount = 0;

void runTest() {
    ASSERT(0 == MyType::s_liveCount);

    #if defined(BDE_BUILD_TARGET_EXC)

    try {
        const MyWrapper X = { 1, 2 };
        (void)X;
    }
    catch(...) {
        ASSERT(!"Strange error! No exceptions were expected.");
    }
    ASSERT(0 == MyType::s_liveCount);

    try {
        const MyWrapper X = { 1, 2, 3, 4, 5, 6, 7, 8, 9 };
        (void)X;
    }
    catch(int) {
#ifdef BSLS_COMPILERFEATURES_INITIALIZER_LIST_LEAKS_ON_EXCEPTIONS
            ASSERT(0 != MyType::s_liveCount);
#else
            ASSERT(0 == MyType::s_liveCount);
#endif
    }
    #endif  // BDE_BUILD_TARGET_EXC
}

}  // close namespace test_case_24
#endif  // BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS

namespace test_case_25 {

#ifdef BSLS_COMPILERFEATURES_SUPPORT_ATTRIBUTE_NORETURN

[[noreturn]] void runTest() {
    while (true) {}
}
#elif u_HAS_CPP_ATTRIBUTE(noreturn)
    // If specific compiler versions do have '__has_cpp_attribute' but do not
    // support '[[noreturn]]' properly then those should be excluded from the
    // check here.
    #error '[[noreturn]]' is available but                                    \
            'BSLS_COMPILERFEATURES_SUPPORT_ATTRIBUTE_NORETURN' is not defined.
#endif  // supported attribute not indicated
}  // close namespace test_case_25

namespace test_case_26 {
#ifdef BSLS_COMPILERFEATURES_SUPPORT_ATTRIBUTE_NODISCARD

[[nodiscard]] int runTest() {
    return 1;
}
#elif u_HAS_CPP_ATTRIBUTE(nodiscard)
    // If specific compiler versions do have '__has_cpp_attribute' but do not
    // support '[[nodiscard]]' properly then those should be excluded from the
    // check here.
    #error '[[nodiscard]]' is available but                                   \
           'BSLS_COMPILERFEATURES_SUPPORT_ATTRIBUTE_NODISCARD' is not defined.
#endif  // supported attribute not indicated
}  // close namespace test_case_26

namespace test_case_27 {
#ifdef BSLS_COMPILERFEATURES_SUPPORT_ATTRIBUTE_FALLTHROUGH

void runTest() {

    int i = 5;

    switch (i) {
      case 4: [[fallthrough]];
      case 7:
        return;                                                       // RETURN
      default:
        return;                                                       // RETURN
    }
}
#elif u_HAS_CPP_ATTRIBUTE(fallthrough)
    // if specific compiler versions do have '__has_cpp_attribute' but do not
    // support '[[fallthrough]]' properly then those should be excluded from
    // the check here.
    #error '[[fallthrough]]' is available but                                 \
         'BSLS_COMPILERFEATURES_SUPPORT_ATTRIBUTE_FALLTHROUGH' is not defined.
#endif  // supported attribute not indicated
}  // close namespace test_case_27

namespace test_case_28 {
#ifdef BSLS_COMPILERFEATURES_SUPPORT_ATTRIBUTE_MAYBE_UNUSED

void runTest([[maybe_unused]] int i) {}

#elif u_HAS_CPP_ATTRIBUTE(maybe_unused) && !defined(BSLS_PLATFORM_CMP_CLANG)
    // clang does not allow maybe_unused where other compilers do, so we
    // exclude it to keep portability.  If any other specific compiler version
    // does have '__has_cpp_attribute' but do not support '[[maybe_unused]]'
    // properly those should also be excluded from the check here.
        #error '[[maybe_unused]]' is available but                            \
        'BSLS_COMPILERFEATURES_SUPPORT_ATTRIBUTE_MAYBE_UNUSED' is not defined.
#endif  // supported attribute not indicated
}  // close namespace test_case_28

namespace test_case_31 {
#ifdef BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT

void foo()          {}
void bar() noexcept {}

#endif  // BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT
}  // close namespace test_case_31

namespace test_case_32 {
#ifdef BSLS_COMPILERFEATURES_GUARANTEED_COPY_ELISION

class NeverCopied {
    // Instances of this class can be constructed only using the factory
    // methods and must never be copied or moved.  This class is used to test
    // guaranteed copy elision.

    enum { NOT_COPIED = false };

    // PRIVATE DATA
    int d_data;

    // PRIVATE CREATORS
    explicit NeverCopied(int v) : d_data(v) { }

  public:
    // CLASS METHODS
    static NeverCopied factory1(int v);
        // Return a 'NeverCopied' object by value.  A compiler for which
        // 'BSLS_COMPILERFEATURES_GUARANTEED_COPY_ELISION' is defined will
        // create no copies in the process of returning the prvalue.

    static NeverCopied factory2(int v);
        // Return 'factory1(v)'.  A compiler for which
        // 'BSLS_COMPILERFEATURES_GUARANTEED_COPY_ELISION' is defined will
        // create no copies in the process of returning the prvalue.

    static const NeverCopied factory3(int v);
        // Return 'factory1(v)' as a const prvalue.  A compiler for which
        // 'BSLS_COMPILERFEATURES_GUARANTEED_COPY_ELISION' is defined will
        // create no copies in the process of returning the prvalue.

    // CREATORS
    NeverCopied(const NeverCopied&, int *allocator = 0)
        // "Extended" copy constructor (which is also the move constructor)
        // that is never called.  Note that some compilers (e.g., xlC) will
        // elide some calls to a regular copy constructor, but not to an
        // extended copy constructor like this one.
        : d_data(allocator ? *allocator : 0) { ASSERT(NOT_COPIED); }

    // MANIPULATORS
    NeverCopied& operator=(const NeverCopied&)
        // Copy assignment operator (which is also the move assignment
        // operator) that is never called.
        { ASSERT(NOT_COPIED); return *this; }

    // ACCESSORS
    int value() const { return d_data; }
};

NeverCopied NeverCopied::factory1(int v) {
    // RVO and copy elision prevent a copy from being made.
    return NeverCopied(v);
}

NeverCopied NeverCopied::factory2(int v) {
    // RVO and copy elision prevent a copy from being made.
    return factory1(v);
}

const NeverCopied NeverCopied::factory3(int v) {
    // RVO and copy elision prevent a copy from being made.
    return factory1(v);
}

class NCWrapper {
    // Instances of this class wrap a 'NeverCopied' object.  The constructors
    // use the 'NeverCopied' factory methods and ensure that initializing the
    // wrapped value is done without making a copy.

    NeverCopied d_data;

  public:
    explicit NCWrapper(int v)
        // Create an 'NCWrapper' by initializing its data member from
        // 'NeverCopied::factory1()'.
        : d_data(NeverCopied::factory1(v)) { }

    NCWrapper(int v, int)
        // Create an 'NCWrapper' by initializing its data member from
        // 'NeverCopied::factory2()'.
        : d_data(NeverCopied::factory2(v)) { }

    NCWrapper(int v, int, int)
        // Create an 'NCWrapper' by initializing its data member from
        // 'NeverCopied::factory3()'.
        : d_data(NeverCopied::factory3(v)) { }

    int value() const { return d_data.value(); }
};

#endif
}  // close namespace test_case_31

namespace test_case_34 {
#ifdef BSLS_COMPILERFEATURES_SUPPORT_CTAD

struct Empty {};
   // An empty class used to test template argument deduction.

template <class T>
struct Holder {
    // A class to hold a single value.  Used to test template argument
    // deduction.

    // CREATORS
    explicit Holder(const T &value) : d_value(value) {}
        // Create a 'Holder' by initializing its data member from the specified
        // 'value'.

    // PUBLIC DATA
    T d_value;
};

// CLASS TEMPLATE ARGUMENT DEDUCTION GUIDE
template<class T>
Holder(const T &) -> Holder<T>;

#endif
}  // close namespace test_case_34

// ============================================================================
//                              HELPER FUNCTIONS
// ----------------------------------------------------------------------------

static void printFlags()
    // Print a diagnostic message to standard output if any of the preprocessor
    // flags of interest are defined, and their value if a value had been set.
    // An "Enter" and "Leave" message is printed unconditionally so there is
    // some report even if all of the flags are undefined.
{
    puts("printFlags: Enter");

    puts("\n==printFlags: bsls_compilerfeatures Macros==");

    fputs("\n  BSLS_COMPILERFEATURES_FILLT(n): ", stdout);
#ifdef BSLS_COMPILERFEATURES_FILLT
    puts(STRINGIFY(BSLS_COMPILERFEATURES_FILLT(n)));
#else
    puts("UNDEFINED");
#endif

    fputs("\n  BSLS_COMPILERFEATURES_FILLV(n): ", stdout);
#ifdef BSLS_COMPILERFEATURES_FILLV
    puts(STRINGIFY(BSLS_COMPILERFEATURES_FILLV(n)));
#else
    puts("UNDEFINED");
#endif

    fputs("\n  BSLS_COMPILERFEATURES_FORWARD(T,V): ", stdout);
#ifdef BSLS_COMPILERFEATURES_FORWARD
    puts(STRINGIFY(BSLS_COMPILERFEATURES_FORWARD(T,V)));
#else
    puts("UNDEFINED");
#endif

    fputs("\n  BSLS_COMPILERFEATURES_FORWARD_REF(T): ", stdout);
#ifdef BSLS_COMPILERFEATURES_FORWARD_REF
    puts(STRINGIFY(BSLS_COMPILERFEATURES_FORWARD_REF(T)));
#else
    puts("UNDEFINED");
#endif

    fputs("\n  BSLS_COMPILERFEATURES_INITIALIZER_LIST_LEAKS_ON_EXCEPTIONS: ",
          stdout);
#ifdef BSLS_COMPILERFEATURES_INITIALIZER_LIST_LEAKS_ON_EXCEPTIONS
    puts(STRINGIFY(
                  BSLS_COMPILERFEATURES_INITIALIZER_LIST_LEAKS_ON_EXCEPTIONS));
#else
    puts("UNDEFINED");
#endif

    fputs("\n  BSLS_COMPILERFEATURES_NILT: ", stdout);
#ifdef BSLS_COMPILERFEATURES_NILT
    puts(STRINGIFY(BSLS_COMPILERFEATURES_NILT));
#else
    puts("UNDEFINED");
#endif

    fputs("\n  BSLS_COMPILERFEATURES_NILTR(n): ", stdout);
#ifdef BSLS_COMPILERFEATURES_NILTR
    puts(STRINGIFY(BSLS_COMPILERFEATURES_NILTR(n)));
#else
    puts("UNDEFINED");
#endif

    fputs("\n  BSLS_COMPILERFEATURES_NILV: ", stdout);
#ifdef BSLS_COMPILERFEATURES_NILV
    puts(STRINGIFY(BSLS_COMPILERFEATURES_NILV));
#else
    puts("UNDEFINED");
#endif

    fputs("\n  BSLS_COMPILERFEATURES_NILVR(n): ", stdout);
#ifdef BSLS_COMPILERFEATURES_NILVR
    puts(STRINGIFY(BSLS_COMPILERFEATURES_NILVR(n)));
#else
    puts("UNDEFINED");
#endif

    fputs("\n  BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES: ", stdout);
#ifdef BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
    puts(STRINGIFY(BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES));
#else
    puts("UNDEFINED");
#endif

    fputs("\n  BSLS_COMPILERFEATURES_SIMULATE_FORWARD_WORKAROUND: ", stdout);
#ifdef BSLS_COMPILERFEATURES_SIMULATE_FORWARD_WORKAROUND
    puts(STRINGIFY(BSLS_COMPILERFEATURES_SIMULATE_FORWARD_WORKAROUND));
#else
    puts("UNDEFINED");
#endif

    fputs("\n  BSLS_COMPILERFEATURES_SIMULATE_VARIADIC_TEMPLATES: ", stdout);
#ifdef BSLS_COMPILERFEATURES_SIMULATE_VARIADIC_TEMPLATES
    puts(STRINGIFY(BSLS_COMPILERFEATURES_SIMULATE_VARIADIC_TEMPLATES));
#else
    puts("UNDEFINED");
#endif

    fputs("\n  BSLS_COMPILERFEATURES_SUPPORT_ALIAS_TEMPLATES: ", stdout);
#ifdef BSLS_COMPILERFEATURES_SUPPORT_ALIAS_TEMPLATES
    puts(STRINGIFY(BSLS_COMPILERFEATURES_SUPPORT_ALIAS_TEMPLATES));
#else
    puts("UNDEFINED");
#endif

    fputs("\n  BSLS_COMPILERFEATURES_SUPPORT_ALIGNAS: ", stdout);
#ifdef BSLS_COMPILERFEATURES_SUPPORT_ALIGNAS
    puts(STRINGIFY(BSLS_COMPILERFEATURES_SUPPORT_ALIGNAS));
#else
    puts("UNDEFINED");
#endif

    fputs("\n  BSLS_COMPILERFEATURES_SUPPORT_ATTRIBUTE_FALLTHROUGH: ", stdout);
#ifdef BSLS_COMPILERFEATURES_SUPPORT_ATTRIBUTE_FALLTHROUGH
    puts(STRINGIFY(BSLS_COMPILERFEATURES_SUPPORT_ATTRIBUTE_FALLTHROUGH));
#else
    puts("UNDEFINED");
#endif

    fputs("\n  BSLS_COMPILERFEATURES_SUPPORT_ATTRIBUTE_MAYBE_UNUSED: ",
          stdout);
#ifdef BSLS_COMPILERFEATURES_SUPPORT_ATTRIBUTE_MAYBE_UNUSED
    puts(STRINGIFY(BSLS_COMPILERFEATURES_SUPPORT_ATTRIBUTE_MAYBE_UNUSED));
#else
    puts("UNDEFINED");
#endif

    fputs("\n  BSLS_COMPILERFEATURES_SUPPORT_ATTRIBUTE_NODISCARD: ", stdout);
#ifdef BSLS_COMPILERFEATURES_SUPPORT_ATTRIBUTE_NODISCARD
    puts(STRINGIFY(BSLS_COMPILERFEATURES_SUPPORT_ATTRIBUTE_NODISCARD));
#else
    puts("UNDEFINED");
#endif

    fputs("\n  BSLS_COMPILERFEATURES_SUPPORT_ATTRIBUTE_NORETURN: ", stdout);
#ifdef BSLS_COMPILERFEATURES_SUPPORT_ATTRIBUTE_NORETURN
    puts(STRINGIFY(BSLS_COMPILERFEATURES_SUPPORT_ATTRIBUTE_NORETURN));
#else
    puts("UNDEFINED");
#endif

    fputs("\n  BSLS_COMPILERFEATURES_SUPPORT_CONSTEXPR: ", stdout);
#ifdef BSLS_COMPILERFEATURES_SUPPORT_CONSTEXPR
    puts(STRINGIFY(BSLS_COMPILERFEATURES_SUPPORT_CONSTEXPR));
#else
    puts("UNDEFINED");
#endif

    fputs("\n  BSLS_COMPILERFEATURES_SUPPORT_CONSTEXPR_CPP14: ", stdout);
#ifdef BSLS_COMPILERFEATURES_SUPPORT_CONSTEXPR_CPP14
    puts(STRINGIFY(BSLS_COMPILERFEATURES_SUPPORT_CONSTEXPR_CPP14));
#else
    puts("UNDEFINED");
#endif

    fputs("\n  BSLS_COMPILERFEATURES_SUPPORT_CONSTEXPR_CPP17: ", stdout);
#ifdef BSLS_COMPILERFEATURES_SUPPORT_CONSTEXPR_CPP17
    puts(STRINGIFY(BSLS_COMPILERFEATURES_SUPPORT_CONSTEXPR_CPP17));
#else
    puts("UNDEFINED");
#endif

    fputs("\n  BSLS_COMPILERFEATURES_SUPPORT_DECLTYPE: ", stdout);
#ifdef BSLS_COMPILERFEATURES_SUPPORT_DECLTYPE
    puts(STRINGIFY(BSLS_COMPILERFEATURES_SUPPORT_DECLTYPE));
#else
    puts("UNDEFINED");
#endif

    fputs("\n  BSLS_COMPILERFEATURES_SUPPORT_DEFAULTED_FUNCTIONS: ", stdout);
#ifdef BSLS_COMPILERFEATURES_SUPPORT_DEFAULTED_FUNCTIONS
    puts(STRINGIFY(BSLS_COMPILERFEATURES_SUPPORT_DEFAULTED_FUNCTIONS));
#else
    puts("UNDEFINED");
#endif

    fputs("\n  BSLS_COMPILERFEATURES_SUPPORT_DEFAULT_TEMPLATE_ARGS: ", stdout);
#ifdef BSLS_COMPILERFEATURES_SUPPORT_DEFAULT_TEMPLATE_ARGS
    puts(STRINGIFY(BSLS_COMPILERFEATURES_SUPPORT_DEFAULT_TEMPLATE_ARGS));
#else
    puts("UNDEFINED");
#endif

    fputs("\n  BSLS_COMPILERFEATURES_SUPPORT_DELETED_FUNCTIONS: ", stdout);
#ifdef BSLS_COMPILERFEATURES_SUPPORT_DELETED_FUNCTIONS
    puts(STRINGIFY(BSLS_COMPILERFEATURES_SUPPORT_DELETED_FUNCTIONS));
#else
    puts("UNDEFINED");
#endif

    fputs("\n  BSLS_COMPILERFEATURES_SUPPORT_ENUM_CLASS: ", stdout);
#ifdef BSLS_COMPILERFEATURES_SUPPORT_ENUM_CLASS
    puts(STRINGIFY(BSLS_COMPILERFEATURES_SUPPORT_ENUM_CLASS));
#else
    puts("UNDEFINED");
#endif

    fputs("\n  BSLS_COMPILERFEATURES_SUPPORT_EXTERN_TEMPLATE: ", stdout);
#ifdef BSLS_COMPILERFEATURES_SUPPORT_EXTERN_TEMPLATE
    puts(STRINGIFY(BSLS_COMPILERFEATURES_SUPPORT_EXTERN_TEMPLATE));
#else
    puts("UNDEFINED");
#endif

    fputs("\n  BSLS_COMPILERFEATURES_SUPPORT_FINAL: ", stdout);
#ifdef BSLS_COMPILERFEATURES_SUPPORT_FINAL
    puts(STRINGIFY(BSLS_COMPILERFEATURES_SUPPORT_FINAL));
#else
    puts("UNDEFINED");
#endif

    fputs("\n  BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS: ",
          stdout);
#ifdef BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS
    puts(STRINGIFY(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS));
#else
    puts("UNDEFINED");
#endif

    fputs("\n  BSLS_COMPILERFEATURES_SUPPORT_HAS_INCLUDE: ", stdout);
#ifdef BSLS_COMPILERFEATURES_SUPPORT_HAS_INCLUDE
    puts(STRINGIFY(BSLS_COMPILERFEATURES_SUPPORT_HAS_INCLUDE));
#else
    puts("UNDEFINED");
#endif

    fputs("\n  BSLS_COMPILERFEATURES_SUPPORT_HEXFLOAT_LITERALS: ", stdout);
#ifdef BSLS_COMPILERFEATURES_SUPPORT_HEXFLOAT_LITERALS
    puts(STRINGIFY(BSLS_COMPILERFEATURES_SUPPORT_HEXFLOAT_LITERALS));
#else
    puts("UNDEFINED");
#endif

    fputs("\n  BSLS_COMPILERFEATURES_SUPPORT_INCLUDE_NEXT: ", stdout);
#ifdef BSLS_COMPILERFEATURES_SUPPORT_INCLUDE_NEXT
    puts(STRINGIFY(BSLS_COMPILERFEATURES_SUPPORT_INCLUDE_NEXT));
#else
    puts("UNDEFINED");
#endif

    fputs("\n  BSLS_COMPILERFEATURES_SUPPORT_INLINE_NAMESPACE: ", stdout);
#ifdef BSLS_COMPILERFEATURES_SUPPORT_INLINE_NAMESPACE
    puts(STRINGIFY(BSLS_COMPILERFEATURES_SUPPORT_INLINE_NAMESPACE));
#else
    puts("UNDEFINED");
#endif

    fputs("\n  BSLS_COMPILERFEATURES_SUPPORT_INLINE_VARIABLES: ", stdout);
#ifdef BSLS_COMPILERFEATURES_SUPPORT_INLINE_VARIABLES
    puts(STRINGIFY(BSLS_COMPILERFEATURES_SUPPORT_INLINE_VARIABLES));
#else
    puts("UNDEFINED");
#endif

    fputs("\n  BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT: ", stdout);
#ifdef BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT
    puts(STRINGIFY(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT));
#else
    puts("UNDEFINED");
#endif

    fputs("\n  BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT_IN_FNC_TYPE: ", stdout);
#ifdef BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT_IN_FNC_TYPE
    puts(STRINGIFY(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT_IN_FNC_TYPE));
#else
    puts("UNDEFINED");
#endif

    fputs("\n  **deprecated** alias:", stdout);
    fputs("\n    BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT_TYPES: ", stdout);
#ifdef BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT_TYPES
    puts(STRINGIFY(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT_TYPES));
#else
    puts("UNDEFINED");
#endif

    fputs("\n  BSLS_COMPILERFEATURES_SUPPORT_NULLPTR: ", stdout);
#ifdef BSLS_COMPILERFEATURES_SUPPORT_NULLPTR
    puts(STRINGIFY(BSLS_COMPILERFEATURES_SUPPORT_NULLPTR));
#else
    puts("UNDEFINED");
#endif

    fputs("\n  BSLS_COMPILERFEATURES_SUPPORT_OPERATOR_EXPLICIT: ", stdout);
#ifdef BSLS_COMPILERFEATURES_SUPPORT_OPERATOR_EXPLICIT
    puts(STRINGIFY(BSLS_COMPILERFEATURES_SUPPORT_OPERATOR_EXPLICIT));
#else
    puts("UNDEFINED");
#endif

    fputs("\n  BSLS_COMPILERFEATURES_SUPPORT_OVERRIDE: ", stdout);
#ifdef BSLS_COMPILERFEATURES_SUPPORT_OVERRIDE
    puts(STRINGIFY(BSLS_COMPILERFEATURES_SUPPORT_OVERRIDE));
#else
    puts("UNDEFINED");
#endif

    fputs("\n  BSLS_COMPILERFEATURES_SUPPORT_RAW_STRINGS: ", stdout);
#ifdef BSLS_COMPILERFEATURES_SUPPORT_RAW_STRINGS
    puts(STRINGIFY(BSLS_COMPILERFEATURES_SUPPORT_RAW_STRINGS));
#else
    puts("UNDEFINED");
#endif

    fputs("\n  BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS: ", stdout);
#ifdef BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS
    puts(STRINGIFY(BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS));
#else
    puts("UNDEFINED");
#endif

    fputs("\n  BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES: ", stdout);
#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
    puts(STRINGIFY(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES));
#else
    puts("UNDEFINED");
#endif

    fputs("\n  BSLS_COMPILERFEATURES_SUPPORT_STATIC_ASSERT: ", stdout);
#ifdef BSLS_COMPILERFEATURES_SUPPORT_STATIC_ASSERT
    puts(STRINGIFY(BSLS_COMPILERFEATURES_SUPPORT_STATIC_ASSERT));
#else
    puts("UNDEFINED");
#endif

    fputs("\n  BSLS_COMPILERFEATURES_SUPPORT_THROW_SPECIFICATIONS: ", stdout);
#ifdef BSLS_COMPILERFEATURES_SUPPORT_THROW_SPECIFICATIONS
    puts(STRINGIFY(BSLS_COMPILERFEATURES_SUPPORT_THROW_SPECIFICATIONS));
#else
    puts("UNDEFINED");
#endif

    fputs("\n  BSLS_COMPILERFEATURES_SUPPORT_TRAITS_HEADER: ", stdout);
#ifdef BSLS_COMPILERFEATURES_SUPPORT_TRAITS_HEADER
    puts(STRINGIFY(BSLS_COMPILERFEATURES_SUPPORT_TRAITS_HEADER));
#else
    puts("UNDEFINED");
#endif

    fputs("\n  BSLS_COMPILERFEATURES_SUPPORT_UNICODE_CHAR_TYPES: ", stdout);
#ifdef BSLS_COMPILERFEATURES_SUPPORT_UNICODE_CHAR_TYPES
    puts(STRINGIFY(BSLS_COMPILERFEATURES_SUPPORT_UNICODE_CHAR_TYPES));
#else
    puts("UNDEFINED");
#endif

    fputs("\n  BSLS_COMPILERFEATURES_SUPPORT_VARIABLE_TEMPLATES: ", stdout);
#ifdef BSLS_COMPILERFEATURES_SUPPORT_VARIABLE_TEMPLATES
    puts(STRINGIFY(BSLS_COMPILERFEATURES_SUPPORT_VARIABLE_TEMPLATES));
#else
    puts("UNDEFINED");
#endif

    fputs("\n  BSLS_COMPILERFEATURES_SUPPORT_VARIADIC_TEMPLATES: ", stdout);
#ifdef BSLS_COMPILERFEATURES_SUPPORT_VARIADIC_TEMPLATES
    puts(STRINGIFY(BSLS_COMPILERFEATURES_SUPPORT_VARIADIC_TEMPLATES));
#else
    puts("UNDEFINED");
#endif

    fputs("\n  BSLS_COMPILERFEATURES_SUPPORT_CTAD: ", stdout);
#ifdef BSLS_COMPILERFEATURES_SUPPORT_CTAD
    puts(STRINGIFY(BSLS_COMPILERFEATURES_SUPPORT_CTAD));
#else
    puts("UNDEFINED");
#endif

    puts("\n\n==printFlags: bsls_compilerfeatures Referenced Macros==");

    fputs("\n  BSLS_COMPILERFEATURES_SIMULATE_FORWARD_WORKAROUND: ", stdout);
#ifdef BSLS_COMPILERFEATURES_SIMULATE_FORWARD_WORKAROUND
    puts(STRINGIFY(BSLS_COMPILERFEATURES_SIMULATE_FORWARD_WORKAROUND));
#else
    puts("UNDEFINED");
#endif

    fputs("\n  BSLS_COMPILERFEATURES_SUPPORT_ALIAS_TEMPLATES: ", stdout);
#ifdef BSLS_COMPILERFEATURES_SUPPORT_ALIAS_TEMPLATES
    puts(STRINGIFY(BSLS_COMPILERFEATURES_SUPPORT_ALIAS_TEMPLATES));
#else
    puts("UNDEFINED");
#endif

    fputs("\n  BSLS_COMPILERFEATURES_SUPPORT_EXTERN_TEMPLATE: ", stdout);
#ifdef BSLS_COMPILERFEATURES_SUPPORT_EXTERN_TEMPLATE
    puts(STRINGIFY(BSLS_COMPILERFEATURES_SUPPORT_EXTERN_TEMPLATE));
#else
    puts("UNDEFINED");
#endif

    fputs("\n  BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES: ", stdout);
#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
    puts(STRINGIFY(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES));
#else
    puts("UNDEFINED");
#endif

    fputs("\n  BSLS_PLATFORM_CMP_CLANG: ", stdout);
#ifdef BSLS_PLATFORM_CMP_CLANG
    puts(STRINGIFY(BSLS_PLATFORM_CMP_CLANG));
#else
    puts("UNDEFINED");
#endif

    fputs("\n  BSLS_PLATFORM_CMP_GNU: ", stdout);
#ifdef BSLS_PLATFORM_CMP_GNU
    puts(STRINGIFY(BSLS_PLATFORM_CMP_GNU));
#else
    puts("UNDEFINED");
#endif

    fputs("\n  BSLS_PLATFORM_CMP_IBM: ", stdout);
#ifdef BSLS_PLATFORM_CMP_IBM
    puts(STRINGIFY(BSLS_PLATFORM_CMP_IBM));
#else
    puts("UNDEFINED");
#endif

    fputs("\n  BSLS_PLATFORM_CMP_MSVC: ", stdout);
#ifdef BSLS_PLATFORM_CMP_MSVC
    puts(STRINGIFY(BSLS_PLATFORM_CMP_MSVC));
#else
    puts("UNDEFINED");
#endif

    fputs("\n  BSLS_PLATFORM_CMP_SUN: ", stdout);
#ifdef BSLS_PLATFORM_CMP_SUN
    puts(STRINGIFY(BSLS_PLATFORM_CMP_SUN));
#else
    puts("UNDEFINED");
#endif

    fputs("\n  BSLS_PLATFORM_CMP_VERSION: ", stdout);
#ifdef BSLS_PLATFORM_CMP_VERSION
    puts(STRINGIFY(BSLS_PLATFORM_CMP_VERSION));
#else
    puts("UNDEFINED");
#endif

    fputs("\n  __APPLE_CC__: ", stdout);
#ifdef __APPLE_CC__
    puts(STRINGIFY(__APPLE_CC__));
#else
    puts("UNDEFINED");
#endif

    fputs("\n  __GXX_EXPERIMENTAL_CXX0X__: ", stdout);
#ifdef __GXX_EXPERIMENTAL_CXX0X__
    puts(STRINGIFY(__GXX_EXPERIMENTAL_CXX0X__));
#else
    puts("UNDEFINED");
#endif

    fputs("\n  __IBMCPP_CONSTEXPR: ", stdout);
#ifdef __IBMCPP_CONSTEXPR
    puts(STRINGIFY(__IBMCPP_CONSTEXPR));
#else
    puts("UNDEFINED");
#endif

    fputs("\n  __IBMCPP_DECLTYPE: ", stdout);
#ifdef __IBMCPP_DECLTYPE
    puts(STRINGIFY(__IBMCPP_DECLTYPE));
#else
    puts("UNDEFINED");
#endif

    fputs("\n  __IBMCPP_DEFAULTED_AND_DELETED_FUNCTIONS: ", stdout);
#ifdef __IBMCPP_DEFAULTED_AND_DELETED_FUNCTIONS
    puts(STRINGIFY(__IBMCPP_DEFAULTED_AND_DELETED_FUNCTIONS));
#else
    puts("UNDEFINED");
#endif

    fputs("\n  __IBMCPP_EXPLICIT: ", stdout);
#ifdef __IBMCPP_EXPLICIT
    puts(STRINGIFY(__IBMCPP_EXPLICIT));
#else
    puts("UNDEFINED");
#endif

    fputs("\n  __IBMCPP_EXTERN_TEMPLATE: ", stdout);
#ifdef __IBMCPP_EXTERN_TEMPLATE
    puts(STRINGIFY(__IBMCPP_EXTERN_TEMPLATE));
#else
    puts("UNDEFINED");
#endif

    fputs("\n  __IBMCPP_INLINE_NAMESPACE: ", stdout);
#ifdef __IBMCPP_INLINE_NAMESPACE
    puts(STRINGIFY(__IBMCPP_INLINE_NAMESPACE));
#else
    puts("UNDEFINED");
#endif

    fputs("\n  __IBMCPP_NULLPTR: ", stdout);
#ifdef __IBMCPP_NULLPTR
    puts(STRINGIFY(__IBMCPP_NULLPTR));
#else
    puts("UNDEFINED");
#endif

    fputs("\n  __IBMCPP_OVERRIDE: ", stdout);
#ifdef __IBMCPP_OVERRIDE
    puts(STRINGIFY(__IBMCPP_OVERRIDE));
#else
    puts("UNDEFINED");
#endif

    fputs("\n  __IBMCPP_RVALUE_REFERENCES: ", stdout);
#ifdef __IBMCPP_RVALUE_REFERENCES
    puts(STRINGIFY(__IBMCPP_RVALUE_REFERENCES));
#else
    puts("UNDEFINED");
#endif

    fputs("\n  __IBMCPP_STATIC_ASSERT: ", stdout);
#ifdef __IBMCPP_STATIC_ASSERT
    puts(STRINGIFY(__IBMCPP_STATIC_ASSERT));
#else
    puts("UNDEFINED");
#endif

    fputs("\n  __IBMCPP_VARIADIC_TEMPLATES: ", stdout);
#ifdef __IBMCPP_VARIADIC_TEMPLATES
    puts(STRINGIFY(__IBMCPP_VARIADIC_TEMPLATES));
#else
    puts("UNDEFINED");
#endif

    fputs("\n  __IBMC_NORETURN: ", stdout);
#ifdef __IBMC_NORETURN
    puts(STRINGIFY(__IBMC_NORETURN));
#else
    puts("UNDEFINED");
#endif

    fputs("\n  __IBM_INCLUDE_NEXT: ", stdout);
#ifdef __IBM_INCLUDE_NEXT
    puts(STRINGIFY(__IBM_INCLUDE_NEXT));
#else
    puts("UNDEFINED");
#endif

    fputs("\n  __C99_HEX_FLOAT_CONST: ", stdout);
#ifdef __C99_HEX_FLOAT_CONST
    puts(STRINGIFY(__C99_HEX_FLOAT_CONST));
#else
    puts("UNDEFINED");
#endif

    fputs("\n  __STRICT_ANSI__: ", stdout);
#ifdef __STRICT_ANSI__
    puts(STRINGIFY(__STRICT_ANSI__));
#else
    puts("UNDEFINED");
#endif

    fputs("\n  __cplusplus: ", stdout);
#ifdef __cplusplus
    puts(STRINGIFY(__cplusplus));
#else
    puts("UNDEFINED");
#endif

    fputs("\n  __cpp_hex_float: ", stdout);
#ifdef __cpp_hex_float
    puts(STRINGIFY(__cpp_hex_float));
#else
    puts("UNDEFINED");
#endif

    fputs("\n  __cpp_unicode_characters: ", stdout);
#ifdef __cpp_unicode_characters
    puts(STRINGIFY(__cpp_unicode_characters));
#else
    puts("UNDEFINED");
#endif

    fputs("\n  __cpp_unicode_literals: ", stdout);
#ifdef __cpp_unicode_literals
    puts(STRINGIFY(__cpp_unicode_literals));
#else
    puts("UNDEFINED");
#endif

    fputs("\n  __has_cpp_attribute: ", stdout);
#ifdef __has_cpp_attribute
    puts("DEFINED");
#else
    puts("UNDEFINED");
#endif

    puts("\n\nprintFlags: Leave\n");
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

    (void)       veryVerbose;  // unused variable warning
    (void)   veryVeryVerbose;  // unused variable warning
    (void)veryVeryVeryVerbose;  // unused variable warning

    setbuf(stdout, NULL);    // Use unbuffered output

    printf("TEST " __FILE__ " CASE %d\n", test);

    if (veryVeryVerbose) {
        printFlags();
    }

    switch (test) { case 0:
      case 37: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //
        // Concerns:
        //: 1 The usage example provided in the component header file must
        //:   compile, link, and run on all platforms as shown.
        //
        // Plan:
        //: 1 Incorporate usage example from header into driver, remove leading
        //:   comment characters, and replace 'assert' with 'ASSERT'.  (C-1)
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) puts("\nUSAGE EXAMPLE"
                          "\n=============");

///Example 2: '__LINE__' macro multi-line value differences demonstration
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Note that this isn't an example of use, it is a demonstration of compiler
// preprocessor behavior and the 'BSLS_COMPILERFEATURES_PP_LINE_IS_ON_FIRST'
// macro.
//
// Sometimes we write code that uses line numbers for logging or other
// purposes.  Although most of the time the precise values of those line
// numbers (in program output, such as assertions, or logs) is unimportant
// (the output is read by humans who are good at finding the line that
// actually emitted the text), sometimes programs read other programs' output.
// In such cases the precise values for the line numbers may matter.  This
// example demonstrates the two ways our currently supported C++ compilers
// generate line numbers in multi-line macro expansion contexts (from the
// '__LINE__' macro), and how the presence (or absence) of the macro
// 'BSLS_COMPILERFEATURES_PP_LINE_IS_ON_FIRST' indicates which method the
// current compiler uses.  First, we define a macro that uses '__LINE__' in its
// replacement text:
//..
      #define THATS_MY_LINE(dummy) __LINE__
//..
// Note that this macro has the function-like syntax so we can easily span its
// invocation to multiple lines.
//
// Next, we record the current line number in a constant, and also record the
// line number from our macro, but we span the macro invocation multiple lines
// to invoke the unspecified behavior.
//
// The two lines must follow each other due to working with line numbering:
//..
      const long A_LINE = __LINE__;
      const long LINE_FROM_MACRO = THATS_MY_LINE
          (
               "dummy"
          )
          ;
//..
// We deliberately extended the macro invocation to more than 2 physical source
// code lines so that we can demonstrate the two distinct behaviors: using the
// line number of the first character or the last.  Extending the number of
// lines *beyond* the macro invocation (by placing the semicolon on its own
// line) has no effect on the line number substitution inside the macro.  The
// dummy argument is required for C++03 compatibility.
//
// If we follow the definition of 'A_LINE' without any intervening empty lines
// the line number of the first character of the macro invocation will be
// 'A_LINE + 1', while the last falls on line 'A_LINE + 4'.
//
// Now we demonstrate the two different behaviors and how the presence of
// 'BSLS_COMPILERFEATURES_PP_LINE_IS_ON_FIRST' indicates which one will occur:
//..
      #ifdef BSLS_COMPILERFEATURES_PP_LINE_IS_ON_FIRST
          ASSERT(A_LINE + 1 == LINE_FROM_MACRO);
      #else
          ASSERT(A_LINE + 4 == LINE_FROM_MACRO);
      #endif
//..
// Finally note that WG14 N2322 defines this behavior as *unspecified*,
// therefore it is in the realm of possibilities, although not likely (in C++
// compilers) that further, more complicated or even indeterminate behaviors
// may arise.
#undef THATS_MY_LINE
      } break;
      case 36: {
        // --------------------------------------------------------------------
        // TESTING 'BSLS_COMPILERFEATURES_SUPPORT_THREE_WAY_COMPARISON'
        //
        // Concerns:
        //: 1 'BSLS_COMPILERFEATURES_SUPPORT_THREE_WAY_COMPARISON' is defined
        //:   when '<=>' operator is fully supported, including the library
        //:   support.
        //:
        //: 2 'BSLS_COMPILERFEATURES_SUPPORT_THREE_WAY_COMPARISON' is defined
        //:   when '__cpp_impl_three_way_comparison' and
        //:   '__cpp_lib_three_way_comparison' are both defined and have values
        //:   as defined by the ISO C++20 or greater.
        //
        // Plan:
        //: 1 Verify that both '__cpp_*' macros are defined and have a value at
        //:   least '201907L' when the macro is defined.
        //:
        //: 2 Verify that '<=>' operator can be used when the macro is defined.
        //
        // Testing:
        //   BSLS_COMPILERFEATURES_SUPPORT_THREE_WAY_COMPARISON
        // --------------------------------------------------------------------
        MACRO_TEST_TITLE("_SUPPORT_THREE_WAY_COMPARISON",
                         "=============================");

#ifdef BSLS_COMPILERFEATURES_SUPPORT_THREE_WAY_COMPARISON
            ASSERTV(__cpp_impl_three_way_comparison,
                    __cpp_impl_three_way_comparison >= 201907L);
            // TODO: Add tests to ensure compliance of the compiler support for
            // the three way operator, per DRQS 171563596

            // Sanity tests
            ASSERT(0 <=> 1 <  0);
            ASSERT(1 <=> 1 == 0);
            ASSERT(1 <=> 0 >  0);

            // Compile time sanity tests
            static_assert(0 <=> 1 <  0);
            static_assert(1 <=> 1 == 0);
            static_assert(1 <=> 0 >  0);
#else
        if (verbose) printf("'<=>' is not supported in this configuration\n");
#endif
      } break;
      case 35: {
        // --------------------------------------------------------------------
        // BSLS_COMPILERFEATURES_SUPPORT_HEXFLOAT_LITERALS
        //
        // Concerns:
        //: 1 When 'BSLS_COMPILERFEATURES_SUPPORT_HEXFLOAT_LITERALS' is defined
        //:   hexadecimal floating point constants are supported.
        //
        // Plan:
        //: 1 When the feature macro is defined verify hexfloat literal support
        //:   by comparing 'float', 'double', and 'long double' hexfloat
        //:   literals to their decimal value, and verifying their type, too.
        //
        // Testing:
        //   BSLS_COMPILERFEATURES_SUPPORT_HEXFLOAT_LITERALS
        // --------------------------------------------------------------------

        MACRO_TEST_TITLE("_SUPPORT_HEXFLOAT_LITERALS",
                         "==========================");

#ifndef BSLS_COMPILERFEATURES_SUPPORT_HEXFLOAT_LITERALS
        VERBOSE_PUTS("The feature is not supported in this configuration.");
#else
        // 'float'
        ASSERTV(0x1.5p5f, 42.0f, 0x1.5p5f == 42.0f);
        ASSERT(isSameType(0x1.5p5f, 42.0f));

        // 'double'
        typedef std::numeric_limits<double> DblLims;
        ASSERTV(0x0.0000000000001p-1022,   DblLims::denorm_min(),
                0x0.0000000000001p-1022 == DblLims::denorm_min());
        ASSERT(isSameType(0x0.0000000000001p-1022, DblLims::denorm_min()));
        // 'double' has exacts tests as much of BDE code depends on it being
        // IEEE 754 binary64.

#ifdef BSLS_PLATFORM_CMP_AIX
        // AIX xlC 16.1.0 (BSLS_PLATFORM_CMP_VER == 0x1001) mistakenly compiles
        // literals less than '0x0.8000000000000p-1022' into zero.  These
        // additional tests to kick in when the compiler version is raised, and
        // roughly verify if the same issue exists (or some new one).

        // We avoid using decimal floating point literals that may also be
        // interpreted differently by manipulating the smallest denormal value
        // '0x0.0000000000001p-1022'.  The 'k_M51' multiplier below moves the
        // one set bit of the significant up to the top position by moving it
        // 51 positions.  That will result in '0x0.8000000000000p-1022', the
        // smallest hexfloat xlC 16.1.0 compiles properly.  Subtracting
        // 'denorm_min()' from that value gives us the largest subnormal that
        // won't compile properly (the smallest was just tested above).

        static const double k_M51        = 0x8000000000000ull;
        static const double k_DENORM_MIN = DblLims::denorm_min();

        ASSERTV(
               BSLS_PLATFORM_CMP_VERSION,
               0x0.7ffffffffffffp-1022,   k_DENORM_MIN * k_M51 - k_DENORM_MIN,
               0x0.7ffffffffffffp-1022 == k_DENORM_MIN * k_M51 - k_DENORM_MIN);

        ASSERTV(BSLS_PLATFORM_CMP_VERSION,
                0x0.8000000000000p-1022,   k_DENORM_MIN * k_M51,
                0x0.8000000000000p-1022 == k_DENORM_MIN * k_M51);
#endif

        // 'long double'
        ASSERTV(0x1.5p5l, 42.0l, 0x1.5p5l == 42.0l);
        ASSERT(isSameType(0x1.5p5l, 42.0l));
        // As 'long double' may be 64 or 128 bits we don't try anything fancy.
#endif
      } break;
      case 34: {
        // --------------------------------------------------------------------
        // BSLS_COMPILERFEATURES_SUPPORT_CTAD
        //
        // Concerns:
        //: 1 'BSLS_COMPILERFEATURES_SUPPORT_CTAD' is defined when template
        //:   deduction guides are supported.
        //
        // Plan:
        //: 1 Verify that template argument deduction occurs when the macro
        //:   is defined.
        //
        // Testing:
        //   BSLS_COMPILERFEATURES_SUPPORT_CTAD
        // --------------------------------------------------------------------

        MACRO_TEST_TITLE("_SUPPORT_CTAD",
                         "=============");

        using namespace test_case_34;

#ifndef BSLS_COMPILERFEATURES_SUPPORT_CTAD
        VERBOSE_PUTS("CTAD not supported in this configuration");
#else  // BSLS_COMPILERFEATURES_SUPPORT_CTAD
        Holder h1(1);
        Holder h2(2.0);
        Holder h3(Empty{});

        ASSERT((is_same<decltype(h1), Holder<int>>::value));
        ASSERT((is_same<decltype(h2), Holder<double>>::value));
        ASSERT((is_same<decltype(h3), Holder<Empty>>::value));
#endif
      } break;
      case 33: {
        // --------------------------------------------------------------------
        // BSLS_COMPILERFEATURES_PP_LINE_IS_ON_FIRST
        //
        // Note that in this test case we test "unspecified behavior", hence
        // the assertions here serve as an early warning system to detect when
        // compilers start to deviate from our assumptions.  It is, strictly
        // speaking, not a "fault" or an "error" when unspecified behavior
        // changes and our code does not know it, because that is what
        // unspecified means: implementers of the preprocessor do not need to
        // tell.  So we have no other way to find it out but to test.
        //
        // Also note that there are more distinct '__LINE__' use scenarios for
        // which WG14 N2322 gives recommendations.  Our code is affected by one
        // of those only, hence we have one macro in the header, and test for
        // that one use case only ('__LINE__' substitution value in replacement
        // text of a macro that is invoked in a multiline manner).  There has
        // been an experiment done for the other major case, see the
        // conclusions within the test case.  There are no tests or compiler
        // feature macro for that use case ('__LINE__' as a macro argument not
        // in replacement text) because our code is not affected by that
        // behavior (yet).
        //
        // Concerns:
        //: 1 'BSLS_COMPILERFEATURES_PP_LINE_IS_ON_FIRST' is defined whenever
        //:   the preprocessor follows WG21 N2322 Recommended practice.
        //:
        //: 2 When 'BSLS_COMPILERFEATURES_PP_LINE_IS_ON_FIRST' is not defined
        //:   the preprocessor uses the line number of the last character of
        //:   the macro invocation.
        //
        // Plan:
        //: 1 Reuse the technique in {Example 2} to check that substituted
        //:   line numbers match expectations to verify C-1 and C-2.
        //
        // Testing:
        //   BSLS_COMPILERFEATURES_PP_LINE_IS_ON_FIRST
        // --------------------------------------------------------------------

        MACRO_TEST_TITLE("_PP_LINE_IS_ON_FIRST",
                         "====================");

        VERYVERBOSE_PUTS("'__LINE__' in substitution");
        {
#define BALL_LOG_LINENR_TESTER(dummy) __LINE__
    // The dummy argument is required for C++03 compatibility

            static const long k_LINE_BEFORE = __LINE__;
            static const long k_LINE_MACRO  = BALL_LOG_LINENR_TESTER
                (
                    "dummy"
                )
                ;
#undef BALL_LOG_LINENR_TESTER

#ifdef BSLS_COMPILERFEATURES_PP_LINE_IS_ON_FIRST
            ASSERTV(k_LINE_MACRO, k_LINE_BEFORE,
                   (k_LINE_MACRO - k_LINE_BEFORE),
                   (k_LINE_MACRO - k_LINE_BEFORE) == 1);  // C-1
#else
            ASSERTV(k_LINE_MACRO, k_LINE_BEFORE,
                   (k_LINE_MACRO - k_LINE_BEFORE),
                   (k_LINE_MACRO - k_LINE_BEFORE) == 4);  // C-2
#endif
        }

        // The other use case where WG14 N2322 appears to have caused changes
        // in preprocessor (compiler) behavior is the replacement value of
        // '__LINE__' as a macro argument, in a context that is not macro
        // replacement text.  In other words: passing in '__LINE__' to a
        // macro from code that is not a macro.  The experiment we have done on
        // that behavior is captured in this comment (code and all) so it is
        // not lost to time, but no attempt will be made to indicate that
        // behavior as a compiler feature as none of our code is affected by
        // it, and none of us has seen production code ever been affected by
        // it.  This variability is also simple to work around by placing the
        // macro invocation with the '__LINE__' argument into another macro,
        // thereby changing the rules that apply.
        //..
        //    if (veryVerbose) puts("'__LINE__' as macro argument");
        //    {
        //        #define BALL_LOG_LINENR_TESTER2(passthrough) (passthrough)
        //
        //        static const long k_LINE_BEFORE = __LINE__;
        //        static const long k_LINE_MACRO  = BALL_LOG_LINENR_TESTER2
        //            (
        //
        //                __LINE__
        //
        //            )
        //            ;
        //        #undef BALL_LOG_LINENR_TESTER2
        //
        //        P(k_LINE_MACRO - k_LINE_BEFORE); // 6 or 4
        //    }
        //..
        // Older compilers consistently substitute '__LINE__' with the line
        // number of the last character of the macro invocation to which
        // '__LINE__' is the argument (hence the value 6), while newer
        // compilers (clang and gcc) report the number of the line '__LINE__'
        // is on, as I believe WG14 N2322 recommends (hence the value 4).
      } break;
      case 32: {
        // --------------------------------------------------------------------
        // BSLS_COMPILERFEATURES_GUARANTEED_COPY_ELISION
        //
        // Concerns:
        //: 1 If 'BSLS_COMPILERFEATURES_GUARANTEED_COPY_ELISION' is defined,
        //:   a function returning a prvalue that is initialized in its return
        //:   statement does not make a copy of the initialized prvalue
        //:   (i.e., the object is constructed in the caller, not
        //:   constructed locally and then copied or moved).
        //:
        //: 2 If 'BSLS_COMPILERFEATURES_GUARANTEED_COPY_ELISION' is defined,
        //:   then initializing an object from a call to a function returning
        //:   a prvalue of the same type does not make a copy of the returned
        //:   object (i.e., the target object is initialized directly by
        //:   the function call).
        //:
        //: 3 Concern 2 applies when the target object is a class member
        //:   being initialized within a member initialization list.
        //:
        //: 4 Concerns 2 and 3 apply when the source and target differ in
        //:   'const' qualification.
        //
        // Plan
        //: 1 For concern 1, create a class, 'NeverCopied', that asserts
        //:   failure if its copy/move constructor or assignment operator are
        //:   called.  Create a static 'factory1' method for 'NeverCopied'
        //:   that returns a 'NeverCopied' object by value, constructing and
        //:   returning the 'NeverCopied' object in a single 'return'
        //:   statement.  Call the 'factory' method and verify that the
        //:   no-copy assertion is not tripped.  Add a static 'factory2'
        //:   method such that 'NeverCopied::factory2' returns the result
        //:   of calling, 'NeverCopied::factory1'.  Call 'factory2' and verify
        //:   that the no-copy assertion is still not tripped, even across
        //:   multiple levels of function calls.
        //:
        //: 2 For concern 2, create an instance of a 'NeverCopied' object
        //:   initialized by calling 'NeverCopied::factory2()' as the
        //:   constructor argument and verify that the no-copy assertion is
        //:   not tripped.  Repeat using the '=' form of initialization (i.e.,
        //:   'NeverCopied x = NeverCopied::factory2()').
        //:
        //: 3 For concern 3, create a class, 'NCWrapper', containing a data
        //:   member of type 'NeverCopied'.  Initialize the data member in the
        //:   constructor's member-initializer list using
        //:   'NeverCopied::factory2'.  Verify that the non-copied assertion
        //:   is not tripped.
        //:
        //: 4 For concern 4, initialize a 'const NeverCopied' object using
        //:   'factory2' and verify that the no-copy assertion is not tripped.
        //:   Create a static 'factory3' method returning a 'const
        //:   NeverCopied' object and use this overload to initialize a
        //:   non-const 'NeverCopied' object; again, the no-copy assertion
        //:   should not be tripped.  Repeat step 3 using an overload for
        //:   'NCWrapper' that calls 'factory3' instead of 'factory2'.
        //
        // Testing:
        //   BSLS_COMPILERFEATURES_GUARANTEED_COPY_ELISION
        // --------------------------------------------------------------------

        MACRO_TEST_TITLE("_GUARANTEED_COPY_ELISION",
                         "========================");

#ifndef BSLS_COMPILERFEATURES_GUARANTEED_COPY_ELISION
        VERBOSE_PUTS("The feature is not supported in this configuration");
#else
        using namespace test_case_32;

        // Step 1:
        (void)NeverCopied::factory1(8);
        (void)NeverCopied::factory2(9);
        (void)NeverCopied::factory3(10);

        // Step 2: Direct initialization
        NeverCopied z(NeverCopied::factory1(1));
        ASSERT(1 == z.value());
        NeverCopied a(NeverCopied::factory2(10));
        ASSERT(10 == a.value());

        // Step 2: Copy initialization (should have same semantics as direct
        // initialization in this case).
        NeverCopied w = NeverCopied::factory1(19);
        ASSERT(19 == w.value());
        NeverCopied b = NeverCopied::factory2(11);
        ASSERT(11 == b.value());

        // Step 3: 'NCWrapper' member initialization
        NCWrapper c(12);
        ASSERT(12 == c.value());
        NCWrapper h(17, 0);
        ASSERT(17 == h.value());

        // Step 4:
        const NeverCopied d(NeverCopied::factory2(13));
        ASSERT(13 == d.value());
        const NeverCopied e = NeverCopied::factory2(14);
        ASSERT(14 == e.value());
        NeverCopied f(NeverCopied::factory3(15));
        ASSERT(15 == f.value());
        NeverCopied g = NeverCopied::factory3(16);
        ASSERT(16 == g.value());
        NCWrapper j(18, 0, 0);
        ASSERT(18 == j.value());
#endif
      } break;
      case 31: {
        // --------------------------------------------------------------------
        // BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT_IN_FNC_TYPE
        //
        // Concerns:
        //: 1 'BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT_IN_FNC_TYPE' is defined
        //:   whenever 'noexcept' is part of the type system on function types.
        //:
        //: 2 The 'BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT_TYPES' deprecated
        //:   alias is IFF 'BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT_IN_FNC_TYPE'
        //:   is defined.
        //
        // Plan:
        //: 1 Verify that the types of functions with a 'noexcept' qualifier
        //:   are different if the macro is defined, and are the same if not.
        //
        // Testing:
        //   BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT_IN_FNC_TYPE
        //   BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT_TYPES -- deprecated
        // --------------------------------------------------------------------

        MACRO_TEST_TITLE("_SUPPORT_NOEXCEPT_TYPES",
                         "=======================");

#ifndef BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT
        VERBOSE_PUTS("The feature is not supported in this configuration");
#else  // BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT
        using namespace test_case_31;
  #ifdef BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT_IN_FNC_TYPE
        ASSERT((!is_same< decltype(foo), decltype(bar) >::value ));
  #else
        ASSERT(( is_same< decltype(foo), decltype(bar) >::value ));
  #endif  // BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT_IN_FNC_TYPE

#endif // BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT

#ifdef BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT_IN_FNC_TYPE
        const bool IsSupportNoexceptInFncType_defined = true;
#else
        const bool IsSupportNoexceptInFncType_defined = false;
#endif  // BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT_IN_FNC_TYPE

#ifdef BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT_TYPES
        const bool isDeprecatedAlias_defined = true;
#else
        const bool isDeprecatedAlias_defined = false;
#endif  // BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT_TYPES

        ASSERTV(
              IsSupportNoexceptInFncType_defined,   isDeprecatedAlias_defined,
              IsSupportNoexceptInFncType_defined == isDeprecatedAlias_defined);
      } break;
      case 30: {
        // --------------------------------------------------------------------
        // BSLS_COMPILERFEATURES_SUPPORT_RAW_STRINGS
        //
        // Concerns:
        //: 1 'BSLS_COMPILERFEATURES_SUPPORT_RAW_STRINGS' is defined
        //:    only when the compiler properly supports C++11 raw string
        //:    literals.
        //
        // Plan:
        //: 1 If 'BSLS_COMPILERFEATURES_SUPPORT_RAW_STRINGS' is defined then
        //:   compile code that attempts to use a raw string.
        //
        // Testing:
        //   BSLS_COMPILERFEATURES_SUPPORT_RAW_STRINGS
        // --------------------------------------------------------------------

        MACRO_TEST_TITLE("_SUPPORT_RAW_STRINGS",
                         "====================");

#ifndef BSLS_COMPILERFEATURES_SUPPORT_RAW_STRINGS
        VERBOSE_PUTS("The feature is not supported in this configuration.");
#else
        const char rawStringLiteral[] = R"RAW(
This is a raw string.  It is not, however, an "uncooked" string - baking it
will not improve the flavor.
)RAW";
        const char stringLiteral[] = "\nThis is a raw string.  "
                     "It is not, however, an \"uncooked\" string - baking it\n"
                     "will not improve the flavor.\n";
        ASSERTV(rawStringLiteral, stringLiteral,
                sizeof rawStringLiteral,   sizeof stringLiteral,
                sizeof rawStringLiteral == sizeof stringLiteral &&
                0 == memcmp(rawStringLiteral,          // Note the '&&'.
                            stringLiteral,            // 'memcmp' isn't called
                            sizeof stringLiteral));  // if size isn't the same.

        VERBOSE_P(rawStringLiteral);
#endif
      } break;
      case 29: {
        // --------------------------------------------------------------------
        // BSLS_COMPILERFEATURES_SUPPORT_THROW_SPECIFICATIONS
        //
        // Concerns:
        //: 1 'BSLS_COMPILERFEATURES_SUPPORT_THROW_SPECIFICATIONS' is defined
        //:    only when the compiler properly supports C++98 exception
        //:    specifications, which are removed in C++17, and were never
        //:    properly implemented by Microsoft.
        //
        // Plan:
        //: 1 If exceptions are disabled, then there is nothing to test, and
        //:   any reasonable attempt at testing will fail to compile.  Report
        //:   an supported configuration and return.
        //:
        //: 2 If 'BSLS_COMPILERFEATURES_SUPPORT_THROW_SPECIFICATIONS' is
        //:   defined then compile code that attempts to throw an invalid
        //:   exception out of a function that has an exception specification
        //:   that permits 'std::bad_exception', and confirm it is translated
        //:   into the expected type through a call to 'unexpected'.
        //
        // Testing:
        //   BSLS_COMPILERFEATURES_SUPPORT_THROW_SPECIFICATIONS
        // --------------------------------------------------------------------

        MACRO_TEST_TITLE("_SUPPORT_THROW_SPECIFICATIONS",
                         "=============================");

#ifndef BSLS_COMPILERFEATURES_SUPPORT_THROW_SPECIFICATIONS
        VERBOSE_PUTS("The feature is not supported in this configuration.");
#elif !defined(BDE_BUILD_TARGET_EXC)
        VERBOSE_PUTS("Exceptions are disabled: test skipped.");
#else
        struct LocalClass {

#ifdef BSLS_PLATFORM_HAS_PRAGMA_GCC_DIAGNOSTIC
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wdeprecated"
#endif
            static void test() throw (std::bad_exception, double) {

#ifdef BSLS_PLATFORM_HAS_PRAGMA_GCC_DIAGNOSTIC
    #pragma GCC diagnostic pop
#endif
                throw 13;
            }

            static void throwBadException() {
                throw std::bad_exception();
            }
        };

#ifdef BSLS_PLATFORM_HAS_PRAGMA_GCC_DIAGNOSTIC
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif
        std::set_unexpected(&LocalClass::throwBadException);

#ifdef BSLS_PLATFORM_HAS_PRAGMA_GCC_DIAGNOSTIC
    #pragma GCC diagnostic pop
#endif
        bool caughtBadException = false;
        try {
            LocalClass::test();
        }
        catch (const std::bad_exception&) {
            caughtBadException = true;
        }
        catch(int) {
            ASSERTV(!"Exception specification was ignored.");
        }
        catch(...) {
            ASSERTV(!"Unexpected exception type was caught.");
        }

        ASSERT(true == caughtBadException);
#endif
      } break;
      case 28: {
        // --------------------------------------------------------------------
        // BSLS_COMPILERFEATURES_SUPPORT_ATTRIBUTE_MAYBE_UNUSED
        //
        // Concerns:
        //: 1 If 'BSLS_COMPILERFEATURES_SUPPORT_ATTRIBUTE_MAYBE_UNUSED' is
        //:   defined then '[[maybe_unused]]' can be put on a function
        //:   declaration without issue.
        //:
        //: 2 The converse might not be true if there are compiler bugs related
        //:   to '[[maybe_unused]]', so we cannot test that.
        //
        // Plan:
        //: 1 If the macro is defined we have a function declared above which
        //:   uses '[[maybe_unused]]' that we make sure we can call.
        //
        // Testing:
        //   BSLS_COMPILERFEATURES_SUPPORT_ATTRIBUTE_MAYBE_UNUSED
        // --------------------------------------------------------------------

        MACRO_TEST_TITLE("_SUPPORT_ATTRIBUTE_MAYBE_UNUSED",
                         "===============================");

#ifndef BSLS_COMPILERFEATURES_SUPPORT_ATTRIBUTE_MAYBE_UNUSED
        VERBOSE_PUTS("The feature is not supported in this configuration.");
#else
        test_case_28::runTest(5);

        VERBOSE_PUTS("Compile-time-only test passed.");
#endif
      } break;
      case 27: {
        // --------------------------------------------------------------------
        // BSLS_COMPILERFEATURES_SUPPORT_ATTRIBUTE_FALLTHROUGH
        //
        // Concerns:
        //: 1 If 'BSLS_COMPILERFEATURES_SUPPORT_ATTRIBUTE_FALLTHROUGH' is
        //:   defined then '[[fallthrough]]' can be put on a function
        //:   declaration without issue.
        //:
        //: 2 The converse might not be true if there are compiler bugs related
        //:   to '[[fallthrough]]', so we cannot test that.
        //
        // Plan:
        //: 1 If the macro is defined we have a function declared above which
        //:   uses '[[fallthrough]]' that we make sure we can call.
        //
        // Testing:
        //   BSLS_COMPILERFEATURES_SUPPORT_ATTRIBUTE_FALLTHROUGH
        // --------------------------------------------------------------------

        MACRO_TEST_TITLE("_SUPPORT_ATTRIBUTE_FALLTHROUGH",
                         "==============================");

#ifndef BSLS_COMPILERFEATURES_SUPPORT_ATTRIBUTE_FALLTHROUGH
        VERBOSE_PUTS("The feature is not supported in this configuration.");
#else
        test_case_27::runTest();

        VERBOSE_PUTS("Compile-time-only test passed.");
#endif
      } break;
      case 26: {
        // --------------------------------------------------------------------
        // BSLS_COMPILERFEATURES_SUPPORT_ATTRIBUTE_NODISCARD
        //
        // Concerns:
        //: 1 If 'BSLS_COMPILERFEATURES_SUPPORT_ATTRIBUTE_NODISCARD' is defined
        //:   then '[[nodiscard]]' can be put on a function declaration without
        //:   issue.
        //:
        //: 2 The converse might not be true if there are compiler bugs related
        //:   to '[[nodiscard]]', so we cannot test that.
        //
        // Plan:
        //: 1 If the macro is defined we have a function declared above which
        //:   uses '[[nodiscard]]' that we make sure we can call.
        //
        // Testing:
        //   BSLS_COMPILERFEATURES_SUPPORT_ATTRIBUTE_NODISCARD
        // --------------------------------------------------------------------

        MACRO_TEST_TITLE("_SUPPORT_ATTRIBUTE_NODISCARD",
                         "============================");

#ifndef BSLS_COMPILERFEATURES_SUPPORT_ATTRIBUTE_NODISCARD
        VERBOSE_PUTS("The feature is not supported in this configuration.");
#else
        const int i = test_case_26::runTest();
        (void)i;

        VERBOSE_PUTS("Compile-time-only test passed.");
#endif
      } break;
      case 25: {
        // --------------------------------------------------------------------
        // BSLS_COMPILERFEATURES_SUPPORT_ATTRIBUTE_NORETURN
        //
        // Concerns:
        //: 1 If 'BSLS_COMPILERFEATURES_SUPPORT_ATTRIBUTE_NORETURN' is defined
        //:   then '[[noreturn]]' can be put on a function declaration without
        //:   issue.
        //:
        //: 2 The converse might not be true if there are compiler bugs related
        //:   to '[[noreturn]]', so we cannot test that.
        //
        // Plan:
        //: 1 If the macro is defined we have a function declared above which
        //:   uses '[[noreturn]]' that we make sure we can call.
        //
        // Testing:
        //   BSLS_COMPILERFEATURES_SUPPORT_ATTRIBUTE_NORETURN
        // --------------------------------------------------------------------

        MACRO_TEST_TITLE("_SUPPORT_ATTRIBUTE_NORETURN",
                         "===========================");

#ifndef BSLS_COMPILERFEATURES_SUPPORT_ATTRIBUTE_NORETURN
        VERBOSE_PUTS("The feature is not supported in this configuration.");
#else
        if (false) { test_case_25::runTest(); }

        VERBOSE_PUTS("Compile-time-only test passed.");
#endif
      } break;
      case 24: {
        // --------------------------------------------------------------------
        // BSLS_COMPILERFEATURES_INITIALIZER_LIST_LEAKS_ON_EXCEPTIONS
        //
        // Concerns:
        //: 1 The 'BSLS_COMPILERFEATURES_INITIALIZER_LIST_LEAKS_ON_EXCEPTIONS'
        //:   flag is defined when the native standard library provides the
        //:   class template 'std::initializer_list<T>', and the constructor
        //:   called by the compiler when providing a list of initializers does
        //:   not properly clean up after itself if an exception is thrown
        //:   copying one of those elements.  This macro should not be defined
        //:   if the library does not provide 'std::initializer_list<T>', nor
        //:   in the expected case that the library implementation does not
        //:   leak.
        //
        // Plan:
        //: 1 When 'BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS' is
        //:   defined conditionally compile code that includes
        //:   '<initializer_list>', and try to initialize from a list of
        //:   instrumented user-defined object types, using a constructor that
        //:   throws when the (instrumented) global object count gets too high.
        //:
        //: 2 Verify that the live global object count is reduced to zero after
        //:   throwing the exception, unless the macro
        //:   'BSLS_COMPILERFEATURES_HAS_CPP17_PRECISE_BITWIDTH_ATOMICS' is
        //:   defined.  (C-1)
        //
        // Testing:
        //   BSLS_COMPILERFEATURES_INITIALIZER_LIST_LEAKS_ON_EXCEPTIONS
        // --------------------------------------------------------------------

        MACRO_TEST_TITLE("_INITIALIZER_LIST_LEAKS_ON_EXCEPTIONS",
                         "=====================================");

#ifdef BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS
        VERBOSE_PUTS("The feature is not supported in this configuration.");
    #ifndef BDE_BUILD_TARGET_EXC
        VERBOSE_PUTS("Exceptions disabled: test skipped.");
    #else
        test_case_24::runTest();
    #endif
#endif
      } break;
      case 23: {
        // --------------------------------------------------------------------
        // BSLS_COMPILERFEATURES_SUPPORT_HAS_INCLUDE
        //
        // Concerns:
        //: 1 When 'BSLS_COMPILERFEATURES_SUPPORT_HAS_INCLUDE' is defined
        //:   '__has_include' is available in preprocessor conditionals.
        //:
        //: 2 When 'BSLS_COMPILERFEATURES_SUPPORT_HAS_INCLUDE' is not
        //:   available.
        //
        // Plan:
        //: 1 If 'BSLS_COMPILERFEATURES_SUPPORT_HAS_INCLUDE' is defined, try
        //:   '__has_include(<stddef.h>)', as that is a C header that exists in
        //:   non-hosted environments as well, so it is the safest bet.
        //:
        //: 2 If 'BSLS_COMPILERFEATURES_SUPPORT_HAS_INCLUDE' is *not* defined,
        //:   use the clang-suggested
        //:   (https://clang.llvm.org/docs/LanguageExtensions.html#has-include)
        //:   test '#if !defined(__has_include)' to verify that '__has_include'
        //:   is not available.
        //
        // Testing:
        //   BSLS_COMPILERFEATURES_SUPPORT_HAS_INCLUDE
        // --------------------------------------------------------------------

        MACRO_TEST_TITLE("_SUPPORT_HAS_INCLUDE",
                         "====================");

#ifdef BSLS_COMPILERFEATURES_SUPPORT_HAS_INCLUDE
    #if !__has_include(<stddef.h>)
        #error '__has_include(<stddef.h>)' does NOT work.
        BSLS_PLATFORM_COMPILER_ERROR;
    #endif
        VERBOSE_PUTS("Compile-time-only test passed.");
#else  // BSLS_COMPILERFEATURES_SUPPORT_HAS_INCLUDE
    #if defined(__has_include)
        #if __has_include(<stddef.h>)
            // The above two conditions must not be in the same '#if' condition
            // as those compilers that does not have '__has_include' defined
            // may not be able to parse the expression with '<' in it.

            #error '__has_include' appears to be present and working but      \
                   'BSLS_COMPILERFEATURES_SUPPORT_HAS_INCLUDE' is not defined.
        #endif  // '__has_include' appears to work
    #else   // __has_include is defined

        VERBOSE_PUTS("The feature is not supported in this configuration.");
    #endif  // Feature is not supported and doesn't appear to be present
#endif  // !BSLS_COMPILERFEATURES_SUPPORT_HAS_INCLUDE
      } break;
      case 22: {
        // --------------------------------------------------------------------
        // BSLS_COMPILERFEATURES_SUPPORT_UTF8_CHAR_TYPE
        //
        // Concerns:
        //: 1 'BSLS_COMPILERFEATURES_SUPPORT_UTF8_CHAR_TYPE' is defined
        //:   only when the compiler supports the 8-bit 'char8_t' type as
        //:   defined by C++20.
        //:
        //: 2 'u8' prefixed (UTF-8) string literals defined using universal
        //:   character names result in the expected in-memory sequence of
        //:   'unsigned char' bytes.
        //
        // Plan:
        //: 1 If 'BSLS_COMPILERFEATURES_SUPPORT_UTF8_CHAR_TYPE' is defined
        //:   then define 'char8_t[]' and 'char8_t *' variables initialized to
        //:   string constants with the 'u8' prefix.  Note that 'u8' character
        //:   constants do not make much sense to test as any "interesting"
        //:   Unicode code point (that isn't an original 7-bit ASCII character)
        //:   will require more than one byte of memory, and 'char8_t' is
        //:   defined to be equivalent in representation to 'unsigned char',
        //:   which has no more than 8 bits on most architectures.  (C-1)
        //:
        //: 2 Also define the expected 'unsigned char' sequence (array) of the
        //:   equivalent encoding of the universal characters used in the
        //:   'char8_t' initializer u8-string literals.  Compare the bytes of
        //:   those arrays to the array and pointer 'char8_t' constants.  (C-2)
        //
        // Testing:
        //   BSLS_COMPILERFEATURES_SUPPORT_UTF8_CHAR_TYPE
        // --------------------------------------------------------------------

        MACRO_TEST_TITLE("_SUPPORT_UTF8_CHAR_TYPE",
                         "=======================");

#ifndef BSLS_COMPILERFEATURES_SUPPORT_UTF8_CHAR_TYPE
        VERBOSE_PUTS("The feature is not supported in this configuration.");
#else
        // Verify ISO C++ size guarantee
        ASSERTV(sizeof(char8_t), 1 == sizeof(char8_t));
            // 'sizeof(unsigned char)' is defined to be 1 by the standard.

        // UTF-8 arrays
        const char8_t pound[]     = u8"\u00A3";
        const char8_t euro[]      = u8"\U000020AC";
        const char8_t poundEuro[] = u8"\u00A3\u20AC";

        ASSERTV(sizeof pound,     sizeof pound     == 3);
        ASSERTV(sizeof euro,      sizeof euro      == 4);
        ASSERTV(sizeof poundEuro, sizeof poundEuro == 6);

        // Verify content with direct array element access
        ASSERTV(pound[0], 0xc2 == pound[0]);
        ASSERTV(pound[1], 0xa3 == pound[1]);
        ASSERTV(pound[2], 0    == pound[2]);

        ASSERTV(euro[0], 0xe2 == euro[0]);
        ASSERTV(euro[1], 0x82 == euro[1]);
        ASSERTV(euro[2], 0xac == euro[2]);
        ASSERTV(euro[3], 0    == euro[3]);

        ASSERTV(poundEuro[0], 0xc2 == poundEuro[0]);
        ASSERTV(poundEuro[1], 0xa3 == poundEuro[1]);
        ASSERTV(poundEuro[2], 0xe2 == poundEuro[2]);
        ASSERTV(poundEuro[3], 0x82 == poundEuro[3]);
        ASSERTV(poundEuro[4], 0xac == poundEuro[4]);
        ASSERTV(poundEuro[5], 0    == poundEuro[5]);

        const size_t poundArrSize     = 3;
        const size_t euroArrSize      = 4;
        const size_t poundEuroArrSize = 6;

        const size_t poundStrLen     = poundArrSize     - 1;  // Cut the
        const size_t euroStrLen      = euroArrSize      - 1;  // closing
        const size_t poundEuroStrLen = poundEuroArrSize - 1;  // null.

        // There are no equivalents yet of 'strcmp' or 'strncmp' for dealing
        // with 'const char8_t *' strings therefore 'memcmp' is used with the
        // sizes established above.  We compare to 'unsigned char' arrays as
        // per ISO standard specification.

        const unsigned char ucPound[] = { 0xc2u, 0xa3u, 0};
        const unsigned char ucEuro[]  = { 0xe2u, 0x82u, 0xacu, 0 };

        // Sanity check
        ASSERTV(sizeof ucPound, sizeof ucPound == sizeof pound);
        ASSERTV(sizeof ucEuro,  sizeof ucEuro  == sizeof euro );

        // Verify content with 'memcmp'
        ASSERT(0 == memcmp(ucPound,                 pound, poundArrSize));
        ASSERT(0 == memcmp(ucEuro,                  euro,  euroArrSize));

        ASSERT(0 == memcmp(poundEuro,               pound, poundStrLen));
        ASSERT(0 == memcmp(poundEuro + poundStrLen, euro,  euroStrLen));

        ASSERT(0 == poundEuro[poundStrLen + euroStrLen]);

        // UTF-8 pointers initialized same as the arrays
        const char8_t *pound_p       = u8"\U000000A3";
        const char8_t *euro_p        = u8"\u20AC";
        const char8_t *poundEuro_p   = u8"\u00A3\u20AC";

        // Verify content with direct array element access
        ASSERTV(pound_p[0], 0xc2 == pound_p[0]);
        ASSERTV(pound_p[1], 0xa3 == pound_p[1]);
        ASSERTV(pound_p[2], 0    == pound_p[2]);

        ASSERTV(euro_p[0], 0xe2 == euro_p[0]);
        ASSERTV(euro_p[1], 0x82 == euro_p[1]);
        ASSERTV(euro_p[2], 0xac == euro_p[2]);
        ASSERTV(euro_p[3], 0    == euro_p[3]);

        ASSERTV(poundEuro_p[0], 0xc2 == poundEuro_p[0]);
        ASSERTV(poundEuro_p[1], 0xa3 == poundEuro_p[1]);
        ASSERTV(poundEuro_p[2], 0xe2 == poundEuro_p[2]);
        ASSERTV(poundEuro_p[3], 0x82 == poundEuro_p[3]);
        ASSERTV(poundEuro_p[4], 0xac == poundEuro_p[4]);
        ASSERTV(poundEuro_p[5], 0    == poundEuro_p[5]);

        // Verify content with 'memcmp'
        ASSERT(0 == memcmp(ucPound,                   pound_p, poundArrSize));
        ASSERT(0 == memcmp(ucEuro,                    euro_p,  euroArrSize));

        ASSERT(0 == memcmp(poundEuro_p,               pound_p, poundStrLen));
        ASSERT(0 == memcmp(poundEuro_p + poundStrLen, euro_p,  euroStrLen));

        if (veryVerbose) {
            // 'printf("...%s...")' expects a 'const char *', so for printing
            // 'stdio.h' 'fwrite' is used with the length that we had verified
            // earlier (minus the closing null character).

            fputs("Pound: ", stdout);
            fwrite(pound_p, 1, poundStrLen, stdout);

            fputs(", euro: ", stdout);
            fwrite(euro_p, 1, euroStrLen, stdout);

            fputs(", poundEuro: ", stdout);
            fwrite(poundEuro_p, 1, poundEuroStrLen, stdout);

            puts(""); // new line
        }
#endif
      } break;
      case 21: {
        // --------------------------------------------------------------------
        // BSLS_COMPILERFEATURES_SUPPORT_UNICODE_CHAR_TYPES
        //
        // Concerns:
        //: 1 'BSLS_COMPILERFEATURES_SUPPORT_UNICODE_CHAR_TYPES' is defined
        //:   only when the compiler supports Unicode character types Unicode
        //:   character literals, and Unicode string literals.
        //:
        //: 2 Both 16-bit and 32-bit Unicode are supported.
        //
        // Plan:
        //: 1 If 'BSLS_COMPILERFEATURES_SUPPORT_UNICODE_CHAR_TYPES' is defined
        //:   then define 'char16_t' and 'char16_t[]' variables initialized to
        //:   character and string constants with the 'u' prefix.  (C-1)
        //:
        //: 2 Also define 'char32_t' and 'char32_t[]' variables initialized to
        //:   character and string constants with the 'U' prefix.  (C-2)
        //
        // Testing:
        //   BSLS_COMPILERFEATURES_SUPPORT_UNICODE_CHAR_TYPES
        // --------------------------------------------------------------------

        MACRO_TEST_TITLE("_SUPPORT_UNICODE_CHAR_TYPES",
                         "===========================");

#ifndef BSLS_COMPILERFEATURES_SUPPORT_UNICODE_CHAR_TYPES
        VERBOSE_PUTS("The feature is not supported in this configuration.");
#else
        const char16_t leftArrow   = u'\u2190';
        const char16_t rightArrow  = u'\u2192';
        const char16_t leftRight[] = u"\u2190\u2192";

        ASSERT(0x2190     == leftArrow);
        ASSERT(0x2192     == rightArrow);
        ASSERT(leftArrow  == leftRight[0]);
        ASSERT(rightArrow == leftRight[1]);
        ASSERT(0          == leftRight[2]);

        const char32_t sadEmoticon    = U'\U0001F641';
        const char32_t smileyEmoticon = U'\U0001F642';
        const char32_t happySad[]     = U"\U0001F642\U0001F641";

        ASSERT(0x1F641        == sadEmoticon);
        ASSERT(0x1F642        == smileyEmoticon);
        ASSERT(smileyEmoticon == happySad[0]);
        ASSERT(sadEmoticon    == happySad[1]);
        ASSERT(0              == happySad[2]);
#endif
      } break;
      case 20: {
        // --------------------------------------------------------------------
        // BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS
        //
        // Concerns:
        //: 1 'BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS' is defined only
        //:   when the compiler is able to compile code with ref-qualified
        //:   functions.
        //:
        //: 2 If rvalue references are also supported, then functions can be
        //:   qualified with rvalue references.
        //:
        //: 3 Ref qualification is orthogonal to cv-qualification.
        //
        // Plan:
        //: 1 For concern 1, if 'BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS'
        //:   is defined then compile a class that defines ref-qualified member
        //:   functions.
        //:
        //: 2 For concern 2, if
        //:   'BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES' is also
        //:   defined, include rvalue-ref-qualified member functions.
        //:
        //: 3 For concern 3, try every combination of cv qualification on all
        //:   ref-qualified member functions.
        //
        // Testing:
        //   BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS
        // --------------------------------------------------------------------

        MACRO_TEST_TITLE("_SUPPORT_REF_QUALIFIERS",
                         "=======================");

#ifndef BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS
        VERBOSE_PUTS("The feature is not supported in this configuration.");
#else
        struct TestClass {
            // This class defines reference-qualified member functions.

            void foo(int)                &  { }
            void foo(int) const          &  { }
            void foo(int)       volatile &  { }
            void foo(int) const volatile &  { }

    #ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
            void foo(int)                && { }
            void foo(int) const          && { }
            void foo(int)       volatile && { }
            void foo(int) const volatile && { }
    #endif
        };
        VERBOSE_PUTS("Compile-time-only test passed.");
#endif
      } break;
      case 19: {
        // --------------------------------------------------------------------
        // BSLS_COMPILERFEATURES_SUPPORT_ALIGNAS
        //
        // Concerns:
        //: 1 'BSLS_COMPILERFEATURES_SUPPORT_ALIGNAS' is defined
        //:    only when the compiler is able to compile code with the
        //:    'alignas' specifier.
        //
        // Plan:
        //: 1 If 'BSLS_COMPILERFEATURES_SUPPORT_ALIGNAS' is defined then
        //:   compile code that uses the align as specifier.
        //
        // Testing:
        //   BSLS_COMPILERFEATURES_SUPPORT_ALIGNAS
        // --------------------------------------------------------------------

        MACRO_TEST_TITLE("_SUPPORT_ALIGNAS",
                         "================");

#ifndef BSLS_COMPILERFEATURES_SUPPORT_ALIGNAS
        VERBOSE_PUTS("The feature is not supported in this configuration.");
#else
        alignas(8) int foo; (void)foo;
        VERBOSE_PUTS("Compile-time-only test passed.");
#endif
      } break;
      case 18: {
        // --------------------------------------------------------------------
        // BSLS_COMPILERFEATURES_SUPPORT_VARIADIC_TEMPLATES
        //
        // Concerns:
        //: 1 'BSLS_COMPILERFEATURES_SUPPORT_VARIADIC_TEMPLATES' is defined
        //:    only when the compiler is able to compile code with variadic
        //:    template parameters.
        //
        // Plan:
        //: 1 If 'BSLS_COMPILERFEATURES_SUPPORT_VARIADIC_TEMPLATES' is defined
        //:   then compile code that uses variadic template parameter pack to
        //:   count the number of template parameters in the parameter pack.
        //
        // Testing:
        //   BSLS_COMPILERFEATURES_SUPPORT_VARIADIC_TEMPLATES
        // --------------------------------------------------------------------

        MACRO_TEST_TITLE("_SUPPORT_VARIADIC_TEMPLATES",
                         "===========================");

#ifndef BSLS_COMPILERFEATURES_SUPPORT_VARIADIC_TEMPLATES
        VERBOSE_PUTS("The feature is not supported in this configuration.");
#else
        ASSERT((PackSize<int, char, double, void>::VALUE == 4));
#endif
      } break;
      case 17: {
        // --------------------------------------------------------------------
        // BSLS_COMPILERFEATURES_SUPPORT_STATIC_ASSERT
        //
        // Concerns:
        //: 1 'BSLS_COMPILERFEATURES_SUPPORT_STATIC_ASSERT' is defined only
        //:    when the compiler is able to compile code with 'static_assert'.
        //
        // Plan:
        //: 1 If 'BSLS_COMPILERFEATURES_SUPPORT_STATIC_ASSERT' is defined then
        //:   compile code that uses 'static_assert'.
        //
        // Testing:
        //   BSLS_COMPILERFEATURES_SUPPORT_STATIC_ASSERT
        // --------------------------------------------------------------------

        MACRO_TEST_TITLE("_SUPPORT_STATIC_ASSERT",
                         "======================");

#ifndef BSLS_COMPILERFEATURES_SUPPORT_STATIC_ASSERT
        VERBOSE_PUTS("The feature is not supported in this configuration.");
#else
        static_assert(true, "static_assert with bool");
        static_assert(1,    "static_assert with int");

        VERBOSE_PUTS("Compile-time-only test passed.");
#endif
      } break;
      case 16: {
        // --------------------------------------------------------------------
        // BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
        //
        // Concerns:
        //: 1 'BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES' is defined only
        //:   when the compiler is able to compile code with rvalue references.
        //
        // Plan:
        //: 1 If 'BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES' is defined
        //:   then compile code that uses rvalue references to implement
        //:   perfect forwarding.
        //
        // Testing:
        //   BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
        // --------------------------------------------------------------------

        MACRO_TEST_TITLE("_SUPPORT_RVALUE_REFERENCES",
                         "==========================");

#ifndef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
        VERBOSE_PUTS("The feature is not supported in this configuration.");
#else
        RvalueTest obj(my_factory<RvalueTest>(RvalueArg()));    (void)obj;

        TemplateType<int> x = make_rvalue<TemplateType<int> >();
        x = make_rvalue<TemplateType<int> >();

        Wrapper<int> z{};
        z.test();

        VERBOSE_PUTS("Compile-time-only test passed.");
#endif
      } break;
      case 15: {
        // --------------------------------------------------------------------
        // BSLS_COMPILERFEATURES_SUPPORT_OVERRIDE
        //
        // Concerns:
        //: 1 'BSLS_COMPILERFEATURES_SUPPORT_OVERRIDE' is defined only when the
        //:    compiler supports using the 'override' keyword when overriding a
        //:    'virtual' function.
        //
        // Plan:
        //: 1 If 'BSLS_COMPILERFEATURES_SUPPORT_OVERRIDE' is defined then
        //:   compile code using the 'override' keyword when overriding a
        //:   'virtual' function.  To really test the feature works a function
        //:   that isn't an override needs to use the 'override' keyword to
        //:   produce a compile-time error.
        //
        // Testing:
        //   BSLS_COMPILERFEATURES_SUPPORT_OVERRIDE
        // --------------------------------------------------------------------

        MACRO_TEST_TITLE("_SUPPORT_OVERRIDE",
                         "=================");

#ifndef BSLS_COMPILERFEATURES_SUPPORT_OVERRIDE
        VERBOSE_PUTS("The feature is not supported in this configuration.");
#else
        struct PolymorphBase           { virtual void f() const {}          };
        struct Override: PolymorphBase {         void f() const override {} };

        VERBOSE_PUTS("Compile-time-only test passed.");
#endif
      } break;
      case 14: {
        // --------------------------------------------------------------------
        // BSLS_COMPILERFEATURES_SUPPORT_OPERATOR_EXPLICIT
        //
        // Concerns:
        //: 1 'BSLS_COMPILERFEATURES_SUPPORT_OPERATOR_EXPLICIT' is defined only
        //:    when the compiler supports 'explicit' conversion operators.
        //
        // Plan:
        //: 1 If 'BSLS_COMPILERFEATURES_SUPPORT_OPERATOR_EXPLICIT' is defined
        //:   compile code that uses an 'explicit' conversion operator for a
        //:   class with another conversion operator and use an object in a
        //:   context where an ambiguity is caused if 'explicit' is absent
        //
        // Testing:
        //   BSLS_COMPILERFEATURES_SUPPORT_OPERATOR_EXPLICIT
        // --------------------------------------------------------------------

        MACRO_TEST_TITLE("_SUPPORT_OPERATOR_EXPLICIT",
                         "==========================");

#ifndef BSLS_COMPILERFEATURES_SUPPORT_OPERATOR_EXPLICIT
        VERBOSE_PUTS("The feature is not supported in this configuration.");
#else
        struct Explicit {
            static bool match(int)  { return false; }
            static bool match(char) { return true; }

            explicit operator int() const { return 0; }
            operator char() const { return 0; }
        };

        Explicit ex;
        const bool result = Explicit::match(ex);

        ASSERT(true == result);
#endif
      } break;
      case 13: {
        // --------------------------------------------------------------------
        // BSLS_COMPILERFEATURES_SUPPORT_NULLPTR
        //
        // Concerns:
        //: 1 'BSLS_COMPILERFEATURES_SUPPORT_NULLPTR' is defined only when the
        //:    compiler is able to compile code with 'nullptr'.
        //
        // Plan:
        //: 1 If 'BSLS_COMPILERFEATURES_SUPPORT_NULLPTR' is defined then
        //:   compile code that uses 'nullptr' in various contexts.
        //
        // Testing:
        //   BSLS_COMPILERFEATURES_SUPPORT_NULLPTR
        // --------------------------------------------------------------------

        MACRO_TEST_TITLE("_SUPPORT_NULLPTR",
                         "================");

#ifndef BSLS_COMPILERFEATURES_SUPPORT_NULLPTR
        VERBOSE_PUTS("The feature is not supported in this configuration.");
#else
        void *p = nullptr;
        if (p == nullptr) {}
        OverloadForNullptr(nullptr);

        VERBOSE_PUTS("Compile-time-only test passed.");
#endif
      } break;
      case 12:{
        // --------------------------------------------------------------------
        // BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT
        //
        // Concerns:
        //: 1 'BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT' is defined only when
        //:    the compiler is able to compile code with 'noexcept'.
        //
        // Plan:
        //: 1 If 'BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT' is defined then
        //:   compile code that uses 'noexcept' in various contexts.
        //
        // Testing:
        //   BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT
        // --------------------------------------------------------------------

        MACRO_TEST_TITLE("_SUPPORT_NOEXCEPT",
                         "=================");

#ifndef BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT
        VERBOSE_PUTS("The feature is not supported in this configuration.");
#else
        noexceptTest1();
        noexceptTest2();
        noexceptTest3();
        notNoexceptTest1();
        notNoexceptTest2();

        ASSERT(noexcept(noexceptTest1()));
        ASSERT(noexcept(noexceptTest2()));
        ASSERT(noexcept(noexceptTest3()));
        ASSERT(false == noexcept(notNoexceptTest1()));
        ASSERT(false == noexcept(notNoexceptTest2()));
#endif
      } break;
      case 11: {
        // --------------------------------------------------------------------
        // BSLS_COMPILERFEATURES_SUPPORT_INCLUDE_NEXT
        //
        // Concerns:
        //: 1 'BSLS_COMPILERFEATURES_SUPPORT_INCLUDE_NEXT' is defined only when
        //:   the compiler is actually able to compile code using
        //:   'include_next'.
        //
        // Plan:
        //: 1 If 'BSLS_COMPILERFEATURES_SUPPORT_INCLUDE_NEXT' is defined then
        //:   compile code that uses this feature include a header file.
        //
        // Testing:
        //   BSLS_COMPILERFEATURES_SUPPORT_INCLUDE_NEXT
        // --------------------------------------------------------------------

        MACRO_TEST_TITLE("_SUPPORT_INCLUDE_NEXT",
                         "=====================");

#ifndef BSLS_COMPILERFEATURES_SUPPORT_INCLUDE_NEXT
        VERBOSE_PUTS("The feature is not supported in this configuration.");
#else
        VERBOSE_PUTS("Compile-time-only test passed.");
        // '#include_next' is tested at global scope.
#endif
      } break;
      case 10: {
        // --------------------------------------------------------------------
        // BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS
        //
        // Concerns:
        //: 1 'BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS' is
        //:   defined only when the compiler is actually able to compile code
        //:   using 'include_next'.
        //
        // Plan:
        //: 1 If 'BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS' is
        //:    defined then compile code that uses this feature include a
        //:    header file.
        //
        // Testing:
        //   BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS
        // --------------------------------------------------------------------

        MACRO_TEST_TITLE("_SUPPORT_GENERALIZED_INITIALIZERS",
                         "=================================");

#ifndef BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS
        VERBOSE_PUTS("The feature is not supported in this configuration.");
#else
        std::initializer_list<int> il = {10,20,30,40,50}; (void)il;

        using namespace initializerFeatureTest;
        coupling<object, couple<object, object> > mX;
        mX.use( {object{}, { object{}, object{} } });

        VERBOSE_PUTS("Compile-time-only test passed.");
#endif
      } break;
      case 9: {
        // --------------------------------------------------------------------
        // BSLS_COMPILERFEATURES_SUPPORT_FINAL
        //
        // Concerns:
        //: 1 'BSLS_COMPILERFEATURES_SUPPORT_FINAL' is defined only when the
        //:   compiler is actually able to compile code using final classes and
        //:   final member functions.
        //
        // Plan:
        //: 1 If 'BSLS_COMPILERFEATURES_SUPPORT_FINAL' is defined then compile
        //:   code that uses this feature to a final class and class with a
        //:   final function (to really verify the 'final' keyword work it is
        //:   necessary to try refining a class or a function declared
        //:   'final').
        //
        // Testing:
        //   BSLS_COMPILERFEATURES_SUPPORT_FINAL
        // --------------------------------------------------------------------

        MACRO_TEST_TITLE("_SUPPORT_FINAL",
                         "==============");

#ifndef BSLS_COMPILERFEATURES_SUPPORT_FINAL
        VERBOSE_PUTS("The feature is not supported in this configuration.");
#else
        struct FinalClass final {};

        struct FinalMember { virtual void f() final {} };

        VERBOSE_PUTS("Compile-time-only test passed.");
#endif
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // BSLS_COMPILERFEATURES_SUPPORT_EXTERN_TEMPLATE
        //
        // Concerns:
        //: 1 'BSLS_COMPILERFEATURES_SUPPORT_EXTERN_TEMPLATE' is defined only
        //:   when the compiler is actually able to compile code with extern
        //:   templates.
        //
        // Plan:
        //: 1 If 'BSLS_COMPILERFEATURES_SUPPORT_EXTERN_TEMPLATE' is defined
        //:   then compile code that uses this feature to declare extern class
        //:   templates.
        //
        // Testing:
        //   BSLS_COMPILERFEATURES_SUPPORT_EXTERN_TEMPLATE
        // --------------------------------------------------------------------

        MACRO_TEST_TITLE("_SUPPORT_EXTERN_TEMPLATE",
                         "========================");

#ifndef BSLS_COMPILERFEATURES_SUPPORT_EXTERN_TEMPLATE
        VERBOSE_PUTS("The feature is not supported in this configuration.");
#else
        externTemplateTesting::ExternTemplateClass<char> obj; (void)obj;

        VERBOSE_PUTS("Compile-time-only test passed.");
#endif
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // BSLS_COMPILERFEATURES_SUPPORT_DELETED_FUNCTIONS
        //
        // Concerns:
        //: 1 'BSLS_COMPILERFEATURES_SUPPORT_DELETED_FUNCTIONS' is defined only
        //:   when the compiler is actually able to compile code with deleted
        //:   functions.
        //
        // Plan:
        //: 1 If 'BSLS_COMPILERFEATURES_SUPPORT_DELETED_FUNCTIONS' is defined
        //:   then compile code that uses this feature to delete functions in
        //:   classes.
        //
        // Testing:
        //   BSLS_COMPILERFEATURES_SUPPORT_DELETED_FUNCTIONS
        // --------------------------------------------------------------------

        MACRO_TEST_TITLE("_SUPPORT_DELETED_FUNCTIONS",
                         "==========================");

#ifndef BSLS_COMPILERFEATURES_SUPPORT_DELETED_FUNCTIONS
        VERBOSE_PUTS("The feature is not supported in this configuration.");
#else
        ClassWithDeletedOps *p; (void)p;

        VERBOSE_PUTS("Compile-time-only test passed.");
#endif
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // BSLS_COMPILERFEATURES_SUPPORT_DEFAULTED_FUNCTIONS
        //
        // Concerns:
        //: 1 'BSLS_COMPILERFEATURES_SUPPORT_DEFAULTED_FUNCTIONS' is defined
        //:   only when the compiler is actually able to compile code with
        //:   defaulted functions.
        //
        // Plan:
        //: 1 If 'BSLS_COMPILERFEATURES_SUPPORT_DEFAULTED_FUNCTIONS' is defined
        //:   then compile code that uses this feature to defaulted functions
        //:   in classes.
        //
        // Testing:
        //   BSLS_COMPILERFEATURES_SUPPORT_DEFAULTED_FUNCTIONS
        // --------------------------------------------------------------------

        MACRO_TEST_TITLE("_SUPPORT_DEFAULTED_FUNCTIONS",
                         "============================");

#ifndef BSLS_COMPILERFEATURES_SUPPORT_DEFAULTED_FUNCTIONS
        VERBOSE_PUTS("The feature is not supported in this configuration.");
#else
        const ClassWithDefaultOps original(42);

        // test default construction.
        ClassWithDefaultOps defaulted;

        // test copy construction
        const ClassWithDefaultOps copied(original);
        ASSERT(42 == copied.d_value);

        // test copy assignment
        defaulted = original;
        ASSERT(42 == defaulted.d_value);

        // test move construction
        const ClassWithDefaultOps movedInto(ClassWithDefaultOps(42));
        ASSERT(42 == movedInto.d_value);

        // test move assignment
        defaulted.d_value = 0;
        defaulted = ClassWithDefaultOps(42);
        ASSERT(42 == defaulted.d_value);
#endif
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // BSLS_COMPILERFEATURES_SUPPORT_DECLTYPE
        //
        // Concerns:
        //: 1 'BSLS_COMPILERFEATURES_SUPPORT_DECLTYPE' is defined only when the
        //:   compiler is actually able to compile code with decltype.
        //
        // Plan:
        //: 1 If 'BSLS_COMPILERFEATURES_SUPPORT_DECLTYPE' is defined then
        //:   compile code that uses this feature to define variables of type
        //:   inferred from decltype.
        //
        // Testing:
        //   BSLS_COMPILERFEATURES_SUPPORT_DECLTYPE
        // --------------------------------------------------------------------

        MACRO_TEST_TITLE("_SUPPORT_DECLTYPE",
                         "=================");

#ifndef BSLS_COMPILERFEATURES_SUPPORT_DECLTYPE
        VERBOSE_PUTS("The feature is not supported in this configuration.");
#else
        int                                  obj1; (void)obj1;
        decltype(obj1)                       obj2; (void)obj2;
        decltype(testFuncForDecltype(10)) obj3; (void)obj3;

        const short  s      = 1000;
        const double d      = 3.2;
        const auto   maxVal = my_max(s, d);

        ASSERT(isSameType(maxVal, d));
        ASSERT(maxVal == s);
#endif
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // BSLS_COMPILERFEATURES_SUPPORT_CONSTEXPR_CPP17
        //
        // Concerns:
        //: 1 'BSLS_COMPILERFEATURES_SUPPORT_CONSTEXPR_CPP17' is defined only
        //:   when the compiler is actually able to compile code with relaxed
        //:   constexpr functions that may define lambdas that are themselves
        //:   'constexpr' or not.
        //
        // Plan:
        //: 1 If 'BSLS_COMPILERFEATURES_SUPPORT_CONSTEXPR_CPP17' is defined
        //:   then compile code that uses this feature to define relaxed
        //:   constant expression functions.
        //
        // Testing:
        //   BSLS_COMPILERFEATURES_SUPPORT_CONSTEXPR_CPP17
        // --------------------------------------------------------------------

        MACRO_TEST_TITLE("_SUPPORT_CONSTEXPR_CPP17",
                         "========================");

#ifndef BSLS_COMPILERFEATURES_SUPPORT_CONSTEXPR_CPP17
        VERBOSE_PUTS("The feature is not supported in this configuration.");
#else
        static_assert(moreRelaxedConstExprFunc(true) == 42,
                      "Relaxed (C++17) 'constexpr' is not supported");

        if (sizeof(Sniffer::test<Feature17>(0)) == sizeof(FalseType)) {
            ASSERT(!"C++17 did not detect more relaxed constexpr");
        }

        if (sizeof(Sniffer::test<Feature14>(0)) == sizeof(FalseType)) {
            ASSERT(!"C++17 did not detect relaxed constexpr");
        }

        if (sizeof(Sniffer::test<Feature11>(0)) == sizeof(FalseType)) {
            ASSERT(!"C++17 did not detect original constexpr");
        }
#endif
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // BSLS_COMPILERFEATURES_SUPPORT_CONSTEXPR_CPP14
        //
        // Concerns:
        //: 1 'BSLS_COMPILERFEATURES_SUPPORT_CONSTEXPR_CPP14' is defined only
        //:   when the compiler is actually able to compile code with relaxed
        //:   constexpr functions that may comprise of multiple statements,
        //:   including multiple return statements, and may mutate the state of
        //:   local variables.
        //:
        //: 2 When 'BSLS_COMPILERFEATURES_SUPPORT_CONSTEXPR_CPP14' is defined
        //:   constexpr member functions are not implicitly const.
        //
        // Plan:
        //: 1 If 'BSLS_COMPILERFEATURES_SUPPORT_CONSTEXPR_CPP14' is defined
        //:   then compile code that uses this feature to define relaxed
        //:   constant expression functions.
        //:
        //: 2 If 'BSLS_COMPILERFEATURES_SUPPORT_CONSTEXPR_CPP14' is defined
        //:   then compile code that uses this feature to define a constexpr
        //:   member function and detect that it is not a const member
        //:   function.  Use expression SFINAE to detect this without invoking
        //:   a compiler error.
        //
        // Testing:
        //   BSLS_COMPILERFEATURES_SUPPORT_CONSTEXPR_CPP14
        // --------------------------------------------------------------------

        MACRO_TEST_TITLE("_SUPPORT_CONSTEXPR_CPP14",
                         "========================");

#ifndef BSLS_COMPILERFEATURES_SUPPORT_CONSTEXPR_CPP14
        VERBOSE_PUTS("The feature is not supported in this configuration.");
#else
        static_assert(relaxedConstExprFunc(true) == 42,
                      "Relaxed (C++14) 'constexpr' is not supported");

        if (sizeof(Sniffer::test<Feature14>(0)) == sizeof(FalseType)) {
            ASSERT(!"C++14 did not detect relaxed constexpr");
        }

        if (sizeof(Sniffer::test<Feature11>(0)) == sizeof(FalseType)) {
            ASSERT(!"C++14 did not detect original constexpr");
        }

        if (sizeof(ConstexprConst::test<Feature14>(0)) == sizeof(FalseType)) {
            ASSERT(!"C++14 constexpr erroneously makes member function const");
        }
#endif
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // BSLS_COMPILERFEATURES_SUPPORT_CONSTEXPR
        //
        // Concerns:
        //: 1 'BSLS_COMPILERFEATURES_SUPPORT_CONSTEXPR' is defined only when
        //:   the compiler is actually able to compile code with constexpr.
        //
        // Plan:
        //: 1 If 'BSLS_COMPILERFEATURES_SUPPORT_CONSTEXPR' is defined then
        //:   compile code that uses this feature to define constant
        //:   expressions functions.
        //
        // Testing:
        //   BSLS_COMPILERFEATURES_SUPPORT_CONSTEXPR
        // --------------------------------------------------------------------

        MACRO_TEST_TITLE("_SUPPORT_CONSTEXPR",
                         "==================");

#ifndef BSLS_COMPILERFEATURES_SUPPORT_CONSTEXPR
        VERBOSE_PUTS("The feature is not supported in this configuration.");
#else
        constexpr OracleMiscompile d; // Just declaring 'd' crashes CC 12.4.
        (void)d;

        constexpr int v = A(true).d_m;
        ASSERT(v == 42);

        if (sizeof(Sniffer::test<Feature11>(0)) == sizeof(FalseType)) {
            ASSERT(!"C++11 did not detect original 'constexpr'");
        }
#endif
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BSLS_COMPILERFEATURES_SUPPORT_ALIAS_TEMPLATES
        //
        // Concerns:
        //: 1 'BSLS_COMPILERFEATURES_SUPPORT_ALIAS_TEMPLATES' is defined only
        //:   when the compiler is actually able to compile code with alias
        //:   templates.
        //
        // Plan:
        //: 1 If 'BSLS_COMPILERFEATURES_SUPPORT_ALIAS_TEMPLATES' is defined
        //:   then compile code that uses this feature to declare both alias
        //:   templates and simple aliases.
        //
        // Testing:
        //   BSLS_COMPILERFEATURES_SUPPORT_ALIAS_TEMPLATES
        // --------------------------------------------------------------------

        MACRO_TEST_TITLE("_SUPPORT_ALIAS_TEMPLATES",
                         "========================");

#ifndef BSLS_COMPILERFEATURES_SUPPORT_ALIAS_TEMPLATES
        VERBOSE_PUTS("The feature is not supported in this configuration.");
#else
        my_own_int            intObj;         (void)intObj;
        alias_nontemplate     nontemplateObj; (void)nontemplateObj;
        alias_template1<char> templateObj1;   (void)templateObj1;
        alias_template2<char> templateObj2;   (void)templateObj2;

        VERBOSE_PUTS("Compile-time-only test passed.");
#endif
      } break;
      default: {
        fprintf(stderr, "WARNING: CASE `%d' NOT FOUND.\n", test);
        testStatus = -1;
      }
    }

#if defined(BSLS_COMPILERFEATURES_SUPPORT_DEFAULT_TEMPLATE_ARGS)
    (void)&test_default_template_args;
#endif

#if defined(BSLS_COMPILERFEATURES_SUPPORT_NULLPTR)
    typedef void (*OverloadForNullPtrFuncTYpe)(int);
    OverloadForNullPtrFuncTYpe ofnp = &OverloadForNullptr;    (void)ofnp;
#endif

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) &&               \
    defined(BSLS_COMPILERFEATURES_SUPPORT_ALIAS_TEMPLATES)
    (void)&showRefCollapsingBug;
#endif

#if defined(BSLS_COMPILERFEATURES_SUPPORT_VARIADIC_TEMPLATES)
    (void)&test_func;
#endif

    if (testStatus > 0) {
        fprintf(stderr, "Error, non-zero test status = %d.\n", testStatus);
    }
    return testStatus;
}

// ----------------------------------------------------------------------------
// Copyright 2020 Bloomberg Finance L.P.
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
