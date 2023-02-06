// bslmf_resulttype.h                                                 -*-C++-*-
#ifndef INCLUDED_BSLMF_RESULTTYPE
#define INCLUDED_BSLMF_RESULTTYPE

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide access to 'result_type' or 'ResultType' nested type.
//
//@CLASSES:
//  bslmf::ResultType: metafunction to return result type
//
//@SEE_ALSO:
//
//@DESCRIPTION: A number of functor classes provide a 'typedef' alias for the
// returned type when invoking the functor.  Unfortunately, standard types name
// this 'typedef' 'result_type' whereas BDE types name this 'typedef'
// 'ResultType'.  This component facilitates writing code that depends on these
// 'typedef's by providing a uniform interface, 'bslmf::ResultType', for
// extracting the correct alias.  'bslmf::ResultType<t_FUNC>::type' is
// identical to 't_FUNC::result_type' if such a type exists; otherwise, it is
// identical to 't_FUNC::ResultType' if that type exists; otherwise, it is
// undefined.  A fallback type can be optionally specified such that
// 'bslmf::ResultType<t_FUNC, t_FALLBACK>::type' is identical to 't_FALLBACK'
// if neither 't_FUNC::result_type' nor 't_FUNC::ResultType' is defined.
//
// Note that 'ResultType' checks only for a nested type within its 't_FUNC'
// parameter; it does not attempt to deduce the return type of calling
// 'operator()' the way C++11 'std::result_of' does.
//
///Usage
///-----
// In this section we show intended use of this component.
//
///Usage Example 1
///- - - - - - - -
// In this example, we write a C++03-compatible function template,
// 'wrapInvoke<t_FUNC>(t_A1 a1, t_A2 a2)', that constructs an instance of
// functor 't_FUNC', invokes it with arguments 'a1' and 'a2', and translates
// any thrown exception to a generic exception type.
//
// First, we declare the generic exception type:
//..
//  struct InvocationException { };
//..
// Now, we declare 'wrapInvoke'.  The return type of 'wrapInvoke' should be the
// same as the return type of invoking an object of type 't_FUNC'.  There is no
// non-intrusive way to deduce the return type of 't_FUNC' in C++03.  We
// therefore require that 't_FUNC' provide either a 'result_type' nested type
// (the idiom used in standard library functors) or a 'ResultType' nested type
// (the idiom used in BDE library functors).  We use 'bslmf::ResultType' to
// automatically select the correct idiom:
//..
//  template <class t_FUNC, class t_A1, class t_A2>
//  typename bslmf::ResultType<t_FUNC>::type
//  wrapInvoke(t_A1 a1, t_A2 a2)
//  {
//      t_FUNC f;
//      try {
//          return f(a1, a2);                                         // RETURN
//      }
//      catch (...) {
//          throw InvocationException();
//      }
//  }
//..
// Next, we declare a functor class that compares its arguments, returns the
// string "less" if 'a1 < a2', returns "greater" if 'a2 > a1', and throws an
// exception if neither is true.  The return type of this functor is declared
// using the BDE-style 'ResultType' nested type:
//..
//  struct LessGreater {
//      typedef const char *ResultType;
//      struct BadArgs { };  // exception class
//
//      const char *operator()(long a1, long a2);
//          // For the specified long integers 'a1' and 'a2', return the
//          // null-terminated string "less" if 'a1 < a2' and "greater" if
//          // 'a2 < a1'; otherwise, throw 'BadArgs()'.
//  };
//
//  const char *LessGreater::operator()(long a1, long a2)
//  {
//      if (a1 < a2) {
//          return "less";                                            // RETURN
//      }
//      else if (a2 < a1) {
//          return "greater";                                         // RETURN
//      }
//      else {
//          throw BadArgs();
//      }
//  }
//..
// For comparison, let's also define a 'plus' functor that conforms to the
// C++11 standard definition of 'std::plus':
//..
//  template <class TYPE>
//  struct plus {
//      typedef TYPE first_argument_type;
//      typedef TYPE second_argument_type;
//      typedef TYPE result_type;
//      TYPE operator()(const TYPE& x, const TYPE& y) const { return x + y; }
//  };
//..
// Now, we can use 'wrapInvoke' with our 'LessGreater' functor:
//..
//  int main()
//  {
//      const char *s = wrapInvoke<LessGreater>(5, -2);
//      assert(0 == std::strcmp(s, "greater"));
//..
// Finally, we confirm that we can also use 'wrapInvoke' with the functor
// 'plus<int>':
//..
//      int sum = wrapInvoke<plus<int> >(5, -2);
//      assert(3 == sum);
//
//      return 0;
//  }
//..
///Usage Example 2
///- - - - - - - -
// This example extends the previous one by considering a functor that does not
// declare either 'result_type' or 'ResultType'.  The 'PostIncrement' functor
// performs the operation '*a += b' and returns the old value of '*a':
//..
//  struct PostIncrement {
//      int operator()(int *a, int b)
//      {
//          unsigned tmp = *a;
//          *a += b;
//          return tmp;
//      }
//  };
//..
// Unfortunately, the following innocent-looking code is ill-formed; even
// though the return value is being discarded, the return type of 'wrapInvoke'
// is undefined because 'bslmf::ResultType<PostIncrement>::type' is undefined:
//..
//  // int x = 10;
//  // wrapInvoke<PostIncrement>(x, 2);  // ill-formed
//..
// To make 'wrapInvoke' usable in these situations, we define a new version,
// 'wrapInvoke2', that will fall back to a 'void' return type if neither
// 't_FUNC::result_type' nor 't_FUNC::ResultType' is defined:
//..
//  template <class t_FUNC, class t_A1, class t_A2>
//  typename bslmf::ResultType<t_FUNC, void>::type
//  wrapInvoke2(t_A1 a1, t_A2 a2)
//  {
//      typedef typename bslmf::ResultType<t_FUNC, void>::type RetType;
//      t_FUNC f;
//      try {
//          // C-style cast needed for some compilers
//          return (RetType)f(a1, a2);                                // RETURN
//      }
//      catch (...) {
//          throw InvocationException();
//      }
//  }
//..
// This use of the fallback parameter allows us to use 'bslmf::ResultType' in a
// context where the return type of a function might be ignored:
//..
//  int main()
//  {
//      int x = 10;
//      wrapInvoke2<PostIncrement>(&x, 2);
//      assert(12 == x);
//      return 0;
//  }
//..

#include <bslscm_version.h>

#include <bslmf_nil.h>
#include <bslmf_voidtype.h>

namespace BloombergLP {
namespace bslmf {

template <class t_FUNC, class t_FALLBACK, class = void>
struct ResultType_BdeIdiom;

                        // ================
                        // class ResultType
                        // ================

template <class t_FUNC, class t_FALLBACK = bslmf::Nil, class = void>
struct ResultType : ResultType_BdeIdiom<t_FUNC, t_FALLBACK> {
    // Metafunction to return the result type of the specified functor type
    // 't_FUNC'.  The nested 'type' is identical to 't_FUNC::result_type' if
    // such a type exists; otherwise, it is identical to 't_FUNC::ResultType'
    // if that type exists; otherwise, it is identical to the 't_FALLBACK'
    // template parameter if it was specified; otherwise, it is undefined.
};

template <class t_FUNC, class t_FALLBACK>
struct ResultType<t_FUNC,
                  t_FALLBACK,
                  BSLMF_VOIDTYPE(typename t_FUNC::result_type)> {
    // Specialization of 'ResultType' for when 't_FUNC::result_type' exists,
    // i.e., when the C++ standard idiom for 'result_type' is used.

    typedef typename t_FUNC::result_type type;
};

// ============================================================================
//                TEMPLATE AND INLINE FUNCTION IMPLEMENTATIONS
// ============================================================================

template <class t_FALLBACK>
struct ResultType_Fallback {
    // Metafunction that defines 'type' to be the specified 't_FALLBACK' type
    // unless 't_FALLBACK' is 'bslmf::Nil'.

    typedef t_FALLBACK type;
};

template <>
struct ResultType_Fallback<bslmf::Nil> {
    // Specialization of 'ResultType_Fallback' that does not define 'type' if
    // 'bslmf::Nil' is specified as the fallback parameter.
};

template <class t_FUNC, class t_FALLBACK, class>
struct ResultType_BdeIdiom : ResultType_Fallback<t_FALLBACK> {
    // Metafunction to detect the BDE 'ResultType' idiom as part of the
    // implementation of 'bslmf::ResultType'.  This 'struct' is instantiated
    // when 't_FUNC::result_type' doesn't exist.  This primary template is
    // matched when 't_FUNC::ResultType' also does not exist.  The 't_FALLBACK'
    // type, if any, is produced.
};

template <class t_FUNC, class t_FALLBACK>
struct ResultType_BdeIdiom<t_FUNC,
                           t_FALLBACK,
                           BSLMF_VOIDTYPE(typename t_FUNC::ResultType)> {
    // Metafunction to detect the BDE 'ResultType' idiom as part of the
    // implementation of 'bslmf::ResultType'.  This specialization is
    // instantiated by 'ResultType' only if 't_FUNC::result_type' does not
    // exist and 't_FUNC::ResultType' does exist.

    typedef typename t_FUNC::ResultType type;
};

}  // close package namespace
}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2019 Bloomberg Finance L.P.
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
