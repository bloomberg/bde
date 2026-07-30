// baljsn_converttojsonoptions.t.cpp                                  -*-C++-*-
#include <baljsn_converttojsonoptions.h>

#include <bslim_testutil.h>

#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>

#include <bsls_assert.h>
#include <bsls_asserttest.h>
#include <bsls_review.h>

#include <bsl_cstdlib.h>
#include <bsl_iostream.h>
#include <bsl_sstream.h>

using namespace BloombergLP;
using namespace bsl;

//=============================================================================
//                                 TEST PLAN
//-----------------------------------------------------------------------------
//                                 Overview
//                                 --------
// The component under test implements a single, unconstrained
// (value-semantic) attribute class.  The Primary Manipulators and Basic
// Accessors are therefore, respectively, the attribute setters and getters,
// each of which follows our standard unconstrained attribute-type naming
// conventions: `setAttributeName` and `attributeName`.
//
// Primary Manipulators:
//  - `setConvertEmptyArrays`
//  - `setConvertNullElements`
//
// Basic Accessors:
//  - `convertEmptyArrays`
//  - `convertNullElements`
//
// Certain standard value-semantic-type test cases are omitted:
//  - [ 8] -- `swap` is not implemented for this class.
//  - [10] -- BSLX streaming is not (yet) implemented for this class.
//
// Global Concerns:
//  - ACCESSOR methods are declared `const`.
//  - No memory is allocated from any allocator.
//
// Global Assumptions:
//  - ACCESSOR methods are `const` thread-safe.
//  - Individual attribute types are presumed to be *alias-safe*; hence, only
//    certain methods require the testing of this property:
//    - copy-assignment
// ----------------------------------------------------------------------------
// CREATORS
// [ 2] baljsn::ConvertToJsonOptions();
// [ 7] baljsn::ConvertToJsonOptions(const CTJO& original);
// [ 2] ~baljsn::ConvertToJsonOptions();
//
// MANIPULATORS
// [ 9] operator=(const baljsn::ConvertToJsonOptions& rhs);
// [10] STREAM& bdexStreamIn(STREAM& stream, int version);
// [ 3] setConvertEmptyArrays(bool value);
// [ 3] setConvertNullElements(bool value);
//
// ACCESSORS
// [10] STREAM& bdexStreamOut(STREAM& stream, int version) const;
// [ 4] bool convertEmptyArrays() const;
// [ 4] bool convertNullElements() const;
//
// [ 5] ostream& print(ostream& s, int level = 0, int sPL = 4) const;
//
// FREE OPERATORS
// [ 6] bool operator==(const baljsn::ConvertToJsonOptions& lhs, rhs);
// [ 6] bool operator!=(const baljsn::ConvertToJsonOptions& lhs, rhs);
// [ 5] operator<<(ostream& s, const baljsn::ConvertToJsonOptions& d);
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [11] USAGE EXAMPLE

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
//                     NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT_SAFE_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(EXPR)
#define ASSERT_SAFE_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(EXPR)
#define ASSERT_PASS(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS(EXPR)
#define ASSERT_FAIL(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL(EXPR)
#define ASSERT_OPT_PASS(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS(EXPR)
#define ASSERT_OPT_FAIL(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL(EXPR)

#define ASSERT_SAFE_PASS_RAW(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS_RAW(EXPR)
#define ASSERT_SAFE_FAIL_RAW(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL_RAW(EXPR)
#define ASSERT_PASS_RAW(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS_RAW(EXPR)
#define ASSERT_FAIL_RAW(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL_RAW(EXPR)
#define ASSERT_OPT_PASS_RAW(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(EXPR)
#define ASSERT_OPT_FAIL_RAW(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(EXPR)

// ============================================================================
//                        GLOBAL TYPEDEFS FOR TESTING
// ----------------------------------------------------------------------------

typedef baljsn::ConvertToJsonOptions Obj;

// ============================================================================
//                             GLOBAL TEST DATA
// ----------------------------------------------------------------------------

// Define DEFAULT DATA (potentially) used by test cases 3, 7, (8), 9, and (10)

struct DefaultDataRow {
    int  d_line;                // source line number
    bool d_convertEmptyArrays;
    bool d_convertNullElements;
};

const bool T = true;
const bool F = false;

static
const DefaultDataRow DEFAULT_DATA[] =
{

    //LINE   EEA  ENE
    //----   ---  ---

    // default (must be first)
    { L_,    T,   T },

    { L_,    F,   T },
    { L_,    T,   F },
    { L_,    F,   F },

};
const int DEFAULT_NUM_DATA = sizeof DEFAULT_DATA / sizeof *DEFAULT_DATA;

// ============================================================================
//                               MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int                 test = argc > 1 ? atoi(argv[1]) : 0;
    bool             verbose = argc > 2;
    bool         veryVerbose = argc > 3;
    bool     veryVeryVerbose = argc > 4;
    bool veryVeryVeryVerbose = argc > 5;

    bsl::cout << "TEST " << __FILE__ << " CASE " << test << bsl::endl;;

    // CONCERN: `BSLS_REVIEW` failures should lead to test failures.
    bsls::ReviewFailureHandlerGuard reviewGuard(&bsls::Review::failByAbort);

    bslma::TestAllocator globalAllocator("global", veryVeryVeryVerbose);
    bslma::Default::setGlobalAllocator(&globalAllocator);

    bslma::TestAllocator defaultAllocator("default", veryVeryVeryVerbose);
    ASSERT(0 == bslma::Default::setDefaultAllocator(&defaultAllocator));

    switch (test) { case 0:  // Zero is always the leading case.
      case 11: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //   Extracted from component header file.
        //
        // Concerns:
        // 1. The usage example provided in the component header file compiles,
        //    links, and runs as shown.
        //
        // Plan:
        // 1. Incorporate usage example from header into test driver, remove
        //    leading comment characters, and replace `assert` with `ASSERT`.
        //    (C-1)
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << "\nUSAGE EXAMPLE"
                          << "\n=============" << endl;

        baljsn::ConvertToJsonOptions options;
        ASSERT(true == options.convertEmptyArrays());
        ASSERT(true == options.convertNullElements());

        options.setConvertEmptyArrays(false);
        ASSERT(false == options.convertEmptyArrays());

        options.setConvertNullElements(false);
        ASSERT(false == options.convertNullElements());

      } break;
      case 10: {
        // --------------------------------------------------------------------
        // TESTING BDEX STREAMING
        //
        // Concerns:
        //   N/A
        //
        // Plan:
        //   N/A
        //
        // Testing:
        //  Reserved for `swap` testing.
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING BDEX STREAMING"
                          << "\n======================" << endl;
      } break;
      case 9: {
        // --------------------------------------------------------------------
        // TESTING COPY-ASSIGNMENT OPERATOR
        //   Ensure that we can assign the value of any object of the class to
        //   any object of the class, such that the two objects subsequently
        //   have the same value.
        //
        // Concerns:
        // 1. The assignment operator can change the value of any modifiable
        //    target object to that of any source object.
        //
        // 2. The signature and return type are standard.
        //
        // 3. The reference returned is to the target object (i.e., `*this`).
        //
        // 4. The value of the source object is not modified.
        //
        // 5. Assigning an object to itself behaves as expected (alias-safety).
        //
        // Testing:
        //   operator=(const baljsn::ConvertToJsonOptions& rhs);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING COPY-ASSIGNMENT OPERATOR"
                          << "\n================================" << endl;

        if (verbose) cout <<
                 "\nAssign the address of the operator to a variable." << endl;
        {
            typedef Obj& (Obj::*operatorPtr)(const Obj&);

            operatorPtr operatorAssignment = &Obj::operator=;

            (void)operatorAssignment;  // quash potential compiler warning
        }

        const int NUM_DATA                     = DEFAULT_NUM_DATA;
        const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int   LINE1 = DATA[ti].d_line;
            const bool  EEA1  = DATA[ti].d_convertEmptyArrays;
            const bool  ENE1  = DATA[ti].d_convertNullElements;

            Obj mZ;  const Obj& Z = mZ;
            mZ.setConvertEmptyArrays(EEA1);
            mZ.setConvertNullElements(ENE1);

            Obj mZZ;  const Obj& ZZ = mZZ;
            mZZ.setConvertEmptyArrays(EEA1);
            mZZ.setConvertNullElements(ENE1);

            if (veryVerbose) { T_ P_(LINE1) P_(Z) P(ZZ) }

            static bool firstFlag = true;
            if (firstFlag) {
                LOOP3_ASSERT(LINE1, Obj(), Z, Obj() == Z);
                firstFlag = false;
            }

            for (int tj = 0; tj < NUM_DATA; ++tj) {
                const int   LINE2 = DATA[tj].d_line;
                const bool  EEA2  = DATA[tj].d_convertEmptyArrays;
                const bool  ENE2  = DATA[tj].d_convertNullElements;

                Obj mX;  const Obj& X = mX;
                mX.setConvertEmptyArrays(EEA2);
                mX.setConvertNullElements(ENE2);

                if (veryVerbose) { T_ P_(LINE2) P(X) }

                LOOP4_ASSERT(LINE1, LINE2, Z, X, (Z == X) == (LINE1 == LINE2));

                Obj *mR = &(mX = Z);
                LOOP4_ASSERT(LINE1, LINE2,  Z,   X,  Z == X);
                LOOP4_ASSERT(LINE1, LINE2, mR, &mX, mR == &mX);

                LOOP4_ASSERT(LINE1, LINE2, ZZ, Z, ZZ == Z);
            }

            if (verbose) cout << "Testing self-assignment" << endl;

            {
                Obj mX;
                mX.setConvertEmptyArrays(EEA1);
                mX.setConvertNullElements(ENE1);

                Obj mZZ;  const Obj& ZZ = mZZ;
                mZZ.setConvertEmptyArrays(EEA1);
                mZZ.setConvertNullElements(ENE1);

                const Obj& Z = mX;

                LOOP3_ASSERT(LINE1, ZZ, Z, ZZ == Z);

                Obj *mR = &(mX = Z);
                LOOP3_ASSERT(LINE1, ZZ,   Z, ZZ == Z);
                LOOP3_ASSERT(LINE1, mR, &mX, mR == &mX);
            }
        }
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // TESTING SWAP MEMBER AND FREE FUNCTIONS
        //
        // Concerns:
        //   N/A
        //
        // Plan:
        //   N/A
        //
        // Testing:
        //  Reserved for `swap` testing.
        // --------------------------------------------------------------------

        if (verbose)
                    cout << "\nTESTING SWAP MEMBER AND FREE FUNCTIONS"
                         << "\n======================================" << endl;
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TESTING COPY CONSTRUCTOR
        //   Ensure that we can create a distinct object of the class from any
        //   other one, such that the two objects have the same value.
        //
        // Concerns:
        // 1. The copy constructor creates an object having the same value as
        //    that of the supplied original object.
        //
        // 2. The original object is passed as a reference providing
        //    non-modifiable access to that object.
        //
        // 3. The value of the original object is unchanged.
        //
        // Testing:
        //   baljsn::ConvertToJsonOptions(const CTJO& original);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING COPY CONSTRUCTOR"
                          << "\n========================" << endl;

        const int NUM_DATA                     = DEFAULT_NUM_DATA;
        const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int   LINE = DATA[ti].d_line;
            const bool  EEA  = DATA[ti].d_convertEmptyArrays;
            const bool  ENE  = DATA[ti].d_convertNullElements;

            Obj mZ;  const Obj& Z = mZ;
            mZ.setConvertEmptyArrays(EEA);
            mZ.setConvertNullElements(ENE);

            Obj mZZ;  const Obj& ZZ = mZZ;
            mZZ.setConvertEmptyArrays(EEA);
            mZZ.setConvertNullElements(ENE);

            if (veryVerbose) { T_ P_(Z) P(ZZ) }

            Obj mX(Z);  const Obj& X = mX;

            if (veryVerbose) { T_ T_ P(X) }

            static bool firstFlag = true;
            if (firstFlag) {
                LOOP3_ASSERT(LINE, Obj(), Z, Obj() == Z)
                firstFlag = false;
            }

            LOOP3_ASSERT(LINE,  Z, X,  Z == X);

            LOOP3_ASSERT(LINE, ZZ, Z, ZZ == Z);
        }
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING EQUALITY-COMPARISON OPERATORS
        //   Ensure that `==` and `!=` are the operational definition of value.
        //
        // Concerns:
        // 1. Two objects, `X` and `Y`, compare equal if and only if each of
        //    their corresponding salient attributes respectively compares
        //    equal.
        //
        // 2. All salient attributes participate in the comparison.
        //
        // Testing:
        //   bool operator==(const baljsn::ConvertToJsonOptions& lhs, rhs);
        //   bool operator!=(const baljsn::ConvertToJsonOptions& lhs, rhs);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING EQUALITY-COMPARISON OPERATORS"
                          << "\n=====================================" << endl;

        typedef bool  T1;        // `convertEmptyArrays`
        typedef bool  T2;        // `convertNullElements`

        const T1 A1 = true;            // baseline
        const T1 B1 = false;

        const T2 A2 = true;            // baseline
        const T2 B2 = false;

        static const struct {
            int   d_line;
            bool  d_convertEmptyArrays;
            bool  d_convertNullElements;
        } DATA[] = {
            //LINE   EEA   ENE
            //----   ---   ---
            { L_,    A1,   A2, }
          , { L_,    B1,   A2, }
          , { L_,    A1,   B2, }
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int   LINE1 = DATA[ti].d_line;
            const bool  EEA1  = DATA[ti].d_convertEmptyArrays;
            const bool  ENE1  = DATA[ti].d_convertNullElements;

            if (veryVerbose) { T_ P_(LINE1) P_(EEA1) P(ENE1) }

            {
                Obj mX;  const Obj& X = mX;

                mX.setConvertEmptyArrays(EEA1);
                mX.setConvertNullElements(ENE1);

                LOOP2_ASSERT(LINE1, X,   X == X);
                LOOP2_ASSERT(LINE1, X, !(X != X));
            }

            for (int tj = 0; tj < NUM_DATA; ++tj) {
                const int   LINE2 = DATA[tj].d_line;
                const bool  EEA2  = DATA[tj].d_convertEmptyArrays;
                const bool  ENE2  = DATA[tj].d_convertNullElements;

                if (veryVerbose) { T_ P_(LINE2) P_(EEA2) P(ENE2) }

                const bool EXP = ti == tj;

                Obj mX;  const Obj& X = mX;
                Obj mY;  const Obj& Y = mY;

                mX.setConvertEmptyArrays(EEA1);
                mX.setConvertNullElements(ENE1);

                mY.setConvertEmptyArrays(EEA2);
                mY.setConvertNullElements(ENE2);

                LOOP4_ASSERT(LINE1, LINE2, X, Y,  EXP == (X == Y));
                LOOP4_ASSERT(LINE1, LINE2, Y, X,  EXP == (Y == X));

                LOOP4_ASSERT(LINE1, LINE2, X, Y, !EXP == (X != Y));
                LOOP4_ASSERT(LINE1, LINE2, Y, X, !EXP == (Y != X));
            }
        }
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING PRINT AND OUTPUT OPERATOR
        //   Ensure that the value of the object can be formatted appropriately
        //   on an `ostream` in some standard, human-readable form.
        //
        // Concerns:
        // 1. The `print` method writes the value to the specified `ostream`.
        //
        // 2. The `print` method writes the value in the intended format.
        //
        // 3. The output using `s << obj` is the same as `obj.print(s, 0, -1)`.
        //
        // 4. The `print` method returns the supplied `ostream`.
        //
        // 5. The output `operator<<` returns the supplied `ostream`.
        //
        // Testing:
        //   ostream& print(ostream& s, int level = 0, int sPL = 4) const;
        //   operator<<(ostream& s, const baljsn::ConvertToJsonOptions& d);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING PRINT AND OUTPUT OPERATOR"
                          << "\n=================================" << endl;

        bslma::TestAllocator         da("default", veryVeryVeryVerbose);
        bslma::DefaultAllocatorGuard dag(&da);

        const bool T = true;
        const bool F = false;

        static const struct {
            int         d_line;
            int         d_level;
            int         d_spl;

            bool        d_convertEmptyArrays;
            bool        d_convertNullElements;

            const char *d_expected_p;
        } DATA[] = {

#define NL "\n"
#define SP " "

   // ------------------------------------------------------------------
   // P-2.1.1: { A } x { 0 } x { 0, 1, -1 } --> 3 expected outputs
   // ------------------------------------------------------------------

//LINE  L  SPL  EEA  ENE          EXP
//----  -  ---  ---  ---          ---

{ L_,  0,  0,   T,   T, "["                                                  NL
                        "convertEmptyArrays = true"                          NL
                        "convertNullElements = true"                         NL
                        "]"                                                  NL
                                                                             },

{ L_,  0,  1,   T,   T, "["                                                  NL
                        " convertEmptyArrays = true"                         NL
                        " convertNullElements = true"                        NL
                        "]"                                                  NL
                                                                             },

{ L_,  0, -1,   T,   F, "["                                                  SP
                        "convertEmptyArrays = true"                          SP
                        "convertNullElements = false"                        SP
                        "]"
                                                                             },

   // ------------------------------------------------------------------
   // P-2.1.2: { A } x { 3, -3 } x { 0, 2, -2 }  -->  6 expected outputs
   // ------------------------------------------------------------------

{ L_,  3,  0,   T,   T, "["                                                  NL
                         "convertEmptyArrays = true"                         NL
                         "convertNullElements = true"                        NL
                         "]"                                                 NL
                                                                             },

{ L_,  3,  2,   F,   F, "      ["                                            NL
                        "        convertEmptyArrays = false"                 NL
                        "        convertNullElements = false"                NL
                        "      ]"                                            NL
                                                                             },

{ L_,  3, -2,   T,   F, "      ["                                            SP
                        "convertEmptyArrays = true"                          SP
                        "convertNullElements = false"                        SP
                        "]"
                                                                             },

{ L_, -3,  0,   F,   T, "["                                                  NL
                        "convertEmptyArrays = false"                         NL
                        "convertNullElements = true"                         NL
                        "]"                                                  NL
                                                                             },

{ L_, -3,  2,   T,   F, "["                                                  NL
                        "        convertEmptyArrays = true"                  NL
                        "        convertNullElements = false"                NL
                        "      ]"                                            NL
                                                                             },

{ L_, -3, -2,   T,   T, "["                                                  SP
                        "convertEmptyArrays = true"                          SP
                        "convertNullElements = true"                         SP
                        "]"
                                                                             },

   // -----------------------------------------------------------------
   // P-2.1.3: { B } x { 2 }     x { 3 }         -->  1 expected output
   // -----------------------------------------------------------------

{ L_,  2,  3,   T,   T, "      ["                                            NL
                        "         convertEmptyArrays = true"                 NL
                        "         convertNullElements = true"                NL
                        "      ]"                                            NL
                                                                             },

        // -----------------------------------------------------------------
        // P-2.1.4: { A B } x { -9 }   x { -9 }  -->  2 expected outputs
        // -----------------------------------------------------------------

{ L_, -9, -9,   F,   T, "["                                                  SP
                         "convertEmptyArrays = false"                        SP
                         "convertNullElements = true"                        SP
                         "]"
                                                                             },

{ L_, -9, -9,   F,   F, "["                                                  SP
                         "convertEmptyArrays = false"                        SP
                         "convertNullElements = false"                       SP
                         "]"
                                                                             },

#undef NL
#undef SP

        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE = DATA[ti].d_line;
            const int         L    = DATA[ti].d_level;
            const int         S    = DATA[ti].d_spl;

            const bool        EEA  = DATA[ti].d_convertEmptyArrays;
            const bool        ENE  = DATA[ti].d_convertNullElements;

            const char *const EXP  = DATA[ti].d_expected_p;

            if (veryVerbose) { T_ P_(L) P_(S) P_(EEA) P_(ENE) }

            if (veryVeryVerbose) { T_ T_ Q(EXPECTED) cout << EXP; }

            Obj mX;  const Obj& X = mX;
            mX.setConvertEmptyArrays(EEA);
            mX.setConvertNullElements(ENE);

            ostringstream os;

            if (-9 == L && -9 == S) {

                LOOP_ASSERT(LINE, &os == &(os << X));

                if (veryVeryVerbose) { T_ T_ Q(operator<<) }
            }
            else {

                LOOP_ASSERT(LINE, &os == &X.print(os, L, S));

                if (veryVeryVerbose) { T_ T_ Q(print) }
            }

            if (veryVeryVerbose) { P(os.str()) }

            LOOP3_ASSERT(LINE, EXP, os.str(), EXP == os.str());
        }
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING ACCESSORS
        //
        // Concerns:
        // 1. Each accessor returns the value of the corresponding attribute
        //    of the object.
        //
        // 2. Each accessor method is declared `const`.
        //
        // Testing:
        //   bool convertEmptyArrays() const;
        //   bool convertNullElements() const;
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING ACCESSORS"
                          << "\n=================" << endl;

        typedef bool  T1;        // `convertEmptyArrays`
        typedef bool  T2;        // `convertNullElements`

        const T1  D1  = true;                // `convertEmptyArrays`
        const T2  D2  = true;                // `convertNullElements`

        const bool  A1   = false;            // `convertEmptyArrays`
        const bool  A2   = false;            // `convertNullElements`

        Obj mX;  const Obj& X = mX;

        {
            const T1& convertEmptyArrays = X.convertEmptyArrays();
            LOOP2_ASSERT(D1, convertEmptyArrays, D1 == convertEmptyArrays);

            const T2& convertNullElements = X.convertNullElements();
            LOOP2_ASSERT(D2, convertNullElements, D2 == convertNullElements);
        }

        if (veryVerbose) { T_ Q(convertEmptyArrays) }
        {
            mX.setConvertEmptyArrays(A1);

            const T1& convertEmptyArrays = X.convertEmptyArrays();
            LOOP2_ASSERT(A1, convertEmptyArrays, A1 == convertEmptyArrays);
        }

        if (veryVerbose) { T_ Q(convertNullElements) }
        {
            mX.setConvertNullElements(A2);

            const T2& convertNullElements = X.convertNullElements();
            LOOP2_ASSERT(A2, convertNullElements, A2 == convertNullElements);
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING MANIPULATORS
        //
        // Concerns:
        // 1. Any argument can be `const`.
        //
        // 2. Each attribute is modifiable independently.
        //
        // 3. Each attribute can be set to represent any value that does not
        //    violate that attribute's documented constraints.
        //
        // Testing:
        //   setConvertEmptyArrays(bool value);
        //   setConvertNullElements(bool value);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING MANIPULATORS"
                          << "\n====================" << endl;

        const bool  D1   = true;            // `convertEmptyArrays`
        const bool  D2   = true;            // `convertNullElements`

        const bool  A1   = false;           // `convertEmptyArrays`
        const bool  A2   = false;           // `convertNullElements`

        const bool  B1   = true;            // `convertEmptyArrays`
        const bool  B2   = true;            // `convertNullElements`

        Obj mX;  const Obj& X = mX;

        // -------------------
        // `convertEmptyArrays`
        // -------------------
        {
            mX.setConvertEmptyArrays(A1);
            ASSERT(A1 == X.convertEmptyArrays());
            ASSERT(D2 == X.convertNullElements());

            mX.setConvertEmptyArrays(B1);
            ASSERT(B1 == X.convertEmptyArrays());
            ASSERT(D2 == X.convertNullElements());

            mX.setConvertEmptyArrays(D1);
            ASSERT(D1 == X.convertEmptyArrays());
            ASSERT(D2 == X.convertNullElements());
        }

        // --------------------
        // `convertNullElements`
        // --------------------
        {
            mX.setConvertNullElements(A2);
            ASSERT(D1 == X.convertEmptyArrays());
            ASSERT(A2 == X.convertNullElements());

            mX.setConvertNullElements(B2);
            ASSERT(D1 == X.convertEmptyArrays());
            ASSERT(B2 == X.convertNullElements());

            mX.setConvertNullElements(D2);
            ASSERT(D1 == X.convertEmptyArrays());
            ASSERT(D2 == X.convertNullElements());
        }

        if (verbose) cout << "Corroborate attribute independence." << endl;
        {
            mX.setConvertEmptyArrays(A1);

            ASSERT(A1 == X.convertEmptyArrays());
            ASSERT(D2 == X.convertNullElements());

            mX.setConvertNullElements(A2);

            ASSERT(A1 == X.convertEmptyArrays());
            ASSERT(A2 == X.convertNullElements());

            mX.setConvertEmptyArrays(B1);

            ASSERT(B1 == X.convertEmptyArrays());
            ASSERT(A2 == X.convertNullElements());

            mX.setConvertNullElements(B2);

            ASSERT(B1 == X.convertEmptyArrays());
            ASSERT(B2 == X.convertNullElements());
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING CREATORS
        //
        // Concerns:
        // 1. An object created with the default constructor has the
        //    contractually specified default value.
        //
        // Testing:
        //   baljsn::ConvertToJsonOptions();
        //   ~baljsn::ConvertToJsonOptions();
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING CREATORS"
                          << "\n================" << endl;

        const bool  D1   = true;            // `convertEmptyArrays`
        const bool  D2   = true;            // `convertNullElements`

        Obj mX;  const Obj& X = mX;

        LOOP2_ASSERT(D1,   X.convertEmptyArrays(),
                     D1 == X.convertEmptyArrays());
        LOOP2_ASSERT(D2,   X.convertNullElements(),
                     D2 == X.convertNullElements());
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //   This case exercises (but does not fully test) basic functionality.
        //
        // Concerns:
        // 1. The class is sufficiently functional to enable comprehensive
        //    testing in subsequent test cases.
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) cout << "\nBREATHING TEST"
                          << "\n==============" << endl;

        typedef bool  T1;        // `convertEmptyArrays`
        typedef bool  T2;        // `convertNullElements`

        const T1 D1 = true;    // default value
        const T1 A1 = false;

        const T2 D2 = true;    // default value
        const T2 A2 = false;

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\n 1. Create an object `w` (default ctor)."
                             "\t\t{ w:D             }" << endl;

        Obj mW;  const Obj& W = mW;

        ASSERT(D1  == W.convertEmptyArrays());
        ASSERT(D2  == W.convertNullElements());

        ASSERT(1 == (W == W));        ASSERT(0 == (W != W));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\n 2. Create an object `x` (copy from `w`)."
                             "\t\t{ w:D x:D         }" << endl;

        Obj mX(W);  const Obj& X = mX;

        ASSERT(D1  == X.convertEmptyArrays());
        ASSERT(D2  == X.convertNullElements());

        ASSERT(1 == (X == W));        ASSERT(0 == (X != W));
        ASSERT(1 == (X == X));        ASSERT(0 == (X != X));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\n 3. Set `x` to `A` (value distinct from `D`)."
                             "\t\t{ w:D x:A         }" << endl;

        mX.setConvertEmptyArrays(A1);
        mX.setConvertNullElements(A2);

        ASSERT(A1  == X.convertEmptyArrays());
        ASSERT(A2  == X.convertNullElements());

        ASSERT(0 == (X == W));        ASSERT(1 == (X != W));
        ASSERT(1 == (X == X));        ASSERT(0 == (X != X));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\n 4. Create an object `y` (init. to `A`)."
                             "\t\t{ w:D x:A y:A     }" << endl;

        Obj mY;  const Obj& Y = mY;
        mY.setConvertEmptyArrays(A1);
        mY.setConvertNullElements(A2);

        ASSERT(A1  == Y.convertEmptyArrays());
        ASSERT(A2  == Y.convertNullElements());

        ASSERT(0 == (Y == W));        ASSERT(1 == (Y != W));
        ASSERT(1 == (Y == X));        ASSERT(0 == (Y != X));
        ASSERT(1 == (Y == Y));        ASSERT(0 == (Y != Y));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\n 5. Create an object `z` (copy from `y`)."
                             "\t\t{ w:D x:A y:A z:A }" << endl;

        Obj mZ(Y);  const Obj& Z = mZ;

        ASSERT(A1  == Z.convertEmptyArrays());
        ASSERT(A2  == Z.convertNullElements());

        ASSERT(0 == (Z == W));        ASSERT(1 == (Z != W));
        ASSERT(1 == (Z == X));        ASSERT(0 == (Z != X));
        ASSERT(1 == (Z == Y));        ASSERT(0 == (Z != Y));
        ASSERT(1 == (Z == Z));        ASSERT(0 == (Z != Z));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\n 6. Set `z` to `D` (the default value)."
                             "\t\t\t{ w:D x:A y:A z:D }" << endl;

        mZ.setConvertEmptyArrays(D1);
        mZ.setConvertNullElements(D2);

        ASSERT(D1  == Z.convertEmptyArrays());
        ASSERT(D2  == Z.convertNullElements());

        ASSERT(1 == (Z == W));        ASSERT(0 == (Z != W));
        ASSERT(0 == (Z == X));        ASSERT(1 == (Z != X));
        ASSERT(0 == (Z == Y));        ASSERT(1 == (Z != Y));
        ASSERT(1 == (Z == Z));        ASSERT(0 == (Z != Z));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\n 7. Assign `w` from `x`."
                             "\t\t\t\t{ w:A x:A y:A z:D }" << endl;
        mW = X;

        ASSERT(A1  == W.convertEmptyArrays());
        ASSERT(A2  == W.convertNullElements());

        ASSERT(1 == (W == W));        ASSERT(0 == (W != W));
        ASSERT(1 == (W == X));        ASSERT(0 == (W != X));
        ASSERT(1 == (W == Y));        ASSERT(0 == (W != Y));
        ASSERT(0 == (W == Z));        ASSERT(1 == (W != Z));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\n 8. Assign `w` from `z`."
                             "\t\t\t\t{ w:D x:A y:A z:D }" << endl;
        mW = Z;

        ASSERT(D1  == W.convertEmptyArrays());
        ASSERT(D2  == W.convertNullElements());

        ASSERT(1 == (W == W));        ASSERT(0 == (W != W));
        ASSERT(0 == (W == X));        ASSERT(1 == (W != X));
        ASSERT(0 == (W == Y));        ASSERT(1 == (W != Y));
        ASSERT(1 == (W == Z));        ASSERT(0 == (W != Z));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\n 9. Assign `x` from `x` (aliasing)."
                             "\t\t\t{ w:D x:A y:A z:D }" << endl;
        mX = X;

        ASSERT(A1  == X.convertEmptyArrays());
        ASSERT(A2  == X.convertNullElements());

        ASSERT(0 == (X == W));        ASSERT(1 == (X != W));
        ASSERT(1 == (X == X));        ASSERT(0 == (X != X));
        ASSERT(1 == (X == Y));        ASSERT(0 == (X != Y));
        ASSERT(0 == (X == Z));        ASSERT(1 == (X != Z));
      } break;
      default: {
        bsl::cerr << "WARNING: CASE `" << test << "' NOT FOUND." << bsl::endl;
        testStatus = -1;
      }
    }

    LOOP_ASSERT(globalAllocator.numBlocksTotal(),
                0 == globalAllocator.numBlocksTotal());

    LOOP_ASSERT(defaultAllocator.numBlocksTotal(),
                0 == defaultAllocator.numBlocksTotal());

    if (testStatus > 0) {
        bsl::cerr << "Error, non-zero test status = " << testStatus << "."
                  << bsl::endl;
    }
    return testStatus;
}

// ----------------------------------------------------------------------------
// Copyright 2026 Bloomberg Finance L.P.
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
