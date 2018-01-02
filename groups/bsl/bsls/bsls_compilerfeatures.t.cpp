// bsls_compilerfeatures.t.cpp                                        -*-C++-*-

#include <bsls_compilerfeatures.h>

#include <bsls_bsltestutil.h>

#include <stdio.h>      // 'printf'
#include <stdlib.h>     // 'atoi'
#include <iostream>

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                            * Overview *
// Testing available C++11 language features by trying to compile code that
// uses them.  For example, if 'BSLS_COMPILERFEATURES_SUPPORT_STATIC_ASSERT' is
// defined, then we try to compile code that uses 'static_assert'.  This is a
// purely compile-time test.  If the code compiles than the test succeeds, if
// the code fails to compile than the test fails.  Due to the limitations of
// the testing framework there is no way to turn compile-time failures into
// runtime failures.  Note that we don't intend to test the correctness of the
// implementation of C++ features, but just the fact that features are
// supported.
//-----------------------------------------------------------------------------
// [ 1] BSLS_COMPILERFEATURES_SUPPORT_ALIAS_TEMPLATES
// [ 2] BSLS_COMPILERFEATURES_SUPPORT_CONSTEXPR
// [ 3] BSLS_COMPILERFEATURES_SUPPORT_DECLTYPE
// [ 4] BSLS_COMPILERFEATURES_SUPPORT_DEFAULTED_FUNCTIONS
// [ 5] BSLS_COMPILERFEATURES_SUPPORT_DELETED_FUNCTIONS
// [ 6] BSLS_COMPILERFEATURES_SUPPORT_EXTERN_TEMPLATE
// [ 7] BSLS_COMPILERFEATURES_SUPPORT_FINAL
// [ 8] BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS
// [ 9] BSLS_COMPILERFEATURES_SUPPORT_INCLUDE_NEXT
// [10] BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT
// [11] BSLS_COMPILERFEATURES_SUPPORT_NULLPTR
// [12] BSLS_COMPILERFEATURES_SUPPORT_OPERATOR_EXPLICIT
// [13] BSLS_COMPILERFEATURES_SUPPORT_OVERRIDE
// [14] BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
// [15] BSLS_COMPILERFEATURES_SUPPORT_STATIC_ASSERT
// [16] BSLS_COMPILERFEATURES_SUPPORT_VARIADIC_TEMPLATES
// [17] BSLS_COMPILERFEATURES_SUPPORT_ALIGNAS
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

//=============================================================================
//              SUPPORTING FUNCTIONS AND TYPES USED FOR TESTING
//-----------------------------------------------------------------------------

#if defined(BSLS_COMPILERFEATURES_SUPPORT_ALIAS_TEMPLATES)

namespace {

template <class T, class U>
struct alias_base {};

using my_own_int = int;
using alias_nontemplate = alias_base<int, char>;
template <class T> using alias_template1 = alias_base<T, int>;
template <class T> using alias_template2 = alias_base<char, T>;

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
#endif // BSLS_COMPILERFEATURES_SUPPORT_CONSTEXPR

#if defined(BSLS_COMPILERFEATURES_SUPPORT_DECLTYPE)

// libc++ #define's decltype when it is not provided by the compiler.
// Our test should not depend on this #define.
#if defined(decltype)
#undef decltype
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

template <class Element>
struct Meta {
    typedef Element type;
};

template <class Element>
struct SmartPtrWithSfinaeConstructor {
    // CREATORS
    template <class AnyType, typename Meta<AnyType>::type * = nullptr>
    SmartPtrWithSfinaeConstructor(AnyType *ptr);

    SmartPtrWithSfinaeConstructor(nullptr_t) {}
};

void test_default_template_args() {
    SmartPtrWithSfinaeConstructor<int> x = 0;  // Oracle CC 12.4 asserts
    (void)x;
    (void)test_default_template_args;
}

}
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
template <class T>
class ExternTemplateClass {};

// don't instantiate in this translation unit
extern template class ExternTemplateClass<char>;

// instantiate in this translation unit
template class ExternTemplateClass<char>;

}  // close unnamed namespace

#endif  // BSLS_COMPILERFEATURES_SUPPORT_EXTERN_TEMPLATE


#if defined(BSLS_COMPILERFEATURES_SUPPORT_INCLUDE_NEXT)

#include_next<cstdio>

#endif  // BSLS_COMPILERFEATURES_SUPPORT_INCLUDE_NEXT


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

}  // close unnamed namespace
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
    (void)OverloadForNullptr;
}
void OverloadForNullptr(void *) {}

}  // close unnamed namespace

#endif  // BSLS_COMPILERFEATURES_SUPPORT_NULLPTR

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)

namespace {

// Check support for the perfect forwaring idiom
//
template <class T>
struct my_remove_reference {
    typedef T type;
};

template <class T>
struct my_remove_reference<T&> {
    typedef T type;
};

template <class T>
struct my_remove_reference<T&&> {
    typedef T type;
};

template <class T>
T&& my_forward(typename my_remove_reference<T>::type& t)
{
    return static_cast<T&&>(t);
}

template <class T>
T&& my_forward(typename my_remove_reference<T>::type&& t)
{
    return static_cast<T&&>(t);
}

template <class T>
typename my_remove_reference<T>::type&& my_move(T&& t)
{
    return static_cast<typename my_remove_reference<T>::type&&>(t);
}

template <class T, class Arg>
T my_factory(Arg&& arg)
{
    return my_move(T(my_forward<Arg>(arg)));
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
    // The class template 'MovableRef_Helper' just defines a nested type
    // 'type' that is used by an alias template.  Using this indirection the
    // template argument of the alias template is prevented from being deduced.
    using type = TYPE&&;
        // The type 'type' defined to be an r-value reference to the argument
        // type of 'MovableRef_Helper.
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

}  // close unnamed namespace

#endif  // BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES

#if defined(BSLS_COMPILERFEATURES_SUPPORT_VARIADIC_TEMPLATES)

namespace {

template <class... Types>
struct PackSize;

template <class Head, class... Tail>
struct PackSize<Head, Tail...> {
    enum { VALUE = 1 + PackSize<Tail...>::VALUE };
};

template <class T>
struct PackSize<T> {
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
    // This function should deduce 'TYPE' from the pointer, perfectly match
    // the second argument.
}

void test_func() {
    int x = 0;
    func(&x, 0);  // This line will be ambiguous on buggy compilers
    (void)test_func;
}

}  // close unnamed namespace
#endif  // BSLS_COMPILERFEATURES_SUPPORT_VARIADIC_TEMPLATES

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

    switch (test) { case 0:
      case 17: {
        // --------------------------------------------------------------------
        // TESTING BSLS_COMPILERFEATURES_SUPPORT_ALIGNAS
        //
        // Concerns:
        //: 1 'BSLS_COMPILERFEATURES_SUPPORT_ALIGNAS' is defined
        //:    only when the compiler is able to compile code with the
        //:    alignas specifier.
        //
        // Plan:
        //: 1 If 'BSLS_COMPILERFEATURES_SUPPORT_ALIGNAS' is defined
        //:   then compile code that uses the align as specifier.
        //
        // Testing:
        //   BSLS_COMPILERFEATURES_SUPPORT_ALIGNAS
        // --------------------------------------------------------------------

#if !defined(BSLS_COMPILERFEATURES_SUPPORT_ALIGNAS)
        if (verbose) printf("Testing 'alignas' skipped\n"
                            "===========================\n");
#else
        if (verbose) printf("Testing 'alignas' specifier\n"
                            "====================================\n");

        alignas(8) int foo; (void) foo;
#endif
      } break;
      case 16: {
        // --------------------------------------------------------------------
        // TESTING BSLS_COMPILERFEATURES_SUPPORT_VARIADIC_TEMPLATES
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

#if !defined(BSLS_COMPILERFEATURES_SUPPORT_VARIADIC_TEMPLATES)
        if (verbose) printf("Testing variadic template parameters skipped\n"
                            "============================================\n");
#else
        if (verbose) printf("Testing variadic template parameters\n"
                            "====================================\n");

        ASSERT((PackSize<int, char, double, void>::VALUE == 4));
#endif
      } break;
      case 15: {
        // --------------------------------------------------------------------
        // TESTING BSLS_COMPILERFEATURES_SUPPORT_STATIC_ASSERT
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

#if !defined(BSLS_COMPILERFEATURES_SUPPORT_STATIC_ASSERT)
        if (verbose) printf("Testing static_assert skipped\n"
                            "=============================\n");
#else
        if (verbose) printf("Testing static_assert\n"
                            "=====================\n");

        static_assert(true, "static_assert with bool");
        static_assert(1,    "static_assert with int");
#endif
      } break;
      case 14: {
        // --------------------------------------------------------------------
        // TESTING BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
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

#if !defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
        if (verbose) printf("Testing rvalue references skipped\n"
                            "=================================\n");
#else
        if (verbose) printf("Testing rvalue references\n"
                            "=========================\n");

        RvalueTest obj(my_factory<RvalueTest>(RvalueArg()));

        TemplateType<int> x = make_rvalue<TemplateType<int> >();
        x = make_rvalue<TemplateType<int> >();

        Wrapper<int> z{};
        z.test();
#endif
      } break;
      case 13: {
        // --------------------------------------------------------------------
        // TESTING BSLS_COMPILERFEATURES_SUPPORT_OVERRIDE
        //
        // Concerns:
        //: 1 'BSLS_COMPILERFEATURES_SUPPORT_OVERRIDE' is defined only when the
        //:    compiler supports using the 'override' keyword when overriding a
        //:    'virtual' function.
        //
        // Plan:
        //: 1 If 'BSLS_COMPILERFEATURES_SUPPORT_OVERRIDE' is defined then
        //:   compile code using the 'override' keyword when overriding a
        //:   'virtual' function. To really test the feature works a function
        //:   that isn't an override needs to use the 'override' keyword to
        //:   produce a compile-time error.
        //
        // Testing:
        //   BSLS_COMPILERFEATURES_SUPPORT_OVERRIDE
        // --------------------------------------------------------------------

#if !defined(BSLS_COMPILERFEATURES_SUPPORT_OVERRIDE)
        if (verbose) printf("Testing override skipped\n"
                            "========================\n");
#else
        if (verbose) printf("Testing override\n"
                            "================\n");

        struct OverrideBase { virtual void f() const {} };
        struct Override: OverrideBase { void f() const override {} };
#endif
      } break;
      case 12: {
        // --------------------------------------------------------------------
        // TESTING BSLS_COMPILERFEATURES_SUPPORT_OPERATOR_EXPLICIT
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

#if !defined(BSLS_COMPILERFEATURES_SUPPORT_OPERATOR_EXPLICIT)
        if (verbose) printf("Testing operator explicit skipped\n"
                            "=================================\n");
#else
        if (verbose) printf("Testing operator explicit\n"
                            "=========================\n");

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
      case 11: {
        // --------------------------------------------------------------------
        // TESTING BSLS_COMPILERFEATURES_SUPPORT_NULLPTR
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

#if !defined(BSLS_COMPILERFEATURES_SUPPORT_NULLPTR)
        if (verbose) printf("Testing nullptr skipped\n"
                            "=======================\n");
#else
        if (verbose) printf("Testing nullptr\n"
                            "===============\n");

        void *p = nullptr;
        if (p == nullptr) {}
        OverloadForNullptr(nullptr);
#endif
      } break;
      case 10:{
        // --------------------------------------------------------------------
        // TESTING BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT
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
#if !defined (BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
        if (verbose) printf("Testing noexcept skipped\n"
                              "========================\n");
#else
        if (verbose) printf("Testing noexcept\n"
                              "================\n");
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
      case 9: {
        // --------------------------------------------------------------------
        // TESTING BSLS_COMPILERFEATURES_SUPPORT_INCLUDE_NEXT
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

#if !defined(BSLS_COMPILERFEATURES_SUPPORT_INCLUDE_NEXT)
        if (verbose) printf("Testing include_next skipped\n"
                            "============================\n");
#else
        if (verbose) printf("Testing include_next\n"
                            "====================\n");
#endif
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // TESTING BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS
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

#if !defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
        if (verbose) printf("Testing generalized initializers skipped\n"
                            "============================\n");
#else
        if (verbose) printf("Testing generalized initializers\n"
                            "================================\n");
        std::initializer_list<int> il = {10,20,30,40,50}; (void) il;

        using namespace initializer_feature_test;
        coupling<object, couple<object, object> > mX;
        mX.use( {object{}, { object{}, object{} } });
#endif
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TESTING BSLS_COMPILERFEATURES_SUPPORT_FINAL
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

#if !defined(BSLS_COMPILERFEATURES_SUPPORT_FINAL)
        if (verbose) printf("Testing final skipped\n"
                            "=====================\n");
#else
        if (verbose) printf("Testing final\n"
                            "=============\n");

        struct Final final {};
        struct FinalMember { virtual void f() final {} };
#endif

      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING BSLS_COMPILERFEATURES_SUPPORT_EXTERN_TEMPLATE
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

#if !defined(BSLS_COMPILERFEATURES_SUPPORT_EXTERN_TEMPLATE)
        if (verbose) printf("Testing extern template skipped\n"
                            "===============================\n");
#else
        if (verbose) printf("Testing extern template\n"
                            "=======================\n");

        ExternTemplateClass<char> obj; (void) obj;
#endif

      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING BSLS_COMPILERFEATURES_SUPPORT_DELETED_FUNCTIONS
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

#if !defined(BSLS_COMPILERFEATURES_SUPPORT_DELETED_FUNCTIONS)
        if (verbose) printf("Testing deleted functions skipped\n"
                            "=================================\n");
#else
        if (verbose) printf("Testing deleted functions template\n"
                            "==================================\n");
        ClassWithDeletedOps* p; (void)p;
#endif
      }break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING BSLS_COMPILERFEATURES_SUPPORT_DEFAULTED_FUNCTIONS
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

#if !defined(BSLS_COMPILERFEATURES_SUPPORT_DEFAULTED_FUNCTIONS)
        if (verbose) printf("Testing defaulted functions skipped\n"
                            "=================================\n");
#else
        if (verbose) printf("Testing defaulted functions template\n"
              "==================================\n");

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
      case 3: {
        // --------------------------------------------------------------------
        // TESTING BSLS_COMPILERFEATURES_SUPPORT_DECLTYPE
        //
        // Concerns:
        //: 1 'BSLS_COMPILERFEATURES_SUPPORT_DECLTYPE' is defined only when
        //    the compiler is actually able to compile code with decltype.
        //
        // Plan:
        //: 1 If 'BSLS_COMPILERFEATURES_SUPPORT_DECLTYPE' is defined then
        //:   compile code that uses this feature to define variables of type
        //:   inferred from decltype.
        //
        // Testing:
        //   BSLS_COMPILERFEATURES_SUPPORT_DECLTYPE
        // --------------------------------------------------------------------

#if !defined(BSLS_COMPILERFEATURES_SUPPORT_DECLTYPE)
        if (verbose) printf("Testing decltype skipped\n"
                            "========================\n");
#else
        if (verbose) printf("Testing decltype\n"
                            "================\n");

        int obj1; (void) obj1;
        decltype(obj1) obj2; (void) obj2;
        decltype(testFuncForDecltype(10)) obj3; (void) obj3;

        auto maxVal = my_max(short(10), 'a');
        ASSERT(sizeof(maxVal) == sizeof(int));
        ASSERT(maxVal == 'a');
#endif

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING BSLS_COMPILERFEATURES_SUPPORT_CONSTEXPR
        //
        // Concerns:
        //: 1 'BSLS_COMPILERFEATURES_SUPPORT_CONSTEXPR' is defined only when
        //    the compiler is actually able to compile code with constexpr.
        //
        // Plan:
        //: 1 If 'BSLS_COMPILERFEATURES_SUPPORT_CONSTEXPR' is defined then
        //:   compile code that uses this feature to define constant
        //:   expressions functions.
        //
        // Testing:
        //   BSLS_COMPILERFEATURES_SUPPORT_CONSTEXPR
        // --------------------------------------------------------------------

#if !defined(BSLS_COMPILERFEATURES_SUPPORT_CONSTEXPR)
        if (verbose) printf("Testing constexpr skipped\n"
                            "=========================\n");
#else
        if (verbose) printf("Testing constexpr\n"
                            "=================\n");

        constexpr OracleMiscompile d; // Just declaring 'd' crashes CC 12.4.

        constexpr int v = A(true).m;
        ASSERT(v == 42);
#endif

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // TESTING BSLS_COMPILERFEATURES_SUPPORT_ALIAS_TEMPLATES
        //
        // Concerns:
        //: 1 'BSLS_COMPILERFEATURES_SUPPORT_ALIAS_TEMPLATES' is defined only
        //    when the compiler is actually able to compile code with alias
        //    templates.
        //
        // Plan:
        //: 1 If 'BSLS_COMPILERFEATURES_SUPPORT_ALIAS_TEMPLATES' is defined
        //:   then compile code that uses this feature to declare both alias
        //:   templates and simple aliases.
        //
        // Testing:
        //   BSLS_COMPILERFEATURES_SUPPORT_ALIAS_TEMPLATES
        // --------------------------------------------------------------------

#if !defined(BSLS_COMPILERFEATURES_SUPPORT_ALIAS_TEMPLATES)
        if (verbose) printf("Testing alias templates skipped\n"
                            "===============================\n");
#else
        if (verbose) printf("Testing alias templates\n"
                            "=======================\n");

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
