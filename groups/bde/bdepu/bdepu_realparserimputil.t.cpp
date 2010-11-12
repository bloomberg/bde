// bdepu_realparserimputil.t.cpp              -*-C++-*-

#include <bdepu_realparserimputil.h>

#include <bsl_iostream.h>
#include <bsl_c_stdlib.h>     // atoi()

using namespace BloombergLP;
using namespace bsl;  // automatically added by script


// TBD parameter ordering in doc is wrong

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// TBD doc
//
//-----------------------------------------------------------------------------
// [ 1] convertBinaryExponentToDecimal(int, Uint64 *, int *);
// [ 2] convertBinaryFractionToDecimalFraction(Uint64, Uint64 *);
// [ 3] convertBinaryToDecimal(Uint64, int, Uint64 *, int *);
// [ 4] convertBinaryToDouble(int, Uint64, int, double);
// [ 5] convertDecimalExponentToBinary(int, Uint64 *, int *);
// [ 6] convertDecimalFractionToBinaryFraction(Uint64, Uint64 *);
// [ 7] convertDecimalToBinary(Uint64, int, Uint64 *, int *);
// [ 8] convertDoubleToBinary(double, int *, Uint64 *, int *);
//--------------------------------------------------------------------------
// [ 9] CONCERN: DECIMAL FRACTION TO BINARY FRACTION TO DECIMAL FRACTION
// [10] CONCERN: DECIMAL EXPONENT TO BINARY TO DECIMAL
// [11] CONCERN: DECIMAL TO BINARY TO DECIMAL
// [12] USAGE EXAMPLE
//--------------------------------------------------------------------------



//==========================================================================
//                  STANDARD BDE ASSERT TEST MACRO
//--------------------------------------------------------------------------
static int testStatus = 0;

static void aSsErT(int c, const char *s, int i) {
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}

# define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }
//--------------------------------------------------------------------------
#define LOOP_ASSERT(I,X) { \
    if (!(X)) { cout << #I << ": " << I << "\n"; aSsErT(1, #X, __LINE__);}}

#define LOOP2_ASSERT(I,J,X) { \
    if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " \
        << J << "\n"; aSsErT(1, #X, __LINE__); } }


//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", "<< flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number
#define T_()  cout << '\t' << flush;          // Print a tab character

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

typedef bsls_PlatformUtil::Int64 Int64;
typedef bsls_PlatformUtil::Uint64 Uint64;

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------


int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;
    // int veryVeryVerbose = argc > 4;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;;

    switch (test) { case 0:  // Zero is always the leading case.
      case 12: {
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

        if (verbose) cout << endl
                          << "Testing Usage Example" << endl
                          << "=====================" << endl;

        double initial = 3.1415;

        int                       binSign;
        bsls_PlatformUtil::Uint64 binFrac;
        int                       binExp;
           bdepu_RealParserImpUtil::
                   convertDoubleToBinary(&binSign, &binFrac, &binExp, initial);

        bsls_PlatformUtil::Uint64 decFrac;
        int                       decExp;
           bdepu_RealParserImpUtil::
                    convertBinaryToDecimal(&decFrac, &decExp, binFrac, binExp);

           bdepu_RealParserImpUtil::
                    convertDecimalToBinary(&binFrac, &binExp, decFrac, decExp);

        double result;
           bdepu_RealParserImpUtil::
                   convertBinaryToDouble(&result, binSign, binFrac, binExp);
        ASSERT(initial == result);
      } break;
      case 11: {
        // --------------------------------------------------------------------
        // TESTING DECIMAL TO BINARY TO DECIMAL
        //
        // Concerns:
        // TBD doc
        //
        // Plan:
        // TBD doc
        //
        // Testing:
        //   CONCERN: DECIMAL TO BINARY TO DECIMAL
        // --------------------------------------------------------------------

        {
            const Uint64 TOL = 1500uLL;

            const Uint64 STEP = 10203040506070849uLL;
            const int STEP_EXP = 17;
            const int NUM = 40000;

            Uint64 offset = 0uLL;
            int offset_exp = 0;
            for (int i = 0; i < NUM; ++i) {
                Uint64 DECFRAC = 100000000000000000uLL + offset;
                int    DECEXP = offset_exp - 300;
                Uint64 binFrac;
                int    binExp;
                Uint64 decFrac;
                int    decExp;

                bdepu_RealParserImpUtil::
                    convertDecimalToBinary(&binFrac, &binExp, DECFRAC, DECEXP);
                bdepu_RealParserImpUtil::
                    convertBinaryToDecimal(&decFrac, &decExp, binFrac, binExp);
                if (decExp < DECEXP) {
                    LOOP2_ASSERT(DECFRAC, DECEXP, DECEXP == decExp + 1);
                    LOOP2_ASSERT(DECFRAC, DECEXP, decFrac/10 + TOL > DECFRAC);
                }
                else if (decExp > DECEXP) {
                    LOOP2_ASSERT(DECFRAC, DECEXP, DECEXP == decExp - 1);
                    LOOP2_ASSERT(DECFRAC, DECEXP, DECFRAC/10 + TOL > decFrac);
                }
                else {
                    LOOP2_ASSERT(DECFRAC, DECEXP, DECEXP == decExp);
                    if (decFrac < DECFRAC) {
                        LOOP2_ASSERT(DECFRAC, DECEXP, decFrac + TOL > DECFRAC);
                    }
                    else {
                        LOOP2_ASSERT(DECFRAC, DECEXP, DECFRAC + TOL > decFrac);
                    }
                }
                if (veryVerbose) {
                    P_(DECFRAC);
                    P(DECEXP);
                    P_(decFrac);
                    P(decExp);
                }
                offset = (offset + STEP) % 900000000000000000uLL;
                offset_exp = (offset_exp + STEP_EXP) % 601;
            }
        }
      } break;
      case 10: {
        // --------------------------------------------------------------------
        // TESTING DECIMAL EXPONENT TO BINARY TO DECIMAL
        //
        // Concerns:
        // TBD doc
        //
        // Plan:
        // TBD doc
        //
        // Testing:
        //   CONCERN: DECIMAL EXPONENT TO BINARY TO DECIMAL
        // --------------------------------------------------------------------

        {
            const Uint64 TOL = 200uLL;

            for (int i = 0; i < 10000; ++i) {
                for (int DECEXP = -307; DECEXP <= 307; ++DECEXP) {
                    Uint64 DECFRAC = 100000000000000000uLL;
                    Uint64 binFrac;
                    int    binExp;
                    Uint64 decFrac;
                    int    decExp;

                    bdepu_RealParserImpUtil::
                        convertDecimalExponentToBinary(&binFrac,
                                                       &binExp,
                                                       DECEXP - 1);
                    bdepu_RealParserImpUtil::convertBinaryToDecimal(&decFrac,
                                                                    &decExp,
                                                                    binFrac,
                                                                    binExp);
                    if (decExp < DECEXP) {
                        LOOP2_ASSERT(DECFRAC, DECEXP, DECEXP == decExp + 1);
                        LOOP2_ASSERT(DECFRAC,
                                     DECEXP,
                                     decFrac/10 + TOL > DECFRAC);
                    }
                    else if (decExp > DECEXP) {
                        LOOP2_ASSERT(DECFRAC, DECEXP, DECEXP == decExp - 1);
                        LOOP2_ASSERT(DECFRAC,
                                     DECEXP,
                                     DECFRAC/10 + TOL > decFrac);
                    }
                    else {
                        LOOP2_ASSERT(DECFRAC, DECEXP, DECEXP == decExp);
                        if (decFrac < DECFRAC) {
                            LOOP2_ASSERT(DECFRAC,
                                         DECEXP,
                                         decFrac + TOL > DECFRAC);
                        }
                        else {
                            LOOP2_ASSERT(DECFRAC,
                                         DECEXP,
                                         DECFRAC + TOL > decFrac);
                        }
                    }
                    if (veryVerbose) {
                        P_(DECFRAC);
                        P(DECEXP);
                        P_(decFrac);
                        P(decExp);
                    }
                }
            }
        }
      } break;
      case 9: {
        // --------------------------------------------------------------------
        // TESTING DECIMAL FRACTION TO BINARY FRACTION TO DECIMAL FRACTION
        //
        // Concerns:
        // TBD doc
        //
        // Plan:
        // TBD doc
        //
        // Testing:
        //   CONCERN: DECIMAL FRACTION TO BINARY FRACTION TO DECIMAL FRACTION
        // --------------------------------------------------------------------

        {
            const Uint64 TOL = 2uLL;

            const Uint64 STEP = 10203040506070849uLL;
            const int NUM = 80000;

            Uint64 offset = 0uLL;
            for (int i = 0; i < 100; ++i) {
                for (int i = 0; i < NUM; ++i) {
                    Uint64 DECFRAC = 100000000000000000uLL + offset;
                    Uint64 binFrac;
                    Uint64 decFrac;

                    bdepu_RealParserImpUtil::
                     convertDecimalFractionToBinaryFraction(&binFrac, DECFRAC);
                    bdepu_RealParserImpUtil::
                     convertBinaryFractionToDecimalFraction(&decFrac, binFrac);
                    if (decFrac < DECFRAC) {
                        LOOP_ASSERT(DECFRAC, decFrac + TOL > DECFRAC);
                    }
                    else {
                        LOOP_ASSERT(DECFRAC, DECFRAC + TOL > decFrac);
                    }
                    if (veryVerbose) {
                        P_(DECFRAC);
                        cout << "binFrac = 0x"
                             << bsl::hex
                             << binFrac
                             << bsl::dec
                             << endl;
                        P(decFrac);
                    }
                    offset = (offset + STEP) % 900000000000000000uLL;
                }
            }
        }
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // TESTING DOUBLE TO BINARY
        //
        // Concerns:
        // TBD doc
        //
        // Plan:
        // TBD doc
        //
        // Testing:
        //   convertDoubleToBinary(double, int *, Uint64 *, int *);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING convertDoubleToBinary" << endl
                          << "=============================" << endl;

        {
            static const struct {
                int    d_lineNum;  // source line number
                double d_value;    // specification value
                int    d_binSign;  // expected binary sign
                Uint64 d_binFrac;  // expected binary fraction
                int    d_binExp;   // expected binary exponent
            } DATA[] = {
                //line  value    binSign  binFrac                binExp
                //----  -------  -------  ---------------------  -----------
                { L_,       0.0,       0, 0x0000000000000000uLL,           0 },
                { L_,       1.0,       0, 0x8000000000000000uLL,           1 },
                { L_,      -1.0,       1, 0x8000000000000000uLL,           1 },
                { L_,       0.5,       0, 0x8000000000000000uLL,           0 },
                { L_,      -0.5,       1, 0x8000000000000000uLL,           0 },
                { L_,      0.25,       0, 0x8000000000000000uLL,          -1 },
                { L_,     -0.25,       1, 0x8000000000000000uLL,          -1 },
                { L_,      0.75,       0, 0xC000000000000000uLL,           0 },
                { L_,     -0.75,       1, 0xC000000000000000uLL,           0 },
                { L_,     0.125,       0, 0x8000000000000000uLL,          -2 },
                { L_,    -0.125,       1, 0x8000000000000000uLL,          -2 },
                { L_,     0.875,       0, 0xE000000000000000uLL,           0 },
                { L_,    -0.875,       1, 0xE000000000000000uLL,           0 },
                { L_,    0.0625,       0, 0x8000000000000000uLL,          -3 },
                { L_,   -0.0625,       1, 0x8000000000000000uLL,          -3 },
                { L_,    0.9375,       0, 0xF000000000000000uLL,           0 },
                { L_,   -0.9375,       1, 0xF000000000000000uLL,           0 },

                // TBD even more vectors - case 4 and case 8 should share
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int ti = 0; ti < NUM_DATA ; ++ti) {
                const int    LINE    = DATA[ti].d_lineNum;
                const double VALUE   = DATA[ti].d_value;
                const int    BINSIGN = DATA[ti].d_binSign;
                const Uint64 BINFRAC = DATA[ti].d_binFrac;
                const int    BINEXP  = DATA[ti].d_binExp;

                int binSign;
                Uint64 binFrac;
                int binExp;
                bdepu_RealParserImpUtil::
                     convertDoubleToBinary(&binSign, &binFrac, &binExp, VALUE);

                if (veryVerbose) {
                    P(LINE);
                    P(VALUE);
                    P(BINSIGN);
                    P(binSign);
                    cout << "BINFRAC = 0x" << bsl::hex << BINFRAC
                        << bsl::dec << "\n";
                    cout << "binFrac = 0x" << bsl::hex << binFrac
                        << bsl::dec << "\n";
                    P(BINEXP);
                    P(binExp);
                }

                LOOP_ASSERT(LINE, BINSIGN == binSign);
                LOOP_ASSERT(LINE, BINFRAC == binFrac);
                LOOP_ASSERT(LINE, BINEXP == binExp);

                {
                    double value;
                    bdepu_RealParserImpUtil::convertBinaryToDouble(&value,
                                                                   binSign,
                                                                   binFrac,
                                                                   binExp);
                    LOOP_ASSERT(LINE, VALUE == value);
                }
            }
        }
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TESTING DECIMAL TO BINARY
        //
        // Concerns:
        // TBD doc
        //
        // Plan:
        // TBD doc
        //
        // Testing:
        //   convertDecimalToBinary(Uint64, int, Uint64 *, int *);
        // --------------------------------------------------------------------

        if (verbose) {
            cout << endl
                 << "TESTING convertDecimalToBinary" << endl
                 << "==============================" << endl;
        }

        {
            static const struct {
                int    d_lineNum;  // source line number
                Uint64 d_decFrac;  // specification decimal fraction
                int    d_decExp;   // specification decimal exponent
                int    d_fail;     // expected to fail indicator
                Uint64 d_binFrac;  // expected binary fraction
                int    d_binExp;   // expected binary exponent
            } DATA[] = {
//line decFrac                decExp      fail binFrac                binExp
//---- ---------------------  ----------  ---- ---------------------  ------
{ L_,  000000000000000000uLL,          0,   0, 0x0000000000000000uLL,     0 },

{ L_,  100000000000000000uLL,          1,   0, 0x8000000000000000uLL,   0x01 },
{ L_,  200000000000000000uLL,          1,   0, 0x8000000000000000uLL,   0x02 },
{ L_,  300000000000000000uLL,          1,   0, 0xc000000000000000uLL,   0x02 },
{ L_,  400000000000000000uLL,          1,   0, 0x8000000000000000uLL,   0x03 },
{ L_,  500000000000000000uLL,          1,   0, 0xa000000000000000uLL,   0x03 },
{ L_,  600000000000000000uLL,          1,   0, 0xc000000000000000uLL,   0x03 },
{ L_,  700000000000000000uLL,          1,   0, 0xe000000000000000uLL,   0x03 },
{ L_,  800000000000000000uLL,          1,   0, 0x8000000000000000uLL,   0x04 },
{ L_,  900000000000000000uLL,          1,   0, 0x9000000000000000uLL,   0x04 },
{ L_,  100000000000000000uLL,          2,   0, 0xa000000000000000uLL,   0x04 },
{ L_,  110000000000000000uLL,          2,   0, 0xb000000000000000uLL,   0x04 },
{ L_,  120000000000000000uLL,          2,   0, 0xc000000000000000uLL,   0x04 },
{ L_,  130000000000000000uLL,          2,   0, 0xd000000000000000uLL,   0x04 },
{ L_,  140000000000000000uLL,          2,   0, 0xe000000000000000uLL,   0x04 },
{ L_,  150000000000000000uLL,          2,   0, 0xf000000000000000uLL,   0x04 },
{ L_,  160000000000000000uLL,          2,   0, 0x8000000000000000uLL,   0x05 },
{ L_,  170000000000000000uLL,          2,   0, 0x8800000000000000uLL,   0x05 },
{ L_,  180000000000000000uLL,          2,   0, 0x9000000000000000uLL,   0x05 },
{ L_,  190000000000000000uLL,          2,   0, 0x9800000000000000uLL,   0x05 },
{ L_,  200000000000000000uLL,          2,   0, 0xa000000000000000uLL,   0x05 },
{ L_,  210000000000000000uLL,          2,   0, 0xa800000000000000uLL,   0x05 },
{ L_,  220000000000000000uLL,          2,   0, 0xb000000000000000uLL,   0x05 },
{ L_,  230000000000000000uLL,          2,   0, 0xb800000000000000uLL,   0x05 },
{ L_,  240000000000000000uLL,          2,   0, 0xc000000000000000uLL,   0x05 },
{ L_,  250000000000000000uLL,          2,   0, 0xc800000000000000uLL,   0x05 },
{ L_,  260000000000000000uLL,          2,   0, 0xd000000000000000uLL,   0x05 },
{ L_,  270000000000000000uLL,          2,   0, 0xd800000000000000uLL,   0x05 },
{ L_,  280000000000000000uLL,          2,   0, 0xe000000000000000uLL,   0x05 },
{ L_,  290000000000000000uLL,          2,   0, 0xe800000000000000uLL,   0x05 },
{ L_,  300000000000000000uLL,          2,   0, 0xf000000000000000uLL,   0x05 },
{ L_,  310000000000000000uLL,          2,   0, 0xf800000000000000uLL,   0x05 },
{ L_,  320000000000000000uLL,          2,   0, 0x8000000000000000uLL,   0x06 },
{ L_,  330000000000000000uLL,          2,   0, 0x8400000000000000uLL,   0x06 },
{ L_,  340000000000000000uLL,          2,   0, 0x8800000000000000uLL,   0x06 },
{ L_,  350000000000000000uLL,          2,   0, 0x8c00000000000000uLL,   0x06 },
{ L_,  360000000000000000uLL,          2,   0, 0x9000000000000000uLL,   0x06 },
{ L_,  370000000000000000uLL,          2,   0, 0x9400000000000000uLL,   0x06 },
{ L_,  380000000000000000uLL,          2,   0, 0x9800000000000000uLL,   0x06 },
{ L_,  390000000000000000uLL,          2,   0, 0x9c00000000000000uLL,   0x06 },
{ L_,  400000000000000000uLL,          2,   0, 0xa000000000000000uLL,   0x06 },
{ L_,  410000000000000000uLL,          2,   0, 0xa400000000000000uLL,   0x06 },
{ L_,  420000000000000000uLL,          2,   0, 0xa800000000000000uLL,   0x06 },
{ L_,  430000000000000000uLL,          2,   0, 0xac00000000000000uLL,   0x06 },
{ L_,  440000000000000000uLL,          2,   0, 0xb000000000000000uLL,   0x06 },
{ L_,  450000000000000000uLL,          2,   0, 0xb400000000000000uLL,   0x06 },
{ L_,  460000000000000000uLL,          2,   0, 0xb800000000000000uLL,   0x06 },
{ L_,  470000000000000000uLL,          2,   0, 0xbc00000000000000uLL,   0x06 },
{ L_,  480000000000000000uLL,          2,   0, 0xc000000000000000uLL,   0x06 },
{ L_,  490000000000000000uLL,          2,   0, 0xc400000000000000uLL,   0x06 },
{ L_,  500000000000000000uLL,          2,   0, 0xc800000000000000uLL,   0x06 },
{ L_,  510000000000000000uLL,          2,   0, 0xcc00000000000000uLL,   0x06 },
{ L_,  520000000000000000uLL,          2,   0, 0xd000000000000000uLL,   0x06 },
{ L_,  530000000000000000uLL,          2,   0, 0xd400000000000000uLL,   0x06 },
{ L_,  540000000000000000uLL,          2,   0, 0xd800000000000000uLL,   0x06 },
{ L_,  550000000000000000uLL,          2,   0, 0xdc00000000000000uLL,   0x06 },
{ L_,  560000000000000000uLL,          2,   0, 0xe000000000000000uLL,   0x06 },
{ L_,  570000000000000000uLL,          2,   0, 0xe400000000000000uLL,   0x06 },
{ L_,  580000000000000000uLL,          2,   0, 0xe800000000000000uLL,   0x06 },
{ L_,  590000000000000000uLL,          2,   0, 0xec00000000000000uLL,   0x06 },
{ L_,  600000000000000000uLL,          2,   0, 0xf000000000000000uLL,   0x06 },
{ L_,  610000000000000000uLL,          2,   0, 0xf400000000000000uLL,   0x06 },
{ L_,  620000000000000000uLL,          2,   0, 0xf800000000000000uLL,   0x06 },
{ L_,  630000000000000000uLL,          2,   0, 0xfc00000000000000uLL,   0x06 },
{ L_,  640000000000000000uLL,          2,   0, 0x8000000000000000uLL,   0x07 },
{ L_,  650000000000000000uLL,          2,   0, 0x8200000000000000uLL,   0x07 },
{ L_,  660000000000000000uLL,          2,   0, 0x8400000000000000uLL,   0x07 },
{ L_,  670000000000000000uLL,          2,   0, 0x8600000000000000uLL,   0x07 },
{ L_,  680000000000000000uLL,          2,   0, 0x8800000000000000uLL,   0x07 },
{ L_,  690000000000000000uLL,          2,   0, 0x8a00000000000000uLL,   0x07 },
{ L_,  700000000000000000uLL,          2,   0, 0x8c00000000000000uLL,   0x07 },
{ L_,  710000000000000000uLL,          2,   0, 0x8e00000000000000uLL,   0x07 },
{ L_,  720000000000000000uLL,          2,   0, 0x9000000000000000uLL,   0x07 },
{ L_,  730000000000000000uLL,          2,   0, 0x9200000000000000uLL,   0x07 },
{ L_,  740000000000000000uLL,          2,   0, 0x9400000000000000uLL,   0x07 },
{ L_,  750000000000000000uLL,          2,   0, 0x9600000000000000uLL,   0x07 },
{ L_,  760000000000000000uLL,          2,   0, 0x9800000000000000uLL,   0x07 },
{ L_,  770000000000000000uLL,          2,   0, 0x9a00000000000000uLL,   0x07 },
{ L_,  780000000000000000uLL,          2,   0, 0x9c00000000000000uLL,   0x07 },
{ L_,  790000000000000000uLL,          2,   0, 0x9e00000000000000uLL,   0x07 },
{ L_,  800000000000000000uLL,          2,   0, 0xa000000000000000uLL,   0x07 },
{ L_,  810000000000000000uLL,          2,   0, 0xa200000000000000uLL,   0x07 },
{ L_,  820000000000000000uLL,          2,   0, 0xa400000000000000uLL,   0x07 },
{ L_,  830000000000000000uLL,          2,   0, 0xa600000000000000uLL,   0x07 },
{ L_,  840000000000000000uLL,          2,   0, 0xa800000000000000uLL,   0x07 },
{ L_,  850000000000000000uLL,          2,   0, 0xaa00000000000000uLL,   0x07 },
{ L_,  860000000000000000uLL,          2,   0, 0xac00000000000000uLL,   0x07 },
{ L_,  870000000000000000uLL,          2,   0, 0xae00000000000000uLL,   0x07 },
{ L_,  880000000000000000uLL,          2,   0, 0xb000000000000000uLL,   0x07 },
{ L_,  890000000000000000uLL,          2,   0, 0xb200000000000000uLL,   0x07 },
{ L_,  900000000000000000uLL,          2,   0, 0xb400000000000000uLL,   0x07 },
{ L_,  910000000000000000uLL,          2,   0, 0xb600000000000000uLL,   0x07 },
{ L_,  920000000000000000uLL,          2,   0, 0xb800000000000000uLL,   0x07 },
{ L_,  930000000000000000uLL,          2,   0, 0xba00000000000000uLL,   0x07 },
{ L_,  940000000000000000uLL,          2,   0, 0xbc00000000000000uLL,   0x07 },
{ L_,  950000000000000000uLL,          2,   0, 0xbe00000000000000uLL,   0x07 },
{ L_,  960000000000000000uLL,          2,   0, 0xc000000000000000uLL,   0x07 },
{ L_,  970000000000000000uLL,          2,   0, 0xc200000000000000uLL,   0x07 },
{ L_,  980000000000000000uLL,          2,   0, 0xc400000000000000uLL,   0x07 },
{ L_,  990000000000000000uLL,          2,   0, 0xc600000000000000uLL,   0x07 },
{ L_,  100000000000000000uLL,          3,   0, 0xc800000000000000uLL,   0x07 },
{ L_,  128000000000000000uLL,          3,   0, 0x8000000000000000uLL,   0x08 },
{ L_,  129000000000000000uLL,          3,   0, 0x8100000000000000uLL,   0x08 },
{ L_,  256000000000000000uLL,          3,   0, 0x8000000000000000uLL,   0x09 },
{ L_,  257000000000000000uLL,          3,   0, 0x8080000000000000uLL,   0x09 },
{ L_,  512000000000000000uLL,          3,   0, 0x8000000000000000uLL,   0x0a },
{ L_,  513000000000000000uLL,          3,   0, 0x8040000000000000uLL,   0x0a },
{ L_,  100000000000000000uLL,          4,   0, 0xfa00000000000000uLL,   0x0a },
{ L_,  102400000000000000uLL,          4,   0, 0x8000000000000000uLL,   0x0b },
{ L_,  102500000000000000uLL,          4,   0, 0x8020000000000000uLL,   0x0b },
{ L_,  204800000000000000uLL,          4,   0, 0x8000000000000000uLL,   0x0c },
{ L_,  204900000000000000uLL,          4,   0, 0x8010000000000000uLL,   0x0c },
{ L_,  409600000000000000uLL,          4,   0, 0x8000000000000000uLL,   0x0d },
{ L_,  409700000000000000uLL,          4,   0, 0x8008000000000000uLL,   0x0d },
{ L_,  819200000000000000uLL,          4,   0, 0x8000000000000000uLL,   0x0e },
{ L_,  819300000000000000uLL,          4,   0, 0x8004000000000000uLL,   0x0e },
{ L_,  100000000000000000uLL,          5,   0, 0x9c40000000000000uLL,   0x0e },
{ L_,  163840000000000000uLL,          5,   0, 0x8000000000000000uLL,   0x0f },
{ L_,  163850000000000000uLL,          5,   0, 0x8002000000000000uLL,   0x0f },
{ L_,  327680000000000000uLL,          5,   0, 0x8000000000000000uLL,   0x10 },
{ L_,  327690000000000000uLL,          5,   0, 0x8001000000000000uLL,   0x10 },
{ L_,  655360000000000000uLL,          5,   0, 0x8000000000000000uLL,   0x11 },
{ L_,  655370000000000000uLL,          5,   0, 0x8000800000000000uLL,   0x11 },
{ L_,  100000000000000000uLL,          6,   0, 0xc350000000000000uLL,   0x11 },
{ L_,  131072000000000000uLL,          6,   0, 0x8000000000000000uLL,   0x12 },
{ L_,  131073000000000000uLL,          6,   0, 0x8000400000000000uLL,   0x12 },
{ L_,  262144000000000000uLL,          6,   0, 0x8000000000000000uLL,   0x13 },
{ L_,  262145000000000000uLL,          6,   0, 0x8000200000000000uLL,   0x13 },
{ L_,  524288000000000000uLL,          6,   0, 0x8000000000000000uLL,   0x14 },
{ L_,  524289000000000000uLL,          6,   0, 0x8000100000000000uLL,   0x14 },
{ L_,  100000000000000000uLL,          7,   0, 0xf424000000000000uLL,   0x14 },
{ L_,  104857600000000000uLL,          7,   0, 0x8000000000000000uLL,   0x15 },
{ L_,  104857700000000000uLL,          7,   0, 0x8000080000000000uLL,   0x15 },
{ L_,  209715200000000000uLL,          7,   0, 0x8000000000000000uLL,   0x16 },
{ L_,  209715300000000000uLL,          7,   0, 0x8000040000000000uLL,   0x16 },
{ L_,  419430400000000000uLL,          7,   0, 0x8000000000000000uLL,   0x17 },
{ L_,  419430500000000000uLL,          7,   0, 0x8000020000000000uLL,   0x17 },
{ L_,  838860800000000000uLL,          7,   0, 0x8000000000000000uLL,   0x18 },
{ L_,  838860900000000000uLL,          7,   0, 0x8000010000000000uLL,   0x18 },
{ L_,  100000000000000000uLL,          8,   0, 0x9896800000000000uLL,   0x18 },
{ L_,  167772160000000000uLL,          8,   0, 0x8000000000000000uLL,   0x19 },
{ L_,  167772170000000000uLL,          8,   0, 0x8000008000000000uLL,   0x19 },
{ L_,  335544320000000000uLL,          8,   0, 0x8000000000000000uLL,   0x1a },
{ L_,  335544330000000000uLL,          8,   0, 0x8000004000000000uLL,   0x1a },
{ L_,  671088640000000000uLL,          8,   0, 0x8000000000000000uLL,   0x1b },
{ L_,  671088650000000000uLL,          8,   0, 0x8000002000000000uLL,   0x1b },
{ L_,  100000000000000000uLL,          9,   0, 0xbebc200000000000uLL,   0x1b },
{ L_,  134217728000000000uLL,          9,   0, 0x8000000000000000uLL,   0x1c },
{ L_,  134217729000000000uLL,          9,   0, 0x8000001000000000uLL,   0x1c },
{ L_,  268435456000000000uLL,          9,   0, 0x8000000000000000uLL,   0x1d },
{ L_,  268435457000000000uLL,          9,   0, 0x8000000800000000uLL,   0x1d },
{ L_,  536870912000000000uLL,          9,   0, 0x8000000000000000uLL,   0x1e },
{ L_,  536870913000000000uLL,          9,   0, 0x8000000400000000uLL,   0x1e },
{ L_,  100000000000000000uLL,         10,   0, 0xee6b280000000000uLL,   0x1e },
{ L_,  107374182400000000uLL,         10,   0, 0x8000000000000000uLL,   0x1f },
{ L_,  107374182500000000uLL,         10,   0, 0x8000000200000000uLL,   0x1f },
{ L_,  214748364800000000uLL,         10,   0, 0x8000000000000000uLL,   0x20 },
{ L_,  214748364900000000uLL,         10,   0, 0x8000000100000000uLL,   0x20 },
{ L_,  429496729600000000uLL,         10,   0, 0x8000000000000000uLL,   0x21 },
{ L_,  429496729700000000uLL,         10,   0, 0x8000000080000000uLL,   0x21 },
{ L_,  858993459200000000uLL,         10,   0, 0x8000000000000000uLL,   0x22 },
{ L_,  858993459300000000uLL,         10,   0, 0x8000000040000000uLL,   0x22 },
{ L_,  100000000000000000uLL,         11,   0, 0x9502f90000000000uLL,   0x22 },
{ L_,  171798691840000000uLL,         11,   0, 0x8000000000000000uLL,   0x23 },
{ L_,  171798691850000000uLL,         11,   0, 0x8000000020000000uLL,   0x23 },
{ L_,  343597383680000000uLL,         11,   0, 0x8000000000000000uLL,   0x24 },
{ L_,  343597383690000000uLL,         11,   0, 0x8000000010000000uLL,   0x24 },
{ L_,  687194767360000000uLL,         11,   0, 0x8000000000000000uLL,   0x25 },
{ L_,  687194767370000000uLL,         11,   0, 0x8000000008000000uLL,   0x25 },
{ L_,  100000000000000000uLL,         12,   0, 0xba43b74000000000uLL,   0x25 },
{ L_,  137438953472000000uLL,         12,   0, 0x8000000000000000uLL,   0x26 },
{ L_,  137438953473000000uLL,         12,   0, 0x8000000004000000uLL,   0x26 },
{ L_,  274877906944000000uLL,         12,   0, 0x8000000000000000uLL,   0x27 },
{ L_,  274877906945000000uLL,         12,   0, 0x8000000002000000uLL,   0x27 },
{ L_,  549755813888000000uLL,         12,   0, 0x8000000000000000uLL,   0x28 },
{ L_,  549755813889000000uLL,         12,   0, 0x8000000001000000uLL,   0x28 },
{ L_,  100000000000000000uLL,         13,   0, 0xe8d4a51000000000uLL,   0x28 },
{ L_,  109951162777600000uLL,         13,   0, 0x8000000000000000uLL,   0x29 },
{ L_,  109951162777700000uLL,         13,   0, 0x8000000000800000uLL,   0x29 },
{ L_,  219902325555200000uLL,         13,   0, 0x8000000000000000uLL,   0x2a },
{ L_,  219902325555300000uLL,         13,   0, 0x8000000000400000uLL,   0x2a },
{ L_,  439804651110400000uLL,         13,   0, 0x8000000000000000uLL,   0x2b },
{ L_,  439804651110500000uLL,         13,   0, 0x8000000000200000uLL,   0x2b },
{ L_,  879609302220800000uLL,         13,   0, 0x8000000000000000uLL,   0x2c },
{ L_,  879609302220900000uLL,         13,   0, 0x8000000000100000uLL,   0x2c },
{ L_,  100000000000000000uLL,         14,   0, 0x9184e72a00000000uLL,   0x2c },
{ L_,  175921860444160000uLL,         14,   0, 0x8000000000000000uLL,   0x2d },
{ L_,  175921860444170000uLL,         14,   0, 0x8000000000080000uLL,   0x2d },
{ L_,  351843720888320000uLL,         14,   0, 0x8000000000000000uLL,   0x2e },
{ L_,  351843720888330000uLL,         14,   0, 0x8000000000040000uLL,   0x2e },
{ L_,  703687441776640000uLL,         14,   0, 0x8000000000000000uLL,   0x2f },
{ L_,  703687441776650000uLL,         14,   0, 0x8000000000020000uLL,   0x2f },
{ L_,  100000000000000000uLL,         15,   0, 0xb5e620f480000000uLL,   0x2f },
{ L_,  140737488355328000uLL,         15,   0, 0x8000000000000000uLL,   0x30 },
{ L_,  140737488355329000uLL,         15,   0, 0x8000000000010000uLL,   0x30 },
{ L_,  281474976710656000uLL,         15,   0, 0x8000000000000000uLL,   0x31 },
{ L_,  281474976710657000uLL,         15,   0, 0x8000000000008000uLL,   0x31 },
{ L_,  562949953421312000uLL,         15,   0, 0x8000000000000000uLL,   0x32 },
{ L_,  562949953421313000uLL,         15,   0, 0x8000000000004000uLL,   0x32 },
{ L_,  100000000000000000uLL,         16,   0, 0xe35fa931a0000000uLL,   0x32 },
{ L_,  112589990684262400uLL,         16,   0, 0x8000000000000000uLL,   0x33 },
{ L_,  112589990684262500uLL,         16,   0, 0x8000000000002000uLL,   0x33 },
{ L_,  112589990684263500uLL,         16,   0, 0x8000000000016000uLL,   0x33 },
{ L_,  225179981368524800uLL,         16,   0, 0x8000000000000000uLL,   0x34 },
{ L_,  225179981368524900uLL,         16,   0, 0x8000000000001000uLL,   0x34 },
{ L_,  225179981368527000uLL,         16,   0, 0x8000000000016000uLL,   0x34 },
{ L_,  450359962737049600uLL,         16,   0, 0x8000000000000000uLL,   0x35 },
{ L_,  450359962737049700uLL,         16,   0, 0x8000000000000800uLL,   0x35 },
{ L_,  450359962737054100uLL,         16,   0, 0x8000000000016800uLL,   0x35 },
{ L_,  900719925474099200uLL,         16,   0, 0x8000000000000000uLL,   0x36 },
{ L_,  900719925474108200uLL,         16,   0, 0x8000000000016800uLL,   0x36 },
{ L_,  100000000000000000uLL,         17,   0, 0x8e1bc9bf04000000uLL,   0x36 },
{ L_,  100000000000000000uLL,         18,   0, 0xb1a2bc2ec5000000uLL,   0x39 },

{ L_,  100000000000000000uLL,  646456993,   1, 0x0000000000000000uLL,      0 },
{ L_,  000000000000000001uLL, -646456992,   0, 0x0000000000000000uLL,      0 },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int ti = 0; ti < NUM_DATA ; ++ti) {
                const int    LINE    = DATA[ti].d_lineNum;
                const Uint64 DECFRAC = DATA[ti].d_decFrac;
                const int    DECEXP  = DATA[ti].d_decExp;
                const int    FAIL    = DATA[ti].d_fail;
                const Uint64 BINFRAC = DATA[ti].d_binFrac;
                const int    BINEXP  = DATA[ti].d_binExp;

                Uint64 binFrac;
                int binExp;
                int rv = bdepu_RealParserImpUtil::
                    convertDecimalToBinary(&binFrac, &binExp, DECFRAC, DECEXP);
                LOOP_ASSERT(LINE, FAIL == !!rv);

                if (!FAIL) {
                    LOOP_ASSERT(LINE, BINFRAC == binFrac);
                    LOOP_ASSERT(LINE, BINEXP  == binExp);
                }

                if (veryVerbose) {
                    if (FAIL) {
                        P_(FAIL);
                        P(rv);
                        cout << endl;
                    }
                    else {
                        P(DECFRAC);
                        P(DECEXP);
                        cout << "BINFRAC = 0x" << bsl::hex << BINFRAC
                             << bsl::dec << ", ";
                        P(BINEXP);
                        cout << "binFrac = 0x" << bsl::hex << binFrac
                             << bsl::dec << ", ";
                        P(binExp);
                    }
                }
            }
        }
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING DECIMAL FRACTION TO BINARY FRACTION
        //
        // Concerns:
        // TBD doc
        //
        // Plan:
        // TBD doc
        //
        // Testing:
        //   convertDecimalFractionToBinaryFraction(Uint64, Uint64 *);
        // --------------------------------------------------------------------

        if (verbose) {
            cout << endl
                 << "TESTING convertDecimalFractionToBinaryFraction" << endl
                 << "==============================================" << endl;
        }

        {
            static const struct {
                int    d_lineNum;  // source line number
                Uint64 d_decFrac;  // specification decimal fraction
                Uint64 d_binFrac;  // expected binary fraction
            } DATA[] = {
                //line decFrac                binFrac
                //---- ---------------------  ---------------------
                { L_,  000000000000000000uLL, 0x0000000000000000uLL },
                { L_,  500000000000000000ULL, 0x8000000000000000uLL },
                { L_,  625000000000000000ULL, 0xA000000000000000uLL },
                { L_,  750000000000000000ULL, 0xC000000000000000uLL },
                { L_,  875000000000000000ULL, 0xE000000000000000uLL },
                { L_,  937500000000000000ULL, 0xF000000000000000uLL },

                                // TBD even more vectors
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int ti = 0; ti < NUM_DATA ; ++ti) {
                const int    LINE    = DATA[ti].d_lineNum;
                const Uint64 DECFRAC = DATA[ti].d_decFrac;
                const Uint64 BINFRAC = DATA[ti].d_binFrac;

                Uint64 binFrac;
                bdepu_RealParserImpUtil::
                     convertDecimalFractionToBinaryFraction(&binFrac, DECFRAC);
                if (veryVerbose) {
                    P(DECFRAC);
                    cout << "BINFRAC = 0x" << bsl::hex << BINFRAC << bsl::dec
                         << endl;
                    cout << "binFrac = 0x" << bsl::hex << binFrac << bsl::dec
                         << endl;
                    cout << endl;
                }
                LOOP_ASSERT(LINE, BINFRAC == binFrac);
            }
        }
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING DECIMAL EXPONENT TO BINARY
        //
        // Concerns:
        // TBD doc
        //
        // Plan:
        // TBD doc
        //
        // Testing:
        //   convertDecimalExponentToBinary(int, Uint64 *, int *);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING convertDecimalExponentToBinary" << endl
                          << "======================================" << endl;

        {
            static const struct {
                int    d_lineNum;  // source line number
                int    d_decExp;   // specification decimal exponent
                int    d_fail;     // expected to fail indicator
                Uint64 d_binFrac;  // expected binary fraction
                int    d_binExp;   // expected binary exponent
            } DATA[] = {
                //line  decExp      fail  binFrac                binExp
                //----  ----------  ----  ---------------------  -----------
                { L_,            0,    0, 0x8000000000000000uLL,           1 },
                { L_,            1,    0, 0xA000000000000000uLL,           4 },
                { L_,            2,    0, 0xC800000000000000uLL,           7 },
                { L_,            3,    0, 0xFA00000000000000uLL,          10 },
                { L_,            4,    0, 0x9C40000000000000uLL,          14 },
                { L_,           -1,    0, 0xCCCCCCCCCCCCCCCCuLL,          -3 },
                { L_,           -2,    0, 0xA3D70A3D70A3D708uLL,          -6 },
                { L_,           -3,    0, 0x83126E978D4FDF38uLL,          -9 },
                { L_,           -4,    0, 0xD1B71758E2196524uLL,         -13 },
                { L_,    646456992,    0, 0xE883A0C5C3B394DBuLL,  2147483644 },
                { L_,    646456993,    1,                                    },
                { L_,   -646456992,    0, 0x8CEDD5D5E3D631BAuLL, -2147483643 },
                { L_,   -646456993,    0, 0x0000000000000000uLL,           0 },

                // TBD more vectors
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int ti = 0; ti < NUM_DATA ; ++ti) {
                const int    LINE    = DATA[ti].d_lineNum;
                const int    DECEXP  = DATA[ti].d_decExp;
                const int    FAIL    = DATA[ti].d_fail;
                const Uint64 BINFRAC = DATA[ti].d_binFrac;
                const int    BINEXP  = DATA[ti].d_binExp;

                Uint64 binFrac;
                int binExp;
                int rv = bdepu_RealParserImpUtil::
                                       convertDecimalExponentToBinary(&binFrac,
                                                                      &binExp,
                                                                      DECEXP);
                if (veryVerbose) {
                    if (FAIL) {
                        P_(FAIL);
                        P(rv);
                        cout << endl;
                    }
                    else {
                        P(DECEXP);
                        cout << "BINFRAC = 0x" << bsl::hex << BINFRAC
                             << bsl::dec << ", ";
                        P(BINEXP);
                        cout << "binFrac = 0x" << bsl::hex << binFrac
                             << bsl::dec << ", ";
                        P(binExp);
                    }
                }
                LOOP_ASSERT(LINE, FAIL == !!rv);
                LOOP_ASSERT(LINE, FAIL ? 1 : BINFRAC == binFrac);
                LOOP_ASSERT(LINE, FAIL ? 1 : BINEXP == binExp);

                if (0 == FAIL && DECEXP > -646456992 && DECEXP < 646456992) {
                    Uint64 decFrac;
                    int decExp;
                    bdepu_RealParserImpUtil::convertBinaryToDecimal(&decFrac,
                                                                    &decExp,
                                                                    binFrac,
                                                                    binExp);
                    if (veryVerbose) {
                        P_(decFrac);
                        P(decExp);
                        cout << endl;
                    }
                    if (decFrac < 500000000000000000uLL) {
                        LOOP_ASSERT(LINE, 100000000000000100uLL > decFrac);
                        LOOP_ASSERT(LINE, DECEXP + 1 == decExp);
                    }
                    else {
                        LOOP_ASSERT(LINE, 999999999999999900uLL < decFrac);
                        LOOP_ASSERT(LINE, DECEXP == decExp);
                    }
                }
            }
        }
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING BINARY TO DOUBLE
        //
        // Concerns:
        // TBD doc
        //
        // Plan:
        // TBD doc
        //
        // Testing:
        //   convertBinaryToDouble(double *, int, Uint64, int);
        // --------------------------------------------------------------------

        if (verbose) {
            cout << endl
                 << "TESTING convertBinaryToDouble" << endl
                 << "=============================" << endl;
        }

        {
            const double INITIAL_VALUE_1 = -3;  // first initial value
            const double INITIAL_VALUE_2 =  9;  // second initial value

            static const struct {
                int    d_lineNum;  // source line number
                int    d_binSign;  // specification negative sign indicator
                Uint64 d_binFrac;  // specification binary fraction
                int    d_binExp;   // specification binary exponent
                int    d_fail;     // expected to fail indicator
                double d_value;    // expected return value
            } DATA[] = {
                //line  bS  binFrac                binExp  fail  value
                //----  --  ---------------------  ------  ----  ----------
                { L_,    0, 0x0000000000000000uLL,      0,    0,       0.0 },
                { L_,    0, 0x0000000000000000uLL,   1024,    1,           },
                { L_,    0, 0x1000000000000000uLL,  -1023,    0,       0.0 },

                { L_,    0, 0x8000000000000000uLL,      1,    0,       1.0 },
                { L_,    1, 0x8000000000000000uLL,      1,    0,      -1.0 },
                { L_,    0, 0x8000000000000000uLL,      0,    0,       0.5 },
                { L_,    1, 0x8000000000000000uLL,      0,    0,      -0.5 },
                { L_,    0, 0x8000000000000000uLL,     -1,    0,      0.25 },
                { L_,    1, 0x8000000000000000uLL,     -1,    0,     -0.25 },
                { L_,    0, 0xC000000000000000uLL,      0,    0,      0.75 },
                { L_,    1, 0xC000000000000000uLL,      0,    0,     -0.75 },
                { L_,    0, 0x8000000000000000uLL,     -2,    0,     0.125 },
                { L_,    1, 0x8000000000000000uLL,     -2,    0,    -0.125 },
                { L_,    0, 0xE000000000000000uLL,      0,    0,     0.875 },
                { L_,    1, 0xE000000000000000uLL,      0,    0,    -0.875 },
                { L_,    0, 0x8000000000000000uLL,     -3,    0,    0.0625 },
                { L_,    1, 0x8000000000000000uLL,     -3,    0,   -0.0625 },
                { L_,    0, 0xF000000000000000uLL,      0,    0,    0.9375 },
                { L_,    1, 0xF000000000000000uLL,      0,    0,   -0.9375 },

                // TBD even more vectors - case 4 and case 8 should share
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int ti = 0; ti < NUM_DATA ; ++ti) {
                const int    LINE    = DATA[ti].d_lineNum;
                const int    BINSIGN = DATA[ti].d_binSign;
                const Uint64 BINFRAC = DATA[ti].d_binFrac;
                const int    BINEXP  = DATA[ti].d_binExp;
                const int    FAIL    = DATA[ti].d_fail;
                const double VALUE   = DATA[ti].d_value;

                {  // test with first initial value
                    double result = INITIAL_VALUE_1;
                    int rv = bdepu_RealParserImpUtil::
                                                 convertBinaryToDouble(&result,
                                                                       BINSIGN,
                                                                       BINFRAC,
                                                                       BINEXP);
                    LOOP_ASSERT(LINE, FAIL == !!rv);
                    LOOP_ASSERT(LINE,result == (rv ? INITIAL_VALUE_1 : VALUE));
                }

                {  // test with second initial value
                    double result = INITIAL_VALUE_2;
                    int rv = bdepu_RealParserImpUtil::
                                                 convertBinaryToDouble(&result,
                                                                       BINSIGN,
                                                                       BINFRAC,
                                                                       BINEXP);
                    LOOP_ASSERT(LINE, FAIL == !!rv);
                    LOOP_ASSERT(LINE,result == (rv ? INITIAL_VALUE_2 : VALUE));
                }
            }
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING BINARY TO DECIMAL
        //
        // Concerns:
        // TBD doc
        //
        // Plan:
        // TBD doc
        //
        // Testing:
        //   convertBinaryToDecimal(Uint64, int, Uint64 *, int *);
        // --------------------------------------------------------------------

        if (verbose) {
            cout << endl
                 << "TESTING convertBinaryToDecimal" << endl
                 << "==============================" << endl;
        }

        {
            static const struct {
                int    d_lineNum;  // source line number
                Uint64 d_binFrac;  // specification binary fraction
                int    d_binExp;   // specification binary exponent
                Uint64 d_decFrac;  // expected decimal fraction
                int    d_decExp;   // expected decimal exponent
            } DATA[] = {
        //line  binFrac                binExp  decFrac                decExp
        //----  ---------------------  ------  ---------------------  ------
        { L_,   0x0000000000000000uLL,      0, 000000000000000000ULL,      0 },
        { L_,   0x8000000000000000uLL,      0, 500000000000000000ULL,      0 },
        { L_,   0xA000000000000000uLL,      0, 625000000000000000ULL,      0 },
        { L_,   0xC000000000000000uLL,      0, 750000000000000000ULL,      0 },
        { L_,   0xE000000000000000uLL,      0, 875000000000000000ULL,      0 },
        { L_,   0xF000000000000000uLL,      0, 937500000000000000ULL,      0 },

        // TBD even more vectors
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int ti = 0; ti < NUM_DATA ; ++ti) {
                const int    LINE    = DATA[ti].d_lineNum;
                const Uint64 BINFRAC = DATA[ti].d_binFrac;
                const int    BINEXP  = DATA[ti].d_binExp;
                const Uint64 DECFRAC = DATA[ti].d_decFrac;
                const int    DECEXP  = DATA[ti].d_decExp;

                Uint64 decFrac;
                int decExp;
                bdepu_RealParserImpUtil::
                    convertBinaryToDecimal(&decFrac, &decExp, BINFRAC, BINEXP);
                LOOP_ASSERT(LINE, DECFRAC == decFrac);
                LOOP_ASSERT(LINE, DECEXP == decExp);
            }
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING BINARY FRACTION TO DECIMAL FRACTION
        //
        // Concerns:
        // TBD doc
        //
        // Plan:
        // TBD doc
        //
        // Testing:
        //   convertBinaryFractionToDecimalFraction(Uint64, Uint64 *);
        // --------------------------------------------------------------------

        if (verbose) {
            cout << endl
                 << "TESTING convertBinaryFractionToDecimalFraction" << endl
                 << "==============================================" << endl;
        }

        {
            static const struct {
                int    d_lineNum;  // source line number
                Uint64 d_binFrac;  // specification binary fraction
                Uint64 d_decFrac;  // expected decimal fraction
            } DATA[] = {
                //line  binFrac                decFrac
                //----  ---------------------  ---------------------
                { L_,   0x8000000000000000uLL, 500000000000000000ULL },
                { L_,   0xA000000000000000uLL, 625000000000000000ULL },
                { L_,   0xC000000000000000uLL, 750000000000000000ULL },
                { L_,   0xE000000000000000uLL, 875000000000000000ULL },

                // TBD more vectors
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int ti = 0; ti < NUM_DATA ; ++ti) {
                const int    LINE    = DATA[ti].d_lineNum;
                const Uint64 BINFRAC = DATA[ti].d_binFrac;
                const Uint64 DECFRAC = DATA[ti].d_decFrac;

                Uint64 decFrac;
                bdepu_RealParserImpUtil::
                     convertBinaryFractionToDecimalFraction(&decFrac, BINFRAC);
                if (veryVerbose) {
                    cout << "BINFRAC = 0x" << bsl::hex << BINFRAC << bsl::dec
                         << endl;
                    P(DECFRAC);
                    P(decFrac);
                    cout << endl;
                }
                LOOP_ASSERT(LINE, DECFRAC == decFrac);
            }
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // TESTING BINARY EXPONENT TO DECIMAL
        //
        // Concerns:
        // TBD doc
        //
        // Plan:
        // TBD doc
        //
        // Testing:
        //   convertBinaryExponentToDecimal(int, Uint64 *, int *);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING convertBinaryExponentToDecimal" << endl
                          << "======================================" << endl;

        {
            static const struct {
                int    d_lineNum;  // source line number
                int    d_binExp;   // specification binary exponent
                Uint64 d_decFrac;  // expected decimal fraction
                int    d_decExp;   // expected decimal exponent
            } DATA[] = {
                //line  binExp  decFrac                decExp
                //----  ------  ---------------------  ------
                { L_,        0, 100000000000000000ULL,      1 },
                { L_,        1, 200000000000000000ULL,      1 },
                { L_,        2, 400000000000000000ULL,      1 },
                { L_,        3, 800000000000000000ULL,      1 },
                { L_,        4, 160000000000000000ULL,      2 },
                { L_,       -1, 500000000000000000ULL,      0 },
                { L_,       -2, 250000000000000000ULL,      0 },
                { L_,       -3, 125000000000000000ULL,      0 },
                { L_,       -4, 625000000000000000ULL,     -1 },

                // TBD more vectors
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int ti = 0; ti < NUM_DATA ; ++ti) {
                const int    LINE    = DATA[ti].d_lineNum;
                const int    BINEXP  = DATA[ti].d_binExp;
                const Uint64 DECFRAC = DATA[ti].d_decFrac;
                const int    DECEXP  = DATA[ti].d_decExp;

                Uint64 decFrac;
                int decExp;
                bdepu_RealParserImpUtil::
                                       convertBinaryExponentToDecimal(&decFrac,
                                                                      &decExp,
                                                                      BINEXP);
                LOOP_ASSERT(LINE, DECFRAC == decFrac);
                LOOP_ASSERT(LINE, DECEXP == decExp);
            }
        }
      } break;
      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        cerr << "Error, non-zero test status = " << testStatus << "." << endl;
    }

    return testStatus;
}

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2003
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
