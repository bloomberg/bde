// bdlb_printadapter.t.cpp                                            -*-C++-*-
#include <bdlb_printadapter.h>

#include <bdlb_random.h>

#include <bdlb_indexspan.h>
#include <bdlb_printmethods.h> // 'bdlb::HasPrintMethod'

#include <bslim_printer.h>
#include <bslim_testutil.h>

#include <bslmf_integralconstant.h>
#include <bslmf_issame.h>
#include <bslmf_nestedtraitdeclaration.h>

#include <bsls_assert.h>
#include <bsls_asserttest.h>
#include <bsls_compilerfeatures.h>
#include <bsls_nameof.h>
#include <bsls_review.h>
#include <bsls_types.h>

#include <bsl_cmath.h>                    // 'bsl::abs'
#include <bsl_cstdlib.h>                  // 'bsl::atoi'
#include <bsl_iostream.h>
#include <bsl_sstream.h>
#include <bsl_string.h>                   // 'bsl::string', 'bsl::to_string'
#include <bsl_vector.h>

using namespace BloombergLP;
using bsl::cout;
using bsl::cerr;
using bsl::endl;
using bsl::flush;
using bsl::size_t;

// ============================================================================
//                                 TEST PLAN
// ----------------------------------------------------------------------------
//                                 Overview
//                                 --------
//
//-----------------------------------------------------------------------------
//
//                              //-------------
//                              // PrintAdapter
//                              //-------------
//
// CREATORS
// [ 3] PrintAdapter (C++17)
// [ 3] PrintAdapter<TYPE>
// [ 2] PrintAdapter (C++17)
// [ 2] PrintAdapter<TYPE>
//
// FREE FUNCTIONS
// [ 3] operator<<
// [ 2] operator<<
//
//                            //-----------------
//                            // PrintAdapterUtil
//                            //-----------------
//
// STATIC FUNCTIONS
// [ 3] PrintAdapterUtil::makeAdapter
// [ 2] PrintAdapterUtil::makeAdapter
//
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 4] USAGE EXAMPLE 1
// [ 5] USAGE EXAMPLE 2
// [ 6] USAGE EXAMPLE 3

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

// ============================================================================
//                     GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------

// ============================================================================
//                         GLOBAL VARIABLES FOR TESTING
// ----------------------------------------------------------------------------

bool             verbose;
bool         veryVerbose;
bool     veryVeryVerbose;
bool veryVeryVeryVerbose;

//=============================================================================
//                      GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

namespace {
namespace u {

template <class TYPE>
void streamValueImp(bsl::ostream& stream,
                    const TYPE&   value,
                    int           level,
                    int           spacesPerLevel,
                    bsl::true_type)
    // Use 'TYPE::print' to stream the specified 'value' to the specified
    // 'stream', using the specified 'level' and 'spacesPerLevel'.
{
    value.print(stream, level, spacesPerLevel);
}

template <class TYPE>
void streamValueImp(bsl::ostream& stream,
                    const TYPE&   value,
                    int           level,
                    int           spacesPerLevel,
                    bsl::false_type)
    // The template parameter 'TYPE' has no 'print' method.  Use
    // 'bdlb::PrintMethods::print' to stream the specified 'value' to the
    // specified 'stream', using the speceified 'level' and 'spacesPerLevel'.
{
    bdlb::PrintMethods::print(stream, value, level, spacesPerLevel);
}

template <class TYPE>
void streamValue(bsl::ostream& stream,
                 const TYPE&   value,
                 int           level,
                 int           spacesPerLevel)
    // If 'TYPE' has a 'print' method, use that to stream the specified 'value'
    // to the specified 'stream', passing 'print' the specified 'level' and
    // 'spacesPerLevel'.  If not, stream using 'bdlb::PrintMethods::print'.
{
    u::streamValueImp(stream,
                      value,
                      level,
                      spacesPerLevel,
                      bdlb::HasPrintMethod<TYPE>());
}

struct NoPrintType {
    // This 'struct' provides a stateful type that has no 'print' operator.

    // DATA
    int d_ii;

    // CREATOR
    explicit
    NoPrintType(int ii)
    : d_ii(ii)
    {}

    // NoPrintType(const NoPrintType&) = default;

    // MANIPULATORS
    // NoPrintType& operator=(const NoPrintType&) = default;

    // ACCESSORS
    operator int() const
    {
        return d_ii;
    }
};

// FREE OPERATORS
bsl::ostream& operator<<(bsl::ostream& stream, const NoPrintType& value)
{
    stream << "[ ii = " << static_cast<int>(value) << " ]";

    return stream;
}

struct HasPrintType {
    // This 'struct' provides a stateful type that has a 'print' operator.

    // DATA
    int d_woof;
    int d_meow;
    int d_arf;
    int d_bowwow;

    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(HasPrintType, bdlb::HasPrintMethod);

    // ACCESSORS
    bsl::ostream& print(bsl::ostream& stream,
                        int           level          = 0,
                        int           spacesPerLevel = 4) const;
};

// TRAITS
bsl::ostream& HasPrintType::print(bsl::ostream& stream,
                                  int           level,
                                  int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute("woof", d_woof);
    printer.printAttribute("meow", d_meow);
    printer.printAttribute("arf", d_arf);
    printer.printAttribute("bowwow", d_bowwow);
    printer.end();

    return stream;
}

template <class TYPE>
void testValues(const TYPE *valueArray, int valueArrayLen)
    // --------------------------------------------------------------------
    // CONSTRUCTOR, STREAM, and MAKEADAPTER
    //
    // Concerns:
    //: 1 That the output created by the 'class' under test streams the desired
    //:   output over a wide range of inputs.
    //:
    //: 2 That 'operator<<' returns a reference to the 'ostream' that it is
    //:   passed.
    //
    // PlAN:
    //: 1 Iterate a reference 'X' through the specified 'valueArray'.
    //:
    //: 2 Within that loop, iterate through several values of 'level'.
    //:
    //: 3 Within that loop, iterate through several values of 'spacesPerLevel'.
    //:
    //: 4 Use 'bdlb::HasPrintMethod' to determine whether 'TYPE' has a BDE
    //:   standard 'print' member, and call the template function
    //:   'streamValue':
    //:   1 If 'TYPE' has a print member, call that directly with the 'level'
    //:     and 'spacesPerLevel' from the loop variables.
    //:
    //:   2 If 'TYPE' has no print member, call 'bdlb::PrintMethods' to stream
    //:     'X'.
    //:
    //:   3 After either '1' or '2', capture the streamed output to a string,
    //:     'rawResult' to be compared to output streamed using 'PrintAdapter'.
    //:
    //: 5 If the compiler supports CTAD, construct a 'PrintAdapter' object from
    //:   'X' (without supplying a template argument) and the 'level' and
    //:   'spacesPerLevel' from the loop variables, and stream it, capturing
    //:   the result in the 'string' 'ctadResult' and compare it to
    //:   'rawResult'.
    //:
    //: 6 Construct a 'PrintAdapter<TYPE>' object from 'X' and the 'level' and
    //:   'spacesPerLevel' from the loop variables, and stream it, capturing
    //:   the result in the 'string' 'adapterResult' and compare it to
    //:   'rawResults'.
    //:
    //: 7 In P-5 and P-6, if it is possible to default the 'level' and
    //:   'spacesPerLevel' arguments to the 'PrintAdapter', do that instead of
    //:   passing values.
    //:
    //: 8 In P-5 and P-6, take a pointer to the stream reference returned, and
    //:   verify that it matches the address of the 'ostringstream' passed to
    //:   'operator<<' call.
    // --------------------------------------------------------------------
{
    const char *name = bsls::NameOf<TYPE>().name();

    if (verbose) cout << "u::testValues<TYPE = " << name << ">\n";
    if (verbose) cout << "bdlb::HasPrintMethod<" << name << ">::value = " <<
                                     bdlb::HasPrintMethod<TYPE>::value << endl;

    for (int ti = 0; ti < valueArrayLen; ++ti) {
        const TYPE& X = valueArray[ti];

        for (int lev = -2; lev < 4; ++lev) {
            for (int spcPerLev = -2; spcPerLev < 6; ++spcPerLev) {
                bsl::ostringstream oss;
                u::streamValue(oss,
                               X,
                               lev,
                               spcPerLev);
                const bsl::string& rawResult = oss.str();

                bsl::ostream *ret_p;
                static bool firstTime = true;
#ifdef BSLS_COMPILERFEATURES_SUPPORT_CTAD
                if (firstTime && verbose) Q(CTAD);

                oss.str("");
                ret_p = 4 == spcPerLev
                      ? (0 == lev
                        ? &(oss << bdlb::PrintAdapter(&X))
                        : &(oss << bdlb::PrintAdapter(&X, lev)))
                      : &(oss << bdlb::PrintAdapter(&X, lev, spcPerLev));
                ASSERT(&oss == ret_p);
                const bsl::string& ctadResult = oss.str();

                ASSERTV(lev, spcPerLev, rawResult, ctadResult,
                                                      rawResult == ctadResult);
#else
                if (firstTime && verbose) Q(NO CTAD);
#endif
                firstTime = false;

                oss.str("");
                typedef bdlb::PrintAdapter<TYPE> PAIS;
                ret_p = 4 == spcPerLev
                      ? (0 == lev
                        ? &(oss << PAIS(&X))
                        : &(oss << PAIS(&X, lev)))
                      : &(oss << PAIS(&X, lev, spcPerLev));
                ASSERT(&oss == ret_p);
                const bsl::string& adapterResult = oss.str();

                ASSERTV(lev, spcPerLev, rawResult, adapterResult,
                                                   rawResult == adapterResult);

                oss.str("");
                typedef bdlb::PrintAdapterUtil Util;
                ret_p = 4 == spcPerLev
                      ? (0 == lev
                        ? &(oss << Util::makeAdapter(X))
                        : &(oss << Util::makeAdapter(X, lev)))
                      : &(oss << Util::makeAdapter(X, lev, spcPerLev));
                ASSERT(&oss == ret_p);
                const bsl::string& makeResult = oss.str();

                ASSERTV(lev, spcPerLev, rawResult, makeResult,
                                                      rawResult == makeResult);

                if (veryVerbose) {
                    P_(lev); P_(spcPerLev); P(adapterResult);
                }
            }
        }
    }
}

void randIntVec(bsl::vector<int> *result)
{
    static int seed = 12345678;
    const unsigned len = bdlb::Random::generate15(&seed) & 7;
    for (unsigned uu = 0; uu < len; ++uu) {
        result->push_back(bdlb::Random::generate15(&seed));
    }
}

}  // close namespace u
}  // close unnamed namespaceo

//=============================================================================
//                        GLOBAL CLASSES FOR TESTING
//-----------------------------------------------------------------------------

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char **argv)
{
    const int      test = argc > 1 ? atoi(argv[1]) : 0;
                verbose = argc > 2;
            veryVerbose = argc > 3;
        veryVeryVerbose = argc > 4; (void) veryVeryVerbose;
    veryVeryVeryVerbose = argc > 5; (void) veryVeryVeryVerbose;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    // CONCERN: 'BSLS_REVIEW' failures should lead to test failures.

    bsls::ReviewFailureHandlerGuard reviewGuard(&bsls::Review::failByAbort);

    switch (test) { case 0:
      case 6: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE 3 -- C++17
        //
        // Concern:
        //: 1 Demonstrate the usage of the component in C++17.
        //
        // Plan:
        //: 1 Call the 'PrintAdapter' constructor directly without explicitly
        //:   specifying the template parameter and taking advantage of CTAD.
        //:   fully-specified c'tor.
        //
        // Testing:
        //   USAGE EXAMPLE 3 -- C++17
        // --------------------------------------------------------------------

#ifdef BSLS_COMPILERFEATURES_SUPPORT_CTAD
    if (verbose) cout << "USAGE EXAMPLE 3 -- C++17\n"
                         "========================\n";

//
///Example 3: Use in C++17 With CTAD
///- - - - - - - - - - - - - - - - -
// Suppose you have a few 'bdlb::IdentSpan' objects that you want to print,
// only you want to stream them with 'operator<<' and you want to specify
// non-default values of 'level' and 'spacesPerLevel' to their streaming.
//..
    const bdlb::IndexSpan a(3, 7), b(241, 22), c(23, 17);
//..
// First, we create a line of dashes:
//..
    const char * const line = "------------------------------------"
                              "------------------------------------\n";
//..
// Now, you call the constructor 'PrintAdapter' on the 'IndexSpan' objects to
// stream and CTAD will create the right template specification:
//..
if (verbose)
    cout << "    a: " << line << bdlb::PrintAdapter(&a, 2, 2)
         << "    b: " << line << bdlb::PrintAdapter(&b, 3, 2)
         << "    c: " << line << bdlb::PrintAdapter(&c, 4, 2);
//..
// Finally, we see the output:
//..
//  a: ------------------------------------------------------------------------
//  [
//    position = 3
//    length = 7
//  ]
//  b: ------------------------------------------------------------------------
//    [
//      position = 241
//      length = 22
//    ]
//  c: ------------------------------------------------------------------------
//      [
//        position = 23
//        length = 17
//      ]
//..
#else
    if (verbose) cout << "USAGE EXAMPLE 3 *DISABLED* PRE-C++17\n"
                         "====================================\n";
#endif
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE 2 -- 'PrintAdapterUtil'
        //
        // Concern:
        //: 1 Demonstrate the use of 'PrintAdapterUtil::makeAdapter'.
        //
        // Plan:
        //: 2 Call 'bdlb::PrintAdapterUtil::makeAdapter' to print 3 objects.
        //
        // Testing:
        //   USAGE EXAMPLE 2 -- C++03
        // --------------------------------------------------------------------

    if (verbose) cout << "USAGE EXAMPLE 2 -- C++03\n"
                         "========================\n";

//
///Example 2: Use of 'PrintAdapterUtil::makeAdapter'
///- - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose we have a few 'bdlb::IdentSpan' objects that we want to print, only
// we want to stream them with 'operator<<' and we want to specify
// non-default values of 'level' and 'spacesPerLevel' to their streaming.
//..
    const bdlb::IndexSpan a(3, 7), b(241, 22), c(23, 17);
//..
// First, we make a typedef to the namespace 'struct':
    typedef bdlb::PrintAdapterUtil Util;
//..
// Then, we create a line of dashes:
//..
    const char * const line = "------------------------------------"
                              "------------------------------------\n";
//..
// Now, we call the static function 'PrintAdapterUtil::makeAdapter' on the
// 'IndexSpan' objects to stream which will return streamable 'PrintAdapter'
// objects:
//..
if (verbose)
    cout << "    a: " << line << Util::makeAdapter(a, 2, 2)
         << "    b: " << line << Util::makeAdapter(b, 3, 2)
         << "    c: " << line << Util::makeAdapter(c, 4, 2);
//..
// Finally, we see the output:
//..
//  a: ------------------------------------------------------------------------
//  [
//    position = 3
//    length = 7
//  ]
//  b: ------------------------------------------------------------------------
//    [
//      position = 241
//      length = 22
//    ]
//  c: ------------------------------------------------------------------------
//      [
//        position = 23
//        length = 17
//      ]
//..
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE 1 -- C++03
        //
        // Concern:
        //: 1 Demonstrate the usage of the component in C++03.
        //
        // Plan:
        //: 1 Create a 'typedef' of 'PrintAdapter<IndexSpan>' for calling the
        //:   fully-specified c'tor.
        //
        // Testing:
        //   USAGE EXAMPLE 1 -- C++03
        // --------------------------------------------------------------------

        if (verbose) cout << "USAGE EXAMPLE 1 -- C++03\n"
                             "========================\n";

///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Use in C++03
///- - - - - - - - - - - -
// Suppose we have a few 'bdlb::IdentSpan' objects that we want to print, only
// we want to stream them with 'operator<<' and we want to specify non-default
// values of 'level' and 'spacesPerLevel' to their streaming.
//..
    const bdlb::IndexSpan a(3, 7), b(241, 22), c(23, 17);
//..
// First, we create a typedef of 'PrintAdapter<IdentSpan>' to use:
//..
    typedef bdlb::PrintAdapter<bdlb::IndexSpan> PAIS;
//..
// Then, we create a line of dashes:
//..
    const char * const line = "------------------------------------"
                              "------------------------------------\n";
//..
// Now, we use the 'typedef' to construct temporary 'PrintAdapter' objects to
// stream our 'IndexSpan' objects:
//..
if (verbose)
    cout << "    a: " << line << PAIS(&a, 2, 2)
         << "    b: " << line << PAIS(&b, 3, 2)
         << "    c: " << line << PAIS(&c, 4, 2);
//..
// Finally, we see the output:
//..
//  a: ------------------------------------------------------------------------
//  [
//    position = 3
//    length = 7
//  ]
//  b: ------------------------------------------------------------------------
//    [
//      position = 241
//      length = 22
//    ]
//  c: ------------------------------------------------------------------------
//      [
//        position = 23
//        length = 17
//      ]
//..
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // CONSTRUCTOR, STREAM, and MAKEADAPTER -- NO PRINT
        //
        // Concerns:
        //: 1 That 'PrintAdapter' construction works properly.
        //:
        //: 2 That streaming works properly for types that do not have a
        //:   'print' method.
        //:
        //: 3 That 'makeAdapter' properly constructs 'PrintAdapter' objects.
        //
        // Plan:
        //: 1 For several types:
        //:   o Create an array of values of the type.
        //:
        //:   o Call 'u::testValues' on that array (see doc of 'u::testValues'
        //:     to see full description of that function).
        //:
        //: 2 Types testing include:
        //:   o 'u::NoPrintType': a simple struct with no 'print' method, but
        //:     for which 'operator<<' is defined.
        //:
        //:   o 'int'
        //:
        //:   o 'vector<int>', which has no 'print' method, but does have the
        //:     'HasStlIterators' trait defined, and contains 'int', for which
        //:     'operator<<' is defined.
        //
        // Testing:
        //   PrintAdapter (C++17)(...);
        //   PrintAdapter<TYPE>(...);
        //   PrintAdapterUtil::makeAdapter(...);
        //   stream& operator<<(stream&, const PrintAdapter&);
        // --------------------------------------------------------------------

        if (verbose) cout <<
                          "CONSTRUCTOR, STREAM, and MAKEADAPTER -- NO PRINT\n"
                          "================================================\n";

        {
            const u::NoPrintType noPrintValues[] = { u::NoPrintType(0),
                 u::NoPrintType(5), u::NoPrintType(-1), u::NoPrintType(1000) };
            enum { k_NUM_NO_PRINT_VALUES = sizeof noPrintValues /
                                                       sizeof *noPrintValues };
            u::testValues(noPrintValues, k_NUM_NO_PRINT_VALUES);
        }

        {
            const int ints[] = { 8, 2, 21, -71, 99, 512348 };
            enum { k_NUM_INTS = sizeof ints / sizeof *ints };
            u::testValues(ints, k_NUM_INTS);
        }

        {
            typedef bsl::vector<int> IntVec;

            enum { k_NUM_INT_VECS = 4 };
            bsl::vector<IntVec> intVecs(k_NUM_INT_VECS);
            for (int ii = 0; ii < k_NUM_INT_VECS; ++ii) {
                u::randIntVec(&intVecs[ii]);
            }

            u::testValues(intVecs.data(), k_NUM_INT_VECS);
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // CONSTRUCTOR, STREAM, and MAKEADAPTER -- HAS PRINT
        //
        // Concerns:
        //: 1 That 'PrintAdapter' construction works properly.
        //:
        //: 2 That streaming works properly for types that do not have a
        //:   'print' method.
        //:
        //: 3 That 'makeAdapter' properly constructs 'PrintAdapter' objects.
        //
        // Plan:
        //: 1 For several types:
        //:   o Create an array of values of the type.
        //:
        //:   o Call 'u::testValues' on that array (see doc of 'u::testValues'
        //:     to see full description of that function).
        //:
        //: 2 Types testing include:
        //:   o 'bdlb::IndexSpan', which has a 'print' method.
        //:
        //:   o 'u::HasPrintType': a simple struct with 'print' method, but for
        //:     which no 'operator<<' is defined.
        //
        // Testing:
        //   PrintAdapter (C++17)(...);
        //   PrintAdapter<TYPE>(...);
        //   PrintAdapterUtil::makeAdapter(...);
        //   stream& operator<<(stream&, const PrintAdapter&);
        // --------------------------------------------------------------------

        if (verbose) cout <<
                         "CONSTRUCTOR, STREAM, and MAKEADAPTER -- HAS PRINT\n"
                         "=================================================\n";

        {
            using bdlb::IndexSpan;

            const IndexSpan indexSpanValues[] = { IndexSpan(0, 0),
                      IndexSpan(10, 10), IndexSpan(20, 4), IndexSpan(12, 50) };
            enum { k_NUM_INDEX_SPAN_VALUES =
                            sizeof indexSpanValues / sizeof *indexSpanValues };
            u::testValues(indexSpanValues, k_NUM_INDEX_SPAN_VALUES);
        }

        {
            const u::HasPrintType hasPrintValues[] = { { 1, 2, 3, 4 },
                { 11, 22, 33, 44 }, { 0, 0, 0, 0 }, { -1, -2, -3, -4 } };
            enum { k_NUM_HAS_PRINT_VALUES = sizeof hasPrintValues /
                                                      sizeof *hasPrintValues };

            u::testValues(hasPrintValues, k_NUM_HAS_PRINT_VALUES);
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //   This case exercises (but does not fully test) basic functionality.
        //
        // Concerns:
        //: 1 Use the 'class' under test a couple of times, and verify that
        //:   the output is as expected.
        //
        // Plan:
        //: 1 Use 'bdlb::IndexSpan' as the type of object to be printed.
        //:
        //: 2 Call 'bdlb::IndexSpan::print' explicitly to get 'exp', the
        //:   expected output.
        //:
        //: 3 If we're on C++17, pass the 'IndexSpan' object to the
        //:   'PrintAdapter' constructor, without specifying the template
        //:   parameter, and stream the result.  Compare the result to 'exp'.
        //:
        //: 4 Pass the 'IndexSpan' object to the 'PrintAdapter<IndexSpan>'
        //:   constructor and stream the result.  Compare the result to 'exp'.
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) cout << "BREATHING TEST\n"
                             "==============\n";

        enum { k_LEVEL = 2, k_SPACESPERLEVEL = 5 };

        if (verbose) cout << "\n";
        {
            const bdlb::IndexSpan is(2, 3);
            bsl::ostringstream oss;

            is.print(oss, k_LEVEL, k_SPACESPERLEVEL);
            const bsl::string& exp = oss.str();
            if (veryVerbose) P(exp);

#ifdef BSLS_COMPILERFEATURES_SUPPORT_CTAD
            if (verbose) Q(CTAD);

            oss.str("");
            oss << bdlb::PrintAdapter(&is, k_LEVEL, k_SPACESPERLEVEL);
            const bsl::string& resultCtad = oss.str();

            ASSERT(exp == resultCtad);
#else
            if (verbose) Q(No CTAD);
#endif

            oss.str("");
            typedef bdlb::PrintAdapter<bdlb::IndexSpan> PA;
            oss << PA(&is, k_LEVEL, k_SPACESPERLEVEL);
            const bsl::string& resultAdapted = oss.str();
            if (veryVerbose) P(resultAdapted);

            ASSERTV(exp, resultAdapted, exp == resultAdapted);
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
