// bsls_compilerfeatures.t.cpp                                        -*-C++-*-

#include <bsls_compilerfeatures.h>

#include <cstdio>      // 'printf'
#include <cstdlib>     // 'atoi'
#include <iostream>

using namespace BloombergLP;
using namespace std;

//=============================================================================
//                  STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------

namespace {

int testStatus = 0;

void aSsErT(int c, const char *s, int i)
{
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}

}  // close unnamed namespace

#define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

//=============================================================================
//              SUPPORTING FUNCTIONS AND TYPES USED FOR TESTING
//-----------------------------------------------------------------------------

#if defined(BSLS_COMPILERFEATURES_SUPPORT_ALIAS_TEMPLATES)

namespace {

template <typename T, typename U>
struct alias_base {};

using my_own_int = int;
using alias_nontemplate = alias_base<int, char>;
template <typename T> using alias_template1 = alias_base<T, int>;
template <typename T> using alias_template2 = alias_base<char, T>;

}  // close unnamed namespace

#endif  // BSLS_COMPILERFEATURES_SUPPORT_ALIAS_TEMPLATES

#if defined(BSLS_COMPILERFEATURES_SUPPORT_DECLTYPE)

namespace {

char testFuncForDecltype(int);

template <typename T, typename U>
auto my_max(T t, U u) -> decltype(t > u ? t : u)
{
    return t > u ? t : u;
}

}  // close unnamed namespace

#endif  // BSLS_COMPILERFEATURES_SUPPORT_DECLTYPE

#if defined(BSLS_COMPILERFEATURES_SUPPORT_EXTERN_TEMPLATE)

namespace {

// define class template
template <typename T>
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

#if defined(BSLS_COMPILERFEATURES_SUPPORT_NULLPTR)

namespace {

void OverloadForNullptr(int) {}
void OverloadForNullptr(void *) {}

}  // close unnamed namespace

#endif  // BSLS_COMPILERFEATURES_SUPPORT_NULLPTR

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)

namespace {

template <typename T>
struct my_remove_reference {
    typedef T type;
};

template <typename T>
struct my_remove_reference<T&> {
    typedef T type;
};

template <typename T>
struct my_remove_reference<T&&> {
    typedef T type;
};

template <typename T>
T&& my_forward(typename my_remove_reference<T>::type& t)
{
    return static_cast<T&&>(t);
}

template <typename T>
T&& my_forward(typename my_remove_reference<T>::type&& t)
{
    return static_cast<T&&>(t);
}

template <typename T>
typename my_remove_reference<T>::type&& my_move(T&& t)
{
    return static_cast<typename my_remove_reference<T>::type&&>(t);
}

template <typename T, typename Arg>
T my_factory(Arg&& arg)
{
    return my_move(T(my_forward<Arg>(arg)));
}

struct RvalueArg {};

struct RvalueTest {
    RvalueTest(RvalueArg const &) {}
};

}  // close unnamed namespace

#endif  // BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES

#if defined(BSLS_COMPILERFEATURES_SUPPORT_VARIADIC_TEMPLATES)

namespace {

template <typename... Types>
struct PackSize;

template <typename Head, typename... Tail>
struct PackSize<Head, Tail...> {
    enum { VALUE = 1 + PackSize<Tail...>::VALUE };
};

template <typename T>
struct PackSize<T> {
    enum { VALUE = 1 };
};

}  // close unnamed namespace

#endif  // BSLS_COMPILERFEATURES_SUPPORT_VARIADIC_TEMPLATES

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
// [ 2] BSLS_COMPILERFEATURES_SUPPORT_DECLTYPE
// [ 3] BSLS_COMPILERFEATURES_SUPPORT_EXTERN_TEMPLATE
// [ 4] BSLS_COMPILERFEATURES_SUPPORT_INCLUDE_NEXT
// [ 5] BSLS_COMPILERFEATURES_SUPPORT_NULLPTR
// [ 6] BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
// [ 7] BSLS_COMPILERFEATURES_SUPPORT_STATIC_ASSERT
// [ 8] BSLS_COMPILERFEATURES_SUPPORT_VARIADIC_TEMPLATES
//=============================================================================

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    // int veryVerbose = argc > 3;
    // int veryVeryVerbose = argc > 4;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:
      case 8: {
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
      case 7: {
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
      case 6: {
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
#endif
      } break;
      case 5: {
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
      case 4: {
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
      case 3: {
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
      case 2: {
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
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        cerr << "Error, non-zero test status = " << testStatus << "." << endl;
    }
    return testStatus;
}

// ----------------------------------------------------------------------------
// Copyright (C) 2012 Bloomberg L.P.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
// ----------------------------- END-OF-FILE ----------------------------------
