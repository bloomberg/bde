// bsla_unused.h                                                      -*-C++-*-
#ifndef INCLUDED_BSLA_UNUSED
#define INCLUDED_BSLA_UNUSED

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a macro to suppress 'unused' warnings.
//
//@CLASSES:
//
//@MACROS:
//  BSLA_UNUSED: do not warn if annotated entity is unused
//  BSLA_UNUSED_IS_ACTIVE: 0 if 'BSLA_UNUSED' expands to nothing, else 1
//
//@SEE ALSO: bsla_annotations
//
//@AUTHOR: Andrew Paprocki (apaprock), Bill Chapman (bchapman2)
//
//@DESCRIPTION: This component provides a preprocessor macro that
// will suppress 'unused' warnings on a locally defined function, type, or
// variable that is unused.
//
///Macro Reference
///---------------
//: o BSLA_UNUSED
//:
//: o This annotation indicates that the so-annotated function, variable, or
//:   type is possibly unused and the compiler should not generate a warning
//:   for the unused identifier.
//
//: o BSLA_UNUSED_IS_ACTIVE
//:
//: o The macro 'BSLA_UNUSED_IS_ACTIVE' is defined to 0 if 'BSLA_UNUSED'
//:   expands to nothing and 1 otherwise.
//
///Usage
///-----
//
///Example 1: Unused Warnings:
/// - - - - - - - - - - - - -
// First, we define a namespace 'warn' within the unnamed namespace with a
// type, a function, and a function in it.  They are unused:
//..
//  namespace {
//  namespace warn {
//
//  struct ResultRec {
//      double d_x;
//      double d_y;
//  };
//
//  double x;
//
//  int quadratic(double *zeroA,
//                double *zeroB,
//                double a,
//                double b,
//                double c)
//      // Solve the quadratic function for the specified 'a', 'b', and 'c',
//      // where '0 = a * x^2 + b * x + c'.  If the quadratic has no solutions,
//      // return a non-zero value, and set the specified '*zeroA' and '*zeroB'
//      // to those solutions and return 0 otherwise.
//  {
//      const double discriminant = b * b - 4 * a * c;
//      if (discriminant < 0 || 0.0 == a) {
//          *zeroA = *zeroB = 0.0;
//          return -1;                                                // RETURN
//      }
//
//      const double root = ::sqrt(discriminant);
//      *zeroA = (-b + root) / (2 * a);
//      *zeroB = (-b - root) / (2 * a);
//
//      return 0;
//  }
//
//  }  // close namespace warn
//  }  // close unnamed namespace
//..
// Then, we observe the warnings:
//..
//  .../bsla_unused.t.cpp:130:12: warning: '{anonymous}::warn::x' defined but n
//  ot used [-Wunused-variable]
//       double x;
//              ^
//  .../bsla_unused.t.cpp:132:9: warning: 'int {anonymous}::warn::quadratic(dou
//  ble*, double*, double, double, double)' defined but not used [-Wunused-func
//  tion]
//       int quadratic(double *zeroA,
//           ^
//..
// Note that none of the compilers we are currently using issue a warning on
// the unused 'warn::ResultRec', but some in the future might.
//
// Next, we define a namespace 'nowarn' within the unused namespace with
// exactly the same unused entities, using the 'BSLA_UNUSED' annotation to
// silence the warnings:
//..
//  namespace {
//  namespace nowarn {
//
//  struct ResultRec {
//      double d_x;
//      double d_y;
//  } BSLA_UNUSED;
//
//  double x BSLA_UNUSED;
//
//  int quadratic(double *zeroA,
//                double *zeroB,
//                double a,
//                double b,
//                double c) BSLA_UNUSED;
//      // Solve the quadratic function for the specified 'a', 'b', and 'c',
//      // where '0 = a * x^2 + b * x + c'.  If the quadratic has no solutions,
//      // return a non-zero value, and set the specified '*zeroA' and '*zeroB'
//      // to those solutions and return 0 otherwise.
//
//  int quadratic(double *zeroA,
//                double *zeroB,
//                double a,
//                double b,
//                double c)
//  {
//      const double discriminant = b * b - 4 * a * c;
//      if (discriminant < 0 || 0.0 == a) {
//          *zeroA = *zeroB = 0.0;
//          return -1;                                                // RETURN
//      }
//
//      const double root = ::sqrt(discriminant);
//      *zeroA = (-b + root) / (2 * a);
//      *zeroB = (-b - root) / (2 * a);
//
//      return 0;
//  }
//
//  }  // close namespace nowarn
//  }  // close unnamed namespace
//..
// Finally, we observe that the warnings for the 'nowarn' namespace are
// suppressed.

#include <bsls_platform.h>

// Note that we could conceivably migrate this to use '[[maybe_unused]]' when
// available, but that has more specific constraints over where it can be
// syntactically placed than the older vendor annotations.

#if defined(BSLS_PLATFORM_CMP_GNU) || defined(BSLS_PLATFORM_CMP_CLANG)
    #define BSLA_UNUSED     __attribute__((__unused__))

    #define BSLA_UNUSED_IS_ACTIVE 1
#else
    #define BSLA_UNUSED

    #define BSLA_UNUSED_IS_ACTIVE 0
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
