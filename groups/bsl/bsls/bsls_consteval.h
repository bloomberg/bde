// bsls_consteval.h                                                   -*-C++-*-
#ifndef INCLUDED_BSLS_CONSTEVAL
#define INCLUDED_BSLS_CONSTEVAL

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide macros related to compile-time evaluation.
//
//@MACROS:
//  BSLS_CONSTEVAL_IS_CONSTANT_EVALUATED: 'true' during constant evaluation
//  BSLS_CONSTEVAL_IS_CONSTANT_EVALUATED_IS_ACTIVE: undefined if inactive
//  BSLS_CONSTEVAL_CONSTEXPR: 'constexpr' if 'IS_ACTIVE' defined
//  BSLS_CONSTEVAL_CONSTEXPR_MEMBER: 'constexpr' if 'IS_ACTIVE', else 'const'
//
//@DESCRIPTION: This component provides preprocessor macros that, when
// possible, will identify if a function is being evaluated at compile time.
// This enables branching to avoid the use of constructs that would not be
// valid to evaluate at compile time.  When available, the
// 'std::is_constant_evaluated()' function will be used.  On some platforms
// where that is unavailable a compiler intrinsic will be used instead.
// Finally, when no option for this functionality is available the
// 'BSLS_CONSTEVAL_IS_CONSTANT_EVALUATED' macro will expand to nothing and
// 'BSLS_CONSTEVAL_IS_CONSTANT_EVALUATED_IS_ACTIVE' will not be defined.  To
// ease writing declarations of functions where there is conditionally
// available compile-time behavior, 'BSLS_CONSTEVAL_CONSTEXPR' is defined to be
// equivalent to 'constexpr' if
// 'BSLS_CONSTEVAL_IS_CONSTANT_EVALUATED_IS_ACTIVE' is defined.
//
///Macro Reference
///---------------
// This section documents the preprocessor macros defined in this component.
//
//: 'BSLS_CONSTEVAL_IS_CONSTANT_EVALUATED'
//:     This macro expands to 'std::is_constant_evaluated' when it is
//:     available, or to a compiler intrinsic with equivalent functionality if
//:     available, else, it expands to nothing.
//
//: 'BSLS_CONSTEVAL_IS_CONSTANT_EVALUATED_IS_ACTIVE'
//:     This macro is defined to 1 if 'BSLS_CONSTEVAL_IS_CONSTANT_EVALUATED'
//:     can be used to identify compile-time evaluation, and is undefined
//:     otherwise.
//
//: 'BSLS_CONSTEVAL_CONSTEXPR'
//:     This macro is defined to be 'constexpr' if
//:     'BSLS_CONSTEVAL_IS_CONSTANT_EVALUATED_IS_ACTIVE' is defined, otherwise
//:     it is defined as empty.
//
//: 'BSLS_CONSTEVAL_CONSTEXPR_MEMBER'
//:      This macro is defined to be 'constexpr' if
//:      'BSLS_CONSTEVAL_IS_CONSTANT_EVALUATED_IS_ACTIVE' is defined;
//:      otherwise it is 'const'.  This should be applied to variables and
//:      member variables that should be initialized by a function that is
//:      'BSLS_CONSTEVAL_CONSTEXPR'.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: BSLS_CONSTEVAL_CONSTEXPR 'compute' with output
///---------------------------------------------------------
// In this simple example, the macros are used to determine when it is
// permissible to log a message to 'stdout'.
//..
//  BSLS_CONSTEVAL_CONSTEXPR int compute()
//      // Return '23' if the invocation is evaluated at compile time and that
//      // is detectable, otherwise print a diagnostic message to 'stdout' and
//      // return '17'.
//  {
//  #ifdef BSLS_CONSTEVAL_IS_CONSTANT_EVALUATED_IS_ACTIVE
//      if (BSLS_CONSTEVAL_IS_CONSTANT_EVALUATED) {
//          return 23;                                                // RETURN
//      }
//  #endif
//      printf("Computing value\n");
//      return 17;                                                    // RETURN
//  }
//..
// Now we define a 'test1' function to invoke 'compute' in different contexts.
// This function can be evaluated both at runtime and at compile time without
// errors.  Below, this function is evaluated in both cases, and the difference
// in behavior is observed.
//..
//  void test1()
//      // Invoke 'compute' in both a const and non-const initialization,
//      // verifying the expected results.
//  {
//                                      int i = compute();
//      BSLS_CONSTEVAL_CONSTEXPR_MEMBER int j = compute();
//  #ifdef BSLS_CONSTEVAL_IS_CONSTANT_EVALUATED_IS_ACTIVE
//      ASSERT(17 == i);
//      ASSERT(23 == j);
//  #else
//      ASSERT(17 == i);
//      ASSERT(17 == j);
//  #endif
//  }
//..
// When '17 == i' or '17 == j', 'compute' will write "Computing value/n" to
// 'stdout'.  So, this message will be written once or twice depending on
// whether 'BSLS_CONSTEVAL_IS_CONSTANT_EVALUATED_IS_ACTIVE' is defined.  The
// variable 'j' will always be 'const', and on platforms where the 'compute'
// function supports being 'constexpr', i.e. those where it can use
// 'BSLS_CONSTEVAL_IS_CONSTANT_EVALUATED' to suppress its use of standard
// output, 'j' will also be 'constexpr' and its value will be computed at
// compile time.
//
///Example 2: Evolving a 'constexpr' function
///------------------------------------------
// Consider the situation where there exist two implementations of the same
// algorithm, one of which satisfies the (stringent and pessimizing)
// requirements needed to be a C++11 'constexpr' function, the other of which
// takes advantage of runtime optimizations (such as hardware acceleration,
// exceptions, or non-'constexpr' third-party libraries) that are not available
// at compile time on any platform:
//..
//  int runtimeCompute(int input);
//  BSLS_KEYWORD_CONSTEXPR int compiletimeCompute(int input);
//      // Return a complicated computed value based on the specified 'input'.
//..
// Assuming these functions were introduced long ago, it is likely they are
// heavily used wherever valid throughout a codebase.  The 'compiletimeCompute'
// function is likely used to initialize many 'const' and 'constexpr'
// variables, but also potentially used in many runtime-only expressions, or as
// part of other 'constexpr' functions that are themselves sometimes used at
// runtime.  The 'runtimeCompute' function, similarly, is likely used in many
// contexts that could become 'constexpr' or be evaluated at compile time, but
// is hindering that due to itself not being 'constexpr'.
//
// We can begin to transform 'compiletimeCompute' and 'runtimeCompute' to both
// have improved performance wherever they might be used by moving their
// implementations to separate functions:
//..
//  int runtimeComputeImpl(int input);
//      // Return a complicated computed value based on the specified 'input'.
//
//  BSLS_KEYWORD_CONSTEXPR int compiletimeComputeImpl(int input);
//      // Return a complicated computed value based on the specified 'input'.
//..
// Then, for 'compiletimeCompute' we can provide a new implementation that will
// use the better runtime algorithm when possible, while remaining 'constexpr'
// on all of the platforms where it previously was 'constexpr' (i.e., without
// changing its declaration):
//..
//  BSLS_KEYWORD_CONSTEXPR int compiletimeCompute(int input)
//      // Return a complicated computed value based on the specified 'input'.
//  {
//  #ifdef BSLS_CONSTEVAL_IS_CONSTANT_EVALUATED_IS_ACTIVE
//      if (!BSLS_CONSTEVAL_IS_CONSTANT_EVALUATED) {
//          return runtimeComputeImpl(input);                         // RETURN
//      }
//  #endif  // BSLS_CONSTEVAL_IS_CONSTANT_EVALUATED_IS_ACTIVE
//      return compiletimeComputeImpl(input);
//  }
//..
// Now clients using 'compiletimeCompute' at runtime, both within and outside
// of other 'constexpr' functions,  will get the benefits of an improved
// algorithm without any need for change.
//
// Similarly, the implementation of 'runtimeCompute' can be improved to be
// opportunistically 'constexpr' by taking advantage of
// 'BSLS_CONSTEVAL_CONSTEXPR', potentially allowing some already existing
// expressions to be compile time evaluated on more modern platforms:
//..
//  BSLS_CONSTEVAL_CONSTEXPR int runtimeCompute(int input)
//      // Return a complicated computed value based on the specified 'input'.
//  {
//  #ifdef BSLS_CONSTEVAL_IS_CONSTANT_EVALUATED_IS_ACTIVE
//      if (BSLS_CONSTEVAL_IS_CONSTANT_EVALUATED) {
//          return compiletimeComputeImpl(input);                     // RETURN
//      }
//  #endif  // BSLS_CONSTEVAL_IS_CONSTANT_EVALUATED_IS_ACTIVE
//      return runtimeComputeImpl(input);
//  }
//..
// Clients of 'runtimeCompute' can continue to use it at runtime with no
// changes, occasionally getting the benefits of the compile-time algorithm.
// When using 'runtimeCompute' to initialize a variable, the compile-time
// behavior can be forced by annotating the variable with
// 'BSLS_CONSTEVAL_CONSTEXPR_MEMBER'.
//
// With these changes, clients on older platforms can continue to take
// advantage of having optimal algorithms available at both compile time and
// runtime while getting the best available implementation on newer platforms
// that enable the detection of compile-time evaluation.

#include <bsls_compilerfeatures.h>

#ifdef BSLS_COMPILERFEATURES_SUPPORT_TRAITS_HEADER
#include <type_traits>
#endif

#if defined(BSLS_COMPILERFEATURES_SUPPORT_CONSTEXPR_CPP14)
    #if defined(BSLS_COMPILERFEATURES_SUPPORT_IS_CONSTANT_EVALUATED)
        #define BSLS_CONSTEVAL_IS_CONSTANT_EVALUATED                          \
                std::is_constant_evaluated()
        #define BSLS_CONSTEVAL_IS_CONSTANT_EVALUATED_IS_ACTIVE 1
    #elif defined(__has_builtin)
        #if __has_builtin(__builtin_is_constant_evaluated)
            #define BSLS_CONSTEVAL_IS_CONSTANT_EVALUATED                      \
                    __builtin_is_constant_evaluated()
            #define BSLS_CONSTEVAL_IS_CONSTANT_EVALUATED_IS_ACTIVE 1
        #endif // __has_builtin(__builtin_is_constant_evaluated)
    #endif // defined(BSLS_COMPILERFEATURES_SUPPORT_IS_CONSTANT_EVALUATED)
#endif // defined BSLS_COMPILERFEATURES_SUPPORT_CONSTEXPR_CPP14

#if !defined(BSLS_CONSTEVAL_IS_CONSTANT_EVALUATED)
    #define BSLS_CONSTEVAL_IS_CONSTANT_EVALUATED
#endif // !defined(BSLS_CONSTEVAL_IS_CONSTANT_EVALUATED)

#ifdef BSLS_CONSTEVAL_IS_CONSTANT_EVALUATED_IS_ACTIVE
    #define BSLS_CONSTEVAL_CONSTEXPR constexpr
    #define BSLS_CONSTEVAL_CONSTEXPR_MEMBER constexpr
#else
    #define BSLS_CONSTEVAL_CONSTEXPR
    #define BSLS_CONSTEVAL_CONSTEXPR_MEMBER const
#endif

#endif // INCLUDED_BSLS_CONSTEVAL
// ----------------------------------------------------------------------------
// Copyright 2022 Bloomberg Finance L.P.
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
