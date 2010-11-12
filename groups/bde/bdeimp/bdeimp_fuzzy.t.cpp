// bdeimp_fuzzy.t.cpp           -*-C++-*-

#include <bdeimp_fuzzy.h>

#include <bsl_cstdlib.h>
#include <bsl_iostream.h>

using namespace BloombergLP;
using namespace bsl;  // automatically added by script


// ============================================================================
//                                   TEST PLAN
// ----------------------------------------------------------------------------
// [ 2] static int compare(double, double)
// [ 3] static int compare(double, double, double)
// [ 4] static int compare(double, double, double, double)
// [ 2] static bool eq(double a, double b);
// [ 3] static bool eq(double a, double b, double relTol);
// [ 4] static bool eq(double a, double b, double relTol, double absTol);
// [ 2] static bool ne(double a, double b);
// [ 3] static bool ne(double a, double b, double relTol);
// [ 4] static bool ne(double a, double b, double relTol, double absTol);
// [ 2] static bool lt(double a, double b);
// [ 3] static bool lt(double a, double b, double relTol);
// [ 4] static bool lt(double a, double b, double relTol, double absTol);
// [ 2] static bool le(double a, double b);
// [ 3] static bool le(double a, double b, double relTol);
// [ 4] static bool le(double a, double b, double relTol, double absTol);
// [ 2] static bool ge(double a, double b);
// [ 3] static bool ge(double a, double b, double relTol);
// [ 4] static bool ge(double a, double b, double relTol, double absTol);
// [ 2] static bool gt(double a, double b);
// [ 3] static bool gt(double a, double b, double relTol);
// [ 4] static bool gt(double a, double b, double relTol, double absTol);
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 5] Tabular EXAMPLE from component-level documentation
// [ 6] USAGE example
//=============================================================================
//                  STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------
static int testStatus = 0;

static void aSsErT(int c, const char *s, int i)
{
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (0 <= testStatus && testStatus <= 100) ++testStatus;
    }
}

#define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }
//-----------------------------------------------------------------------------
#define LOOP_ASSERT(I,X) { \
   if (!(X)) { cout << #I << ": " << I << "\n"; aSsErT(1, #X, __LINE__); }}

#define LOOP2_ASSERT(I,J,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " \
              << J << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP3_ASSERT(I,J,K,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" \
              << #K << ": " << K << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP4_ASSERT(I,J,K,L,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" << \
       #K << ": " << K << "\t" << #L << ": " << L << "\n"; \
       aSsErT(1, #X, __LINE__); } }


//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", "<< flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number
#define T_()  cout << "\t" << flush;          // Print tab w/o newline

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

typedef bdeimp_Fuzzy BF;

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test    = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;
    // int veryVeryVerbose = argc > 4;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;;

    switch (test) { case 0:  // Zero is always the leading case.
      case 6: {
        // --------------------------------------------------------------------
        // TESTING USAGE EXAMPLE
        //   The usage example provided in the component header file must
        //   compile, link, and run on all platforms as shown.
        //
        // Plan:
        //   Incorporate usage example from header into driver, remove leading
        //   comment characters, and replace 'assert' with 'ASSERT'.
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Usage Example"
                          << "\n=====================" << endl;

        {
            double a = 1.0;
            double b = 1.0 + 1.0e-25;

// The 'bdeimp_Fuzzy' utility functions are well suited for comparing both
// final or intermediate values such as prices, volumes, interest rates, and
// the products and quotients thereof.   We'll now illustrate the use of some
// of the various 'bdeimp_Fuzzy' comparison methods on two 'double' values 'a'
// 'a' and 'b'.  First we'll determine whether the two values are
// "CLOSE ENOUGH" using the implementation-defined (default) tolerances:
//..
            if (bdeimp_Fuzzy::eq(a, b)) {
                if (verbose) {
                    bsl::cout << "Values 'a' and 'b' are CLOSE ENOUGH."
                              << bsl::endl;
                }
            }
//..
// Next, we'll determine whether the same two values are "NOT RELATIVELY
// CLOSE" using our own (unusually large) criteria of 1.0 for relative
// tolerance, but continuing to rely on the default value for absolute
// tolerance:
//..
            if (bdeimp_Fuzzy::ne(a, b, 1.0)) {
                if (verbose) {
                    bsl::cout << "Values 'a' and 'b' are "
                              << "NOT RELATIVELY CLOSE." << bsl::endl;
                }
            }
//..
// Finally, we'll determine if the value 'a' is "SIGNIFICANTLY GREATER THAN"
// 'b' by supplying our own rather larger values of 1e-1 and 1e-3 for the
// relative and absolute tolerances, respectively:
//..
            if (bdeimp_Fuzzy::gt(a, b, 1e-1, 1e-3)) {
                if (verbose) {
                    bsl::cout << "Value 'a' is SIGNIFICANTLY "
                              << "GREATER THAN 'b'." << bsl::endl;
                }
            }
//..
// Which will print to bsl::cout if and only if
//..
//  bdeimp_fuzzy::ne(a, b, 1e-1, 1e-3) && a > b
//..
// is true.
        }
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING TABULAR "EXAMPLE":
        //   The tabular example provided in the component header file must
        //   be checked for correctness.
        //
        // Plan:
        //   Incorporate the tabular example from header as a conventional set
        //   of table-generated test vectors and 'ASSERT' the expected results.
        //
        // Testing:
        //   The numerical example from 'bdeimp_fuzzy.h'.
        // --------------------------------------------------------------------
        if (verbose) cout << endl
            << "Testing: Numerical 'EXAMPLE'" << endl
            << "============================" << endl;

        if (verbose) cout
            << "\nTesting numerical EXAMPLE from the component documentation:"
            << endl;

        static const struct {        // Test input/expected output struct:
            int    d_lineNum;        //   source line number
            double d_a;              //   rhs operand to test
            double d_b;              //   lhs operand to test
            double d_relTol;         //   relative tolerance
            double d_absTol;         //   absolute tolerance
            int    d_eq;             //   return for eq(d_a, d_b)
            int    d_ne;             //   return for ne(d_a, d_b)
            int    d_lt;             //   return for lt(d_a, d_b)
            int    d_le;             //   return for le(d_a, d_b)
            int    d_ge;             //   return for ge(d_a, d_b)
            int    d_gt;             //   return for gt(d_a, d_b)
        } DATA[] = {
            //                                                  Expected
            //                                              -----------------
            // line    d_a       d_b      relTol   absTol   EQ NE LT LE GE GT
            // ----  --------  --------   ------   ------   -- -- -- -- -- --
            {  L_,    99.0 ,    100.0 ,   0.010 ,  0.001 ,  0, 1, 1, 1, 0, 0 },
            {  L_,   100.0 ,     99.0 ,   0.010 ,  0.001 ,  0, 1, 0, 0, 1, 1 },
            {  L_,    99.0 ,    100.0 ,   0.011 ,  0.001 ,  1, 0, 0, 1, 1, 0 },
            {  L_,    99.0 ,    100.0 ,   0.010 ,  0.990 ,  0, 1, 1, 1, 0, 0 },
            {  L_,    99.0 ,    100.0 ,   0.010 ,  1.000 ,  1, 0, 0, 1, 1, 0 },
            {  L_,   100.0 ,    101.0 ,   0.009 ,  0.001 ,  0, 1, 1, 1, 0, 0 },
            {  L_,   101.0 ,    100.0 ,   0.009 ,  0.001 ,  0, 1, 0, 0, 1, 1 },
            {  L_,   100.0 ,    101.0 ,   0.010 ,  0.001 ,  1, 0, 0, 1, 1, 0 },
            {  L_,   100.0 ,    101.0 ,   0.009 ,  0.990 ,  0, 1, 1, 1, 0, 0 },
            {  L_,   100.0 ,    101.0 ,   0.009 ,  1.000 ,  1, 0, 0, 1, 1, 0 }
        };

        const int NUM_DATA = sizeof DATA / sizeof DATA[0];

        for (int i = 0; i < NUM_DATA; ++i) {
            const int    LINE   = DATA[i].d_lineNum;
            const double a      = DATA[i].d_a;
            const double b      = DATA[i].d_b;
            const double relTol = DATA[i].d_relTol;
            const double absTol = DATA[i].d_absTol;

            if (veryVerbose) { T_(); P_(a); P_(b); P_(relTol); P(absTol); }

            LOOP_ASSERT(LINE, BF::eq(a, b, relTol, absTol) == DATA[i].d_eq);
            LOOP_ASSERT(LINE, BF::ne(a, b, relTol, absTol) == DATA[i].d_ne);
            LOOP_ASSERT(LINE, BF::lt(a, b, relTol, absTol) == DATA[i].d_lt);
            LOOP_ASSERT(LINE, BF::le(a, b, relTol, absTol) == DATA[i].d_le);
            LOOP_ASSERT(LINE, BF::ge(a, b, relTol, absTol) == DATA[i].d_ge);
            LOOP_ASSERT(LINE, BF::gt(a, b, relTol, absTol) == DATA[i].d_gt);
        }
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING EQUALITY AND RELATIONAL FUNCTIONS:
        //   Basic tests with user-specified absolute and relative tolerances.
        //   'eq' and 'ne' should be symmetric in the first two arguments.
        //
        // Plan:
        //   Specify a set of pairs of values (a, b) along with conveniently
        //   chosen absolute and relative tolerances, and verify that each of
        //   the six relational functions returns the expected value for the
        //   tabulated input.  Test the symmetry of 'eq' and 'ne' in the first
        //   two arguments explicitly.
        //
        // Testing:
        //   static int compare(double, double, double, double);
        //   static bool eq(double a, double b, double relTol, double absTol);
        //   static bool ne(double a, double b, double relTol, double absTol);
        //   static bool lt(double a, double b, double relTol, double absTol);
        //   static bool le(double a, double b, double relTol, double absTol);
        //   static bool gt(double a, double b, double relTol, double absTol);
        //   static bool ge(double a, double b, double relTol, double absTol);
        // --------------------------------------------------------------------
        if (verbose)
            cout << "\nTesting: eq(a, b, relTol, absTol), etc."
                    "\n=======================================" << endl;

        if (verbose) cout
            << "\nTesting using user-supplied tolerances:" << endl;

        static const struct {        // Test input/expected output struct:
            int    d_lineNum;        //   source line number
            double d_a;              //   rhs operand to test
            double d_b;              //   lhs operand to test
            double d_relTol;         //   relative tolerance
            double d_absTol;         //   absolute tolerance
            int    d_eq;             //   return for eq(d_a, d_bMinusA + d_a)
            int    d_ne;             //   return for ne(d_a, d_bMinusA + d_a)
            int    d_lt;             //   return for lt(d_a, d_bMinusA + d_a)
            int    d_le;             //   return for le(d_a, d_bMinusA + d_a)
            int    d_ge;             //   return for ge(d_a, d_bMinusA + d_a)
            int    d_gt;             //   return for gt(d_a, d_bMinusA + d_a)
        } DATA[] = {
            //                                                Expected
            //                                            -----------------
            //line    d_a       d_b      relTol  absTol   EQ NE LT LE GE GT
            //----  --------  --------   ------  ------   -- -- -- -- -- --
            { L_,   1.0    ,  1.0     ,   1e-6,   1e-3,    1, 0, 0, 1, 1, 0 },
            { L_,   1.0    ,  0.9999  ,   1e-6,   1e-3,    1, 0, 0, 1, 1, 0 },
            { L_,   1.0    ,  0.9995  ,   1e-6,   1e-3,    1, 0, 0, 1, 1, 0 },
            { L_,   1.0    ,  0.999   ,   1e-6,   1e-3,    0, 1, 0, 0, 1, 1 },
            { L_,   1.0    ,  0.99    ,   1e-6,   1e-3,    0, 1, 0, 0, 1, 1 },
            { L_,   1.0    ,  0.9     ,   1e-6,   1e-3,    0, 1, 0, 0, 1, 1 },
            { L_,   1.0    ,  1.1     ,   1e-6,   1e-3,    0, 1, 1, 1, 0, 0 },
            { L_,   1.0    ,  1.01    ,   1e-6,   1e-3,    0, 1, 1, 1, 0, 0 },
            { L_,   1.0    ,  1.00099 ,   1e-6,   1e-3,    1, 0, 0, 1, 1, 0 },
            { L_,   1.0    ,  1.001   ,   1e-6,   1e-3,    1, 0, 0, 1, 1, 0 },
            { L_,   1.0    ,  1.0001  ,   1e-6,   1e-3,    1, 0, 0, 1, 1, 0 },
            { L_,   0.9999 ,  1.0     ,   1e-6,   1e-3,    1, 0, 0, 1, 1, 0 },
            { L_,   0.9995 ,  1.0     ,   1e-6,   1e-3,    1, 0, 0, 1, 1, 0 },
            { L_,   0.999  ,  1.0     ,   1e-6,   1e-3,    0, 1, 1, 1, 0, 0 },
            { L_,   0.99   ,  1.0     ,   1e-6,   1e-3,    0, 1, 1, 1, 0, 0 },
            { L_,   0.9    ,  1.0     ,   1e-6,   1e-3,    0, 1, 1, 1, 0, 0 }
        };

        const int NUM_DATA = sizeof DATA / sizeof DATA[0];

        for (int i = 0; i < NUM_DATA; ++i) {
            const int    LINE   = DATA[i].d_lineNum;
            const double a      = DATA[i].d_a;
            const double b      = DATA[i].d_b;
            const double relTol = DATA[i].d_relTol;
            const double absTol = DATA[i].d_absTol;

            if (veryVerbose) { T_(); P_(a); P_(b); P_(relTol); P(absTol); }

            LOOP_ASSERT(LINE, BF::eq(a, b, relTol, absTol) == DATA[i].d_eq);
            LOOP_ASSERT(LINE, BF::eq(b, a, relTol, absTol) == DATA[i].d_eq);
            LOOP_ASSERT(LINE, BF::ne(a, b, relTol, absTol) == DATA[i].d_ne);
            LOOP_ASSERT(LINE, BF::ne(b, a, relTol, absTol) == DATA[i].d_ne);
            LOOP_ASSERT(LINE, BF::lt(a, b, relTol, absTol) == DATA[i].d_lt);
            LOOP_ASSERT(LINE, BF::le(a, b, relTol, absTol) == DATA[i].d_le);
            LOOP_ASSERT(LINE, BF::ge(a, b, relTol, absTol) == DATA[i].d_ge);
            LOOP_ASSERT(LINE, BF::gt(a, b, relTol, absTol) == DATA[i].d_gt);
            const int RESULT = BF::compare(a, b, relTol, absTol);

            if (DATA[i].d_eq) {
                LOOP_ASSERT(LINE, 0 == RESULT);
            }
            else if (DATA[i].d_lt) {
                LOOP_ASSERT(LINE, RESULT < 0);
            }
            else {
                LOOP_ASSERT(LINE, DATA[i].d_gt);
                LOOP_ASSERT(LINE, RESULT > 0);
            }
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING EQUALITY AND RELATIONAL FUNCTIONS:
        //   Basic tests with default absolute tolerance and specified relative
        //   tolerance.  'eq' and 'ne' should be symmetric in the first two
        //   arguments.
        //
        // Plan:
        //   Specify a set of values (a, b).  For convenience of notation,
        //   tabulate a and b - a along with a conveniently chosen relative
        //   tolerance, and verify that each of the six relational functions
        //   returns the expected value for the tabulated input.  Test the
        //   symmetry of 'eq' and 'ne' in the first two arguments explicitly.
        //
        // Testing:
        //   static int compare(double a, double b, double relTol);
        //   static bool eq(double a, double b, double relTol);
        //   static bool ne(double a, double b, double relTol);
        //   static bool lt(double a, double b, double relTol);
        //   static bool le(double a, double b, double relTol);
        //   static bool gt(double a, double b, double relTol);
        //   static bool ge(double a, double b, double relTol);
        // --------------------------------------------------------------------
        if (verbose) cout << "\nTesting: eq(a, b, relTol), etc."
                             "\n===============================" << endl;

        if (verbose) cout
            << "\nTesting using user-supplied relative tolerances:" << endl;

        static const struct {        // Test input/expected output struct:
            int    d_lineNum;        //   source line number
            double d_a;              //   rhs operand to test
            double d_bMinusA;        //   lhs operand = d_bMinusA + d_a
            double d_relTol;         //   relative tolerance
            int    d_eq;             //   return for eq(d_a, d_bMinusA + d_a)
            int    d_ne;             //   return for ne(d_a, d_bMinusA + d_a)
            int    d_lt;             //   return for lt(d_a, d_bMinusA + d_a)
            int    d_le;             //   return for le(d_a, d_bMinusA + d_a)
            int    d_ge;             //   return for ge(d_a, d_bMinusA + d_a)
            int    d_gt;             //   return for gt(d_a, d_bMinusA + d_a)
        } DATA[] = {
            //                                            Expected
            //                                        -----------------
            //line   d_a       d_bMinusA  relTol      EQ NE LT LE GE GT
            //----  --------   ---------  ------      -- -- -- -- -- --
            { L_,    1.0     ,   0.0    ,   1e-6,      1, 0, 0, 1, 1, 0 },
            { L_,   -1.0     ,   0.0    ,   1e-6,      1, 0, 0, 1, 1, 0 },
            { L_,    1.0     ,   1.0e-7 ,   1e-6,      1, 0, 0, 1, 1, 0 },
            { L_,    1.0     ,  -1.0e-7 ,   1e-6,      1, 0, 0, 1, 1, 0 },
            { L_,   -1.0     ,   1.0e-7 ,   1e-6,      1, 0, 0, 1, 1, 0 },
            { L_,   -1.0     ,  -1.0e-7 ,   1e-6,      1, 0, 0, 1, 1, 0 },
            { L_,    1.0     ,   1.1e-6 ,   1e-6,      0, 1, 1, 1, 0, 0 },
            { L_,    1.0     ,  -1.1e-6 ,   1e-6,      0, 1, 0, 0, 1, 1 },
            { L_,   -1.0     ,   1.1e-6 ,   1e-6,      0, 1, 1, 1, 0, 0 },
            { L_,   -1.0     ,  -1.1e-6 ,   1e-6,      0, 1, 0, 0, 1, 1 },
            { L_,    1.0     ,   1.0e-7 ,   0.0 ,      0, 1, 1, 1, 0, 0 },
            { L_,    1.0     ,  -1.0e-7 ,   0.0 ,      0, 1, 0, 0, 1, 1 },
            { L_,   -1.0     ,   1.0e-7 ,   0.0 ,      0, 1, 1, 1, 0, 0 },
            { L_,   -1.0     ,  -1.0e-7 ,   0.0 ,      0, 1, 0, 0, 1, 1 },
            { L_,    1.0     ,   1.0e-25,   0.0 ,      1, 0, 0, 1, 1, 0 },
            { L_,    1.0     ,  -1.0e-25,   0.0 ,      1, 0, 0, 1, 1, 0 },
            { L_,   -1.0     ,   1.0e-25,   0.0 ,      1, 0, 0, 1, 1, 0 },
            { L_,   -1.0     ,  -1.0e-25,   0.0 ,      1, 0, 0, 1, 1, 0 }
        };
        const int NUM_DATA = sizeof DATA / sizeof DATA[0];

        for (int i = 0; i < NUM_DATA; ++i) {
            const int    LINE   = DATA[i].d_lineNum;
            const double a      = DATA[i].d_a;
            const double b      = DATA[i].d_bMinusA + DATA[i].d_a;
            const double relTol = DATA[i].d_relTol;

            if (veryVerbose) { T_();  P_(a);  P_(b);  P(relTol); }

            LOOP_ASSERT(LINE, BF::eq(a, b, relTol) == DATA[i].d_eq);
            LOOP_ASSERT(LINE, BF::eq(b, a, relTol) == DATA[i].d_eq);
            LOOP_ASSERT(LINE, BF::ne(a, b, relTol) == DATA[i].d_ne);
            LOOP_ASSERT(LINE, BF::ne(b, a, relTol) == DATA[i].d_ne);
            LOOP_ASSERT(LINE, BF::lt(a, b, relTol) == DATA[i].d_lt);
            LOOP_ASSERT(LINE, BF::le(a, b, relTol) == DATA[i].d_le);
            LOOP_ASSERT(LINE, BF::ge(a, b, relTol) == DATA[i].d_ge);
            LOOP_ASSERT(LINE, BF::gt(a, b, relTol) == DATA[i].d_gt);
            const int RESULT = BF::compare(a, b, relTol);
            if (DATA[i].d_eq) {
                LOOP_ASSERT(LINE, 0 == RESULT);
            }
            else if (DATA[i].d_lt) {
                LOOP_ASSERT(LINE, RESULT < 0);
            }
            else {
                LOOP_ASSERT(LINE, DATA[i].d_gt);
                LOOP_ASSERT(LINE, RESULT > 0);
            }
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING EQUALITY AND RELATIONAL FUNCTIONS:
        //   Basic tests with default tolerances.  'eq' and 'ne' should be
        //   symmetric in the first two arguments.
        //
        // Plan:
        //   Specify a set of values (a, b).  For convenience of notation,
        //   tabulate a and b - a and verify that each of the six relational
        //   functions returns the expected value for the tabulated input.
        //   Test the symmetry of 'eq' and 'ne' in the first two arguments
        //   explicitly.
        //
        // Testing:
        //   static int compare(double a, double b);
        //   static bool eq(double a, double b);
        //   static bool ne(double a, double b);
        //   static bool lt(double a, double b);
        //   static bool le(double a, double b);
        //   static bool gt(double a, double b);
        //   static bool ge(double a, double b);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting: eq(a, b), etc."
                             "\n=======================" << endl;

        //  1) Test eq(), etc., with default tolerances

        if (verbose) cout << "\nTesting using default tolerances:" << endl;

        static const struct {        // Test input/expected output struct:
            int    d_lineNum;        //   source line number
            double d_a;              //   rhs operand to test
            double d_bMinusA;        //   lhs operand = d_bMinusA + d_a
            int    d_eq;             //   return for eq(d_a, d_bMinusA + d_a)
            int    d_ne;             //   return for ne(d_a, d_bMinusA + d_a)
            int    d_lt;             //   return for lt(d_a, d_bMinusA + d_a)
            int    d_le;             //   return for le(d_a, d_bMinusA + d_a)
            int    d_ge;             //   return for ge(d_a, d_bMinusA + d_a)
            int    d_gt;             //   return for gt(d_a, d_bMinusA + d_a)
        } DATA[] = {
            //                                         Expected
            //                                     -----------------
            //line     d_a            d_bMinusA        EQ NE LT LE GE GT
            //---- --------------   --------------     -- -- -- -- -- --
            { L_,      1.0         ,    0.0          ,   1, 0, 0, 1, 1, 0 },
            { L_,     -1.0         ,    0.0          ,   1, 0, 0, 1, 1, 0 },
            { L_,      1.0         ,    0.9e-12      ,   1, 0, 0, 1, 1, 0 },
            { L_,      1.0         ,   -0.9e-12      ,   1, 0, 0, 1, 1, 0 },
            { L_,     -1.0         ,    0.9e-12      ,   1, 0, 0, 1, 1, 0 },
            { L_,     -1.0         ,   -0.9e-12      ,   1, 0, 0, 1, 1, 0 },
            { L_,      1.0         ,    1.1e-12      ,   0, 1, 1, 1, 0, 0 },
            { L_,      1.0         ,   -1.1e-12      ,   0, 1, 0, 0, 1, 1 },
            { L_,     -1.0         ,    1.1e-12      ,   0, 1, 1, 1, 0, 0 },
            { L_,     -1.0         ,   -1.1e-12      ,   0, 1, 0, 0, 1, 1 },
            { L_,      1.0e-20     ,    0.9e-24      ,   1, 0, 0, 1, 1, 0 },
            { L_,      1.0e-20     ,   -0.9e-24      ,   1, 0, 0, 1, 1, 0 },
            { L_,     -1.0e-20     ,    0.9e-24      ,   1, 0, 0, 1, 1, 0 },
            { L_,     -1.0e-20     ,   -0.9e-24      ,   1, 0, 0, 1, 1, 0 },
            { L_,      1.0e-20     ,    1.1e-24      ,   0, 1, 1, 1, 0, 0 },
            { L_,      1.0e-20     ,   -1.1e-24      ,   0, 1, 0, 0, 1, 1 },
            { L_,     -1.0e-20     ,    1.1e-24      ,   0, 1, 1, 1, 0, 0 },
            { L_,     -1.0e-20     ,   -1.1e-24      ,   0, 1, 0, 0, 1, 1 }
        };

        const int NUM_DATA = sizeof DATA / sizeof DATA[0];

        for (int i = 0; i < NUM_DATA; ++i) {
            const int    LINE = DATA[i].d_lineNum;
            const double a    = DATA[i].d_a;
            const double b    = DATA[i].d_bMinusA + DATA[i].d_a;

            if (veryVerbose) { T_();  P_(a);  P(b); }

            LOOP_ASSERT(LINE, BF::eq(a, b) == DATA[i].d_eq);
            LOOP_ASSERT(LINE, BF::eq(b, a) == DATA[i].d_eq);
            LOOP_ASSERT(LINE, BF::ne(a, b) == DATA[i].d_ne);
            LOOP_ASSERT(LINE, BF::ne(b, a) == DATA[i].d_ne);
            LOOP_ASSERT(LINE, BF::lt(a, b) == DATA[i].d_lt);
            LOOP_ASSERT(LINE, BF::le(a, b) == DATA[i].d_le);
            LOOP_ASSERT(LINE, BF::ge(a, b) == DATA[i].d_ge);
            LOOP_ASSERT(LINE, BF::gt(a, b) == DATA[i].d_gt);
            const int RESULT = BF::compare(a, b);
            if (DATA[i].d_eq) {
                LOOP_ASSERT(LINE, 0 == RESULT);
            }
            else if (DATA[i].d_lt) {
                LOOP_ASSERT(LINE, RESULT < 0);
            }
            else {
                LOOP_ASSERT(LINE, DATA[i].d_gt);
                LOOP_ASSERT(LINE, RESULT > 0);
            }
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // Breathing Test:
        //   Test basic use of compare().
        // Testing:
        //   int compare(double a, double b, double relTol, double absTol)
        // --------------------------------------------------------------------

        if (verbose)
            cout << "\nTesting: compare(a, b, relTol, absTol)"
                    "\n======================================" << endl;

        if (verbose) cout
            << "\nTesting compare() on absolute tolerances:" << endl;

        {
            double relTol = 0.0;         // relative tolerance
            double absTol = 1.0e-3;      // absolute tolerance

            static const struct {        // Test input/expected output struct:
                int    d_lineNum;        //   source line number
                double d_a;              //   rhs operand to test
                double d_b;              //   lhs operand to test
                int    d_spec;           //   return for compare(d_a, d_b)
            } DATA[] = {
                //line     d_a             d_b               expected d_spec
                //---- --------------   --------------       ---------------
                { L_,      1.0         ,    1.0            ,          0      },
                { L_,      1.0         ,    0.9999         ,          0      },
                { L_,      1.0         ,    0.9995         ,          0      },
                { L_,      1.0         ,    0.999          ,          1      },
                { L_,      1.0         ,    0.99           ,          1      },
                { L_,      1.0         ,    0.9            ,          1      },
                { L_,      1.0         ,    1.1            ,         -1      },
                { L_,      1.0         ,    1.01           ,         -1      },
                { L_,      1.0         ,    1.00099        ,          0      },
                { L_,      1.0         ,    1.001          ,          0      },
                { L_,      1.0         ,    1.0001         ,          0      },
                { L_,      0.9999      ,    1.0            ,          0      },
                { L_,      0.9995      ,    1.0            ,          0      },
                { L_,      0.999       ,    1.0            ,         -1      },
                { L_,      0.99        ,    1.0            ,         -1      },
                { L_,      0.9         ,    1.0            ,         -1      },
                { L_,     -1.0         ,    1.0            ,         -1      },
                { L_,     -0.1         ,    0.1            ,         -1      },
                { L_,     -0.01        ,    0.01           ,         -1      },
                { L_,     -0.001       ,    0.001          ,         -1      },
                { L_,     -0.0001      ,    0.0001         ,          0      },
                { L_,     -0.00001     ,    0.00001        ,          0      },
                { L_,     -0.000001    ,    0.000001       ,          0      },
                { L_,     -1.e-19      ,    1.e-19         ,          0      },
                { L_,      1.0         ,   -1.0            ,          1      },
                { L_,      0.1         ,   -0.1            ,          1      },
                { L_,      0.01        ,   -0.01           ,          1      },
                { L_,      0.001       ,   -0.001          ,          1      },
                { L_,      0.0001      ,   -0.0001         ,          0      },
                { L_,      0.00001     ,   -0.00001        ,          0      },
                { L_,      0.000001    ,   -0.000001       ,          0      },
                { L_,      1.e-19      ,   -1.e-19         ,          0      }
            };

            const int NUM_DATA = sizeof DATA / sizeof DATA[0];

            for (int i = 0; i < NUM_DATA; ++i) {
                const int    LINE   = DATA[i].d_lineNum;
                const double a      = DATA[i].d_a;
                const double b      = DATA[i].d_b;
                const double retVal = BF::compare(a, b, relTol, absTol);

                if (veryVerbose) { T_();  P_(a);  P_(b);  P(retVal); }

                LOOP_ASSERT(LINE, DATA[i].d_spec == retVal);
            }
        }

        if (verbose) cout
            << "\nTesting compare() on relative tolerances:" << endl;

        {
            double relTol = 1.0e-3;             // relative tolerance
            double absTol = 0.0;                // absolute tolerance

            static const struct {        // Test input/expected output struct:
                int    d_lineNum;        //   source line number
                double d_a;              //   rhs operand to test
                double d_b;              //   lhs operand to test
                int    d_spec;           //   return for compare(d_a, d_b)
            } DATA[] = {
                //line     d_a              d_b               expected d_spec
                //----  --------------   --------------       ---------------
                { L_,      1.0         ,    1.0            ,          0      },
                { L_,      1.0         ,    0.9999         ,          0      },
                { L_,      1.0         ,    0.9995         ,          0      },
                { L_,      1.0         ,    0.999          ,          1      },
                { L_,      1.0         ,    0.99           ,          1      },
                { L_,      1.0         ,    0.9            ,          1      },
                { L_,      1.0         ,    1.1            ,         -1      },
                { L_,      1.0         ,    1.01           ,         -1      },
                { L_,      1.0         ,    1.00099        ,          0      },
                { L_,      1.0         ,    1.001          ,          0      },
                { L_,      1.0         ,    1.0001         ,          0      },
                { L_,      0.9999      ,    1.0            ,          0      },
                { L_,      0.9995      ,    1.0            ,          0      },
                { L_,      0.999       ,    1.0            ,         -1      },
                { L_,      0.99        ,    1.0            ,         -1      },
                { L_,      0.9         ,    1.0            ,         -1      },
                { L_,     -1.0         ,    1.0            ,         -1      },
                { L_,     -0.1         ,    0.1            ,         -1      },
                { L_,     -0.01        ,    0.01           ,         -1      },
                { L_,     -0.001       ,    0.001          ,         -1      },
                { L_,     -0.0001      ,    0.0001         ,         -1      },
                { L_,     -0.00001     ,    0.00001        ,         -1      },
                { L_,     -0.000001    ,    0.000001       ,         -1      },
                { L_,     -1.e-19      ,    1.e-19         ,         -1      },
                { L_,      1.0         ,   -1.0            ,          1      },
                { L_,      0.1         ,   -0.1            ,          1      },
                { L_,      0.01        ,   -0.01           ,          1      },
                { L_,      0.001       ,   -0.001          ,          1      },
                { L_,      0.0001      ,   -0.0001         ,          1      },
                { L_,      0.00001     ,   -0.00001        ,          1      },
                { L_,      0.000001    ,   -0.000001       ,          1      },
                { L_,      1.e-19      ,   -1.e-19         ,          1      }
            };

            const int NUM_DATA = sizeof DATA / sizeof DATA[0];

            for (int i = 0; i < NUM_DATA; ++i) {
                const int    LINE   = DATA[i].d_lineNum;
                const double a      = DATA[i].d_a;
                const double b      = DATA[i].d_b;
                const double retVal = BF::compare(a, b, relTol, absTol);

                if (veryVerbose) { T_();  P_(a);  P_(b);  P(retVal); }

                LOOP_ASSERT(LINE, DATA[i].d_spec == retVal);
            }
        }

      } break;
      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        cerr << "Error: non-zero test status = " << testStatus << "." << endl;
    }
    return testStatus;
}
// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2002
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
