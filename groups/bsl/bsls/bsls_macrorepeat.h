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
//@DESCRIPTION: This component provides a set of macros that expand to
// multiple repetitions of a user-specified "repetition phrase".  The
// repetition phrase is specified as macro which is invoked multiple times in
// each invocation of a 'BSLS_MACROREPEAT*' macro.  For example:
//..
//  #define FOO(n) foo ## n
//  doit(BSLS_MACROREPEAT_COMMA(5, FOO));
//..
// will expand FOO(n) with arguments 1 through 5, inserting commas beteween
// the expansions, resulting in:
//..
//  doit(foo1, foo2, foo3, foo4, foo5);
//..
// Use of these macros is less error-prone and often more compact than
// manually repeating the specified pattern.  In addition, it is sometimes
// more readable than the cut-and-paste alternative because the reader does
// not need to examine each argument to verify that it forms a linear
// sequence.
//
// Each of these macros can appear within the repetition phrase of another of
// these macros but, because of limitations in the C proprocessor language,
// none of these macros can appear (directly or indirectly) within its own
// repetition phrase.
//
///Usage
///-----
// The following examples demonstrate potential uses of the macros in this
// component.
//
///Example 1: Repeated Template Instantiation
/// - - - - - - - - - - - - - - - - - - - - -
// In this example, we wish to explictly instantiate a template with a
// sequence of integer values.  First, assume a function template 'foo<V>'
// that adds the (compile-time) value 'V' to a global 'total' each time it is
// called:
//..
//  int total = 0;
//  template <int V> void foo() { total += V; }
//..
// Now, if we instantiate and call 'foo<X>()' once for each 'X' in the range
// '2' to '6'.  To do that, we create a macro, 'FOO_STMNT(X)' which
// and calls 'foo<X+1>' (i.e., 'FOO_STMNT(1)' will call 'foo<2>()'). Then
// we invoke 'FOO_STMNT' 5 times with arguments 1, 2, 3, 4, and 5 using the
// 'BSLS_MACROREPEAT' macro:
//..
//  int main() {
//
//      #define FOO_STMNT(X) foo<X+1>();  // Semicolon at end of each statement
//      BSLS_MACROREPEAT(5, FOO_STMNT)
//      assert(20 == total);
//      return 0;
// }
//..
//
///Example 2: Repeated Function Arguments
/// - - - - - - - - - - - - - - - - - - -
// In this example, we supply as series of identical arguments to a function
// invocation, using 'BSLS_MACROREPEAT_COMMA'.  First, assume a function,
// 'fmtQuartet' that takes four integer arguments and formats them into a
// string:
//..
//  #include <cstring>
//  #include <cstdio>
//
//  void fmtQuartet(char *result, int a, int b, int c, int d) {
//      std::sprintf(result, "%d %d %d %d", a, b, c, d);
//  }
//..
// Now we wish to invoke this function, but in a context where the last three
// arguments are always the same as each other.  For this situation we define
// a macro 'X(x)' that ignores its argument and simply expands to an
// unchanging set of tokens. If the repeated argument is named 'i', then the
// expansion of 'X(x)' is simply '(i)':
//..
//  int main() {
//      char buffer[20];
//      int i = 8;
//      #define X(x) (i)
//..
// Finally, we invoke macro 'X(x)' three times within the argument list of
// 'fmtQuart'.  We use 'BSLS_MACROREPEAT_COMMA' for these invocations, as it
// inserts a comma between each repetition:
//..
//      fmtQuartet(buffer, "%d %d %d %d", 7, BSLS_MACROREPEAT_COMMA(3, X));
//      assert(0 == std::strcmp(buffer, "7 8 8 8"));
//      return 0;
//  }
//..
//
///Example 3: Bitmask Computation
/// - - - - - - - - - - - - - - -
// In this example, we Compute (at compile time) a 7-bit mask.  First, we
// defined a macro 'BITVAL' that computes the value of a single bit 'B' in the
// mask:
//..
//  #define BITVAL(B) (1 << (B - 1))
//..
// Then we use the 'BSLS_MACROREPEAT_SEP' to invoke 'BITVAL' 7 times,
// separating the repetitions with the bitwise OR operator:
//..
//  const unsigned mask = BSLS_MACROREPEAT_SEP(7, BITVAL, |);
//
//  int main() {
//      assert(127 == mask);
//      return 0;
//  }
//..

#define BSLS_MACROREPEAT(N, MACRO) BSLS_MACROREPEAT_##N(MACRO)
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

#define BSLS_MACROREPEAT_0(MACRO)
#define BSLS_MACROREPEAT_1(MACRO) MACRO(1)
#define BSLS_MACROREPEAT_2(MACRO) BSLS_MACROREPEAT_1(MACRO) MACRO(2)
#define BSLS_MACROREPEAT_3(MACRO) BSLS_MACROREPEAT_2(MACRO) MACRO(3)
#define BSLS_MACROREPEAT_4(MACRO) BSLS_MACROREPEAT_3(MACRO) MACRO(4)
#define BSLS_MACROREPEAT_5(MACRO) BSLS_MACROREPEAT_4(MACRO) MACRO(5)
#define BSLS_MACROREPEAT_6(MACRO) BSLS_MACROREPEAT_5(MACRO) MACRO(6)
#define BSLS_MACROREPEAT_7(MACRO) BSLS_MACROREPEAT_6(MACRO) MACRO(7)
#define BSLS_MACROREPEAT_8(MACRO) BSLS_MACROREPEAT_7(MACRO) MACRO(8)
#define BSLS_MACROREPEAT_9(MACRO) BSLS_MACROREPEAT_8(MACRO) MACRO(9)
#define BSLS_MACROREPEAT_10(MACRO) BSLS_MACROREPEAT_9(MACRO) MACRO(10)
#define BSLS_MACROREPEAT_11(MACRO) BSLS_MACROREPEAT_10(MACRO) MACRO(11)
#define BSLS_MACROREPEAT_12(MACRO) BSLS_MACROREPEAT_11(MACRO) MACRO(12)
#define BSLS_MACROREPEAT_13(MACRO) BSLS_MACROREPEAT_12(MACRO) MACRO(13)
#define BSLS_MACROREPEAT_14(MACRO) BSLS_MACROREPEAT_13(MACRO) MACRO(14)
#define BSLS_MACROREPEAT_15(MACRO) BSLS_MACROREPEAT_14(MACRO) MACRO(15)
#define BSLS_MACROREPEAT_16(MACRO) BSLS_MACROREPEAT_15(MACRO) MACRO(16)
#define BSLS_MACROREPEAT_17(MACRO) BSLS_MACROREPEAT_16(MACRO) MACRO(17)
#define BSLS_MACROREPEAT_18(MACRO) BSLS_MACROREPEAT_17(MACRO) MACRO(18)
#define BSLS_MACROREPEAT_19(MACRO) BSLS_MACROREPEAT_18(MACRO) MACRO(19)
#define BSLS_MACROREPEAT_20(MACRO) BSLS_MACROREPEAT_19(MACRO) MACRO(20)

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
