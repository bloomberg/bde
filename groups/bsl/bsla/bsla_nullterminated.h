// bsla_nullterminated.h                                              -*-C++-*-
#ifndef INCLUDED_BSLA_NULLTERMINATED
#define INCLUDED_BSLA_NULLTERMINATED

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide macros for use with null-terminated variadic functions.
//
//@MACROS:
//  BSLA_NULLTERMINATED:            warn if last argument is non-'NULL'
//  BSLA_NULLTERMINATEDAT(ARG_IDX): warn if argument at 'ARG_IDX' is non-'NULL'
//  BSLA_NULLTERMINATED_IS_ACTIVE:   1 if 'BSLA_NULLTERMINATED' is active
//  BSLA_NULLTERMINATEDAT_IS_ACTIVE: 1 if 'BSLA_NULLTERMINATEDAT' is active
//
//@SEE_ALSO: bsla_annotations
//
//@AUTHOR: Andrew Paprocki (apaprock), Bill Chapman (bchapman2)
//
//@DESCRIPTION: This component provides preprocessor macros to
// indicate that a variadic function's arguments are terminated by a 'NULL'
// value, or, in the case of 'BSLA_NULLTERMINATEDAT', by a 'NULL' value at a
// certain index.  Note that the terminating 'NULL' must actually be 'NULL';
// passing 0 in it's place will result in a warning.
//
///Macro Reference
///---------------
//: 'BSLA_NULLTERMINATED'
//:     This annotation on a variadic macro indicates that a warning should be
//:     issued unless the last argument to the function is explicitly 'NULL'.
//
//: 'BSLA_NULLTERMINATEDAT(ARG_IDX)'
//:     This annotation on a variadic function indicates that a warning should
//:     be issued unless the argument at 'ARG_IDX' is 'NULL', where 'ARG_IDX'
//:     is the number of arguments from the last, the last argument having
//:     'ARG_IDX == 0'.  Thus, 'BSLA_NULLTERMINATED' is equivalent to
//:     'BSLA_NULLTERMINATEDAT(0)'.
//
//: 'BSLA_NULLTERMINATED_IS_ACTIVE'
//: 'BSLA_NULLTERMINATEDAT_IS_ACTIVE'
//:     In these two cases, 'X_IS_ACTIVE' is defined to 0 if 'X' expands to
//:     nothing and 1 otherwise.
//
///Usage
///-----
//
///Example 1: 'catStrings' function
/// - - - - - - - - - - - - - - - -
// Suppose we want to have a function that, passed a variable length argument
// list of 'const char *' strings terminated by 'NULL', concatenates the
// strings, separated by spaces, into a buffer.
//
// First, we declare and define the function, annotated with
// 'BSLA_NULL_TERMINATED':
//..
//  void catStrings(char *outputBuffer, ...) BSLA_NULLTERMINATED;
//  void catStrings(char *outputBuffer, ...)
//      // The specified 'outputBuffer' is a buffer where the output of this
//      // function is placed.  The specified '...' is a 'NULL'-terminated list
//      // of 'const char *' strings, which are to be copied into
//      // 'outputBuffer', concatenated together and separated by spaces.  The
//      // behavior is undefined unless the '...' is a 'NULL'-terminated list
//      // of 'const char *' arguments.
//  {
//      *outputBuffer = 0;
//
//      va_list ap;
//      va_start(ap, outputBuffer);
//      const char *next;
//      for (bool first = 1; (next = va_arg(ap, const char *)); first = 0) {
//          ::strcat(outputBuffer, first ? "" : " ");
//          ::strcat(outputBuffer, next);
//      }
//      va_end(ap);
//  }
//..
// Then, in 'main', we call 'catStrings' correctly:
//..
//      char buf[1000];
//      catStrings(buf, "Now", "you", "see", "it.", NULL);
//      printf("%s\n", buf);
//..
// which compiles without a warning and produces the output:
//..
//  Now you see it.
//..
// Now, we call 'catStrings" again and forget to add the terminating 'NULL':
//..
//      catStrings(buf, "Now", "you", "don't.");
//      printf("%s\n", buf);
//..
// Finally, we get the compiler warning:
//..
//  .../bsla_nullterminated.t.cpp:412:47: warning: missing sentinel in function
//  call [-Wsentinel]
//      catStrings(buf, "Now", "you", "don't.");
//                                            ^
//                                            , nullptr
//  .../bsla_nullterminated.t.cpp:137:10: note: function has been explicitly
//  marked sentinel here
//  void catStrings(char *outputBuffer, ...)
//       ^
//..
//
///Example 2: 'catVerdict' function
/// - - - - - - - - - - - - - - - -
// Suppose we want to have a function that, passed a variable length argument
// list of 'const char *' strings terminated by 'NULL', concatenates the
// strings, separated by spaces, into a buffer, and then there's an additional
// integer argument, interpreted as a boolean, that determines what is to be
// appended to the end of the buffer.
//
// First, we declare and define the function, annotated with
// 'BSLA_NULL_TERMINATEDAT(1)':
//..
//  void catVerdict(char *outputBuffer, ...) BSLA_NULLTERMINATEDAT(1);
//  void catVerdict(char *outputBuffer, ...)
//      // The specified 'outputBuffer' is a buffer where output is to be
//      // placed.  All but the last 2 of the specified '...' arguments are
//      // 'const char *' strings to be concatenated together into
//      // 'outputBuffer', separated by spaces.  The second-to-last argument is
//      // to be 'NULL', and the last argument is an 'int' interpreted as a
//      // boolean to determine whether the buffer is to end with a verdict of
//      // "guilty" or "not guilty".  The behavior is undefined unless the
//      // types of all the arguments are correct and the second to last
//      // argument is 'NULL'.
//  {
//      *outputBuffer = 0;
//
//      va_list ap;
//      va_start(ap, outputBuffer);
//      const char *next;
//      for (bool first = 1; (next = va_arg(ap, const char *)); first = 0) {
//          ::strcat(outputBuffer, first ? "" : " ");
//          ::strcat(outputBuffer, next);
//      }
//
//      const bool guilty = va_arg(ap, int);
//      ::strcat(outputBuffer, guilty ? ": guilty" : ": not guilty");
//      va_end(ap);
//  }
//..
// Then, in 'main', we call 'catVerdict' correctly:
//..
//      char buf[1000];
//      catVerdict(buf, "We find the", "defendant,", "Bugs Bunny", NULL, 0);
//      printf("%s\n", buf);
//..
// which compiles without a warning and produces the output:
//..
//  We find the defendant, Bugs Bunny: not guilty
//..
// Next, we call 'catVerdict' with no 'NULL' passed, and get a warning (and
// probably a segfault if we ran it):
//..
//      catVerdict(buf, "We find the", "defendant,", "Wile E. Coyote", 1);
//      printf("%s\n", buf);
//..
// And we get the following compiler warning:
//..
//  .../bsla_nullterminated.t.cpp:447:70: warning: missing sentinel in function
//  call [-Wsentinel]
//      catVerdict(buf, "We find the", "defendant,", "Wile E. Coyote", 1);
//                                                                   ^
//                                                                   , nullptr
//  .../bsla_nullterminated.t.cpp:171:10: note: function has been explicitly
//  marked sentinel here
//  void catVerdict(char *outputBuffer, ...)
//       ^
//..
// Now, we call 'catVerdict' and forget to put the integer that indicates guilt
// or innocence after the 'NULL'.  This means that 'NULL' is happening at index
// 0, not index 1, which violates the requirement imposed by the annotation:
//..
//      catVerdict(buf, "We find the", "defendant,", "Road Runner", NULL);
//      printf("%s\n", buf);
//..
// Finally, we get the compiler warning:
//..
//  .../bsla_nullterminated.t.cpp:471:67: warning: missing sentinel in function
//  call [-Wsentinel]
//      catVerdict(buf, "We find the", "defendant,", "Road Runner", NULL);
//                                                                ^
//                                                                , nullptr
//  .../bsla_nullterminated.t.cpp:171:10: note: function has been explicitly
//   marked sentinel here
//  void catVerdict(char *outputBuffer, ...)
//       ^
//..

#include <bsls_platform.h>

#if defined(BSLS_PLATFORM_CMP_GNU) || defined(BSLS_PLATFORM_CMP_CLANG)
    #define BSLA_NULLTERMINATED         __attribute__((__sentinel__))
    #define BSLA_NULLTERMINATEDAT(ARG_IDX)                                    \
                                        __attribute__((__sentinel__(ARG_IDX)))

    #define BSLA_NULLTERMINATED_IS_ACTIVE   1
    #define BSLA_NULLTERMINATEDAT_IS_ACTIVE 1
#else
    #define BSLA_NULLTERMINATED
    #define BSLA_NULLTERMINATEDAT(ARG_IDX)

    #define BSLA_NULLTERMINATED_IS_ACTIVE   0
    #define BSLA_NULLTERMINATEDAT_IS_ACTIVE 0
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
