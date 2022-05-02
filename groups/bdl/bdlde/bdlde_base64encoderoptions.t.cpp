// bdlde_base64encoder.t.cpp                                          -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------


#include <bdlde_base64encoderoptions.h>

#include <bslim_testutil.h>

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
// [ 5] ostream& print(ostream&, int = 0, int = 4) const;
// [ 5] operator<<(ostream&, const bdlde::EncoderOptions&);
// [ 4] Obj(const Obj&);
// [ 4] Obj& operator=(const Obj&);
// [ 4] bool operator==(const Obj&, const Obj&);
// [ 4] bool operator!=(const Obj&, const Obj&);
// [ 3] Obj& setMaxLineLength(int) const;
// [ 3] Obj& setAlphabet(Base64Alphabet::Enum) const;
// [ 3] Obj& setIsPadded(bool) const;
// [ 3] int maxLineLength() const;
// [ 3] Base64Alphabet::Enum alphabet() const;
// [ 3] bool isPadded() const;
// [ 2] Obj();
// [ 2] int maxLineLength() const;
// [ 2] Base64Alphabet::Enum alphabet() const;
// [ 2] bool isPadded() const;
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST -- (developer's sandbox)
// [ 6] USAGE EXAMPLE
//-----------------------------------------------------------------------------

// ============================================================================
//                     STANDARD BSL ASSERT TEST FUNCTION
// ----------------------------------------------------------------------------

namespace {

int testStatus = 0;

void aSsErT(bool condition, const char *message, int line)
{
    if (condition) {
        printf("Error " __FILE__ "(%d): %s    (failed)\n", line, message);

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

typedef bdlde::Base64EncoderOptions Obj;
typedef bdlde::Base64Alphabet       Alpha;

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
    int verbose = argc > 2;             (verbose);
    int veryVerbose = argc > 3;         (veryVerbose);
    int veryVeryVerbose = argc > 4;     (veryVeryVerbose);
    int veryVeryVeryVerbose = argc > 5; (veryVeryVeryVerbose);

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:  // Zero is always the leading case.
      case 6: {
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

///Usage
///-----
// In this section we show intended use of this component.
//
///Example 1:
/// - - - - -
// The following snippets of code provide a simple illustration of
// 'bdlde::Base64EncoderOptions' usage.
//
// This 'class' does not have a value constructor, only a default constructor.
// The settors all return a reference to the object, so in order
// to create an object with attributes other than the default ones, call
// the default constructor and the settors in a single statement, and bind the
// result to a reference (or pass the result directly to the 'Base64Encoder'
// being constructed).
//
// First, we declare some typedefs for brevity:
//..
    typedef bdlde::Base64Alphabet       Alphabet;
    typedef bdlde::Base64EncoderOptions EncoderOptions;
//..
// Then, we default construct and object, immediately call the desired settors
// on the temporary, and bind the result to a reference:
//..
    EncoderOptions& options = EncoderOptions().setAlphabet(Alphabet::e_URL).
                                                            setIsPadded(false);
//..
// Next, we observe that the properies are as expected:
//..
    ASSERT(options.maxLineLength() == 76);
    ASSERT(options.alphabet()      == Alphabet::e_URL);
    ASSERT(options.isPadded()      == false);
//..
// Now, we stream the object:
//..
if (verbose) {
    options.print(cout);
}
//..
// Finally, we observe the output:
//..
//  [
//      maxLineLength = 76
//      alphabet = URL
//      isPadded = false
//  ]
//..
      } break;
      case 5: {
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
        //   operator<<(ostream&, const bdlde::EncoderOptions&);
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

        static const struct Data {
            int         d_line;           // source line number
            int         d_level;
            int         d_spl;

            int         d_maxLineLength;
            Alpha::Enum d_alphabet;
            bool        d_isPadded;

            const char *d_expected_p;
        } DATA[] = {

#define NL "\n"
#define SP " "

//LINE L  SPL  MLL  A  P
//---- -  ---  ---  -  -

{ L_,  0,   0,   0, B, T, "["                                                NL
                                 "maxLineLength = 0"                         NL
                                 "alphabet = BASIC"                          NL
                                 "isPadded = true"                           NL
                                        "]"                                  NL
                                                                             },

{ L_,  0,  1,  20,  U, F, "["                                                NL

                                 " maxLineLength = 20"                       NL
                                 " alphabet = URL"                           NL
                                 " isPadded = false"                         NL
                                       "]"                                   NL
                                                                             },

{ L_,  0, -1,  20, U, T, "["                                                 SP
                                 "maxLineLength = 20"                        SP
                                 "alphabet = URL"                            SP
                                 "isPadded = true"                           SP
                                       "]"
                                                                             },

   // ------------------------------------------------------------------
   // P-2.1.2: { A } x { 3, -3 } x { 0, 2, -2 }  -->  6 expected outputs
   // ------------------------------------------------------------------

//LINE L  SPL  MLL  A  P
//---- -  ---  ---  -  -

{ L_,  3,   0,  89, B, T, "["                                                NL
                                 "maxLineLength = 89"                        NL
                                 "alphabet = BASIC"                          NL
                                 "isPadded = true"                           NL
                                       "]"                                   NL
                                                                             },

{ L_,  3,   2,  89, U, F,
                               "      ["                                     NL
                         "        maxLineLength = 89"                        NL
                         "        alphabet = URL"                            NL
                         "        isPadded = false"                          NL
                               "      ]"                                     NL
                                                                             },

{ L_,  3,  -2,  89, B, T,  "      ["                                         SP
                                 "maxLineLength = 89"                        SP
                                 "alphabet = BASIC"                          SP
                                 "isPadded = true"                           SP
                                       "]"
                                                                             },

{ L_, -3,  0,  89,  U, T,  "["                                               NL
                                 "maxLineLength = 89"                        NL
                                 "alphabet = URL"                            NL
                                 "isPadded = true"                           NL
                                       "]"                                   NL
                                                                             },

{ L_, -3,  2,  89,  B, T,  "["                                               NL
                         "        maxLineLength = 89"                        NL
                         "        alphabet = BASIC"                          NL
                         "        isPadded = true"                           NL
                               "      ]"                                     NL
                                                                             },

{ L_, -3, -2,  89,  U, F,  "["                                               SP
                                 "maxLineLength = 89"                        SP
                                 "alphabet = URL"                            SP
                                 "isPadded = false"                          SP
                                       "]"
                                                                             },

//LINE L  SPL  MLL  A  P
//---- -  ---  ---  -  -

{ L_,  2,   3,  89, B, F,"      ["                                           NL
                         "         maxLineLength = 89"                       NL
                         "         alphabet = BASIC"                         NL
                         "         isPadded = false"                         NL
                               "      ]"                                     NL
                                                                             },

//LINE L  SPL  MLL  A  P
//---- -  ---  ---  -  -

{ L_, -9,  -9,  89, B, F,        "["                                         SP
                                 "maxLineLength = 89"                        SP
                                 "alphabet = BASIC"                          SP
                                 "isPadded = false"                          SP
                                 "]" },

{ L_, -9,  -9,   7, U, T,        "["                                         SP
                                 "maxLineLength = 7"                         SP
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
                const Data&       data = DATA[ti];

                const int         LINE = data.d_line;
                const int         L    = data.d_level;
                const int         S    = data.d_spl;

                const int         MLL  = data.d_maxLineLength;
                const Alpha::Enum A    = data.d_alphabet;
                const bool        PAD  = data.d_isPadded;
                const bsl::string EXP  = data.d_expected_p;

                if (veryVerbose) { T_ P_(L) P_(S) P_(MLL) P_(A) P(PAD) }

                if (veryVeryVerbose) { T_ T_ Q(EXPECTED) cout << EXP; }

                Obj mX, *p;  const Obj& X = mX;

                p = &mX.setMaxLineLength(MLL);
                ASSERT(&mX == p);
                p = &mX.setAlphabet(A);
                ASSERT(&mX == p);
                p = &mX.setIsPadded(PAD);
                ASSERT(&mX == p);

                bsl::ostringstream os;

                if (-9 == L && -9 == S) {
                    // Verify supplied stream is returned by reference.

                    LOOP_ASSERT(LINE, &os == &(os << X));

                    if (veryVeryVerbose) { T_ T_ Q(operator<<) }
                }
                else {
                    // Verify supplied stream is returned by reference.

                    LOOP_ASSERT(LINE, &os == &X.print(os, L, S));

                    if (veryVeryVerbose) { T_ T_ Q(print) }
                }

                // Verify output is formatted as expected.

                if (veryVeryVerbose) { P(os.str()) }

                ASSERTV(LINE, EXP, os.str(), EXP.length(),
                                           os.str().length(), EXP == os.str());
            }
        }
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // EXHAUSTIVE STATES, '==', '!=', COPY CONSTRUCT/ASSIGN
        //
        // Concerns:
        //: 1 Test the settors, accessors, '==', and '!=' across a wide variety
        //:   of possible states of the object.
        //
        // Plan:
        //: 1 Iterate 3 nested loops to drive a wide variety of values for
        //:   'maxLineLength', and all possible values for 'alphabet' and
        //:   'isPadded'.  For all 3 attributes, have the loops also allow
        //:   defaulting values.
        //:
        //: 2 Default-construct an object, 'master'.
        //:
        //: 3 Call the 3 manipulators to set the state of 'master', except
        //:   sometimes don't call certain ones to leave attributes in their
        //:   default state.
        //:
        //: 4 Verify the state of the object through the accessors.
        //:
        //: 5 In several cases, default-construct objects and call all 3
        //:   manipulators to set the objects to the same state as master.
        //:   Confirm with '==' and '!='.  Repeat, calling the manipulators in
        //:   different orders.
        //:
        //: 6 If any of the 3 attributes have default values, default construct
        //:   objects and set the other 2 attributes with their manipulators
        //:   and confirm the state matches 'master' with '==' and '!='.
        //:
        //: 7 Nest another 3 loops to go through all of the same set of states
        //:   as the outer 3 loops do, calculate boolean 'EQ' to represent
        //:   whether the 3 attributes driven by the inner loops match those of
        //:   the outer loops, and then default-construct an inner object 'mY',
        //:   setting the 3 attribues of it according to the inner loops.
        //:   o Confirm that '==' and '!=' between 'master' and 'mY' yield the
        //:     expected results.
        //:
        //:   o Copy-assign 'master' to 'mY' and observe with '==' and '!='
        //:     that they match.
        //:
        //:  After the inner 3 loops are done, copy-construct and copy-assign
        //:   master to other objects and observe equivalence with '==' and
        //:   '!='.
        //
        // Testing:
        //   Obj(const Obj&);
        //   Obj& operator=(const Obj&);
        //   bool operator==(const Obj&, const Obj&);
        //   bool operator!=(const Obj&, const Obj&);
        // --------------------------------------------------------------------

        const int MLLS[] = { 0, 1, 2, 7, 9, 20, 76, 200, 1024 };
        enum { k_NUM_MLLS = sizeof MLLS / sizeof *MLLS };

        for (int mi = -1; mi < k_NUM_MLLS; ++mi) {
            const int MLL = mi < 0 ? k_DEFAULT_MLL : MLLS[mi];

            for (int ai = -1; ai < 2; ++ai) {
                const Alpha::Enum alphabet = ai < 0 ? Alpha::e_BASIC
                                                    : ai ? Alpha::e_URL
                                                         : Alpha::e_BASIC;

                for (int pi = -1; pi < 2; ++pi) {
                    const bool PADDED = pi < 0 || pi;

                    Obj master, *p = &master;    const Obj& MASTER = master;
                    if (0 <= MLL) {
                        p = &master.setMaxLineLength(MLL);
                    }
                    ASSERT(&master == p);
                    if (0 <= ai) {
                        p = &master.setAlphabet(alphabet);
                    }
                    ASSERT(&master == p);
                    if (0 <= pi) {
                        p = &master.setIsPadded(PADDED);
                    }
                    ASSERT(&master == p);

                    ASSERT(MASTER.maxLineLength() == MLL);
                    ASSERT(MASTER.alphabet()      == alphabet);
                    ASSERT(MASTER.isPadded()      == PADDED);

                    if (veryVerbose) cout << "Set all 3 in different orders\n";
                    {
                        Obj& mX = Obj().setAlphabet(alphabet).
                                     setIsPadded(PADDED).setMaxLineLength(MLL);
                        const Obj& X = mX;

                        ASSERT(  X == MASTER );
                        ASSERT(  MASTER == X );
                        ASSERT(!(X != MASTER));
                        ASSERT(!(MASTER != X));

                        Obj& mY = Obj().setIsPadded(PADDED).
                                   setMaxLineLength(MLL).setAlphabet(alphabet);
                        const Obj& Y = mY;

                        ASSERT(  Y == MASTER );
                        ASSERT(  MASTER == Y );
                        ASSERT(!(Y != MASTER));
                        ASSERT(!(MASTER != Y));
                    }

                    if (veryVerbose) cout << "Default MLL, set other two\n";
                    if (k_DEFAULT_MLL == MLL) {
                        Obj& mX = Obj().setIsPadded(PADDED).
                                                         setAlphabet(alphabet);
                        const Obj& X = mX;

                        ASSERT(  X == MASTER );
                        ASSERT(  MASTER == X );
                        ASSERT(!(X != MASTER));
                        ASSERT(!(MASTER != X));

                        Obj& mY = Obj().setAlphabet(alphabet).
                                                           setIsPadded(PADDED);
                        const Obj& Y = mY;

                        ASSERT(  Y == MASTER );
                        ASSERT(  MASTER == Y );
                        ASSERT(!(Y != MASTER));
                        ASSERT(!(MASTER != Y));
                    }

                    if (veryVerbose) cout <<
                                           "Default alphabet, set other two\n";
                    if (Alpha::e_BASIC == alphabet) {
                        Obj& mX = Obj().setIsPadded(PADDED).
                                                         setMaxLineLength(MLL);
                        const Obj& X = mX;

                        ASSERT(  X == MASTER );
                        ASSERT(  MASTER == X );
                        ASSERT(!(X != MASTER));
                        ASSERT(!(MASTER != X));

                        Obj& mY = Obj().setMaxLineLength(MLL).
                                                           setIsPadded(PADDED);
                        const Obj& Y = mY;

                        ASSERT(  Y == MASTER );
                        ASSERT(  MASTER == Y );
                        ASSERT(!(Y != MASTER));
                        ASSERT(!(MASTER != Y));
                    }

                    if (veryVerbose) cout <<
                                           "Default isPadded, set other two\n";
                    if (PADDED) {
                        Obj& mX = Obj().setAlphabet(alphabet).
                                                         setMaxLineLength(MLL);
                        const Obj& X = mX;

                        ASSERT(  X == MASTER );
                        ASSERT(  MASTER == X );
                        ASSERT(!(X != MASTER));
                        ASSERT(!(MASTER != X));

                        Obj& mY = Obj().setMaxLineLength(MLL).
                                                         setAlphabet(alphabet);
                        const Obj& Y = mY;

                        ASSERT(  Y == MASTER );
                        ASSERT(  MASTER == Y );
                        ASSERT(!(Y != MASTER));
                        ASSERT(!(MASTER != Y));
                    }

                    if (veryVeryVerbose) cout <<
                                               "Compare to all other states\n";
                    for (int miB = 0; miB < k_NUM_MLLS; ++miB) {
                        const int MLLB = MLLS[miB];

                        for (int aiB = 0; aiB < 2; ++aiB) {
                            const Alpha::Enum alphabetB = aiB ? Alpha::e_URL
                                                              : Alpha::e_BASIC;

                            for (int piB = 0; piB < 2; ++piB) {
                                const bool PADDED_B = piB;

                                const bool EQ = MLL      == MLLB
                                             && alphabet == alphabetB
                                             && PADDED   == PADDED_B;

                                Obj& mY = Obj().setMaxLineLength(MLLB)
                                                    .setAlphabet(alphabetB)
                                                    .setIsPadded(PADDED_B);
                                const Obj& Y = mY;

                                ASSERT(Y.maxLineLength() == MLLB);
                                ASSERT(Y.alphabet()      == alphabetB);
                                ASSERT(Y.isPadded()      == PADDED_B);

                                ASSERT( EQ == (MASTER == Y));
                                ASSERT( EQ == (Y == MASTER));
                                ASSERT(!EQ == (MASTER != Y));
                                ASSERT(!EQ == (Y != MASTER));

                                if (veryVeryVeryVerbose) cout <<
                                                         "Inner copy assign\n";
                                p = &(mY = MASTER);
                                ASSERT(&mY == p);
                                ASSERT(  Y == MASTER );
                                ASSERT(  MASTER == Y );
                                ASSERT(!(Y != MASTER));
                                ASSERT(!(MASTER != Y));
                            }
                        }
                    }

                    if (veryVeryVerbose) cout << "Copy construct\n";
                    {
                        const Obj X(master);
                        ASSERT(  X == MASTER );
                        ASSERT(  MASTER == X );
                        ASSERT(!(X != MASTER));
                        ASSERT(!(MASTER != X));
                    }

                    if (veryVeryVerbose) cout << "Outer copy assign\n";
                    {
                        Obj mY;    const Obj& Y = mY;
                        p = &(mY = master);
                        ASSERT(&mY == p);
                        ASSERT(  Y == MASTER );
                        ASSERT(  MASTER == Y );
                        ASSERT(!(Y != MASTER));
                        ASSERT(!(MASTER != Y));
                    }
                }
            }
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // PRIMARY MANIPULATORS and ACCESSORS
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
        //   Obj& setMaxLineLength(int) const;
        //   Obj& setAlphabet(Base64Alphabet::Enum) const;
        //   Obj& setIsPadded(bool) const;
        //   int maxLineLength() const;
        //   Base64Alphabet::Enum alphabet() const;
        //   bool isPadded() const;
        // --------------------------------------------------------------------

        if (verbose) cout << "PRIMARY MANIPULATORS\n"
                             "====================\n";


        if (verbose) cout << "\nTry a few different settings." << endl;

        if (verbose) cout << "\tmaxLineLength = 0, default alphabet" << endl;
        {
            Obj& obj = Obj().setMaxLineLength(0);    const Obj& OBJ = obj;
            ASSERT(0              == OBJ.maxLineLength());
            ASSERT(Alpha::e_BASIC == OBJ.alphabet());
            ASSERT(true           == OBJ.isPadded());
        }

        if (verbose) cout << "\tmaxLineLength = 1, default alphabet" << endl;
        {
            Obj& obj = Obj().setMaxLineLength(1);    const Obj& OBJ = obj;
            ASSERT(1              == OBJ.maxLineLength());
            ASSERT(Alpha::e_BASIC == OBJ.alphabet());
            ASSERT(true           == OBJ.isPadded());
        }

        if (verbose) cout << "\tmaxLineLength = 2, \"base64\" alphabet"
                          << endl;
        {
            Obj& obj = Obj().setMaxLineLength(2).setAlphabet(Alpha::e_BASIC);
            const Obj& OBJ = obj;
            ASSERT(2              == OBJ.maxLineLength());
            ASSERT(Alpha::e_BASIC == OBJ.alphabet());
            ASSERT(true           == OBJ.isPadded());
        }

        if (verbose) cout << "\tmaxLineLength = INT_MAX, \"base64url\" "
                          << "alphabet" << endl;
        {
            Obj& obj = Obj().setMaxLineLength(INT_MAX).setAlphabet(
                                                                 Alpha::e_URL);
            const Obj& OBJ = obj;
            ASSERT(INT_MAX      == OBJ.maxLineLength());
            ASSERT(Alpha::e_URL == OBJ.alphabet());
            ASSERT(true         == OBJ.isPadded());
        }

        if (verbose) cout << "\tmaxLineLength = INT_MAX, \"base64url\" "
                          << "alphabet, padded = false" << endl;
        {
            Obj& obj = Obj().setMaxLineLength(INT_MAX).setAlphabet(
                                              Alpha::e_URL).setIsPadded(false);
            const Obj& OBJ = obj;
            ASSERT(INT_MAX      == OBJ.maxLineLength());
            ASSERT(Alpha::e_URL == OBJ.alphabet());
            ASSERT(false        == OBJ.isPadded());
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // PRIMARY CONSTRUCTOR and ACCESSORS
        //   Make sure we can bring the object to any attainable state.
        //
        // Concerns:
        //: 1 That we can default-constructed state is as expected.
        //
        // Plan:
        //: 1 Default construct an object.
        //:
        //: 2 Verify its state is as expect with the accessors.
        //
        // Testing:
        //   Obj();
        //   int maxLineLength() const;
        //   Base64Alphabet::Enum alphabet() const;
        //   bool isPadded() const;
        // --------------------------------------------------------------------

        if (verbose) cout << "PRIMARY CONSTRUCTOR\n"
                             "===================\n";

        if (verbose) cout << "\nTry a few different settings." << endl;

        if (verbose) cout << "\tDefault object\n";
        {
            const Obj OBJ;
            ASSERT(76             == OBJ.maxLineLength());
            ASSERT(Alpha::e_BASIC == OBJ.alphabet());
            ASSERT(true           == OBJ.isPadded());
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
