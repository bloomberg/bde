// bsls_macrorepeat.t.cpp                  -*-C++-*-

#include "bsls_macrorepeat.h"

#include <cstdio>
#include <cstdlib>

using namespace std;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// This component provides a set of macros that are invoked with different
// repetition counts and different repetition phrases.  Testing these macros
// (and most compile-time constructs in general) requires basically a
// brute-force invocation of these macros with repetition counts of 0 to 10
// and with a few, representative repetition phrases.
//-----------------------------------------------------------------------------
// [2] BSLS_MACROREPEAT(N, MACRO)
// [3] BSLS_MACROREPEAT_COMMA(N, MACRO)
// [4] BSLS_MACROREPEAT_SEP(N, MACRO, S)
// ----------------------------------------------------------------------------
// [1] BREATHING TEST
// [5] USAGE EXAMPLE
//-----------------------------------------------------------------------------

//==========================================================================
//                  STANDARD BDE ASSERT TEST MACRO
//--------------------------------------------------------------------------
// NOTE: THIS IS A LOW-LEVEL COMPONENT AND MAY NOT USE ANY C++ LIBRARY
// FUNCTIONS, INCLUDING IOSTREAMS.

namespace {

int testStatus = 0;

int verbose = 0;
int veryVerbose = 0;
int veryVeryVerbose = 0;

void aSsErT(int c, const char *s, int i) {
    if (c) {
        printf("Error " __FILE__ "(%d): %s    (failed)\n", i, s);
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}

}  // close unnamed namespace

# define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

//=============================================================================
//                  STANDARD BDE LOOP-ASSERT TEST MACROS
//-----------------------------------------------------------------------------
// NOTE: This implementation of LOOP_ASSERT macros must use printf since
//       cout uses new and must not be called during exception testing.

#define LOOP_ASSERT(I,X) { \
    if (!(X)) { printf("%s", #I ": "); dbg_print(I); printf("\n"); \
                fflush(stdout); aSsErT(1, #X, __LINE__); } }

#define LOOP2_ASSERT(I,J,X) { \
    if (!(X)) { printf("%s", #I ": "); dbg_print(I); printf("\t"); \
                printf("%s", #J ": "); dbg_print(J); printf("\n"); \
                fflush(stdout); aSsErT(1, #X, __LINE__); } }

#define LOOP3_ASSERT(I,J,K,X) {                    \
    if (!(X)) { printf("%s", #I ": "); dbg_print(I); printf("\t"); \
                printf("%s", #J ": "); dbg_print(J); printf("\t"); \
                printf("%s", #K ": "); dbg_print(K); printf("\n"); \
                fflush(stdout); aSsErT(1, #X, __LINE__); } }

#define LOOP4_ASSERT(I,J,K,L,X) {                  \
    if (!(X)) { printf("%s", #I ": "); dbg_print(I); printf("\t"); \
                printf("%s", #J ": "); dbg_print(J); printf("\t"); \
                printf("%s", #K ": "); dbg_print(K); printf("\t"); \
                printf("%s", #L ": "); dbg_print(L); printf("\n"); \
                fflush(stdout); aSsErT(1, #X, __LINE__); } }

#define LOOP5_ASSERT(I,J,K,L,M,X) {                \
    if (!(X)) { printf("%s", #I ": "); dbg_print(I); printf("\t"); \
                printf("%s", #J ": "); dbg_print(J); printf("\t"); \
                printf("%s", #K ": "); dbg_print(K); printf("\t"); \
                printf("%s", #L ": "); dbg_print(L); printf("\t"); \
                printf("%s", #M ": "); dbg_print(M); printf("\n"); \
                fflush(stdout); aSsErT(1, #X, __LINE__); } }

//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define Q(X) printf("<| " #X " |>\n");     // Quote identifier literally.
#define P(X) dbg_print(#X " = ", X, "\n")  // Print identifier and value.
#define P_(X) dbg_print(#X " = ", X, ", ") // P(X) without '\n'
#define L_ __LINE__                        // current Line number
#define T_ putchar('\t');                  // Print a tab (w/o newline)

//=============================================================================
//                      GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

// Fundamental-type-specific print functions.
inline void dbg_print(char c) { printf("%c", c); fflush(stdout); }
inline void dbg_print(unsigned char c) { printf("%c", c); fflush(stdout); }
inline void dbg_print(signed char c) { printf("%c", c); fflush(stdout); }
inline void dbg_print(short val) { printf("%hd", val); fflush(stdout); }
inline void dbg_print(unsigned short val) {printf("%hu", val); fflush(stdout);}
inline void dbg_print(int val) { printf("%d", val); fflush(stdout); }
inline void dbg_print(unsigned int val) { printf("%u", val); fflush(stdout); }
inline void dbg_print(long val) { printf("%lu", val); fflush(stdout); }
inline void dbg_print(unsigned long val) { printf("%lu", val); fflush(stdout);}
// inline void dbg_print(Int64 val) { printf("%lld", val); fflush(stdout); }
// inline void dbg_print(Uint64 val) { printf("%llu", val); fflush(stdout); }
inline void dbg_print(float val) { printf("'%f'", val); fflush(stdout); }
inline void dbg_print(double val) { printf("'%f'", val); fflush(stdout); }
inline void dbg_print(const char* s) { printf("\"%s\"", s); fflush(stdout); }

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

enum { VERBOSE_ARG_NUM = 2, VERY_VERBOSE_ARG_NUM, VERY_VERY_VERBOSE_ARG_NUM };

//=============================================================================
//                  USAGE EXAMPLES
//-----------------------------------------------------------------------------

// The following examples demonstrate potential uses of the macros in this
// component.
//
///Usage Example 1: Repeated template instantiation
///- - - - - - - - - - - - - - - - - - - - - - - -
// In this example, we wish to explictly instantiate a template with a
// sequence of integer values.  First, assume a function template 'foo<V>'
// that adds the (compile-time) value 'V' to a global 'total' each time it is
// called:
//..
    int total = 0;
    template <int V> void foo() { total += V; }
//..
// Now, if we instantiate and call 'foo<X>()' once for each 'X' in the range
// '2' to '6'.  To do that, we create a macro, 'FOO_STMNT(X)' which
// and calls 'foo<X+1>' (i.e., 'FOO_STMNT(1)' will call 'foo<2>()'). Then
// we invoke 'FOO_STMNT' 5 times with arguments 1, 2, 3, 4, and 5 using the
// 'BSLS_MACROREPEAT' macro:
//..
    int usageExample1() {

        #define FOO_STMNT(X) foo<X+1>();  // Semicolon at end of each statement
        BSLS_MACROREPEAT(5, FOO_STMNT)
        ASSERT(20 == total);
        return 0;
   }
//..
//
///Usage Example 2: Repeated function arguments
///- - - - - - - - - - - - - - - - - - - - - -
// In this example, we supply as series of identical arguments to a function
// invocation, using 'BSLS_MACROREPEAT_COMMA'.  First, assume a function,
// 'fmtQuartet' that takes four integer arguments and formats them into a
// string:
//..
    #include <cstring>
    #include <cstdio>

    void fmtQuartet(char *result, int a, int b, int c, int d) {
        std::sprintf(result, "%d %d %d %d", a, b, c, d);
    }
//..
// Now we wish to invoke this function, but in a context where the last three
// arguments are always the same as each other.  For this situation we define
// a macro 'X(x)' that ignores its argument and simply expands to an
// unchanging set of tokens. If the repeated argument is named 'i', then the
// expansion of 'X(x)' is simply '(i)':
//..
    int usageExample2() {
        char buffer[20];
        int i = 8;
        #define X(x) (i)
//..
// Finally, we invoke macro 'X(x)' three times within the argument list of
// 'fmtQuart'.  We use 'BSLS_MACROREPEAT_COMMA' for these invocations, as it
// inserts a comma between each repetition:
//..
        fmtQuartet(buffer, 7, BSLS_MACROREPEAT_COMMA(3, X));
        ASSERT(0 == std::strcmp(buffer, "7 8 8 8"));
        return 0;
    }
//..
//
///Usage Example 3: Bitmask computation
///- - - - - - - - - - - - - - - - - -
// In this example, we Compute (at compile time) a 7-bit mask.  First, we
// defined a macro 'BITVAL' that computes the value of a single bit 'B' in the
// mask:
//..
    #define BITVAL(B) (1 << (B - 1))
//..
// Then we use the 'BSLS_MACROREPEAT_SEP' to invoke 'BITVAL' 7 times,
// separating the repetitions with the bitwise OR operator:
//..
    const unsigned mask = BSLS_MACROREPEAT_SEP(7, BITVAL, |);

    int usageExample3() {
        ASSERT(127 == mask);
        return 0;
    }

//=============================================================================
//                              TEST FUNCTIONS
//-----------------------------------------------------------------------------

// Nest all three macros in this component to generate 10 overloads of
// sumArgs, with 1 to 20 arguments.  The return value of each function will
// have the sum of the arguments in low 24 bits, and the count of arguments in
// the next 5 bits.  Thus 'sumArgs(0xa, 0xb)' would return '0x2000015', where
// the first '2' represents the number of arguments passed and the (hex) '15'
// is the sum of 0xa and 0xb.
#define SUMARG_A(n) a ## n
#define SUMARG_INTA(n) int a ## n
#define SUMARG_FUNC(n) int sumArgs(BSLS_MACROREPEAT_COMMA(n, SUMARG_INTA)) { \
        return (n << 24) | (BSLS_MACROREPEAT_SEP(n, SUMARG_A, +));           \
    }
BSLS_MACROREPEAT(20, SUMARG_FUNC)

int sumArgs()
{
    return 0;
}

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    verbose = argc > 2;
    veryVerbose = argc > 3;
    veryVeryVerbose = argc > 4;

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:  // Zero is always the leading case.
      case 5: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //
        // Concerns: The usage example in the header compiles and runs
        //
        // Plan:
        //: o Copy the usage example from the component header.
        //: o Change 'ASSERT' to 'assert' and change 'main' to 'usageExample1',
        //:   'usageExample2' and 'usageExample3'.
        //: o Invoke each usage example function.
	//
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) printf("\nUSAGE EXAMPLE"
                            "\n=============\n");

        usageExample1();
        usageExample2();
        usageExample3();

      } break;

      case 4: {
        // --------------------------------------------------------------------
        // TESTING BSLS_MACROREPEAT_SEP
        //
        // Concerns:
        //: 1 When invoked with a repetition count if 0, the macro expands to
        //:   an empty token stream.
        //: 2 When invoked with a repetition count of 1 to 20, the repetition
        //:   phrase is repeated the specified number of times.
        //: 3 The specified separator token appears appears between repetitions
        //: 4 The repetition phrase is invoked with an integer literal
        //:   argument which indicates the argument number (starting at 1).
        //
        // Plan:
        //: 1 To test concern 1, invoke 'BSLS_MACROREPEAT_SEP' with a repetition
        //:   count if 0 in between two string literals.  Verify that the
        //:   result is the concatenation of the two strings with no
        //:   intervening characters.
        //: 2 For concern 2, invoke 'BSLS_MACROREPEAT_SEP' with each repetition
        //:   count from 1 to 20 using a repetition phrase that ORs bits such
        //:   that the resulting value verifies the actual repetitions
        //:   invoked.
        //: 3 For concern 3, the separator is a '|', which produces a result
        //:   that can be verified.
        //: 4 For concern 4, the repetition phrase constructs the values being
        //:   ORed together by token-concatenation between the token 'val' and
        //:   the integer literal generated by 'BSLS_MACROREPEAT_SEP'.  A set
        //:   of constants, 'val0', 'val1', etc. containg the actual values to
        //:   be ORed together.
        //
        // Testing
        //   BSLS_MACROREPEAT_SEP(N, MACRO, S)
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING BSLS_MACROREPEAT_SEP"
                            "\n============================\n");

        const int val1  = 0x00001;
        const int val2  = 0x00002;
        const int val3  = 0x00004;
        const int val4  = 0x00008;
        const int val5  = 0x00010;
        const int val6  = 0x00020;
        const int val7  = 0x00040;
        const int val8  = 0x00080;
        const int val9  = 0x00100;
        const int val10 = 0x00200;
        const int val11 = 0x00400;
        const int val12 = 0x00800;
        const int val13 = 0x01000;
        const int val14 = 0x02000;
        const int val15 = 0x04000;
        const int val16 = 0x08000;
        const int val17 = 0x10000;
        const int val18 = 0x20000;
        const int val19 = 0x40000;
        const int val20 = 0x80000;

#define ORARG(n) val ## n

        // Test that macro expansion is empty
        const char s[3] = "a" BSLS_MACROREPEAT_SEP(0, ORARG, FOO) "b";
        ASSERT('a' == s[0] && 'b' == s[1] && '\0' == s[2]);

        ASSERT(0x000001 == BSLS_MACROREPEAT_SEP(1 , ORARG, | ));
        ASSERT(0x000003 == BSLS_MACROREPEAT_SEP(2 , ORARG, | ));
        ASSERT(0x000007 == BSLS_MACROREPEAT_SEP(3 , ORARG, | ));
        ASSERT(0x00000f == BSLS_MACROREPEAT_SEP(4 , ORARG, | ));
        ASSERT(0x00001f == BSLS_MACROREPEAT_SEP(5 , ORARG, | ));
        ASSERT(0x00003f == BSLS_MACROREPEAT_SEP(6 , ORARG, | ));
        ASSERT(0x00007f == BSLS_MACROREPEAT_SEP(7 , ORARG, | ));
        ASSERT(0x0000ff == BSLS_MACROREPEAT_SEP(8 , ORARG, | ));
        ASSERT(0x0001ff == BSLS_MACROREPEAT_SEP(9 , ORARG, | ));
        ASSERT(0x0003ff == BSLS_MACROREPEAT_SEP(10, ORARG, | ));
        ASSERT(0x0007ff == BSLS_MACROREPEAT_SEP(11, ORARG, | ));
        ASSERT(0x000fff == BSLS_MACROREPEAT_SEP(12, ORARG, | ));
        ASSERT(0x001fff == BSLS_MACROREPEAT_SEP(13, ORARG, | ));
        ASSERT(0x003fff == BSLS_MACROREPEAT_SEP(14, ORARG, | ));
        ASSERT(0x007fff == BSLS_MACROREPEAT_SEP(15, ORARG, | ));
        ASSERT(0x00ffff == BSLS_MACROREPEAT_SEP(16, ORARG, | ));
        ASSERT(0x01ffff == BSLS_MACROREPEAT_SEP(17, ORARG, | ));
        ASSERT(0x03ffff == BSLS_MACROREPEAT_SEP(18, ORARG, | ));
        ASSERT(0x07ffff == BSLS_MACROREPEAT_SEP(19, ORARG, | ));
        ASSERT(0x0fffff == BSLS_MACROREPEAT_SEP(20, ORARG, | ));

#undef ORARG

      } break;

      case 3: {
        // --------------------------------------------------------------------
        // TESTING BSLS_MACROREPEAT_COMMA
        //
        // Concerns:
        //: 1 When invoked with a repetition count if 0, the macro expands to
        //:   an empty token stream.
        //: 2 When invoked with a repetition count of 1 to 20, the repetition
        //:   phrase is repeated the specified number of times.
        //: 3 A comma is inserted between repetitions.
        //: 4 The repetition phrase is invoked with an integer literal
        //:   argument which indicates the argument number (starting at 1).
        //: 5 'BSLS_MACROREPEAT_COMMA' can be used to generate the formal
        //:   parameter list for a function prototype.
        //: 6 'BSLS_MACROREPEAT_COMMA' can be used to generate the actual
        //:   argument list for a function call.
        //
        // Plan:
        //: 1 To test concern 1, invoke 'BSLS_MACROREPEAT_COMMA' with a
        //:   repetition count if 0 in between two string literals.  Verify
        //:   that the result is the concatenation of the two strings with no
        //:   intervening characters.
        //: 2 For concern 2, invoke 'BSLS_MACROREPEAT_COMMA' with each
        //:   repetition count from 1 to 20 within a function argument list,
        //:   where the function counts the number of repetitions and sums them
        //:   up.
        //: 3 For concerns 3 and 6, the invocation occurs as a function
        //:   argument list, which requires comma-separation.
        //: 4 For concern 4, the repetition phrase constructs the values being
        //:   summed together by token-concatenation between the token 'val'
        //:   and the integer literal generated by 'BSLS_MACROREPEAT_COMMA'.
        //:   A set of constants, 'val0', 'val1', etc. containg the actual
        //:   values to be summed together.
        //: 5 For concern 5, a function 'sumArgs' is generated with 0 to 20
        //:   arguments using 'BSLS_MACROREPEAT_COMMA' to generate the
        //:   argument list.
        //
        // Testing
        //   BSLS_MACROREPEAT_COMMA(N, MACRO)
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING BSLS_MACROREPEAT_COMMA"
                            "\n==============================\n");

        const int val1  = 0x00001;
        const int val2  = 0x00002;
        const int val3  = 0x00004;
        const int val4  = 0x00008;
        const int val5  = 0x00010;
        const int val6  = 0x00020;
        const int val7  = 0x00040;
        const int val8  = 0x00080;
        const int val9  = 0x00100;
        const int val10 = 0x00200;
        const int val11 = 0x00400;
        const int val12 = 0x00800;
        const int val13 = 0x01000;
        const int val14 = 0x02000;
        const int val15 = 0x04000;
        const int val16 = 0x08000;
        const int val17 = 0x10000;
        const int val18 = 0x20000;
        const int val19 = 0x40000;
        const int val20 = 0x80000;

#define SUMARG(n) val ## n

        // Test that macro expansion is empty
        const char s[3] = "a" BSLS_MACROREPEAT(0, SUMARG) "b";
        ASSERT('a' == s[0] && 'b' == s[1] && '\0' == s[2]);

        ASSERT(0x00000000 == sumArgs(BSLS_MACROREPEAT_COMMA(0 , SUMARG)));
        ASSERT(0x01000001 == sumArgs(BSLS_MACROREPEAT_COMMA(1 , SUMARG)));
        ASSERT(0x02000003 == sumArgs(BSLS_MACROREPEAT_COMMA(2 , SUMARG)));
        ASSERT(0x03000007 == sumArgs(BSLS_MACROREPEAT_COMMA(3 , SUMARG)));
        ASSERT(0x0400000f == sumArgs(BSLS_MACROREPEAT_COMMA(4 , SUMARG)));
        ASSERT(0x0500001f == sumArgs(BSLS_MACROREPEAT_COMMA(5 , SUMARG)));
        ASSERT(0x0600003f == sumArgs(BSLS_MACROREPEAT_COMMA(6 , SUMARG)));
        ASSERT(0x0700007f == sumArgs(BSLS_MACROREPEAT_COMMA(7 , SUMARG)));
        ASSERT(0x080000ff == sumArgs(BSLS_MACROREPEAT_COMMA(8 , SUMARG)));
        ASSERT(0x090001ff == sumArgs(BSLS_MACROREPEAT_COMMA(9 , SUMARG)));
        ASSERT(0x0A0003ff == sumArgs(BSLS_MACROREPEAT_COMMA(10, SUMARG)));
        ASSERT(0x0B0007ff == sumArgs(BSLS_MACROREPEAT_COMMA(11, SUMARG)));
        ASSERT(0x0C000fff == sumArgs(BSLS_MACROREPEAT_COMMA(12, SUMARG)));
        ASSERT(0x0D001fff == sumArgs(BSLS_MACROREPEAT_COMMA(13, SUMARG)));
        ASSERT(0x0E003fff == sumArgs(BSLS_MACROREPEAT_COMMA(14, SUMARG)));
        ASSERT(0x0F007fff == sumArgs(BSLS_MACROREPEAT_COMMA(15, SUMARG)));
        ASSERT(0x1000ffff == sumArgs(BSLS_MACROREPEAT_COMMA(16, SUMARG)));
        ASSERT(0x1101ffff == sumArgs(BSLS_MACROREPEAT_COMMA(17, SUMARG)));
        ASSERT(0x1203ffff == sumArgs(BSLS_MACROREPEAT_COMMA(18, SUMARG)));
        ASSERT(0x1307ffff == sumArgs(BSLS_MACROREPEAT_COMMA(19, SUMARG)));
        ASSERT(0x140fffff == sumArgs(BSLS_MACROREPEAT_COMMA(20, SUMARG)));

#undef SUMARG
      } break;

      case 2: {
        // --------------------------------------------------------------------
        // TESTING BSLS_MACROREPEAT
        //
        // Concerns:
        //: 1 When invoked with a repetition count if 0, the macro expands to
        //:   an empty token stream.
        //: 2 When invoked with a repetition count of 1 to 20, the repetition
        //:   phrase is repeated the specified number of times.
        //: 3 No extra tokens are inserted between repetitions.
        //: 4 The repetition phrase is invoked with an integer literal
        //:   argument which indicates the argument number (starting at 1).
        //
        // Plan:
        //: 1 To test concern 1, invoke 'BSLS_MACROREPEAT' with a repetition
        //:   count if 0 in between two string literals.  Verify that the
        //:   result is the concatenation of the two strings with no
        //:   intervening characters.
        //: 2 For concern 2, invoke 'BSLS_MACROREPEAT' with each repetition
        //:   count from 1 to 20 using a repetition phrase that XORs bits such
        //:   that the resulting value verifies the actual repetitions
        //:   invoked.
        //: 3 For concern 3, the repetition phrase ends in a '^', which is not
        //:   likely to compile if extra tokens are inserted between
        //:   repetitions.
        //: 4 For concern 4, the repetition phrase constructs the values being
        //:   XORed together by token-concatenation between the token 'val'
        //:   and the integer literal generated by 'BSLS_MACROREPEAT'.  A set
        //:   of constants, 'val0', 'val1', etc. containg the actual values to
        //:   be XORed together.
        //
        // Testing
        //   BSLS_MACROREPEAT(N, MACRO)
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING BSLS_MACROREPEAT"
                            "\n========================\n");

        const int val0  = 0x00000;
        const int val1  = 0x00001;
        const int val2  = 0x00002;
        const int val3  = 0x00004;
        const int val4  = 0x00008;
        const int val5  = 0x00010;
        const int val6  = 0x00020;
        const int val7  = 0x00040;
        const int val8  = 0x00080;
        const int val9  = 0x00100;
        const int val10 = 0x00200;
        const int val11 = 0x00400;
        const int val12 = 0x00800;
        const int val13 = 0x01000;
        const int val14 = 0x02000;
        const int val15 = 0x04000;
        const int val16 = 0x08000;
        const int val17 = 0x10000;
        const int val18 = 0x20000;
        const int val19 = 0x40000;
        const int val20 = 0x80000;

#define XORVAL(n) val ## n ^

        // Test that macro expansion is empty
        const char s[3] = "a" BSLS_MACROREPEAT(0, XORVAL) "b";
        ASSERT('a' == s[0] && 'b' == s[1] && '\0' == s[2]);

        ASSERT(0x000000 == BSLS_MACROREPEAT(0 , XORVAL) 0);
        ASSERT(0x000001 == BSLS_MACROREPEAT(1 , XORVAL) 0);
        ASSERT(0x000003 == BSLS_MACROREPEAT(2 , XORVAL) 0);
        ASSERT(0x000007 == BSLS_MACROREPEAT(3 , XORVAL) 0);
        ASSERT(0x00000f == BSLS_MACROREPEAT(4 , XORVAL) 0);
        ASSERT(0x00001f == BSLS_MACROREPEAT(5 , XORVAL) 0);
        ASSERT(0x00003f == BSLS_MACROREPEAT(6 , XORVAL) 0);
        ASSERT(0x00007f == BSLS_MACROREPEAT(7 , XORVAL) 0);
        ASSERT(0x0000ff == BSLS_MACROREPEAT(8 , XORVAL) 0);
        ASSERT(0x0001ff == BSLS_MACROREPEAT(9 , XORVAL) 0);
        ASSERT(0x0003ff == BSLS_MACROREPEAT(10, XORVAL) 0);
        ASSERT(0x0007ff == BSLS_MACROREPEAT(11, XORVAL) 0);
        ASSERT(0x000fff == BSLS_MACROREPEAT(12, XORVAL) 0);
        ASSERT(0x001fff == BSLS_MACROREPEAT(13, XORVAL) 0);
        ASSERT(0x003fff == BSLS_MACROREPEAT(14, XORVAL) 0);
        ASSERT(0x007fff == BSLS_MACROREPEAT(15, XORVAL) 0);
        ASSERT(0x00ffff == BSLS_MACROREPEAT(16, XORVAL) 0);
        ASSERT(0x01ffff == BSLS_MACROREPEAT(17, XORVAL) 0);
        ASSERT(0x03ffff == BSLS_MACROREPEAT(18, XORVAL) 0);
        ASSERT(0x07ffff == BSLS_MACROREPEAT(19, XORVAL) 0);
        ASSERT(0x0fffff == BSLS_MACROREPEAT(20, XORVAL) 0);

#undef XORVAL

      } break;

      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //
        // Concerns: Exercise basic functionality of this component
        //
        // Plan:
        //   Invoke each of the 'BSLS_MACROREPEAT*' macros with a few
        //   different repeat counts and a few different repetition phrase
        //   in such a way that the result can be easily verified.
	//
        // Testing:
        //   This "test" exercises basic functionality, but tests nothing.
        // --------------------------------------------------------------------

        if (verbose) printf("\nBREATHING TEST"
                            "\n==============\n");

        // OR together the bits from 1 to n (excluding bit 0)
        int i;
#define ADDBIT(n) i |= (1 << (n));
        i = 0; BSLS_MACROREPEAT(0,  ADDBIT) ASSERT(0        == i);
        i = 0; BSLS_MACROREPEAT(5,  ADDBIT) ASSERT(62       == i);
        i = 0; BSLS_MACROREPEAT(10, ADDBIT) ASSERT(2046     == i);
        i = 0; BSLS_MACROREPEAT(20, ADDBIT) ASSERT(0x1ffffe == i);
#undef ADDBIT

        // This class holds an integer and overloads the comma operator to act
        // like the addition operator.
        struct CommaTest {
            int m_data;

            CommaTest(int n = 0) : m_data(n) { }

            CommaTest operator,(CommaTest other) const
                { return CommaTest(m_data + other.m_data); }

            bool operator==(int i) const { return m_data == i; }
        };

        // Add together the numbers from 1 to n using the comma operator
        // instead of the + operator
#define CT(n) CommaTest(n)
        ASSERT(BSLS_MACROREPEAT_COMMA( 0, CT) true); // expands to empty
        ASSERT((BSLS_MACROREPEAT_COMMA( 1, CT)) ==  1);
        ASSERT((BSLS_MACROREPEAT_COMMA( 9, CT)) == 45);
        ASSERT((BSLS_MACROREPEAT_COMMA(10, CT)) == 55);
        ASSERT((BSLS_MACROREPEAT_COMMA(20, CT)) == 210);
#undef CT

        // Add together the numbers from 1 to n using the comma operator
#define N(n) (n)
        ASSERT(true  BSLS_MACROREPEAT_SEP( 0, N, +)); // expands to empty
        ASSERT(3   == BSLS_MACROREPEAT_SEP( 2, N, +));
        ASSERT(36  == BSLS_MACROREPEAT_SEP( 8, N, +));
        ASSERT(120 == BSLS_MACROREPEAT_SEP(15, N, +));
        ASSERT(210 == BSLS_MACROREPEAT_SEP(20, N, +));
#undef N

      } break;

      default: {
        fprintf(stderr, "WARNING: CASE `%d' NOT FOUND.\n", test);
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        fprintf(stderr, "Error, non-zero test status = %d.\n", testStatus);
    }

    return testStatus;
}

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
