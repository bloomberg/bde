// bsla_nodiscard.h                                                   -*-C++-*-
#ifndef INCLUDED_BSLA_NODISCARD
#define INCLUDED_BSLA_NODISCARD

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a macro for warning about ignored function results.
//
//@MACROS:
//  BSLA_NODISCARD: warn if annotated function result is not used
//  BSLA_NODISCARD_IS_ACTIVE: defined if 'BSLA_NODISCARD' is active
//  BSLA_NODISCARD_CPP17: insert C++17 '[[nodiscard]]' if available
//  BSLA_NODISCARD_CPP17_IS_ACTIVE: defined if 'BSLA_NODISCARD_CPP17' is active
//
//@SEE_ALSO: bsla_annotations
//
//@DESCRIPTION: This component provides two preprocessor macros that annotate
// entities that should not be ignored.  Widely supported is annotating a
// function such that a compiler warning will be generated if the return value
// of the function is ignored, which is what 'BSLA_NODISCARD' does.  C++17 and
// later compilers also support annotating types, 'BSLA_NODISCARD_CPP17' is
// defined when such annotation is supported.
//
///Macro Reference
///---------------
//: 'BSLA_NODISCARD':
//:     This annotation causes a warning to be emitted if the caller of a
//:     so-annotated function does not use its return value.  This is useful
//:     for functions where not checking the result is either a security
//:     problem or always a bug, such as with the 'realloc' function.
//:     The annotation cannot be used portably on constructors if C++03 support
//:     is required because the GNU compiler reuses their C (language)
//:     implementation for compatibility and that does not understand that
//:     constructors.  Attempt to apply 'BSLA_NODISCARD' to a constructor will
//:     result in a warning from gcc:
//..
//:     warning: 'warn_unused_result' attribute ignored [-Wattributes]
//:          4 |     BSLA_NODISCARD Type() {}
//..
//:
//: 'BSLA_NODISCARD_IS_ACTIVE':
//:     The macro 'BSLA_NODISCARD_IS_ACTIVE' is defined if 'BSLA_NODISCARD'
//:     expands to something with the desired effect; otherwise
//:     'BSLA_NODISCARD_IS_ACTIVE' is not defined and 'BSLA_NODISCARD' expands
//:     to nothing.
//:
//: 'BSLA_NODISCARD_CPP17':
//:     This annotation can be used on both types and functions.  Due to
//:     differences in compiler parser implementations this macro must be
//:     placed *after* the 'class' (or 'struct') keyword and before the name of
//:     the type; otherwise it might not compile.
//:
//:     o GNU gcc-9 does not fully implement this C++17 standard attribute as
//:       it is ignored on constructors and warns:
//..
//:     warning: 'nodiscard' attribute applied to 'Type::Type()' with void
//:                                                  return type [-Wattributes]
//:     4 | BSLA_NODISCARD Type() {}
//:                      | ^ ~~~
//..
//:     o There is no known use case for marking individual constructors
//:       '[[ nodiscard ]]' instead of the whole type, so it is easy to avoid
//:       portability issues by marking the type itself nodiscard.
//:
//:     o Marking a type with 'BSLA_NODISCARD_CPP17' in effect makes any
//:       function (including constructors) that return such a type by value or
//:       create an object of that type (in case of constructors) behave as if
//:       they were all (individually) marked by 'BSLA_NODISCARD_CPP17'.  This
//:       ability is very useful for guards or any other RAII types where using
//:       the object as part of a discarded-value expression has completely
//:       different behavior than using a (named) variable of it.
//:
//: 'BSLA_NODISCARD_CPP17_IS_ACTIVE':
//:     The macro 'BSLA_NODISCARD_CPP17_IS_ACTIVE' is defined if
//:     'BSLA_NODISCARD_CPP17' expands to something with the desired effect;
//:     otherwise 'BSLA_NODISCARD_CPP17_IS_ACTIVE' is not defined and
//:     'BSLA_NODISCARD_CPP17' expands to nothing.
//
///Usage Examples
///--------------
// This section illustrates intended use of this component.
//
///Example 1: Square Root Function
///- - - - - - - - - - - - - - - -
// First, we define a function, 'newtonsSqrt', which uses Newton's method for
// calculating a square root.  Since the function has no side effects, it
// doesn't make sense to call it and ignore its result, so we annotate it with
// 'BSLA_NODISCARD':
//..
//  BSLA_NODISCARD
//  double newtonsSqrt(double x);
//  double newtonsSqrt(double x)
//      // Take the square root of the specified 'x'.  The behavior is
//      // undefined unless 'x' is positive.
//  {
//      BSLS_ASSERT(x > 0);
//
//      double guess = 1.0;
//      for (int ii = 0; ii < 100; ++ii) {
//          guess = (guess + x / guess) / 2;
//      }
//
//      return guess;
//  }
//..
// Then, in 'main', we call it normally a few times and observe that it works
// with no compiler warnings generated:
//..
//  printf("Square root of 9.0 = %g\n",           newtonsSqrt(9.0));
//  printf("Square root of 0.01 = %g\n",          newtonsSqrt(0.01));
//  printf("Square root of 0.917 * 0.917 = %g\n", newtonsSqrt(0.917 * 0.917));
//..
// Next, we call it and do nothing with the result, which will generate a
// warning:
//..
//  newtonsSqrt(36.0);
//..
// Now, we call it and explicitly void the result, which, with gcc, still won't
// suppress the "unused result" warning:
//..
//  (void) newtonsSqrt(25.0);
//..
// Finally, we observe the compiler warnings from the last 2 calls:
//..
//  .../bsla_nodiscard.t.cpp:289:22: warning: ignoring return value of 'double
//  newtonsSqrt(double)', declared with attribute warn_unused_result
//  [-Wunused-result]
//       newtonsSqrt(36.0);
//                        ^
//  .../bsla_nodiscard.t.cpp:294:29: warning: ignoring return value of 'double
//  newtonsSqrt(double)', declared with attribute warn_unused_result
//  [-Wunused-result]
//       (void) newtonsSqrt(25.0);
//                               ^
//..
//
///Example 2: No-discard Type
/// - - - - - - - - - - - - -
// Suppose we create a guard type that is capable of closing delimiters that we
// have opened while operating on an output stream.  The example uses C I/O and
// string literals for brevity.
//
// First, we define a guard type 'DelimiterGuard0':
//..
//  class DelimGuard0 {
//    private:
//      // DATA
//      const char *d_closingText_p;  // Held, not owned
//
//    public:
//      // CREATORS
//      explicit DelimGuard0(const char *closingText);
//          // Create a delimiter guard that upon its destruction prints the
//          // specified 'closingText' to 'stdout'.  The behavior is undefined
//          // unless 'closingText' outlives the created object.
//
//      ~DelimGuard0();
//          // Print the closing text to the output file, then destroy this
//          // object.
//  };
//..
// Then we can write code that uses the guard properly:
//..
//  void guard0ProperUse()
//  {
//      printf("\n```ruby\n");  DelimGuard0 closeCode("```\n");
//
//      // Suppose long and complicated code with early returns writing some
//      // source code between the delimiters.  Instead we write something
//      // trivial for brevity:
//      printf("puts 'Hello World'\n");
//  }
//..
// Next, we demonstrate that the guard works as intended:
//..
//  guard0ProperUse();  // prints: [\n]```ruby[\n]puts 'Hello World'[\n]```[\n]
//..
// Then, we write code missing the variable name for the guard.  By not giving
// a variable name we turn what should be an automatic (local) variable
// definition into a so-called expression statement: '<expression>;'.
// Expression statements execute an expression for its side effects, then
// destroy all temporaries created in the expression "at the semicolon".  All
// the 'printf' function calls below are expression statements, they just don't
// have any temporaries to destroy.
//..
//  void discardedGuard0()
//  {
//      printf("(");  DelimGuard0(")\n");
//      printf("in-parens");
//  }
//..
// Next, we demonstrate the bug cause by the guard variable name missing:
//..
//  discardedGuard0();  // prints: ()[\n]in-parens
//..
// Then, we add the no-discard annotation to our guard type directly:
//..
//  class BSLA_NODISCARD_CPP17 DelimGuardCpp17 {
//..
// The rest is omitted for brevity.
//
// Next, we can write the buggy code again using the annotated type:
//..
//  void discardedGuardCpp17()
//  {
//      printf("[");  DelimGuardCpp17("]");
//      printf("in-brackets");
//  }
//..
// Finally, we can demonstrate using a C++17 compiler that we get a warning for
// the buggy code:
//..
// .../bsla_nodiscard.t.cpp:LLL:CC: warning: ignoring temporary created by a
//             constructor declared with 'nodiscard' attribute [-Wunused-value]
//     printf("[");  DelimGuardCpp17("]");
//                   ^~~~~~~~~~~~~~~~~~~~
//
// ...\bsla_nodiscard.t.cpp(227,36): warning C4834: discarding return value of
//                                          function with 'nodiscard' attribute
//..

#include <bsls_compilerfeatures.h>
#include <bsls_platform.h>

                       // =============================
                       // Checks for Pre-Defined macros
                       // =============================

#if defined(BSLA_NODISCARD)
#error BSLA_NODISCARD is already defined!
#endif

#if defined(BSLA_NODISCARD_CPP17)
#error BSLA_NODISCARD_CPP17 is already defined!
#endif

#if defined(BSLA_NODISCARD_IS_ACTIVE)
#error BSLA_NODISCARD_IS_ACTIVE is already defined!
#endif

#if defined(BSLA_NODISCARD_CPP17_IS_ACTIVE)
#error BSLA_NODISCARD_CPP17_IS_ACTIVE is already defined!
#endif

                       // =========================
                       // Set macros as appropriate
                       // =========================

#if defined(BSLS_COMPILERFEATURES_SUPPORT_ATTRIBUTE_NODISCARD)
    #define BSLA_NODISCARD           [[ nodiscard ]]
    #define BSLA_NODISCARD_IS_ACTIVE 1

    #define BSLA_NODISCARD_CPP17           BSLA_NODISCARD
    #define BSLA_NODISCARD_CPP17_IS_ACTIVE 1

#elif defined(BSLS_PLATFORM_CMP_GNU) // [[nodiscard]] not supported
    #if BSLS_COMPILERFEATURES_CPLUSPLUS >= 201103L
        #define BSLA_NODISCARD           __attribute__((warn_unused_result))
        #define BSLA_NODISCARD_IS_ACTIVE 1
    #else
        // gcc attribute cannot be disabled using (void) in C++03 mode, so it
        // would create too many false warnings.
        #define BSLA_NODISCARD
    #endif

    #define BSLA_NODISCARD_CPP17

#elif defined(BSLS_PLATFORM_CMP_CLANG) // nodiscard not supported && not g++
    #define BSLA_NODISCARD           __attribute__((warn_unused_result))
    #define BSLA_NODISCARD_IS_ACTIVE 1

    #define BSLA_NODISCARD_CPP17           __attribute__((warn_unused_result))
    #define BSLA_NODISCARD_CPP17_IS_ACTIVE 1

#else   // not (gcc/g++ or clang) && [[nodiscard]] not supported

    #define BSLA_NODISCARD
    #define BSLA_NODISCARD_CPP17

#endif  // not (gcc/g++ or clang) && [[nodiscard]] not supported

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
