// bsla_scanf.h                                                       -*-C++-*-
#ifndef INCLUDED_BSLA_SCANF
#define INCLUDED_BSLA_SCANF

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a macro for checking 'scanf'-style format strings.
//
//@MACROS:
//  BSLA_SCANF(FMTIDX, STARTIDX): validate 'scanf' format and arguments
//  BSLA_SCANF_IS_ACTIVE: 0 if 'BSLA_SCANF' expands to nothing and 1 otherwise
//
//@SEE_ALSO: bsla_annotations
//
//@AUTHOR: Andrew Paprocki (apaprock), Bill Chapman (bchapman2)
//
//@DESCRIPTION: This component provides a preprocessor macro that indicates
// that one of the arguments to a function is a 'scanf'-style format string,
// and that the arguments starting at a certain index are to be checked for
// compatibility with that format string.
//
///Macro Reference
///---------------
//: 'BSLA_SCANF(FMTIDX, STARTIDX)'
//:     This annotation instructs the compiler to perform additional checks on
//:     so-annotated functions that take 'scanf'-style arguments, which should
//:     be type-checked against a format string.
//:
//:     The 'FMTIDX' parameter is the one-based index to the 'const' format
//:     string.  The 'STARTIDX' parameter is the one-based index to the first
//:     variable argument to type-check against that format string.  For
//:     example:
//..
//  extern int my_scanf(void *obj, const char *format, ...) BSLA_SCANF(2, 3);
//..
//
//: 'BSLA_SCANF_IS_ACTIVE'
//:     The macro 'BSLA_SCANF_IS_ACTIVE' is defined to 0 if 'BSLA_SCANF'
//:     expands to nothing and 1 otherwise.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Populate a Sequence of 'int's and 'float's with Random Numbers
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose we want to have a function that will populate a list of 'int's and
// 'float's with random numbers in the range '[ 0 .. 100 )'.
//
// First, we define our function:
//..
//  int populateValues(const char *format, ...) BSLA_SCANF(1, 2);
//      // Use 'rand' to populate 'int's and 'float's, passed by pointer after
//      // the specified 'format', which will specify the types of the
//      // variables passed.  Return the number of variables populated, or -1
//      // if the format string is invalid.
//
//  int populateValues(const char *format, ...)
//  {
//      int ret = 0;
//
//      va_list ap;
//      va_start(ap, format);
//
//      for (const char *pc = format; *pc; ++pc) {
//          if ('%' != *pc) {
//              continue;
//          }
//          const char c = *++pc;
//          if ('%' == c) {
//              continue;
//          }
//          else if ('d' == c) {
//              * va_arg(ap, int *)   = static_cast<unsigned>(rand()) % 100;
//          }
//          else if ('f' == c || 'e' == c || 'g' == c) {
//              const int characteristic = static_cast<unsigned>(rand()) % 100;
//              const int mantissa = static_cast<unsigned>(rand()) % 1000;
//
//              * va_arg(ap, float *) = static_cast<float>(characteristic +
//                                                          mantissa / 1000.0);
//          }
//          else {
//              // Unrecognized character.  Return a negative value.
//
//              ret = -1;
//              break;
//          }
//
//          ++ret;
//      }
//
//      va_end(ap);
//
//      return ret;
//  }
//..
// Then, in 'main', we call 'populateValues' properly:
//..
//      float ff[3] = { 0, 0, 0 };
//      int   ii[3] = { 0, 0, 0 };
//
//      int numVars = populateValues("%d %g %g %d %d %g",
//                             &ii[0], &ff[0], &ff[1], &ii[1], &ii[2], &ff[2]);
//      assert(6 == numVars);
//      for (int jj = 0; jj < 3; ++jj) {
//          assert(0 <= ii[jj]);
//          assert(0 <= ff[jj]);
//          assert(     ii[jj] < 100);
//          assert(     ff[jj] < 100);
//      }
//      printf("%d %g %g %d %d %g\n",
//                                   ii[0], ff[0], ff[1], ii[1], ii[2], ff[2]);
//..
// Next, we observe that there are no compiler warnings and a reasonable set of
// random numbers are output:
//..
//  83 86.777 15.793 35 86 92.649
//..
// Now, we make a call where the arguments don't match the format string:
//..
//      numVars = populateValues("%d %g", &ff[0], &ii[0]);
//..
// Finally, we observe the following compiler warnings with clang:
//..
//  .../bsla_scanf.t.cpp:351:43: warning: format specifies type 'int *' but the
//  argument has type 'float *' [-Wformat]
//      numVars = populateValues("%d %g", &ff[0], &ii[0]);
//                                ~~      ^~~~~~
//                                %f
//  .../bsla_scanf.t.cpp:351:51: warning: format specifies type 'float *' but
//  the argument has type 'int *' [-Wformat]
//      numVars = populateValues("%d %g", &ff[0], &ii[0]);
//                                   ~~           ^~~~~~
//                                   %d
//..

#include <bsls_compilerfeatures.h>
#include <bsls_platform.h>

#if defined(BSLS_PLATFORM_CMP_GNU)   ||                                       \
    defined(BSLS_PLATFORM_CMP_CLANG) ||                                       \
    defined(BSLS_PLATFORM_CMP_HP)
    #define BSLA_SCANF(FMTIDX, STARTIDX)                                      \
                               __attribute__((format(scanf, FMTIDX, STARTIDX)))

    #define BSLA_SCANF_IS_ACTIVE 1
#else
    #define BSLA_SCANF(FMTIDX, STARTIDX)

    #define BSLA_SCANF_IS_ACTIVE 0
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
