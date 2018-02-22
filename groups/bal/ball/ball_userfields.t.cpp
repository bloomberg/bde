// ball_userfields.t.cpp                                              -*-C++-*-
#include <ball_userfields.h>

#include <bslim_testutil.h>

#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>

#include <bslmf_assert.h>

#include <bsls_assert.h>
#include <bsls_asserttest.h>
#include <bsls_platform.h>
#include <bsls_types.h>

#include <bsl_iostream.h>
#include <bsl_sstream.h>
#include <bsl_vector.h>

using namespace BloombergLP;
using namespace bsl;

// ============================================================================
//                                   TEST PLAN
// ----------------------------------------------------------------------------
//                                   Overview
//                                   --------
// TBD
//
// Primary Manipulators:
//: o TBD
//
// Basic Accessors:
//: o TBD
//
// Global Concerns:
//: o The test driver is robust w.r.t. reuse in other, similar components.
//: o ACCESSOR methods are declared 'const'.
//: o CREATOR/MANIPULATOR/OPERATOR ptr./ref. parameters are declared 'const'.
//: o No memory is ever allocated from the global allocator.
//: o No memory is ever allocated from the default allocator.
//: o Precondition violations are detected in appropriate build modes.
// ----------------------------------------------------------------------------
// CREATORS
// [  ] UserFields(Allocator *ba = 0);
// [  ] UserFields(const UserFields& original, Allocator *ba = 0);
// [  ] ~UserFields() = default;
//
// MANIPULATORS
// [  ] UserFields& operator=(const UserFields& rhs);
// [  ] void removeAll();
// [  ] void append(const UserFieldValue& value);
// [  ] void appendNull();
// [  ] void appendInt64(bsls::Types::Int64 value);
// [  ] void appendDouble(double value);
// [  ] void appendString(bslstl::StringRef value);
// [  ] void appendDatetimeTz(const bdlt::DatetimeTz& value);
// [  ] void appendCharArray(const bsl::vector<char>& value);
// [  ] UserFieldValue& operator[](int index);
// [  ] UserFieldValue& value(int index);
// [  ] void swap(UserFields& other);
//
// ACCESSORS
// [  ] ConstIterator begin() const;
// [  ] ConstIterator end() const;
// [  ] int length () const;
// [  ] const UserFieldValue& operator[](int index) const;
// [  ] const UserFieldValue& value(int index) const;
// [  ] bslma::Allocator *allocator() const;
// [ 5] ostream& print(ostream& s, int level = 0, int sPL = 4) const;
//
// FREE OPERATORS
// [  ] bool operator==(const UserFields& lhs, const UserFields& rhs);
// [  ] bool operator!=(const UserFields& lhs, const UserFields& rhs);
// [ 5] ostream& operator<<(ostream &os, const UserFields& object);
//
// FREE FUNCTIONS
// [  ] void swap(UserFields& a, UserFields& b);
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 6] USAGE EXAMPLE

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
//                  NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT_SAFE_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(EXPR)
#define ASSERT_SAFE_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(EXPR)
#define ASSERT_PASS(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS(EXPR)
#define ASSERT_FAIL(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL(EXPR)
#define ASSERT_OPT_PASS(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS(EXPR)
#define ASSERT_OPT_FAIL(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL(EXPR)

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

typedef ball::UserFields    Obj;
typedef ball::UserFieldType Type;

typedef bsls::Types::Int64  Int64;

// ============================================================================
//                                 TYPE TRAITS
// ----------------------------------------------------------------------------

BSLMF_ASSERT(true == bslma::UsesBslmaAllocator<Obj>::value);

//=============================================================================
//                      HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

//=============================================================================
//                               USAGE EXAMPLE
//-----------------------------------------------------------------------------

///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Basic Use of 'ball::UserFields'
/// - - - - - - - - - - - - - - - - - - - - -
// In the following example we demonstrate populating a 'ball::UserFields'
// object with a sequence of field values.
//
// First, we define the signature for a callback, 'populateUserFields'.  Most
// often 'ball::UserFields' objects are populated by a callback, such as the
// one described by the 'ball::LoggerManagerConfiguration'
// 'UserFieldsPopulatorCallback'.
//..
    void populateLoggingFields(ball::UserFields *fields)
        // Populate the specified 'fields' with the username and current task
        // identifier.  The behavior is undefined unless 'fields' is empty.
    {
//..
// Next, we assert the precondition that 'fields' is empty:
//..
      BSLS_ASSERT(0 == fields->length());
//..
// Now, we populate the 'fields' object with the username and current task
// identifier (for the purpose of illustration, these are simply constants):
//..
      static const char               *TEST_USER = "testUser";
      static const bsls::Types::Int64  TEST_TASK = 4315;
//
      fields->appendString(TEST_USER);
      fields->appendInt64(TEST_TASK);
//..
// Finally, for the purposes of illustration, we verify that 'fields' has been
// set correctly:
//..
      ASSERT(2              == fields->length());
      ASSERT(Type::e_STRING == fields->value(0).type());
      ASSERT(TEST_USER      == fields->value(0).theString());
      ASSERT(Type::e_INT64  == fields->value(1).type());
      ASSERT(TEST_TASK      == fields->value(1).theInt64());
    }
//..

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    const int                 test = argc > 1 ? atoi(argv[1]) : 0;
    const bool             verbose = argc > 2;
    const bool         veryVerbose = argc > 3;
    const bool     veryVeryVerbose = argc > 4;
    const bool veryVeryVeryVerbose = argc > 5;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    // CONCERN: This test driver is reusable w/other, similar components.

    // CONCERN: In no case does memory come from the global allocator.

    bslma::TestAllocator globalAllocator("global", veryVeryVeryVerbose);
    bslma::Default::setGlobalAllocator(&globalAllocator);

    // CONCERN: In no case does memory come from the default allocator.

    bslma::TestAllocator defaultAllocator("default", veryVeryVeryVerbose);
    bslma::DefaultAllocatorGuard defaultAllocatorGuard(&defaultAllocator);

    switch (test) { case 0:
      case 6: {
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

        if (verbose) cout << endl
                          << "USAGE EXAMPLE" << endl
                          << "=============" << endl;

        // This test intentionally uses the default allocator.

        bslma::TestAllocator testAllocator("usage", veryVeryVeryVerbose);
        bslma::DefaultAllocatorGuard guard(&testAllocator);

        ball::UserFields result;

        populateLoggingFields(&result);

        ASSERT(2          == result.length());
        ASSERT("testUser" == result[0].theString());
        ASSERT(4315       == result[1].theInt64());

      } break;
      case 5: {
        // --------------------------------------------------------------------
        // PRINT AND OUTPUT OPERATOR (<<)
        //   Ensure that the value of the object can be formatted appropriately
        //   on an 'ostream' in some standard, human-readable form.
        //
        // Concerns:
        //: 1 The 'print' method writes the value to the specified 'ostream'.
        //:
        //: 2 The 'print' method writes the value in the intended format.
        //:
        //: 3 The output using 's << obj' is the same as 'obj.print(s, 0, -1)'.
        //:
        //: 4 The 'print' method's signature and return type are standard.
        //:
        //: 5 The 'print' method returns the supplied 'ostream'.
        //:
        //: 6 The optional 'level' and 'spacesPerLevel' parameters have the
        //:   correct default values (0 and 4, respectively).
        //:
        //: 7 The output 'operator<<'s signature and return type are standard.
        //:
        //: 8 The output 'operator<<' returns the destination 'ostream'.
        //
        // Plan:
        //: 1 Use the addresses of the 'print' member function and 'operator<<'
        //:   free function defined in this component to initialize,
        //:   respectively, member-function and free-function pointers having
        //:   the appropriate signatures and return types.  (C-4, 7)
        //:
        //: 2 Using the table-driven technique:  (C-1..3, 5..6, 8)
        //:
        //:   1 Define fourteen carefully selected combinations of (two) object
        //:     values ('A' and 'B'), having distinct values, and various
        //:     values for the two formatting parameters, along with the
        //:     expected output.
        //:
        //:     ( 'value' x  'level'   x 'spacesPerLevel' ):
        //:     1 { A   } x {  0     } x {  0, 1, -1, -8 } --> 3 expected o/ps
        //:     2 { A   } x {  3, -3 } x {  0, 2, -2, -8 } --> 6 expected o/ps
        //:     3 { B   } x {  2     } x {  3            } --> 1 expected o/p
        //:     4 { A B } x { -8     } x { -8            } --> 2 expected o/ps
        //:     5 { A B } x { -9     } x { -9            } --> 2 expected o/ps
        //:
        //:   2 For each row in the table defined in P-2.1:  (C-1..3, 5..6, 8)
        //:
        //:     1 Using a 'const' 'Obj', supply each object value and pair of
        //:       formatting parameters to 'print', omitting the 'level' or
        //:       'spacesPerLevel' parameter if the value of that argument is
        //:       '-8'.  If the parameters are, arbitrarily, '(-9, -9)', then
        //:       invoke the 'operator<<' instead.
        //:
        //:     2 Use a standard 'ostringstream' to capture the actual output.
        //:
        //:     3 Verify the address of what is returned is that of the
        //:       supplied stream.  (C-5, 8)
        //:
        //:     4 Compare the contents captured in P-2.2.2 with what is
        //:       expected.  (C-1..3, 6)
        //
        // Testing:
        //   ostream& print(ostream& s, int level = 0, int sPL = 4) const;
        //   ostream& operator<<(ostream &os, const UserFields& object);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "PRINT AND OUTPUT OPERATOR (<<)" << endl
                          << "==============================" << endl;

        if (verbose) cout << "\nAssign the addresses of 'print' and "
                             "the output 'operator<<' to variables." << endl;
        {
            typedef ostream& (Obj::*funcPtr)(ostream&, int, int) const;
            typedef ostream& (*operatorPtr)(ostream&, const Obj&);

            // Verify that the signatures and return types are standard.

            funcPtr     printMember = &Obj::print;
            operatorPtr operatorOut = ball::operator<<;

            (void)printMember;  // quash potential compiler warnings
            (void)operatorOut;
        }

        if (verbose) cout <<
             "\nCreate a table of distinct value/format combinations." << endl;

        static const struct {
            int         d_line;            // source line number
            int         d_level;
            int         d_spacesPerLevel;

            const char *d_string_p;
            Int64       d_int64;

            const char *d_expected_p;
        } DATA[] = {

#define NL "\n"

        // ------------------------------------------------------------------
        // P-2.1.1: { A } x { 0 }     x { 0, 1, -1, -8 } -->  4 expected o/ps
        // ------------------------------------------------------------------

        //LINE L SPL   STRING      INT64   EXP
        //---- - ---   ------      -----   ---

        { L_,  0,  0,  "segment",  247,    "["            NL
                                           "segment"      NL
                                           "247"          NL
                                           "]"            NL                 },

        { L_,  0,  1,  "segment",  247,    "["            NL
                                           " segment"     NL
                                           " 247"         NL
                                           "]"            NL                 },

        { L_,  0, -1,  "segment",  247,    "[ segment 247 ]"                 },

        { L_,  0, -8,  "segment",  247,    "["            NL
                                           "    segment"  NL
                                           "    247"      NL
                                           "]"            NL                 },

        // ------------------------------------------------------------------
        // P-2.1.2: { A } x { 3, -3 } x { 0, 2, -2, -8 } -->  6 expected o/ps
        // ------------------------------------------------------------------

        //LINE L SPL   STRING      INT64   EXP
        //---- - ---   ------      -----   ---

        { L_,  3,  0,  "segment",  247,    "["       NL
                                           "segment" NL
                                           "247"     NL
                                           "]"       NL                      },

        { L_,  3,  2,  "segment",  247,    "      ["         NL
                                           "        segment" NL
                                           "        247"     NL
                                           "      ]"         NL              },

        { L_,  3, -2,  "segment",  247,    "      [ segment 247 ]"           },

        { L_,  3, -8,  "segment",  247,    "            ["           NL
                                           "                segment" NL
                                           "                247"     NL
                                           "            ]"           NL      },

        { L_, -3,  0,  "segment",  247,    "["       NL
                                           "segment" NL
                                           "247"     NL
                                           "]"       NL                      },

        { L_, -3,  2,  "segment",  247,    "["               NL
                                           "        segment" NL
                                           "        247"     NL
                                           "      ]"         NL              },

        { L_, -3, -2,  "segment",  247,    "[ segment 247 ]"                 },

        { L_, -3, -8,  "segment",  247,    "["                       NL
                                           "                segment" NL
                                           "                247"     NL
                                           "            ]"           NL      },

        // -----------------------------------------------------------------
        // P-2.1.3: { B } x { 2 }     x { 3 }            -->  1 expected o/p
        // -----------------------------------------------------------------

        //LINE L SPL   STRING      INT64   EXP
        //---- - ---   ------      -----   ---

        { L_,  2,  3,  "quantity", 13023,  "      ["            NL
                                           "         quantity"  NL
                                           "         13023"     NL
                                           "      ]"            NL           },

        // -----------------------------------------------------------------
        // P-2.1.4: { A B } x { -8 }   x { -8 }         -->  2 expected o/ps
        // -----------------------------------------------------------------

        //LINE L SPL   STRING      INT64   EXP
        //---- - ---   ------      -----   ---

        { L_, -8, -8,  "segment",    247,  "["            NL
                                           "    segment"  NL
                                           "    247"      NL
                                           "]"            NL                 },

        { L_, -8, -8,  "quantity", 13023,  "["            NL
                                           "    quantity" NL
                                           "    13023"    NL
                                           "]"            NL                 },

        // -----------------------------------------------------------------
        // P-2.1.5: { A B } x { -9 }   x { -9 }         -->  2 expected o/ps
        // -----------------------------------------------------------------

        //LINE L SPL   STRING      INT64   EXP
        //---- - ---   ------      -----   ---

        { L_, -9, -9,  "segment",  247,    "[ segment 247 ]"                 },

        { L_, -9, -9,  "quantity", 13023,  "[ quantity 13023 ]"              },

#undef NL

        };
        const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

        if (verbose) cout << "\nTesting with various print specifications."
                          << endl;
        {
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int         LINE   = DATA[ti].d_line;
                const int         L      = DATA[ti].d_level;
                const int         SPL    = DATA[ti].d_spacesPerLevel;
                const char *const STRING = DATA[ti].d_string_p;
                const Int64       INT64  = DATA[ti].d_int64;
                const char *const EXP    = DATA[ti].d_expected_p;

                if (veryVerbose) {
                    T_ P_(LINE) P_(L) P(SPL)
                    T_ P_(STRING) P(INT64)
                }

                if (veryVeryVerbose) { T_ T_ Q(EXPECTED) cout << EXP; }

                bslma::TestAllocator oa("object", veryVeryVeryVerbose);

                Obj mX(&oa);  const Obj& X = mX;
                mX.appendString(STRING);
                mX.appendInt64(INT64);

                ostringstream os(&oa);

                // Verify supplied stream is returned by reference.

                if (-9 == L && -9 == SPL) {
                    ASSERTV(LINE, &os == &(os << X));

                    if (veryVeryVerbose) { T_ T_ Q(operator<<) }
                }
                else {
                    ASSERTV(LINE, -8 == SPL || -8 != L);

                    if (-8 != SPL) {
                        ASSERTV(LINE, &os == &X.print(os, L, SPL));
                    }
                    else if (-8 != L) {
                        ASSERTV(LINE, &os == &X.print(os, L));
                    }
                    else {
                        ASSERTV(LINE, &os == &X.print(os));
                    }

                    if (veryVeryVerbose) { T_ T_ Q(print) }
                }

                {
                    bslma::TestAllocator da("default", veryVeryVeryVerbose);
                    bslma::DefaultAllocatorGuard dag(&da);

                    // Verify output is formatted as expected.

                    if (veryVeryVerbose) { P(os.str()) }

                    ASSERTV(LINE, EXP, os.str(), EXP == os.str());
                }
            }
        }

      } break;
      case 4: {
        // --------------------------------------------------------------------
        // BASIC ACCESSORS
        //   Ensure each basic accessor properly interprets object state.
        //
        // Concerns:
        //: 1 TBD
        //
        // Plan:
        //: 1 TBD
        //
        // Testing:
        //   TBD
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BASIC ACCESSORS" << endl
                          << "===============" << endl;

        // TBD

      } break;
      case 3: {
        // --------------------------------------------------------------------
        // GENERATOR FUNCTION 'gg'
        //   There is no 'gg' function for this component.
        //
        // Testing:
        //   Reserved for 'gg' generator function.
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "GENERATOR FUNCTION 'gg'" << endl
                          << "=======================" << endl;

        if (verbose) cout << "No 'gg' function for 'ball::UserFields'."
                          << endl;

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // PRIMARY MANIPULATORS & DTOR
        //
        // Concerns:
        //: 1 TBD
        //:
        // Plan:
        //: 1 TBD
        //
        // Testing:
        //   TBD
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "PRIMARY MANIPULATORS & DTOR" << endl
                          << "===========================" << endl;

        // TBD

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //   This case exercises (but does not fully test) basic functionality.
        //
        // Concerns:
        //: 1 The class is sufficiently functional to enable comprehensive
        //:   testing in subsequent test cases.
        //
        // Plan:
        //: 1 Create an object 'w' (default ctor).      { w:0             }
        //: 2 Create an object 'x' (copy from 'w').     { w:0 x:0         }
        //: 3 Set 'x' to 'A' (value distinct from 0).   { w:0 x:A         }
        //: 4 Create an object 'y' (init. to 'A').      { w:0 x:A y:A     }
        //: 5 Create an object 'z' (copy from 'y').     { w:0 x:A y:A z:A }
        //: 6 Set 'z' to 0 (the default value).         { w:0 x:A y:A z:0 }
        //: 7 Assign 'w' from 'x'.                      { w:A x:A y:A z:0 }
        //: 8 Assign 'w' from 'z'.                      { w:0 x:A y:A z:0 }
        //: 9 Assign 'x' from 'x' (aliasing).           { w:0 x:A y:A z:0 }
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BREATHING TEST" << endl
                          << "==============" << endl;

        // This test intentionally uses the default allocator.

        bslma::TestAllocator testAllocator("breathing", veryVeryVeryVerbose);
        bslma::DefaultAllocatorGuard guard(&testAllocator);

        bsl::vector<char> mCA;
        mCA.push_back('a');  mCA.push_back('b');  mCA.push_back('c');

        const char               *A_STRING = "A";
        double                    A_DOUBLE = 2.0;
        const bsls::Types::Int64  A_INT    = 5;
        const bdlt::DatetimeTz    A_DATE(bdlt::Datetime(1999,1,1), 0);
        const bsl::vector<char>&  A_CHAR_ARRAY = mCA;

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\n 1. Create an object 'w' (default ctor)."
                             "\t\t{ w:0             }" << endl;

        Obj mW;  const Obj& W = mW;

        if (veryVerbose) cout << "\ta. Check initial value of 'w'." << endl;
        if (veryVeryVerbose) { T_ P(W) }

        ASSERT(0 == W.length());

        if (veryVerbose) cout <<
                          "\tb. Try equality operators: 'w' <op> 'w'." << endl;

        ASSERT(1 == (W == W));        ASSERT(0 == (W != W));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\n 2. Create an object 'x' (copy from 'w')."
                             "\t\t{ w:0 x:0         }" << endl;

        Obj mX(W);  const Obj& X = mX;

        if (veryVerbose) cout <<
                                "\ta. Check the initial value of 'x'." << endl;
        if (veryVeryVerbose) { T_ P(X) }

        ASSERT(0 == X.length());

        if (veryVerbose) cout <<
                     "\tb. Try equality operators: 'x' <op> 'w', 'x'." << endl;

        ASSERT(1 == (X == W));        ASSERT(0 == (X != W));
        ASSERT(1 == (X == X));        ASSERT(0 == (X != X));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\n 3. Set 'x' to 'A' (value distinct from 0)."
                             "\t\t{ w:0 x:A         }" << endl;

        mX.appendInt64(A_INT);
        mX.appendString(A_STRING);
        mX.appendDatetimeTz(A_DATE);
        mX.appendDouble(A_DOUBLE);
        mX.appendCharArray(A_CHAR_ARRAY);

        if (veryVerbose) cout << "\ta. Check new value of 'x'." << endl;
        if (veryVeryVerbose) { T_ P(X) }

        ASSERT(5            == X.length());
        ASSERT(A_INT        == X.value(0).theInt64());
        ASSERT(A_STRING     == X.value(1).theString());
        ASSERT(A_DATE       == X.value(2).theDatetimeTz());
        ASSERT(A_DOUBLE     == X.value(3).theDouble());
        ASSERT(A_CHAR_ARRAY == X.value(4).theCharArray());

        if (veryVerbose) cout <<
                     "\tb. Try equality operators: 'x' <op> 'w', 'x'." << endl;

        ASSERT(0 == (X == W));        ASSERT(1 == (X != W));
        ASSERT(1 == (X == X));        ASSERT(0 == (X != X));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\n 4. Create an object 'y' and set to 'A'."
                             "\t\t{ w:0 x:A y:A     }" << endl;

        Obj mY;  const Obj& Y = mY;

        mY.appendInt64(A_INT);
        mY.appendString(A_STRING);
        mY.appendDatetimeTz(A_DATE);
        mY.appendDouble(A_DOUBLE);
        mY.appendCharArray(A_CHAR_ARRAY);

        if (veryVerbose) cout << "\ta. Check initial value of 'y'." << endl;
        if (veryVeryVerbose) { T_ P(Y) }

        ASSERT(5            == Y.length());
        ASSERT(A_INT        == Y.value(0).theInt64());
        ASSERT(A_STRING     == Y.value(1).theString());
        ASSERT(A_DATE       == Y.value(2).theDatetimeTz());
        ASSERT(A_DOUBLE     == X.value(3).theDouble());
        ASSERT(A_CHAR_ARRAY == X.value(4).theCharArray());

        if (veryVerbose) cout <<
                "\tb. Try equality operators: 'y' <op> 'w', 'x', 'y'." << endl;

        ASSERT(0 == (Y == W));        ASSERT(1 == (Y != W));
        ASSERT(1 == (Y == X));        ASSERT(0 == (Y != X));
        ASSERT(1 == (Y == Y));        ASSERT(0 == (Y != Y));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\n 5. Create an object 'z' (copy from 'y')."
                             "\t\t{ w:0 x:A y:A z:A }" << endl;

        Obj mZ(Y);  const Obj& Z = mZ;

        if (veryVerbose) cout << "\ta. Check initial value of 'z'." << endl;
        if (veryVeryVerbose) { T_ P(Z) }

        ASSERT(5            == Z.length());
        ASSERT(A_INT        == Z.value(0).theInt64());
        ASSERT(A_STRING     == Z.value(1).theString());
        ASSERT(A_DATE       == Z.value(2).theDatetimeTz());
        ASSERT(A_DOUBLE     == Z.value(3).theDouble());
        ASSERT(A_CHAR_ARRAY == Z.value(4).theCharArray());

        if (veryVerbose) cout <<
           "\tb. Try equality operators: 'z' <op> 'w', 'x', 'y', 'z'." << endl;

        ASSERT(0 == (Z == W));        ASSERT(1 == (Z != W));
        ASSERT(1 == (Z == X));        ASSERT(0 == (Z != X));
        ASSERT(1 == (Z == Y));        ASSERT(0 == (Z != Y));
        ASSERT(1 == (Z == Z));        ASSERT(0 == (Z != Z));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\n 6. Set 'z' to 0 (the default value)."
                             "\t\t\t{ w:0 x:A y:A z:0 }" << endl;

        mZ.removeAll();

        if (veryVerbose) cout << "\ta. Check new value of 'z'." << endl;
        if (veryVeryVerbose) { T_ P(Z) }

        ASSERT(0 == Z.length());

        if (veryVerbose) cout <<
           "\tb. Try equality operators: 'z' <op> 'w', 'x', 'y', 'z'." << endl;

        ASSERT(1 == (Z == W));        ASSERT(0 == (Z != W));
        ASSERT(0 == (Z == X));        ASSERT(1 == (Z != X));
        ASSERT(0 == (Z == Y));        ASSERT(1 == (Z != Y));
        ASSERT(1 == (Z == Z));        ASSERT(0 == (Z != Z));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\n 7. Assign 'w' from 'x'."
                             "\t\t\t\t{ w:A x:A y:A z:0 }" << endl;

        mW = X;

        if (veryVerbose) cout << "\ta. Check new value of 'w'." << endl;
        if (veryVeryVerbose) { T_ P(W) }

        ASSERT(5            == W.length());
        ASSERT(A_INT        == W.value(0).theInt64());
        ASSERT(A_STRING     == W.value(1).theString());
        ASSERT(A_DATE       == W.value(2).theDatetimeTz());
        ASSERT(A_DOUBLE     == W.value(3).theDouble());
        ASSERT(A_CHAR_ARRAY == W.value(4).theCharArray());

        if (veryVerbose) cout <<
           "\tb. Try equality operators: 'w' <op> 'w', 'x', 'y', 'z'." << endl;

        ASSERT(1 == (W == W));        ASSERT(0 == (W != W));
        ASSERT(1 == (W == X));        ASSERT(0 == (W != X));
        ASSERT(1 == (W == Y));        ASSERT(0 == (W != Y));
        ASSERT(0 == (W == Z));        ASSERT(1 == (W != Z));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\n 8. Assign 'w' from 'z'."
                             "\t\t\t\t{ w:0 x:A y:A z:0 }" << endl;

        mW = Z;

        if (veryVerbose) cout << "\ta. Check new value of 'w'." << endl;
        if (veryVeryVerbose) { T_ P(W) }

        ASSERT(0 == W.length());

        if (veryVerbose) cout <<
           "\tb. Try equality operators: 'w' <op> 'w', 'x', 'y', 'z'." << endl;

        ASSERT(1 == (W == W));        ASSERT(0 == (W != W));
        ASSERT(0 == (W == X));        ASSERT(1 == (W != X));
        ASSERT(0 == (W == Y));        ASSERT(1 == (W != Y));
        ASSERT(1 == (W == Z));        ASSERT(0 == (W != Z));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\n 9. Assign 'x' from 'x' (aliasing)."
                             "\t\t\t{ w:0 x:A y:A z:0 }" << endl;

        mX = X;

        if (veryVerbose) cout << "\ta. Check (same) value of 'x'." << endl;
        if (veryVeryVerbose) { T_ P(X) }

        ASSERT(5            == X.length());
        ASSERT(A_INT        == X.value(0).theInt64());
        ASSERT(A_STRING     == X.value(1).theString());
        ASSERT(A_DATE       == X.value(2).theDatetimeTz());
        ASSERT(A_DOUBLE     == X.value(3).theDouble());
        ASSERT(A_CHAR_ARRAY == X.value(4).theCharArray());

        if (veryVerbose) cout <<
           "\tb. Try equality operators: 'x' <op> 'w', 'x', 'y', 'z'." << endl;

        ASSERT(0 == (X == W));        ASSERT(1 == (X != W));
        ASSERT(1 == (X == X));        ASSERT(0 == (X != X));
        ASSERT(1 == (X == Y));        ASSERT(0 == (X != Y));
        ASSERT(0 == (X == Z));        ASSERT(1 == (X != Z));

      } break;
      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      }
    }

    // CONCERN: In no case does memory come from the global allocator.

    LOOP_ASSERT(globalAllocator.numBlocksTotal(),
                0 == globalAllocator.numBlocksTotal());

    // CONCERN: In no case does memory come from the default allocator.

    LOOP_ASSERT(defaultAllocator.numBlocksTotal(),
                0 == defaultAllocator.numBlocksTotal());

    if (testStatus > 0) {
        cerr << "Error, non-zero test status = " << testStatus << "." << endl;
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
