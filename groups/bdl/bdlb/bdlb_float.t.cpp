// bdlb_float.t.cpp                                                   -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------


#include <bdlb_float.h>

#include <bsls_platform.h>

#include <bsl_cstdio.h>
#include <bsl_cstdlib.h>
#include <bsl_cstring.h>
#include <bsl_cfloat.h>

#if defined(BSLS_PLATFORM_CMP_IBM)
// xlC 8 has a more-or-less C99-compliant math library that we can
// use for more thorough testing.
#include <bsl_c_math.h>
#endif

using namespace BloombergLP;

// ============================================================================
//                             TEST PLAN
// ----------------------------------------------------------------------------
//                             Overview
// ----------------------------------------------------------------------------
// These functions in this component can be tested easily by supplying values
// and testing the results.  Exhaustive test is impossible, but a reasonable
// sample will give sufficient confidence in the correctness of the
// implementation.
//
// WARNING: 'bsl::printf' is used in place of 'bsl::cout' because using
//          'bsl::cout' is causing signaling NaN to quiet NaN conversion for
//          'float' to fail in AIX optimized builds, for some odd reason.
// ----------------------------------------------------------------------------
// [2] static Classification classify(float number);
// [2] static Classification classify(double number);
// [2] static FineClassification classifyFine(float number);
// [2] static FineClassification classifyFine(double number);
// [2] static bool isZero(float number);
// [2] static bool isZero(double number);
// [2] static bool isNormal(float number);
// [2] static bool isNormal(double number);
// [2] static bool isSubnormal(float number);
// [2] static bool isSubnormal(double number);
// [2] static bool isInfinite(float number);
// [2] static bool isInfinite(double number);
// [2] static bool isNan(float number);
// [2] static bool isNan(double number);
// [2] static bool signBit(float number);
// [2] static bool signBit(double number);
// [2] static bool isFinite(float number);
// [2] static bool isFinite(double number);
// [2] static bool isQuietNan(float number);
// [2] static bool isQuietNan(double number);
// [2] static bool isSignalingNan(float number);
// [2] static bool isSignalingNan(double number);
// ----------------------------------------------------------------------------
// [1] BREATHING TEST
// [3] USAGE EXAMPLE

// ============================================================================
//              STANDARD BDE TEST DRIVER MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

static int testStatus = 0;

static void aSsErT(int c, const char *s, int i) {
    if (c) {
        bsl::printf("Error %s(%d): %s    (failed)\n", __FILE__, i, s);
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}

# define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }
// ----------------------------------------------------------------------------
#define LOOP_ASSERT(I,X) { \
    if (!(X)) { bsl::printf("%s: %x\n", #I, I);                               \
                aSsErT(1, #X, __LINE__); } }

#define LOOP4_ASSERT(I,J,K,L,X) { \
    if (!(X)) { bsl::printf("%s: %x\t%s: %x\t%s: %x\t%s: %x\n", #I, I, #J, J, \
                                                               #K, K, #L, L); \
               aSsErT(1, #X, __LINE__); } }

#define LOOP4_ASSERT_D(I,J,K,L,X) { \
    if (!(X)) { bsl::printf("%s: %x\t%s: %llx\t%s: %x\t%s: %x\n", \
                                                               #I, I, #J, J, \
                                                               #K, K, #L, L); \
               aSsErT(1, #X, __LINE__); } }

// Allow compilation of individual test-cases (for test drivers that take a
// very long time to compile).  Specify '-DSINGLE_TEST=<testcase>' to compile
// only the '<testcase>' test case.
#define TEST_IS_ENABLED(num) (! defined(SINGLE_TEST) || SINGLE_TEST == (num))

// ============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
// ----------------------------------------------------------------------------
#define Pf(X)  bsl::printf("%s = (%f)\n",   #X, X);
#define PF(X)  bsl::printf("%s = (%lf)\n",  #X, X);
#define Pd(X)  bsl::printf("%s = (%ld)\n",  #X, X);
#define Px(X)  bsl::printf("%s = (%x)\n",   #X, X);
#define PX(X)  bsl::printf("%s = (%llx)\n", #X, X);
#define Pf_(X) bsl::printf("%s = (%f)",     #X, X);
#define PF_(X) bsl::printf("%s = (%lf)",    #X, X);
#define Pd_(X) bsl::printf("%s = (%ld)",    #X, X);
#define Px_(X) bsl::printf("%s = (%x)",     #X, X);
#define PX_(X) bsl::printf("%s = (%llx)",   #X, X);

#define Q(X)   bsl::printf("<| %s |>\n",    #X);
#define L_     __LINE__                                // current Line number
#define T_     bsl::printf("\t");

// ============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------

static bool         verbose = 0;
static bool     veryVerbose = 0;
static bool veryVeryVerbose = 0;

// ============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
// ----------------------------------------------------------------------------

typedef bdlb::Float Obj;

// Some platforms do not support signaling NaNs, especially for
// single-precision floats, and will convert SNaNs to QNaNs during argument
// passing.
#if defined(BSLS_PLATFORM_CPU_POWERPC)
const bool hasFSNan = false;
const bool hasDSNan = true;
#elif defined(BSLS_PLATFORM_CPU_X86) || defined(BSLS_PLATFORM_CPU_X86_64)
// Some documentation of SNaN to QNaN conversions on x86 can be found in
// section 4.8.3.5, Intel 64 Software Developers Manual, Volume 1,
// http://download.intel.com/products/processor/manual/253665.pdf
const bool hasFSNan = false;
const bool hasDSNan = false;
#else
const bool hasFSNan = true;
const bool hasDSNan = true;
#endif

#if (defined(BSLS_PLATFORM_CPU_X86) || defined(BSLS_PLATFORM_CPU_X86_64)) && \
    defined(BSLS_PLATFORM_CMP_CLANG) && \
    defined(BDE_BUILD_TARGET_OPT)
// Both x86 and AMD processors convert SNaNs to QNaNs when certain operations
// are performed on SNaNs.  Therefore 'hasFSNan' & 'hasDSNan', above, are
// 'false'.  But, the Clang optimizer, in certain contexts, avoids using
// floating point registers and instructions, making the tests for SNaN's
// non-deterministic.  See internal-ticket D37511035.

#define OMIT_SNAN_TESTS
#endif

unsigned int floatToRep(float x)
    // Convert the specified 'x' to its integer representation.
{
    unsigned int v;
    bsl::memcpy(&v, &x, sizeof(x));
    return v;
}

unsigned long long doubleToRep(double x)
    // Convert the specified 'x' to its integer representation.
{
    unsigned long long v;
    bsl::memcpy(&v, &x, sizeof(x));
    return v;
}

float repToFloat(unsigned int x)
    // Convert the specified 'x' to a 'float' from its integer representation.
{
    float v;
    bsl::memcpy(&v, &x, sizeof(x));
    return v;
}

double repToDouble(unsigned long long x)
    // Convert the specified 'x' to a 'double' from its integer representation.
{
    double v;
    bsl::memcpy(&v, &x, sizeof(x));
    return v;
}


// Functions for zero: avoid warnings on some compilers when deliberately
// dividing by a zero constant.
float  fnzero() { return -0.0F; }
double dnzero() { return -0.0;  }
float  fzero()  { return  0.0F; }
double dzero()  { return  0.0;  }

// Functions for 'min' and 'max': Avoids constant-folding bug in 'xlC 8':
float  fmin() { return FLT_MIN; }
float  fmax() { return FLT_MAX; }
double dmin() { return DBL_MIN; }
double dmax() { return DBL_MAX; }

// Various NaN values:
static const float  FQNAN1 = repToFloat(0x7fc00000U);
static const float  FQNAN2 = repToFloat(0x7fc12345U);
static const float  FQNAN3 = repToFloat(0x7fffffffU);
static const float  FSNAN1 = repToFloat(0x7f800001U);
static const float  FSNAN2 = repToFloat(0x7fa12345U);
static const float  FSNAN3 = repToFloat(0x7fbfffffU);
static const double DQNAN1 = repToDouble(0x7ff8000000000000ULL);
static const double DQNAN2 = repToDouble(0x7ff9123456781234ULL);
static const double DQNAN3 = repToDouble(0x7fffffffffffffffULL);
static const double DSNAN1 = repToDouble(0x7ff0000000000001ULL);
static const double DSNAN2 = repToDouble(0x7ff5123456781234ULL);
static const double DSNAN3 = repToDouble(0x7ff7ffffffffffffULL);

// ============================================================================
//                  CLASSES FOR TESTING USAGE EXAMPLES
// ----------------------------------------------------------------------------


// ============================================================================
//                              MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int        test = argc > 1 ? bsl::atoi(argv[1]) : 0;
            verbose = argc > 2;
        veryVerbose = argc > 3;
    veryVeryVerbose = argc > 4;

    bsl::printf("TEST %s CASE %d\n", __FILE__, test);

    switch (test) { case 0:  // Zero is always the leading case.
      case 3: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //   Extracted from component header file.
        //
        // Concerns:
        //: 1 The usage example provided in the component header file compiles,
        //:   links, and runs as shown.
        //
        // Plan:
        //: 1 Incorporate usage example from header into test driver, remove
        //:   leading comment characters, and replace 'assert' with 'ASSERT'.
        //:   (C-1)
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) bsl::printf("\nUSAGE EXAMPLE"
                                 "\n=============\n");

///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Basic Syntax
///- - - - - - - - - - - -
// On platforms that the implement IEEE 754 standard for floating-point
// arithmetic, dividing a positive number by zero yields positive infinity and
// dividing a negative number by zero yields negative infinity.  The result of
// division by zero will therefore be detected as infinite by the 'isInfinite'
// method and classified as infinity by the 'classify' and 'classifyFine'
// methods in this component:
//..
    double zero =  0.0;
    double a    =  2.3  / zero;
    double b    = -0.55 / zero;
    ASSERT(true                             == bdlb::Float::isZero(zero));
    ASSERT(true                             == bdlb::Float::isInfinite(a));
    ASSERT(true                             == bdlb::Float::isInfinite(b));
    ASSERT(bdlb::Float::k_ZERO              == bdlb::Float::classify(zero));
    ASSERT(bdlb::Float::k_INFINITE          == bdlb::Float::classify(a));
    ASSERT(bdlb::Float::k_INFINITE          == bdlb::Float::classify(b));
    ASSERT(bdlb::Float::k_POSITIVE_INFINITY == bdlb::Float::classifyFine(a));
    ASSERT(bdlb::Float::k_NEGATIVE_INFINITY == bdlb::Float::classifyFine(b));
//..
// Note that the sign rules apply as usual:
//..
    double nzero = -0.0;
    double bn    = -0.55 / nzero;
    ASSERT(bdlb::Float::k_POSITIVE_INFINITY == bdlb::Float::classifyFine(bn));
//..
// The result of multiplying infinity by infinity is also infinity, but the
// result of multiplying infinity by zero is an indeterminate value (quiet
// NaN):
//..
    double c = a * b;
    double d = a * zero;
    ASSERT(true  == bdlb::Float::isInfinite(c));
    ASSERT(false == bdlb::Float::isInfinite(d));
    ASSERT(true  == bdlb::Float::isNan(d));
    ASSERT(true  == bdlb::Float::isQuietNan(d));
    ASSERT(false == bdlb::Float::isSignalingNan(d));
//..
// Quiet NaNs propagate such that further calculations also yield quiet NaNs:
//..
    double g = d - 3.4e12;
    ASSERT(false == bdlb::Float::isInfinite(g));
    ASSERT(true  == bdlb::Float::isNan(g));
    ASSERT(true  == bdlb::Float::isQuietNan(g));
//..
// We can also detect whether a value has full precision (normal) or is so
// small (close to zero) that precision has been lost (subnormal):
//..
    double e = -10.0 / 11.0;    // Full precision
    double f = e     / DBL_MAX; // Lost precision
    ASSERT(true                     == bdlb::Float::isNormal(e));
    ASSERT(false                    == bdlb::Float::isSubnormal(e));
    ASSERT(false                    == bdlb::Float::isNormal(f));
    ASSERT(true                     == bdlb::Float::isSubnormal(f));
    ASSERT(bdlb::Float::k_NORMAL    == bdlb::Float::classify(e));
    ASSERT(bdlb::Float::k_SUBNORMAL == bdlb::Float::classify(f));
//..
// The 'Classification' enumeration type is designed so that each
// classification occupies a separate bit.  This makes it easy to test for
// multiple classifications in one test.  For example, if we are interested in
// very that zero or denormalized (i.e., very small), we can detect both
// conditions with a single mask:
//..
    const int SMALL_MASK = bdlb::Float::k_ZERO | bdlb::Float::k_SUBNORMAL;
    ASSERT(0 != (SMALL_MASK & bdlb::Float::classify(0.0)));
    ASSERT(0 != (SMALL_MASK & bdlb::Float::classify(f)));
    ASSERT(0 == (SMALL_MASK & bdlb::Float::classify(e)));
//..
// Note, however, that although we can create a mask with several
// classification bits, a single number belongs to only one classification and
// the return value of 'classify' will have only one bit set at a time.

      } break;
      case 2: {
        // -------------------------------------------------------------------
        // CLASSIFICATION TEST
        //
        // Concerns:
        //: 1 Each classification function returns the correct values for a
        //:   wide range of inputs.
        //: 2 Each classification function works for both 'float' and 'double'
        //:   values.
        //
        // Plan:
        //: 1 Choose a set of values spanning the range of each fine-grained
        //:   classification.  Choose values near the boundaries and use
        //:   expressions that generate values with known classifications.
        //: 2 For each value, call each classification function.
        //: 3 Compare the result of each call to the expected value.
        //: 4 Perform the test for 'float' and 'double' values.
        //
        // Testing:
        //     static Classification classify(float number);
        //     static Classification classify(double number);
        //     static FineClassification classifyFine(float number);
        //     static FineClassification classifyFine(double number);
        //     static bool isZero(float number);
        //     static bool isZero(double number);
        //     static bool isNormal(float number);
        //     static bool isNormal(double number);
        //     static bool isSubnormal(float number);
        //     static bool isSubnormal(double number);
        //     static bool isInfinite(float number);
        //     static bool isInfinite(double number);
        //     static bool isNan(float number);
        //     static bool isNan(double number);
        //     static bool signBit(float number);
        //     static bool signBit(double number);
        //     static bool isFinite(float number);
        //     static bool isFinite(double number);
        //     static bool isQuietNan(float number);
        //     static bool isQuietNan(double number);
        //     static bool isSignalingNan(float number);
        //     static bool isSignalingNan(double number);
        // -------------------------------------------------------------------

        if (verbose) bsl::printf("\nCLASSIFICATION TEST"
                                 "\n===================\n");

        if (veryVerbose) bsl::printf("Testing 'float'\n");

        static const int X = -1; // Unknown result

        static const int NEGATIVE      = Obj::k_NEGATIVE;
        static const int SIGNALING     = Obj::k_SIGNALING;

        static const int POS_INFINITY  = Obj::k_POSITIVE_INFINITY;
        static const int NEG_INFINITY  = Obj::k_NEGATIVE_INFINITY;
        static const int QNAN          = Obj::k_QNAN;
        static const int SNAN          = Obj::k_SNAN;
        static const int POS_NORMAL    = Obj::k_POSITIVE_NORMAL;
        static const int NEG_NORMAL    = Obj::k_NEGATIVE_NORMAL;
        static const int POS_SUBNORMAL = Obj::k_POSITIVE_SUBNORMAL;
        static const int NEG_SUBNORMAL = Obj::k_NEGATIVE_SUBNORMAL;
        static const int POS_ZERO      = Obj::k_POSITIVE_ZERO;
        static const int NEG_ZERO      = Obj::k_NEGATIVE_ZERO;

// This macro tests a function 'bdlb::Float::f(input)', where 'f' is the name
// of one of the class functions in 'bdlb::Float'.  We assume that there is a
// local variable, 'input' holding the argument to the function and a local
// variable, 'f' holding the expected result of the function call.  If the 'f'
// variable is less than zero, then the result is considered unpredictable and
// will not be tested.  This corresponds to a value of 'X' in the test vectors.
#define FUNCTION_TEST(f) do { if (f >= 0) { \
    if (veryVeryVerbose) { Pd_(input); Pd(Obj::f(input)) } \
    int expected = (int) f; \
    int result = Obj::f(input); \
    LOOP4_ASSERT(LINE, input, expected, Obj::f(input), expected == result); \
} } while (0)

#define FUNCTION_TEST_F(f) do { if (f >= 0) { \
    if (veryVeryVerbose) { Px_(floatToRep(input)); Pd((long)Obj::f(input)) } \
    int expected = (int) f; \
    int result = Obj::f(input); \
    LOOP4_ASSERT(LINE, floatToRep(input), expected, Obj::f(input), \
                (long)(expected == result)); \
} } while (0)

        static const struct {
            int   d_line;
            float d_input;
            int   d_isZero;
            int   d_isNormal;
            int   d_isSubnormal;
            int   d_isInfinite;
            int   d_isNan;
            int   d_signBit;
            int   d_isFinite;
            int   d_isQNan;
            int   d_isSNan;
            int   d_classification;
        } FDATA[] = {
//--------------------^
//                                          Z N S     S   Q S
//                                          e o u I N i F N N
//                                          r r b n a g i a a
//Lin Input                                 o m N f N n n N N  Classification
//--- ------------------------------------- - - - - - - - - -  --------------
{ L_, 0.0F                                , 1,0,0,0,0,0,1,0,0, POS_ZERO      },
{ L_, fzero() + fnzero()                  , 1,0,0,0,0,0,1,0,0, POS_ZERO      },
{ L_, fmin() / 16777216.0F                , 1,0,0,0,0,0,1,0,0, POS_ZERO      },
{ L_, 1.2e-7F / fmax()                    , 1,0,0,0,0,0,1,0,0, POS_ZERO      },
{ L_, 1.234F                              , 0,1,0,0,0,0,1,0,0, POS_NORMAL    },
{ L_, fmax()                              , 0,1,0,0,0,0,1,0,0, POS_NORMAL    },
{ L_, fmin()                              , 0,1,0,0,0,0,1,0,0, POS_NORMAL    },
{ L_, fmin() / 2.0F                       , 0,0,1,0,0,0,1,0,0, POS_SUBNORMAL },
{ L_, fmin() / 8388608.0F                 , 0,0,1,0,0,0,1,0,0, POS_SUBNORMAL },
{ L_, 1.0F / fmax()                       , 0,0,1,0,0,0,1,0,0, POS_SUBNORMAL },
{ L_, 1.0F / 4194303.5F / fmax()          , 0,0,1,0,0,0,1,0,0, POS_SUBNORMAL },
{ L_, 1.0F / fzero()                      , 0,0,0,1,0,0,0,0,0, POS_INFINITY  },
{ L_, -1.0F / fnzero()                    , 0,0,0,1,0,0,0,0,0, POS_INFINITY  },
{ L_, 2.0F * fmax()                       , 0,0,0,1,0,0,0,0,0, POS_INFINITY  },
{ L_, 8388608.0F / fmin()                 , 0,0,0,1,0,0,0,0,0, POS_INFINITY  },
{ L_, fzero() / fzero()                   , 0,0,0,0,1,X,0,1,0, QNAN          },
{ L_, FQNAN1                              , 0,0,0,0,1,X,0,1,0, QNAN          },
{ L_, FQNAN2                              , 0,0,0,0,1,X,0,1,0, QNAN          },
{ L_, FQNAN3                              , 0,0,0,0,1,X,0,1,0, QNAN          },
{ L_, fmin() / -16777216.0F               , 1,0,0,0,0,1,1,0,0, NEG_ZERO      },
{ L_, -1.2e-7F / fmax()                   , 1,0,0,0,0,1,1,0,0, NEG_ZERO      },
{ L_, -1.234F                             , 0,1,0,0,0,1,1,0,0, NEG_NORMAL    },
{ L_, -fmax()                             , 0,1,0,0,0,1,1,0,0, NEG_NORMAL    },
{ L_, -fmin()                             , 0,1,0,0,0,1,1,0,0, NEG_NORMAL    },
{ L_, -fmin() / 2.0F                      , 0,0,1,0,0,1,1,0,0, NEG_SUBNORMAL },
{ L_, fmin() / -8388608.0F                , 0,0,1,0,0,1,1,0,0, NEG_SUBNORMAL },
{ L_, -1.0F / fmax()                      , 0,0,1,0,0,1,1,0,0, NEG_SUBNORMAL },
{ L_, 1.0F / 4194303.5F / -fmax()         , 0,0,1,0,0,1,1,0,0, NEG_SUBNORMAL },
{ L_, 1.0F / fnzero()                     , 0,0,0,1,0,1,0,0,0, NEG_INFINITY  },
{ L_, -1.0F / fzero()                     , 0,0,0,1,0,1,0,0,0, NEG_INFINITY  },
{ L_, 2.0F * -fmax()                      , 0,0,0,1,0,1,0,0,0, NEG_INFINITY  },
{ L_, -8388608.0F / fmin()                , 0,0,0,1,0,1,0,0,0, NEG_INFINITY  },
#if defined(OMIT_SNAN_TESTS)
{ L_, FSNAN1                              , 0,0,0,0,1,X,0,0,1, SNAN          },
{ L_, FSNAN2                              , 0,0,0,0,1,X,0,0,1, SNAN          },
{ L_, FSNAN3                              , 0,0,0,0,1,X,0,0,1, SNAN          },
#endif
        };
//--------------------v

        static const int NUM_FDATA = sizeof FDATA / sizeof FDATA[0];

        for (int ti = 0; ti < NUM_FDATA; ++ti) {
            int   LINE           = FDATA[ti].d_line;

#if ( defined(BSLS_PLATFORM_CPU_X86_64) || defined(BSLS_PLATFORM_CPU_X86) ) \
    && defined(BSLS_PLATFORM_CMP_GNU) && defined(BDE_BUILD_TARGET_OPT)
            // This is necessary because on linux when building 64-bit
            // optimized with gcc 4.3.5 (at least) some tests will incorrectly
            // fail if 'input' is not 'volatile'.  This probably forces a
            // narrowing of the value to a 32-bit float from the wider internal
            // processor FP registers, but we're not certain of the exact
            // mechanism.

            volatile
#endif
            float input          = FDATA[ti].d_input;
            int   isZero         = FDATA[ti].d_isZero;
            int   isNormal       = FDATA[ti].d_isNormal;
            int   isSubnormal    = FDATA[ti].d_isSubnormal;
            int   isInfinite     = FDATA[ti].d_isInfinite;
            int   isNan          = FDATA[ti].d_isNan;
            int   signBit        = FDATA[ti].d_signBit;
            int   isFinite       = FDATA[ti].d_isFinite;
            int   isQuietNan     = FDATA[ti].d_isQNan;
            int   isSignalingNan = FDATA[ti].d_isSNan;
            int   classifyFine   = FDATA[ti].d_classification;

            if (veryVeryVerbose) Pd((long)LINE);

            if (! hasFSNan) {
                // If signaling NaN not supported, convert SNaN results to
                // equivalent QNaN results.
                isQuietNan     |= isSignalingNan;
                isSignalingNan  = 0;
                classifyFine   &= ~Obj::k_SIGNALING;
            }

            int classify = classifyFine & ~(NEGATIVE | SIGNALING);

            FUNCTION_TEST_F(isZero        );
            FUNCTION_TEST_F(isNormal      );
            FUNCTION_TEST_F(isSubnormal   );
            FUNCTION_TEST_F(isInfinite    );
            FUNCTION_TEST_F(isNan         );
            FUNCTION_TEST_F(signBit       );
            FUNCTION_TEST_F(isFinite      );
            FUNCTION_TEST_F(isQuietNan    );
            FUNCTION_TEST_F(isSignalingNan);
            FUNCTION_TEST_F(classify      );
            FUNCTION_TEST_F(classifyFine  );
        }

        if (veryVerbose) bsl::printf("Testing 'double'\n");

// This macro tests a function 'bdlb::Float::f(input)', where 'f' is the name
// of one of the class functions in 'bdlb::Float'.  We assume that there is a
// local variable, 'input' holding the argument to the function and a local
// variable, 'f' holding the expected result of the function call.  If the 'f'
// variable is less than zero, then the result is considered unpredictable and
// will not be tested.  This corresponds to a value of 'X' in the test vectors.
#define FUNCTION_TEST_D(f) do { if (f >= 0) { \
    if (veryVeryVerbose) { PX_(doubleToRep(input)); Pd((long)Obj::f(input)) } \
    int expected = (int) f; \
    int result = Obj::f(input); \
    LOOP4_ASSERT_D(LINE, doubleToRep(input), expected, Obj::f(input), \
                   (long)(expected == result)); \
} } while (0)

        static const struct {
            int    d_line;
            double d_input;
            int    d_isZero;
            int    d_isNormal;
            int    d_isSubnormal;
            int    d_isInfinite;
            int    d_isNan;
            int    d_signBit;
            int    d_isFinite;
            int    d_isQNan;
            int    d_isSNan;
            int    d_classification;
        } DDATA[] = {
//--------------------^
//                                          Z N S     S   Q S
//                                          e o u I N i F N N
//                                          r r b n a g i a a
//Lin Input                                 o m N f N n n N N  Classification
//--- ------------------------------------- - - - - - - - - -  --------------
{ L_, 0.0                                 , 1,0,0,0,0,0,1,0,0, POS_ZERO      },
{ L_, dzero() + dnzero()                  , 1,0,0,0,0,0,1,0,0, POS_ZERO      },
{ L_, dmin() / 9007199254740992.0         , 1,0,0,0,0,0,1,0,0, POS_ZERO      },
{ L_, 1.2e-16 / dmax()                    , 1,0,0,0,0,0,1,0,0, POS_ZERO      },
{ L_, 1.234                               , 0,1,0,0,0,0,1,0,0, POS_NORMAL    },
{ L_, dmax()                              , 0,1,0,0,0,0,1,0,0, POS_NORMAL    },
{ L_, dmin()                              , 0,1,0,0,0,0,1,0,0, POS_NORMAL    },
{ L_, dmin() / 2.0                        , 0,0,1,0,0,0,1,0,0, POS_SUBNORMAL },
{ L_, dmin() / 4503599627370496.0         , 0,0,1,0,0,0,1,0,0, POS_SUBNORMAL },
{ L_, 1.0 / dmax()                        , 0,0,1,0,0,0,1,0,0, POS_SUBNORMAL },
{ L_, 1.0 / 2251799813685248.0 / dmax()   , 0,0,1,0,0,0,1,0,0, POS_SUBNORMAL },
{ L_, 1.0 / dzero()                       , 0,0,0,1,0,0,0,0,0, POS_INFINITY  },
{ L_, -1.0 / dnzero()                     , 0,0,0,1,0,0,0,0,0, POS_INFINITY  },
{ L_, dmax() * 2.0                        , 0,0,0,1,0,0,0,0,0, POS_INFINITY  },
{ L_, 4503599627370496.0 / dmin()         , 0,0,0,1,0,0,0,0,0, POS_INFINITY  },
{ L_, dzero() / dzero()                   , 0,0,0,0,1,X,0,1,0, QNAN          },
{ L_, DQNAN1                              , 0,0,0,0,1,X,0,1,0, QNAN          },
{ L_, DQNAN2                              , 0,0,0,0,1,X,0,1,0, QNAN          },
{ L_, DQNAN3                              , 0,0,0,0,1,X,0,1,0, QNAN          },
{ L_, dmin() / -9007199254740992.0        , 1,0,0,0,0,1,1,0,0, NEG_ZERO      },
{ L_, -1.2e-16 / dmax()                   , 1,0,0,0,0,1,1,0,0, NEG_ZERO      },
{ L_, -1.234                              , 0,1,0,0,0,1,1,0,0, NEG_NORMAL    },
{ L_, -dmax()                             , 0,1,0,0,0,1,1,0,0, NEG_NORMAL    },
{ L_, -dmin()                             , 0,1,0,0,0,1,1,0,0, NEG_NORMAL    },
{ L_, -dmin() / 2.0                       , 0,0,1,0,0,1,1,0,0, NEG_SUBNORMAL },
{ L_, dmin() / -4503599627370496.0        , 0,0,1,0,0,1,1,0,0, NEG_SUBNORMAL },
{ L_, -1.0 / dmax()                       , 0,0,1,0,0,1,1,0,0, NEG_SUBNORMAL },
{ L_, 1.0 / -2251799813685248.0 / dmax()  , 0,0,1,0,0,1,1,0,0, NEG_SUBNORMAL },
{ L_, 1.0 / dnzero()                      , 0,0,0,1,0,1,0,0,0, NEG_INFINITY  },
{ L_, -1.0 / dzero()                      , 0,0,0,1,0,1,0,0,0, NEG_INFINITY  },
{ L_, 2.0 * -dmax()                       , 0,0,0,1,0,1,0,0,0, NEG_INFINITY  },
{ L_, 4503599627370496.0 / -dmin()        , 0,0,0,1,0,1,0,0,0, NEG_INFINITY  },
#if defined(OMIT_SNAN_TESTS)
{ L_, DSNAN1                              , 0,0,0,0,1,X,0,0,1, SNAN          },
{ L_, DSNAN2                              , 0,0,0,0,1,X,0,0,1, SNAN          },
{ L_, DSNAN3                              , 0,0,0,0,1,X,0,0,1, SNAN          },
#endif
        };
//--------------------v

        static const int NUM_DDATA = sizeof DDATA / sizeof DDATA[0];

        for (int ti = 0; ti < NUM_DDATA; ++ti) {
            int    LINE           = DDATA[ti].d_line;

#if ( defined(BSLS_PLATFORM_CPU_X86_64) || defined(BSLS_PLATFORM_CPU_X86) ) \
    && defined(BSLS_PLATFORM_CMP_GNU) && defined(BDE_BUILD_TARGET_OPT)
            // This is necessary because on linux when building 64-bit
            // optimized with gcc 4.3.5 (at least) some 'float' tests will
            // incorrectly fail if 'input' is not 'volatile'.  The problem does
            // not occur for 'double' yet, but we are adding the qualifier here
            // for symmetry.

            volatile
#endif
            double input          = DDATA[ti].d_input;
            int    isZero         = DDATA[ti].d_isZero;
            int    isNormal       = DDATA[ti].d_isNormal;
            int    isSubnormal    = DDATA[ti].d_isSubnormal;
            int    isInfinite     = DDATA[ti].d_isInfinite;
            int    isNan          = DDATA[ti].d_isNan;
            int    signBit        = DDATA[ti].d_signBit;
            int    isFinite       = DDATA[ti].d_isFinite;
            int    isQuietNan     = DDATA[ti].d_isQNan;
            int    isSignalingNan = DDATA[ti].d_isSNan;
            int    classifyFine   = DDATA[ti].d_classification;

            if (veryVeryVerbose) Pd((long)LINE);

            if (! hasDSNan) {
                // If signaling NaN not supported, convert SNaN results to
                // equivalent QNaN results.
                isQuietNan     |= isSignalingNan;
                isSignalingNan  = 0;
                classifyFine    &= ~Obj::k_SIGNALING;
            }

            int classify = classifyFine & ~(NEGATIVE | SIGNALING);

            FUNCTION_TEST_D(isZero        );
            FUNCTION_TEST_D(isNormal      );
            FUNCTION_TEST_D(isSubnormal   );
            FUNCTION_TEST_D(isInfinite    );
            FUNCTION_TEST_D(isNan         );
            FUNCTION_TEST_D(signBit       );
            FUNCTION_TEST_D(isFinite      );
            FUNCTION_TEST_D(isQuietNan    );
            FUNCTION_TEST_D(isSignalingNan);
            FUNCTION_TEST_D(classify      );
            FUNCTION_TEST_D(classifyFine  );
        }
      } break;
      case 1: {
        // -------------------------------------------------------------------
        // BREATHING TEST
        //   This case exercises (but does not fully test) basic functionality.
        //
        // Concerns:
        //: 1 The class is sufficiently functional to enable comprehensive
        //:   testing in subsequent test cases.
        //
        // Plan:
        //: 1 Call each method using ad-hoc values and verify that the return
        //:   value is as expected.
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) bsl::printf("\nBREATHING TEST"
                                 "\n==============\n");

        // Assert that this platform uses IEC 559 (IEEE 754) format by
        // converting comparing the of several floating-point numbers against
        // their known bit-representation in IEEE 754 format.  This also tests
        // the 'toRep' and 'fromRep' functions in the test driver.
        ASSERT(floatToRep(0.0F)    == 0x00000000U);
        ASSERT(floatToRep(-0.0F)   == 0x80000000U);
        ASSERT(floatToRep(1.0F)    == 0x3f800000U);
        ASSERT(floatToRep(-1.0F)   == 0xbf800000U);
        ASSERT(floatToRep(0.0625F) == 0x3d800000U);
        ASSERT(0.5029144287109375  == repToFloat(0x3F00BF00U));
        ASSERT(doubleToRep(0.0)    == 0x0000000000000000ULL);
        ASSERT(doubleToRep(-0.0)   == 0x8000000000000000ULL);
        ASSERT(doubleToRep(1.0)    == 0x3ff0000000000000ULL);
        ASSERT(doubleToRep(-1.0)   == 0xbff0000000000000ULL);
        ASSERT(doubleToRep(0.0625) == 0x3fb0000000000000ULL);
        ASSERT(0.5029144287109375  == repToDouble(0x3FE017E000000000ULL));

//      // Can't use bsl::numeric_limits because this component lives
//      // exists below STL in the dependency hierarchy:
//         using bsl::numeric_limits;
//         float  finf  = numeric_limits<float>::infinity();
//         double dinf  = numeric_limits<double>::infinity();
//         float  fnan  = numeric_limits<float>::quiet_NaN();
//         double dnan  = numeric_limits<double>::quiet_NaN();
//         float  fsnan = numeric_limits<float>::signaling_NaN();
//         double dsnan = numeric_limits<double>::signaling_NaN();
        const float  finf  = 1.0F / fzero(); // divide-by-zero yields infinity
        const double dinf  = 1.0  / dzero(); // divide-by-zero yields infinity
        const float  fnan  = finf * fzero(); // infinity * zero yields NaN
        const double dnan  = dinf * dzero(); // infinity * zero yields NaN

        // Construct signaling NaNs from infinity by tampering with the binary
        // representation.
        const unsigned int       fsnan_rep = floatToRep(finf) | 1;
        const unsigned long long dsnan_rep = doubleToRep(dinf) | 1;
        const float              fsnan     = repToFloat(fsnan_rep);
        const double             dsnan     = repToDouble(dsnan_rep);

        float  f =   3.0;
        double d = -34;

        ASSERT(  Obj::isInfinite(finf));
        ASSERT(  Obj::isInfinite(dinf));
        ASSERT(! Obj::isInfinite(fnan));
        ASSERT(! Obj::isInfinite(dnan));
        ASSERT(! Obj::isInfinite(fsnan));
        ASSERT(! Obj::isInfinite(dsnan));
        ASSERT(! Obj::isInfinite(f));
        ASSERT(! Obj::isInfinite(d));

        ASSERT(! Obj::isNan(finf));
        ASSERT(! Obj::isNan(dinf));
        ASSERT(  Obj::isNan(fnan));
        ASSERT(  Obj::isNan(dnan));
        ASSERT(  Obj::isNan(fsnan));
        ASSERT(  Obj::isNan(dsnan));
        ASSERT(! Obj::isNan(f));
        ASSERT(! Obj::isNan(d));

        ASSERT(! Obj::isQuietNan(finf));
        ASSERT(! Obj::isQuietNan(dinf));
        ASSERT(  Obj::isQuietNan(fnan));
        ASSERT(  Obj::isQuietNan(dnan));

#if defined(OMIT_SNAN_TESTS)
        LOOP_ASSERT(hasFSNan, hasFSNan != Obj::isQuietNan(fsnan));
        LOOP_ASSERT(hasDSNan, hasDSNan != Obj::isQuietNan(dsnan));
#endif
        ASSERT(! Obj::isQuietNan(f));
        ASSERT(! Obj::isQuietNan(d));

        ASSERT(! Obj::isSignalingNan(finf));
        ASSERT(! Obj::isSignalingNan(dinf));
        ASSERT(! Obj::isSignalingNan(fnan));
        ASSERT(! Obj::isSignalingNan(dnan));

#if defined(OMIT_SNAN_TESTS)
        LOOP_ASSERT(hasFSNan, hasFSNan == Obj::isSignalingNan(fsnan));
        LOOP_ASSERT(hasDSNan, hasDSNan == Obj::isSignalingNan(dsnan));
#endif

        ASSERT(! Obj::isSignalingNan(f));
        ASSERT(! Obj::isSignalingNan(d));

        ASSERT(  Obj::isInfinite(-finf));
        ASSERT(  Obj::isInfinite(-dinf));
        ASSERT(! Obj::isInfinite(-fnan));
        ASSERT(! Obj::isInfinite(-dnan));
        ASSERT(! Obj::isInfinite(-f));
        ASSERT(! Obj::isInfinite(-d));

        ASSERT(! Obj::isNan(-finf));
        ASSERT(! Obj::isNan(-dinf));
        ASSERT(  Obj::isNan(-fnan));
        ASSERT(  Obj::isNan(-dnan));
        ASSERT(! Obj::isNan(-f));
        ASSERT(! Obj::isNan(-d));

        ASSERT(! Obj::isQuietNan(-finf));
        ASSERT(! Obj::isQuietNan(-dinf));
        ASSERT(  Obj::isQuietNan(-fnan));
        ASSERT(  Obj::isQuietNan(-dnan));
        ASSERT(! Obj::isQuietNan(-f));
        ASSERT(! Obj::isQuietNan(-d));

        ASSERT(! Obj::isSignalingNan(-finf));
        ASSERT(! Obj::isSignalingNan(-dinf));
        ASSERT(! Obj::isSignalingNan(-fnan));
        ASSERT(! Obj::isSignalingNan(-dnan));
        ASSERT(! Obj::isSignalingNan(-f));
        ASSERT(! Obj::isSignalingNan(-d));

#if defined(BSLS_PLATFORM_CMP_IBM)
        // Extra testing to ensure consistency with native C math library
        ASSERT(  isinf(finf));
        ASSERT(  isinf(dinf));
        ASSERT(! isinf(fnan));
        ASSERT(! isinf(dnan));
        ASSERT(! isinf(fsnan));
        ASSERT(! isinf(dsnan));
        ASSERT(! isinf(f));
        ASSERT(! isinf(d));

        ASSERT(! isnan(finf));
        ASSERT(! isnan(dinf));
        ASSERT(  isnan(fnan));
        ASSERT(  isnan(dnan));
        ASSERT(  isnan(fsnan));
        ASSERT(  isnan(dsnan));
        ASSERT(! isnan(f));
        ASSERT(! isnan(d));
#endif

        if (veryVerbose) {
            Pd((long)hasFSNan);
            Pd((long)hasDSNan);

            Pf(finf);
            PF(dinf);
            Pf(fnan);
            PF(dnan);
            Pf(fsnan);
            PF(dsnan);
            Pf(f);
            PF(d);

            Px( floatToRep(finf));
            PX(doubleToRep(dinf));
            Px( floatToRep(fnan));
            PX(doubleToRep(dnan));
            Px( floatToRep(fsnan));
            PX(doubleToRep(dsnan));
            Px( floatToRep(f));
            PX(doubleToRep(d));
        }
      } break;
      default: {
        bsl::printf("WARNING: CASE '%d' NOT FOUND.\n", test);
        testStatus = -1;
      } break;
    }

    if (testStatus > 0) {
        bsl::printf("Error, non-zero test status = %d.\n", test);
    }

    return testStatus;
}

// ----------------------------------------------------------------------------
// Copyright 2015 Bloomberg Finance L.P.
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
