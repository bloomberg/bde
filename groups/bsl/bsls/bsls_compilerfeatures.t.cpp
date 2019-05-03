// bsls_compilerfeatures.t.cpp                                        -*-C++-*-

#include <bsls_compilerfeatures.h>

#include <bsls_bsltestutil.h>
#include <bsls_buildtarget.h>

#include <exception>    // testing exception specifications

#include <stdio.h>      // 'printf'
#include <stdlib.h>     // 'atoi'

#if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
#include <initializer_list>
#endif

#if defined(BSLS_COMPILERFEATURES_SUPPORT_INCLUDE_NEXT)
# include_next<cstdio>  // Preprocessor feature test: this *IS* the check.
#endif  // BSLS_COMPILERFEATURES_SUPPORT_INCLUDE_NEXT

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                            * Overview *
// Testing available C++11 language features by trying to compile code that
// uses them.  For example, if 'BSLS_COMPILERFEATURES_SUPPORT_STATIC_ASSERT' is
// defined, then we try to compile code that uses 'static_assert'.  This is a
// purely compile-time test.  If the code compiles, then the test succeeds, if
// the code fails to compile, then the test fails.  Due to the limitations of
// the testing framework, there is no way to turn compile-time failures into
// runtime failures.  Note that we don't intend to test the correctness of the
// implementation of C++ features, but just the fact that features are
// supported.
//-----------------------------------------------------------------------------
// [22] BSLS_COMPILERFEATURES_INITIALIZER_LIST_LEAKS_ON_EXCEPTIONS
// [ 1] BSLS_COMPILERFEATURES_SUPPORT_ALIAS_TEMPLATES
// [18] BSLS_COMPILERFEATURES_SUPPORT_ALIGNAS
// [23] BSLS_COMPILERFEATURES_SUPPORT_ATTRIBUTE_NORETURN
// [24] BSLS_COMPILERFEATURES_SUPPORT_ATTRIBUTE_NODISCARD
// [25] BSLS_COMPILERFEATURES_SUPPORT_ATTRIBUTE_FALLTHROUGH
// [26] BSLS_COMPILERFEATURES_SUPPORT_ATTRIBUTE_MAYBE_UNUSED
// [ 2] BSLS_COMPILERFEATURES_SUPPORT_CONSTEXPR
// [ 3] BSLS_COMPILERFEATURES_SUPPORT_CONSTEXPR_RELAXED
// [ 4] BSLS_COMPILERFEATURES_SUPPORT_DECLTYPE
// [  ] BSLS_COMPILERFEATURES_SUPPORT_DEFAULT_TEMPLATE_ARGS
// [ 5] BSLS_COMPILERFEATURES_SUPPORT_DEFAULTED_FUNCTIONS
// [ 6] BSLS_COMPILERFEATURES_SUPPORT_DELETED_FUNCTIONS
// [  ] BSLS_COMPILERFEATURES_SUPPORT_ENUM_CLASS
// [ 7] BSLS_COMPILERFEATURES_SUPPORT_EXTERN_TEMPLATE
// [ 8] BSLS_COMPILERFEATURES_SUPPORT_FINAL
// [ 9] BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS
// [21] BSLS_COMPILERFEATURES_SUPPORT_HAS_INCLUDE
// [10] BSLS_COMPILERFEATURES_SUPPORT_INCLUDE_NEXT
// [  ] BSLS_COMPILERFEATURES_SUPPORT_INLINE_NAMESPACE
// [  ] BSLS_COMPILERFEATURES_SUPPORT_INLINE_VARIABLES
// [11] BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT
// [  ] BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT_TYPES
// [12] BSLS_COMPILERFEATURES_SUPPORT_NULLPTR
// [13] BSLS_COMPILERFEATURES_SUPPORT_OPERATOR_EXPLICIT
// [14] BSLS_COMPILERFEATURES_SUPPORT_OVERRIDE
// [19] BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS
// [15] BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
// [16] BSLS_COMPILERFEATURES_SUPPORT_STATIC_ASSERT
// [27] BSLS_COMPILERFEATURES_SUPPORT_THROW_SPECIFICATIONS
// [  ] BSLS_COMPILERFEATURES_SUPPORT_TRAITS_HEADER
// [20] BSLS_COMPILERFEATURES_SUPPORT_UNICODE_CHAR_TYPES
// [  ] BSLS_COMPILERFEATURES_SUPPORT_VARIABLE_TEMPLATES
// [17] BSLS_COMPILERFEATURES_SUPPORT_VARIADIC_TEMPLATES
// [  ] BSLS_COMPILERFEATURES_FORWARD_REF
// [  ] BSLS_COMPILERFEATURES_FORWARD
//=============================================================================

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
//              SUPPORTING FUNCTIONS AND TYPES USED FOR TESTING
//-----------------------------------------------------------------------------

#if defined(BSLS_COMPILERFEATURES_SUPPORT_ALIAS_TEMPLATES)

namespace {

template <class TYPE, class OTHER>
struct alias_base {};

using my_own_int = int;
using alias_nontemplate = alias_base<int, char>;
template <class TYPE> using alias_template1 = alias_base<TYPE, int>;
template <class TYPE> using alias_template2 = alias_base<char, TYPE>;

}  // close unnamed namespace

#endif  // BSLS_COMPILERFEATURES_SUPPORT_ALIAS_TEMPLATES

#if defined(BSLS_COMPILERFEATURES_SUPPORT_CONSTEXPR)

int x; // not constant
struct A {
    constexpr A(bool b) : m(b?42:x) { }
    int m;
};

class OracleMiscompile {
    // This class and out-of-line constructor demonstrate a known problem when
    // trying to support 'constexpr' with Oracle studio CC 5.12.4
    unsigned d_data[2];

  public:
    constexpr OracleMiscompile();
};

constexpr OracleMiscompile::OracleMiscompile()
: d_data()
{
}

template <class TYPE>
struct aggregate_base {};

template <class TYPE>
struct aggregate_derived : aggregate_base<TYPE> {};

void test_dependent_constexpr_aggregate() {
    // The following line is a regression that will not compile with Oracle CC
    // 12.5/6, and is a significant problem for type traits.
    constexpr aggregate_derived<bool> X{};
}

#endif // BSLS_COMPILERFEATURES_SUPPORT_CONSTEXPR

#if defined(BSLS_COMPILERFEATURES_SUPPORT_DECLTYPE)

// libc++ defines 'decltype' as a function-like macro when it is not provided
// by the compiler.  Our test should not be fooled by this macro.
#if defined(decltype)
# undef decltype
#endif

namespace {

#if defined(BSLS_COMPILERFEATURES_SUPPORT_DECLTYPE)
char testFuncForDecltype(int);
#endif

template <class T, class U>
auto my_max(T t, U u) -> decltype(t > u ? t : u)
{
    return t > u ? t : u;
}

}  // close unnamed namespace

#endif  // BSLS_COMPILERFEATURES_SUPPORT_DECLTYPE


#if defined(BSLS_COMPILERFEATURES_SUPPORT_DEFAULTED_FUNCTIONS)
struct ClassWithDefaultOps {
    ClassWithDefaultOps(int value) : d_value(value) {}
    ClassWithDefaultOps() = default;
    ClassWithDefaultOps(const ClassWithDefaultOps &) = default;
    ClassWithDefaultOps& operator=(const ClassWithDefaultOps &) = default;
#if !(defined(BSLS_PLATFORM_CMP_MSVC) && BSLS_PLATFORM_CMP_VERSION <= 1800)
    // MSVC 1800 does not support default move constructors/assignments
    ClassWithDefaultOps(ClassWithDefaultOps &&) = default;
    ClassWithDefaultOps & operator=(ClassWithDefaultOps &&) = default;
#endif
    ~ClassWithDefaultOps() = default;
    int d_value;
};
#endif  //BSLS_COMPILERFEATURES_SUPPORT_DEFAULTED_FUNCTIONS


#if defined(BSLS_COMPILERFEATURES_SUPPORT_DEFAULT_TEMPLATE_ARGS)

// This test case is distilled from 'shared_ptr' use of the feature that has
// been shown to crash on at least one buggy compiler.

namespace {
typedef decltype(nullptr) nullptr_t;  // avoids dependencies

template <class ELEMENT>
struct Meta {
    typedef ELEMENT type;
};

template <class ELEMENT>
struct SmartPtrWithSfinaeConstructor {
    // CREATORS
    template <class ANY_TYPE, typename Meta<ANY_TYPE>::type * = nullptr>
    SmartPtrWithSfinaeConstructor(ANY_TYPE *ptr);

    SmartPtrWithSfinaeConstructor(nullptr_t) {}
};

void test_default_template_args() {
    SmartPtrWithSfinaeConstructor<int> x = 0;   // Oracle CC 12.4 asserts.
    (void)x;
    (void)&test_default_template_args;  // Portably silence unused warnings.
}

}  // close unnamed namespace
#endif  //BSLS_COMPILERFEATURES_SUPPORT_DEFAULT_TEMPLATE_ARGS


#if defined(BSLS_COMPILERFEATURES_SUPPORT_DELETED_FUNCTIONS)
struct ClassWithDeletedOps {
    ClassWithDeletedOps() = delete;
    ClassWithDeletedOps(const ClassWithDeletedOps &) = delete;
    ClassWithDeletedOps& operator=(ClassWithDeletedOps &) = delete;
#if !(defined(BSLS_PLATFORM_CMP_MSVC) && BSLS_PLATFORM_CMP_VERSION <= 1800)
    // MSVC 1800 does not support deleted move constructors/assignments
    ClassWithDeletedOps(ClassWithDeletedOps &&) = delete;
    ClassWithDeletedOps & operator=(ClassWithDeletedOps &&) = delete;
#endif
    ~ClassWithDeletedOps() = delete;
};
#endif  //BSLS_COMPILERFEATURES_SUPPORT_DELETED_FUNCTIONS

#if defined(BSLS_COMPILERFEATURES_SUPPORT_EXTERN_TEMPLATE)

namespace {

// define class template
template <class TYPE>
class ExternTemplateClass {};

// don't instantiate in this translation unit
extern template class ExternTemplateClass<char>;

// instantiate in this translation unit
template class ExternTemplateClass<char>;

}  // close unnamed namespace

#endif  // BSLS_COMPILERFEATURES_SUPPORT_EXTERN_TEMPLATE


#if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)

namespace initializer_feature_test {
// The following code demonstrates a bug with Oracle CC 12.4, where the
// 'initializer_list' method dominates another single-argument method in
// overload resolution, despite not having a suitable conversion to the
// required 'initalizer_list' instantiation, and so rejecting a valid call.
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

}  // close namespace initializer_feature_test
#endif


#if defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)

void noexceptTest1() noexcept {
}

void noexceptTest2() noexcept(true) {
}

void noexceptTest3() noexcept(noexcept(noexceptTest1())) {
}

void notNoexceptTest1() noexcept(false) {
}

void notNoexceptTest2() noexcept(noexcept(notNoexceptTest1())) {
}

#endif  // BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT

#if defined(BSLS_COMPILERFEATURES_SUPPORT_NULLPTR)

namespace {

void OverloadForNullptr(int)
{
    (void)&OverloadForNullptr;  // Portably silence unused function warnings.
}
void OverloadForNullptr(void *) {}

}  // close unnamed namespace

#endif  // BSLS_COMPILERFEATURES_SUPPORT_NULLPTR

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)

namespace {

// Check support for the perfect forwaring idiom
//
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
TYPE&& my_forward(typename my_remove_reference<TYPE>::type& t)
{
    return static_cast<TYPE&&>(t);
}

template <class TYPE>
TYPE&& my_forward(typename my_remove_reference<TYPE>::type&& t)
{
    return static_cast<TYPE&&>(t);
}

template <class TYPE>
typename my_remove_reference<TYPE>::type&& my_move(TYPE&& t)
{
    return static_cast<typename my_remove_reference<TYPE>::type&&>(t);
}

template <class TYPE, class ARG>
TYPE my_factory(ARG&& arg)
{
    return my_move(TYPE(my_forward<ARG>(arg)));
}

struct RvalueArg {};

struct RvalueTest {
    RvalueTest(RvalueArg const &) {}
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
TemplateType<TYPE>::operator=(const TemplateType&)
{
    return *this;
}

template <class TYPE>
template <class OTHER>
TemplateType<TYPE>&
TemplateType<TYPE>::operator=(my_movable_ref<TemplateType<OTHER> >)
{
    return *this;
}

template <class TYPE>
template <class OTHER>
TemplateType<TYPE>&
TemplateType<TYPE>::operator=(my_movable_ref<OTHER>)
{
    return *this;
}

template <class TYPE>
TYPE make_rvalue() { return TYPE(); }


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


# if defined(BSLS_COMPILERFEATURES_SUPPORT_ALIAS_TEMPLATES)
// The next test exposes a bug specific to the CC 12.6 beta compiler, when
// reference-collapsing lvalue-references with rvalue-refences provided in a
// non-deducing context.  This test also relies on alias templates to set up
// the error condition, but it prevents us migrating code from C++03 -> C++11
// with our move-emulation library.

template <class TYPE>
struct RValueType { using type = TYPE &&; };

template <class TYPE>
using RValueeRef = typename RValueType<TYPE>::type;

template <class>
struct AClassTemplate {};

void showRefCollapsingBug()
{
    AClassTemplate< RValueeRef<int>& > X;
    (void)X;
}

#endif

}  // close unnamed namespace

#endif  // BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES

#if defined(BSLS_COMPILERFEATURES_SUPPORT_VARIADIC_TEMPLATES)

namespace {

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

}  // close unnamed namespace

namespace {
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

}  // close unnamed namespace
#endif  // BSLS_COMPILERFEATURES_SUPPORT_VARIADIC_TEMPLATES


#if defined(BSLS_COMPILERFEATURES_SUPPORT_CONSTEXPR)
    // Note that the tests below also rely on the use of rvalue-references and
    // the 'decltype' operator.  This has not been a problem on any tested
    // platform that also supports at least the C++11 level of 'constexpr'.
namespace {

#if defined(BSLS_COMPILERFEATURES_SUPPORT_CONSTEXPR_RELAXED)
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
#endif

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
        // Return an rvalue-reference to the (temmplate parameter) 'TYPE'.
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

constexpr Feature11::Feature11()
{
}

constexpr int Feature11::call(bool) const
{
    return 42;
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_CONSTEXPR_RELAXED)
struct Feature14 {
    // This literal type is for testing C++11 'constexpr' support.

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

constexpr Feature14::Feature14()
: d_value(-1)
{
}

constexpr int Feature14::call(bool b)
{
    if (b) {
        d_value = 42;
        return d_value;                                               // RETURN
    }
    else {
        d_value = 21;
        return d_value;                                               // RETURN
    }
}
#endif

}  // close unnamed namespace
#endif // BSLS_COMPILERFEATURES_SUPPORT_CONSTEXPR_RELAXED


                    // case 22

static const bool
    u_BSLS_COMPILERFEATURES_INITIALIZER_LIST_LEAKS_ON_EXCEPTIONS_defined =
#if defined(BSLS_COMPILERFEATURES_INITIALIZER_LIST_LEAKS_ON_EXCEPTIONS)
                                                                          true;
#else
                                                                         false;
#endif

#if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)

namespace test_case_22 {

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
    MyType(int) { addToLiveCount(); }
    MyType(const MyType&) { addToLiveCount(); }
    ~MyType() { --s_liveCount; }
};

struct MyWrapper {
    MyWrapper(std::initializer_list<MyType>) {}
};

int MyType::s_liveCount = 0;

void runTest() {
    ASSERT(0 == MyType::s_liveCount);

# if defined(BDE_BUILD_TARGET_EXC)
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
        if (
          u_BSLS_COMPILERFEATURES_INITIALIZER_LIST_LEAKS_ON_EXCEPTIONS_defined)
            ASSERT(0 != MyType::s_liveCount);
        else
            ASSERT(0 == MyType::s_liveCount);
    }
# endif
}

}  // close namespace test_case_22
#endif

                    // case 23

namespace test_case_23 {
#ifdef BSLS_COMPILERFEATURES_SUPPORT_ATTRIBUTE_NORETURN

[[noreturn]] void runTest() {
    while (true) {}
}

#else

#if (__cplusplus >= 201103L) && defined(__has_cpp_attribute)

// if specific compiler versions do have '__has_cpp_attribute' but do not
// support '[[noreturn]]' properly then those should be excluded from that
// check here.
    #if __has_cpp_attribute(noreturn)

    #error [[noreturn]] is available but feature macro is not defined.

    #endif

#endif

#endif
}  // close namespace test_case_23

                    // case 24

namespace test_case_24 {
#ifdef BSLS_COMPILERFEATURES_SUPPORT_ATTRIBUTE_NODISCARD

[[nodiscard]] int runTest() {
    return 1;
}

#else

#if (__cplusplus >= 201103L) && defined(__has_cpp_attribute)

// if specific compiler versions do have '__has_cpp_attribute' but do not
// support '[[nodiscard]]' properly then those should be excluded from that
// check here.
    #if __has_cpp_attribute(nodiscard)

    #error [[nodiscard]] is available but feature macro is not defined.

    #endif

#endif

#endif
}  // close namespace test_case_24

                    // case 25

namespace test_case_25 {
#ifdef BSLS_COMPILERFEATURES_SUPPORT_ATTRIBUTE_FALLTHROUGH

void runTest() {

    int i = 5;

    switch (i)
    {
    case 4: [[fallthrough]];
    case 7:
        return;
    default:
        return;
    }
}

#else

#if (__cplusplus >= 201103L) && defined(__has_cpp_attribute)

// if specific compiler versions do have '__has_cpp_attribute' but do not
// support '[[fallthrough]]' properly then those should be excluded from that
// check here.
    #if __has_cpp_attribute(fallthrough)

    #error [[fallthrough]] is available but feature macro is not defined.

    #endif

#endif

#endif
}  // close namespace test_case_25

                    // case 26

namespace test_case_26 {
#ifdef BSLS_COMPILERFEATURES_SUPPORT_ATTRIBUTE_MAYBE_UNUSED

void runTest([[maybe_unused]] int i) {
}

#else

#if (__cplusplus >= 201103L) && defined(__has_cpp_attribute)

// if specific compiler versions do have '__has_cpp_attribute' but do not
// support '[[maybe_unused]]' properly then those should be excluded from that
// check here.
    #if __has_cpp_attribute(maybe_unused)

    #if defined(BSLS_PLATFORM_CMP_CLANG)
    // clang does not allow maybe_unused where other compilers do.
    #else
    #error [[maybe_unused]] is available but feature macro is not defined.
    #endif

    #endif

#endif

#endif
}  // close namespace test_case_26

// ============================================================================
//                              HELPER FUNCTIONS
// ----------------------------------------------------------------------------

static void printFlags()
    // Print a diagnostic message to standard output if any of the preprocessor
    // flags of interest are defined, and their value if a value had been set.
    // An "Enter" and "Leave" message is printed unconditionally so there is
    // some report even if all of the flags are undefined.
{
    printf("printFlags: Enter\n");

    printf("\n  printFlags: bsls_compilerfeatures Macros\n");

    printf("\n  BSLS_COMPILERFEATURES_FILLT(n): ");
#ifdef BSLS_COMPILERFEATURES_FILLT
    printf("%s\n", STRINGIFY(BSLS_COMPILERFEATURES_FILLT(n)) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_COMPILERFEATURES_FILLV(n): ");
#ifdef BSLS_COMPILERFEATURES_FILLV
    printf("%s\n", STRINGIFY(BSLS_COMPILERFEATURES_FILLV(n)) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_COMPILERFEATURES_FORWARD(T,V): ");
#ifdef BSLS_COMPILERFEATURES_FORWARD
    printf("%s\n", STRINGIFY(BSLS_COMPILERFEATURES_FORWARD(T,V)) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_COMPILERFEATURES_FORWARD_REF(T): ");
#ifdef BSLS_COMPILERFEATURES_FORWARD_REF
    printf("%s\n", STRINGIFY(BSLS_COMPILERFEATURES_FORWARD_REF(T)) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_COMPILERFEATURES_INITIALIZER_LIST_LEAKS_ON_EXCEPTIONS: ");
#ifdef BSLS_COMPILERFEATURES_INITIALIZER_LIST_LEAKS_ON_EXCEPTIONS
    printf("%s\n",
       STRINGIFY(BSLS_COMPILERFEATURES_INITIALIZER_LIST_LEAKS_ON_EXCEPTIONS) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_COMPILERFEATURES_NILT: ");
#ifdef BSLS_COMPILERFEATURES_NILT
    printf("%s\n", STRINGIFY(BSLS_COMPILERFEATURES_NILT) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_COMPILERFEATURES_NILTR(n): ");
#ifdef BSLS_COMPILERFEATURES_NILTR
    printf("%s\n", STRINGIFY(BSLS_COMPILERFEATURES_NILTR(n)) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_COMPILERFEATURES_NILV: ");
#ifdef BSLS_COMPILERFEATURES_NILV
    printf("%s\n", STRINGIFY(BSLS_COMPILERFEATURES_NILV) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_COMPILERFEATURES_NILVR(n): ");
#ifdef BSLS_COMPILERFEATURES_NILVR
    printf("%s\n", STRINGIFY(BSLS_COMPILERFEATURES_NILVR(n)) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES: ");
#ifdef BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
    printf("%s\n", STRINGIFY(BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_COMPILERFEATURES_SIMULATE_FORWARD_WORKAROUND: ");
#ifdef BSLS_COMPILERFEATURES_SIMULATE_FORWARD_WORKAROUND
    printf("%s\n",
                STRINGIFY(BSLS_COMPILERFEATURES_SIMULATE_FORWARD_WORKAROUND) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_COMPILERFEATURES_SIMULATE_VARIADIC_TEMPLATES: ");
#ifdef BSLS_COMPILERFEATURES_SIMULATE_VARIADIC_TEMPLATES
    printf("%s\n",
                STRINGIFY(BSLS_COMPILERFEATURES_SIMULATE_VARIADIC_TEMPLATES) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_COMPILERFEATURES_SUPPORT_ALIAS_TEMPLATES: ");
#ifdef BSLS_COMPILERFEATURES_SUPPORT_ALIAS_TEMPLATES
    printf("%s\n", STRINGIFY(BSLS_COMPILERFEATURES_SUPPORT_ALIAS_TEMPLATES) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_COMPILERFEATURES_SUPPORT_ALIGNAS: ");
#ifdef BSLS_COMPILERFEATURES_SUPPORT_ALIGNAS
    printf("%s\n", STRINGIFY(BSLS_COMPILERFEATURES_SUPPORT_ALIGNAS) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_COMPILERFEATURES_SUPPORT_ATTRIBUTE_FALLTHROUGH: ");
#ifdef BSLS_COMPILERFEATURES_SUPPORT_ATTRIBUTE_FALLTHROUGH
    printf("%s\n",
              STRINGIFY(BSLS_COMPILERFEATURES_SUPPORT_ATTRIBUTE_FALLTHROUGH) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_COMPILERFEATURES_SUPPORT_ATTRIBUTE_MAYBE_UNUSED: ");
#ifdef BSLS_COMPILERFEATURES_SUPPORT_ATTRIBUTE_MAYBE_UNUSED
    printf("%s\n",
             STRINGIFY(BSLS_COMPILERFEATURES_SUPPORT_ATTRIBUTE_MAYBE_UNUSED) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_COMPILERFEATURES_SUPPORT_ATTRIBUTE_NODISCARD: ");
#ifdef BSLS_COMPILERFEATURES_SUPPORT_ATTRIBUTE_NODISCARD
    printf("%s\n",
                STRINGIFY(BSLS_COMPILERFEATURES_SUPPORT_ATTRIBUTE_NODISCARD) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_COMPILERFEATURES_SUPPORT_ATTRIBUTE_NORETURN: ");
#ifdef BSLS_COMPILERFEATURES_SUPPORT_ATTRIBUTE_NORETURN
    printf("%s\n",
                 STRINGIFY(BSLS_COMPILERFEATURES_SUPPORT_ATTRIBUTE_NORETURN) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_COMPILERFEATURES_SUPPORT_CONSTEXPR: ");
#ifdef BSLS_COMPILERFEATURES_SUPPORT_CONSTEXPR
    printf("%s\n", STRINGIFY(BSLS_COMPILERFEATURES_SUPPORT_CONSTEXPR) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_COMPILERFEATURES_SUPPORT_CONSTEXPR_RELAXED: ");
#ifdef BSLS_COMPILERFEATURES_SUPPORT_CONSTEXPR_RELAXED
    printf("%s\n",
                  STRINGIFY(BSLS_COMPILERFEATURES_SUPPORT_CONSTEXPR_RELAXED) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_COMPILERFEATURES_SUPPORT_DECLTYPE: ");
#ifdef BSLS_COMPILERFEATURES_SUPPORT_DECLTYPE
    printf("%s\n", STRINGIFY(BSLS_COMPILERFEATURES_SUPPORT_DECLTYPE) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_COMPILERFEATURES_SUPPORT_DEFAULTED_FUNCTIONS: ");
#ifdef BSLS_COMPILERFEATURES_SUPPORT_DEFAULTED_FUNCTIONS
    printf("%s\n",
                STRINGIFY(BSLS_COMPILERFEATURES_SUPPORT_DEFAULTED_FUNCTIONS) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_COMPILERFEATURES_SUPPORT_DEFAULT_TEMPLATE_ARGS: ");
#ifdef BSLS_COMPILERFEATURES_SUPPORT_DEFAULT_TEMPLATE_ARGS
    printf("%s\n",
              STRINGIFY(BSLS_COMPILERFEATURES_SUPPORT_DEFAULT_TEMPLATE_ARGS) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_COMPILERFEATURES_SUPPORT_DELETED_FUNCTIONS: ");
#ifdef BSLS_COMPILERFEATURES_SUPPORT_DELETED_FUNCTIONS
    printf("%s\n",
                  STRINGIFY(BSLS_COMPILERFEATURES_SUPPORT_DELETED_FUNCTIONS) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_COMPILERFEATURES_SUPPORT_ENUM_CLASS: ");
#ifdef BSLS_COMPILERFEATURES_SUPPORT_ENUM_CLASS
    printf("%s\n", STRINGIFY(BSLS_COMPILERFEATURES_SUPPORT_ENUM_CLASS) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_COMPILERFEATURES_SUPPORT_EXTERN_TEMPLATE: ");
#ifdef BSLS_COMPILERFEATURES_SUPPORT_EXTERN_TEMPLATE
    printf("%s\n", STRINGIFY(BSLS_COMPILERFEATURES_SUPPORT_EXTERN_TEMPLATE) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_COMPILERFEATURES_SUPPORT_FINAL: ");
#ifdef BSLS_COMPILERFEATURES_SUPPORT_FINAL
    printf("%s\n", STRINGIFY(BSLS_COMPILERFEATURES_SUPPORT_FINAL) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS: ");
#ifdef BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS
    printf("%s\n",
           STRINGIFY(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_COMPILERFEATURES_SUPPORT_HAS_INCLUDE: ");
#ifdef BSLS_COMPILERFEATURES_SUPPORT_HAS_INCLUDE
    printf("%s\n", STRINGIFY(BSLS_COMPILERFEATURES_SUPPORT_HAS_INCLUDE) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_COMPILERFEATURES_SUPPORT_INCLUDE_NEXT: ");
#ifdef BSLS_COMPILERFEATURES_SUPPORT_INCLUDE_NEXT
    printf("%s\n", STRINGIFY(BSLS_COMPILERFEATURES_SUPPORT_INCLUDE_NEXT) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_COMPILERFEATURES_SUPPORT_INLINE_NAMESPACE: ");
#ifdef BSLS_COMPILERFEATURES_SUPPORT_INLINE_NAMESPACE
    printf("%s\n",
                   STRINGIFY(BSLS_COMPILERFEATURES_SUPPORT_INLINE_NAMESPACE) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_COMPILERFEATURES_SUPPORT_INLINE_VARIABLES: ");
#ifdef BSLS_COMPILERFEATURES_SUPPORT_INLINE_VARIABLES
    printf("%s\n",
                   STRINGIFY(BSLS_COMPILERFEATURES_SUPPORT_INLINE_VARIABLES) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT: ");
#ifdef BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT
    printf("%s\n", STRINGIFY(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT_TYPES: ");
#ifdef BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT_TYPES
    printf("%s\n", STRINGIFY(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT_TYPES) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_COMPILERFEATURES_SUPPORT_NULLPTR: ");
#ifdef BSLS_COMPILERFEATURES_SUPPORT_NULLPTR
    printf("%s\n", STRINGIFY(BSLS_COMPILERFEATURES_SUPPORT_NULLPTR) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_COMPILERFEATURES_SUPPORT_OPERATOR_EXPLICIT: ");
#ifdef BSLS_COMPILERFEATURES_SUPPORT_OPERATOR_EXPLICIT
    printf("%s\n",
                  STRINGIFY(BSLS_COMPILERFEATURES_SUPPORT_OPERATOR_EXPLICIT) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_COMPILERFEATURES_SUPPORT_OVERRIDE: ");
#ifdef BSLS_COMPILERFEATURES_SUPPORT_OVERRIDE
    printf("%s\n", STRINGIFY(BSLS_COMPILERFEATURES_SUPPORT_OVERRIDE) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS: ");
#ifdef BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS
    printf("%s\n", STRINGIFY(BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES: ");
#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
    printf("%s\n",
                  STRINGIFY(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_COMPILERFEATURES_SUPPORT_STATIC_ASSERT: ");
#ifdef BSLS_COMPILERFEATURES_SUPPORT_STATIC_ASSERT
    printf("%s\n", STRINGIFY(BSLS_COMPILERFEATURES_SUPPORT_STATIC_ASSERT) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_COMPILERFEATURES_SUPPORT_THROW_SPECIFICATIONS: ");
#ifdef BSLS_COMPILERFEATURES_SUPPORT_THROW_SPECIFICATIONS
    printf("%s\n",
               STRINGIFY(BSLS_COMPILERFEATURES_SUPPORT_THROW_SPECIFICATIONS) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_COMPILERFEATURES_SUPPORT_TRAITS_HEADER: ");
#ifdef BSLS_COMPILERFEATURES_SUPPORT_TRAITS_HEADER
    printf("%s\n", STRINGIFY(BSLS_COMPILERFEATURES_SUPPORT_TRAITS_HEADER) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_COMPILERFEATURES_SUPPORT_UNICODE_CHAR_TYPES: ");
#ifdef BSLS_COMPILERFEATURES_SUPPORT_UNICODE_CHAR_TYPES
    printf("%s\n",
                 STRINGIFY(BSLS_COMPILERFEATURES_SUPPORT_UNICODE_CHAR_TYPES) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_COMPILERFEATURES_SUPPORT_VARIABLE_TEMPLATES: ");
#ifdef BSLS_COMPILERFEATURES_SUPPORT_VARIABLE_TEMPLATES
    printf("%s\n",
                 STRINGIFY(BSLS_COMPILERFEATURES_SUPPORT_VARIABLE_TEMPLATES) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_COMPILERFEATURES_SUPPORT_VARIADIC_TEMPLATES: ");
#ifdef BSLS_COMPILERFEATURES_SUPPORT_VARIADIC_TEMPLATES
    printf("%s\n",
                 STRINGIFY(BSLS_COMPILERFEATURES_SUPPORT_VARIADIC_TEMPLATES) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n\n  printFlags: bsls_compilerfeatures Referenced Macros\n");

    printf("\n  BSLS_COMPILERFEATURES_SIMULATE_FORWARD_WORKAROUND: ");
#ifdef BSLS_COMPILERFEATURES_SIMULATE_FORWARD_WORKAROUND
    printf("%s\n",
                STRINGIFY(BSLS_COMPILERFEATURES_SIMULATE_FORWARD_WORKAROUND) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_COMPILERFEATURES_SUPPORT_ALIAS_TEMPLATES: ");
#ifdef BSLS_COMPILERFEATURES_SUPPORT_ALIAS_TEMPLATES
    printf("%s\n", STRINGIFY(BSLS_COMPILERFEATURES_SUPPORT_ALIAS_TEMPLATES) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_COMPILERFEATURES_SUPPORT_EXTERN_TEMPLATE: ");
#ifdef BSLS_COMPILERFEATURES_SUPPORT_EXTERN_TEMPLATE
    printf("%s\n", STRINGIFY(BSLS_COMPILERFEATURES_SUPPORT_EXTERN_TEMPLATE) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES: ");
#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
    printf("%s\n",
                  STRINGIFY(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_PLATFORM_CMP_CLANG: ");
#ifdef BSLS_PLATFORM_CMP_CLANG
    printf("%s\n", STRINGIFY(BSLS_PLATFORM_CMP_CLANG) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_PLATFORM_CMP_GNU: ");
#ifdef BSLS_PLATFORM_CMP_GNU
    printf("%s\n", STRINGIFY(BSLS_PLATFORM_CMP_GNU) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_PLATFORM_CMP_IBM: ");
#ifdef BSLS_PLATFORM_CMP_IBM
    printf("%s\n", STRINGIFY(BSLS_PLATFORM_CMP_IBM) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_PLATFORM_CMP_MSVC: ");
#ifdef BSLS_PLATFORM_CMP_MSVC
    printf("%s\n", STRINGIFY(BSLS_PLATFORM_CMP_MSVC) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_PLATFORM_CMP_SUN: ");
#ifdef BSLS_PLATFORM_CMP_SUN
    printf("%s\n", STRINGIFY(BSLS_PLATFORM_CMP_SUN) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  __APPLE_CC__: ");
#ifdef __APPLE_CC__
    printf("%s\n", STRINGIFY(__APPLE_CC__) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  __GXX_EXPERIMENTAL_CXX0X__: ");
#ifdef __GXX_EXPERIMENTAL_CXX0X__
    printf("%s\n", STRINGIFY(__GXX_EXPERIMENTAL_CXX0X__) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  __IBMCPP_CONSTEXPR: ");
#ifdef __IBMCPP_CONSTEXPR
    printf("%s\n", STRINGIFY(__IBMCPP_CONSTEXPR) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  __IBMCPP_DECLTYPE: ");
#ifdef __IBMCPP_DECLTYPE
    printf("%s\n", STRINGIFY(__IBMCPP_DECLTYPE) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  __IBMCPP_DEFAULTED_AND_DELETED_FUNCTIONS: ");
#ifdef __IBMCPP_DEFAULTED_AND_DELETED_FUNCTIONS
    printf("%s\n", STRINGIFY(__IBMCPP_DEFAULTED_AND_DELETED_FUNCTIONS) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  __IBMCPP_EXPLICIT: ");
#ifdef __IBMCPP_EXPLICIT
    printf("%s\n", STRINGIFY(__IBMCPP_EXPLICIT) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  __IBMCPP_EXTERN_TEMPLATE: ");
#ifdef __IBMCPP_EXTERN_TEMPLATE
    printf("%s\n", STRINGIFY(__IBMCPP_EXTERN_TEMPLATE) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  __IBMCPP_INLINE_NAMESPACE: ");
#ifdef __IBMCPP_INLINE_NAMESPACE
    printf("%s\n", STRINGIFY(__IBMCPP_INLINE_NAMESPACE) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  __IBMCPP_NULLPTR: ");
#ifdef __IBMCPP_NULLPTR
    printf("%s\n", STRINGIFY(__IBMCPP_NULLPTR) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  __IBMCPP_OVERRIDE: ");
#ifdef __IBMCPP_OVERRIDE
    printf("%s\n", STRINGIFY(__IBMCPP_OVERRIDE) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  __IBMCPP_RVALUE_REFERENCES: ");
#ifdef __IBMCPP_RVALUE_REFERENCES
    printf("%s\n", STRINGIFY(__IBMCPP_RVALUE_REFERENCES) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  __IBMCPP_STATIC_ASSERT: ");
#ifdef __IBMCPP_STATIC_ASSERT
    printf("%s\n", STRINGIFY(__IBMCPP_STATIC_ASSERT) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  __IBMCPP_VARIADIC_TEMPLATES: ");
#ifdef __IBMCPP_VARIADIC_TEMPLATES
    printf("%s\n", STRINGIFY(__IBMCPP_VARIADIC_TEMPLATES) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  __IBMC_NORETURN: ");
#ifdef __IBMC_NORETURN
    printf("%s\n", STRINGIFY(__IBMC_NORETURN) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  __IBM_INCLUDE_NEXT: ");
#ifdef __IBM_INCLUDE_NEXT
    printf("%s\n", STRINGIFY(__IBM_INCLUDE_NEXT) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  __cplusplus: ");
#ifdef __cplusplus
    printf("%s\n", STRINGIFY(__cplusplus) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  __cpp_unicode_characters: ");
#ifdef __cpp_unicode_characters
    printf("%s\n", STRINGIFY(__cpp_unicode_characters) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  __cpp_unicode_literals: ");
#ifdef __cpp_unicode_literals
    printf("%s\n", STRINGIFY(__cpp_unicode_literals) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  __has_cpp_attribute: ");
#ifdef __has_cpp_attribute
    printf("DEFINED\n");
#else
    printf("UNDEFINED\n");
#endif

    printf("\n\nprintFlags: Leave\n");
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

    (void)        veryVerbose;  // unused variable warning
    (void)    veryVeryVerbose;  // unused variable warning
    (void)veryVeryVeryVerbose;  // unused variable warning

    setbuf(stdout, NULL);    // Use unbuffered output

    printf("TEST " __FILE__ " CASE %d\n", test);

    if (veryVeryVerbose) {
        printFlags();
    }

    switch (test) { case 0:
      case 27: {
        // --------------------------------------------------------------------
        // TESTING 'BSLS_COMPILERFEATURES_SUPPORT_THROW_SPECIFICATIONS'
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
        //: 2 If 'BSLS_COMPILERFEATURES_SUPPORT_THROW_SPECIFICATIONS' is
        //:   defined then compile code that attempts to throw an invalid
        //:   exception out of a function that has an exception specification
        //:   that permits 'std::bad_exception', and confirm it is translated
        //:   into the expected type through a call to 'unexpected'.
        //
        // Testing:
        //   BSLS_COMPILERFEATURES_SUPPORT_THROW_SPECIFICATIONS
        // --------------------------------------------------------------------

        if (verbose) printf(
           "\nTESTING 'BSLS_COMPILERFEATURES_SUPPORT_THROW_SPECIFICATIONS'"
           "\n============================================================\n");

#if !defined(BSLS_COMPILERFEATURES_SUPPORT_THROW_SPECIFICATIONS)
        if (verbose) printf("Feature not supported in this configuration.\n");
#elif !defined(BDE_BUILD_TARGET_EXC)
        if (verbose) printf("Test disabled as exceptions are NOT enabled.\n");
#else
        struct LocalClass {
            static void test() throw (std::bad_exception, double) {
                throw 13;
            }

            static void throwBadException() {
                throw std::bad_exception();
            }
        };

        std::set_unexpected(&LocalClass::throwBadException);

        bool caughtBadException = false;
        try {
            LocalClass::test();
        }
        catch (const std::bad_exception&) {
            caughtBadException = true;
        }
        catch(int) {
            ASSERTV(!"Exception specifications are ignored");
        }
        catch(...) {
            ASSERTV(!"Exception translated to unknown type");
        }

        ASSERTV(caughtBadException, caughtBadException);
#endif
      } break;
    case 26: {
        // --------------------------------------------------------------------
        // TESTING 'BSLS_COMPILERFEATURES_SUPPORT_ATTRIBUTE_MAYBE_UNUSED'
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

        if (verbose) printf(
             "TESTING 'BSLS_COMPILERFEATURES_SUPPORT_ATTRIBUTE_MAYBE_UNUSED'\n"
             "==============================================================\n"
            );

#if defined(BSLS_COMPILERFEATURES_SUPPORT_ATTRIBUTE_MAYBE_UNUSED)
        test_case_26::runTest(5);
#endif

      } break;
    case 25: {
        // --------------------------------------------------------------------
        // TESTING 'BSLS_COMPILERFEATURES_SUPPORT_ATTRIBUTE_FALLTHROUGH'
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

        if (verbose) printf(
              "TESTING 'BSLS_COMPILERFEATURES_SUPPORT_ATTRIBUTE_FALLTHROUGH'\n"
              "=============================================================\n"
            );

#if defined(BSLS_COMPILERFEATURES_SUPPORT_ATTRIBUTE_FALLTHROUGH)
        test_case_25::runTest();
#endif

      } break;
    case 24: {
        // --------------------------------------------------------------------
        // TESTING 'BSLS_COMPILERFEATURES_SUPPORT_ATTRIBUTE_NODISCARD'
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

        if (verbose) printf(
                "TESTING 'BSLS_COMPILERFEATURES_SUPPORT_ATTRIBUTE_NODISCARD'\n"
                "===========================================================\n"
            );

#if defined(BSLS_COMPILERFEATURES_SUPPORT_ATTRIBUTE_NODISCARD)
        int i = test_case_24::runTest();
        (void)i;
#endif

      } break;
    case 23: {
        // --------------------------------------------------------------------
        // TESTING 'BSLS_COMPILERFEATURES_SUPPORT_ATTRIBUTE_NORETURN'
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

        if (verbose) printf(
                 "TESTING 'BSLS_COMPILERFEATURES_SUPPORT_ATTRIBUTE_NORETURN'\n"
                 "==========================================================\n"
            );

#if defined(BSLS_COMPILERFEATURES_SUPPORT_ATTRIBUTE_NORETURN)
        if (false) { test_case_23::runTest(); }
#endif

      } break;
      case 22: {
        // --------------------------------------------------------------------
        // TESTING 'BSLS_COMPILERFEATURES_INITIALIZER_LIST_LEAKS_ON_EXCEPTIONS'
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
        //: 2 Verify that the live global object count is reduced to zero after
        //:   throwing the exception, unless the macro
        //:   'BSLS_COMPILERFEATURES_HAS_CPP17_PRECISE_BITWIDTH_ATOMICS' is
        //:   defined.  (C-1)
        //
        // Testing:
        //   BSLS_COMPILERFEATURES_INITIALIZER_LIST_LEAKS_ON_EXCEPTIONS
        // --------------------------------------------------------------------

        if (verbose) printf(
   "\nTESTING 'BSLS_COMPILERFEATURES_INITIALIZER_LIST_LEAKS_ON_EXCEPTIONS'"
   "\n====================================================================\n");

#if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
#    if defined(BDE_BUILD_TARGET_EXC)
        test_case_22::runTest();
#    else
        if (verbose) printf("Test disabled as exceptions are NOT enabled.\n");
#    endif
#endif

        if (verbose) {
            P(
          u_BSLS_COMPILERFEATURES_INITIALIZER_LIST_LEAKS_ON_EXCEPTIONS_defined)
        }

        if (veryVeryVerbose) {
            P(BSLS_PLATFORM_CMP_VERSION);
        }

      } break;
      case 21: {
        // --------------------------------------------------------------------
        // TESTING 'BSLS_COMPILERFEATURES_SUPPORT_HAS_INCLUDE'
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
        //: 2 If 'BSLS_COMPILERFEATURES_SUPPORT_HAS_INCLUDE' is *not* defined,
        //:   use the clang-suggested
        //:   (https://clang.llvm.org/docs/LanguageExtensions.html#has-include)
        //:   test '#if !defined(__has_include)' to verify that '__has_include'
        //:   is not available.
        //
        // Testing:
        //   BSLS_COMPILERFEATURES_SUPPORT_HAS_INCLUDE
        // --------------------------------------------------------------------

        if (verbose) printf(
                    "\nTESTING 'BSLS_COMPILERFEATURES_SUPPORT_HAS_INCLUDE'"
                    "\n===================================================\n");

#if defined(BSLS_COMPILERFEATURES_SUPPORT_HAS_INCLUDE)
# if !__has_include(<stddef.h>)
#   error '__has_include' appears not to be working.
# endif
#else
# if defined(__has_include)
#   if __has_include(<stddef.h>)
#     error '__has_include' appears to be present but \
            'BSLS_COMPILERFEATURES_SUPPORT_HAS_INCLUDE' is not defined.
#   endif
# endif
#endif
      } break;
      case 20: {
        // --------------------------------------------------------------------
        // TESTING 'BSLS_COMPILERFEATURES_SUPPORT_UNICODE_CHAR_TYPES'
        //
        // Concerns:
        //: 1 'BSLS_COMPILERFEATURES_SUPPORT_UNICODE_CHAR_TYPES' is defined
        //:   only when the compiler supports unicode character types unicode
        //:   character literals, and unicode string literals.
        //: 2 Both 16-bit and 32-bit unicode are supported.
        //: 3 8-bit unicode is a C++17 feature and is not tested.
        //
        // Plan:
        //: 1 For concern 1, if
        //:   'BSLS_COMPILERFEATURES_SUPPORT_UNICODE_CHAR_TYPES' is defined
        //:   then define 'char16_t' and 'char16_t[]' variables initialized to
        //:   character and string constants with the 'u' prefix.
        //: 2 For concern 2, also define 'char32_t' and 'char32_t[]' variables
        //:   initialized to character and string constants with the 'U'
        //:   prefix.
        //: 3 For concern 3, eventually define 'char8_t' and 'char8_t[]'
        //:   variables initialized to character and string constants with the
        //:   'u8' prefix.  It is likely that a different macro will be tested
        //:   for the u8 types.
        //
        // Testing:
        //   BSLS_COMPILERFEATURES_SUPPORT_UNICODE_CHAR_TYPES
        // --------------------------------------------------------------------

        if (verbose) printf(
             "\nTESTING 'BSLS_COMPILERFEATURES_SUPPORT_UNICODE_CHAR_TYPES'"
             "\n==========================================================\n");

#if !defined(BSLS_COMPILERFEATURES_SUPPORT_UNICODE_CHAR_TYPES)
        if (verbose) printf("Feature not supported in this configuration.\n");
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
      case 19: {
        // --------------------------------------------------------------------
        // TESTING 'BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS'
        //
        // Concerns:
        //: 1 'BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS' is defined only
        //:   when the compiler is able to compile code with ref-qualified
        //:   functions.
        //: 2 If rvalue references are also supported, then functions can be
        //:   qualified with rvalue references.
        //: 3 Ref qualification is orthogonal to cv-qualification.
        //
        // Plan:
        //: 1 For concern 1, if 'BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS'
        //:   is defined then compile a class that defines ref-qualified member
        //:   functions.
        //: 2 For concern 2, if
        //:   'BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES' is also
        //:   defined, include rvalue-ref-qualified member functions.
        //: 3 For concern 3, try every combination of cv qualification on all
        //:   ref-qualified member functions.
        //
        // Testing:
        //   BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS
        // --------------------------------------------------------------------

        if (verbose) printf(
                 "\nTESTING 'BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS'"
                 "\n======================================================\n");

#if !defined(BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS)
        if (verbose) printf("Feature not supported in this configuration.\n");
#else
        struct TestClass {
            // This class defines reference-qualified member functions

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
#endif
      } break;
      case 18: {
        // --------------------------------------------------------------------
        // TESTING 'BSLS_COMPILERFEATURES_SUPPORT_ALIGNAS'
        //
        // Concerns:
        //: 1 'BSLS_COMPILERFEATURES_SUPPORT_ALIGNAS' is defined
        //:    only when the compiler is able to compile code with the
        //:    alignas specifier.
        //
        // Plan:
        //: 1 If 'BSLS_COMPILERFEATURES_SUPPORT_ALIGNAS' is defined then
        //:   compile code that uses the align as specifier.
        //
        // Testing:
        //   BSLS_COMPILERFEATURES_SUPPORT_ALIGNAS
        // --------------------------------------------------------------------

        if (verbose) printf(
                        "\nTESTING 'BSLS_COMPILERFEATURES_SUPPORT_ALIGNAS'"
                        "\n===============================================\n");

#if !defined(BSLS_COMPILERFEATURES_SUPPORT_ALIGNAS)
        if (verbose) printf("Feature not supported in this configuration.\n");
#else
        alignas(8) int foo; (void) foo;
#endif
      } break;
      case 17: {
        // --------------------------------------------------------------------
        // TESTING 'BSLS_COMPILERFEATURES_SUPPORT_VARIADIC_TEMPLATES'
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

        if (verbose) printf(
             "\nTESTING 'BSLS_COMPILERFEATURES_SUPPORT_VARIADIC_TEMPLATES'"
             "\n==========================================================\n");

#if !defined(BSLS_COMPILERFEATURES_SUPPORT_VARIADIC_TEMPLATES)
        if (verbose) printf("Feature not supported in this configuration.\n");
#else
        ASSERT((PackSize<int, char, double, void>::VALUE == 4));
#endif
      } break;
      case 16: {
        // --------------------------------------------------------------------
        // TESTING 'BSLS_COMPILERFEATURES_SUPPORT_STATIC_ASSERT'
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

        if (verbose) printf(
                  "\nTESTING 'BSLS_COMPILERFEATURES_SUPPORT_STATIC_ASSERT'"
                  "\n=====================================================\n");

#if !defined(BSLS_COMPILERFEATURES_SUPPORT_STATIC_ASSERT)
        if (verbose) printf("Feature not supported in this configuration.\n");
#else
        static_assert(true, "static_assert with bool");
        static_assert(1,    "static_assert with int");
#endif
      } break;
      case 15: {
        // --------------------------------------------------------------------
        // TESTING 'BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES'
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

        if (verbose) printf(
              "\nTESTING 'BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES'"
              "\n=========================================================\n");

#if !defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
        if (verbose) printf("Feature not supported in this configuration.\n");
#else
        RvalueTest obj(my_factory<RvalueTest>(RvalueArg()));

        TemplateType<int> x = make_rvalue<TemplateType<int> >();
        x = make_rvalue<TemplateType<int> >();

        Wrapper<int> z{};
        z.test();
#endif
      } break;
      case 14: {
        // --------------------------------------------------------------------
        // TESTING 'BSLS_COMPILERFEATURES_SUPPORT_OVERRIDE'
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

        if (verbose) printf(
                       "\nTESTING 'BSLS_COMPILERFEATURES_SUPPORT_OVERRIDE'"
                       "\n================================================\n");

#if !defined(BSLS_COMPILERFEATURES_SUPPORT_OVERRIDE)
        if (verbose) printf("Feature not supported in this configuration.\n");
#else
        struct OverrideBase { virtual void f() const {} };
        struct Override: OverrideBase { void f() const override {} };
#endif
      } break;
      case 13: {
        // --------------------------------------------------------------------
        // TESTING 'BSLS_COMPILERFEATURES_SUPPORT_OPERATOR_EXPLICIT'
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

        if (verbose) printf(
              "\nTESTING 'BSLS_COMPILERFEATURES_SUPPORT_OPERATOR_EXPLICIT'"
              "\n=========================================================\n");

#if !defined(BSLS_COMPILERFEATURES_SUPPORT_OPERATOR_EXPLICIT)
        if (verbose) printf("Feature not supported in this configuration.\n");
#else
        struct Explicit {
            static bool match(int)  { return false; }
            static bool match(char) { return true; }

            explicit operator int() const { return 0; }
            operator char() const { return 0; }
        };

        Explicit ex;
        bool result = Explicit::match(ex);
        ASSERT(result);
#endif
      } break;
      case 12: {
        // --------------------------------------------------------------------
        // TESTING 'BSLS_COMPILERFEATURES_SUPPORT_NULLPTR'
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

        if (verbose) printf(
                        "\nTESTING 'BSLS_COMPILERFEATURES_SUPPORT_NULLPTR'"
                        "\n===============================================\n");

#if !defined(BSLS_COMPILERFEATURES_SUPPORT_NULLPTR)
        if (verbose) printf("Feature not supported in this configuration.\n");
#else
        void *p = nullptr;
        if (p == nullptr) {}
        OverloadForNullptr(nullptr);
#endif
      } break;
      case 11:{
        // --------------------------------------------------------------------
        // TESTING 'BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT'
        //
        // Concerns:
        //: 1 'BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT' is defined only when
        //:    the compiler is able to compile code with 'noexcept'.
        //
        // Plan:
        //: 1 If 'BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT' is defined then
        //:   compile code that uses 'noexecpt' in various contexts.
        //
        // Testing:
        //   BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT
        // --------------------------------------------------------------------

        if (verbose) printf(
                       "\nTESTING 'BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT'"
                       "\n================================================\n");

#if !defined (BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
        if (verbose) printf("Feature not supported in this configuration.\n");
#else
        noexceptTest1();
        noexceptTest2();
        notNoexceptTest1();
        notNoexceptTest2();
        ASSERT(noexcept(noexceptTest1()));
        ASSERT(noexcept(noexceptTest2()));
        ASSERT(noexcept(noexceptTest3()));
        ASSERT(false == noexcept(notNoexceptTest1()));
        ASSERT(false == noexcept(notNoexceptTest2()));
#endif
      } break;
      case 10: {
        // --------------------------------------------------------------------
        // TESTING 'BSLS_COMPILERFEATURES_SUPPORT_INCLUDE_NEXT'
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

        if (verbose) printf(
                   "\nTESTING 'BSLS_COMPILERFEATURES_SUPPORT_INCLUDE_NEXT'"
                   "\n====================================================\n");

#if !defined(BSLS_COMPILERFEATURES_SUPPORT_INCLUDE_NEXT)
        if (verbose) printf("Feature not supported in this configuration.\n");
#else
        if (verbose) printf("#include_next is tested at global scope.\n");
#endif
      } break;
      case 9: {
        // --------------------------------------------------------------------
        // TESTING 'BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS'
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

        if (verbose) printf(
       "\nTESTING 'BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS'"
       "\n================================================================\n");

#if !defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
        if (verbose) printf("Feature not supported in this configuration.\n");
#else
        std::initializer_list<int> il = {10,20,30,40,50}; (void) il;

        using namespace initializer_feature_test;
        coupling<object, couple<object, object> > mX;
        mX.use( {object{}, { object{}, object{} } });
#endif
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // TESTING 'BSLS_COMPILERFEATURES_SUPPORT_FINAL'
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

        if (verbose) printf(
                          "\nTESTING 'BSLS_COMPILERFEATURES_SUPPORT_FINAL'"
                          "\n=============================================\n");

#if !defined(BSLS_COMPILERFEATURES_SUPPORT_FINAL)
        if (verbose) printf("Feature not supported in this configuration.\n");
#else
        struct Final final {};
        struct FinalMember { virtual void f() final {} };
#endif

      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TESTING 'BSLS_COMPILERFEATURES_SUPPORT_EXTERN_TEMPLATE'
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

        if (verbose) printf(
                "\nTESTING 'BSLS_COMPILERFEATURES_SUPPORT_EXTERN_TEMPLATE'"
                "\n=======================================================\n");

#if !defined(BSLS_COMPILERFEATURES_SUPPORT_EXTERN_TEMPLATE)
        if (verbose) printf("Feature not supported in this configuration.\n");
#else
        ExternTemplateClass<char> obj; (void) obj;
#endif

      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING 'BSLS_COMPILERFEATURES_SUPPORT_DELETED_FUNCTIONS'
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

        if (verbose) printf(
              "\nTESTING 'BSLS_COMPILERFEATURES_SUPPORT_DELETED_FUNCTIONS'"
              "\n=========================================================\n");

#if !defined(BSLS_COMPILERFEATURES_SUPPORT_DELETED_FUNCTIONS)
        if (verbose) printf("Feature not supported in this configuration.\n");
#else
        ClassWithDeletedOps* p; (void)p;
#endif
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING 'BSLS_COMPILERFEATURES_SUPPORT_DEFAULTED_FUNCTIONS'
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

        if (verbose) printf(
            "\nTESTING 'BSLS_COMPILERFEATURES_SUPPORT_DEFAULTED_FUNCTIONS'"
            "\n===========================================================\n");

#if !defined(BSLS_COMPILERFEATURES_SUPPORT_DEFAULTED_FUNCTIONS)
        if (verbose) printf("Feature not supported in this configuration.\n");
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
      case 4: {
        // --------------------------------------------------------------------
        // TESTING 'BSLS_COMPILERFEATURES_SUPPORT_DECLTYPE'
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

        if (verbose) printf(
                       "\nTESTING 'BSLS_COMPILERFEATURES_SUPPORT_DECLTYPE'"
                       "\n================================================\n");

#if !defined(BSLS_COMPILERFEATURES_SUPPORT_DECLTYPE)
        if (verbose) printf("Feature not supported in this configuration.\n");
#else
        int                               obj1; (void) obj1;
        decltype(obj1)                    obj2; (void) obj2;
        decltype(testFuncForDecltype(10)) obj3; (void) obj3;

        auto maxVal = my_max(short(10), 'a');

        ASSERT(sizeof(maxVal) == sizeof(int));
        ASSERT(maxVal == 'a');
#endif

      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING 'BSLS_COMPILERFEATURES_SUPPORT_CONSTEXPR_RELAXED'
        //
        // Concerns:
        //: 1 'BSLS_COMPILERFEATURES_SUPPORT_CONSTEXPR_RELAXED' is defined only
        //:   when the compiler is actually able to compile code with relaxed
        //:   constexpr functions that may comprise of multiple statements,
        //:   including multiple return statements, and may mutate the state of
        //:   local variables.
        //: 2 When 'BSLS_COMPILERFEATURES_SUPPORT_CONSTEXPR_RELAXED' is defined
        //:   constexpr member functions are not implicitly const.
        //
        // Plan:
        //: 1 If 'BSLS_COMPILERFEATURES_SUPPORT_CONSTEXPR_RELAXED' is defined
        //:   then compile code that uses this feature to define relaxed
        //:   constant expression functions.
        //: 2 If 'BSLS_COMPILERFEATURES_SUPPORT_CONSTEXPR_RELAXED' is defined
        //:   then compile code that uses this feature to define a constexpr
        //:   member function and detect that it is not a const member
        //:   function.  Use expression SFINAE to detect this without invoking
        //:   a compiler error.
        //
        // Testing:
        //   BSLS_COMPILERFEATURES_SUPPORT_CONSTEXPR_RELAXED
        // --------------------------------------------------------------------

        if (verbose) printf(
              "\nTESTING 'BSLS_COMPILERFEATURES_SUPPORT_CONSTEXPR_RELAXED'"
              "\n=========================================================\n");

#if !defined(BSLS_COMPILERFEATURES_SUPPORT_CONSTEXPR_RELAXED)
        if (verbose) printf("Feature not supported in this configuration.\n");
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
        // TESTING 'BSLS_COMPILERFEATURES_SUPPORT_CONSTEXPR'
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

        if (verbose) printf(
                      "\nTESTING 'BSLS_COMPILERFEATURES_SUPPORT_CONSTEXPR'"
                      "\n=================================================\n");

#if !defined(BSLS_COMPILERFEATURES_SUPPORT_CONSTEXPR)
        if (verbose) printf("Feature not supported in this configuration.\n");
#else
        constexpr OracleMiscompile d; // Just declaring 'd' crashes CC 12.4.

        constexpr int v = A(true).m;
        ASSERT(v == 42);

        if (sizeof(Sniffer::test<Feature11>(0)) == sizeof(FalseType)) {
            ASSERT(!"C++11 did not detect original constexpr");
        }
#endif

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // TESTING 'BSLS_COMPILERFEATURES_SUPPORT_ALIAS_TEMPLATES'
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

        if (verbose) printf(
                "\nTESTING 'BSLS_COMPILERFEATURES_SUPPORT_ALIAS_TEMPLATES'"
                "\n=======================================================\n");

#if !defined(BSLS_COMPILERFEATURES_SUPPORT_ALIAS_TEMPLATES)
        if (verbose) printf("Feature not supported in this configuration.\n");
#else
        my_own_int intObj; (void) intObj;
        alias_nontemplate nontemplateObj; (void) nontemplateObj;
        alias_template1<char> templateObj1; (void) templateObj1;
        alias_template2<char> templateObj2; (void) templateObj2;
#endif
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
