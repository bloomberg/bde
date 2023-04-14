// bslim_fuzzutil.t.cpp                                               -*-C++-*-

#include <bslim_fuzzdataview.h>
#include <bslim_fuzzutil.h>

#include <bslim_testutil.h>

#include <bslma_testallocator.h>

#include <bsls_assert.h>
#include <bsls_asserttest.h>
#include <bsls_timeutil.h>

#include <bsltf_templatetestfacility.h>

#include <bsl_algorithm.h> // bsl::generate_n
#include <bsl_cstdlib.h>   // bsl::rand()
#include <bsl_iostream.h>
#include <bsl_string.h>
#include <bsl_typeinfo.h>  // typeid().name()
#include <bsl_vector.h>

using namespace BloombergLP;
using bsl::cerr;
using bsl::cout;
using bsl::endl;
using bsl::flush;

//=============================================================================
//                              TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// 'bslim::FuzzUtil' provides a suite of functions for consuming basic types
// from fuzz data.  This test driver tests each implemented utility function
// independently.  Because the 'consumeNumber' and 'consumeNumberInRange' are
// function templates, they are tested for each possible type for which they
// might be instantiated.  Furthermore, because of the different boundary
// conditions -- among other considerations -- these two function families are
// tested separately for integral and floating point types.
//
//-----------------------------------------------------------------------------
// CLASS METHODS
//
// [ 5] bool consumeBool(FDV *);
// [ 4] TYPE consumeNumber<INTEGRAL>(FDV *);
// [ 3] TYPE consumeNumberInRange<INTEGRAL>(FDV *, min, max);
// [ 6] TYPE consumeNumber<FLOATING_POINT>(FDV *);
// [ 7] TYPE consumeNumberInRange<FLOATING_POINT>(FDV *, min, max);
// [ 8] void consumeRandomLengthString(bsl::string*,FDV*,maxLen);
// [ 8] void consumeRandomLengthString(std::string*,FDV*,maxLen);
// [ 8] void consumeRandomLengthString(std::pmr::string*,FDV*,maxLen);
// [ 9] void consumeRandomLengthChars(bsl::vector<char>*,FDV*,maxLen);
// [ 9] void consumeRandomLengthChars(std::vector<char>*,FDV*,maxLen);
// [ 9] void consumeRandomLengthChars(pmr::vector<char>*,FDV*,maxLen);
// ----------------------------------------------------------------------------
// [10] USAGE EXAMPLE
// [ 1] BREATHING TEST
// [ 2] void gg(vector<uint8_t> *buf, string_view& spec);
// [ 2] int ggg(vector<uint8_t> *buf, string_view& spec, bool vF = true);
// [-1] NEGATIVE ASAN TEST 'consumeRandomLengthChars'
// ----------------------------------------------------------------------------

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

#define ASSERT BSLIM_TESTUTIL_ASSERT
#define ASSERTV BSLIM_TESTUTIL_ASSERTV

#define LOOP_ASSERT BSLIM_TESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLIM_TESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLIM_TESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLIM_TESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLIM_TESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLIM_TESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLIM_TESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLIM_TESTUTIL_LOOP6_ASSERT

#define Q BSLIM_TESTUTIL_Q    // Quote identifier literally.
#define P BSLIM_TESTUTIL_P    // Print identifier and value.
#define P_ BSLIM_TESTUTIL_P_  // P(X) without '\n'.
#define T_ BSLIM_TESTUTIL_T_  // Print a tab (w/o newline).
#define L_ BSLIM_TESTUTIL_L_  // current Line number

#define RUN_EACH_TYPE BSLTF_TEMPLATETESTFACILITY_RUN_EACH_TYPE

// ============================================================================
//                  NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT_SAFE_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(EXPR)
#define ASSERT_SAFE_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(EXPR)
#define ASSERT_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_PASS(EXPR)
#define ASSERT_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_FAIL(EXPR)
#define ASSERT_OPT_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_OPT_PASS(EXPR)
#define ASSERT_OPT_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_OPT_FAIL(EXPR)

// ============================================================================
//         GLOBAL TYPEDEFS, CONSTANTS, ROUTINES & MACROS FOR TESTING
// ----------------------------------------------------------------------------

typedef bslim::FuzzUtil Util;

static int verbose;
static int veryVerbose;
static int veryVeryVerbose;
static int veryVeryVeryVerbose;

//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

void generateBytes(bsl::uint8_t *buffer,
                   bsl::size_t   bufLen,
                   bsl::size_t   numBytes)
    // Load into the specified 'buffer' of the specified 'bufLen' a total of
    // the specified 'numBytes' random bytes.  The behavior is undefined unless
    // 'numBytes <= bufLen'.
{
    ASSERT(buffer);
    ASSERT(numBytes <= bufLen);
    bsl::generate_n(buffer, numBytes, bsl::rand);
}

template <class INTEGRAL>
struct ConsumeIntegralTestCases
    // This 'struct' holds a single input for testing 'consumeNumberInRange'
    // for the (parameterized) 'INTEGRAL' type.
{
    int         d_line;             // line number
    bsl::size_t d_numBytes;         // passed to 'FuzzDataView' 'size'
    INTEGRAL    d_min;              // passed to function under test
    INTEGRAL    d_max;              // "      "     "      "
    bsl::size_t d_numBytesRequired; // number of bytes needed to produce a
                                    //   value within 'd_min' and 'd_max'
};

template <class INTEGRAL>
struct ConsumeIntegralTest {
    // This 'struct' is used by test cases for testing 'consumeNumber' and
    // 'consumeNumberInRange' for the (parameterized) 'INTEGRAL' type.
    static void testConsumeIntegral()
        // Perform test for (template type) INTEGRAL passed as an argument.
    {
        if (verbose)
            cout << endl
            << "<" << typeid(INTEGRAL).name() << ">" << endl
            << "=====" << endl;

        const int    BUFLEN = 128;
        bsl::uint8_t FUZZ_BUF[BUFLEN];

        for (bsl::size_t numBytes = 0; numBytes < sizeof(INTEGRAL) + 1;
             ++numBytes) {
            generateBytes(FUZZ_BUF, BUFLEN, numBytes);

            bslim::FuzzDataView fdv(FUZZ_BUF, numBytes);
            bsl::size_t         beforeLength = fdv.length();
            INTEGRAL            i = Util::consumeNumber<INTEGRAL>(&fdv);
            bsl::size_t         afterLength = fdv.length();

            if (numBytes < sizeof(INTEGRAL)) {
                ASSERT(numBytes == beforeLength - afterLength);
            }
            else {
                ASSERT(sizeof(INTEGRAL) == beforeLength - afterLength);
            }

            if (veryVerbose) {
                T_ P_(numBytes) P(((1 == sizeof(INTEGRAL)) ? (int)i : i));
            }

            if (0 == numBytes) {
                ASSERT(i == bsl::numeric_limits<INTEGRAL>::min());
            }
        }
    }

    static void testConsumeIntegralInRange(
                                  ConsumeIntegralTestCases<INTEGRAL> *DATA,
                                  int                                 NUM_DATA)
        // Perform test for (template type) INTEGRAL passed as an argument on
        // the specified 'DATA' of specified 'NUM_DATA' size.
    {
        const int    BUFLEN = 128;
        bsl::uint8_t FUZZ_BUF[BUFLEN];
        generateBytes(FUZZ_BUF, BUFLEN, BUFLEN);

        if (verbose)
            cout << endl
            << "<" << typeid(INTEGRAL).name() << ">" << endl
            << "=====" << endl;

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int           LINE      = DATA[ti].d_line;
            const size_t        NUM_BYTES = DATA[ti].d_numBytes;
            const size_t        BYTES_REQ = DATA[ti].d_numBytesRequired;

            bslim::FuzzDataView fdv(FUZZ_BUF, NUM_BYTES);
            bsl::size_t         beforeLength = fdv.length();

            INTEGRAL i = Util::consumeNumberInRange<INTEGRAL>(
                &fdv, DATA[ti].d_min, DATA[ti].d_max);

            bsl::size_t afterLength = fdv.length();

            if (veryVerbose) {
                T_ P_(
                LINE) P_(NUM_BYTES) P_(BYTES_REQ) P(i);
            }

            ASSERT(DATA[ti].d_max >= i);
            ASSERT(DATA[ti].d_min <= i);

            ASSERT(DATA[ti].d_numBytesRequired ==
                    beforeLength - afterLength);

            if (DATA[ti].d_numBytes == 0) {
                ASSERT(i == DATA[ti].d_min);
            }
        }
    }
};

//=============================================================================
//              GENERATOR FUNCTIONS 'gg' AND 'ggg' FOR TESTING
//-----------------------------------------------------------------------------
// The following functions interpret the given 'spec' in order from left to
// right to configure the buffer according to a custom language.  Digits 1-9
// correspond to the number of times to generate in the output the character
// that immediately follows the digit.  '|' corresponds to the backslash
// character.  '\' is forbidden.  Note that in order to embed '\0' in the spec,
// we employ a function, 'makeStringView', and use string literal
// concatenation.  This is in order to prevent the interpretation of it as an
// octal value.  So, we write 'makeStringView("1a2\0" "3t")' instead of
// 'makeStringView("1a2\03t")' (which does not work) or
// 'makeStringView("1a2\0003t")' (in which '\000' is octal Null).
//
// LANGUAGE SPECIFICATION: -----------------------
//
// <SPEC>       ::= <EMPTY> | <FIELD_LIST>
//
// <EMPTY>      ::=
//
// <FIELD_LIST> ::= <FIELD> | <FIELD> <FIELD_LIST>
//
// <FIELD>      ::= <NUMBER> <VALUE>
//                  // Represents a number/value pair
//
// <NUMBER>     ::= '1' .. '9'
//                  // The number of times to insert the character that
//                  // follows.
//
// <VALUE>      ::= <0x00 - 0x5B | 0x5D - 0xFF>
//                  // i.e., all characters, but note the following:
//                  // '|' represents the backslash character (i.e., 0x5C).
//                  // '\' is forbidden.
//-----------------------------------------------------------------------------
int ggg(bsl::vector<bsl::uint8_t> *buf,
        const bsl::string_view&    spec,
        bool                       verboseFlag = true)
    // Configure the specified 'buf' according to the specified 'spec'.
    // Optionally specify a 'false' 'verboseFlag' to suppress 'spec' syntax
    // error messages.  Return the index of the first invalid character, and a
    // negative value otherwise.  'buf->clear()' is called on entry.  Note that
    // this function is used to implement 'gg' as well as allow for
    // verification of syntax error detection.
{
    ASSERT(buf);
    buf->clear();

    enum { SUCCESS = -1 };
    bool ok = true;

    bsl::string_view::const_iterator it = spec.begin();

    while (it != spec.end()) {
        if (*it < '1' || *it > '9') {
            ok = false;
            break;
        }
        int numChars = static_cast<int>(*it++ - '0');
        if (it == spec.end() || *it == '\\') {
            ok = false;
            break;
        }
        bsl::uint8_t chr = static_cast<bsl::uint8_t>(*it);
        if (*it == '|') {
            chr = static_cast<bsl::uint8_t>('\\');
        }
        while (numChars-- > 0) {
            buf->push_back(chr);
        }
        it++;
    }
    if (!ok) {
        if (verboseFlag) {
            printf("Error, bad character (%c) "
                    "in spec (%s) at position %zd.\n",
                    *it, spec.data(), it - spec.begin());
        }
        return static_cast<int>(it - spec.begin());                   // RETURN
    }
    return SUCCESS;
}

void gg(bsl::vector<bsl::uint8_t> *buf,
        const bsl::string_view&    spec)
    // Load into the specified 'buf' an object with its value populated
    // according to the specified 'spec'.
{
    ASSERT(ggg(buf, spec, true) < 0);
}

template <int LENGTH>
bsl::string_view makeStringView(const char (&str)[LENGTH])
    // Return a 'bsl::string_view' based on the specified 'str' and (template
    // parameter) LENGTH.
{
    return bsl::string_view(str, LENGTH - 1);
}

// ============================================================================
//                               MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;

    verbose             = argc > 2;
    veryVerbose         = argc > 3;
    veryVeryVerbose     = argc > 4;
    veryVeryVeryVerbose = argc > 5;

    bsl::srand(static_cast<unsigned>(time(NULL)));
    uint8_t KB_DATA[1024];
    generateBytes(KB_DATA, 1024, 1024);

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) {
      case 0:  // Zero is always the leading case.
      case 10: {
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
        if (verbose)
            cout << endl << "USAGE EXAMPLE" << endl << "=============" << endl;

///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Consuming Integers in a Range to Pass to an Interface
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose we wish to fuzz test a function with preconditions.
//
// First, we define the 'TradingInterfaceUnderTest' 'struct':
//..
    struct TradingInterfaceUnderTest {
        // This utility class provides sample functionality to demonstrate how
        // fuzz data might be used.

        // CLASS METHODS
        static int numEarningsAnnouncements(int year, int month)
            // Return a value containing the number of earnings announcements
            // in the specified 'year' and 'month'.  The behavior is undefined
            // unless '1950 < year < 2030' and 'month' is in '[1 .. 12]'.  Note
            // that the values here are arbitrary, and in the real-world this
            // data would be obtained from a database or an API.
        {
            BSLS_ASSERT(1950 <  year  && year  < 2030);
            BSLS_ASSERT(   1 <= month && month <=  12);

            if (2020 < year && 6 < month) {
                return 11;                                            // RETURN
            }
            return 6;
        }
    };
//..
// Then, we need a block of raw bytes.  This would normally come from a fuzz
// harness (e.g., the 'LLVMFuzzerTestOneInput' entry point function from
// 'libFuzzer').  Since 'libFuzzer' is not available here, we initialize a
// 'myFuzzData' array that we will use instead.
//..
    const bsl::uint8_t  myFuzzData[] = {0x43, 0x19, 0x0D, 0x44, 0x37, 0x0D,
                                        0x38, 0x5E, 0x9B, 0xAA, 0xF3, 0xDA};
//..
// Next, we create a 'FuzzDataView' to wrap the raw bytes.
//..
    bslim::FuzzDataView fdv(myFuzzData, sizeof myFuzzData);
//..
// Now, we pass this 'FuzzDataView' to 'FuzzUtil' to generate values within the
// permissible range of the function under test:
//..
    int month = bslim::FuzzUtil::consumeNumberInRange<int>(&fdv,    1,   12);
    int year  = bslim::FuzzUtil::consumeNumberInRange<int>(&fdv, 1951, 2029);
    ASSERT(   1 <= month && month <=   12);
    ASSERT(1951 <= year  && year  <= 2029);
//..
// Finally, we can use these 'int' values to pass to a function that returns
// the number of earnings announcements scheduled in a given month.
//..
    int numEarnings =
        TradingInterfaceUnderTest::numEarningsAnnouncements(year, month);
    (void) numEarnings;
//..
      } break;
      case 9: {
        // --------------------------------------------------------------------
        // METHOD 'consumeRandomLengthChars'
        //
        // Concerns:
        //: 1 That if the 'fuzzDataView' has fewer bytes than 'maxLength', we
        //:   load at most 'fuzzDataView->length()' bytes into the 'output'.
        //:
        //: 2 That if the 'fuzzDataView' contains two successive backslash
        //:   characters, they are converted to a single backslash character
        //:   in the 'output'.
        //:
        //: 3 That if a single backslash character is encountered, the
        //:   following byte, if there is one, is consumed, and further
        //:   consumption is terminated.
        //:
        //: 4 That if 'consumeRandomLengthChars' is passed a 'maxLength'
        //:   argument that is less than the length of the 'fuzzDataView', only
        //:   a maximum of 'maxLength' bytes are returned.
        //:
        //: 5 That the function under test works properly for each of its
        //:   overloaded types (i.e., 'bsl::vector<char>', 'std::vector<char>',
        //:   and 'std::pmr::vector<char>').
        //
        // Plan:
        //: 1 Using a Generator Function with the table-driven technique:
        //:
        //:   1 Bring a 'fuzzDataView' into a state with a single byte
        //:     remaining, request more than a single byte, and verify that
        //:     a single byte was loaded into the 'output'.  (C-1)
        //:
        //:   2 Bring a 'fuzzDataView' into a state with two successive
        //:     backslash characters as its beginning, and verify that they are
        //:     converted into a single backslash in 'output'.  (C-2)
        //:
        //:   3 Bring a 'fuzzDataView' into a state with a single backslash
        //:     character at or near its beginning, and verify that when the
        //:     backslash is encountered, any following byte is consumed,
        //:     and further consumption of bytes terminates.  Also, test the
        //:     with a single backslash at the end.  (C-3)
        //:
        //:   4 Verify that invoking 'consumeRandomLengthChars' with a
        //:     'maxLength' less than the length of the fuzz buffer produces
        //:     a 'vector<char>' of no greater than 'maxLength'.  (C-4)
        //:
        //:   5 Construct three 'fuzzDataView' objects -- one each for
        //:     'bsl::vector<char>', 'std::vector<char>', and
        //:     'std::pmr::vector<char>' -- with the same underlying byte
        //:     array, and verify that the above concerns are addressed in each
        //:     case.  (C-5)
        //
        // Testing:
        //   void consumeRandomLengthChars(bsl::vector<char>*,FDV*,maxLen);
        //   void consumeRandomLengthChars(std::vector<char>*,FDV*,maxLen);
        //   void consumeRandomLengthChars(pmr::vector<char>*,FDV*,maxLen);
        // --------------------------------------------------------------------
        if (verbose)
            cout << endl
                 << "METHOD 'consumeRandomLengthChars'" << endl
                 << "=================================" << endl;

        static const struct {
            int               d_line;       // source line number
            bsl::string_view  d_spec;       // spec. for value
            int               d_maxLength;  // parameter to function call
            bsl::string_view  d_expSpec;    // the expected spec output
            bsl::size_t       d_consumed;   // bytes consumed from buffer

        } DATA[] = {
#define makeSV makeStringView
            //LINE       SPEC        MAXLENGTH     EXP_SPEC          CONSUMED
            //---- ---------------    --------     --------           ----
            {L_,   makeSV("6x"),         6,    makeSV("6x"),           6 },
            {L_,   makeSV("6x"),         4,    makeSV("4x"),           4 },
            {L_,   makeSV("6x"),         9,    makeSV("6x"),           6 },
            {L_,   makeSV("1|1x"),       6,    makeSV(""),             2 },
            {L_,   makeSV("1x1|1y"),     3,    makeSV("1x"),           3 },
            {L_,   makeSV("4x1|"),       5,    makeSV("4x"),           5 },
            {L_,   makeSV("2|4x2|"),     6,    makeSV("1|4x1|"),       8 },
            {L_,   makeSV("2|4x2|4y2|"), 20,   makeSV("1|4x1|4y1|"),  14 },
            {L_,   makeSV("99"),         10,   makeSV("99"),           9 },
            {L_,   makeSV("4&"),         4,    makeSV("4&"),           4 },
            {L_,   makeSV("1\0"),        5,    makeSV("1\0"),          1 },
            {L_,   makeSV("1a2\0" "3t"), 6,    makeSV("1a2\0" "3t"),   6 }
#undef makeSV
        };

        enum { NUM_DATA = sizeof DATA / sizeof *DATA };

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE      = DATA[ti].d_line;
            bsl::string_view  SPEC      = DATA[ti].d_spec;
            const int         MAXLENGTH = DATA[ti].d_maxLength;
            bsl::string_view  EXP_SPEC  = DATA[ti].d_expSpec;
            const bsl::size_t CONSUMED  = DATA[ti].d_consumed;

            bsl::vector<bsl::uint8_t> fuzzData;
            bsl::vector<bsl::uint8_t> expected;

            gg(&fuzzData, SPEC);
            gg(&expected, EXP_SPEC);

            // 3 blocks to test the 3 overloads (C-5)
            {
                bslim::FuzzDataView fdvBSL(fuzzData.data(), fuzzData.size());
                bsl::vector<char>   result;
                Util::consumeRandomLengthChars(&result, &fdvBSL, MAXLENGTH);

                if (veryVerbose) {
                    T_ P_(LINE) P_(SPEC) P(result.size());
                }

                ASSERT(expected.size() == result.size());

                ASSERT(
                   bsl::equal(result.begin(), result.end(), expected.begin()));
                ASSERT(CONSUMED == fuzzData.size() - fdvBSL.length());
            }
            {
                bslim::FuzzDataView fdvSTD(fuzzData.data(), fuzzData.size());
                std::vector<char>   result;
                Util::consumeRandomLengthChars(&result, &fdvSTD, MAXLENGTH);

                if (veryVerbose) {
                    T_ P_(LINE) P_(SPEC) P(result.size());
                }

                ASSERT(expected.size() == result.size());
                ASSERT(
                   bsl::equal(result.begin(), result.end(), expected.begin()));
                ASSERT(CONSUMED == fuzzData.size() - fdvSTD.length());
            }
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
            {
                bslim::FuzzDataView fdvPMR(fuzzData.data(), fuzzData.size());

                std::pmr::vector<char> result;
                Util::consumeRandomLengthChars(&result, &fdvPMR, MAXLENGTH);

                if (veryVerbose) {
                    T_ P_(LINE) P_(SPEC) P(result.size());
                }

                ASSERT(expected.size() == result.size());
                ASSERT(bsl::equal(
                    result.begin(), result.end(), expected.begin()));
                ASSERT(CONSUMED == fuzzData.size() - fdvPMR.length());
            }
#endif
        }
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // METHOD 'consumeRandomLengthString'
        //
        // Concerns:
        //: 1 That if the 'fuzzDataView' has fewer bytes than 'maxLength', we
        //:   load at most 'fuzzDataView->length()' bytes into the 'output'.
        //:
        //: 2 That if the 'fuzzDataView' contains two successive backslash
        //:   characters, they are converted to a single backslash character
        //:   in the 'output'.
        //:
        //: 3 That if a single backslash character is encountered, the
        //:   following byte, if there is one, is consumed, and further
        //:   consumption is terminated.
        //:
        //: 4 That if 'consumeRandomLengthString' is passed a 'maxLength'
        //:   argument that is less than the length of the 'fuzzDataView', only
        //:   a maximum of 'maxLength' bytes are returned.
        //:
        //: 5 That the function under test works properly for each of its
        //:   overloaded types (i.e., 'bsl::string', 'std::string', and
        //:   'std::pmr::string').
        //
        // Plan:
        //: 1 Using a Generator Function with the table-driven technique:
        //:
        //:   1 Bring a 'fuzzDataView' into a state with a single byte
        //:     remaining, request more than a single byte, and verify that
        //:     a single byte was loaded into the 'output'.  (C-1)
        //:
        //:   2 Bring a 'fuzzDataView' into a state with two successive
        //:     backslash characters as its beginning, and verify that they are
        //:     converted into a single backslash in 'output'.  (C-2)
        //:
        //:   3 Bring a 'fuzzDataView' into a state with a single backslash
        //:     character at or near its beginning, and verify that when the
        //:     backslash is encountered, any following byte is consumed,
        //:     and further consumption of bytes terminates.  Also, test the
        //:     with a single backslash at the end.  (C-3)
        //:
        //:   4 Verify that invoking 'consumeRandomLengthString' with a
        //:     'maxLength' less than the length of the fuzz buffer produces
        //:     a string of no greater than 'maxLength'.  (C-4)
        //:
        //:   5 Construct three 'fuzzDataView' objects -- one each for
        //:     'bsl::string', 'std::string', and 'std::pmr::string' -- with
        //:     the same underlying byte array, and verify that the above
        //:     concerns are addressed in each case.  (C-5)
        //
        // Testing:
        //   void consumeRandomLengthString(bsl::string*,FDV*,maxLen);
        //   void consumeRandomLengthString(std::string*,FDV*,maxLen);
        //   void consumeRandomLengthString(std::pmr::string*,FDV*,maxLen);
        // --------------------------------------------------------------------
        if (verbose)
            cout << endl
                 << "METHOD 'consumeRandomLengthString'" << endl
                 << "==================================" << endl;

        static const struct {
            int               d_line;       // source line number
            bsl::string_view  d_spec;       // spec. for value
            int               d_maxLength;  // parameter to function call
            bsl::string_view  d_expSpec;    // the expected spec output
            bsl::size_t       d_consumed;   // bytes consumed from buffer

        } DATA[] = {
#define makeSV makeStringView
            //LINE       SPEC        MAXLENGTH     EXP_SPEC          CONSUMED
            //---- ---------------    --------     --------           ----
            {L_,   makeSV("6x"),         6,    makeSV("6x"),           6 },
            {L_,   makeSV("6x"),         4,    makeSV("4x"),           4 },
            {L_,   makeSV("6x"),         9,    makeSV("6x"),           6 },
            {L_,   makeSV("1|1x"),       6,    makeSV(""),             2 },
            {L_,   makeSV("1x1|1y"),     3,    makeSV("1x"),           3 },
            {L_,   makeSV("4x1|"),       5,    makeSV("4x"),           5 },
            {L_,   makeSV("2|4x2|"),     6,    makeSV("1|4x1|"),       8 },
            {L_,   makeSV("2|4x2|4y2|"), 20,   makeSV("1|4x1|4y1|"),  14 },
            {L_,   makeSV("99"),         10,   makeSV("99"),           9 },
            {L_,   makeSV("4&"),         4,    makeSV("4&"),           4 },
            {L_,   makeSV("1\0"),        5,    makeSV("1\0"),          1 },
            {L_,   makeSV("1a2\0" "3t"), 6,    makeSV("1a2\0" "3t"),   6 }
#undef makeSV
        };

        enum { NUM_DATA = sizeof DATA / sizeof *DATA };

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE      = DATA[ti].d_line;
            bsl::string_view  SPEC      = DATA[ti].d_spec;
            const int         MAXLENGTH = DATA[ti].d_maxLength;
            bsl::string_view  EXP_SPEC  = DATA[ti].d_expSpec;
            const bsl::size_t CONSUMED  = DATA[ti].d_consumed;

            bsl::vector<bsl::uint8_t> fuzzData;
            bsl::vector<bsl::uint8_t> expected;

            gg(&fuzzData, SPEC);
            gg(&expected, EXP_SPEC);

            // 3 blocks to test the 3 overloads (C-5)
            {
                bslim::FuzzDataView fdvBSL(fuzzData.data(), fuzzData.size());
                bsl::string         result;
                Util::consumeRandomLengthString(&result, &fdvBSL, MAXLENGTH);

                if (veryVerbose) {
                    T_ P_(LINE) P_(SPEC) P_(result) P(result.length());
                }

                ASSERT(expected.size() == result.length());

                ASSERT(
                   bsl::equal(result.begin(), result.end(), expected.begin()));
                ASSERT(CONSUMED == fuzzData.size() - fdvBSL.length());
            }
            {
                bslim::FuzzDataView fdvSTD(fuzzData.data(), fuzzData.size());
                std::string         result;
                Util::consumeRandomLengthString(&result, &fdvSTD, MAXLENGTH);

                if (veryVerbose) {
                    T_ P_(LINE) P_(SPEC) P_(result) P(result.length());
                }

                ASSERT(expected.size() == result.length());
                ASSERT(
                   bsl::equal(result.begin(), result.end(), expected.begin()));
                ASSERT(CONSUMED == fuzzData.size() - fdvSTD.length());
            }
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
            {
                bslim::FuzzDataView fdvPMR(fuzzData.data(), fuzzData.size());
                std::pmr::string    result;
                Util::consumeRandomLengthString(&result, &fdvPMR, MAXLENGTH);

                if (veryVerbose) {
                    T_ P_(LINE) P_(SPEC) P_(result) P(result.length());
                }

                ASSERT(expected.size() == result.length());
                ASSERT(bsl::equal(
                    result.begin(), result.end(), expected.begin()));
                ASSERT(CONSUMED == fuzzData.size() - fdvPMR.length());
            }
#endif
        }
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // METHOD 'consumeNumberInRange<FLOATING_POINT>'
        //
        // Concerns:
        //: 1 The value returned is within the specified range.
        //:
        //: 2 The method under test works for different floating point types
        //:   (i.e., 'float', 'double').
        //:
        //: 3 If 0 bytes are available in the 'FuzzDataView', 'min' is
        //:   returned.
        //:
        //: 4 The number of bytes consumed is at most '1 + sizeof(TYPE)'.
        //:
        //: 5 The values returned are distributed uniformly within the range.
        //
        // Plan:
        //: 1 Using the table-driven approach, define a representative set of
        //:   valid inputs.  Verify that the function returns values within
        //:   the specified range.  (C-1).
        //:
        //: 2 Use the above approach for 'double' and 'float'.  (C-2)
        //:
        //: 3 Verify that, if there are 0 bytes to consume, the value returned
        //:   is 'min'.  (C-3)
        //:
        //: 4 Check that the number of bytes consumed is what is expected.
        //:   (C-4)
        //:
        //: 5 Perform a basic statistical test on the output values to
        //:   determine if they are distributed uniformly. The results will
        //:   vary depending upon the sample size and the actual fuzz data
        //:   used.  (C-5)
        //
        // Testing:
        //   TYPE consumeNumberInRange<FLOATING_POINT>(FDV *, min, max);
        // --------------------------------------------------------------------

        if (verbose)
            cout << endl
                 << "METHOD 'consumeNumberInRange<FLOATING_POINT>'" << endl
                 << "=============================================" << endl;

        {
            const double LOW_DOUBLE  = -bsl::numeric_limits<double>::max();
            const double HIGH_DOUBLE = bsl::numeric_limits<double>::max();
            const double DENORM_MIN  =
                bsl::numeric_limits<double>::denorm_min();

            static const struct {
                int                 d_line;      // source line number
                bsl::size_t         d_numBytes;  // number of bytes passed
                                                 //   to the 'FuzzDataView'
                double              d_min;       // 'min' method argument
                double              d_max;       // 'max' method argument
                size_t              d_expBytes;  // num expected bytes consumed
            } DATA[] = {
                //LINE     NUM_BYTES     MIN             MAX   EXP
                 //----    ---------     ---            ---   ---
                {L_,          0,         0,              0,    0},
                {L_,          0,         0,     DENORM_MIN,    0},
                {L_,          1,     LOW_DOUBLE,         0,    1},
                {L_,          2,         0,    HIGH_DOUBLE,    2},
                {L_,          12,      -1e307, HIGH_DOUBLE,    9},
                {L_,          4,       -5.32,            0,    4},
                {L_,          8,         0,          55.32,    8},
                {L_,          8,        100,           100,    8},
                {L_,          12, LOW_DOUBLE,  HIGH_DOUBLE,    9}};

            enum { NUM_DATA = sizeof DATA / sizeof *DATA };

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int           LINE      = DATA[ti].d_line;
                const bsl::size_t   NUM_BYTES = DATA[ti].d_numBytes;
                const bsl::size_t   EXP_BYTES = DATA[ti].d_expBytes;

                bslim::FuzzDataView fdv(KB_DATA + ti, NUM_BYTES);
                bsl::size_t         beforeLength = fdv.length();
                double d = Util::consumeNumberInRange<double>(
                    &fdv, DATA[ti].d_min, DATA[ti].d_max);
                bsl::size_t         afterLength = fdv.length();

                ASSERT(EXP_BYTES == beforeLength - afterLength);

                if (veryVerbose) {
                    T_ P_(LINE) P_(NUM_BYTES) P(d);
                }

                ASSERT(DATA[ti].d_max >= d);
                ASSERT(DATA[ti].d_min <= d);

                if (DATA[ti].d_numBytes == 0) {
                    ASSERT(d == DATA[ti].d_min);
                }
            }
        }
        {
            const float LOW_FLOAT  = -bsl::numeric_limits<float>::max();
            const float HIGH_FLOAT = bsl::numeric_limits<float>::max();
            const float DENORM_MIN  = bsl::numeric_limits<float>::denorm_min();

            static const struct {
                int                 d_line;      // source line number
                size_t              d_numBytes;  // number of bytes passed
                                                 //   to the 'FuzzDataView'
                float               d_min;       // 'min' method argument
                float               d_max;       // 'max' method argument
            } DATA[] = {//LINE     NUM_BYTES     MIN             MAX
                        //----     ---------     ---             ---
                        {L_,          0,         0,              0},
                        {L_,          0,         0,     DENORM_MIN},
                        {L_,          1,     LOW_FLOAT,          0},
                        {L_,          2,         0,     HIGH_FLOAT},
                        {L_,          4,       -5.32f,           0},
                        {L_,          8,         0,         55.32f},
                        {L_,          8,        100,           100},
                        {L_,          12,    LOW_FLOAT, HIGH_FLOAT}};

            enum { NUM_DATA = sizeof DATA / sizeof *DATA };

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int           LINE      = DATA[ti].d_line;
                const bsl::size_t   NUM_BYTES = DATA[ti].d_numBytes;

                bslim::FuzzDataView fdv(KB_DATA + ti, NUM_BYTES);
                float d = Util::consumeNumberInRange<float>(
                    &fdv, DATA[ti].d_min, DATA[ti].d_max);

                if (veryVerbose) {
                    T_ P_(LINE) P_(NUM_BYTES) P(d);
                }

                ASSERT(DATA[ti].d_max >= d);
                ASSERT(DATA[ti].d_min <= d);

                if (DATA[ti].d_numBytes == 0) {
                    ASSERT(d == DATA[ti].d_min);
                }
            }
        }
        {
            bslim::FuzzDataView fdv(KB_DATA, sizeof(KB_DATA));

            double    mean      = 0.0;
            double    variance  = 0.0;
            const int NUM_TIMES = 128;
            for (int i = 0; i < NUM_TIMES; i++) {
                double d =
                    Util::consumeNumberInRange<double>(&fdv, 0.0, 1.0);
                mean += d;
                variance += d * d;
                if (veryVeryVerbose) {
                    cout << "\tdouble d = " << d << endl;
                }
            }
            mean /= NUM_TIMES;
            variance = (variance - mean * mean * NUM_TIMES) / NUM_TIMES;
            if (veryVerbose) {
                // If uniformly distributed, 'mean' should be ~.5 and
                // 'variance' should be ~.083.
                T_ P_(mean) P_(variance) P(NUM_TIMES);
            }
        }
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // METHOD 'consumeNumber<FLOATING_POINT>'
        //
        // Concerns:
        //: 1 The method under test works for different floating point types
        //:   (i.e., 'double', 'float').
        //:
        //: 2 If 0 bytes are available in the 'FuzzDataView',
        //:   '-bsl::numeric_limits<TYPE>::max()' returned.
        //:
        //: 3 The number of bytes consumed is <= the size of the view.
        //:
        //: 4 The number of bytes consumed is at most '1 + sizeof(TYPE)'.
        //
        // Plan:
        //: 1 Generate 1024 random bytes in a buffer. Using the random data,
        //:   consume an integer between 0 .. 15 (for 'double') and 0 .. 7 (for
        //:   'float') and to pass as the length of the view. This enables
        //:   testing of proper behavior when fewer or greater bytes are
        //:   available.  (C-1)
        //:
        //: 2 If there are 0 bytes to consume, verify tha the value returned
        //:   is '-bsl::numeric_limits<TYPE>::max()'.  (C-2)
        //:
        //: 3 Check that the number of bytes consumed is what is expected.
        //    (C-3, C-4)
        //
        // Testing:
        //   TYPE consumeNumber<FLOATING_POINT>(FDV *);
        // --------------------------------------------------------------------

        if (verbose)
            cout << endl
                 << "METHOD 'consumeNumber<FLOATING_POINT>'" << endl
                 << "======================================" << endl;
        {
            // Double
            const bsl::size_t NUM_ITERATIONS = 64;
            const bsl::size_t DOUBLE_SIZE_REQ = 9;

            for (bsl::size_t ti = 0; ti < NUM_ITERATIONS; ++ti) {
                bslim::FuzzDataView fdv_numBytes(KB_DATA + ti, 1);
                bsl::size_t         numBytes =
                    Util::consumeNumberInRange<bsl::size_t>(
                        &fdv_numBytes, 0, 15);
                bsl::size_t offset = NUM_ITERATIONS + ti * 15;
                bslim::FuzzDataView fdv_double(KB_DATA + offset, numBytes);
                ASSERT(fdv_double.length() == numBytes);
                double d = Util::consumeNumber<double>(&fdv_double);
                if (veryVerbose) {
                    T_ P_(numBytes) P(d);
                }
                if (0 == numBytes) {
                    ASSERT(0 == fdv_double.length());
                    ASSERT(-bsl::numeric_limits<double>::max() == d);
                }
                else {
                    bsl::size_t numBytesConsumed =
                        numBytes - fdv_double.length();
                    ASSERT(bsl::min(DOUBLE_SIZE_REQ, numBytes) ==
                           numBytesConsumed);
                }
            }
        }
        {
            // Float
            const bsl::size_t NUM_ITERATIONS = 128;
            const bsl::size_t FLOAT_SIZE_REQ = 5;

            for (bsl::size_t ti = 0; ti < NUM_ITERATIONS; ++ti) {
                bslim::FuzzDataView fdv_numBytes(KB_DATA + ti, 1);
                bsl::size_t         numBytes =
                    Util::consumeNumberInRange<bsl::size_t>(
                        &fdv_numBytes, 0, 7);
                bsl::size_t         offset = NUM_ITERATIONS + ti * 7;
                bslim::FuzzDataView fdv_float(KB_DATA + offset, numBytes);
                ASSERT(fdv_float.length() == numBytes);
                float f = Util::consumeNumber<float>(&fdv_float);
                if (veryVerbose) {
                    T_ P_(numBytes) P(f);
                }
                if (0 == numBytes) {
                    ASSERT(0 == fdv_float.length());
                    ASSERT(-bsl::numeric_limits<float>::max() == f);
                }
                else {
                    bsl::size_t numBytesConsumed =
                        numBytes - fdv_float.length();
                    ASSERT(bsl::min(FLOAT_SIZE_REQ, numBytes) ==
                           numBytesConsumed);
                }
            }
        }
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // METHOD 'consumeBool'
        //
        // Concerns:
        //: 1 The method under test returns the expected value.  (C-1)
        //:
        //: 2 If 'FuzzDataView' is not empty, exactly one byte is consumed.
        //:   (C-2)
        //:
        //: 3 If the 'FuzzDataView' is empty, the method returns 'false'.
        //:    (C-3)
        //
        // Plan:
        //: 1 Using the table-driven approach, define a set of inputs and
        //:   verify that the function returns the expected values.
        //:   (C-1).
        //:
        //: 2 Verify that the number of bytes in the view decreases by exactly
        //:   one after invoking the method under test.  (C-2)
        //:
        //: 3 Verify that when a 'FuzzDataView' with 'length() ==0' is passed
        //:   to the method that the resulting value is 'false'.
        //:   (C-3)
        //
        // Testing:
        //   bool consumeBool(FDV *);
        // --------------------------------------------------------------------

        if (verbose)
            cout << endl
                 << "METHOD 'consumeBool'" << endl
                 << "====================" << endl;

        const bsl::size_t NUM_ITERATIONS = 256;
        for (bsl::size_t ti = 0; ti < NUM_ITERATIONS; ++ti) {
            bslim::FuzzDataView fdv_numBytes(KB_DATA + ti, 1);
            bsl::size_t         numBytes =
                Util::consumeNumberInRange<bsl::size_t>(&fdv_numBytes, 0, 2);
            bsl::size_t         offset = NUM_ITERATIONS + ti * 2;
            bslim::FuzzDataView fdv_bool(KB_DATA + offset, numBytes);
            ASSERT(fdv_bool.length() == numBytes);
            bool b = Util::consumeBool(&fdv_bool);
            if (veryVerbose) {
                T_ P_(numBytes) P(b);
            }
            if (0 == numBytes) {
                ASSERT(0 == fdv_bool.length());  // C-2
                ASSERT(false == b);
            }
            else {
                ASSERT(fdv_bool.length() == --numBytes);  // C-2
                ASSERT(b == (KB_DATA[offset] & 1));       // C-1
            }
        }
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // METHOD 'consumeNumber<INTEGRAL>'
        //
        // Concerns:
        //: 1 The method under test works for all integral types.
        //:
        //: 2 The method under test consumes the appropriate number of bytes
        //:   for any type (e.g., for type 'int', the method consumes 4 bytes;
        //:   'char', the method consumes 1 byte, etc.).
        //
        // Plan:
        //: 1 Perform the tests for all integral types.  (C-1)
        //:
        //: 2 Verify that, when consuming an 'int', 4 bytes are consumed; and
        //:   for a 'char', 1 byte is consumed, and so on.  (C-2)
        //
        // Testing:
        //   TYPE consumeNumber<INTEGRAL>(FDV *);
        // --------------------------------------------------------------------
        if (verbose)
            cout << endl
                 << "METHOD 'consumeNumber<INTEGRAL>'" << endl
                 << "================================" << endl;

        RUN_EACH_TYPE(ConsumeIntegralTest,
                      testConsumeIntegral,
                      int,
                      unsigned int,
                      short,
                      unsigned short,
                      long,
                      unsigned long,
                      bsls::Types::Int64,
                      bsls::Types::Uint64,
                      char,
                      signed char,
                      unsigned char);
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // METHOD 'consumeNumberInRange<INTEGRAL>'
        //
        // Concerns:
        //: 1 The value returned is within the specified range.
        //:
        //: 2 The method under test works for all integral types (N.B. it is
        //:   explicitely disallowed for 'bool').
        //:
        //: 3 If 0 bytes are available in the 'FuzzDataView', 'min' is
        //:   returned.
        //:
        //: 4 Only the number of bytes necessary to produce a value in the
        //:   range are consumed.
        //
        // Plan:
        //: 1 Using the table-driven approach, define a representative set of
        //:   valid inputs.  Then, using a templated function,
        //:   'ConsumeIntegralTest<INTEGRAL>::testConsumeIntegralInRange',
        //:   verify that the function under test returns values within the
        //:   specified range for each integral type.  (C-1).
        //:
        //: 2 Perform the tests for all integral types.  (C-2)
        //:
        //: 3 Verify that, if there are 0 bytes to consume, the value returned
        //:   is 'min'.  (C-3)
        //:
        //: 4 Verify that, only enough bytes to produce a value within the
        //:   range are consumed.  (C-4)
        //
        // Testing:
        //   TYPE consumeNumberInRange<INTEGRAL>(FDV *, min, max);
        // --------------------------------------------------------------------

        if (verbose)
            cout << endl
                 << "METHOD 'consumeNumberInRange<INTEGRAL>'" << endl
                 << "=======================================" << endl;
        {
            if (verbose) cout << endl << "<int>" << endl;

            int MIN_INT = bsl::numeric_limits<int>::min();
            int MAX_INT = bsl::numeric_limits<int>::max();
            ConsumeIntegralTestCases<int>
            DATA_INT[] = {  //LINE   LENGTH     MIN     MAX       BYTES
                            //----  -------     ---     ---      ------
                            {L_,       0,        0,      0,          0},
                            {L_,       1,        0,      0,          0},
                            {L_,       2,        0,      0,          0},
                            {L_,       8,       100,    100,         0},
                            {L_,       4,      -100,     0,          1},
                            {L_,       8,        0,     100,         1},
                            {L_,       12,     -256,    256,         2},
                            {L_,       12,    -32000,  32000,        2},
                            {L_,       12, MIN_INT, MIN_INT + 10000, 2},
                            {L_,       12, MAX_INT - 10000, MAX_INT, 2},
                            {L_,       12,    -100000, 100000,       3},
                            {L_,       12,       0,    MAX_INT,      4},
                            {L_,       12,    MIN_INT,   0,          4},
                            {L_,       12,    MIN_INT, MAX_INT,      4}};
            enum { NUM_DATA = sizeof DATA_INT / sizeof *DATA_INT };
            ConsumeIntegralTest<int>::testConsumeIntegralInRange(DATA_INT,
                                                                 NUM_DATA);
        }
        {
            if (verbose) cout << endl << "<unsigned int>" << endl;

            unsigned int MIN_UINT = bsl::numeric_limits<unsigned int>::min();
            unsigned int MAX_UINT = bsl::numeric_limits<unsigned int>::max();
            ConsumeIntegralTestCases<unsigned int>
            DATA_UINT[] = {  //LINE   LENGTH     MIN     MAX       BYTES
                            //----  -------     ---     ---      ------
                            {L_,       0,        0,      0,          0},
                            {L_,       1,        0,      0,          0},
                            {L_,       2,        0,      0,          0},
                            {L_,       8,       100,    100,         0},
                            {L_,       8,        0,     100,         1},
                            {L_,       12,       0,    65535,        2},
                            {L_,       12, MIN_UINT, MIN_UINT + 10000, 2},
                            {L_,       12, MAX_UINT - 10000, MAX_UINT, 2},
                            {L_,       12,       0,   200000,        3},
                            {L_,       12,       0,    MAX_UINT,     4},
                            {L_,       12,    MIN_UINT,   0,         0},
                            {L_,       12,    MIN_UINT, MAX_UINT,    4}};
            enum { NUM_DATA = sizeof DATA_UINT / sizeof *DATA_UINT };
            ConsumeIntegralTest<unsigned int>::testConsumeIntegralInRange(
                                                                     DATA_UINT,
                                                                     NUM_DATA);
        }
        {
            if (verbose) cout << endl << "<short>" << endl;

            short MIN_SHRT = bsl::numeric_limits<short>::min();
            short MAX_SHRT = bsl::numeric_limits<short>::max();
            ConsumeIntegralTestCases<short>
            DATA_SHRT[] = {
            //LINE   LENGTH     MIN     MAX       BYTES
            //----  -------     ---     ---      ------
            {L_,       0,        0,      0,          0},
            {L_,       1,        0,      0,          0},
            {L_,       2,        0,      0,          0},
            {L_,       8,       100,    100,         0},
            {L_,       4,      -100,     0,          1},
            {L_,       8,        0,     100,         1},
            {L_,       12,     -256,    256,         2},
            {L_,       12,    -32000,  32000,        2},
            {L_,       12, MIN_SHRT, static_cast<short>(MIN_SHRT + 10000), 2},
            {L_,       12, static_cast<short>(MAX_SHRT - 10000), MAX_SHRT, 2},
            {L_,       12,       0,    MAX_SHRT,     2},
            {L_,       12,    MIN_SHRT,   0,         2},
            {L_,       12,    MIN_SHRT, MAX_SHRT,    2}};
            enum { NUM_DATA = sizeof DATA_SHRT / sizeof *DATA_SHRT };
            ConsumeIntegralTest<short>::testConsumeIntegralInRange(DATA_SHRT,
                                                                   NUM_DATA);
        }
        {
            if (verbose)
                cout << endl << "<unsigned short>" << endl;

            unsigned short MIN_USHRT =
                bsl::numeric_limits<unsigned short>::min();
            unsigned short MAX_USHRT =
                bsl::numeric_limits<unsigned short>::max();
            ConsumeIntegralTestCases<unsigned short> DATA_USHRT[] = {
            //LINE   LENGTH     MIN     MAX       BYTES
            //----  -------     ---     ---      ------
            {L_,       0,        0,      0,        0},
            {L_,       1,        0,      0,        0},
            {L_,       2,        0,      0,        0},
            {L_,       8,      100,    100,        0},
            {L_,       4,        0,    200,        1},
            {L_,       12,       0,    512,        2},
            {L_,       12,       0,  65535,        2},
            {L_,  12, MIN_USHRT,
                            static_cast<unsigned short>(MIN_USHRT + 10000), 2},
            {L_,  12, static_cast<unsigned short>(MAX_USHRT - 10000),
                                                   MAX_USHRT, 2},
            {L_,       12,       0, MAX_USHRT,     2},
            {L_,       12, MIN_USHRT,    0,        0},
            {L_,       12, MIN_USHRT, MAX_USHRT,   2}};
            enum { NUM_DATA = sizeof DATA_USHRT / sizeof *DATA_USHRT };
            ConsumeIntegralTest<unsigned short>::testConsumeIntegralInRange(
                                                                   DATA_USHRT,
                                                                   NUM_DATA);
        }
        {
            if (verbose)
                cout << endl << "<long>" << endl;

            const long MAX_LONG = bsl::numeric_limits<long>::max();
            const long MIN_LONG = bsl::numeric_limits<long>::min();
            bsl::size_t sizeOfLong = sizeof(long);
            ConsumeIntegralTestCases<long>
            DATA_LONG[] = { //LINE   LENGTH     MIN     MAX      BYTES
                            //----  -------     ---     ---      ------
                            {L_,       0,        0,      0,       0},
                            {L_,       1,        0,      0,       0},
                            {L_,       2,        0,      0,       0},
                            {L_,       8,      100,    100,       0},
                            {L_,       4,     -100,      0,       1},
                            {L_,       8,        0,    100,       1},
                            {L_,      12,      -44,     80,       1},
                            {L_,      12,     -256,    256,       2},
                            {L_,      12,    -32000,  32000,      2},
                            {L_,      12, MIN_LONG, MIN_LONG + 10000, 2},
                            {L_,      12, MAX_LONG - 10000, MAX_LONG, 2},
                            {L_,      12,    -100000, 100000,     3},
                            {L_,      12,       0,    MAX_LONG,   sizeOfLong},
                            {L_,      12,  MIN_LONG,   0,         sizeOfLong},
                            {L_,      12,  MIN_LONG, MAX_LONG,    sizeOfLong}};
            enum { NUM_DATA = sizeof DATA_LONG / sizeof *DATA_LONG };
            ConsumeIntegralTest<long>::testConsumeIntegralInRange(DATA_LONG,
                                                                  NUM_DATA);
        }
        {
            if (verbose)
                cout << endl << "<unsigned long>" << endl;

            const unsigned long MAX_ULONG =
                    bsl::numeric_limits<unsigned long>::max();
            const unsigned long MIN_ULONG =
                    bsl::numeric_limits<unsigned long>::min();
            bsl::size_t sizeOfULong = sizeof(unsigned long);
            ConsumeIntegralTestCases<unsigned long>
            DATA_ULONG[] = {//LINE   LENGTH     MIN     MAX     BYTES
                            //----  -------     ---     ---      ------
                            {L_,       0,        0,      0,       0},
                            {L_,       1,        0,      0,       0},
                            {L_,       2,        0,      0,       0},
                            {L_,       8,       100,    100,      0},
                            {L_,       8,        0,     100,      1},
                            {L_,      12,        0,     512,      2},
                            {L_,      12,        0,   65535,      2},
                            {L_,      12, MIN_ULONG, MIN_ULONG + 10000, 2},
                            {L_,      12, MAX_ULONG - 10000, MAX_ULONG, 2},
                            {L_,      12,        0,  100000,      3},
                            {L_,      12,        0,   MAX_ULONG, sizeOfULong},
                            {L_,      12,  MIN_ULONG,   0,        0},
                            {L_,      12,  MIN_ULONG, MAX_ULONG, sizeOfULong}};
            enum { NUM_DATA = sizeof DATA_ULONG / sizeof *DATA_ULONG };
            ConsumeIntegralTest<unsigned long>::testConsumeIntegralInRange(
                                                                   DATA_ULONG,
                                                                   NUM_DATA);
        }
        {
            if (verbose)
                cout << endl << "<bsls::Types::Int64>" << endl;

            const bsls::Types::Int64 MAX_INT64 =
                    bsl::numeric_limits<bsls::Types::Int64>::max();
            const bsls::Types::Int64 MIN_INT64 =
                    bsl::numeric_limits<bsls::Types::Int64>::min();
            ConsumeIntegralTestCases<bsls::Types::Int64>
            DATA_INT64[] = {//LINE   LENGTH     MIN     MAX     BYTES
                            //----  -------     ---     ---      ------
                            {L_,       0,        0,      0,      0},
                            {L_,       1,        0,      0,      0},
                            {L_,       2,        0,      0,      0},
                            {L_,       8,      100,    100,      0},
                            {L_,       4,     -100,      0,      1},
                            {L_,       8,        0,    100,      1},
                            {L_,      12,      -44,     80,      1},
                            {L_,      12,     -256,    256,      2},
                            {L_,      12,    -32000,  32000,     2},
                            {L_,      12, MIN_INT64, MIN_INT64 + 10000, 2},
                            {L_,      12, MAX_INT64 - 10000, MAX_INT64, 2},
                            {L_,      12,    -100000, 100000,    3},
                            {L_,      12,       0,    MAX_INT64, 8},
                            {L_,      12,  MIN_INT64,    0,      8},
                            {L_,      12,  MIN_INT64, MAX_INT64, 8}};
            enum { NUM_DATA = sizeof DATA_INT64 / sizeof *DATA_INT64 };
            ConsumeIntegralTest<
                bsls::Types::Int64>::testConsumeIntegralInRange(DATA_INT64,
                                                                NUM_DATA);
        }
        {
            if (verbose)
                cout << endl << "<bsls::Types::Uint64>" << endl;

            const bsls::Types::Uint64 MAX_UINT64 =
                    bsl::numeric_limits<bsls::Types::Uint64>::max();
            const bsls::Types::Uint64 MIN_UINT64 =
                    bsl::numeric_limits<bsls::Types::Uint64>::min();
            ConsumeIntegralTestCases<bsls::Types::Uint64>
            DATA_UINT64[] = {//LINE   LENGTH     MIN     MAX     BYTES
                            //----  -------     ---     ---      ------
                            {L_,       0,        0,      0,       0},
                            {L_,       1,        0,      0,       0},
                            {L_,       2,        0,      0,       0},
                            {L_,       8,       100,    100,      0},
                            {L_,       8,        0,     100,      1},
                            {L_,      12,        0,     512,      2},
                            {L_,      12,        0,   65535,      2},
                            {L_,      12, MIN_UINT64, MIN_UINT64 + 10000, 2},
                            {L_,      12, MAX_UINT64 - 10000, MAX_UINT64, 2},
                            {L_,      12,       0,    200000,     3},
                            {L_,      12,       0,    MAX_UINT64, 8},
                            {L_,      12,  MIN_UINT64,   0,       0},
                            {L_,      12,  MIN_UINT64, MAX_UINT64, 8}};
            enum { NUM_DATA = sizeof DATA_UINT64 / sizeof *DATA_UINT64 };
            ConsumeIntegralTest<
                bsls::Types::Uint64>::testConsumeIntegralInRange(DATA_UINT64,
                                                                 NUM_DATA);
        }
        {
            if (verbose) cout << endl << "<char>" << endl;

            const char MAX_CHAR = bsl::numeric_limits<char>::max();
            const char MIN_CHAR = bsl::numeric_limits<char>::min();

            ConsumeIntegralTestCases<char>
            DATA_CHAR[] = {
                // LINE   LENGTH     MIN     MAX     BYTES
                //----  -------     ---     ---      ------
                {L_,       0,        0,      0,       0},
                {L_,       1,        0,      0,       0},
                {L_,       2,        0,      0,       0},
                {L_,       8,      100,    100,       0},
                {L_,       8,        0,    100,       1},
                {L_,      12,        0,    127,       1},
                {L_,      12, MIN_CHAR, static_cast<char>(MIN_CHAR + 100), 1},
                {L_,      12, static_cast<char>(MAX_CHAR - 100), MAX_CHAR, 1},
                {L_,      12,        0,  MAX_CHAR,    1},
             // {L_,      12,  MIN_CHAR,   0,         1}, // fails on IBM/AIX
                {L_,      12,  MIN_CHAR, MAX_CHAR,    1}};
            enum { NUM_DATA = sizeof DATA_CHAR / sizeof *DATA_CHAR };
            ConsumeIntegralTest<char>::testConsumeIntegralInRange(DATA_CHAR,
                                                                  NUM_DATA);
        }
        {
            if (verbose)
                cout << endl << "<signed char>" << endl;

            const signed char MAX_SCHAR =
                bsl::numeric_limits<signed char>::max();
            const signed char MIN_SCHAR =
                bsl::numeric_limits<signed char>::min();

            ConsumeIntegralTestCases<signed char>
            DATA_SCHAR[] = {
            // LINE   LENGTH     MIN     MAX     BYTES
            //----  -------     ---     ---      ------
            {L_,       0,        0,      0,       0},
            {L_,       1,        0,      0,       0},
            {L_,       2,        0,      0,       0},
            {L_,       8,      100,    100,       0},
            {L_,       4,     -100,      0,       1},
            {L_,       8,        0,    100,       1},
            {L_,      12,      -44,     80,       1},
            {L_,      12,        0,    127,       1},
            {L_,  12, MIN_SCHAR, static_cast<signed char>(MIN_SCHAR + 100), 1},
            {L_,  12, static_cast<signed char>(MAX_SCHAR - 100), MAX_SCHAR, 1},
            {L_,      12,       0,    MAX_SCHAR,  1},
            {L_,      12,  MIN_SCHAR,    0,       1},
            {L_,      12,  MIN_SCHAR, MAX_SCHAR,  1}};
            enum { NUM_DATA = sizeof DATA_SCHAR / sizeof *DATA_SCHAR };
            ConsumeIntegralTest<signed char>::testConsumeIntegralInRange(
                                                                   DATA_SCHAR,
                                                                   NUM_DATA);
        }
        {
            if (verbose) cout << endl << "<unsigned char>" << endl;

            const unsigned char MAX_UCHAR =
                bsl::numeric_limits<unsigned char>::max();
            const unsigned char MIN_UCHAR =
                bsl::numeric_limits<unsigned char>::min();
            ASSERT(0 == MIN_UCHAR);
            ConsumeIntegralTestCases<unsigned char>
            DATA_UCHAR[] = {
            // LINE   LENGTH     MIN     MAX     BYTES
            //----  -------     ---     ---      ------
            {L_,       0,        0,      0,       0},
            {L_,       1,        0,      0,       0},
            {L_,       2,        0,      0,       0},
            {L_,       8,      100,    100,       0},
            {L_,       8,        0,    100,       1},
            {L_,      12,        0,    127,       1},
            {L_,12, MIN_UCHAR, static_cast<unsigned char>(MIN_UCHAR + 100), 1},
            {L_,12, static_cast<unsigned char>(MAX_UCHAR - 100), MAX_UCHAR, 1},
            {L_,      12,        0,   MAX_UCHAR,  1},
            {L_,      12,   MIN_UCHAR,   0,       0},
            {L_,      12,   MIN_UCHAR, MAX_UCHAR, 1}};
            enum { NUM_DATA = sizeof DATA_UCHAR / sizeof *DATA_UCHAR };
            ConsumeIntegralTest<unsigned char>::testConsumeIntegralInRange(
                                                                   DATA_UCHAR,
                                                                   NUM_DATA);
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING GENERATOR FUNCTION 'ggg'
        //
        // Concerns:
        //: 1 That valid generator syntax produces expected results.
        //:
        //: 2 That invalid syntax is detected and reported. This includes
        //:   invalid specifications as well as not enough buffer space.
        //
        // Plan:
        //: 1 For each of an enumerated sequence of 'spec' values, use the
        //:   generator function 'ggg' to set the state of a buffer.  The first
        //:   set of tests employ valid 'spec' values, and the second set
        //:   employ invalid 'spec' values.  Verify that 'ggg' returns a status
        //:   as expected.  (C-1, C-2)
        //
        // Testing:
        //   void gg(vector<uint8_t> *buf, string_view& spec);
        //   int ggg(vector<uint8_t> *buf, string_view& spec, bool vF = true);
        // --------------------------------------------------------------------
        if (verbose)
            cout << endl
                 << "TESTING GENERATOR FUNCTION 'ggg'" << endl
                 << "================================" << endl;

        static const struct {
            int              d_lineNum;  // source line number
            bsl::string_view d_spec;     // specification string
            const bool       d_isValid;  // is valid generator 'spec'
            bsl::string_view d_expected; // expected output from 'gg'
        } DATA[] = {
#define makeSV makeStringView
            //LINE      SPEC           VALID     EXPECTED
            //----      ----          -------    --------
            { L_,        "",            true,       ""           },
            { L_, makeSV("1x1\0" "1x"), true,  makeSV("x\0" "x") },
            { L_,        "1x1|1x",      true,       "x\\x"       },
            { L_,        "2x3y1|",      true,       "xxyyy\\"    },
            { L_,        "2|6x",        true,       "\\\\xxxxxx" },
            { L_,        "5x1|3y",      true,       "xxxxx\\yyy" },
            { L_,        ".",           false,      ""           },
            { L_,        "0",           false,      ""           },
            { L_,        "1",           false,      ""           },
            { L_,        "3x1",         false,      ""           },
            { L_,        "3\\",         false,      ""           },
            { L_,        "0a",          false,      ""           }
#undef makeSV
        };
        bsl::vector<bsl::uint8_t> testBuf;

        enum { NUM_DATA = sizeof DATA / sizeof *DATA };

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int        LINE     = DATA[ti].d_lineNum;
            bsl::string_view SPEC     = DATA[ti].d_spec;
            const bool       VALID    = DATA[ti].d_isValid;
            bsl::string_view EXPECTED = DATA[ti].d_expected;

            if (ggg(&testBuf, SPEC, false) < 0) {
                ASSERT(true == VALID);
                gg(&testBuf, SPEC);

                ASSERT(testBuf.size() == EXPECTED.length());
                ASSERT(
                 bsl::equal(testBuf.begin(), testBuf.end(), EXPECTED.begin()));
            }
            else {
                ASSERT(false == VALID);
            }
            if (veryVerbose) {
                T_ P_(LINE) P_(SPEC) P(VALID)
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
        //: 1 Perform a test of the primary utilities.
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        const uint8_t smallData[]  = {0x61, 0x63, 0x65};
        const uint8_t stringData[] = {0x61, 0x63, 0x65, 0x67, 0x69, 0x21,
                                      0x44, 0x46, 0x48, 0x50, 0x52, 0x54,
                                      0x42, 0x22, 0x72, 0x47, 0x49, 0x51,
                                      0x53, 0x55, 0x56, 0x57, 0x58, 0x59};

        int testIters = 100;

        if (verbose)
            cout << endl
                 << "BREATHING TEST" << endl
                 << "==============" << endl;
        {
            bslim::FuzzDataView fdv(KB_DATA, sizeof(KB_DATA));
            ASSERT(1024 == fdv.length());

            if (veryVerbose) {
                cout << "Testing 'consumeNumberInRange<INTEGRAL>' 0, 0"
                     << endl;
                cout << "\tfdv.length(): " << fdv.length() << endl;
            }
            for (int j = 0; j < testIters; ++j) {
                int i = Util::consumeNumberInRange<int>(&fdv, 0, 0);
                if (veryVeryVerbose) {
                    T_ T_ P_(fdv.length()) P(i)
                }
                ASSERT(0 == i);
            }

            if (veryVerbose) {
                cout << "Testing 'consumeNumberInRange<INTEGRAL>' 1, 1000"
                     << endl;
                cout << "\tfdv.length(): " << fdv.length() << endl;
            }
            for (int j = 0; j < testIters; ++j) {
                int i = Util::consumeNumberInRange<int>(&fdv, 1, 1000);
                if (veryVeryVerbose) {
                    T_ T_ P_(fdv.length()) P(i)
                }
                ASSERT(i >= 0 && i <= 1000);
            }

            if (veryVerbose) {
                cout << "Testing 'consumeNumberInRange<INTEGRAL>' -1000, 1"
                     << endl;
                cout << "\tfdv.length(): " << fdv.length() << endl;
            }
            for (int j = 0; j < testIters; ++j) {
                int i = Util::consumeNumberInRange<int>(&fdv, -1000, 1);
                ASSERT(1 >= i && -1000 <= i);
                if (veryVeryVerbose) {
                    T_ T_ P_(fdv.length()) P(i)
                }
            }

            if (veryVerbose) {
                cout << "Testing 'consumeNumberInRange<INTEGRAL>' -10000, "
                        "-1000"
                     << endl;
                cout << "\tfdv.length(): " << fdv.length() << endl;
            }
            for (int j = 0; j < testIters; ++j) {
                // both 'min' and 'max' are negative
                int i = Util::consumeNumberInRange<int>(&fdv, -10000, -1000);
                ASSERT(-1000 >= i && -10000 <= i);
                if (veryVeryVerbose) {
                    T_ T_ P_(fdv.length()) P(i)
                }
            }
        }
        {
            bslim::FuzzDataView fdv(KB_DATA, sizeof(KB_DATA));

            if (veryVerbose) {
                cout << "Testing 'consumeNumber<INTEGRAL>'" << endl;
                cout << "\tfdv.length(): " << fdv.length() << endl;
            }

            for (int j = 0; j < testIters; ++j) {
                int i = Util::consumeNumber<int>(&fdv);
                if (veryVeryVerbose) {
                    T_ T_ P_(fdv.length()) P(i)
                }
            }
        }
        {
            bslim::FuzzDataView fdv(smallData, 0);
            if (veryVerbose) {
                cout << "Testing 'consumeNumber<INTEGRAL>' with 0 byte "
                        "buffer: "
                     << endl;
                cout << "\tfdv.length(): " << fdv.length() << endl;
            }
            ASSERT(0 == fdv.length());
            int i = Util::consumeNumber<int>(&fdv);
            ASSERT(bsl::numeric_limits<int>::min() == i);
        }
        {
            if (veryVerbose) {
                cout << "Testing 'consumeNumber<INTEGRAL>' with 3 byte "
                        "buffer: "
                     << endl;
            }
            bslim::FuzzDataView fdv(smallData, sizeof(smallData));
            ASSERT(3 == fdv.length());
            int i = Util::consumeNumber<int>(&fdv);
            ASSERT(bsl::numeric_limits<int>::min() != i);
        }
        {
            if (veryVerbose) {
                cout << "Testing 'consumeNumber<FLOATING_POINT>' with 0 byte "
                        "buffer: "
                     << endl;
            }
            bslim::FuzzDataView fdv(smallData, 0);
            ASSERT(0 == fdv.length());
            double d = Util::consumeNumber<double>(&fdv);
            ASSERT(-bsl::numeric_limits<double>::max() == d);
        }
        {
            if (veryVerbose) {
                cout << "Testing 'consumeNumber<FLOATING_POINT>' with 3 byte "
                        "buffer: "
                     << endl;
            }
            bslim::FuzzDataView fdv(smallData, sizeof(smallData));
            ASSERT(3 == fdv.length());
            double d = Util::consumeNumber<double>(&fdv);
            ASSERT(-bsl::numeric_limits<double>::max() != d);
        }
        {
            if (veryVerbose) {
                cout << "Testing creation of a 'bsl::string': " << endl;
            }
            bslim::FuzzDataView fdv(stringData, sizeof(stringData));
            bslim::FuzzDataView v2 = fdv.removePrefix(3);
            bsl::string         s2(v2.begin(), v2.end());
            if (veryVerbose) {
                cout << "\tstring: " << s2 << endl;
            }
        }
        {
            if (veryVerbose) {
                cout << "Testing 'consumeNumber<FLOATING_POINT>': " << endl;
            }
            bslim::FuzzDataView fdv(KB_DATA, sizeof(KB_DATA));
            double              d = Util::consumeNumber<double>(&fdv);
            if (veryVerbose) {
                cout << "\tdouble d = " << d << endl;
            }
        }
        {
            if (veryVerbose) {
                cout << "Testing removing all remaining bytes and creating "
                        "'bsl::vector': "
                     << endl;
            }
            bslim::FuzzDataView        fdv(KB_DATA, sizeof(KB_DATA));
            bsl::size_t                fdvOriginalLen = fdv.length();
            bslim::FuzzDataView        v1 = fdv.removePrefix(fdv.length());
            bsl::vector<unsigned char> vec(v1.begin(), v1.end());
            ASSERT(vec.size() == fdvOriginalLen);

            if (veryVerbose) {
                cout << "\tremaining bytes: " << fdv.length() << endl;
                cout << "\tvec[0]: " << vec[0] << endl;
                cout << "\tvec.size(): " << vec.size() << endl;
            }
        }
        {
            if (veryVerbose) {
                cout << "Testing 'consumeRandomLengthString': " << endl;
            }
            bslim::FuzzDataView fdv(KB_DATA, sizeof(KB_DATA));
            bsl::string s;
            Util::consumeRandomLengthString(&s, &fdv, 100);
            ASSERT(100 >= s.length());
            if (veryVerbose) {
                cout << "\tstring s = " << s << endl;
            }
        }
        {
            if (veryVerbose) {
                cout << "Testing 'consumeRandomLengthChars': " << endl;
            }
            bslim::FuzzDataView fdv(KB_DATA, sizeof(KB_DATA));
            bsl::vector<char> v;
            Util::consumeRandomLengthChars(&v, &fdv, 100);
            bsl::string_view sv(v.data(), v.size());
            ASSERT(100 >= v.size());
            if (veryVerbose) {
                cout << "\tstring_view sv = " << sv << endl;
            }
        }
      } break;
#if defined(BDE_BUILD_TARGET_ASAN)
      case -1: {
        // --------------------------------------------------------------------
        // NEGATIVE ASAN TEST 'consumeRandomLengthChars'
        //
        // Concerns:
        //: 1 That ASan will detect overruns when a 'string_view' generated
        //:   from a 'vector' returned by 'consumeRandomLengthChars' is
        //:   accessed.
        //
        // Plan:
        //: 1 Implement a rudimentary version of 'strlen' and pass it a
        //:   'string_view' obtained from a 'vector' returned by
        //:   'consumeRandomLengthChars'.
        //:
        //:   1 Create a buffer and use it to construct a 'FuzzDataView'.
        //:
        //:   2 Fill a 'vector' using 'consumeRandomLengthChars'.
        //:
        //:   3 Create a 'string_view' from this 'vector'.
        //:
        //:   4 Verify that ASan catches the attempt to access beyond the end
        //:     of 'vector'.
        //
        // Testing:
        //   void consumeRandomLengthChars(bsl::vector<char>*,FDV*,maxLen);
        // --------------------------------------------------------------------
        if (verbose)
            cout << "NEGATIVE ASAN TEST 'consumeRandomLengthChars'" << endl
                 << "=============================================" << endl;
        uint8_t ASAN_DATA[] = {0x41, 0x53, 0x41, 0x4E};

        bslim::FuzzDataView fdv(ASAN_DATA, sizeof(ASAN_DATA));
        bsl::vector<char>   v;
        Util::consumeRandomLengthChars(&v, &fdv, sizeof(ASAN_DATA));
        bsl::string_view sv(v.data(), v.size());

        // rudimentary 'strlen'
        const char *c = sv.data();
        while (*c) {
            c++;
        }
        BSLS_ASSERT_INVOKE_NORETURN("Should not reach here");
      } break;
#endif // BDE_BUILD_TARGET_ASAN
      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND.\n";
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        cerr << "Error, non-zero test status = " << testStatus << ".\n";
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
