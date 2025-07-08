// baljsn_encodeimplutil.t.cpp                                        -*-C++-*-
#include <baljsn_encodeimplutil.h>

#include <baljsn_formatter.h>

#include <balb_testmessages.h>  // 'balb::CustomString', etc.

#include <bslim_testutil.h>

#include <bdlb_nullablevalue.h>
#include <bdlb_variant.h>

#include <bdlt_date.h>
#include <bdlt_datetz.h>
#include <bdlt_datetime.h>
#include <bdlt_datetimetz.h>
#include <bdlt_time.h>
#include <bdlt_timetz.h>

#include <bsl_climits.h>
#include <bsl_cstddef.h>
#include <bsl_cstdlib.h>
#include <bsl_iostream.h>
#include <bsl_limits.h>
#include <bsl_memory.h>
#include <bsl_sstream.h>  // `bsl::ostringstream`
#include <bsl_string.h>
#include <bsl_utility.h>
#include <bsl_vector.h>

using namespace BloombergLP;
using bsl::cout;
using bsl::cerr;
using bsl::endl;

// ============================================================================
//                             TEST PLAN
// ----------------------------------------------------------------------------
//                             Overview
//                             --------
// The component under test implements an encoder for outputting
// `bdeat`-compatible objects in the JSON format.  The object types that can be
// encoded include `bdeat` sequence, choice, array, enumeration, customized,
// simple, and dynamic types.  In addition, the encoder supports options to
// allow compact and pretty formatting.
//
// We use standard table-based approach to testing where we put both input and
// expected output in the same table row and verify that the actual result
// matches the expected value.
// ----------------------------------------------------------------------------
// CLASS METHODS
//
// [ 9] int encode(bsl::ostream& stream, const TYPE& v, o);   // SEQUENCES
// [ 8] int encode(bsl::ostream& stream, const TYPE& v, o);     // CHOICES
// [ 7] int encode(bsl::ostream& s, const bsl::vector<TYPE>& v, options);
// [ 6] int encode(ostream& s, const bdlb::NullableValue<TYPE>& v, o);
// [ 5] int encode(ostream& s, const TYPE& v, o);          // ENUMERATIONS
// [ 4] int encode(ostream& s, const TYPE& v, o);         // DATE AND TIME
// [ 3] int encode(ostream& s, const TYPE& v, o);               // NUMBERS
// [ 2] int encode(const bsl::string & value);
// [ 2] int encode(const char *value);
// [ 1] int encode(const bool& value);

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

#define ASSERT_EQ(X, Y) ASSERTV(X,Y,X == Y)
#define ASSERT_NE(X, Y) ASSERTV(X,Y,X != Y)
#define ASSERT_LE(X, Y) ASSERTV(X,Y,X <= Y)
#define ASSERT_GE(X, Y) ASSERTV(X,Y,X >= Y)

#define LOOP1_ASSERT_EQ(L,X,Y) ASSERTV(L,X,Y,X == Y)
#define LOOP1_ASSERT_NE(L,X,Y) ASSERTV(L,X,Y,X != Y)

#define Q            BSLIM_TESTUTIL_Q   // Quote identifier literally.
#define P            BSLIM_TESTUTIL_P   // Print identifier and value.
#define P_           BSLIM_TESTUTIL_P_  // P(X) without '\n'.
#define T_           BSLIM_TESTUTIL_T_  // Print a tab (w/o newline).
#define L_           BSLIM_TESTUTIL_L_  // current Line number

// ============================================================================
//                   GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------

#if defined(BSLS_PLATFORM_CMP_IBM)
#define U_SKIP_DUE_TO_COMPILER_RESOURCE_LIMITATIONS
#endif

typedef baljsn::EncodeImplUtil<baljsn::Formatter> ImplUtil;
typedef baljsn::EncoderOptions                    Options;
typedef baljsn::EncoderOptions::EncodingStyle     Style;
typedef bsls::Types::Int64                        Int64;
typedef bsls::Types::Uint64                       Uint64;

// ============================================================================
//                          GLOBAL COUNTS
// ============================================================================

static bsl::size_t countAssertEncodedValueIsEqual                  = 0U;
static bsl::size_t countAssertEncodedValueIsEqual_SkipNullElements = 0U;
static bsl::size_t countAssertEncodedValueIsEqual_SkipEmptyArrays  = 0U;
static bsl::size_t countAssertEncodedValueIsEqualExtended          = 0U;

static void dumpCounts(int testCase)
{
    P(testCase);
    P(countAssertEncodedValueIsEqual);
    P(countAssertEncodedValueIsEqual_SkipNullElements);
    P(countAssertEncodedValueIsEqual_SkipEmptyArrays);
    P(countAssertEncodedValueIsEqualExtended);
}

// ============================================================================
//                          TEST ENTITY DECLARATIONS
// ----------------------------------------------------------------------------

namespace {
namespace u {

/// Assert that `encoder` can encode a diverse set of values of the specified
/// integral `TYPE` to a JSON number literal, and that the encoding loses no
/// precision of the original value.
template <class TYPE>
void testNumber()
{
    // Test non-negative numbers.

    const struct {
        int         d_line;
        Uint64      d_value;
        const char *d_result;
    } DATA_1[] = {
        //LINE   VALUE            RESULT
        //---- ---------  -----------------------
        { L_,          0,                    "0" },
        { L_,          1,                    "1" },
        { L_,  UCHAR_MAX,                  "255" },
        { L_,   SHRT_MAX,                "32767" },
        { L_,  USHRT_MAX,                "65535" },
        { L_,    INT_MAX,           "2147483647" },
        { L_,   UINT_MAX,           "4294967295" },
#if   defined(BSLS_PLATFORM_CPU_32_BIT)                                       \
  || (defined(BSLS_PLATFORM_CPU_64_BIT) && defined(BSLS_PLATFORM_OS_WINDOWS))
        { L_,   LONG_MAX,           "2147483647" },
        { L_,  ULONG_MAX,           "4294967295" },
#elif defined(BSLS_PLATFORM_CPU_64_BIT)
        { L_,   LONG_MAX,  "9223372036854775807" },
        { L_,  ULONG_MAX, "18446744073709551615" },
#else
# error "baljsn_encoder.t.cpp does not support the platform's bitness."
#endif
        { L_,  LLONG_MAX,  "9223372036854775807" },
        { L_, ULLONG_MAX, "18446744073709551615" }
    };

    const int NUM_DATA_1 = sizeof DATA_1 / sizeof *DATA_1;

    for (int ti = 0; ti < NUM_DATA_1; ++ti) {
        const int         LINE  = DATA_1[ti].d_line;
        const Uint64      VALUE = DATA_1[ti].d_value;
        const char *const EXP   = DATA_1[ti].d_result;

        if (VALUE > static_cast<Uint64>(bsl::numeric_limits<TYPE>::max())) {
            continue;                                               // CONTINUE
        }

        const TYPE VALUE_AS_TYPE = static_cast<TYPE>(VALUE);

        bsl::ostringstream oss;
        ASSERTV(LINE, 0 == ImplUtil::encode(&oss, VALUE_AS_TYPE));

        bsl::string result = oss.str();
        ASSERTV(LINE, result, EXP, result == EXP);
    }

    // Test negative numbers.

    const struct {
        int         d_line;
        Int64       d_value;
        const char *d_result;
    } DATA_2[] = {
        //LINE   VALUE            RESULT
        //---- ---------  -----------------------
        { L_,         -1,                   "-1" },
        { L_,  SCHAR_MIN,                 "-128" },
        { L_,   SHRT_MIN,               "-32768" },
        { L_,    INT_MIN,          "-2147483648" },
#if   defined(BSLS_PLATFORM_CPU_32_BIT)                                       \
  || (defined(BSLS_PLATFORM_CPU_64_BIT) && defined(BSLS_PLATFORM_OS_WINDOWS))
        { L_,   LONG_MIN,          "-2147483648" },
#elif defined(BSLS_PLATFORM_CPU_64_BIT)
        { L_,   LONG_MIN, "-9223372036854775808" },
#else
# error "baljsn_encoder.t.cpp does not support the platform's bitness."
#endif
        { L_,  LLONG_MIN, "-9223372036854775808" }
    };

    const int NUM_DATA_2 = sizeof DATA_2 / sizeof *DATA_2;

    for (int ti = 0; ti < NUM_DATA_2; ++ti) {
        const int         LINE  = DATA_2[ti].d_line;
        const Int64       VALUE = DATA_2[ti].d_value;
        const char *const EXP   = DATA_2[ti].d_result;

        if (VALUE < static_cast<Int64>(bsl::numeric_limits<TYPE>::min())) {
            continue;                                               // CONTINUE
        }

        const TYPE VALUE_AS_TYPE = static_cast<TYPE>(VALUE);

        bsl::ostringstream oss;
        ASSERTV(LINE, 0 == ImplUtil::encode(&oss, VALUE_AS_TYPE));      // TEST

        bsl::string result = oss.str();
        ASSERTV(LINE, result, EXP, result == EXP);
    }
}

}  // close namespace u
}  // close anonymous namespace

// ============================================================================
//                               MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int             test = argc > 1 ? atoi(argv[1]) : 0;
    bool         verbose = argc > 2; static_cast<void>(        verbose);
    bool     veryVerbose = argc > 3; static_cast<void>(    veryVerbose);
    bool veryVeryVerbose = argc > 4; static_cast<void>(veryVeryVerbose);

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:
      case 9: {
        // --------------------------------------------------------------------
        // ENCODING SEQUENCES
        //
        // Concerns:
        // 1. Sequence objects are encoded as name-value pairs.
        //
        // 2. Null elements are not encoded.
        //
        // 3. Empty sequence will be encoded as an object without any member.
        //
        // 4. Error occurred when encoding an element of a sequence causes
        //    error in encoding a sequence.
        //
        // Plan:
        // 1. Encode a void sequence and verify the output is an empty JSON
        //    object.
        //
        // 2. Encode a sequence with some null values an verify only the
        //    non-null values are encoded.
        //
        // 3. Encoded a sequence with an unselected Choice and verify an error
        //    is returned.
        //
        // Testing:
        //   int encode(bsl::ostream& stream, const TYPE& v, o);   // SEQUENCES
        // --------------------------------------------------------------------

        if (verbose) cout << "\nENCODING SEQUENCES"
                          << "\n==================" << endl;

        if (verbose) cout << "Encode empty sequence." << endl;
        {
            const balb::VoidSequence X;

            bsl::ostringstream oss;
            ASSERTV(0 == ImplUtil::encode(&oss, X));

            bsl::string result = oss.str();
            ASSERTV(result, result == "{}");
        }

        {
            balb::Sequence2 mX; const balb::Sequence2& X = mX;
            mX.element1() = balb::CustomString("Hello");
            mX.element2() = 42;
            mX.element3() = bdlt::DatetimeTz(
                                 bdlt::Datetime(2012, 12, 31, 12, 59, 59, 999),
                                 -720);

            {
                bsl::ostringstream oss;
                ASSERTV(0 == ImplUtil::encode(&oss, X));

                const char *EXP =
                    "{"
                        "\"element1\":\"Hello\","
                        "\"element2\":42,"
                        "\"element3\":\"2012-12-31T12:59:59.999-12:00\""
                    "}";
                bsl::string result = oss.str();
                ASSERTV(result, EXP, result == EXP);
            }

            mX.element4().makeValue();
            mX.element5().makeValue(3.14);
            {
                // `element4` is an unselected Choice.  Ensure encode sequence
                // propagate errors.

                bsl::ostringstream lss;
                bsl::ostringstream oss;
                ASSERTV(0 != ImplUtil::encode(&lss, &oss, X));
                ASSERTV("" != lss.str());
            }

            mX.element4().value().makeSelection1(99);
            {
                bsl::ostringstream oss;
                ASSERTV(0 == ImplUtil::encode(&oss, X));
                const char *EXP =
                    "{"
                        "\"element1\":\"Hello\","
                        "\"element2\":42,"
                        "\"element3\":\"2012-12-31T12:59:59.999-12:00\","
                        "\"element4\":{\"selection1\":99},"
                        "\"element5\":3.14"
                    "}";
                bsl::string result = oss.str();
                ASSERTV(result, EXP, result == EXP);
            }
        }
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // ENCODING CHOICES
        //
        // Concerns:
        // 1. Encoding a Choice object results in a JSON object with one
        //    name-value pair, where the name is the selection name and value
        //    is the selected value.
        //
        // 2. Unselected Choice returns an error.
        //
        // 3. Error when encoding the selection is propagated.
        //
        // 4. Array Choice selections are encoded correctly.
        //
        // Plan:
        // 1. Use a brute force approach:  (C-1..3)
        //
        //   1. Encode an unselected Choice object and verify it returns an
        //      error.  (C-2)
        //
        //   2. Encode a selected Choice and verify it returns a name-value
        //      pair.  (C-1)
        //
        //   3. Encode a selected Choice, where the selection is an unselected
        //      Choice and verify it returns an error.  (C-3)
        //
        // 2. Use the table-driven approach to verify that array Choice
        //    selections encode as expected.  Test empty and non-empty arrays
        //    with both settings for the `encodeEmptyArrays` encoder option.
        //    (C-4)
        //
        // Testing:
        //   int encode(bsl::ostream& stream, const TYPE& v, o);     // CHOICES
        // --------------------------------------------------------------------

        if (verbose) cout << "\nENCODING CHOICES"
                          << "\n================" << endl;

        if (verbose) cout << "Encode Choice" << endl;
        {
            balb::Choice2 mX; const balb::Choice2& X = mX;
            {
                // Test that it fails without selection.

                bsl::ostringstream lss;
                bsl::ostringstream oss;
                ASSERTV(0 != ImplUtil::encode(&lss, &oss, X));
                ASSERTV("" != lss.str());
            }
            {
                mX.makeSelection1(true);

                bsl::ostringstream oss;
                ASSERTV(0 == ImplUtil::encode(&oss, X));

                bsl::string result = oss.str();
                ASSERTV(result, result == "{\"selection1\":true}");
            }
            {
                mX.makeSelection2("A quick brown fox");

                bsl::ostringstream oss;
                ASSERTV(0 == ImplUtil::encode(&oss, X));

                bsl::string result = oss.str();
                ASSERTV(result,
                        result == "{\"selection2\":\"A quick brown fox\"}");
            }
            {
                mX.makeSelection3();

                bsl::ostringstream lss;
                bsl::ostringstream oss;
                ASSERTV(0 != ImplUtil::encode(&lss, &oss, X));
                ASSERTV("" != lss.str());
            }
            {
                mX.selection3().makeSelection1(42);

                bsl::ostringstream oss;
                ASSERTV(0 == ImplUtil::encode(&oss, X));

                bsl::string result = oss.str();
                ASSERTV(result,
                        result == "{\"selection3\":{\"selection1\":42}}");
            }
        }

        if (verbose) cout << "Encode Choice w/Array Selection" << endl;
        {
            const struct {
                int         d_line;
                const char *d_input_p;            // input for array selection
                bool        d_encodeEmptyArrays;  // encoder option
                const char *d_result_p;           // expected result
            } DATA[] = {
                //LINE  INPUT   "EEA"   RESULT
                //----  -----   -----   ------
                { L_,       0,  false,  "{\"selection1\":[]}"                },
                { L_,       0,  true,   "{\"selection1\":[]}"                },
                { L_,   "XyZ",  false,  "{\"selection1\":[\"XyZ\"]}"         },
                { L_,   "XyZ",  true,   "{\"selection1\":[\"XyZ\"]}"         },
            };
            enum { NUM_DATA = sizeof DATA / sizeof *DATA };

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int   LINE  = DATA[ti].d_line;
                const char *INPUT = DATA[ti].d_input_p;
                const bool  EEA   = DATA[ti].d_encodeEmptyArrays;
                const char *EXP   = DATA[ti].d_result_p;

                bsl::vector<bsl::string> ARRAY;
                if (INPUT) {  // test with non-empty array selection
                    ARRAY.push_back(INPUT);
                }

                Options options;
                options.setEncodeEmptyArrays(EEA);

                balb::Choice4 mX; const balb::Choice4& X = mX;
                mX.makeSelection1(ARRAY);

                if (veryVerbose) { P_(X); P(EXP); }

                bsl::ostringstream oss;
                ASSERTV(LINE, 0 == ImplUtil::encode(&oss, X, options));

                bsl::string result = oss.str();
                ASSERTV(LINE, result, EXP, result == EXP);
            }
        }
        {
            const struct {
                int         d_line;
                const char *d_input_p;            // input for array selection
                bool        d_encodeEmptyArrays;  // encoder option
                const char *d_result_p;           // expected result
            } DATA[] = {
                //LINE  INPUT   "EEA"   RESULT
                //----  -----   -----   ------
                { L_,       0, false, "{\"selection1\":{\"selection1\":[]}}" },
                { L_,       0,  true, "{\"selection1\":{\"selection1\":[]}}" },
                { L_,   "XyZ",  false,
                               "{\"selection1\":{\"selection1\":[\"XyZ\"]}}" },
                { L_,   "XyZ",  true,
                               "{\"selection1\":{\"selection1\":[\"XyZ\"]}}" },
            };
            enum { NUM_DATA = sizeof DATA / sizeof *DATA };

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int   LINE  = DATA[ti].d_line;
                const char *INPUT = DATA[ti].d_input_p;
                const bool  EEA   = DATA[ti].d_encodeEmptyArrays;
                const char *EXP   = DATA[ti].d_result_p;

                bsl::vector<bsl::string> ARRAY;
                if (INPUT) {  // test with non-empty array selection
                    ARRAY.push_back(INPUT);
                }

                Options options;
                options.setEncodeEmptyArrays(EEA);

                balb::Choice5 mX; const balb::Choice5& X = mX;
                mX.makeSelection1().makeSelection1(ARRAY);

                if (veryVerbose) { P_(X); P(EXP); }

                bsl::ostringstream oss;
                ASSERTV(LINE, 0 == ImplUtil::encode(&oss, X, options));

                bsl::string result = oss.str();
                ASSERTV(LINE, result, EXP, result == EXP);
            }
        }
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // ENCODING ARRAYS
        //
        // Concerns:
        // 1. `bsl::vector<char>` is encoded into as a JSON string type in
        //    base64 encoding.
        //
        // 2. Empty `bsl::vector<char>` results in an empty string.
        //
        // 3. Empty arrays of other types by will not be encoded by default.
        //
        // 4. Empty vectors with the `encodeEmptyArrays` option set to `true`
        //    result in an '[' and ']' being output.
        //
        // 5. The formatting is appropriately output for all vectors including
        //    empty vectors when used in consonance with the
        //    `encodeEmptyArrays` option.
        //
        // Plan:
        // 1. Use a table-driven approach:
        //
        //   1. Create a set of values with various length of `vector<char>`.
        //
        //   2. Encode each values and verify the result is in base64 format.
        //
        // 2. Repeat step one with `vector<int>` instead.
        //
        // 3. Ensure that the output is as expected in all cases.
        //
        // Testing:
        //   int encode(bsl::ostream& s, const bsl::vector<TYPE>& v, options);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nENCODING ARRAYS"
                          << "\n===============" << endl;

        if (verbose) cout << "Encode `vector<char>`" << endl;
        {
            const struct {
                int         d_line;
                const char *d_input_p;
                int         d_inputLength;
                const char *d_result_p;
            } DATA[] = {

            //LINE  INPUT  LEN  RESULT
            //----  -----  ---  ------

            { L_,   "",     0,   "\"\""  },
            { L_,   "\x00", 1,   "\"AA==\""  },
            { L_,   "\x01", 1,   "\"AQ==\""  },
            { L_,   "\xFF", 1,   "\"\\/w==\""  },
            { L_,   "\x00\x00\x00\x00\x00\x00\x00\x00\00", 9,
                                                           "\"AAAAAAAAAAAA\"" }

            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int         LINE   = DATA[ti].d_line;
                const char *const INPUT  = DATA[ti].d_input_p;
                const int         LENGTH = DATA[ti].d_inputLength;
                const char *const EXP    = DATA[ti].d_result_p;
                const bsl::vector<char> VALUE(INPUT, INPUT + LENGTH);

                {
                    bsl::ostringstream oss;
                    ASSERTV(LINE, 0 == ImplUtil::encode(&oss, VALUE));

                    bsl::string result = oss.str();
                    ASSERTV(LINE, result, EXP, result == EXP);
                }

                {
                    Options options;
                    options.setEncodeEmptyArrays(true);

                    bsl::ostringstream oss;
                    ASSERTV(LINE, 0 == ImplUtil::encode(&oss, VALUE, options));

                    bsl::string result = oss.str();
                    ASSERTV(LINE, result, EXP, result == EXP);
                }
            }
        }

        if (verbose) cout << "Encode `vector<int>`" << endl;
        {
            typedef Options::EncodingStyle Style;
            Style P = Options::e_PRETTY;
            Style C = Options::e_COMPACT;

#define NL "\n"

            const struct {
                int            d_line;
                const char    *d_input_p;
                bool           d_encodeEmptyArrays;
                Style          d_encodingStyle;
                int            d_initialIndentLevel;
                int            d_spacesPerLevel;
                const char    *d_result_p;
            } DATA[] = {

            //LINE  INPUT  "EEA"   Style  INDENT    "SPL"   RESULT
            //----  -----  -----   -----  ------    -----   ------

             { L_,    "",  false,     C,      0,      0,    ""           },
             { L_,    "",  false,     P,      0,      0,    ""           },
             { L_,    "",  false,     C,      1,      2,    ""           },
             { L_,    "",  false,     P,      1,      2,    ""           },

             { L_,    "",  true,      C,      0,      0,    "[]"         },
             { L_,    "",  true,      P,      0,      0,    "[]"         },
             { L_,    "",  true,      C,      1,      2,    "[]"         },
             { L_,    "",  true,      P,      1,      2,    "  []"         },

             { L_,    "2", false,     C,      0,      0,    "[2]"        },

             { L_,    "2", false,     P,      0,      0,    "["      NL
                                                            "2"      NL
                                                            "]"          },

             { L_,    "2", false,     C,      1,      2,    "[2]"        },

             { L_,    "2", false,     P,      1,      2,    "  ["    NL
                                                            "    2"  NL
                                                            "  ]"        },

             { L_,    "2", true,      C,      0,      0,    "[2]"        },

             { L_,    "2", true,      P,      0,      0,    "["      NL
                                                            "2"      NL
                                                            "]"          },

             { L_,    "2", true,      C,      1,      2,    "[2]"        },

             { L_,    "2", true,      P,      1,      2,    "  ["    NL
                                                            "    2"  NL
                                                            "  ]"        },

             { L_,  "012", false,     C,      0,      0,    "[0,1,2]"    },

             { L_,  "012", false,     P,      0,      0,    "["      NL
                                                            "0,"     NL
                                                            "1,"     NL
                                                            "2"      NL
                                                            "]"          },

             { L_,  "012", false,     C,      1,      2,    "[0,1,2]"    },

             { L_,  "012", false,     P,      1,      2,    "  ["    NL
                                                            "    0," NL
                                                            "    1," NL
                                                            "    2"  NL
                                                            "  ]"        },

             { L_,  "012", true,      C,      0,      0,    "[0,1,2]"    },

             { L_,  "012", true,      P,      0,      0,    "["      NL
                                                            "0,"     NL
                                                            "1,"     NL
                                                            "2"      NL
                                                            "]"          },

             { L_,  "012", true,      C,      1,      2,    "[0,1,2]"    },

             { L_,  "012", true,      P,      1,      2,    "  ["    NL
                                                            "    0," NL
                                                            "    1," NL
                                                            "    2"  NL
                                                            "  ]"        },

            };
#undef NL
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int         LINE   = DATA[ti].d_line;
                const bsl::string INPUT  = DATA[ti].d_input_p;
                const bool        EEA    = DATA[ti].d_encodeEmptyArrays;
                const Style       STYLE  = DATA[ti].d_encodingStyle;
                const int         INDENT = DATA[ti].d_initialIndentLevel;
                const int         SPL    = DATA[ti].d_spacesPerLevel;
                const bsl::string EXP    = DATA[ti].d_result_p;

                bsl::vector<int> value;
                for (bsl::size_t i = 0; i < INPUT.size(); ++i) {
                    value.push_back(INPUT[i] - '0');
                }

                Options options;
                options.setEncodingStyle(STYLE);
                options.setInitialIndentLevel(INDENT);
                options.setSpacesPerLevel(SPL);
                options.setEncodeEmptyArrays(EEA);

                bsl::ostringstream oss;
                ASSERTV(LINE, 0 == ImplUtil::encode(&oss, value, options));

                bsl::string result = oss.str();
                ASSERTV(LINE, result, EXP, result == EXP);
            }
        }
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // ENCODING NULL-ABLES
        //
        // Concerns:
        // 1. Null value is encoded to "null".
        //
        // 2. Encoding a Nullable object with non-null value is the same as it
        //    the encoding is performed on the value directly.
        //
        // Plan:
        // 1. Use the brute force approach:
        //
        //   1. Create a Nullable object.
        //
        //   2. Encode the Nullable object and verify it is encoded as "null".
        //
        //   3. Make the value non-null.
        //
        //   4. Encode the value and verify the result is as expected.
        //
        // Testing:
        //   int encode(ostream& s, const bdlb::NullableValue<TYPE>& v, o);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nENCODING NULL-ABLES"
                          << "\n===================" << endl;

        if (verbose) cout << "Encode null value" << endl;
        {
            bdlb::NullableValue<int> mX;
            const bdlb::NullableValue<int>& X = mX;

            {
                bsl::ostringstream oss;
                ASSERTV(0 == ImplUtil::encode(&oss, X));

                bsl::string result = oss.str();
                ASSERTV(result, result == "null");
            }

            mX = 0;
            {
                bsl::ostringstream oss;
                ASSERTV(0 == ImplUtil::encode(&oss, X));

                bsl::string result = oss.str();
                ASSERTV(result, result == "0");
            }

            mX = 42;
            {
                bsl::ostringstream oss;
                ASSERTV(0 == ImplUtil::encode(&oss, X));

                bsl::string result = oss.str();
                ASSERTV(result, result == "42");
            }

            mX.reset();
            {
                bsl::ostringstream oss;
                ASSERTV(0 == ImplUtil::encode(&oss, X));

                bsl::string result = oss.str();
                ASSERTV(result, result == "null");
            }
        }
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // ENCODING ENUMERATIONS
        //
        // Concerns:
        // 1. Encoding an Enumeration object result in a JSON string of the
        //    string representation of the Enumeration value.
        //
        // Plan:
        // 1. Use a generated Enumeration type and encode each enumeration
        //    value.
        //
        // 2. Verify that the result is equal to the value of the `toString`
        //    method enclosed in double quotes.
        //
        // Testing:
        //   int encode(ostream& s, const TYPE& v, o);          // ENUMERATIONS
        // --------------------------------------------------------------------

        if (verbose) cout << "\nENCODING ENUMERATIONS"
                          << "\n=====================" << endl;

        const int NUM_ENUMERATORS = balb::Enumerated::NUM_ENUMERATORS;
        for (int ti = 0; ti < NUM_ENUMERATORS; ++ti) {
            balb::Enumerated::Value mX = (balb::Enumerated::Value) ti;
            const balb::Enumerated::Value& X = mX;

            bsl::string exp("\"");
            exp += balb::Enumerated::toString(X);
            exp += '\"';

            bsl::ostringstream oss;
            ASSERTV(ti, 0 == ImplUtil::encode(&oss, X));

            bsl::string result = oss.str();
            ASSERTV(ti, result, exp, result == exp);
        }
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // ENCODING DATE AND TIME TYPES
        //
        // Concerns:
        // 1. Date/time are encoded in ISO 8601 format.
        //
        // 2. Output contains only information contained in the type being
        //    encoded.  (i.e., encoding `bdlt::Date` will not print out a time
        //    or offset.)
        //
        // 3. The encoder option "datetimeFractionalSecondPrecision" affects
        //    the number of digits of precision in the outputs.
        //
        // Plan:
        // 1. Use the table-driven technique:
        //
        //   1. Specify a set of valid values.
        //
        //   2. Encode each value and verify the output is as expected.
        //
        // 2. Perform step one for every date/time types.
        //
        // 3. For a Datetime and DatetimeTz value, set the
        //    "datetimeFractionalSecondPrecision" attribute from 0 to 6, and
        //    verify that the output is as expected.
        //
        // Testing:
        //   int encode(ostream& s, const TYPE& v, o);         // DATE AND TIME
        // --------------------------------------------------------------------

        if (verbose) cout << "\nENCODING DATE AND TIME TYPES"
                          << "\n============================" << endl;

        typedef bdlb::Variant2<bdlt::Date, bdlt::DateTz>  DateOrDateTz;
        typedef bdlb::Variant2<bdlt::Time, bdlt::TimeTz>  TimeOrTimeTz;
        typedef bdlb::Variant2<bdlt::Datetime, bdlt::DatetimeTz>
                                                          DatetimeOrDatetimeTz;

        const struct {
            int        d_line;
            int        d_year;
            int        d_month;
            int        d_day;
            int        d_hour;
            int        d_minute;
            int        d_second;
            int        d_millisecond;
            int        d_offset;
        } DATA[] = {
            //Line Year   Mon  Day  Hour  Min  Sec     ms   offset
            //---- ----   ---  ---  ----  ---  ---     --   ------

            // Valid dates and times
            { L_,     1,   1,   1,    0,   0,   0,     0,        0 },
            { L_,  2005,   1,   1,    0,   0,   0,     0,      -90 },
            { L_,   123,   6,  15,   13,  40,  59,     0,     -240 },
            { L_,  1999,  10,  12,   23,   0,   1,     0,     -720 },

            // Vary milliseconds
            { L_,  1999,  10,  12,   23,   0,   1,     0,       90 },
            { L_,  1999,  10,  12,   23,   0,   1,   456,      240 },
            { L_,  1999,  10,  12,   23,   0,   1,   999,      720 },
            { L_,  1999,  12,  31,   23,  59,  59,   999,      720 }
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        const char *expectedDate[] = {
            "\"0001-01-01\"",
            "\"2005-01-01\"",
            "\"0123-06-15\"",
            "\"1999-10-12\"",
            "\"1999-10-12\"",
            "\"1999-10-12\"",
            "\"1999-10-12\"",
            "\"1999-12-31\""
        };

        const char *expectedDateTz[] = {
            "\"0001-01-01+00:00\"",
            "\"2005-01-01-01:30\"",
            "\"0123-06-15-04:00\"",
            "\"1999-10-12-12:00\"",
            "\"1999-10-12+01:30\"",
            "\"1999-10-12+04:00\"",
            "\"1999-10-12+12:00\"",
            "\"1999-12-31+12:00\""
        };

        const char *expectedTime[] = {
            "\"00:00:00.000\"",
            "\"00:00:00.000\"",
            "\"13:40:59.000\"",
            "\"23:00:01.000\"",
            "\"23:00:01.000\"",
            "\"23:00:01.456\"",
            "\"23:00:01.999\"",
            "\"23:59:59.999\""
        };

        const char *expectedTimeTz[] = {
            "\"00:00:00.000+00:00\"",
            "\"00:00:00.000-01:30\"",
            "\"13:40:59.000-04:00\"",
            "\"23:00:01.000-12:00\"",
            "\"23:00:01.000+01:30\"",
            "\"23:00:01.456+04:00\"",
            "\"23:00:01.999+12:00\"",
            "\"23:59:59.999+12:00\""
        };

        const char *expectedDatetime[] = {
            "\"0001-01-01T00:00:00.000\"",
            "\"2005-01-01T00:00:00.000\"",
            "\"0123-06-15T13:40:59.000\"",
            "\"1999-10-12T23:00:01.000\"",
            "\"1999-10-12T23:00:01.000\"",
            "\"1999-10-12T23:00:01.456\"",
            "\"1999-10-12T23:00:01.999\"",
            "\"1999-12-31T23:59:59.999\""
        };

        const char *expectedDatetimeTz[] = {
            "\"0001-01-01T00:00:00.000+00:00\"",
            "\"2005-01-01T00:00:00.000-01:30\"",
            "\"0123-06-15T13:40:59.000-04:00\"",
            "\"1999-10-12T23:00:01.000-12:00\"",
            "\"1999-10-12T23:00:01.000+01:30\"",
            "\"1999-10-12T23:00:01.456+04:00\"",
            "\"1999-10-12T23:00:01.999+12:00\"",
            "\"1999-12-31T23:59:59.999+12:00\""
        };

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int LINE        = DATA[ti].d_line;
            const int YEAR        = DATA[ti].d_year;
            const int MONTH       = DATA[ti].d_month;
            const int DAY         = DATA[ti].d_day;
            const int HOUR        = DATA[ti].d_hour;
            const int MINUTE      = DATA[ti].d_minute;
            const int SECOND      = DATA[ti].d_second;
            const int MILLISECOND = DATA[ti].d_millisecond;
            const int OFFSET      = DATA[ti].d_offset;;

            bdlt::Date theDate(YEAR, MONTH, DAY);
            bdlt::Time theTime(HOUR, MINUTE, SECOND, MILLISECOND);
            bdlt::Datetime theDatetime(YEAR, MONTH, DAY,
                                      HOUR, MINUTE, SECOND, MILLISECOND);

            bdlt::DateTz     theDateTz(theDate, OFFSET);
            bdlt::TimeTz     theTimeTz(theTime, OFFSET);
            bdlt::DatetimeTz theDatetimeTz(theDatetime, OFFSET);

            if (verbose) cout << "Encode Date" << endl;
            {
                const char *EXP = expectedDate[ti];

                bsl::ostringstream oss;
                ASSERTV(LINE, 0 == ImplUtil::encode(&oss, theDate));

                bsl::string result = oss.str();
                ASSERTV(LINE, result, EXP, result == EXP);
            }

            if (verbose) cout << "Encode DateTz" << endl;
            {
                const char *EXP = expectedDateTz[ti];

                bsl::ostringstream oss;
                ASSERTV(LINE, 0 == ImplUtil::encode(&oss, theDateTz));

                bsl::string result = oss.str();
                ASSERTV(LINE, result, EXP, result == EXP);
            }

            if (verbose) cout << "Encode Time" << endl;
            {
                const char *EXP = expectedTime[ti];

                bsl::ostringstream oss;
                ASSERTV(LINE, 0 == ImplUtil::encode(&oss, theTime));

                bsl::string result = oss.str();
                ASSERTV(LINE, result, EXP, result == EXP);
            }

            if (verbose) cout << "Encode TimeTz" << endl;
            {
                const char *EXP = expectedTimeTz[ti];

                bsl::ostringstream oss;
                ASSERTV(LINE, 0 == ImplUtil::encode(&oss, theTimeTz));

                bsl::string result = oss.str();
                ASSERTV(LINE, result, EXP, result == EXP);
            }

            if (verbose) cout << "Encode Datetime" << endl;
            {
                const char *EXP = expectedDatetime[ti];

                bsl::ostringstream oss;
                ASSERTV(LINE, 0 == ImplUtil::encode(&oss, theDatetime));

                bsl::string result = oss.str();
                ASSERTV(LINE, result, EXP, result == EXP);
            }

            if (verbose) cout << "Encode DatetimeTz" << endl;
            {
                const char *EXP = expectedDatetimeTz[ti];

                bsl::ostringstream oss;
                ASSERTV(LINE, 0 == ImplUtil::encode(&oss, theDatetimeTz));

                bsl::string result = oss.str();
                ASSERTV(LINE, result, EXP, result == EXP);
            }

            if (verbose) cout << "Encode DateOrDateTz" << endl;
            {
                const char *EXP_DATE   = expectedDate[ti];
                const char *EXP_DATETZ = expectedDateTz[ti];

                DateOrDateTz dateVariant(theDate);

                bsl::ostringstream ossDate;
                ASSERTV(LINE, 0 == ImplUtil::encode(&ossDate, dateVariant));

                bsl::string result = ossDate.str();
                ASSERTV(LINE, result, EXP_DATE, result == EXP_DATE);

                dateVariant = theDateTz;

                bsl::ostringstream ossDateTz;
                ASSERTV(LINE, 0 == ImplUtil::encode(&ossDateTz, dateVariant));

                result = ossDateTz.str();
                ASSERTV(LINE, result, EXP_DATETZ, result == EXP_DATETZ);
            }

            if (verbose) cout << "Encode TimeOrTimeTz" << endl;
            {
                const char *EXP_TIME   = expectedTime[ti];
                const char *EXP_TIMETZ = expectedTimeTz[ti];

                TimeOrTimeTz timeVariant(theTime);

                bsl::ostringstream ossTime;
                ASSERTV(LINE, 0 == ImplUtil::encode(&ossTime, timeVariant));

                bsl::string result = ossTime.str();
                ASSERTV(LINE, result, EXP_TIME, result == EXP_TIME);

                timeVariant = theTimeTz;

                bsl::ostringstream ossTimeTz;
                ASSERTV(LINE, 0 == ImplUtil::encode(&ossTimeTz, timeVariant));

                result = ossTimeTz.str();
                ASSERTV(LINE, result, EXP_TIMETZ, result == EXP_TIMETZ);
            }

            if (verbose) cout << "Encode DatetimeOrDatetimeTz" << endl;
            {
                const char *EXP_DATETIME   = expectedDatetime[ti];
                const char *EXP_DATETIMETZ = expectedDatetimeTz[ti];

                DatetimeOrDatetimeTz datetimeVariant(theDatetime);

                bsl::ostringstream ossDatetime;
                ASSERTV(LINE,
                        0 == ImplUtil::encode(&ossDatetime, datetimeVariant));

                bsl::string result = ossDatetime.str();
                ASSERTV(LINE, result, EXP_DATETIME, result == EXP_DATETIME);

                datetimeVariant = theDatetimeTz;

                bsl::ostringstream ossDatetimeTz;
                ASSERTV(LINE,
                        0 == ImplUtil::encode(&ossDatetimeTz,
                                              datetimeVariant));

                result = ossDatetimeTz.str();
                ASSERTV(LINE, result, EXP_DATETIMETZ,
                        result == EXP_DATETIMETZ);
            }
        }
        {
            bdlt::Datetime theDatetime(2010, 03, 15, 17, 23, 45, 123, 456);

            const char *expectedDatetime[] = {
                "\"2010-03-15T17:23:45\"",
                "\"2010-03-15T17:23:45.1\"",
                "\"2010-03-15T17:23:45.12\"",
                "\"2010-03-15T17:23:45.123\"",
                "\"2010-03-15T17:23:45.1234\"",
                "\"2010-03-15T17:23:45.12345\"",
                "\"2010-03-15T17:23:45.123456\"",
            };

            for (int pi = 0; pi <= 6; ++pi) {
                const char *EXP = expectedDatetime[pi];
                bsl::ostringstream oss;
                Options opt;
                opt.setDatetimeFractionalSecondPrecision(pi);
                ASSERTV(0 == ImplUtil::encode(&oss, theDatetime, opt));
                bsl::string result = oss.str();
                ASSERTV(pi, result, EXP, result == EXP);
            }
        }
        {
            bdlt::Datetime theDatetime(2010, 03, 15, 17, 23, 45, 123, 456);
            bdlt::DatetimeTz theDatetimeTz(theDatetime, -300);

            const char *expectedDatetimeTz[] = {
                "\"2010-03-15T17:23:45-05:00\"",
                "\"2010-03-15T17:23:45.1-05:00\"",
                "\"2010-03-15T17:23:45.12-05:00\"",
                "\"2010-03-15T17:23:45.123-05:00\"",
                "\"2010-03-15T17:23:45.1234-05:00\"",
                "\"2010-03-15T17:23:45.12345-05:00\"",
                "\"2010-03-15T17:23:45.123456-05:00\"",
            };

            for (int pi = 0; pi <= 6; ++pi) {
                const char *EXP = expectedDatetimeTz[pi];
                bsl::ostringstream oss;
                Options opt;
                opt.setDatetimeFractionalSecondPrecision(pi);
                ASSERTV(0 == ImplUtil::encode(&oss, theDatetimeTz, opt));
                bsl::string result = oss.str();
                ASSERTV(pi, result, EXP, result == EXP);
            }
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // ENCODING NUMBERS
        //
        // Concerns:
        // 1. Encoded numbers have the expected precisions.
        //
        // 2. Encoded numbers used default format.
        //
        // 3. Encoding `unsigned char` prints a number instead of string.
        //
        // Plan:
        // 1. Use the table-driven technique:
        //
        //   1. Specify a set of valid values, including those that will test
        //      the precision of the output.
        //
        //   2. Encode each value and verify the output is as expected.
        //
        // Testing:
        //   int encode(ostream& s, const TYPE& v, o);               // NUMBERS
        // --------------------------------------------------------------------

        if (verbose) cout << "\nENCODING NUMBERS"
                          << "\n================" << endl;

        if (verbose) cout << "Encode `double`" << endl;
        {
            typedef bsl::numeric_limits<double> Limits;

            const double negZero = -1 / Limits::infinity();

            const struct {
                int         d_line;
                double      d_value;
                const char *d_result;
            } DATA[] = {
                //LINE  VALUE                RESULT
                //----  ----------------     -------------------------
                { L_,    0.0,                    "0"                   },
                { L_,    1.0e-1,                 "0.1"                 },
                { L_,    0.125,                  "0.125"               },
                { L_,    1.0,                    "1"                   },
                { L_,    1.5,                    "1.5"                 },
                { L_,   10.0,                   "10"                   },
                { L_,    1.5e1,                 "15"                   },
                { L_,    9.9e100,                "9.9e+100"            },
                { L_,    3.14e300,               "3.14e+300"           },
                { L_,    2.23e-308,              "2.23e-308"           },
                { L_,    0.12345678912345,       "0.12345678912345"    },
                { L_,    0.12345678901234567,    "0.12345678901234566" },
                { L_,    0.123456789012345678,   "0.12345678901234568" },

                { L_, negZero,                  "-0"                   },
                { L_,   -1.0e-1,                "-0.1"                 },
                { L_,   -0.125,                 "-0.125"               },
                { L_,   -1.0,                   "-1"                   },
                { L_,   -1.5,                   "-1.5"                 },
                { L_,  -10.0,                  "-10"                   },
                { L_,   -1.5e1,                "-15"                   },
                { L_,   -9.9e100,               "-9.9e+100"            },
                { L_,   -3.14e300,              "-3.14e+300"           },
                { L_,   -2.23e-308,             "-2.23e-308"           },
                { L_,   -0.12345678912345,      "-0.12345678912345"    },
                { L_,   -0.12345678901234567,   "-0.12345678901234566" },
                { L_,   -0.123456789012345678,  "-0.12345678901234568" },

                // Small Integers
                { L_, 123456789012345.,           "123456789012345"    },
                { L_, 1234567890123456.,          "1234567890123456"   },

                // Full Mantissa Integers
                { L_, 1.0 * 0x1FFFFFFFFFFFFFull, "9007199254740991"     },
                { L_, 1.0 * 0x1FFFFFFFFFFFFFull  // This is also limits::max()
                      * (1ull << 63) * (1ull << 63) * (1ull << 63)
                      * (1ull << 63) * (1ull << 63) * (1ull << 63)
                      * (1ull << 63) * (1ull << 63) * (1ull << 63)
                      * (1ull << 63) * (1ull << 63) * (1ull << 63)
                      * (1ull << 63) * (1ull << 63) * (1ull << 63)
                      * (1ull << 26),         "1.7976931348623157e+308" },

                // Boundary Values
                { L_,  Limits::min(),         "2.2250738585072014e-308" },
                { L_,  Limits::denorm_min(),  "5e-324"                  },
                { L_,  Limits::max(),         "1.7976931348623157e+308" },
                { L_, -Limits::min(),        "-2.2250738585072014e-308" },
                { L_, -Limits::denorm_min(), "-5e-324"                  },
                { L_, -Limits::max(),        "-1.7976931348623157e+308" },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int         LINE      = DATA[ti].d_line;
                const double      VALUE     = DATA[ti].d_value;
                const char * const EXPECTED = DATA[ti].d_result;

                bsl::ostringstream oss;
                ASSERTV(LINE, 0 == ImplUtil::encode(&oss, VALUE));

                const bsl::string result = oss.str();
                ASSERTV(LINE, result, EXPECTED, result == EXPECTED);
            }
        }

        if (verbose) cout << "Encode invalid `double`" << endl;
        {
            bsl::ostringstream oss;

            oss.clear();
            ASSERTV(0 != ImplUtil::encode(
                             &oss, bsl::numeric_limits<double>::infinity()));

            oss.clear();
            ASSERTV(0 != ImplUtil::encode(
                             &oss, bsl::numeric_limits<double>::infinity()));

            oss.clear();
            ASSERTV(0 != ImplUtil::encode(
                             &oss, bsl::numeric_limits<double>::quiet_NaN()));

            oss.clear();
            ASSERTV(0 !=
                    ImplUtil::encode(
                        &oss, bsl::numeric_limits<double>::signaling_NaN()));
        }

        if (verbose) cout << "Encode `float`" << endl;
        {
            typedef bsl::numeric_limits<float> Limits;

            const float negZero = -1 / Limits::infinity();

            const struct {
                int         d_line;
                float       d_value;
                const char *d_result;
            } DATA[] = {
                //LINE        VALUE         RESULT
                //----  -------------  ---------------
                { L_,           0.0f,   "0"           },
                { L_,         0.125f,   "0.125"       },
                { L_,        1.0e-1f,   "0.1"         },
                { L_,      0.123456f,   "0.123456"    },
                { L_,           1.0f,   "1"           },
                { L_,           1.5f,   "1.5"         },
                { L_,          10.0f,  "10"           },
                { L_,         1.5e1f,  "15"           },
                { L_,   1.23456e-20f,   "1.23456e-20" },
                { L_,   0.123456789f,   "0.12345679"  },
                { L_,  0.1234567891f,   "0.12345679"  },

                { L_, negZero,         "-0"           },
                { L_,        -0.125f,  "-0.125"       },
                { L_,       -1.0e-1f,  "-0.1"         },
                { L_,     -0.123456f,  "-0.123456"    },
                { L_,          -1.0f,  "-1"           },
                { L_,          -1.5f,  "-1.5"         },
                { L_,         -10.0f, "-10"           },
                { L_,        -1.5e1f,  "-15"          },
                { L_,  -1.23456e-20f,  "-1.23456e-20" },
                { L_,  -0.123456789f,  "-0.12345679"  },
                { L_, -0.1234567891f,  "-0.12345679"  },

                // {DRQS 165162213} regression, 2^24 loses precision as float
                { L_, 1.0f * 0xFFFFFF,  "16777215"    },

                // Full Mantissa Integers
                { L_, 1.0f * 0xFFFFFF,  "16777215"        },
                { L_, 1.0f * 0xFFFFFF      // this happens to be also
                       * (1ull << 63)      // `Limits::max()`
                       * (1ull << 41),    "3.4028235e+38" },

                // Boundary Values
                { L_,  Limits::min(),         "1.1754944e-38" },
                { L_,  Limits::denorm_min(),  "1e-45"         },
                { L_,  Limits::max(),         "3.4028235e+38" },
                { L_, -Limits::min(),        "-1.1754944e-38" },
                { L_, -Limits::denorm_min(), "-1e-45"         },
                { L_, -Limits::max(),        "-3.4028235e+38" },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int          LINE     = DATA[ti].d_line;
                const float        VALUE    = DATA[ti].d_value;
                const char * const EXPECTED = DATA[ti].d_result;

                bsl::ostringstream oss;
                ASSERTV(LINE, 0 == ImplUtil::encode(&oss, VALUE));

                const bsl::string result = oss.str();
                ASSERTV(LINE, result, EXPECTED, result == EXPECTED);
            }
        }

        if (verbose) cout << "Encode `Decimal64`" << endl;
        {
#define DEC(X) BDLDFP_DECIMAL_DD(X)
            using bdldfp::Decimal64;

            typedef bsl::numeric_limits<Decimal64> Limits;

            const struct {
                int         d_line;
                Decimal64   d_value;
                const char *d_result;
            } DATA[] = {
                //LINE  VALUE       RESULT
                //----  -----       ------
                { L_,   DEC(0.0),    "\"0.0\""                    },
                { L_,   DEC(-0.0),  "\"-0.0\""                    },
                { L_,   DEC(1.13),   "\"1.13\""                   },

                { L_,   DEC(-9.876543210987654e307),
                                    "\"-9.876543210987654e+307\"" },
                { L_,   DEC(-9.8765432109876548e307),
                                    "\"-9.876543210987655e+307\"" },
                { L_,   DEC(-9.87654321098765482e307),
                                    "\"-9.876543210987655e+307\"" },

                { L_,    Limits::min(),         "\"1e-383\""                 },
                { L_,    Limits::denorm_min(),  "\"1e-398\""                 },
                { L_,    Limits::max(),         "\"9.999999999999999e+384\"" },
                { L_,   -Limits::min(),        "\"-1e-383\""                 },
                { L_,   -Limits::denorm_min(), "\"-1e-398\""                 },
                { L_,   -Limits::max(),        "\"-9.999999999999999e+384\"" },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int          LINE     = DATA[ti].d_line;
                const Decimal64    VALUE    = DATA[ti].d_value;
                const char * const EXPECTED = DATA[ti].d_result;

                bsl::ostringstream oss;
                ASSERTV(LINE, 0 == ImplUtil::encode(&oss, VALUE));

                const bsl::string result = oss.str();
                ASSERTV(LINE, result, EXPECTED, result == EXPECTED);
            }
#undef DEC
        }

        if (verbose) cout << "Encode integral types" << endl;
        {
            u::testNumber<short>();
            u::testNumber<int>();
            u::testNumber<Int64>();
            u::testNumber<unsigned char>();
            u::testNumber<unsigned short>();
            u::testNumber<unsigned int>();
            u::testNumber<Uint64>();
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // ENCODING STRINGS
        //
        // Concerns:
        // 1. Character are encoded as a single character string.
        //
        // 2. All escape characters are encoded corrected.
        //
        // 3. Control characters are encoded as hex.
        //
        // Plan:
        // 1. Using the table-driven technique:
        //
        //   1. Specify a set of values that include all escaped characters and
        //      some control characters.
        //
        //   2. Encode the value and verify the results.
        //
        // 2. Repeat for strings and Customized type.
        //
        // Testing:
        //  int encode(const bsl::string & value);
        //  int encode(const char *value);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nENCODING STRINGS"
                          << "\n================" << endl;

        if (verbose) cout << "Encode char *" << endl;
        {
            const struct {
                int         d_line;
                char        d_value;
                const char *d_result;
            } DATA[] = {
                //LINE    VAL     RESULT
                //----    ---     ------
                { L_,    '\0',      "0"     },
                { L_,    '\t',      "9"     },
                { L_,    '\n',     "10"     },
                { L_,    '\r',     "13"     },
                { L_,     ' ',     "32"     },
                { L_,     '!',     "33"     },
                { L_,     '"',     "34"     },
                { L_,     '/',     "47"     },
                { L_,     '0',     "48"     },
                { L_,     '9',     "57"     },
                { L_,     'A',     "65"     },
                { L_,     'Z',     "90"     },
                { L_,    '\\',     "92"     },
                { L_,     'a',     "97"     },
                { L_,     'z',    "122"     },
                { L_,     '~',    "126"     },

                { L_,  '\x7F',    "127"     },
                { L_,  '\x80',   "-128"     },
                { L_,  '\xff',     "-1"     },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int         LINE  = DATA[ti].d_line;
                const char        VALUE = DATA[ti].d_value;
                const char *const EXP   = DATA[ti].d_result;

                bsl::ostringstream oss;
                ASSERTV(LINE, 0 == ImplUtil::encode(&oss, VALUE));

                bsl::string result = oss.str();
                ASSERTV(LINE, result, EXP, result == EXP);
            }
        }

        if (verbose) cout << "Encode string" << endl;
        {
            const struct {
                int         d_line;
                const char *d_value;
                const char *d_result;
            } DATA[] = {
                //LINE    VAL  RESULT
                //----    ---  ------
                { L_,  "",     "\"\"" },
                { L_,  " ",    "\" \"" },
                { L_,  "~",    "\"~\"" },
                { L_,  "test", "\"test\"" },
                { L_,  "A quick brown fox jump over a lazy dog!",
                               "\"A quick brown fox jump over a lazy dog!\"" },
                { L_,  "\"",   "\"\\\"\"" },
                { L_,  "\\",   "\"\\\\\"" },
                { L_,  "/",    "\"\\/\"" },
                { L_,  "\b",   "\"\\b\"" },
                { L_,  "\f",   "\"\\f\"" },
                { L_,  "\n",   "\"\\n\"" },
                { L_,  "\r",   "\"\\r\"" },
                { L_,  "\t",   "\"\\t\"" },
                { L_,  "\x01", "\"\\u0001\"" },
                { L_,  "\x1f", "\"\\u001f\"" },
                { L_,  "\\/\b\f\n\r\t",   "\"\\\\\\/\\b\\f\\n\\r\\t\"" },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int         LINE  = DATA[ti].d_line;
                const char *const VALUE = DATA[ti].d_value;
                const char *const EXP   = DATA[ti].d_result;

                if (veryVeryVerbose) cout << "Test `char *`" << endl;
                {
                    bsl::ostringstream oss;
                    ASSERTV(LINE, 0 == ImplUtil::encode(&oss, VALUE));

                    bsl::string result = oss.str();
                    ASSERTV(LINE, result, EXP, result == EXP);
                }

                if (veryVeryVerbose) cout << "Test `string`" << endl;
                {
                    bsl::ostringstream oss;
                    ASSERTV(LINE,
                            0 == ImplUtil::encode(&oss, bsl::string(VALUE)));

                    bsl::string result = oss.str();
                    ASSERTV(LINE, result, EXP, result == EXP);
                }

                if (veryVeryVerbose) cout << "Test Customized" << endl;
                {
                    bsl::ostringstream oss;
                    balb::CustomString str;
                    if (0 == str.fromString(VALUE)) {
                        ASSERTV(LINE, 0 == ImplUtil::encode(&oss, str));

                        bsl::string result = oss.str();
                        ASSERTV(LINE, result, EXP, result == EXP);
                    }
                }
            }
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // ENCODING BOOLEAN
        //
        // Concerns:
        // 1. `true` is encoded into "true" and `false` is encoded into
        //    "false".
        //
        // Plan:
        // 1. Use a brute force approach to test both cases.
        //
        // Testing:
        //   int encode(const bool& value);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nENCODING BOOLEAN"
                          << "\n================" << endl;

        if (verbose) cout << "Encode `true`" << endl;
        {
            bsl::ostringstream oss;
            ASSERTV(0 == ImplUtil::encode(&oss, true));

            bsl::string result = oss.str();
            ASSERTV(result, result == "true");
        }

        if (verbose) cout << "Encode `false`" << endl;
        {
            bsl::ostringstream oss;
            ASSERTV(0 == ImplUtil::encode(&oss, false));

            bsl::string result = oss.str();
            ASSERTV(result, result == "false");
        }
      } break;
      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      }
    }

    dumpCounts(test);

    if (testStatus > 0) {
        cerr << "Error, non-zero test status = " << testStatus << "." << endl;
    }

    return testStatus;
}
