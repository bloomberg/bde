// bdlb_optionalprinter.t.cpp                                         -*-C++-*-
#include <bdlb_optionalprinter.h>

#include <bslim_testutil.h>

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
//                              --------
// The component under test implements a set of helper functions and classes
// to print 'bsl::optional'.
//
// ----------------------------------------------------------------------------
// CREATORS
// [ 2] OptionalPrinter(const bsl::optional<TYPE>* );
//
// ACCESSORS
// [ 2] ostream& OptionalPrinter<TYPE>::print(ostream&, int = 0, int = 4);
//
// CLASS METHODS
// [ 2] OptionalPrinterUtil::makePrinter(const bsl::optional<TYPE>&);
//
// FREE OPERATORS
// [ 2] ostream& operator<<(ostream&, const bdlb::OptionalPrinter<T>&);
//
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 3] USAGE EXAMPLE

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

#define ASSERT_SAFE_PASS_RAW(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS_RAW(EXPR)
#define ASSERT_SAFE_FAIL_RAW(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL_RAW(EXPR)
#define ASSERT_PASS_RAW(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS_RAW(EXPR)
#define ASSERT_FAIL_RAW(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL_RAW(EXPR)
#define ASSERT_OPT_PASS_RAW(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(EXPR)
#define ASSERT_OPT_FAIL_RAW(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(EXPR)

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------
static bool verbose;
static bool veryVerbose;
static bool veryVeryVerbose;
static bool veryVeryVeryVerbose;

typedef bdlb::OptionalPrinterUtil  Util;

//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

namespace {
namespace u {
template <class TYPE>
struct TestUtil  {
    typedef bsl::ostream& (bdlb::OptionalPrinter<TYPE>::*printPtr)(
                                                                 bsl::ostream&,
                                                                 int,
                                                                 int) const;

    typedef bsl::ostream& (*operatorPtr)(bsl::ostream&,
                                         const bdlb::OptionalPrinter<TYPE>&);
};
}  // close namespace u
}  // close unnamed namespace

//=============================================================================
//                              USAGE EXAMPLE
//-----------------------------------------------------------------------------


//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int            test = argc > 1 ? atoi(argv[1]) : 0;
                verbose = argc > 2;
            veryVerbose = argc > 3;
        veryVeryVerbose = argc > 4;
    veryVeryVeryVerbose = argc > 5;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;
    //
    // CONCERN: 'BSLS_REVIEW' failures should lead to test failures.
    bsls::ReviewFailureHandlerGuard reviewGuard(&bsls::Review::failByAbort);

    // CONCERN: In no case does memory come from the global allocator.

    bslma::TestAllocator globalAllocator("global", veryVeryVeryVerbose);
    bslma::Default::setGlobalAllocator(&globalAllocator);

    // CONCERN: In no case does memory come from the default allocator.

    bslma::TestAllocator defaultAllocator("default", veryVeryVeryVerbose);
    bslma::DefaultAllocatorGuard defaultAllocatorGuard(&defaultAllocator);

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

        if (verbose) cout << endl
                          << "USAGE EXAMPLE" << endl
                          << "=============" << endl;

///Example 1: Printing 'bsl::optional' to a stream
///- - - - - - - - - - - - - - - - - - - - - - - -
// In this example, we demonstrate how to use 'bdlb::OptionalPrinterUtil' to
// print 'bsl::optional' to a stream:
//..
    bsl::optional<int> value(42);
    bsl::cout << bdlb::OptionalPrinterUtil::makePrinter(value);
//..
      } break;
      case 2: {
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
        //:   1 Define selected combinations of object values, and various
        //:     values for the two formatting parameters, along with the
        //:     expected output.
        //:
        //:     (  'level'   x 'spacesPerLevel' ):
        //:     1 {  0     } x {  0, 1, -1, -8 }
        //:     2 {  3, -3 } x {  0, 2, -2, -8 }
        //:     3 {  2     } x {  3            }
        //:     4 { -8     } x { -8            }
        //:     5 { -9     } x { -9            }
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
        //   OptionalPrinter(const bsl::optional<TYPE>* );
        //   ostream& OptionalPrinter<TYPE>::print(ostream&, int = 0, int = 4);
        //   ostream& operator<<(ostream&, const bdlb::OptionalPrinter<T>&);
        //   OptionalPrinterUtil::makePrinter(const bsl::optional<TYPE>&);
        // --------------------------------------------------------------------
        if (verbose) cout << "\nPRINT AND OUTPUT OPERATOR (<<)"
                             "\n=============================="
                          << endl;

        if (veryVerbose) cout << "\nAssign the addresses of 'print' and "
                              << "the output 'operator<<' to variables."
                              << endl;
        {
            u::TestUtil<int>::printPtr printPtr =
                                            &bdlb::OptionalPrinter<int>::print;

            u::TestUtil<int>::operatorPtr operatorOut = &bdlb::operator<<<int>;

            (void)printPtr;  // quash potential compiler warnings
            (void)operatorOut;
        }

        if (verbose) cout <<
             "\nCreate a table of distinct value/format combinations." << endl;

        static const struct {
            int         d_line;            // source line number
            int         d_level;
            int         d_spacesPerLevel;

            int         d_value;
            const char *d_expected_p;
        } DATA[] = {

#define NL "\n"

        //LINE L SPL   VALUE  EXP
        //---- - ---   ----   ----

        { L_,  0,  0,  123,   "123"              NL },
        { L_,  0,  1,  124,   "124"              NL },
        { L_,  0, -1,  125,   "125"                 },
        { L_,  0, -8,  126,   "126"              NL },
        { L_,  3,  0,  130,   "130"              NL },
        { L_,  3,  2,  131,   "      131"        NL },
        { L_,  3, -2,  132,   "      132"           },
        { L_,  3, -8,  133,   "            133"  NL },
        { L_, -3,  0,  134,   "134"              NL },
        { L_, -3,  2,  135,   "135"              NL },
        { L_, -3, -2,  136,   "136"                 },
        { L_, -3, -8,  137,   "137"              NL },
        { L_,  2,  3,  140,   "      140"        NL },
        { L_, -8, -8,  150,   "150"              NL },

        { L_, -9, -9,  160,   "160"                 },

        // -1 generates empty bsl::optional for printing
        { L_,  0,  0,  -1,    "NULL"             NL },
        { L_,  0,  1,  -1,    "NULL"             NL },
        { L_,  0, -1,  -1,    "NULL"                },
        { L_,  0, -8,  -1,    "NULL"             NL },
        { L_,  3,  0,  -1,    "NULL"             NL },
        { L_,  3,  2,  -1,    "      NULL"       NL },
        { L_,  3, -2,  -1,    "      NULL"          },
        { L_,  3, -8,  -1,    "            NULL" NL },
        { L_, -3,  0,  -1,    "NULL"             NL },
        { L_, -3,  2,  -1,    "NULL"             NL },
        { L_, -3, -2,  -1,    "NULL"                },
        { L_, -3, -8,  -1,    "NULL"             NL },
        { L_,  2,  3,  -1,    "      NULL"       NL },
        { L_, -8, -8,  -1,    "NULL"             NL },

        { L_, -9, -9,  -1,    "NULL"                },

#undef NL

        };
        const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

        if (verbose) cout << "\nTesting with various print specifications."
                          << endl;
        {
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int         LINE  = DATA[ti].d_line;
                const int         L     = DATA[ti].d_level;
                const int         SPL   = DATA[ti].d_spacesPerLevel;
                const int         VAL   = DATA[ti].d_value;
                const char *const EXP   = DATA[ti].d_expected_p;

                if (veryVerbose) {
                    T_ P_(LINE) P_(L) P(SPL)
                    T_ P(VAL)
                }

                bsl::optional<int> mV(VAL);
                const bsl::optional<int>& V = mV;

                if (-1 == VAL) {
                    mV.reset();
                }

                const bdlb::OptionalPrinter<int> X(&V);


                bslma::TestAllocator oa("object", veryVeryVeryVerbose);

                ostringstream os(&oa);

                // Verify supplied stream is returned by reference.
                if (-9 == L && -9 == SPL) {
                    ASSERTV(LINE, &os == &(os << Util::makePrinter(V)));

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
        //: 1 TBD
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) cout << "\nBREATHING TEST"
                          << "\n=============="
                          << endl;
        {
            bsl::optional<int> value;
            bsl::cout << Util::makePrinter(value) << "\n";
        }

        {
            bsl::optional<int> value(42);
            bsl::cout << Util::makePrinter(value) << "\n";
        }

        {
            bsl::optional<bsl::string> value("Hello, world");
            bdlb::OptionalPrinter<bsl::string>(&value).print(bsl::cout);
        }
      } break;
      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      }
    }

    // CONCERN: In no case does memory come from the global allocator.

    ASSERTV(globalAllocator.numBlocksTotal(),
            0 == globalAllocator.numBlocksTotal());

    // CONCERN: In no case does memory come from the default allocator.

    ASSERTV(defaultAllocator.numBlocksTotal(),
            0 == defaultAllocator.numBlocksTotal());

    if (testStatus > 0) {
        cerr << "Error, non-zero test status = " << testStatus << "." << endl;
    }

    return testStatus;
}

// ----------------------------------------------------------------------------
// Copyright 2021 Bloomberg Finance L.P.
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
