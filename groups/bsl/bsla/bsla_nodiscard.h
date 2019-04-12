// bsla_nodiscard.h                                                   -*-C++-*-
#ifndef INCLUDED_BSLA_NODISCARD
#define INCLUDED_BSLA_NODISCARD

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a macro for warning about ignored function results.
//
//@MACROS:
//  BSLA_NODISCARD: warn if annotated function result is not used
//  BSLA_NODISCARD_IS_ACTIVE: 1 if 'BSLA_NODISCARD' is active and 0 otherwise
//
//@SEE_ALSO: bsla_annotations
//
//@AUTHOR: Andrew Paprocki (apaprock), Bill Chapman (bchapman2)
//
//@DESCRIPTION: This component provides a preprocessor macro that annotates a
// function such that a compiler warning will be generated if the return value
// of the function is ignored.
//
///Macro Reference
///---------------
//: 'BSLA_NODISCARD'
//:     This annotation causes a warning to be emitted if the caller of a
//:     so-annotated function does not use its return value.  This is useful
//:     for functions where not checking the result is either a security
//:     problem or always a bug, such as with the 'realloc' function.
//
//: 'BSLA_NODISCARD_IS_ACTIVE'
//:     The macro 'BSLA_NODISCARD_IS_ACTIVE' is defined to 0 if
//:     'BSLA_NODISCARD' expands to nothing and 1 otherwise.
//
///Usage
///-----
//
///Example 1: Square Root Function
///- - - - - - - - - - - - - - - -
// First, we define a function, 'newtonsSqrt', which uses Newton's method for
// calculating a square root.  Since the function has no side effects, it
// doesn't make sense to call it and ignore its result, so we annotate it with
// 'BSLA_NODISCARD':
//..
//  double newtonsSqrt(double x) BSLA_NODISCARD;
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

#include <bsls_compilerfeatures.h>
#include <bsls_platform.h>

#if defined(BSLS_COMPILERFEATURES_SUPPORT_ATTRIBUTE_NODISCARD)
    #define BSLA_NODISCARD [[ nodiscard ]]

    #define BSLA_NODISCARD_IS_ACTIVE 1
#elif defined(BSLS_PLATFORM_CMP_GNU)
    #define BSLA_NODISCARD __attribute__((warn_unused_result))

    #define BSLA_NODISCARD_IS_ACTIVE 1
#else
    #define BSLA_NODISCARD

    #define BSLA_NODISCARD_IS_ACTIVE 0
#endif

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
