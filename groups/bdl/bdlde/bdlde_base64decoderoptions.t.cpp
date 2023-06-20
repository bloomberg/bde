// bdlde_base64decoder.t.cpp                                          -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------


#include <bdlde_base64decoderoptions.h>

#include <bdlde_base64encoderoptions.h>

#include <bslim_testutil.h>

#include <bslmf_isbitwisecopyable.h>
#include <bslmf_isbitwisemoveable.h>

#include <bsl_limits.h>
#include <bsl_iostream.h>
#include <bsl_sstream.h>

using namespace BloombergLP;
using bsl::cout;
using bsl::cerr;
using bsl::endl;
using bsl::ends;
using bsl::flush;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// [ 4] ostream& print(ostream&, int = 0, int = 4) const;
// [ 4] operator<<(ostream&, const bdlde::DecoderOptions&);
// [ 3] Obj(int, Base64Alphabet::Enum);
// [ 3] Obj(int);
// [ 3] Obj(const Obj&);
// [ 3] Obj& operator=(const Obj&);
// [ 3] bool operator==(const Obj&, const Obj&);
// [ 3] bool operator!=(const Obj&, const Obj&);
// [ 3] void setAlphabet(Base64Alphabet::Enum) const;
// [ 3] void setIsPadded(bool) const;
// [ 3] void setIgnoreMode(Ignore::Enum);
// [ 3] Obj custom(int, Alpha::Enum, bool, bool);
// [ 3] Obj mime(bool);
// [ 3] Obj urlSafe(IgnoreMode, bool);
// [ 3] Obj standard(bool, bool);
// [ 2] Obj();
// [ 2] Obj(int, Base64Alphabet::Enum, bool);
// [ 2] Base64Alphabet::Enum alphabet() const;
// [ 2] bool isPadded() const;
// [ 2] Base64IgnoreMode::Enum ignoreMode() const;
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST -- (developer's sandbox)
// [ 5] USAGE EXAMPLE
//-----------------------------------------------------------------------------

// ============================================================================
//                     STANDARD BSL ASSERT TEST FUNCTION
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
//                         GLOBAL TYPEDEFS/CONSTANTS
// ----------------------------------------------------------------------------

typedef bdlde::Base64DecoderOptions Obj;
typedef bdlde::Base64EncoderOptions EncoderOptions;
typedef bdlde::Base64Alphabet       Alpha;
typedef bdlde::Base64IgnoreMode     Ignore;

enum { k_DEFAULT_MLL = 76 };

// ============================================================================
//                           TEST HELPER FUNCTIONS
// ----------------------------------------------------------------------------

// ============================================================================
//                               MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;             (void) verbose;
    int veryVerbose = argc > 3;         (void) veryVerbose;
    int veryVeryVerbose = argc > 4;     (void) veryVeryVerbose;
    int veryVeryVeryVerbose = argc > 5; (void) veryVeryVeryVerbose;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:  // Zero is always the leading case.
      case 5: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //
        // Concern:
        //: 1 Demonstrate configuring the type.
        //
        // Plan:
        //: 1 Configure an object and verify its state.
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

//
///Usage
///-----
// In this section we show intended use of this component.
//
///Example 1:
/// - - - - -
// Suppose we want a 'Base64DecoderOptions' object configured for MIME
// encoding, meaning 'alphabet == e_BASIC', 'isPadded == true', and
// 'ignoreMode = e_IGNORE_WHITESPACE'.
//
// First, we call the 'mime' class method, and we're done.
//..
    const bdlde::Base64DecoderOptions& mimeOptions =
                                           bdlde::Base64DecoderOptions::mime();
//..
// Then, we check the attributes:
//..
    ASSERT(mimeOptions.ignoreMode() ==
                                 bdlde::Base64IgnoreMode::e_IGNORE_WHITESPACE);
    ASSERT(mimeOptions.alphabet()   == bdlde::Base64Alphabet::e_BASIC);
    ASSERT(mimeOptions.isPadded()   == true);
//..
// Now, we stream the object:
//..
if (verbose) {
    mimeOptions.print(cout);
}
//..
// Finally, we observe the output:
//..
//  [
//      ignoreMode = IGNORE_WHITESPACE
//      alphabet = BASIC
//      isPadded = true
//  ]
//..
//
///Example 2:
/// - - - - -
// Suppose we want a 'Base64DecoderOptions' object configured for translating
// URL's.  That would mean 'alphabet == e_URL', 'isPadded == false', and
// ignoring neither unrecognized characters nor whitespace.
//
// First, the class method 'urlSafe' returns an object configured exactly that
// way, so we simply call it:
//..
    const bdlde::Base64DecoderOptions& urlOptions =
                                        bdlde::Base64DecoderOptions::urlSafe();
//..
// Then, we check the attributes:
//..
    ASSERT(urlOptions.ignoreMode() == bdlde::Base64IgnoreMode::e_IGNORE_NONE);
    ASSERT(urlOptions.alphabet()   == bdlde::Base64Alphabet::e_URL);
    ASSERT(urlOptions.isPadded()   == false);
//..
// Now, we stream the object:
//..
if (verbose) {
    urlOptions.print(cout);
}
//..
// Finally, we observe the output:
//..
//  [
//      ignoreMode = IGNORE_NONE
//      alphabet = URL
//      isPadded = false
//  ]
//..
//
///Example 3:
/// - - - - -
// Suppose we want an options object configured for standard Base64:
//
// First, we can simply call the 'standard' class method:
//..
    const bdlde::Base64DecoderOptions& standardOptions =
                                       bdlde::Base64DecoderOptions::standard();
//..
// Then, we check the attributes:
//..
    ASSERT(standardOptions.ignoreMode() ==
                                       bdlde::Base64IgnoreMode::e_IGNORE_NONE);
    ASSERT(standardOptions.alphabet()            ==
                                               bdlde::Base64Alphabet::e_BASIC);
    ASSERT(standardOptions.isPadded()            == true);
//..
// Now, we stream the object:
//..
if (verbose) {
    standardOptions.print(cout);
}
//..
// Finally, we observe the output:
//..
//  [
//      ignoreMode = IGNORE_NONE
//      alphabet = BASIC
//      isPadded = true
//  ]
//..
//
///Example 4:
/// - - - - -
// Suppose we want a really strangely configured options object with
// 'alphabet == e_URL', and padding, and ignoring neither unrecognized
// characters nor whitespace.
//
// First, we can simply call the 'custom' class method.  The 'padded' and
// 'unrecognizedIsError == true' arguments are last, and they default to
// 'true', so we don't have to pass that.
//..
    const bdlde::Base64DecoderOptions& customOptions =
                           bdlde::Base64DecoderOptions::custom(
                                        bdlde::Base64IgnoreMode::e_IGNORE_NONE,
                                        bdlde::Base64Alphabet::e_URL,
                                        true);
//..
// Then, we check the attributes:
//..
    ASSERT(customOptions.ignoreMode() ==
                                       bdlde::Base64IgnoreMode::e_IGNORE_NONE);
    ASSERT(customOptions.alphabet()   == bdlde::Base64Alphabet::e_URL);
    ASSERT(customOptions.isPadded()   == true);
//..
// Now, we stream the object:
//..
if (verbose) {
    cout << customOptions << endl;
}
//..
// Finally, we observe the output:
//..
//  [ ignoreMode = IGNORE_NONE alphabet = URL isPadded = true ]
//..
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING 'operator<<' AND 'print'
        //
        //: 1 The 'print' method writes the value to the specified 'ostream'.
        //:
        //: 2 The 'print' method writes the value in the intended format.
        //:
        //: 3 The output using 's << obj' is the same as 'obj.print(s, 0, -1)',
        //:   but with each "attributeName = " elided.
        //:
        //: 4 The 'print' method signature and return type are standard.
        //:
        //: 5 The 'print' method returns the supplied 'ostream'.
        //:
        //: 6 The output 'operator<<' signature and return type are standard.
        //:
        //: 7 The output 'operator<<' returns the supplied 'ostream'.
        //
        // Plan:
        //: 1 Use the addresses of the 'print' member function and 'operator<<'
        //:   free function defined in this component to initialize,
        //:   respectively, member-function and free-function pointers having
        //:   the appropriate signatures and return types.  (C-4, 6)
        //:
        //: 2 Using the table-driven technique, define twelve carefully
        //:   selected combinations of (two) object values ('A' and 'B'),
        //:   having distinct values for each corresponding salient attribute,
        //:   and various values for the two formatting parameters, along with
        //:   the expected output
        //:     ( 'value' x  'level'   x 'spacesPerLevel' ):
        //:     1 { A   } x {  0     } x {  0, 1, -1 }  -->  3 expected outputs
        //:     2 { A   } x {  3, -3 } x {  0, 2, -2 }  -->  6 expected outputs
        //:     3 { B   } x {  2     } x {  3        }  -->  1 expected output
        //:     4 { A B } x { -9     } x { -9        }  -->  2 expected output
        //:
        //: 3 For each row in the table defined in P-2.1:  (C-1..3, 5, 7)
        //:
        //:   1 Using a 'const' 'Obj', supply each object value and pair of
        //:     formatting parameters to 'print', unless the parameters are,
        //:     arbitrarily, (-9, -9), in which case 'operator<<' will be
        //:     invoked instead.
        //:
        //:   2 Use a standard 'ostringstream' to capture the actual output.
        //:
        //:   3 Verify the address of what is returned is that of the
        //:     supplied stream.  (C-5, 7)
        //:
        //:   4 Compare the contents captured in P-2.2.2 with what is
        //:     expected.  (C-1..3)
        //
        // Testing:
        //   ostream& print(ostream&, int = 0, int = 4) const;
        //   operator<<(ostream&, const bdlde::DecoderOptions&);
        // --------------------------------------------------------------------

        if (verbose) cout << "TESTING 'operator<<' AND 'print'\n"
                             "================================\n";

        {
            typedef bsl::ostream ostream;

            typedef ostream& (Obj::*funcPtr)(ostream&, int, int) const;
            typedef ostream& (*operatorPtr)(ostream&, const Obj&);

            // Verify that the signatures and return types are standard.

            funcPtr     printMember = &Obj::print;
            operatorPtr operatorOp  = &bdlde::operator<<;

            (void)printMember;  // quash potential compiler warnings
            (void)operatorOp;
        }

        if (verbose) cout <<
             "\nCreate a table of distinct value/format combinations." << endl;

        const Alpha::Enum B = Alpha::e_BASIC;
        const Alpha::Enum U = Alpha::e_URL;

        const bool T = true;
        const bool F = false;

        const Ignore::Enum N  = Ignore::e_IGNORE_NONE;
        const Ignore::Enum W  = Ignore::e_IGNORE_WHITESPACE;
        const Ignore::Enum UN = Ignore::e_IGNORE_UNRECOGNIZED;

        static const struct Data {
            int           d_line;           // source line number
            int           d_level;
            int           d_spl;

            Alpha::Enum   d_alphabet;
            bool          d_isPadded;
            Ignore::Enum  d_ignoreMode;

            const char   *d_expected_p;
        } DATA[] = {

#define NL "\n"
#define SP " "

//LINE L  SPL  A  P  IM
//---- -  ---  -  -  --

{ L_,  0,   0, B, T, W,      "["                                             NL
                                 "ignoreMode = IGNORE_WHITESPACE"            NL
                                 "alphabet = BASIC"                          NL
                                 "isPadded = true"                           NL
                                        "]"                                  NL
                                                                             },

{ L_,  0,  1,  U, F, N,      "["                                             NL
                                 " ignoreMode = IGNORE_NONE"                 NL
                                 " alphabet = URL"                           NL
                                 " isPadded = false"                         NL
                                       "]"                                   NL
                                                                             },

{ L_,  0, -1,  U, T, UN,     "["                                             SP
                                 "ignoreMode = IGNORE_UNRECOGNIZED"          SP
                                 "alphabet = URL"                            SP
                                 "isPadded = true"                           SP
                                       "]"
                                                                             },

   // ------------------------------------------------------------------
   // P-2.1.2: { A } x { 3, -3 } x { 0, 2, -2 }  -->  6 expected outputs
   // ------------------------------------------------------------------

//LINE L  SPL  A  P
//---- -  ---  -  -

{ L_,  3,   0, B, T, N,      "["                                             NL
                                 "ignoreMode = IGNORE_NONE"                  NL
                                 "alphabet = BASIC"                          NL
                                 "isPadded = true"                           NL
                                       "]"                                   NL
                                                                             },

{ L_,  3,   2, U, F, N,
                               "      ["                                     NL
                         "        ignoreMode = IGNORE_NONE"                  NL
                         "        alphabet = URL"                            NL
                         "        isPadded = false"                          NL
                               "      ]"                                     NL
                                                                             },

{ L_,  3,  -2, B, T, W,      "      ["                                       SP
                                 "ignoreMode = IGNORE_WHITESPACE"            SP
                                 "alphabet = BASIC"                          SP
                                 "isPadded = true"                           SP
                                       "]"
                                                                             },

{ L_, -3,  0,  U, T, UN,     "["                                             NL
                                 "ignoreMode = IGNORE_UNRECOGNIZED"          NL
                                 "alphabet = URL"                            NL
                                 "isPadded = true"                           NL
                                       "]"                                   NL
                                                                             },

{ L_, -3,  2,  B, T, W,      "["                                             NL
                         "        ignoreMode = IGNORE_WHITESPACE"            NL
                         "        alphabet = BASIC"                          NL
                         "        isPadded = true"                           NL
                               "      ]"                                     NL
                                                                             },

{ L_, -3, -2,  U, F, N,      "["                                             SP
                                 "ignoreMode = IGNORE_NONE"                  SP
                                 "alphabet = URL"                            SP
                                 "isPadded = false"                          SP
                                       "]"
                                                                             },

//LINE L  SPL  A  P
//---- -  ---  -  -

{ L_,  2,   3, B, F, W,      "      ["                                       NL
                             "         ignoreMode = IGNORE_WHITESPACE"       NL
                             "         alphabet = BASIC"                     NL
                             "         isPadded = false"                     NL
                             "      ]"                                       NL
                                                                             },

//LINE L  SPL  A  P
//---- -  ---  -  -

{ L_, -9,  -9, B, F, N,          "["                                         SP
                                 "ignoreMode = IGNORE_NONE"                  SP
                                 "alphabet = BASIC"                          SP
                                 "isPadded = false"                          SP
                                 "]" },

{ L_, -9,  -9, U, T, UN,         "["                                         SP
                                 "ignoreMode = IGNORE_UNRECOGNIZED"          SP
                                 "alphabet = URL"                            SP
                                 "isPadded = true"                           SP
                                 "]" },

#undef NL
#undef SP

        };
        enum { k_NUM_DATA = sizeof DATA / sizeof *DATA };

        if (verbose) cout << "\nTesting with various print specifications."
                          << endl;
        {
            for (int ti = 0; ti < k_NUM_DATA; ++ti) {
                const Data&        data = DATA[ti];

                const int          LINE = data.d_line;
                const int          L    = data.d_level;
                const int          S    = data.d_spl;

                const Alpha::Enum  A    = data.d_alphabet;
                const bool         PAD  = data.d_isPadded;
                const Ignore::Enum IGN  = data.d_ignoreMode;
                const bsl::string  EXP  = data.d_expected_p;

                if (veryVerbose) { T_ P_(L) P_(S) P_(A) P_(PAD) P(IGN) }

                if (veryVeryVerbose) { T_ T_ Q(EXPECTED) cout << EXP; }

                const Obj& X = Obj::custom(IGN, A, PAD);

                bsl::ostringstream os;

                if (-9 == L && -9 == S) {
                    if (veryVeryVerbose) { T_ T_ Q(operator<<) }

                    // Verify supplied stream is returned by reference.

                    LOOP_ASSERT(LINE, &os == &(os << X));
                }
                else {
                    if (veryVeryVerbose) { T_ T_ Q(print) }

                    // Verify supplied stream is returned by reference.

                    LOOP_ASSERT(LINE, &os == &X.print(os, L, S));
                }
                const bsl::string& RESULT = os.str();

                // Verify output is formatted as expected.

                if (veryVeryVerbose) { P(RESULT); }

                ASSERTV(LINE, EXP, RESULT, EXP.length(),
                                               RESULT.length(), EXP == RESULT);
            }
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // EXHAUSTIVE STATES, ALL C'TORS, '==', '!=', COPY CONSTRUCT/ASSIGN
        //
        // Concerns:
        //: 1 Test the settors, accessors, '==', and '!=' across a wide variety
        //:   of possible states of the object.
        //
        // Plan:
        //: 1 Iterate 3 nested loops to drive a wide variety of values for
        //:   'maxLineLength', and all possible values for 'alphabet' and
        //:   'isPadded'.  For all 3 attributes, have the loops also allow
        //:   separate iterations for defaulting values.
        //:
        //: 2 Default-construct an object, 'main'.
        //:
        //: 3 Call the 3 manipulators to set the state of 'main', except
        //:   sometimes don't call certain ones to leave attributes in their
        //:   default state.
        //:
        //: 4 Verify the state of the object through the accessors.
        //:
        //: 5 Value-construct an object, letting none of the arguments default,
        //:   and observe that the value is the same as 'MAIN' by checking the
        //:   attributes and calling '==' and '!='.
        //:
        //: 6 Value-construct an object, sometimes partially allowing arguments
        //:   to default, and observe that the value is the same as 'MAIN' by
        //:   checking the attributes and calling '==' and '!='.
        //:
        //: 7 Create an object using the 'urlSafe' class method, verify
        //:   that its attributes are as expected and that comparisons between
        //:   it and 'MAIN' using '==' and '!=' yield expected results.
        //:
        //: 8 Nest another 3 loops to go through all of the same set of states,
        //:   other than the defaulting states, as the outer 3 loops do,
        //:   calculate boolean 'EQ' to represent whether the 3 attributes
        //:   driven by the inner loops match those of the outer loops, and
        //:   then value-construct an inner object 'mY', setting the 3
        //:   attribues of it according to the inner loop values.
        //:   o Confirm that '==' and '!=' between 'main' and 'mY' yield the
        //:     expected results.
        //:
        //:   o Copy-assign 'main' to 'mY' and observe with '==' and '!='
        //:     that they match.
        //:
        //: 9 After the inner 3 loops are done:
        //:   o copy-construct another object from 'MAIN' and observe that
        //:     it has the same value.
        //:
        //:   o copy-assign 'MAIN to a default-constructed object and observe
        //:     equivalence with '==' and '!='.
        //
        // Testing:
        //   Obj(int, Base64Alphabet::Enum);
        //   Obj(int);
        //   void setAlphabet(Alpha::Enum);
        //   void setIsPadded(bool);
        //   void setIgnoreMode(Ignore::Enum);
        //   Obj custom(int, Alpha::Enum, bool, bool);
        //   Obj mime(bool);
        //   Obj urlSafe(IgnoreMode, bool);
        //   Obj standard(bool, bool);
        //   Obj(const Obj&);
        //   Obj& operator=(const Obj&);
        //   bool operator==(const Obj&, const Obj&);
        //   bool operator!=(const Obj&, const Obj&);
        // --------------------------------------------------------------------

        if (verbose) cout <<
          "EXHAUSTIVE STATES, ALL C'TORS, '==', '!=', COPY CONSTRUCT/ASSIGN\n"
          "================================================================\n";

        enum { k_MAX_TI = 3 * 3 * 3 };

        bool uIsErrWasSet = false;
        for (int ti = 0; ti < k_MAX_TI; ++ti) {
            int               tii      = ti;
            const int         ai       = (tii % 3) - 1;    tii /= 3;
            const int         pi       = (tii % 3) - 1;    tii /= 3;
            const int         ui       =  tii % 3;         tii /= 3;
            ASSERT(0 == tii);

            const Alpha::Enum  alphabet = ai < 0 ? Alpha::e_BASIC
                                                : ai ? Alpha::e_URL
                                                     : Alpha::e_BASIC;
            const bool         PADDED   = pi < 0 || pi;
            const Ignore::Enum IGNORE   = static_cast<Ignore::Enum>(ui);

            uIsErrWasSet |= 2 == ui;

            Obj main = Obj::custom(IGNORE, Alpha::e_BASIC, true);
            const Obj& MAIN = main;
            if (0 <= ai) {
                main.setAlphabet(alphabet);
            }
            if (0 <= pi) {
                main.setIsPadded(PADDED);
            }

            ASSERT(MAIN.ignoreMode() == IGNORE);
            ASSERT(MAIN.alphabet()   == alphabet);
            ASSERT(MAIN.isPadded()   == PADDED);

            if (veryVerbose) cout << "Call 'custom' class method\n";
            {
                const Obj& X = Obj::custom(IGNORE,
                                           alphabet,
                                           PADDED);

                ASSERT(X.ignoreMode() == IGNORE);
                ASSERT(X.alphabet()   == alphabet);
                ASSERT(X.isPadded()   == PADDED);

                ASSERTV(alphabet, PADDED, X, X == MAIN );
                ASSERT(  MAIN == X );
                ASSERT(!(X != MAIN));
                ASSERT(!(MAIN != X));
            }

            if (veryVerbose) cout << "MIME\n";
            {
                const Obj& X = 0 <= ui
                             ? Obj::mime(IGNORE)
                             : Obj::mime();

                const bool EQ = Alpha::e_BASIC == alphabet &&
                                PADDED;

                ASSERT(X.ignoreMode() == IGNORE);
                ASSERT(X.alphabet()   == Alpha::e_BASIC);
                ASSERT(X.isPadded()   == true);

                ASSERT( EQ == (X == MAIN));
                ASSERT( EQ == (MAIN == X));
                ASSERT(!EQ == (X != MAIN));
                ASSERT(!EQ == (MAIN != X));
            }

            if (veryVerbose) cout << "Standard\n";
            {
                const Obj& X = 0 <= pi
                             ? Obj::standard(IGNORE, PADDED)
                             : 0 <= ui
                             ? Obj::standard(IGNORE)
                             : Obj::standard();

                const bool EQ = Alpha::e_BASIC == alphabet;

                ASSERT(X.ignoreMode() == IGNORE);
                ASSERT(X.alphabet()   == Alpha::e_BASIC);
                ASSERT(X.isPadded()   == PADDED);

                ASSERT( EQ == (X == MAIN));
                ASSERT( EQ == (MAIN == X));
                ASSERT(!EQ == (X != MAIN));
                ASSERT(!EQ == (MAIN != X));
            }

            if (veryVerbose) cout << "URL Safe\n";
            {
                const Obj& X = Obj::urlSafe(IGNORE);

                const bool EQ = Alpha::e_URL == alphabet &&
                                false        == PADDED;

                ASSERT(X.ignoreMode() == IGNORE);
                ASSERT(X.alphabet()   == Alpha::e_URL);
                ASSERT(X.isPadded()   == false);

                ASSERT( EQ == (X == MAIN));
                ASSERT( EQ == (MAIN == X));
                ASSERT(!EQ == (X != MAIN));
                ASSERT(!EQ == (MAIN != X));
            }

            if (veryVerbose) cout << "URL Safe, padded configured\n";
            {
                const Obj& X = Obj::urlSafe(IGNORE, PADDED);

                const bool EQ = Alpha::e_URL == alphabet;

                ASSERT(X.ignoreMode() == IGNORE);
                ASSERT(X.alphabet()   == Alpha::e_URL);
                ASSERT(X.isPadded()   == PADDED);

                ASSERT( EQ == (X == MAIN));
                ASSERT( EQ == (MAIN == X));
                ASSERT(!EQ == (X != MAIN));
                ASSERT(!EQ == (MAIN != X));
            }

            if (veryVeryVerbose) cout << "Compare to all other states\n";

            bool tjjDone = false;
            enum { k_NUM_TJ = 2 * 2 * 3 };
            for (int tj = 0; tj < k_NUM_TJ; ++tj) {
                int                tjj       = tj;
                const Alpha::Enum  alphabetB = (tjj & 1) ? Alpha::e_URL
                                                         : Alpha::e_BASIC;
                                                                      tjj /= 2;
                const bool         PADDED_B  = tjj & 1;    tjj /= 2;
                const Ignore::Enum IGNOREB   = static_cast<Ignore::Enum>(tjj);
                tjjDone |= 2 == tjj;
                tjj /= 3;
                ASSERT(0 == tjj);

                const bool EQ = alphabet == alphabetB
                             && PADDED   == PADDED_B
                             && IGNORE   == IGNOREB;

                const Obj& Y = Obj::custom(IGNOREB,
                                           alphabetB,
                                           PADDED_B);

                ASSERTV(IGNOREB,   Y.ignoreMode() == IGNOREB);
                ASSERTV(alphabetB, Y.alphabet()   == alphabetB);
                ASSERTV(PADDED_B,  Y.isPadded()   == PADDED_B);

                ASSERTV(MAIN, Y, EQ == (MAIN == Y));
                ASSERT( EQ == (Y == MAIN));
                ASSERT(!EQ == (MAIN != Y));
                ASSERT(!EQ == (Y != MAIN));

                if (veryVeryVeryVerbose) cout <<
                              "Inner copy assign to default constructed\n";
                Obj mZ = Obj::mime();    const Obj& Z = mZ;
                Obj *p = &(mZ = Y);
                ASSERT(&mZ == p);

                ASSERT(  Y == Z );
                ASSERT(  Z == Y );
                ASSERT(!(Y != Z));
                ASSERT(!(Z != Y));

                ASSERT( EQ == (MAIN == Z));
                ASSERT( EQ == (Z == MAIN));
                ASSERT(!EQ == (MAIN != Z));
                ASSERT(!EQ == (Z != MAIN));

                if (veryVeryVeryVerbose) cout <<
                                  "Inner copy assign to arbitrary state\n";

                p = &(mZ = MAIN);
                ASSERT(&Z == p);

                ASSERT(  Z == MAIN );
                ASSERT(  MAIN == Z );
                ASSERT(!(Z != MAIN));
                ASSERT(!(MAIN != Z));

                ASSERT( EQ == (Y == Z));
                ASSERT( EQ == (Z == Y));
                ASSERT(!EQ == (Y != Z));
                ASSERT(!EQ == (Z != Y));
            }
            ASSERT(tjjDone);

            if (veryVeryVerbose) cout << "Copy construct\n";
            {
                const Obj X(main);
                ASSERT(  X == MAIN );
                ASSERT(  MAIN == X );
                ASSERT(!(X != MAIN));
                ASSERT(!(MAIN != X));
            }

            if (veryVeryVerbose) cout << "Outer copy assign\n";
            {
                Obj mY(Obj::mime(Ignore::e_IGNORE_NONE));    const Obj& Y = mY;
                Obj *p = &(mY = main);
                ASSERT(&mY == p);
                ASSERT(  Y == MAIN );
                ASSERT(  MAIN == Y );
                ASSERT(!(Y != MAIN));
                ASSERT(!(MAIN != Y));
            }
        }
        ASSERT(uIsErrWasSet);
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING 'CUSTOM' CLASS METHOD and ACCESSORS
        //   Make sure we can bring the object to any attainable state.
        //
        // Concerns:
        //   That we can fully configure the object from the constructor.
        //
        // Plan:
        //   Create the object in several configurations and verify using
        //   all of the (as yet untested) direct accessors.  After this test
        //   case, we can declare the accessors returning configuration state
        //   to be thoroughly tested.
        //
        // Tactics:
        //   - Boundary Data Selection Method
        //   - Brute-Force Implementation Technique
        //
        // Testing:
        //   Obj::custom(int, Base64Alphabet::Enum, bool);
        //   int maxLineLength() const;
        //   Base64Alphabet::Enum alphabet() const;
        //   bool isPadded() const;
        //   bool unrecognizedIsError() const;
        // --------------------------------------------------------------------

        if (verbose) cout << "TESTING 'CUSTOM' CLASS METHOD and ACCESSORS\n"
                             "===========================================\n";

        if (verbose) cout << "\nTry a few different settings." << endl;

        if (verbose) cout << "Default object\n";
        {
            const Obj OBJ(Obj::mime(Ignore::e_IGNORE_WHITESPACE));
            ASSERT(Ignore::e_IGNORE_WHITESPACE == OBJ.ignoreMode());
            ASSERT(Alpha::e_BASIC              == OBJ.alphabet());
            ASSERT(true                        == OBJ.isPadded());
        }

        if (verbose) cout << "\"base64\" alphabet"
                          << endl;
        {
            const Obj& OBJ = Obj::custom(Ignore::e_IGNORE_NONE,
                                         Alpha::e_BASIC,
                                         true);
            ASSERT(Ignore::e_IGNORE_NONE == OBJ.ignoreMode());
            ASSERT(Alpha::e_BASIC        == OBJ.alphabet());
            ASSERT(true                  == OBJ.isPadded());
        }

        if (verbose) cout << "\"base64url\" alphabet" << endl;
        {
            const Obj& OBJ = Obj::custom(Ignore::e_IGNORE_UNRECOGNIZED,
                                         Alpha::e_URL,
                                         true);
            ASSERT(Ignore::e_IGNORE_UNRECOGNIZED == OBJ.ignoreMode());
            ASSERT(Alpha::e_URL                  == OBJ.alphabet());
            ASSERT(true                          == OBJ.isPadded());
        }

        if (verbose) cout << "\"base64url\" alphabet, padded = false" << endl;
        {
            const Obj& OBJ = Obj::custom(Ignore::e_IGNORE_NONE,
                                         Alpha::e_URL, false);
            ASSERT(Ignore::e_IGNORE_NONE == OBJ.ignoreMode());
            ASSERT(Alpha::e_URL          == OBJ.alphabet());
            ASSERT(false                 == OBJ.isPadded());
        }

        if (verbose) cout << "\"base64url\" alphabet, padded = false" << endl;
        {
            const Obj& OBJ = Obj::custom(Ignore::e_IGNORE_WHITESPACE,
                                         Alpha::e_URL,
                                         false);
            ASSERT(Ignore::e_IGNORE_WHITESPACE == OBJ.ignoreMode());
            ASSERT(Alpha::e_URL                == OBJ.alphabet());
            ASSERT(false                       == OBJ.isPadded());
        }

        if (verbose) cout << "\"base64url\" alphabet, padded = false,"
                             " ignoreMode = None" << endl;
        {
            const Obj& OBJ = Obj::custom(Ignore::e_IGNORE_NONE,
                                         Alpha::e_URL,
                                         false);
            ASSERT(Ignore::e_IGNORE_NONE == OBJ.ignoreMode());
            ASSERT(Alpha::e_URL          == OBJ.alphabet());
            ASSERT(false                 == OBJ.isPadded());
        }

        if (verbose) cout << "\"base64url\" alphabet, padded = false,"
                             " ignoreMode = Unrecognized" << endl;
        {
            const Obj& OBJ = Obj::custom(Ignore::e_IGNORE_UNRECOGNIZED,
                                         Alpha::e_URL,
                                         false);
            ASSERT(Ignore::e_IGNORE_UNRECOGNIZED == OBJ.ignoreMode());
            ASSERT(Alpha::e_URL                  == OBJ.alphabet());
            ASSERT(false                         == OBJ.isPadded());
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //   This case is available to be used as a developers' sandbox.
        //
        // Concerns:
        //   None.
        //
        // Testing:
        //   BREATHING TEST -- (developer's sandbox)
        // --------------------------------------------------------------------

        if (verbose) cout << "BREATHING TEST\n"
                             "==============\n";

        ASSERT(bslmf::IsTriviallyCopyableCheck<
                                          bdlde::Base64DecoderOptions>::value);
        ASSERT(bslmf::IsBitwiseMoveable<bdlde::Base64DecoderOptions>::value);
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
// Copyright 2022 Bloomberg Finance L.P.
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
