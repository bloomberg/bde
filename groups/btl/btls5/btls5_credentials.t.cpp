// btls5_credentials.t.cpp                                            -*-C++-*-
#include <btls5_credentials.h>

#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>

#include <bsls_assert.h>
#include <bsls_asserttest.h>

#include <bsl_iostream.h>
#include <bsl_sstream.h>

using namespace BloombergLP;
using namespace bsl;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
// The component under test implements a value-semantic attribute class.  There
// are two attributes, and the primary manipulators modify both at once.  The
// accessors access each individually.  The test exercises each public
// interface, using the accessors to verify the state set by the manipulators.
//
//-----------------------------------------------------------------------------
// CREATORS
// [2] explicit btls5::Credentials(bslma::Allocator *basicAllocator = 0);
// [2] btls5::Credentials(const StringRef& u, const StringRef& p, *a = 0);
// [2] btls5::Credentials(const btls5::Credentials& original, *a = 0);
// [2] ~btls5::Credentials() = default;
//
// MANIPULATORS
// [2] set(const bslstl::StringRef& u, const bslstl::StringRef& p);
//
// ACCESSORS
// [2] string& username() const;
// [2] string& password() const;
// [5] ostream& print(ostream& s, int level = 0, int sPL = 4) const;
//
// FREE OPERATORS
// [2] operator==(btls5::Credentials& lhs, btls5::Credentials& rhs);
// [2] operator!=(btls5::Credentials& lhs, btls5::Credentials& rhs);
// [5] operator<<(ostream& output, const btls5::Credentials& object);
//-----------------------------------------------------------------------------
// [1] BREATHING TEST
// [6] USAGE EXAMPLE
// [2] CONCERN: All memory allocation is from the object's allocator.
// [2] CONCERN: Precondition violations are detected when enabled.

// ============================================================================
//                    STANDARD BDE ASSERT TEST MACROS
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
# define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

// ============================================================================
//                  STANDARD BDE LOOP-ASSERT TEST MACROS
// ----------------------------------------------------------------------------

#define LOOP_ASSERT(I,X) {                                                    \
    if (!(X)) { cout << #I << ": " << I << "\n"; aSsErT(1, #X, __LINE__);}}

#define LOOP2_ASSERT(I,J,X) {                                                 \
    if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": "                 \
              << J << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP3_ASSERT(I,J,K,X) {                                               \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t"     \
              << #K << ": " << K << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP4_ASSERT(I,J,K,L,X) {                                             \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" <<  \
       #K << ": " << K << "\t" << #L << ": " << L << "\n";                    \
       aSsErT(1, #X, __LINE__); } }

#define LOOP5_ASSERT(I,J,K,L,M,X) {                                           \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" <<  \
       #K << ": " << K << "\t" << #L << ": " << L << "\t" <<                  \
       #M << ": " << M << "\n";                                               \
       aSsErT(1, #X, __LINE__); } }

// ============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
// ----------------------------------------------------------------------------

#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", " << flush; // 'P(X)' without '\n'
#define T_ cout << "\t" << flush;             // Print tab w/o newline.
#define L_ __LINE__                           // current Line number

// ============================================================================
//                  NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT_SAFE_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(EXPR)
#define ASSERT_SAFE_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(EXPR)
#define ASSERT_PASS(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS(EXPR)
#define ASSERT_FAIL(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL(EXPR)
#define ASSERT_OPT_PASS(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS(EXPR)
#define ASSERT_OPT_FAIL(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL(EXPR)

// ============================================================================
//                     GLOBAL TYPEDEFS FOR TESTING
// ----------------------------------------------------------------------------

typedef btls5::Credentials Obj;

// ============================================================================
//                            MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int                 test = argc > 1 ? atoi(argv[1]) : 0;
    bool             verbose = argc > 2;
    bool         veryVerbose = argc > 3;
    bool     veryVeryVerbose = argc > 4;
    bool veryVeryVeryVerbose = argc > 5;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:
      case 6: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
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

///Example 1: Assign Username and Password
///- - - - - - - - - - - - - - - - - - - -
// Suppose that we want to specify username and password for authenticating
// with a SOCKS5 host (see RFC 1929).  We can use a 'btls5::Credentials' object
// to represent the credentials.
//
// First, we construct an empty object:
//..
    btls5::Credentials credentials;
//..
// Now, we set the username and password passing in literals:
//..
    credentials.set("john.smith", "pass1");
//..
// Finally, we verify that 'credentials' is no longer empty, and its username
// and password values have been assigned the expected values:
//..
    ASSERT(credentials.username() == "john.smith");
    ASSERT(credentials.password() == "pass1");
//..

      } break;
      case 5: {
        // --------------------------------------------------------------------
        // PRINT AND OUTPUT OPERATOR
        //   Ensure that the value of the object can be formatted appropriately
        //   on an 'ostream' in some standard, human-readable form.
        //
        // Concerns:
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
        //: 6 The optional 'level' and 'spacesPerLevel' parameters have the
        //:   correct default values.
        //:
        //: 7 The output 'operator<<' signature and return type are standard.
        //:
        //: 8 The output 'operator<<' returns the supplied 'ostream'.
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
        //:     values ('A' and 'B'), having distinct values for each
        //:     corresponding salient attribute, and various values for the
        //:     two formatting parameters, along with the expected output.
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
        //:       '-8'.  If the parameters are, arbitrarily, (-9, -9), then
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
        //   operator<<(ostream& output, const btls5::Credentials& object);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "PRINT AND OUTPUT OPERATOR" << endl
                          << "=========================" << endl;

        if (verbose) cout << "\nAssign the addresses of 'print' and "
                             "the output 'operator<<' to variables." << endl;
        {
            using namespace btls5;
            typedef ostream& (Obj::*funcPtr)(ostream&, int, int) const;
            typedef ostream& (*operatorPtr)(ostream&, const Obj&);

            // Verify that the signatures and return types are standard.

            funcPtr     printMember = &Obj::print;
            operatorPtr operatorOp  = operator<<;

            (void)printMember;  // quash potential compiler warnings
            (void)operatorOp;
        }

        if (verbose) cout <<
             "\nCreate a table of distinct value/format combinations." << endl;

        static const struct {
            int         d_line;           // source line number
            int         d_level;
            int         d_spacesPerLevel;

            const char *d_username;
            const char *d_password;

            const char *d_expected_p;
        } DATA[] = {

#define NL "\n"
#define SP " "

        // ------------------------------------------------------------------
        // P-2.1.1: { A } x { 0 }     x { 0, 1, -1, -8 } -->  4 expected o/ps
        // ------------------------------------------------------------------

        //LINE L SPL  UNAME PSWD   EXP
        //---- - ---  ----- ----   ---

        { L_,  0,  0, "UA", "PA",  "["                                       NL
                                   "username = \"UA\""                       NL
                                   "password = \"PA\""                       NL
                                   "]"                                       NL
                                                                             },

        { L_,  0,  1, "UA", "PA",  "["                                       NL
                                   " username = \"UA\""                      NL
                                   " password = \"PA\""                      NL
                                   "]"                                       NL
                                                                             },

        { L_,  0, -1, "UA", "PA",  "["                                       SP
                                   "username = \"UA\""                       SP
                                   "password = \"PA\""                       SP
                                   "]"
                                                                             },

        { L_,  0, -8, "UA", "PA",  "["                                       NL
                                   "    username = \"UA\""                   NL
                                   "    password = \"PA\""                   NL
                                   "]"                                       NL
                                                                             },

        // ------------------------------------------------------------------
        // P-2.1.2: { A } x { 3, -3 } x { 0, 2, -2, -8 } -->  6 expected o/ps
        // ------------------------------------------------------------------

        //LINE L SPL  UNAME PSWD   EXP
        //---- - ---  ----- ----   ---

        { L_,  3,  0, "UA", "PA",  "["                                       NL
                                   "username = \"UA\""                       NL
                                   "password = \"PA\""                       NL
                                   "]"                                       NL
                                                                             },

        { L_,  3,  2, "UA", "PA",  "      ["                                 NL
                                   "        username = \"UA\""               NL
                                   "        password = \"PA\""               NL
                                   "      ]"                                 NL
                                                                             },

        { L_,  3, -2, "UA", "PA",  "      ["                                 SP
                                   "username = \"UA\""                       SP
                                   "password = \"PA\""                       SP
                                   "]"
                                                                             },

        { L_,  3, -8, "UA", "PA",
                                "            ["                              NL
                                "                username = \"UA\""          NL
                                "                password = \"PA\""          NL
                                "            ]"                              NL
                                                                             },

        { L_, -3,  0, "UA", "PA",  "["                                       NL
                                   "username = \"UA\""                       NL
                                   "password = \"PA\""                       NL
                                   "]"                                       NL
                                                                             },

        { L_, -3,  2, "UA", "PA",  "["                                       NL
                                   "        username = \"UA\""               NL
                                   "        password = \"PA\""               NL
                                   "      ]"                                 NL
                                                                             },

        { L_, -3, -2, "UA", "PA",  "["                                       SP
                                   "username = \"UA\""                       SP
                                   "password = \"PA\""                       SP
                                   "]"
                                                                             },
        { L_, -3, -8, "UA", "PA",
                                "["                                          NL
                                "                username = \"UA\""          NL
                                "                password = \"PA\""          NL
                                "            ]"                              NL
                                                                             },

        // -----------------------------------------------------------------
        // P-2.1.3: { B } x { 2 }     x { 3 }            -->  1 expected o/p
        // -----------------------------------------------------------------

        //LINE L SPL  UNAME PSWD   EXP
        //---- - ---  ----- ----   ---

        { L_,  2,  3, "UB", "PB",  "      ["                                 NL
                                   "         username = \"UB\""              NL
                                   "         password = \"PB\""              NL
                                   "      ]"                                 NL
                                                                             },

        // -----------------------------------------------------------------
        // P-2.1.4: { A B } x { -8 }   x { -8 }         -->  2 expected o/ps
        // -----------------------------------------------------------------

        //LINE L SPL  UNAME PSWD   EXP
        //---- - ---  ----- ----   ---

        { L_, -8, -8, "UA", "PA",  "["                                       NL
                                   "    username = \"UA\""                   NL
                                   "    password = \"PA\""                   NL
                                   "]"                                       NL
                                                                             },

        { L_, -8, -8, "UB", "PB", "["                                       NL
                                   "    username = \"UB\""                   NL
                                   "    password = \"PB\""                   NL
                                   "]"                                       NL
                                                                             },
        // -----------------------------------------------------------------
        // P-2.1.5: { A B } x { -9 }   x { -9 }         -->  2 expected o/ps
        // -----------------------------------------------------------------

        //LINE L SPL  UNAME PSWD   EXP
        //---- - ---  ----- ----   ---

        { L_, -9, -9, "UA", "PA",  "[ \"UA\" \"PA\" ]"                       },

        { L_, -9, -9, "UB", "PB",  "[ \"UB\" \"PB\" ]"                       },

#undef NL
#undef SP

        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        if (verbose) cout << "\nTesting with various print specifications."
                          << endl;
        {
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int         LINE   = DATA[ti].d_line;
                const int         L      = DATA[ti].d_level;
                const int         SPL    = DATA[ti].d_spacesPerLevel;
                const char *const UNAME  = DATA[ti].d_username;
                const char *const PSWD   = DATA[ti].d_password;
                const char *const EXP    = DATA[ti].d_expected_p;

                if (veryVerbose) { T_ P_(L) P_(SPL) P_(UNAME) P_(PSWD) }

                if (veryVeryVerbose) { T_ T_ Q(EXPECTED) cout << EXP; }

                const Obj X(UNAME, PSWD);

                ostringstream os;

                // Verify supplied stream is returned by reference.

                if (-9 == L && -9 == SPL) {
                    LOOP_ASSERT(LINE, &os == &(os << X));

                    if (veryVeryVerbose) { T_ T_ Q(operator<<) }
                }
                else {
                    LOOP_ASSERT(LINE, -8 == SPL || -8 != L);

                    if (-8 != SPL) {
                        LOOP_ASSERT(LINE, &os == &X.print(os, L, SPL));
                    }
                    else if (-8 != L) {
                        LOOP_ASSERT(LINE, &os == &X.print(os, L));
                    }
                    else {
                        LOOP_ASSERT(LINE, &os == &X.print(os));
                    }

                    if (veryVeryVerbose) { T_ T_ Q(print) }
                }

                // Verify output is formatted as expected.

                if (veryVeryVerbose) { P(os.str()) }

                LOOP3_ASSERT(LINE, EXP, os.str(), EXP == os.str());
            }
        }
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // BASIC ACCESSORS
        //  This case is reserved for testing basic accessors.
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BASIC ACCESSORS" << endl
                          << "===============" << endl;

      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TEST APPARATUS
        //  This case is reserved for validating test apparatus.
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TEST APPARATUS" << endl
                          << "==============" << endl;

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // BTES5_CREDENTIALS
        //  Test the public interface of 'btls5::Credentials'
        //
        // Concerns:
        //: 1 Objects can be constructed, manipulated and accessed.
        //: 2 If supplied, the memory allocator is propagated.
        //: 3 Preconditions are properly verified (in non-optimized builds).
        //
        // Plan:
        //: 1 Use several ad-hoc values, compare output to expected results.
        //: 2 Compare member allocators to construction-supplied allocator.
        //: 3 Use 'bsls_asserttest' to try construction with invalid values.
        //: 3 Use 'bsls_asserttest' to try to 'set' with invalid values.
        //
        // Testing:
        //   explicit btls5::Credentials(bslma::Allocator *basicAllocator = 0);
        //   btls5::Credentials(const StringRef& u, const StringRef& p, *a = 0);
        //   btls5::Credentials(const btls5::Credentials& original, *a = 0);
        //   ~btls5::Credentials() = default;
        //   set(const bslstl::StringRef& u, const bslstl::StringRef& p);
        //   string& username() const;
        //   string& password() const;
        //   operator==(btls5::Credentials& lhs, btls5::Credentials& rhs);
        //   operator!=(btls5::Credentials& lhs, btls5::Credentials& rhs);
        //   CONCERN: All memory allocation is from the object's allocator.
        //   CONCERN: Precondition violations are detected when enabled.
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BTES5_CREDENTIALS" << endl
                          << "=================" << endl;

        btls5::Credentials credentials1;

        credentials1.set("john.smith", "pass1");
        ASSERT(credentials1.username() == "john.smith");
        ASSERT(credentials1.password() == "pass1");
        if (verbose) {
            cout << "credentials1=" << credentials1 << endl;
        }

        btls5::Credentials credentials2(credentials1);
        ASSERT(credentials2.username() == "john.smith");
        ASSERT(credentials2.password() == "pass1");
        ASSERT(credentials1 == credentials2);

        credentials2.set("jane.doe", "pass2");
        if (verbose) {
            cout << "credentials1=" << credentials1
                 << " credentials2=" << credentials2
                 << endl;
        }
        ASSERT(credentials1 != credentials2);
        credentials1 = credentials2;
        ASSERT(credentials1.username() == "jane.doe");
        ASSERT(credentials1.password() == "pass2");

        if (verbose) cout << " allocator propagation" << endl;

        bslma::TestAllocator defaultAllocator("default", veryVeryVerbose);
        bslma::Default::setDefaultAllocatorRaw(&defaultAllocator);

        bslma::TestAllocator ta1("defaultConstructor", veryVeryVerbose);
        btls5::Credentials c1(&ta1);
        ASSERT(c1.username().allocator() == &ta1);
        ASSERT(c1.password().allocator() == &ta1);

        bslma::TestAllocator ta2("initializer", veryVeryVerbose);
        btls5::Credentials c2(
            "A fairly long name to avoid SSO and cause an allocation",
            "A fairly long password to avoid SSO and cause an allocation",
            &ta2);
        ASSERT(c2.username().allocator() == &ta2);

        bslma::TestAllocator ta3("copyConstructor", veryVeryVerbose);
        btls5::Credentials c3(c1, &ta3);
        ASSERT(c3.username().allocator() == &ta3);
        ASSERT(c3.password().allocator() == &ta3);

        // CONCERN: All memory allocation is from the object's allocator.

        LOOP_ASSERT(defaultAllocator.numBlocksTotal(),
                    0 == defaultAllocator.numBlocksTotal());

        if (verbose) cout << " negative test" << endl;
        {
            bsls::AssertFailureHandlerGuard
                g(bsls::AssertTest::failTestDriver);
            string empty;              // too short
            string legal(255, 'a');    // 255 character-string is okay
            string toolong(256, 'a');  // 256 character-string is too long
            BSLS_ASSERTTEST_ASSERT_PASS(btls5::Credentials(legal, legal));

            BSLS_ASSERTTEST_ASSERT_FAIL(btls5::Credentials(empty, legal));
            BSLS_ASSERTTEST_ASSERT_FAIL(btls5::Credentials(legal, empty));
            BSLS_ASSERTTEST_ASSERT_PASS(btls5::Credentials(empty, empty));

            BSLS_ASSERTTEST_ASSERT_FAIL(btls5::Credentials(toolong, legal));
            BSLS_ASSERTTEST_ASSERT_FAIL(btls5::Credentials(legal, toolong));
            BSLS_ASSERTTEST_ASSERT_FAIL(btls5::Credentials(toolong, toolong));

            btls5::Credentials c;
            BSLS_ASSERTTEST_ASSERT_PASS(c.set(legal, legal));

            BSLS_ASSERTTEST_ASSERT_FAIL(c.set(empty, legal));
            BSLS_ASSERTTEST_ASSERT_FAIL(c.set(legal, empty));
            BSLS_ASSERTTEST_ASSERT_PASS(c.set(empty, empty));

            BSLS_ASSERTTEST_ASSERT_FAIL(c.set(toolong, legal));
            BSLS_ASSERTTEST_ASSERT_FAIL(c.set(legal, toolong));
            BSLS_ASSERTTEST_ASSERT_FAIL(c.set(toolong, toolong));

        }
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
        //: 1 Perform and ad-hoc test of the primary modifiers and accessors.
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BREATHING TEST" << endl
                          << "==============" << endl;

        btls5::Credentials credentials1;

        credentials1.set("john.smith", "pass1");
        ASSERT(credentials1.username() == "john.smith");
        ASSERT(credentials1.password() == "pass1");
        if (verbose) {
            cout << "credentials1=" << credentials1 << endl;
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

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2011
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ----------------------------------
