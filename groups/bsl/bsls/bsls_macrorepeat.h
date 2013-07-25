// bsls_macrorepeat.h                  -*-C++-*-
#ifndef INCLUDED_BSLS_MACROREPEAT
#define INCLUDED_BSLS_MACROREPEAT

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: repreat a macro invocation with different numeric arguments
//
//@CLASSES:
// BSLS_MACROREPEAT(N, MACRO): Invoke 'MACRO(1) MACRO(2) ... MACRO(N)'
// BSLS_MACROREPEAT_COMMA(N, MACRO): 'N' comma-separated invocations of 'MACRO'
// BSLS_MACROREPEAT_SEP(N, MACRO, S): 'N' invocations of 'MACRO' separated by S
//
//@SEE_ALSO:
//
//@AUTHOR: Pablo Halpern (phalpern)
//
//@DESCRIPTION:
//
///Usage
///-----
// Instantiate and call 'foo<X>()' for 'X' in '0' to '5':
//..
//  int total = 0;
//  template <int V> void foo() { total += V; }
//
//  int main() {
//
//      #define FOO_STMNT(X) foo<X>();  // Semicolon at end of each statement
//      #define NUM_REPS 5
//      BSLS_MACROREPEAT(NUM_REPS, FOO_STMNT)
//      ASSERT(15 == total);
//      return 0;
// }
//..
// Insert the numbers 2, 4, 6 and 8 into a string:
//..
//  #include <cstring>
//
//  int main() {
//      char buffer[20];
//      #define DOUBLE(x) ((x) * 2)
//      std::sprintf(buffer, "%d %d %d %d", BSLS_MACROREPEAT_COMMA(4, DOUBLE));
//      ASSERT(0 == std::strcmp(buffer, "2 4 6 8"));
//      return 0;
//  }
//..
// Compute (at compile time) a mask for 7 bits:
//..
//  #define BITVAL(B) (1 << (B - 1))
//  const unsigned mask = BSLS_MACROREPEAT_SEP(7, BITVAL, |);
//
//  int main() {
//      ASSERT(127 == mask);
//      return 0;
//  }

#define BSLS_MACROREPEAT(N, MACRO) BSLS_MACROREPEAT_SEP(N, MACRO, )
    // Expand to 'N' invocations of 'MACRO(x)', where 'x' in each invocation
    // is the next number in the sequence from '1' to 'N'.  If 'N' is '0',
    // then the expansion is empty.  For example 'BSLS_MACROREPEAT(3, XYZ)'
    // expands to 'XYZ(1) XYZ(2) XYZ(3)'.  The behavior is undefined unless
    // 'N' is a decimal integer (or is a macro that expands to a decimal
    // integer) in the range 0 to 20.  Note that 'MACRO' is typically a macro
    // with one argument, but may also be a function or functor.

#define BSLS_MACROREPEAT_COMMA(N, MACRO) BSLS_MACROREPEAT_C##N(MACRO)
    // Expand to 'N' comma-separated invocations of 'MACRO(x)', where 'x' in
    // each invocation is the next number in the sequence from '1' to 'N'.  If
    // 'N' is '0', then the expansion is empty.  For example
    // 'BSLS_MACROREPEAT_COMMA(3, XYZ)' expands to 'XYZ(1), XYZ(2), XYZ(3)'.
    // The behavior is undefined unless 'N' is a decimal integer (or is a
    // macro that expands to a decimal integer) in the range 0 to 20.  Note
    // that 'MACRO' is typically a macro with one argument, but may also be a
    // function or functor.

#define BSLS_MACROREPEAT_SEP(N, MACRO, S) BSLS_MACROREPEAT_S##N(MACRO, S)
    // Expand to 'N' invocations of 'MACRO(x)' separated by the token sequence
    // 'S', where 'x' in each invocation is the next number in the sequence
    // from '1' to 'N'.  If 'N' is '0', then the expansion is empty.  For
    // example 'BSLS_MACROREPEAT_SEP(3, XYZ, ;)' expands to 'XYZ(1) ; XYZ(2) ;
    // XYZ(3)'.  The behavior is undefined unless 'N' is a decimal integer (or
    // is a macro that expands to a decimal integer) in the range 0 to 20.
    // Note that 'MACRO' is typically a macro with one argument, but may also
    // be a function or functor.

// ============================================================================
//                           IMPLEMENTATION MACROS
// ============================================================================

#define BSLS_MACROREPEAT_C0(MACRO)
#define BSLS_MACROREPEAT_C1(MACRO) MACRO(1)
#define BSLS_MACROREPEAT_C2(MACRO) BSLS_MACROREPEAT_C1(MACRO), MACRO(2)
#define BSLS_MACROREPEAT_C3(MACRO) BSLS_MACROREPEAT_C2(MACRO), MACRO(3)
#define BSLS_MACROREPEAT_C4(MACRO) BSLS_MACROREPEAT_C3(MACRO), MACRO(4)
#define BSLS_MACROREPEAT_C5(MACRO) BSLS_MACROREPEAT_C4(MACRO), MACRO(5)
#define BSLS_MACROREPEAT_C6(MACRO) BSLS_MACROREPEAT_C5(MACRO), MACRO(6)
#define BSLS_MACROREPEAT_C7(MACRO) BSLS_MACROREPEAT_C6(MACRO), MACRO(7)
#define BSLS_MACROREPEAT_C8(MACRO) BSLS_MACROREPEAT_C7(MACRO), MACRO(8)
#define BSLS_MACROREPEAT_C9(MACRO) BSLS_MACROREPEAT_C8(MACRO), MACRO(9)
#define BSLS_MACROREPEAT_C10(MACRO) BSLS_MACROREPEAT_C9(MACRO), MACRO(10)
#define BSLS_MACROREPEAT_C11(MACRO) BSLS_MACROREPEAT_C10(MACRO), MACRO(11)
#define BSLS_MACROREPEAT_C12(MACRO) BSLS_MACROREPEAT_C11(MACRO), MACRO(12)
#define BSLS_MACROREPEAT_C13(MACRO) BSLS_MACROREPEAT_C12(MACRO), MACRO(13)
#define BSLS_MACROREPEAT_C14(MACRO) BSLS_MACROREPEAT_C13(MACRO), MACRO(14)
#define BSLS_MACROREPEAT_C15(MACRO) BSLS_MACROREPEAT_C14(MACRO), MACRO(15)
#define BSLS_MACROREPEAT_C16(MACRO) BSLS_MACROREPEAT_C15(MACRO), MACRO(16)
#define BSLS_MACROREPEAT_C17(MACRO) BSLS_MACROREPEAT_C16(MACRO), MACRO(17)
#define BSLS_MACROREPEAT_C18(MACRO) BSLS_MACROREPEAT_C17(MACRO), MACRO(18)
#define BSLS_MACROREPEAT_C19(MACRO) BSLS_MACROREPEAT_C18(MACRO), MACRO(19)
#define BSLS_MACROREPEAT_C20(MACRO) BSLS_MACROREPEAT_C19(MACRO), MACRO(20)

#define BSLS_MACROREPEAT_S0(MACRO,S)
#define BSLS_MACROREPEAT_S1(MACRO,S) MACRO(1)
#define BSLS_MACROREPEAT_S2(MACRO,S) BSLS_MACROREPEAT_S1(MACRO,S) S MACRO(2)
#define BSLS_MACROREPEAT_S3(MACRO,S) BSLS_MACROREPEAT_S2(MACRO,S) S MACRO(3)
#define BSLS_MACROREPEAT_S4(MACRO,S) BSLS_MACROREPEAT_S3(MACRO,S) S MACRO(4)
#define BSLS_MACROREPEAT_S5(MACRO,S) BSLS_MACROREPEAT_S4(MACRO,S) S MACRO(5)
#define BSLS_MACROREPEAT_S6(MACRO,S) BSLS_MACROREPEAT_S5(MACRO,S) S MACRO(6)
#define BSLS_MACROREPEAT_S7(MACRO,S) BSLS_MACROREPEAT_S6(MACRO,S) S MACRO(7)
#define BSLS_MACROREPEAT_S8(MACRO,S) BSLS_MACROREPEAT_S7(MACRO,S) S MACRO(8)
#define BSLS_MACROREPEAT_S9(MACRO,S) BSLS_MACROREPEAT_S8(MACRO,S) S MACRO(9)
#define BSLS_MACROREPEAT_S10(MACRO,S) BSLS_MACROREPEAT_S9(MACRO,S) S MACRO(10)
#define BSLS_MACROREPEAT_S11(MACRO,S) BSLS_MACROREPEAT_S10(MACRO,S) S MACRO(11)
#define BSLS_MACROREPEAT_S12(MACRO,S) BSLS_MACROREPEAT_S11(MACRO,S) S MACRO(12)
#define BSLS_MACROREPEAT_S13(MACRO,S) BSLS_MACROREPEAT_S12(MACRO,S) S MACRO(13)
#define BSLS_MACROREPEAT_S14(MACRO,S) BSLS_MACROREPEAT_S13(MACRO,S) S MACRO(14)
#define BSLS_MACROREPEAT_S15(MACRO,S) BSLS_MACROREPEAT_S14(MACRO,S) S MACRO(15)
#define BSLS_MACROREPEAT_S16(MACRO,S) BSLS_MACROREPEAT_S15(MACRO,S) S MACRO(16)
#define BSLS_MACROREPEAT_S17(MACRO,S) BSLS_MACROREPEAT_S16(MACRO,S) S MACRO(17)
#define BSLS_MACROREPEAT_S18(MACRO,S) BSLS_MACROREPEAT_S17(MACRO,S) S MACRO(18)
#define BSLS_MACROREPEAT_S19(MACRO,S) BSLS_MACROREPEAT_S18(MACRO,S) S MACRO(19)
#define BSLS_MACROREPEAT_S20(MACRO,S) BSLS_MACROREPEAT_S19(MACRO,S) S MACRO(20)

#endif // ! defined(INCLUDED_BSLS_MACROREPEAT)

// ----------------------------------------------------------------------------
// Copyright (C) 2013 Bloomberg L.P.
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
