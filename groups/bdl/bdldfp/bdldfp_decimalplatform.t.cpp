// bdldfp_decimalplatform.t.cpp                                       -*-C++-*-

#include <bdldfp_decimalplatform.h>

#include <bsl_iostream.h>
#include <bsl_cstdlib.h>

#include <bslmf_issame.h>

using namespace BloombergLP;
using bsl::cout;
using bsl::cerr;
using bsl::flush;
using bsl::endl;
using bsl::atoi;

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
//                      STANDARD BDE ASSERT TEST MACROS
// ----------------------------------------------------------------------------

static int testStatus = 0;

static void aSsErT(int c, const char *s, int i)
{
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}
#define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

// ============================================================================
//                  STANDARD BDE LOOP-ASSERT TEST MACROS
// ----------------------------------------------------------------------------

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

#define LOOP5_ASSERT(I,J,K,L,M,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" << \
       #K << ": " << K << "\t" << #L << ": " << L << "\t" << \
       #M << ": " << M << "\n"; \
       aSsErT(1, #X, __LINE__); } }

#define LOOP6_ASSERT(I,J,K,L,M,N,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" << \
       #K << ": " << K << "\t" << #L << ": " << L << "\t" << \
       #M << ": " << M << "\t" << #N << ": " << N << "\n"; \
       aSsErT(1, #X, __LINE__); } }

// ============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
// ----------------------------------------------------------------------------

#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", " << flush; // 'P(X)' without '\n'
#define L_ __LINE__                           // current Line number
#define T_ cout << "\t" << flush;             // Print tab w/o newline.

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
    return memcmp(&lhs, &rhs, sizeof(TYPE));
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
        //
        // Plan:
        //: 1 Assert on unexpected combinations C1-2
        //:
        //: 2 (Optionally compiled) Enumerate all pairings of optional NaN
        //:   expression variants for correct type and no identical bit
        //:   patterns between different expressions. C3, C4.1-4.4
        // --------------------------------------------------------------------
        if (verbose) bsl::cout << "\nTesting for Valid Configuration"
                                << "\n==============================="
                                << bsl::endl;
        ASSERT(BDLDFP_DECIMALPLATFORM_C99_TR +
               BDLDFP_DECIMALPLATFORM_DECNUMBER == 1);

        ASSERT(BDLDFP_DECIMALPLATFORM_HARDWARE +
               BDLDFP_DECIMALPLATFORM_SOFTWARE == 1);

        ASSERT(BDLDFP_DECIMALPLATFORM_BIG_ENDIAN +
               BDLDFP_DECIMALPLATFORM_LITTLE_ENDIAN == 1);

        ASSERT(BDLDFP_DECIMALPLATFORM_DPD +
               BDLDFP_DECIMALPLATFORM_BININT == 1);

        #ifndef BDLDFP_DECIMALPLATFORM_SNPRINTF_BUFFER_SIZE
            ASSERT(!"BDLDFP_DECIMALPLATFORM_SNPRINTF_BUFFER_SIZE"
                    " is not defined!");
        #endif

        // NaN macros must exist, and be valid NaN objects, by not comparing
        // equal:

        #if BDLDFP_DECIMALPLATFORM_C99_TR
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
        ASSERT(BDLDFP_DECIMALPLATFORM_C99_TR +
               BDLDFP_DECIMALPLATFORM_DECNUMBER == 1);

        ASSERT(BDLDFP_DECIMALPLATFORM_HARDWARE +
               BDLDFP_DECIMALPLATFORM_SOFTWARE == 1);

        #ifndef BDLDFP_DECIMALPLATFORM_SNPRINTF_BUFFER_SIZE
            ASSERT(!"BDLDFP_DECIMALPLATFORM_SNPRINTF_BUFFER_SIZE"
                    " is not defined!");
        #endif

        #if BDLDFP_DECIMALPLATFORM_C99_TR
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
// Copyright (C) 2014 Bloomberg L.P.
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
