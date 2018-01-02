// bdldfp_decimalplatform.t.cpp                                       -*-C++-*-

#include <bdldfp_decimalplatform.h>

#include <bslim_testutil.h>

#include <bslmf_issame.h>

#include <bsl_iostream.h>
#include <bsl_cstdlib.h>
#include <bsl_cstring.h>

#include <string.h>

using namespace BloombergLP;
using bsl::atoi;
using bsl::cerr;
using bsl::cout;
using bsl::endl;
using bsl::flush;
using bsl::memcmp;

// ============================================================================
//                                 TEST PLAN
// ----------------------------------------------------------------------------
//                                  Overview
//                                  --------
// TBD:
// ----------------------------------------------------------------------------
// CREATORS
//
// MANIPULATORS
//
// ACCESSORS
//
// FREE OPERATORS
//
// TRAITS
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 2] Testing Apparatus
// [ 3] Testing Apparatus
// [ 4] Macro sanity test
// ----------------------------------------------------------------------------

// ============================================================================
//                     STANDARD BDE ASSERT TEST FUNCTION
// ----------------------------------------------------------------------------

namespace {

int testStatus = 0;

void aSsErT(bool condition, const char *message, int line)
{
    if (condition) {
        cout << "Error " __FILE__ "(" << line << "): " << message
             << "    (failed)" << endl;

        if (0 <= testStatus && testStatus <= 100) {
            ++testStatus;
        }
    }
}

}  // close unnamed namespace

// ============================================================================
//               STANDARD BDE TEST DRIVER MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT       BSLIM_TESTUTIL_ASSERT
#define ASSERTV      BSLIM_TESTUTIL_ASSERTV

#define LOOP_ASSERT  BSLIM_TESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLIM_TESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLIM_TESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLIM_TESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLIM_TESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLIM_TESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLIM_TESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLIM_TESTUTIL_LOOP6_ASSERT

#define Q            BSLIM_TESTUTIL_Q   // Quote identifier literally.
#define P            BSLIM_TESTUTIL_P   // Print identifier and value.
#define P_           BSLIM_TESTUTIL_P_  // P(X) without '\n'.
#define T_           BSLIM_TESTUTIL_T_  // Print a tab (w/o newline).
#define L_           BSLIM_TESTUTIL_L_  // current Line number

// ============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------

// ============================================================================
//                               USAGE EXAMPLE
// ----------------------------------------------------------------------------

namespace UsageExample {
  // TBD
}  // close namespace UsageExample

// ============================================================================
//              GLOBAL HELPER FUNCTIONS AND CLASSES FOR TESTING
// ----------------------------------------------------------------------------

template<class LHS_TYPE, class RHS_TYPE>
bool differentTypes(const LHS_TYPE&, const RHS_TYPE&)
    // Return true if the two passed expressions have the same type, and false
    // otherwise.  Two expressions have the same type only if the
    // 'bsl::is_same<L, R>' meta function evaluates to true.  Note that this
    // function is an adaptor to allow for template deduction of expression
    // types, since C++98 lacks decltype.
{
    return !bsl::is_same<LHS_TYPE, RHS_TYPE>::value;
}

template<class TYPE>
bool differentBits(const TYPE lhs, const TYPE rhs)
    // Return true if the two passed expressions, specified by 'lhs' and 'rhs',
    // (which will have the same type) have the same internal binary
    // representation.  Note that this function adapts 'memcmp' to facilitate
    // comparing r-value temporaries.
{
    return bsl::memcmp(&lhs, &rhs, sizeof(TYPE));
}


// ============================================================================
//                              MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char* argv[])
{
    int            test = argc > 1 ? atoi(argv[1]) : 0;
    int         verbose = argc > 2;
//    int     veryVerbose = argc > 3;


    cout << "TEST " << __FILE__ << " CASE " << test << endl;
    switch (test) { case 0:
    case 4: {
        // --------------------------------------------------------------------
        // Testing:
        //   Macro sanity test
        //
        // Concerns:
        //: 1 Pairs of macros that promise mutally exclusive values (1 and 0)
        //:   actually have mutually exclusive values
        //:
        //: 2 All promised macros are present.
        //:
        //: 3 Any macros which are optional are present as a full group.
        //:
        //: 4 Any macros which evaluate to an expression produce the correct
        //:   expression.  (These are the NaN variants)
        //:
        //:   1 Any NaN expression should compare inequal to itself.
        //:
        //:   2 Any NaN expressions which are intended for different types
        //:     should actually have distinct types at compile time.
        //:
        //:   3 Any NaN expressions which are intended for the same type should
        //:     have the same compile-time type.
        //:
        //:   4 Any two NaN expressions for the same type with different
        //:     semantics should have a different bit pattern (representation),
        //:     thus implying different runtime semantics.
        //:
        //:   5 [Untestable at this time] Any NaN expressions evaluates to a
        //:     value with the intended semantics (i.e.: signalling vs.
        //:     quiet).
        //:
        //:   6 All implementation modes have correct and expected
        //:     implementation detailed macros.
        //
        // Plan:
        //: 1 Assert on unexpected combinations. (C1-2, C6)
        //:
        //: 2 (Optionally compiled) Enumerate all pairings of optional NaN
        //:   expression variants for correct type and no identical bit
        //:   patterns between different expressions. (C3, C4)
        // --------------------------------------------------------------------
        if (verbose) bsl::cout << "\nTesting for Valid Configuration"
                               << "\n==============================="
                               << bsl::endl;

        #ifndef BDLDFP_DECIMALPLATFORM_SNPRINTF_BUFFER_SIZE
            ASSERT(!"BDLDFP_DECIMALPLATFORM_SNPRINTF_BUFFER_SIZE"
                    " is not defined!");
        #endif

        // NaN macros must exist, and be valid NaN objects, by not comparing
        // equal:

        #ifdef BDLDFP_DECIMALPLATFORM_C99_TR
            #ifndef BDLDFP_DECIMALPLATFORM_C99_QNAN32
                ASSERT(!"BDLDFP_DECIMALPLATFORM_C99_QNAN32 is not defined!");
            #else
                ASSERT(BDLDFP_DECIMALPLATFORM_C99_QNAN32
                                         != BDLDFP_DECIMALPLATFORM_C99_QNAN32);
            #endif
            #ifndef BDLDFP_DECIMALPLATFORM_C99_QNAN64
                ASSERT(!"BDLDFP_DECIMALPLATFORM_C99_QNAN64 is not defined!");
            #else
                ASSERT(BDLDFP_DECIMALPLATFORM_C99_QNAN64
                                         != BDLDFP_DECIMALPLATFORM_C99_QNAN64);
            #endif
            #ifndef BDLDFP_DECIMALPLATFORM_C99_QNAN128
                ASSERT(!"BDLDFP_DECIMALPLATFORM_C99_QNAN128 is not defined!");
            #else
                ASSERT(BDLDFP_DECIMALPLATFORM_C99_QNAN128
                                        != BDLDFP_DECIMALPLATFORM_C99_QNAN128);
            #endif
            #ifndef BDLDFP_DECIMALPLATFORM_C99_SNAN32
                ASSERT(!"BDLDFP_DECIMALPLATFORM_C99_SNAN32 is not defined!");
            #else
                ASSERT(BDLDFP_DECIMALPLATFORM_C99_SNAN32
                                         != BDLDFP_DECIMALPLATFORM_C99_SNAN32);
            #endif
            #ifndef BDLDFP_DECIMALPLATFORM_C99_SNAN64
                ASSERT(!"BDLDFP_DECIMALPLATFORM_C99_SNAN64 is not defined!");
            #else
                ASSERT(BDLDFP_DECIMALPLATFORM_C99_SNAN64
                                         != BDLDFP_DECIMALPLATFORM_C99_SNAN64);
            #endif
            #ifndef BDLDFP_DECIMALPLATFORM_C99_SNAN128
                ASSERT(!"BDLDFP_DECIMALPLATFORM_C99_SNAN128 is not defined!");
            #else
                ASSERT(BDLDFP_DECIMALPLATFORM_C99_SNAN128
                                        != BDLDFP_DECIMALPLATFORM_C99_SNAN128);
            #endif

            #if defined(BDLDFP_DECIMALPLATFORM_C99_QNAN32) \
             && defined(BDLDFP_DECIMALPLATFORM_C99_SNAN32) \
             && defined(BDLDFP_DECIMALPLATFORM_C99_QNAN64) \
             && defined(BDLDFP_DECIMALPLATFORM_C99_SNAN64) \
             && defined(BDLDFP_DECIMALPLATFORM_C99_QNAN128) \
             && defined(BDLDFP_DECIMALPLATFORM_C99_SNAN128)

                // qNaN32 must be different from all other larger variants
                ASSERT(differentTypes(BDLDFP_DECIMALPLATFORM_C99_QNAN32,
                                      BDLDFP_DECIMALPLATFORM_C99_QNAN64));

                ASSERT(differentTypes(BDLDFP_DECIMALPLATFORM_C99_QNAN32,
                                      BDLDFP_DECIMALPLATFORM_C99_SNAN64));

                ASSERT(differentTypes(BDLDFP_DECIMALPLATFORM_C99_QNAN32,
                                      BDLDFP_DECIMALPLATFORM_C99_QNAN128));

                ASSERT(differentTypes(BDLDFP_DECIMALPLATFORM_C99_QNAN32,
                                      BDLDFP_DECIMALPLATFORM_C99_SNAN128));


                // sNaN32 must be different from all other larger variants
                ASSERT(differentTypes(BDLDFP_DECIMALPLATFORM_C99_SNAN32,
                                      BDLDFP_DECIMALPLATFORM_C99_QNAN64));

                ASSERT(differentTypes(BDLDFP_DECIMALPLATFORM_C99_SNAN32,
                                      BDLDFP_DECIMALPLATFORM_C99_SNAN64));

                ASSERT(differentTypes(BDLDFP_DECIMALPLATFORM_C99_SNAN32,
                                      BDLDFP_DECIMALPLATFORM_C99_QNAN128));

                ASSERT(differentTypes(BDLDFP_DECIMALPLATFORM_C99_SNAN32,
                                      BDLDFP_DECIMALPLATFORM_C99_SNAN128));

                // qNaN64 must be different from all other larger variants

                ASSERT(differentTypes(BDLDFP_DECIMALPLATFORM_C99_QNAN64,
                                      BDLDFP_DECIMALPLATFORM_C99_QNAN128));

                ASSERT(differentTypes(BDLDFP_DECIMALPLATFORM_C99_QNAN64,
                                      BDLDFP_DECIMALPLATFORM_C99_SNAN128));

                // sNaN64 must be different from all other larger variants

                ASSERT(differentTypes(BDLDFP_DECIMALPLATFORM_C99_QNAN64,
                                      BDLDFP_DECIMALPLATFORM_C99_QNAN128));

                ASSERT(differentTypes(BDLDFP_DECIMALPLATFORM_C99_QNAN64,
                                      BDLDFP_DECIMALPLATFORM_C99_SNAN128));


                // sNaN and qNaN expressions should be different bit patterns:

                ASSERT(differentBits(BDLDFP_DECIMALPLATFORM_C99_SNAN32,
                                     BDLDFP_DECIMALPLATFORM_C99_QNAN32));

                ASSERT(differentBits(BDLDFP_DECIMALPLATFORM_C99_SNAN64,
                                     BDLDFP_DECIMALPLATFORM_C99_QNAN64));

                ASSERT(differentBits(BDLDFP_DECIMALPLATFORM_C99_SNAN128,
                                     BDLDFP_DECIMALPLATFORM_C99_QNAN128));

            #endif
        #endif
    } break;

    case 3: {
        // --------------------------------------------------------------------
        // TESTING
        //      Testing Apparatus
        //
        // Concerns:
        //: 1 Identical expressions (yielding identical bit patterns) should
        //:   compare the same.
        //:
        //: 2 Differing expressions (yielding different bit patterns) should
        //:   compare differently.
        //
        // Plan:
        //: 1 'int', 'float', and 'bool' expressions will be tested.  (The
        //:   function is an adaptible template.)
        //:
        //: 2 Boolean conjunction will be used as the expression operator for
        //:   'bool'.
        //:
        //: 3 Addition will be used as the expression operator for 'int' and
        //:   'float'.
        //:
        //: 4 Types will only be tested against themselves.  (No other forms
        //:   should compile.)
        //:
        //: 5 All permutations of 1 and 2 variable-combining expressions will
        //:   be tested for correctness, among all 3 permutations of types.
        //:
        //: 6 Two representative values will be tested for each above case (two
        //:   different expressions).  All permutations of these values will be
        //:   tested.
        // --------------------------------------------------------------------

        // int, int:
        ASSERT(!differentBits( 1,  1));
        ASSERT( differentBits( 1, 42));
        ASSERT( differentBits(42,  1));
        ASSERT(!differentBits(42, 42));

        ASSERT( differentBits( 1, 1 +  1));
        ASSERT( differentBits( 1, 1 + 42));
        ASSERT( differentBits(42, 1 +  1));
        ASSERT( differentBits(42, 1 + 42));

        ASSERT(!differentBits( 2, 1 +  1));
        ASSERT(!differentBits(43, 1 + 42));

        ASSERT( differentBits(1 +  1,  1));
        ASSERT( differentBits(1 +  1, 42));
        ASSERT( differentBits(1 + 42,  1));
        ASSERT( differentBits(1 + 42, 42));

        ASSERT(!differentBits(1 +  1,  2));
        ASSERT(!differentBits(1 + 42, 43));

        ASSERT(!differentBits(1 +  1, 1 +  1));
        ASSERT( differentBits(1 +  1, 1 + 42));
        ASSERT( differentBits(1 + 42, 1 +  1));
        ASSERT(!differentBits(1 + 42, 1 + 42));
    } break;

    case 2: {
        // --------------------------------------------------------------------
        // Testing:
        //   Testing Apparatus
        //
        // Concerns:
        //: 1 Same types should compare same.
        //:
        //: 2 Different types should compare different.
        //:
        //: 3 Convertible/compatible types should compare different.
        //:
        //: 4 Different types on lhs and rhs should compare different, in both
        //:   permutations.
        //:
        //: 5 Complicated expressions should have correct type evaluation.
        //
        // Plan:
        //: 1 'int's, 'float's, and 'bool's will be tested as three distinct
        //:   types.
        //:
        //: 2 Boolean conjunction will be used as the expression operator for
        //:   'bool's.
        //:
        //: 3 Addition will be used as the expression operator for 'int's and
        //:   'float's.
        //:
        //: 4 All permutation pairings of the three types will be tested for
        //:   correctness with the contract:
        //:   o int,   int
        //:   o int,   bool
        //:   o int,   float
        //:   o bool,  int
        //:   o bool,  bool
        //:   o bool,  float
        //:   o float, int
        //:   o float, bool
        //:   o float, float
        //:
        //: 5 All permutations of 1 and 2 variable-combining expressions will
        //:   be tested for correctness, among all 9 permutations of types.
        //:
        //: 6 Two representative values will be tested for each above case (two
        //:   different expressions).  All permutations of these values will be
        //:   tested.
        // --------------------------------------------------------------------

        // int, int:
        ASSERT(!differentTypes( 1,  1));
        ASSERT(!differentTypes( 1, 42));
        ASSERT(!differentTypes(42,  1));
        ASSERT(!differentTypes(42, 42));

        ASSERT(!differentTypes( 1, 1 +  1));
        ASSERT(!differentTypes( 1, 1 + 42));
        ASSERT(!differentTypes(42, 1 +  1));
        ASSERT(!differentTypes(42, 1 + 42));

        ASSERT(!differentTypes(1 +  1,  1));
        ASSERT(!differentTypes(1 +  1, 42));
        ASSERT(!differentTypes(1 + 42,  1));
        ASSERT(!differentTypes(1 + 42, 42));

        ASSERT(!differentTypes(1 +  1, 1 +  1));
        ASSERT(!differentTypes(1 +  1, 1 + 42));
        ASSERT(!differentTypes(1 + 42, 1 +  1));
        ASSERT(!differentTypes(1 + 42, 1 + 42));

        // int, bool:
        ASSERT( differentTypes( 1,  true));
        ASSERT( differentTypes( 1, false));
        ASSERT( differentTypes(42,  true));
        ASSERT( differentTypes(42, false));

        ASSERT( differentTypes( 1, true &&  true));
        ASSERT( differentTypes( 1, true && false));
        ASSERT( differentTypes(42, true &&  true));
        ASSERT( differentTypes(42, true && false));

        ASSERT( differentTypes(1 +  1,  true));
        ASSERT( differentTypes(1 +  1, false));
        ASSERT( differentTypes(1 + 42,  true));
        ASSERT( differentTypes(1 + 42, false));

        ASSERT( differentTypes(1 +  1, true &&  true));
        ASSERT( differentTypes(1 +  1, true && false));
        ASSERT( differentTypes(1 + 42, true &&  true));
        ASSERT( differentTypes(1 + 42, true && false));

        // int, float:
        ASSERT( differentTypes( 1,  1.0f));
        ASSERT( differentTypes( 1, 42.0f));
        ASSERT( differentTypes(42,  1.0f));
        ASSERT( differentTypes(42, 42.0f));

        ASSERT( differentTypes( 1, 1.0f +  1.0f));
        ASSERT( differentTypes( 1, 1.0f + 42.0f));
        ASSERT( differentTypes(42, 1.0f +  1.0f));
        ASSERT( differentTypes(42, 1.0f + 42.0f));

        ASSERT( differentTypes(1 +  1,  1.0f));
        ASSERT( differentTypes(1 +  1, 42.0f));
        ASSERT( differentTypes(1 + 42,  1.0f));
        ASSERT( differentTypes(1 + 42, 42.0f));

        ASSERT( differentTypes(1 +  1, 1.0f +  1.0f));
        ASSERT( differentTypes(1 +  1, 1.0f + 42.0f));
        ASSERT( differentTypes(1 + 42, 1.0f +  1.0f));
        ASSERT( differentTypes(1 + 42, 1.0f + 42.0f));

        // bool, int
        ASSERT( differentTypes( true,  1));
        ASSERT( differentTypes( true, 42));
        ASSERT( differentTypes(false,  1));
        ASSERT( differentTypes(false, 42));

        ASSERT( differentTypes( true, 1 +  1));
        ASSERT( differentTypes( true, 1 + 42));
        ASSERT( differentTypes(false, 1 +  1));
        ASSERT( differentTypes(false, 1 + 42));

        ASSERT( differentTypes( true && false,  1));
        ASSERT( differentTypes( true && false, 42));
        ASSERT( differentTypes(false &&  true,  1));
        ASSERT( differentTypes(false &&  true, 42));

        ASSERT( differentTypes( true && false, 1 +  1));
        ASSERT( differentTypes( true && false, 1 + 42));
        ASSERT( differentTypes(false &&  true, 1 +  1));
        ASSERT( differentTypes(false &&  true, 1 + 42));

        // bool, bool
        ASSERT(!differentTypes( true,  true));
        ASSERT(!differentTypes( true, false));
        ASSERT(!differentTypes(false,  true));
        ASSERT(!differentTypes(false, false));

        ASSERT(!differentTypes( true,  true && false));
        ASSERT(!differentTypes( true, false &&  true));
        ASSERT(!differentTypes(false,  true && false));
        ASSERT(!differentTypes(false, false &&  true));

        ASSERT(!differentTypes( true && false,  true));
        ASSERT(!differentTypes( true && false, false));
        ASSERT(!differentTypes(false &&  true,  true));
        ASSERT(!differentTypes(false &&  true, false));

        ASSERT(!differentTypes( true && false,  true && false));
        ASSERT(!differentTypes( true && false, false &&  true));
        ASSERT(!differentTypes(false &&  true,  true && false));
        ASSERT(!differentTypes(false &&  true, false &&  true));

        // bool, float
        ASSERT( differentTypes( true,  1.0f));
        ASSERT( differentTypes( true, 42.0f));
        ASSERT( differentTypes(false,  1.0f));
        ASSERT( differentTypes(false, 42.0f));

        ASSERT( differentTypes( true, 1.0f +  1.0f));
        ASSERT( differentTypes( true, 1.0f + 42.0f));
        ASSERT( differentTypes(false, 1.0f +  1.0f));
        ASSERT( differentTypes(false, 1.0f + 42.0f));

        ASSERT( differentTypes( true && false,  1.0f));
        ASSERT( differentTypes( true && false, 42.0f));
        ASSERT( differentTypes(false &&  true,  1.0f));
        ASSERT( differentTypes(false &&  true, 42.0f));

        ASSERT( differentTypes( true && false, 1.0f +  1.0f));
        ASSERT( differentTypes( true && false, 1.0f + 42.0f));
        ASSERT( differentTypes(false &&  true, 1.0f +  1.0f));
        ASSERT( differentTypes(false &&  true, 1.0f + 42.0f));

        // float, int
        ASSERT( differentTypes( 1.0f,  1));
        ASSERT( differentTypes( 1.0f, 42));
        ASSERT( differentTypes(42.0f,  1));
        ASSERT( differentTypes(42.0f, 42));

        ASSERT( differentTypes( 1.0f, 1 &&  1));
        ASSERT( differentTypes( 1.0f, 1 && 42));
        ASSERT( differentTypes(42.0f, 1 &&  1));
        ASSERT( differentTypes(42.0f, 1 && 42));

        ASSERT( differentTypes(1.0f +  1.0f,  1));
        ASSERT( differentTypes(1.0f +  1.0f, 42));
        ASSERT( differentTypes(1.0f + 42.0f,  1));
        ASSERT( differentTypes(1.0f + 42.0f, 42));

        ASSERT( differentTypes(1.0f +  1.0f, 1 &&  1));
        ASSERT( differentTypes(1.0f +  1.0f, 1 && 42));
        ASSERT( differentTypes(1.0f + 42.0f, 1 &&  1));
        ASSERT( differentTypes(1.0f + 42.0f, 1 && 42));

        // float, bool
        ASSERT( differentTypes( 1.0f,  true));
        ASSERT( differentTypes( 1.0f, false));
        ASSERT( differentTypes(42.0f,  true));
        ASSERT( differentTypes(42.0f, false));

        ASSERT( differentTypes( 1.0f,  true && false));
        ASSERT( differentTypes( 1.0f, false &&  true));
        ASSERT( differentTypes(42.0f,  true && false));
        ASSERT( differentTypes(42.0f, false &&  true));

        ASSERT( differentTypes(1.0f +  1.0f,  true));
        ASSERT( differentTypes(1.0f +  1.0f, false));
        ASSERT( differentTypes(1.0f + 42.0f,  true));
        ASSERT( differentTypes(1.0f + 42.0f, false));

        ASSERT( differentTypes(1.0f +  1.0f,  true && false));
        ASSERT( differentTypes(1.0f +  1.0f, false &&  true));
        ASSERT( differentTypes(1.0f + 42.0f,  true && false));
        ASSERT( differentTypes(1.0f + 42.0f, false &&  true));

        // float, float
        ASSERT(!differentTypes( 1.0f,  1.0f));
        ASSERT(!differentTypes( 1.0f, 42.0f));
        ASSERT(!differentTypes(42.0f,  1.0f));
        ASSERT(!differentTypes(42.0f, 42.0f));

        ASSERT(!differentTypes( 1.0f, 1.0f +  1.0f));
        ASSERT(!differentTypes( 1.0f, 1.0f + 42.0f));
        ASSERT(!differentTypes(42.0f, 1.0f +  1.0f));
        ASSERT(!differentTypes(42.0f, 1.0f + 42.0f));

        ASSERT(!differentTypes(1.0f +  1.0f,  1.0f));
        ASSERT(!differentTypes(1.0f +  1.0f, 42.0f));
        ASSERT(!differentTypes(1.0f + 42.0f,  1.0f));
        ASSERT(!differentTypes(1.0f + 42.0f, 42.0f));

        ASSERT(!differentTypes(1.0f +  1.0f, 1.0f +  1.0f));
        ASSERT(!differentTypes(1.0f +  1.0f, 1.0f + 42.0f));
        ASSERT(!differentTypes(1.0f + 42.0f, 1.0f +  1.0f));
        ASSERT(!differentTypes(1.0f + 42.0f, 1.0f + 42.0f));
    } break;

    case 1: {
        // --------------------------------------------------------------------
        // TESTING macros define an expected/sane configuration
        // Concerns: No configs we did not think of.
        // Plan: Assert on unexpected combinations
        // Testing:
        // --------------------------------------------------------------------
        if (verbose) bsl::cout << bsl::endl
                               << "SANE CONFIGURATION" << bsl::endl
                               << "==================" << bsl::endl;
        #ifndef BDLDFP_DECIMALPLATFORM_SNPRINTF_BUFFER_SIZE
            ASSERT(!"BDLDFP_DECIMALPLATFORM_SNPRINTF_BUFFER_SIZE"
                    " is not defined!");
        #endif

        #ifdef BDLDFP_DECIMALPLATFORM_C99_TR
            #ifndef BDLDFP_DECIMALPLATFORM_C99_QNAN32
                ASSERT(!"BDLDFP_DECIMALPLATFORM_C99_QNAN32 is not defined!");
            #endif
            #ifndef BDLDFP_DECIMALPLATFORM_C99_QNAN64
                ASSERT(!"BDLDFP_DECIMALPLATFORM_C99_QNAN64 is not defined!");
            #endif
            #ifndef BDLDFP_DECIMALPLATFORM_C99_QNAN128
                ASSERT(!"BDLDFP_DECIMALPLATFORM_C99_QNAN128 is not defined!");
            #endif
            #ifndef BDLDFP_DECIMALPLATFORM_C99_SNAN32
                ASSERT(!"BDLDFP_DECIMALPLATFORM_C99_SNAN32 is not defined!");
            #endif
            #ifndef BDLDFP_DECIMALPLATFORM_C99_SNAN64
                ASSERT(!"BDLDFP_DECIMALPLATFORM_C99_SNAN64 is not defined!");
            #endif
            #ifndef BDLDFP_DECIMALPLATFORM_C99_SNAN128
                ASSERT(!"BDLDFP_DECIMALPLATFORM_C99_SNAN128 is not defined!");
            #endif
        #endif
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

// ----------------------------------------------------------------------------
// Copyright 2014 Bloomberg Finance L.P.
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
