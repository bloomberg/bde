// bdlb_variantprinter.t.cpp                                          -*-C++-*-
#include <bdlb_variantprinter.h>

#include <bslim_testutil.h>

#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>

#include <bsls_assert.h>
#include <bsls_asserttest.h>
#include <bsls_exceptionutil.h>

#include <bsl_cstring.h>
#include <bsl_iostream.h>
#include <bsl_sstream.h>
#include <bsl_string.h>
#include <bsl_variant.h>

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
// The component under test implements a set of helper functions and classes
// to print 'bsl::variant'.
//
// ----------------------------------------------------------------------------
// CREATORS
// [ 2] VariantPrinter<...>(const bsl::variant<...> *);
//
// ACCESSORS
// [ 2] ostream& VariantPrinter<...>::print(ostream&, int = 0, int = 4);
//
// CLASS METHODS
// [ 2] VariantPrinterUtil::makePrinter(const bsl::variant<...>&);
//
// FREE OPERATORS
// [ 2] ostream& operator<<(ostream&, const bdlb::VariantPrinter<T>&);
//
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 3] USAGE EXAMPLE
// [ 4] TESTING std::variant

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

typedef bdlb::VariantPrinterUtil  Util;

//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

//=============================================================================
//                              USAGE EXAMPLE
//-----------------------------------------------------------------------------

namespace shared {

//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Printing `bsl::variant` to a Stream
/// - - - - - - - - - - - - - - - - - - - - - - -
// In this example, we demonstrate how to use `bdlb::VariantPrinterUtil` to
// print `bsl::variant` to a stream.
//
// First, we create a streamable type that throws `int` on copies.  This will
// enable us to put a `variant` into a valueless state by copying one into it:
// ```
    struct ThrowOnCopy {
        // DATA
        int d_ii;

        // CREATORS
        ThrowOnCopy(int ii)
        : d_ii(ii)
        {}

        ThrowOnCopy(const ThrowOnCopy& original)
        : d_ii(original.d_ii)
        {
            BSLS_THROW(0);
        }

        // MANIPULATORS
        ThrowOnCopy& operator=(const ThrowOnCopy& rhs)
        {
            d_ii = rhs.d_ii;

            BSLS_THROW(0);

            return *this;
        }
    };

    bsl::ostream& operator<<(bsl::ostream&      stream,
                             const ThrowOnCopy& value)
    {
        return stream << value.d_ii;
    }
// ```
// BEGIN ADDED
}  // close namespace shared

namespace usage_example_1 {

void run()
{

using namespace shared;
// END ADDED
// Next, we create a shorthand for `VariantPrinterUtil` and our `variant` type:
// ```
    typedef bdlb::VariantPrinterUtil                    Util;
    typedef bsl::variant<int, bsl::string, ThrowOnCopy> VariantIST;
// ```
// Then, we declare a variable of `variant` type, which can hold, among other
// types, a `ThrowOnCopy`:
// ```
    VariantIST        mV(107);
    const VariantIST& V = mV;

    ThrowOnCopy toc(4);
// ```
// Next, we start doing some output:
// ```
    cout << toc << endl;  // prints "4\n"

    cout << Util::makePrinter(V) << endl;  // prints "107\n"

    mV = bsl::string("woof");
    cout << Util::makePrinter(V) << endl;  // prints "woof\n"

    mV = 27;
    cout << Util::makePrinter(V) << endl;  // prints "27\n"
// ```
// Then, we see that the type `VariantPrinter<...>` returned by
// `makePrinter` has a standard BDE-style `print` accessor that will
// control formatting and indenting:
// ```
    mV = bsl::string("bow");
    Util::makePrinter(V).print(cout, 3, -2);  // prints "      bow"
    mV = bsl::string("wow");
    Util::makePrinter(V).print(cout, 1, 1);  // prints " wow\n"
// ```
// Now, we assign a `ThrowOnCopy` to the variant, which will throw when
// copied, leaving `mV` in a `valueless` state, and we observe what
// happens when we then print it.
// ```
    ASSERT(!V.valueless_by_exception());
    BSLS_TRY
    {
        mV = toc;
        ASSERT(false);
    }
    BSLS_CATCH(...)
    {
    }
    ASSERT(V.valueless_by_exception());

    cout << Util::makePrinter(V) << endl;  // prints "(valueless)\n"
// ```
// Finally, we see the output created by all this:
// ```
//  4
//  107
//  woof
//  27
//        bow wow
//  (valueless)
// ```
}
}  // close namespace usage_example_1

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test            = argc > 1 ? atoi(argv[1]) : 0;
    verbose             = argc > 2;
    veryVerbose         = argc > 3;
    veryVeryVerbose     = argc > 4;
    veryVeryVeryVerbose = argc > 5;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    // CONCERN: 'BSLS_REVIEW' failures should lead to test failures.
    bsls::ReviewFailureHandlerGuard reviewGuard(&bsls::Review::failByAbort);

    // CONCERN: In no case does memory come from the global allocator.

    bslma::TestAllocator globalAllocator("global", veryVeryVeryVerbose);
    bslma::Default::setGlobalAllocator(&globalAllocator);

    // CONCERN: In no case does memory come from the default allocator.

    bslma::TestAllocator defaultAllocator("default", veryVeryVeryVerbose);
    bslma::DefaultAllocatorGuard defaultAllocatorGuard(&defaultAllocator);

    bslma::TestAllocator testAllocator("test", veryVeryVeryVerbose);

    switch (test) {
      case 0:  // Zero is always the leading case.
      case 4: {
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

        if (verbose)
            cout << "USAGE EXAMPLE\n"
                    "=============\n";

        bsl::ostringstream  oss(&testAllocator);
        bsl::streambuf     *saveCoutStreamBuf = cout.rdbuf();
        cout.rdbuf(oss.rdbuf());

        usage_example_1::run();

        cout.rdbuf(saveCoutStreamBuf);

        ASSERT("4\n107\nwoof\n27\n      bow wow\n(valueless)\n" == oss.view());
        if (veryVerbose) {
            cout << oss.view();
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING std::variant
        //
        // Concerns:
        // 1 That 'bdlb::VariantPrinter' can work with 'std::variant' as well
        //   as 'bsl::variant'.
        //
        // Plan:
        // 1. Use an 'std::variant':
        //    * Create the 'std::variant'.
        //
        //    * Assign valuesof different types to it.
        //
        //    * Print them out.
        //
        // 2. We'll just copy the usage example, just using an `std::variant`
        //    rather than a `bsl::variant`.
        //
        // Testing:
        //   COMPATIBILITY WITH std::variant
        // --------------------------------------------------------------------

#ifndef BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY
        if (verbose) cout << "TESTING std::variant skipped on this platform\n"
                             "=============================================\n";
#else
        if (verbose) cout << "TESTING std::variant\n"
                             "====================\n";

        bsl::ostringstream  oss(&testAllocator);
        bsl::streambuf     *saveCoutStreamBuf = cout.rdbuf();
        cout.rdbuf(oss.rdbuf());

        using namespace shared;
        typedef bdlb::VariantPrinterUtil Util;

        // Note that if the 'string' in this 'std::variant' is a 'bsl::string',
        // the ADL choosing 'visit' within 'VariantPrinter<...>::print' gets
        // confused and tries matching 'bsl::visit' which doesn't compile.  If
        // one wants to create a 'variant' that can hold a 'bsl::string', it is
        // best to use 'bsl::variant' (as is done in the usage example), which
        // will work, with the added advantage that enables the client to
        // specify which allocator is to be used.

        std::variant<int, std::string, ThrowOnCopy>        mV(107);
        const std::variant<int, std::string, ThrowOnCopy>& V = mV;

        ThrowOnCopy toc(4);

        cout << toc << endl;

        cout << Util::makePrinter(V) << endl;  // prints "107\n"

        mV = bsl::string("woof");
        cout << Util::makePrinter(V) << endl;  // prints "woof\n"

        mV = 27;
        cout << Util::makePrinter(V) << endl;  // prints "27\n"

        mV = bsl::string("bow");
        Util::makePrinter(V).print(cout, 3, -2);  // prints "      bow"
        mV = bsl::string("wow");
        Util::makePrinter(V).print(cout, 1, 1);  // prints " wow\n"

        ASSERT(!V.valueless_by_exception());
        BSLS_TRY
        {
            mV = toc;
            ASSERT(false);
        }
        BSLS_CATCH(...)
        {
        }
        ASSERT(V.valueless_by_exception());

        cout << Util::makePrinter(V) << endl;  // prints "(valueless)\n"

        const char *EXP = "4\n107\nwoof\n27\n      bow wow\n(valueless)\n";

        cout.rdbuf(saveCoutStreamBuf);

        ASSERT(EXP == oss.view());

        if (veryVerbose) {
            cout << oss.view();
        }
#endif
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // PRINT AND OUTPUT OPERATOR (<<)
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
        // 4. The `print` method's signature and return type are standard.
        //
        // 5. The `print` method returns the supplied `ostream`.
        //
        // 6. The optional `level` and `spacesPerLevel` parameters have the
        //    correct default values (0 and 4, respectively).
        //
        // 7. The output `operator<<`s signature and return type are standard.
        //
        // 8. The output `operator<<` returns the destination `ostream`.
        //
        // Plan:
        // 1. Use the addresses of the `print` member function and `operator<<`
        //    free function defined in this component to initialize,
        //    respectively, member-function and free-function pointers having
        //    the appropriate signatures and return types.  (C-4, 7)
        //
        // 2 .Using the table-driven technique:  (C-1..3, 5..6, 8)
        //
        //   1. Define selected combinations of object values, and various
        //      values for the two formatting parameters, along with the
        //      expected output.  Supply both `int` and `double` values, where
        //      exactly one of the two is always 0, and the non-zero one is
        //      always the one that is used.
        // ```
        //      (  `level`   x `spacesPerLevel` ):
        //      1 {  0     } x {  0, 1, -1, -8 }
        //      2 {  3, -3 } x {  0, 2, -2, -8 }
        //      3 {  2     } x {  3            }
        //      4 { -8     } x { -8            }
        //      5 { -9     } x { -9            }
        // ```
        //   2. Declare a non-modifiable `variant<int, double, bsl::string>`
        //      `mV` and a const reference `V` to `mV`, and a
        //      `bdlb::VariantPrinter<int, double, bsl::string>` `X(&V)`;
        //
        //   3. For each row in the table defined in P-2.1:  (C-1..3, 5..6, 8)
        //
        //     * Assign either the `int` value, the `double` value, or the
        //       string value from the table, whichever is non-zero, to `mV`.
        //
        //     * If the `int`, `double` and string values are all 0, set the
        //       variant to `valueless_by_exception`.  To get `mV` into a
        //       `valueless_by_exception` state, we create the `ThrowOnCopy`
        //       type which throws when it is copied or assigned.  When that
        //       is assigned to the variant, it will throw before the variant
        //       completes its transition to the assigned type, leaving thee
        //       variant "valueless", which will be printed out as
        //       "(valueless)".
        //
        //     * If the `level` and `spacesPerLevel` values are both `-9`,
        //       call `operator<<` on `Util::makePrinter(V)`.
        //
        //     * If the `level` and `spacesPerLevel` values are both `-10`,
        //       call `operator<<` on `X`.
        //
        //     * If the conditions for `2` or `3` aren't met, call `X.print`
        //       formatting parameters to `print`, omitting the `level` or
        //       `spacesPerLevel` parameter if the value of that argument is
        //       `-8`.
        //
        //     * Use a standard `bdlsb::FixedMemOutStreamBuf` to capture the
        //       actual output.
        //
        //     * Verify the address of what is returned is that of the
        //       supplied stream.  (C-5, 8)
        //
        //     * Compare the contents captured in P-2.2.2 with what is
        //       expected.  (C-1..3, 6)
        //
        // Testing:
        //   VariantPrinter<...>(const bsl::variant<...> *);
        //   ostream& VariantPrinter<...>::print(ostream&, int = 0, int = 4);
        //   ostream& operator<<(ostream&, const bdlb::VariantPrinter<T>&);
        //   VariantPrinterUtil::makePrinter(const bsl::variant<...>&);
        // --------------------------------------------------------------------

        if (verbose)
            cout << "PRINT AND OUTPUT OPERATOR (<<)\n"
                    "==============================\n";

        using namespace shared;

        typedef bsl::variant<int, double, bsl::string, ThrowOnCopy>
                                                  VariantIDST;
        typedef bdlb::VariantPrinter<VariantIDST> VariantPrinterIDST;
        if (veryVerbose)
            cout << "Assign the addresses of `print` and "
                    "the output `operator<<` to variables.\n";
        {
            typedef bsl::ostream& (
                VariantPrinterIDST:: *PrintPtr)(bsl::ostream&, int, int) const;
            typedef bsl::ostream& (*OperatorPtr)(bsl::ostream&,
                                                 const VariantPrinterIDST&);

            PrintPtr    printPtr    = &VariantPrinterIDST::print;
            OperatorPtr operatorOut = &bdlb::operator<<;

            (void)printPtr;  // quash potential compiler warnings
            (void)operatorOut;
        }

        if (verbose)
            cout << "\nCreate a table of distinct value/format combinations."
                 << endl;

        static const struct Data {
            int         d_line;            // source line number
            int         d_level;
            int         d_spacesPerLevel;
            int         d_intValue;
            double      d_doubleValue;
            const char *d_stringValue_p;
            const char *d_expected_p;
        } DATA[] = {

#define NL "\n"

        //LINE L SPL   IVALUE  DVALUE  SVALUE    EXP
        //---- - ---   ------  ------  ------    ---------------------

        { L_,  0,  0,  123,    0,      0,        "123"                     NL},
        { L_,  0,  1,  124,    0,      0,        "124"                     NL},
        { L_,  0, -1,  125,    0,      0,        "125"                       },
        { L_,  0, -8,  126,    0,      0,        "126"                     NL},
        { L_,  3,  0,  130,    0,      0,        "130"                     NL},
        { L_,  3,  2,  131,    0,      0,        "      131"               NL},
        { L_,  3, -2,  132,    0,      0,        "      132"                 },
        { L_,  3, -8,  133,    0,      0,        "            133"         NL},
        { L_, -3,  0,  134,    0,      0,        "134"                     NL},
        { L_, -3,  2,  135,    0,      0,        "135"                     NL},
        { L_, -3, -2,  136,    0,      0,        "136"                       },
        { L_, -3, -8,  137,    0,      0,        "137"                     NL},
        { L_,  2,  3,  140,    0,      0,        "      140"               NL},
        { L_, -8, -8,  150,    0,      0,        "150"                     NL},

        { L_, -9, -9,  160,    0,      0,        "160"                       },
        { L_,-10,-10,  170,    0,      0,        "170"                       },

        { L_,  0,  0,    0,    123.5,  0,        "123.5"                   NL},
        { L_,  0,  1,    0,    124.5,  0,        "124.5"                   NL},
        { L_,  0, -1,    0,    125.5,  0,        "125.5"                     },
        { L_,  0, -8,    0,    126.5,  0,        "126.5"                   NL},
        { L_,  3,  0,    0,    130.5,  0,        "130.5"                   NL},
        { L_,  3,  2,    0,    131.5,  0,        "      131.5"             NL},
        { L_,  3, -2,    0,    132.5,  0,        "      132.5"               },
        { L_,  3, -8,    0,    133.5,  0,        "            133.5"       NL},
        { L_, -3,  0,    0,    134.5,  0,        "134.5"                   NL},
        { L_, -3,  2,    0,    135.5,  0,        "135.5"                   NL},
        { L_, -3, -2,    0,    136.5,  0,        "136.5"                     },
        { L_, -3, -8,    0,    137.5,  0,        "137.5"                   NL},
        { L_,  2,  3,    0,    140.5,  0,        "      140.5"             NL},
        { L_, -8, -8,    0,    150.5,  0,        "150.5"                   NL},

        { L_, -9, -9,    0,    160.5,  0,        "160.5"                     },
        { L_,-10,-10,    0,    170.5,  0,        "170.5"                     },

        { L_,  0,  0,    0,    0,      "The",    "The"                     NL},
        { L_,  0,  1,    0,    0,      "rain",   "rain"                    NL},
        { L_,  0, -1,    0,    0,      "in",     "in"                        },
        { L_,  0, -8,    0,    0,      "Spain",  "Spain"                   NL},
        { L_,  3,  0,    0,    0,      "falls",  "falls"                   NL},
        { L_,  3,  2,    0,    0,      "mainly", "      mainly"            NL},
        { L_,  3, -2,    0,    0,      "in",     "      in"                  },
        { L_,  3, -8,    0,    0,      "the",    "            the"         NL},
        { L_, -3,  0,    0,    0,      "plain.", "plain."                  NL},
        { L_, -3,  2,    0,    0,      "To",     "To"                      NL},
        { L_, -3, -2,    0,    0,      "be",     "be"                        },
        { L_, -3, -8,    0,    0,      "or",     "or"                      NL},
        { L_,  2,  3,    0,    0,      "not",    "      not"               NL},
        { L_, -8, -8,    0,    0,      "to",     "to"                      NL},

        { L_, -9, -9,    0,    0,      "be",     "be"                        },
        { L_,-10,-10,    0,    0,      ".",      "."                         },

        { L_,  0,  0,    0,    0,      0,        "(valueless)"             NL},
        { L_,  0,  1,    0,    0,      0,        "(valueless)"             NL},
        { L_,  0, -1,    0,    0,      0,        "(valueless)"               },
        { L_,  0, -8,    0,    0,      0,        "(valueless)"             NL},
        { L_,  3,  0,    0,    0,      0,        "(valueless)"             NL},
        { L_,  3,  2,    0,    0,      0,        "      (valueless)"       NL},
        { L_,  3, -2,    0,    0,      0,        "      (valueless)"         },
        { L_,  3, -8,    0,    0,      0,        "            (valueless)" NL},
        { L_, -3,  0,    0,    0,      0,        "(valueless)"             NL},
        { L_, -3,  2,    0,    0,      0,        "(valueless)"             NL},
        { L_, -3, -2,    0,    0,      0,        "(valueless)"               },
        { L_, -3, -8,    0,    0,      0,        "(valueless)"             NL},
        { L_,  2,  3,    0,    0,      0,        "      (valueless)"       NL},
        { L_, -8, -8,    0,    0,      0,        "(valueless)"             NL},

        { L_, -9, -9,    0,    0,      0,        "(valueless)"               },
        { L_,-10,-10,    0,    0,      0,        "(valueless)"               },
#undef NL

        };
        enum { k_NUM_DATA = sizeof DATA / sizeof *DATA };

        if (verbose)
            cout << "\nTesting with various print specifications." << endl;
        {
            bsl::allocator<char>     alloc(&testAllocator);
            VariantIDST              mV(bsl::allocator_arg_t(), alloc);
            const VariantIDST&       V = mV;
            const VariantPrinterIDST X(&V);
            const ThrowOnCopy        TOC(23);

            for (int ti = 0; ti < k_NUM_DATA; ++ti) {
                const Data&         data  = DATA[ti];
                const int           LINE  = data.d_line;
                const int           L     = data.d_level;
                const int           SPL   = data.d_spacesPerLevel;
                const int           IVAL  = data.d_intValue;
                const double        DVAL  = data.d_doubleValue;
                const char         *SVAL  = data.d_stringValue_p;
                const char         *PSVAL = SVAL ? SVAL : "";
                const char *const   EXP   = data.d_expected_p;

                if (veryVerbose) {
                    P_(LINE) P_(L) P_(SPL) P_(IVAL) P_(DVAL) P_(PSVAL);
                }

                ASSERT(!!IVAL + !!DVAL + !!SVAL <= 1);
                if (0 != IVAL) {
                    mV = IVAL;
                }
                else if (0 != DVAL) {
                    mV = DVAL;
                }
                else if (0 != SVAL) {
                    const bsl::string& stringSVAL(SVAL);
                    ASSERT(!stringSVAL.empty());
                    mV = stringSVAL;
                }
                else {
                    mV = 0;
                    ASSERT(!V.valueless_by_exception());
                    BSLS_TRY
                    {
                        mV = TOC;
                        ASSERT(0);
                    }
                    BSLS_CATCH(...)
                    {
                    }
                    ASSERT(V.valueless_by_exception());
                }

                bsl::ostringstream os(&testAllocator);

                // Verify supplied stream is returned by reference.
                if (-9 == L && -9 == SPL) {
                    ASSERTV(LINE, &os == &(os << Util::makePrinter(V)));
                }
                else if (-10 == L && -10 == SPL) {
                    ASSERTV(LINE, &os == &(os << X));
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
                }

                if (veryVerbose)
                    cout << '"' << os.view() << "\"\n";

                ASSERTV(LINE, EXP, os.view(), os.view() == EXP);
            }
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //   This case exercises (but does not fully test) basic functionality.
        //
        // Concerns:
        // 1. The class is sufficiently functional to enable comprehensive
        //   testing in subsequent test cases.
        //
        // Plan:
        // 1. Demonstrate basic printing of a 'variant<...>' using a
        //   'VariantPrinter<variant<...> >'.
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) cout << "BREATHING TEST\n"
                             "==============\n";

        typedef bsl::variant<int, double, bsl::string> Variant3;
        typedef bdlb::VariantPrinter<Variant3>         Printer3;

        bsl::ostringstream                     oss(&testAllocator);
        Variant3                               value(42);

        Printer3(&value).print(oss);    // "42\n";

        value = 47.25;
        oss << bdlb::VariantPrinterUtil::makePrinter(value);    // "47.25"

        value = bsl::string("Hello, world");
        oss << ", " << Printer3(&value) << endl;         // ", Hello, world\n"

        const char *EXP = "42\n47.25, Hello, world\n";

        if (veryVerbose) cout << "\"" << oss.view() << "\"\n";

        ASSERTV(EXP, oss.view(), bsl::strlen(EXP), oss.view().size(),
                                                            oss.view() == EXP);
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
