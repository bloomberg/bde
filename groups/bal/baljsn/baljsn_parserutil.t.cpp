// baljsn_parserutil.t.cpp                                            -*-C++-*-
#include <baljsn_parserutil.h>

#include <baljsn_printutil.h> // For round-trip testing

#include <bslim_testutil.h>

#include <bdlt_date.h>
#include <bdlt_datetime.h>
#include <bdlt_datetimetz.h>
#include <bdlt_datetz.h>
#include <bdlt_timetz.h>

#include <bsl_sstream.h>
#include <bsl_cfloat.h>
#include <bsl_climits.h>
#include <bsl_cstdlib.h>
#include <bsl_cstring.h>
#include <bsl_limits.h>
#include <bsl_iostream.h>

#include <bsl_string.h>
#include <bsl_cstring.h>

#include <bdlb_printmethods.h>
#include <bdlb_float.h>

#include <bdlsb_memoutstreambuf.h>
#include <bdlsb_fixedmemoutstreambuf.h>
#include <bdlsb_fixedmeminstreambuf.h>

#include <bdldfp_decimalutil.h>

#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>


using namespace BloombergLP;
using namespace bsl;
using bsl::cout;
using bsl::cerr;
using bsl::endl;

// ============================================================================
//                             TEST PLAN
// ----------------------------------------------------------------------------
//                             Overview
//                             --------
// The component under test implements a utility for parsing 'bdeat' compatible
// simple types from a 'bsl::string_view'.  The parsing is done via overloaded
// 'getValue' functions that are provided for fundamental types and 'bdlt'
// types.  Since the functions are independent and do not share any state we
// will test them independently.
//
// We use standard table-based approach to testing where we put both input and
// expected output in the same table row and verify that the actual result
// matches the expected value.
// ----------------------------------------------------------------------------
// CLASS METHODS
// [ 2] static int getValue(bool                 *v, bsl::string_view s);
// [ 3] static int getValue(char                 *v, bsl::string_view s);
// [ 3] static int getValue(signed char          *v, bsl::string_view s);
// [ 4] static int getValue(unsigned char        *v, bsl::string_view s);
// [ 5] static int getValue(short                *v, bsl::string_view s);
// [ 6] static int getValue(unsigned short       *v, bsl::string_view s);
// [ 7] static int getValue(int                  *v, bsl::string_view s);
// [ 8] static int getValue(unsigned int         *v, bsl::string_view s);
// [ 9] static int getValue(bsls::Types::Int64   *v, bsl::string_view s);
// [10] static int getValue(bsls::Types::Uint64  *v, bsl::string_view s);
// [11] static int getValue(float                *v, bsl::string_view s);
// [12] static int getValue(double               *v, bsl::string_view s);
// [13] static int getQuotedString(bsl::string   *v, bsl::string_view s);
// [13] static int getUnquotedString(bsl::string *v, bsl::string_view s);
// [13] static int getValue(bsl::string          *v, bsl::string_view s);
// [14] static int getValue(bdlt::Time           *v, bsl::string_view s);
// [15] static int getValue(bdlt::TimeTz         *v, bsl::string_view s);
// [16] static int getValue(bdlt::Date           *v, bsl::string_view s);
// [17] static int getValue(bdlt::DateTz         *v, bsl::string_view s);
// [18] static int getValue(bdlt::Datetime       *v, bsl::string_view s);
// [19] static int getValue(bdlt::DatetimeTz     *v, bsl::string_view s);
// [26] static int getValue(TimeOrTimeTz         *v, bsl::string_view s);
// [27] static int getValue(DateOrDateTz         *v, bsl::string_view s);
// [28] static int getValue(DatetimeOrDatetimeTz *v, bsl::string_view s);
// [20] static int getValue(vector<char>         *v, bsl::string_view s);
// [21] static int getValue(bdldfp::Decimal64    *v, bsl::string_view s);
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
//
// [22] BOOLEAN ROUND-TRIP
// [23] STRINGS ROUND-TRIP
// [24] NUMBERS ROUND-TRIP
// [25] DATE AND TIME TYPES ROUND-TRIP
//
// [29] USAGE EXAMPLE

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
//                   GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------

typedef baljsn::ParserUtil Util;

typedef baljsn::PrintUtil  Print;

typedef bsls::Types::Int64  Int64;
typedef bsls::Types::Uint64 Uint64;

// ============================================================================
//                              TEST MACHINERY
// ----------------------------------------------------------------------------

bool areBuffersEqual(const char *lhs, const char *rhs)
    // Compare the data written to the specified 'lhs' with the data in the
    // specified 'rhs' ignoring whitespace in the 'lhs'.  Return 'true' if they
    // are equal, and 'false' otherwise.
{
    while (*lhs) {
        if (' ' == *lhs) {
            ++lhs;
            continue;
        }
        if (*lhs != *rhs) {
           return false;                                              // RETURN
        }
        ++lhs;
        ++rhs;
    }
    return true;
}

template <class FLOAT_TYPE>
void roundTripTestNonNumericValues()
{
    typedef FLOAT_TYPE Type;

    const Type NAN_P = bsl::numeric_limits<Type>::quiet_NaN();
    // Negative NaN does not print for any floating point type, so we
    // don't test it for round-trip (on purpose).
    //const Type NAN_N = -NAN_P;
    const Type INF_P = bsl::numeric_limits<Type>::infinity();
    const Type INF_N = -INF_P;

    const struct {
        int         d_line;
        Type        d_value;
    } DATA[] = {
        //---------------
        // LINE | VALUE |
        //---------------
        { L_,    NAN_P },
        // Negative NaN does not print for any floating point type,
        // so we don't test it for round-trip (on purpose).
        //{ L_,    NAN_N },
        { L_,    INF_P },
        { L_,    INF_N },
    };
    const int NUM_DATA = sizeof DATA / sizeof *DATA;

    for (int ti = 0; ti < NUM_DATA; ++ti) {
        const int  LINE  = DATA[ti].d_line;
        const Type VALUE = DATA[ti].d_value;

        baljsn::EncoderOptions options;
        options.setEncodeInfAndNaNAsStrings(true);
        bsl::ostringstream oss;
        ASSERTV(LINE, 0 == Print::printValue(oss, VALUE, &options));

        bsl::string result(oss.str());
        Type decoded;
        ASSERTV(LINE, 0 == Util::getValue(&decoded, result));
        if (VALUE != VALUE) { // A NaN
            ASSERTV(LINE, VALUE, result, decoded, decoded != decoded);
        }
        else {
            ASSERTV(LINE, VALUE, result, decoded, VALUE == decoded);
        }
        // We also use 'memcmp' to ensure that we really get back the
        // same binary IEEE-754.
        ASSERTV(LINE, VALUE, result, decoded,
                0 == bsl::memcmp(&VALUE, &decoded, sizeof VALUE));

    }
}

template <class TYPE>
bool canRepresentImpl(Int64 VALUE, bsl::true_type)  // a signed type
{
    typedef bsl::numeric_limits<TYPE> Limits;

    const TYPE TYPE_MAX  = Limits::max();
    const TYPE TYPE_MIN  = Limits::min();

    return VALUE >= TYPE_MIN && VALUE <= TYPE_MAX;
}

template <class TYPE>
bool canRepresentImpl(Int64 VALUE, bsl::false_type)  // an unsigned type
{
    if (VALUE < 0) {
        return false;                                                 // RETURN
    }

    return static_cast<Uint64>(VALUE) <= bsl::numeric_limits<TYPE>::max();
}

template <class TYPE>
bool canRepresent(Int64 VALUE)
{
    // Unfortunately at the moment 'bsl::is_signed' is not available on C++03
    return canRepresentImpl<TYPE>(
               VALUE,
               bsl::integral_constant<bool,
                                      bsl::numeric_limits<TYPE>::is_signed>());
}

template <class TYPE>
void testIntegerTypeRoundTrip()
{
    const struct {
        int         d_line;
        Int64       d_value;
    } DATA[] = {
        //LINE      VALUE
        //----  ---------
        { L_,          -1 },
        { L_,           0 },
        { L_,           1 },
        { L_,    SHRT_MIN },
        { L_,    SHRT_MAX },
        { L_,   USHRT_MAX },
        { L_,     INT_MIN },
        { L_,     INT_MAX },
        { L_,    UINT_MAX },
        { L_,   LLONG_MIN },
        { L_,   LLONG_MAX }
    };
    const int NUM_DATA = sizeof DATA / sizeof *DATA;

    for (int ti = 0; ti < NUM_DATA; ++ti) {
        const int   LINE  = DATA[ti].d_line;
        const Int64 VAL64 = DATA[ti].d_value;
        const TYPE  VALUE = (TYPE)VAL64;

        if (false == canRepresent<TYPE>(VAL64)) {
            continue;                                               // CONTINUE
        }

        bsl::ostringstream oss;
        ASSERTV(LINE, 0 == Print::printValue(oss, VALUE));

        bsl::string result(oss.str());

        TYPE decoded = 0;
        ASSERTV(LINE, 0 == Util::getValue(&decoded, result));
        ASSERTV(LINE, VALUE, result, decoded, VALUE == decoded);

        // Verify that default options are the same as no options
        const baljsn::EncoderOptions defaultOptions;

        oss.str("");
        ASSERTV(LINE, 0 == Print::printValue(oss, VALUE, &defaultOptions));

        result = oss.str();
        ASSERTV(LINE, 0 == Util::getValue(&decoded, result));
        ASSERTV(LINE, VALUE, result, decoded, VALUE == decoded);
    }
}

// ============================================================================
//                               MAIN PROGRAM
// ----------------------------------------------------------------------------

typedef bsls::Types::Int64  Int64;
typedef bsls::Types::Uint64 Uint64;

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;

    bool verbose         = argc > 2;
    bool veryVerbose     = argc > 3;
    bool veryVeryVerbose = argc > 4;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    bslma::TestAllocator globalAllocator("global", veryVeryVerbose);
    bslma::Default::setGlobalAllocator(&globalAllocator);

    switch (test) { case 0:  // Zero is always the leading case.
      case 29: {
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

        if (verbose) cout << "\nUSAGE EXAMPLE"
                          << "\n=============" << endl;

///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Decoding into a Simple 'struct' from JSON data
///---------------------------------------------------------
// Suppose we want to de-serialize some JSON data into an object.
//
// First, we define a struct, 'Employee', to contain the data:
//..
    struct Employee {
        bsl::string d_name;
        bdlt::Date   d_date;
        int         d_age;
    };
//..
// Then, we create an 'Employee' object:
//..
    Employee employee;
//..
// Next, we specify the string values in JSON format used to represent the
// object data.  Note that the birth date is specified in the ISO 8601 format:
//..
    const char *name = "\"John Smith\"";
    const char *date = "\"1985-06-24\"";
    const char *age  = "21";

    const bsl::string_view nameRef(name);
    const bsl::string_view dateRef(date);
    const bsl::string_view ageRef(age);
//..
// Now, we use the created string refs to populate the employee object:
//..
    ASSERT(0 == baljsn::ParserUtil::getValue(&employee.d_name, nameRef));
    ASSERT(0 == baljsn::ParserUtil::getValue(&employee.d_date, dateRef));
    ASSERT(0 == baljsn::ParserUtil::getValue(&employee.d_age, ageRef));
//..
// Finally, we will verify that the values are as expected:
//..
    ASSERT("John Smith"            == employee.d_name);
    ASSERT(bdlt::Date(1985, 06, 24) == employee.d_date);
    ASSERT(21                      == employee.d_age);
//..
      } break;
      case 28: {
        // --------------------------------------------------------------------
        // TESTING 'getValue' for 'bdlt::DatetimeOrDatetimeTz'
        //
        // Concerns:
        //: 1 Values in the valid range, including the maximum and minimum
        //:   values for this type, are parsed correctly.
        //:
        //: 2 The function fails and the passed in variable remains unmodified
        //:   if the data is not valid.
        //:
        //: 3 The return code is 0 on success and non-zero on failure.
        //
        // Plan:
        //: 1 Using the table-driven technique, specify a set of distinct
        //:   rows of string value, expected parsed value, and return code.
        //:
        //: 2 For each row in the table of P-1:
        //:
        //:   1 Provide the string value and a variable  to be parsed into to
        //:     the 'getValue' function.  Invoke the function three times,
        //:     leaving the variable unset, then assigning a sentinel value of
        //:     type "bdlt::Datetime' to the variable and finally assigning a
        //:     a sentinel value of type 'bdlt::DatetimeTz'.
        //:
        //:   2 If the parsing should succeed then verify that the variable
        //:     value matches the expected value.  Otherwise confirm that the
        //:     variable value is unmodified.  (C-1..2)
        //:
        //:   3 Confirm that the return code is 0 on success and non-zero
        //:     otherwise.  (C-3)
        //
        // Testing:
        //   static int getValue(DatetimeOrDatetimeTz *v, bsl::string_view s);
        // --------------------------------------------------------------------

        if (verbose)
            cout << "\nTESTING 'getValue' for 'bdlt::DatetimeOrDatetimeTz'"
                 << "\n==================================================="
                 << endl;
        {
            static const struct {
                int         d_line;          // source line number

                const char *d_input_p;       // input

                int         d_year;          // year under test

                int         d_month;         // month under test

                int         d_day;           // day under test

                int         d_hour;          // hour under test

                int         d_minutes;       // minutes under test

                int         d_seconds;       // seconds under test

                int         d_milliSecs;     // milli seconds under test

                int         d_microSecs;     // micro seconds under test

                int         d_tzoffset;      // time zone offset

                bool        d_isValid;       // isValid flag

                bool        d_isDatetimeTz;  // flag indicating whether the
                                             // result object is expected to
                                             // contain 'DatetimeTz' or
                                             // 'Datetime' object
            } DATA[] = {
//LINE  INPUT
//----  --------------------------------------------
//                    Y     M   D   HH  MM  SS   MS  US   OFF   VALID  TZ
//                    ----  --  --  --  --  --   --  --   ----- -----  -----
{ L_, "\"0001-01-01T00:00:00.000\"",
                         1,  1,  1,  0,  0,  0,  0,   0,     0,  true, false },
{ L_, "\"0001-01-01T00:00:00.000000\"",
                         1,  1,  1,  0,  0,  0,  0,   0,     0,  true, false },

{ L_, "\"0001-01-01T01:01:01.001\"",
                         1,  1,  1,  1,  1,  1,  1,   0,     0,  true, false },
{ L_, "\"0001-01-01T01:01:01.001000\"",
                         1,  1,  1,  1,  1,  1,  1,   0,     0,  true, false },
{ L_, "\"0001-01-01T01:23:59.059\"",
                         1,  1,  1,  1, 23, 59, 59,   0,     0,  true, false },
{ L_, "\"0001-01-01T01:23:59.059059\"",
                         1,  1,  1,  1, 23, 59, 59,  59,     0,  true, false },
{ L_, "\"0001-01-02T00:00:00.000\"",
                         1,  1,  2,  0,  0,  0,  0,   0,     0,  true, false },
{ L_, "\"0001-01-02T00:00:00.000000\"",
                         1,  1,  2,  0,  0,  0,  0,   0,     0,  true, false },
{ L_, "\"0001-01-02T01:01:01.001\"",
                         1,  1,  2,  1,  1,  1,  1,   0,     0,  true, false },
{ L_, "\"0001-01-02T01:01:01.001000\"",
                         1,  1,  2,  1,  1,  1,  1,   0,     0,  true, false },

{ L_, "\"0001-01-02T01:23:59.059\"",
                         1,  1,  2,  1, 23, 59, 59,   0,     0,  true, false },
{ L_, "\"0001-01-02T01:23:59.059501\"",
                         1,  1,  2,  1, 23, 59, 59, 501,     0,  true, false },

{ L_, "\"0001-02-01T23:59:59.000\"",
                         1,  2,  1, 23, 59, 59,  0,   0,     0,  true, false },
{ L_, "\"0001-02-01T23:59:59.000001\"",
                         1,  2,  1, 23, 59, 59,  0,   1,     1,  true, false },

{ L_, "\"0001-12-31T00:00:00.000\"",
                         1, 12, 31,  0,  0,  0,  0,   0,     0,  true, false },
{ L_, "\"0001-12-31T23:59:59.000\"",
                         1, 12, 31, 23, 59, 59,  0,   0,     0,  true, false },


{ L_, "\"0002-01-01T00:00:00.000000\"",
                         2,  1,  1,  0,  0,  0,  0,   0,     0,  true, false },
{ L_, "\"0002-01-01T23:59:59.000000\"",
                         2,  1,  1, 23, 59, 59,  0,   0,     0,  true, false },

{ L_, "\"0004-02-28T00:00:00.000000\"",
                         4,  2, 28,  0,  0,  0,  0,   0,     0,  true, false },
{ L_, "\"0004-02-28T23:59:59.000000\"",
                         4,  2, 28, 23, 59, 59,  0,   0,     0,  true, false },
{ L_, "\"0004-02-28T23:59:59.000000\"",
                         4,  2, 28, 23, 59, 59,  0,   0,     0,  true, false },

{ L_, "\"0004-02-29T00:00:00.000000\"",
                         4,  2, 29,  0,  0,  0,  0,   0,     0,  true, false },
{ L_, "\"0004-02-29T23:59:59.000000\"",
                         4,  2, 29, 23, 59, 59,  0,   0,     0,  true, false },

{ L_, "\"0004-03-01T00:00:00.000000\"",
                         4,  3,  1,  0,  0,  0,  0,   0,     0,  true, false },
{ L_, "\"0004-03-01T23:59:59.000000\"",
                         4,  3,  1, 23, 59, 59,  0,   0,     0,  true, false },
{ L_, "\"0004-03-01T23:59:59.000000\"",
                         4,  3,  1, 23, 59, 59,  0,   0,     0,  true, false },

{ L_, "\"0008-02-28T00:00:00.000000\"",
                         8,  2, 28,  0,  0,  0,  0,   0,     0,  true, false },
{ L_, "\"0008-02-28T23:59:59.000000\"",
                         8,  2, 28, 23, 59, 59,  0,   0,     0,  true, false },

{ L_, "\"0008-02-29T00:00:00.000000\"",
                         8,  2, 29,  0,  0,  0,  0,   0,     0,  true, false },
{ L_, "\"0008-02-29T23:59:59.000000\"",
                         8,  2, 29, 23, 59, 59,  0,   0,     0,  true, false },

{ L_, "\"0008-03-01T00:00:00.000000\"",
                         8,  3,  1,  0,  0,  0,  0,   0,     0,  true, false },
{ L_, "\"0008-03-01T23:59:59.000000\"",
                         8,  3,  1, 23, 59, 59,  0,   0,     0,  true, false },

{ L_, "\"0100-02-28T00:00:00.000000\"",
                       100,  2, 28,  0,  0,  0,  0,   0,     0,  true, false },
{ L_, "\"0100-02-28T23:59:59.000000\"",
                       100,  2, 28, 23, 59, 59,  0,   0,     0,  true, false },
{ L_, "\"0100-02-28T23:59:59.000000\"",
                       100,  2, 28, 23, 59, 59,  0,   0,     0,  true, false },

{ L_, "\"0100-03-01T00:00:00.000000\"",
                       100,  3,  1,  0,  0,  0,  0,   0,     0,  true, false },
{ L_, "\"0100-03-01T23:59:59.000000\"",
                       100,  3,  1, 23, 59, 59,  0,   0,     0,  true, false },
{ L_, "\"0100-03-01T23:59:59.000000\"",
                       100,  3,  1, 23, 59, 59,  0,   0,     0,  true, false },

{ L_, "\"0400-02-28T00:00:00.000000\"",
                       400,  2, 28,  0,  0,  0,  0,   0,     0,  true, false },
{ L_, "\"0400-02-28T23:59:59.000000\"",
                       400,  2, 28, 23, 59, 59,  0,   0,     0,  true, false },
{ L_, "\"0400-02-28T23:59:59.000000\"",
                       400,  2, 28, 23, 59, 59,  0,   0,     0,  true, false },

{ L_, "\"0400-02-29T00:00:00.000000\"",
                       400,  2, 29,  0,  0,  0,  0,   0,     0,  true, false },
{ L_, "\"0400-02-29T23:59:59.000000\"",
                       400,  2, 29, 23, 59, 59,  0,   0,     0,  true, false },
{ L_, "\"0400-02-29T23:59:59.000000\"",
                       400,  2, 29, 23, 59, 59,  0,   0,     0,  true, false },

{ L_, "\"0400-03-01T00:00:00.000000\"",
                       400,  3,  1,  0,  0,  0,  0,   0,     0,  true, false },
{ L_, "\"0400-03-01T23:59:59.000000\"",
                       400,  3,  1, 23, 59, 59,  0,   0,     0,  true, false },
{ L_, "\"0400-03-01T23:59:59.000000\"",
                       400,  3,  1, 23, 59, 59,  0,   0,     0,  true, false },

{ L_, "\"0500-02-28T00:00:00.000000\"",
                       500,  2, 28,  0,  0,  0,  0,   0,     0,  true, false },
{ L_, "\"0500-02-28T23:59:59.000000\"",
                       500,  2, 28, 23, 59, 59,  0,   0,     0,  true, false },

{ L_, "\"0500-03-01T00:00:00.000000\"",
                      500,  3,  1,  0,  0,  0,  0,   0,     0,  true, false },
{ L_, "\"0500-03-01T23:59:59.000000\"",
                       500,  3,  1, 23, 59, 59,  0,   0,     0,  true, false },

{ L_, "\"0800-02-28T00:00:00.000000\"",
                       800,  2, 28,  0,  0,  0,  0,   0,     0,  true, false },
{ L_, "\"0800-02-28T23:59:59.000000\"",
                       800,  2, 28, 23, 59, 59,  0,   0,     0,  true, false },

{ L_, "\"0800-02-29T00:00:00.000000\"",
                       800,  2, 29,  0,  0,  0,  0,   0,     0,  true, false },
{ L_, "\"0800-02-29T23:59:59.000000\"",
                       800,  2, 29, 23, 59, 59,  0,   0,     0,  true, false },

{ L_, "\"0800-03-01T00:00:00.000000\"",
                       800,  3,  1,  0,  0,  0,  0,   0,     0,  true, false },
{ L_, "\"0800-03-01T23:59:59.000000\"",
                       800,  3,  1, 23, 59, 59,  0,   0,     0,  true, false },

{ L_, "\"1000-02-28T00:00:00.000000\"",
                      1000,  2, 28,  0,  0,  0,  0,   0,     0,  true, false },
{ L_, "\"1000-02-28T23:59:59.000000\"",
                      1000,  2, 28, 23, 59, 59,  0,   0,     0,  true, false },

{ L_, "\"1000-03-01T00:00:00.000000\"",
                      1000,  3,  1,  0,  0,  0,  0,   0,     0,  true, false },
{ L_, "\"1000-03-01T23:59:59.000000\"",
                      1000,  3,  1, 23, 59, 59,  0,   0,     0,  true, false },

{ L_, "\"2000-02-28T00:00:00.000000\"",
                      2000,  2, 28,  0,  0,  0,  0,   0,     0,  true, false },
{ L_, "\"2000-02-28T23:59:59.000000\"",
                      2000,  2, 28, 23, 59, 59,  0,   0,     0,  true, false },

{ L_, "\"2000-02-29T00:00:00.000000\"",
                      2000,  2, 29,  0,  0,  0,  0,   0,     0,  true, false },
{ L_, "\"2000-02-29T23:59:59.000000\"",
                      2000,  2, 29, 23, 59, 59,  0,   0,     0,  true, false },

{ L_, "\"2000-03-01T00:00:00.000000\"",
                      2000,  3,  1,  0,  0,  0,  0,   0,     0,  true, false },
{ L_, "\"2000-03-01T23:59:59.000000\"",
                      2000,  3,  1, 23, 59, 59,  0,   0,     0,  true, false },

{ L_, "\"2016-12-31T00:00:00.000000\"",
                      2016, 12, 31,  0,  0,  0,  0,   0,     0,  true, false },
{ L_, "\"2017-12-31T00:00:00.000000\"",
                      2017, 12, 31,  0,  0,  0,  0,   0,     0,  true, false },
{ L_, "\"2018-12-31T00:00:00.000000\"",
                      2018, 12, 31,  0,  0,  0,  0,   0,     0,  true, false },
{ L_, "\"2019-12-31T00:00:00.000000\"",
                      2019, 12, 31,  0,  0,  0,  0,   0,     0,  true, false },

{ L_, "\"2020-01-01T00:00:00.000000\"",
                      2020,  1,  1,  0,  0,  0,  0,   0,     0,  true, false },
{ L_, "\"2020-01-01T00:00:00.000000\"",
                      2020,  1,  1,  0,  0,  0,  0,   0,     0,  true, false },
{ L_, "\"2020-01-01T00:00:00.000000\"",
                      2020,  1,  1,  0,  0,  0,  0,   0,     0,  true, false },

{ L_, "\"2020-01-01T23:59:59.999999\"",
                      2020,  1,  1, 23, 59, 59,999, 999,     0,  true, false },
{ L_, "\"2020-01-01T23:59:59.999999\"",
                      2020,  1,  1, 23, 59, 59,999, 999,     0,  true, false },
{ L_, "\"2020-01-01T23:59:59.999999\"",
                      2020,  1,  1, 23, 59, 59,999, 999,     0,  true, false },

{ L_, "\"2020-01-02T00:00:00.000000\"",
                      2020,  1,  2,  0,  0,  0,  0,   0,     0,  true, false },
{ L_, "\"2020-01-02T00:00:00.000000\"",
                      2020,  1,  2,  0,  0,  0,  0,   0,     0,  true, false },
{ L_, "\"2020-01-02T00:00:00.000000\"",
                      2020,  1,  2,  0,  0,  0,  0,   0,     0,  true, false },


{ L_, "\"2020-02-28T00:00:00.000000\"",
                      2020,  2, 28,  0,  0,  0,  0,   0,     0,  true, false },
{ L_, "\"2020-02-28T23:59:59.000000\"",
                      2020,  2, 28, 23, 59, 59,  0,   0,     0,  true, false },
{ L_, "\"2020-02-28T23:59:59.000000\"",
                      2020,  2, 28, 23, 59, 59,  0,   0,     0,  true, false },

{ L_, "\"2020-02-29T00:00:00.000000\"",
                      2020,  2, 29,  0,  0,  0,  0,   0,     0,  true, false },
{ L_, "\"2020-02-29T23:59:59.000000\"",
                      2020,  2, 29, 23, 59, 59,  0,   0,     0,  true, false },
{ L_, "\"2020-02-29T23:59:59.000000\"",
                      2020,  2, 29, 23, 59, 59,  0,   0,     0,  true, false },

{ L_, "\"2020-03-01T00:00:00.000000\"",
                      2020,  3,  1,  0,  0,  0,  0,   0,     0,  true, false },
{ L_, "\"2020-03-01T23:59:59.000000\"",
                      2020,  3,  1, 23, 59, 59,  0,   0,     0,  true, false },
{ L_, "\"2020-03-01T23:59:59.000000\"",
                      2020,  3,  1, 23, 59, 59,  0,   0,     0,  true, false },

{ L_, "\"2021-01-02T00:00:00.000000\"",
                      2021,  1,  2,  0,  0,  0,  0,   0,     0,  true, false },
{ L_, "\"2022-01-02T00:00:00.000000\"",
                      2022,  1,  2,  0,  0,  0,  0,   0,     0,  true, false },

{ L_, "\"9999-02-28T00:00:00.000000\"",
                      9999,  2, 28,  0,  0,  0,  0,   0,     0,  true, false },
{ L_, "\"9999-02-28T23:59:59.000000\"",
                      9999,  2, 28, 23, 59, 59,  0,   0,     0,  true, false },
{ L_, "\"9999-02-28T23:59:59.000000\"",
                      9999,  2, 28, 23, 59, 59,  0,   0,     0,  true, false },

{ L_, "\"9999-03-01T00:00:00.000000\"",
                      9999,  3,  1,  0,  0,  0,  0,   0,     0,  true, false },
{ L_, "\"9999-03-01T23:59:59.000000\"",
                      9999,  3,  1, 23, 59, 59,  0,   0,     0,  true, false },
{ L_, "\"9999-03-01T23:59:59.000000\"",
                      9999,  3,  1, 23, 59, 59,  0,   0,     0,  true, false },

{ L_, "\"9999-12-30T00:00:00.000000\"",
                      9999, 12, 30,  0,  0,  0,  0,   0,     0,  true, false },
{ L_, "\"9999-12-30T23:59:59.000000\"",
                      9999, 12, 30, 23, 59, 59,  0,   0,     0,  true, false },

{ L_, "\"9999-12-31T00:00:00.000000\"",
                      9999, 12, 31,  0,  0,  0,  0,   0,     0,  true, false },
{ L_, "\"9999-12-31T23:59:59.000000\"",
                      9999, 12, 31, 23, 59, 59,  0,   0,     0,  true, false },

{ L_, "\"0001-01-01T00:00:00.000Z\"",
                         1,  1,  1,  0,  0,  0,  0,   0,     0,  true,  true },
 { L_, "\"0001-01-01T00:00:00.000+00:00\"",
                         1,  1,  1,  0,  0,  0,  0,   0,     0,  true,  true },
{ L_, "\"0001-01-01T00:00:00.000+00:45\"",
                         1,  1,  1,  0,  0,  0,  0,   0,    45,  true,  true },
{ L_, "\"0001-01-01T00:00:00.000-23:59\"",
                         1,  1,  1,  0,  0,  0,  0,   0, -1439,  true,  true },
{ L_, "\"0001-01-01T00:00:00.000000Z\"",
                         1,  1,  1,  0,  0,  0,  0,   0,     0,  true,  true },
{ L_, "\"0001-01-01T00:00:00.000000+00:00\"",
                         1,  1,  1,  0,  0,  0,  0,   0,     0,  true,  true },
{ L_, "\"0001-01-01T00:00:00.000000+00:45\"",
                         1,  1,  1,  0,  0,  0,  0,   0,    45,  true,  true },
{ L_, "\"0001-01-01T00:00:00.000000-23:59\"",
                         1,  1,  1,  0,  0,  0,  0,   0, -1439,  true,  true },

{ L_, "\"0001-01-01T01:01:01.001z\"",
                         1,  1,  1,  1,  1,  1,  1,   0,     0,  true,  true },
{ L_, "\"0001-01-01T01:01:01.001+00:00\"",
                         1,  1,  1,  1,  1,  1,  1,   0,     0,  true,  true },
{ L_, "\"0001-01-01T01:01:01.001+08:20\"",
                         1,  1,  1,  1,  1,  1,  1,   0,   500,  true,  true },
{ L_, "\"0001-01-01T01:01:01.001000z\"",
                         1,  1,  1,  1,  1,  1,  1,   0,     0,  true,  true },
{ L_, "\"0001-01-01T01:01:01.001000+00:00\"",
                         1,  1,  1,  1,  1,  1,  1,   0,     0,  true,  true },
{ L_, "\"0001-01-01T01:01:01.001000+08:20\"",
                         1,  1,  1,  1,  1,  1,  1,   0,   500,  true,  true },
{ L_, "\"0001-01-01T00:00:00.000000-23:59\"",
                         1,  1,  1,  0,  0,  0,  0,   0, -1439,  true,  true },
{ L_, "\"0001-01-01T01:23:59.059059+00:00\"",
                         1,  1,  1,  1, 23, 59, 59,  59,     0,  true,  true },
{ L_, "\"0001-01-01T01:23:59.059059+23:59\"",
                         1,  1,  1,  1, 23, 59, 59,  59,  1439,  true,  true },
{ L_, "\"0001-01-01T01:23:59.059059-23:59\"",
                         1,  1,  1,  1, 23, 59, 59,  59, -1439,  true,  true },
{ L_, "\"0001-01-02T00:00:00.000000+00:00\"",
                         1,  1,  2,  0,  0,  0,  0,   0,     0,  true,  true },
{ L_, "\"0001-01-02T00:00:00.000000+23:59\"",
                         1,  1,  2,  0,  0,  0,  0,   0,  1439,  true,  true },
{ L_, "\"0001-01-02T00:00:00.000000-23:59\"",
                         1,  1,  2,  0,  0,  0,  0,   0, -1439,  true,  true },
{ L_, "\"0001-01-02T01:01:01.000001+00:00\"",
                         1,  1,  2,  1,  1,  1,  0,   1,     0,  true,  true },
{ L_, "\"0001-01-02T01:01:01.000001+08:20\"",
                         1,  1,  2,  1,  1,  1,  0,   1,   500,  true,  true },

{ L_, "\"0001-01-02T01:23:59.059168Z\"",
                         1,  1,  2,  1, 23, 59, 59, 168,     0,  true,  true },
{ L_, "\"0001-01-02T01:23:59.059168+00:00\"",
                         1,  1,  2,  1, 23, 59, 59, 168,     0,  true,  true },
{ L_, "\"0001-01-02T01:23:59.059168+08:20\"",
                         1,  1,  2,  1, 23, 59, 59, 168,   500,  true,  true },
{ L_, "\"0001-01-02T01:23:59.059168-08:20\"",
                         1,  1,  2,  1, 23, 59, 59, 168,  -500,  true,  true },
{ L_, "\"0001-01-10T00:00:00.000000z\"",
                         1,  1, 10,  0,  0,  0,  0,   0,     0,  true,  true },
{ L_, "\"0001-01-10T00:00:00.000000+00:00\"",
                         1,  1, 10,  0,  0,  0,  0,   0,     0,  true,  true },
{ L_, "\"0001-01-10T01:01:01.001+01:39\"",
                         1,  1, 10,  1,  1,  1,  1,   0,    99,  true,  true },
{ L_, "\"0001-01-10T01:01:01.000001+01:39\"",
                         1,  1, 10,  1,  1,  1,  0,   1,    99,  true,  true },

{ L_, "\"0001-01-30T00:00:00.000000Z\"",
                         1,  1, 30,  0,  0,  0,  0,   0,     0,  true,  true },
{ L_, "\"0001-01-30T00:00:00.000000+00:00\"",
                         1,  1, 30,  0,  0,  0,  0,   0,     0,  true,  true },
{ L_, "\"0001-01-31T00:00:00.000000+23:59\"",
                         1,  1, 31,  0,  0,  0,  0,   0,  1439,  true,  true },
{ L_, "\"0001-01-31T00:00:00.000000-23:59\"",
                         1,  1, 31,  0,  0,  0,  0,   0, -1439,  true,  true },

{ L_, "\"0001-02-01T00:00:00.000000z\"",
                         1,  2,  1,  0,  0,  0,  0,   0,     0,  true,  true },
{ L_, "\"0001-02-01T00:00:00.000000+00:00\"",
                         1,  2,  1,  0,  0,  0,  0,   0,     0,  true,  true },
{ L_, "\"0001-02-01T23:59:59.000000+23:59\"",
                         1,  2,  1, 23, 59, 59,  0,   0,  1439,  true,  true },

{ L_, "\"0001-12-31T00:00:00.000000Z\"",
                         1, 12, 31,  0,  0,  0,  0,   0,     0,  true,  true },
{ L_, "\"0001-12-31T00:00:00.000000+00:00\"",
                         1, 12, 31,  0,  0,  0,  0,   0,     0,  true,  true },
{ L_, "\"0001-12-31T23:59:59.000000+23:59\"",
                         1, 12, 31, 23, 59, 59,  0,   0,  1439,  true,  true },

{ L_, "\"0002-01-01T00:00:00.000000+00:00\"",
                         2,  1,  1,  0,  0,  0,  0,   0,     0,  true,  true },
{ L_, "\"0002-01-01T23:59:59.000000+23:59\"",
                         2,  1,  1, 23, 59, 59,  0,   0,  1439,  true,  true },

{ L_, "\"0004-02-28T00:00:00.000000+00:00\"",
                         4,  2, 28,  0,  0,  0,  0,   0,     0,  true,  true },
{ L_, "\"0004-02-28T23:59:59.000000+23:59\"",
                         4,  2, 28, 23, 59, 59,  0,   0,  1439,  true,  true },
{ L_, "\"0004-02-28T23:59:59.000000-23:59\"",
                         4,  2, 28, 23, 59, 59,  0,   0, -1439,  true,  true },

{ L_, "\"0004-02-29T00:00:00.000000+00:00\"",
                         4,  2, 29,  0,  0,  0,  0,   0,     0,  true,  true },
{ L_, "\"0004-02-29T23:59:59.000000+23:59\"",
                         4,  2, 29, 23, 59, 59,  0,   0,  1439,  true,  true },
{ L_, "\"0004-02-29T23:59:59.000000-23:59\"",
                         4,  2, 29, 23, 59, 59,  0,   0, -1439,  true,  true },

{ L_, "\"0100-02-28T00:00:00.000000+00:00\"",
                       100,  2, 28,  0,  0,  0,  0,   0,     0,  true,  true },
{ L_, "\"0100-02-28T23:59:59.000000+23:59\"",
                       100,  2, 28, 23, 59, 59,  0,   0,  1439,  true,  true },
{ L_, "\"0100-02-28T23:59:59.000000-23:59\"",
                       100,  2, 28, 23, 59, 59,  0,   0, -1439,  true,  true },


{ L_, "\"0100-03-01T00:00:00.000000+00:00\"",
                       100,  3,  1,  0,  0,  0,  0,   0,     0,  true,  true },
{ L_, "\"0100-03-01T23:59:59.000000+23:59\"",
                       100,  3,  1, 23, 59, 59,  0,   0,  1439,  true,  true },
{ L_, "\"0100-03-01T23:59:59.000000-23:59\"",
                       100,  3,  1, 23, 59, 59,  0,   0, -1439,  true,  true },

{ L_, "\"9999-12-31T00:00:00.000000+00:00\"",
                      9999, 12, 31,  0,  0,  0,  0,   0,     0,  true,  true },
{ L_, "\"9999-12-31T23:59:59.000000+23:59\"",
                      9999, 12, 31, 23, 59, 59,  0,   0,  1439,  true,  true },
{ L_, "\"9999-12-31T23:59:59.000000-23:59\"",
                      9999, 12, 31, 23, 59, 59,  0,   0, -1439,  true,  true },

{ L_, "\"0000-01-01T00:00:00.000000+00:00\"",
                         1,  1,  1, 24,  0,  0,  0,   0,     0, false,  true },
{ L_, "\"0001-00-01T00:00:00.000000+00:00\"",
                         1,  1,  1, 24,  0,  0,  0,   0,     0, false,  true },
{ L_, "\"0001-01-00T00:00:00.000000+00:00\"",
                         1,  1,  1, 24,  0,  0,  0,   0,     0, false,  true },
{ L_, "\"0000-00-00T00:00:00.000000+00:00\"",
                         1,  1,  1, 24,  0,  0,  0,   0,     0, false,  true },
{ L_, "\"0001-13-00T00:00:00.000000+00:00\"",
                         1,  1,  1, 24,  0,  0,  0,   0,     0, false,  true },
{ L_, "\"0001-01-32T00:00:00.000000+00:00\"",
                         1,  1,  1, 24,  0,  0,  0,   0,     0, false,  true },
{ L_, "\"0001-01-01T25:00:00.000000+00:00\"",
                         1,  1,  1, 24,  0,  0,  0,   0,     0, false,  true },
{ L_, "\"0001-01-01T00:61:00.000000+00:00\"",
                         1,  1,  1, 24,  0,  0,  0,   0,     0, false,  true },
{ L_, "\"0001-01-01T00:00:61.000000+00:00\"",
                         1,  1,  1, 24,  0,  0,  0,   0,     0, false,  true },
{ L_, "\"0001-01-01T00:00:00.000000+24:00\"",
                         1,  1,  1, 24,  0,  0,  0,   0,     0, false,  true },
{ L_, "\"0001-01-01T00:00:00.000000+00:61\"",
                         1,  1,  1, 24,  0,  0,  0,   0,     0, false,  true },
{ L_, "\"0001-01-01T00:00:00.000000-24:00\"",
                         1,  1,  1, 24,  0,  0,  0,   0,     0, false,  true },
{ L_, "\"0001-01-01T00:00:00.000000-00:61\"",
                         1,  1,  1, 24,  0,  0,  0,   0,     0, false,  true },

{ L_, "\"GARBAGE\"",
                         1,  1,  1, 24,  0,  0,  0,   0,     0, false, false },
{ L_, "\".9999\"",
                         1,  1,  1, 24,  0,  0,  0,   0,     0, false, false },
{ L_, "\"23:59:59.9999\"",
                         1,  1,  1, 24,  0,  0,  0,   0,     0, false, false },
{ L_, "\"T23:59:59.9999\"",
                         1,  1,  1, 24,  0,  0,  0,   0,     0, false, false },
{ L_, "\"01T23:59:59.9999\"",
                         1,  1,  1, 24,  0,  0,  0,   0,     0, false, false },
{ L_, "\"01-01T23:59:59.9999\"",
                         1,  1,  1, 24,  0,  0,  0,   0,     0, false, false },
{ L_, "\"0001-01-01T00:00.000\"",
                         1,  1,  1, 24,  0,  0,  0,   0,     0, false, false },

{ L_, "\"0001-00-01T00:00:00.000\"",
                         1,  1,  1, 24,  0,  0,  0,   0,     0, false, false },
{ L_, "\"0000-01-01T00:00:00.000000\"",
                         1,  1,  1, 24,  0,  0,  0,   0,     0, false, false },
{ L_, "\"0000-00-00T00:00:00.000\"",
                         1,  1,  1, 24,  0,  0,  0,   0,     0, false, false },
{ L_, "\"0001-13-00T00:00:00.000\"",
                         1,  1,  1, 24,  0,  0,  0,   0,     0, false, false },
{ L_, "\"0001-01-32T00:00:00.000\"",
                         1,  1,  1, 24,  0,  0,  0,   0,     0, false, false },
{ L_, "\"0001-01-01T25:00:00.000\"",
                         1,  1,  1, 24,  0,  0,  0,   0,     0, false, false },
{ L_, "\"0001-01-01T00:61:00.000\"",
                         1,  1,  1, 24,  0,  0,  0,   0,     0, false, false },
{ L_, "\"0001-01-01T00:00:61.000\"",
                         1,  1,  1, 24,  0,  0,  0,   0,     0, false, false },
{ L_, "\"0001-01-01T00:00.000000\"",
                         1,  1,  1, 24,  0,  0,  0,   0,     0, false, false },
{ L_, "\"01-01-01T23:59:59.9999\"",
                         1,  1,  1, 24,  0,  0,  0,   0,     0, false, false },
{ L_, "\"01-01-01T23:59:59.9999999\"",
                         1,  1,  1, 24,  0,  0,  0,   0,     0, false, false },

{ L_, "\"0000-01-01T00:00:00.000+00:00\"",
                         1,  1,  1, 24,  0,  0,  0,   0,     0, false, false },
{ L_, "\"0000-01-01T00:00:00.000Z\"",
                         1,  1,  1, 24,  0,  0,  0,   0,     0, false, false },
{ L_, "\"0000-01-01T00:00:00.000z\"",
                         1,  1,  1, 24,  0,  0,  0,   0,     0, false, false },
{ L_, "\"0000-01-01T00:00:00.000-00:00\"",
                         1,  1,  1, 24,  0,  0,  0,   0,     0, false, false },
        };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int         LINE          = DATA[i].d_line;
                const string      INPUT         = DATA[i].d_input_p;
                const int         YEAR          = DATA[i].d_year;
                const int         MONTH         = DATA[i].d_month;
                const int         DAY           = DATA[i].d_day;
                const int         HOUR          = DATA[i].d_hour;
                const int         MINUTE        = DATA[i].d_minutes;
                const int         SECOND        = DATA[i].d_seconds;
                const int         MILLISECOND   = DATA[i].d_milliSecs;
                const int         MICROSECOND   = DATA[i].d_microSecs;
                const int         OFFSET        = DATA[i].d_tzoffset;
                const bool        IS_VALID      = DATA[i].d_isValid;
                const bool        IS_DATETIMETZ = DATA[i].d_isDatetimeTz;

                const bdlt::Datetime   DEFAULT_DATETIME;
                const bdlt::Datetime   EXP_DATETIME(YEAR,
                                                    MONTH,
                                                    DAY,
                                                    HOUR,
                                                    MINUTE,
                                                    SECOND,
                                                    MILLISECOND,
                                                    MICROSECOND);
                const bdlt::DatetimeTz DEFAULT_DATETIMETZ;
                const bdlt::DatetimeTz EXP_DATETIMETZ(EXP_DATETIME, OFFSET);

                if (veryVerbose) { T_ P_(LINE) P(INPUT) }

                const bsl::string_view isb(INPUT);

                // Variant is left unset.

                Util::DatetimeOrDatetimeTz value;
                ASSERTV(LINE, value.isUnset());

                int rc = Util::getValue(&value, isb);

                if (IS_VALID) {
                    ASSERTV(LINE, rc, 0 == rc);
                    if (IS_DATETIMETZ) {
                        ASSERTV(LINE, value.is<bdlt::DatetimeTz>());
                        ASSERTV(LINE,
                                EXP_DATETIMETZ,
                                value.the<bdlt::DatetimeTz>(),
                                EXP_DATETIMETZ ==
                                                value.the<bdlt::DatetimeTz>());
                    }
                    else {
                        ASSERTV(LINE, value.is<bdlt::Datetime>());
                        ASSERTV(LINE,
                                EXP_DATETIME,
                                value.the<bdlt::Datetime>(),
                                EXP_DATETIME == value.the<bdlt::Datetime>());
                    }
                }
                else {
                    ASSERTV(LINE, rc, rc);
                    ASSERTV(LINE, value.isUnset());
                }

                // 'bdlt::Datetime' value is initially assigned.

                value = DEFAULT_DATETIME;
                ASSERTV(LINE, value.is<bdlt::Datetime>());
                ASSERTV(LINE, DEFAULT_DATETIME == value.the<bdlt::Datetime>());

                rc = Util::getValue(&value, isb);

                if (IS_VALID) {
                    ASSERTV(LINE, rc, 0 == rc);
                    if (IS_DATETIMETZ) {
                        ASSERTV(LINE, value.is<bdlt::DatetimeTz>());
                        ASSERTV(LINE,
                                EXP_DATETIMETZ,
                                value.the<bdlt::DatetimeTz>(),
                                EXP_DATETIMETZ ==
                                                value.the<bdlt::DatetimeTz>());
                    }
                    else {
                        ASSERTV(LINE, value.is<bdlt::Datetime>());
                        ASSERTV(LINE,
                                EXP_DATETIME,
                                value.the<bdlt::Datetime>(),
                                EXP_DATETIME == value.the<bdlt::Datetime>());
                    }
                }
                else {
                    ASSERTV(LINE, rc, rc);
                    ASSERTV(LINE, value.is<bdlt::Datetime>());
                    ASSERTV(LINE,
                            DEFAULT_DATETIME == value.the<bdlt::Datetime>());
                }

                // 'bdlt::DatetimeTz' value is initially assigned.

                value = DEFAULT_DATETIMETZ;
                ASSERTV(LINE, value.is<bdlt::DatetimeTz>());
                ASSERTV(LINE,
                        DEFAULT_DATETIMETZ == value.the<bdlt::DatetimeTz>());

                rc = Util::getValue(&value, isb);

                if (IS_VALID) {
                    ASSERTV(LINE, rc, 0 == rc);
                    if (IS_DATETIMETZ) {
                        ASSERTV(LINE, value.is<bdlt::DatetimeTz>());
                        ASSERTV(LINE,
                                EXP_DATETIMETZ,
                                value.the<bdlt::DatetimeTz>(),
                                EXP_DATETIMETZ ==
                                                value.the<bdlt::DatetimeTz>());
                    }
                    else {
                        ASSERTV(LINE, value.is<bdlt::Datetime>());
                        ASSERTV(LINE,
                                EXP_DATETIME,
                                value.the<bdlt::Datetime>(),
                                EXP_DATETIME == value.the<bdlt::Datetime>());
                    }
                }
                else {
                    ASSERTV(LINE, rc, rc);
                    ASSERTV(LINE, value.is<bdlt::DatetimeTz>());
                    ASSERTV(LINE,
                            DEFAULT_DATETIMETZ ==
                                                value.the<bdlt::DatetimeTz>());
                }
            }
        }
      } break;
      case 27: {
        // --------------------------------------------------------------------
        // TESTING 'getValue' for 'DateOrDateTz'
        //
        // Concerns:
        //: 1 Values in the valid range, including the maximum and minimum
        //:   values for this type, are parsed correctly.
        //:
        //: 2 The function fails and the passed in variable remains unmodified
        //:   if the data is not valid.
        //:
        //: 3 The return code is 0 on success and non-zero on failure.
        //
        // Plan:
        //: 1 Using the table-driven technique, specify a set of distinct
        //:   rows of string value, expected parsed value, and return code.
        //:
        //: 2 For each row in the table of P-1:
        //:
        //:   1 Provide the string value and a variable  to be parsed into to
        //:     the 'getValue' function.  Invoke the function three times,
        //:     leaving the variable unset, then assigning a sentinel value of
        //:     type "bdlt::Date' to the variable and finally assigning a
        //:     sentinel value of type 'bdlt::DateTz'.
        //:
        //:   2 If the parsing should succeed then verify that the variable
        //:     value matches the expected value.  Otherwise confirm that the
        //:     variable value is unmodified.  (C-1..2)
        //:
        //:   3 Confirm that the return code is 0 on success and non-zero
        //:     otherwise.  (C-3)
        //
        // Testing:
        //   static int getValue(bdlt::DateOrDateTz   *v, bsl::string_view s);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING 'getValue' for 'DateOrDateTz'"
                          << "\n=====================================" << endl;
        {
            static const struct {
                int         d_line;      // source line number

                const char *d_input_p;   // input

                int         d_year;      // year under test

                int         d_month;     // month under test

                int         d_day;       // day under test

                int         d_offset;    // UTC offset

                bool        d_isValid;   // isValid flag

                bool        d_isDateTz;  // flag indicating whether the result
                                         // object is expected to contain
                                         // 'DateTz' or 'Date' object
            } DATA[] = {
  //LINE  INPUT                     YEAR  MONTH   DAY    OFF  VALID   DATETZ
  //----  ------------------------  ----  -----   ---  -----  -----   ------
  {  L_, "\"0001-01-01\"",             1,     1,    1,     0,  true,  false },
  {  L_, "\"0001-01-02\"",             1,     1,    2,     0,  true,  false },
  {  L_, "\"0001-01-10\"",             1,     1,   10,     0,  true,  false },
  {  L_, "\"0001-01-30\"",             1,     1,   30,     0,  true,  false },
  {  L_, "\"0001-01-31\"",             1,     1,   31,     0,  true,  false },

  {  L_, "\"0001-02-01\"",             1,     2,    1,     0,  true,  false },
  {  L_, "\"0001-12-31\"",             1,    12,   31,     0,  true,  false },
  {  L_, "\"0002-01-01\"",             2,     1,    1,     0,  true,  false },
  {  L_, "\"0004-01-01\"",             4,     1,    1,     0,  true,  false },
  {  L_, "\"0004-02-28\"",             4,     2,   28,     0,  true,  false },
  {  L_, "\"0004-02-29\"",             4,     2,   29,     0,  true,  false },
  {  L_, "\"0004-03-01\"",             4,     3,    1,     0,  true,  false },

  {  L_, "\"0008-02-28\"",             8,     2,   28,     0,  true,  false },
  {  L_, "\"0008-02-29\"",             8,     2,   29,     0,  true,  false },
  {  L_, "\"0008-03-01\"",             8,     3,    1,     0,  true,  false },
  {  L_, "\"0100-02-28\"",           100,     2,   28,     0,  true,  false },
  {  L_, "\"0100-03-01\"",           100,     3,    1,     0,  true,  false },
  {  L_, "\"0400-02-28\"",           400,     2,   28,     0,  true,  false },
  {  L_, "\"0400-02-29\"",           400,     2,   29,     0,  true,  false },
  {  L_, "\"0400-03-01\"",           400,     3,    1,     0,  true,  false },
  {  L_, "\"0500-02-28\"",           500,     2,   28,     0,  true,  false },
  {  L_, "\"0500-03-01\"",           500,     3,    1,     0,  true,  false },

  {  L_, "\"0800-02-28\"",           800,     2,   28,     0,  true,  false },
  {  L_, "\"0800-02-29\"",           800,     2,   29,     0,  true,  false },
  {  L_, "\"0800-03-01\"",           800,     3,    1,     0,  true,  false },
  {  L_, "\"1000-02-28\"",          1000,     2,   28,     0,  true,  false },
  {  L_, "\"1000-03-01\"",          1000,     3,    1,     0,  true,  false },
  {  L_, "\"2000-02-28\"",          2000,     2,   28,     0,  true,  false },
  {  L_, "\"2000-02-29\"",          2000,     2,   29,     0,  true,  false },
  {  L_, "\"2000-03-01\"",          2000,     3,    1,     0,  true,  false },
  {  L_, "\"2016-12-31\"",          2016,    12,   31,     0,  true,  false },
  {  L_, "\"2017-12-31\"",          2017,    12,   31,     0,  true,  false },
  {  L_, "\"2018-12-31\"",          2018,    12,   31,     0,  true,  false },
  {  L_, "\"2019-12-31\"",          2019,    12,   31,     0,  true,  false },

  {  L_, "\"2020-01-01\"",          2020,     1,    1,     0,  true,  false },
  {  L_, "\"2020-01-02\"",          2020,     1,    2,     0,  true,  false },
  {  L_, "\"2020-02-28\"",          2020,     2,   28,     0,  true,  false },
  {  L_, "\"2020-02-29\"",          2020,     2,   29,     0,  true,  false },
  {  L_, "\"2020-03-01\"",          2020,     3,    1,     0,  true,  false },
  {  L_, "\"2022-01-02\"",          2022,     1,    2,     0,  true,  false },

  {  L_, "\"9999-02-28\"",          9999,     2,   28,     0,  true,  false },
  {  L_, "\"9999-03-01\"",          9999,     3,    1,     0,  true,  false },
  {  L_, "\"9999-12-30\"",          9999,    12,   30,     0,  true,  false },
  {  L_, "\"9999-12-31\"",          9999,    12,   31,     0,  true,  false },

  {  L_, "\"0001-01-01+00:00\"",      1,     1,    1,      0,  true,   true },
  {  L_, "\"0001-01-01+00:45\"",      1,     1,    1,     45,  true,   true },
  {  L_, "\"0001-01-01-23:59\"",      1,     1,    1,  -1439,  true,   true },

  {  L_, "\"0001-01-02+00:00\"",      1,     1,    2,      0,  true,   true },
  {  L_, "\"0001-01-02+08:20\"",      1,     1,    2,    500,  true,   true },
  {  L_, "\"0001-01-02-08:20\"",      1,     1,    2,   -500,  true,   true },

  {  L_, "\"0001-02-01+00:00\"",      1,     2,    1,      0,  true,   true },
  {  L_, "\"0001-02-01+23:59\"",      1,     2,    1,   1439,  true,   true },

  {  L_, "\"0004-02-29+00:00\"",      4,     2,   29,      0,  true,   true },
  {  L_, "\"0004-02-29+23:59\"",      4,     2,   29,   1439,  true,   true },
  {  L_, "\"0004-02-29-23:59\"",      4,     2,   29,  -1439,  true,   true },

  {  L_, "\"9999-02-28+00:00\"",   9999,     2,   28,      0,  true,   true },
  {  L_, "\"9999-02-28+23:59\"",   9999,     2,   28,   1439,  true,   true },
  {  L_, "\"9999-02-28-23:59\"",   9999,     2,   28,  -1439,  true,   true },

  {  L_, "\"9999-12-31+00:00\"",   9999,    12,   31,      0,  true,   true },
  {  L_, "\"9999-12-31+23:59\"",   9999,    12,   31,   1439,  true,   true },

  {  L_, "\"0001-01-01Z\"",            1,     1,    1,     0,  true,   true },
  {  L_, "\"0001-01-01z\"",            1,     1,    1,     0,  true,   true },
  {  L_, "\"0001-01-01+00:00\"",       1,     1,    1,     0,  true,   true },
  {  L_, "\"0001-01-01-00:00\"",       1,     1,    1,     0,  true,   true },
  {  L_, "\"9999-01-01+00:00\"",    9999,     1,    1,     0,  true,   true },
  {  L_, "\"9999-01-01-00:00\"",    9999,     1,    1,     0,  true,   true },

  {  L_, "\"GARBAGE\"",                1,     1,    1,     0, false,  false },

  {  L_, "\"0000-01-01\"",             1,     1,    1,     0, false,  false },
  {  L_, "\"0001-00-01\"",             1,     1,    1,     0, false,  false },
  {  L_, "\"0001-01-00\"",             1,     1,    1,     0, false,  false },
  {  L_, "\"0000-00-00\"",             1,     1,    1,     0, false,  false },
  {  L_, "\"0001-13-00\"",             1,     1,    1,     0, false,  false },
  {  L_, "\"0001-01-32\"",             1,     1,    1,     0, false,  false },

  {  L_, "\"0001-01-01T00:00:00.000+00:00\"",
                                       1,     1,    1,     0, false,  false },
        };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int         LINE        = DATA[i].d_line;
                const string      INPUT       = DATA[i].d_input_p;
                const int         YEAR        = DATA[i].d_year;
                const int         MONTH       = DATA[i].d_month;
                const int         DAY         = DATA[i].d_day;
                const int         OFFSET      = DATA[i].d_offset;
                const bool        IS_VALID    = DATA[i].d_isValid;
                const bool        IS_DATETZ   = DATA[i].d_isDateTz;

                const bdlt::Date   DEFAULT_DATE;
                const bdlt::Date   EXP_DATE(YEAR, MONTH, DAY);
                const bdlt::DateTz DEFAULT_DATETZ;
                const bdlt::DateTz EXP_DATETZ(EXP_DATE, OFFSET);

                if (veryVerbose) { T_ P_(LINE) P(INPUT) }

                const bsl::string_view isb(INPUT);

                // Variant is left unset.

                Util::DateOrDateTz value;
                ASSERTV(LINE, value.isUnset());

                int rc = Util::getValue(&value, isb);

                if (IS_VALID) {
                    ASSERTV(LINE, rc, 0 == rc);
                    if (IS_DATETZ) {
                        ASSERTV(LINE, value.is<bdlt::DateTz>());
                        ASSERTV(LINE, EXP_DATETZ, value.the<bdlt::DateTz>(),
                                EXP_DATETZ == value.the<bdlt::DateTz>());
                    }
                    else {
                        ASSERTV(LINE, value.is<bdlt::Date>());
                        ASSERTV(LINE, EXP_DATE, value.the<bdlt::Date>(),
                                EXP_DATE == value.the<bdlt::Date>());
                    }
                }
                else {
                    ASSERTV(LINE, rc, rc);
                    ASSERTV(LINE, value.isUnset());
                }

                // 'bdlt::Date' value is initially assigned.

                value = DEFAULT_DATE;
                ASSERTV(LINE, value.is<bdlt::Date>());
                ASSERTV(LINE, DEFAULT_DATE == value.the<bdlt::Date>());

                rc = Util::getValue(&value, isb);

                if (IS_VALID) {
                    ASSERTV(LINE, rc, 0 == rc);
                    if (IS_DATETZ) {
                        ASSERTV(LINE, value.is<bdlt::DateTz>());
                        ASSERTV(LINE, EXP_DATETZ, value.the<bdlt::DateTz>(),
                                EXP_DATETZ == value.the<bdlt::DateTz>());
                    }
                    else {
                        ASSERTV(LINE, value.is<bdlt::Date>());
                        ASSERTV(LINE, EXP_DATE, value.the<bdlt::Date>(),
                                EXP_DATE == value.the<bdlt::Date>());
                    }
                }
                else {
                    ASSERTV(LINE, rc, rc);
                    ASSERTV(LINE, value.is<bdlt::Date>());
                    ASSERTV(LINE, DEFAULT_DATE == value.the<bdlt::Date>());
                }

                // 'bdlt::DateTz' value is initially assigned.

                value = DEFAULT_DATETZ;
                ASSERTV(LINE, value.is<bdlt::DateTz>());
                ASSERTV(LINE, DEFAULT_DATETZ == value.the<bdlt::DateTz>());

                rc = Util::getValue(&value, isb);

                if (IS_VALID) {
                    ASSERTV(LINE, rc, 0 == rc);
                    if (IS_DATETZ) {
                        ASSERTV(LINE, value.is<bdlt::DateTz>());
                        ASSERTV(LINE, EXP_DATETZ, value.the<bdlt::DateTz>(),
                                EXP_DATETZ == value.the<bdlt::DateTz>());
                    }
                    else {
                        ASSERTV(LINE, value.is<bdlt::Date>());
                        ASSERTV(LINE, EXP_DATE, value.the<bdlt::Date>(),
                                EXP_DATE == value.the<bdlt::Date>());
                    }
                }
                else {
                    ASSERTV(LINE, rc, rc);
                    ASSERTV(LINE, value.is<bdlt::DateTz>());
                    ASSERTV(LINE, DEFAULT_DATETZ == value.the<bdlt::DateTz>());
                }
            }
        }
      } break;
      case 26: {
        // --------------------------------------------------------------------
        // TESTING 'getValue' for 'TimeOrTimeTz'
        //
        // Concerns:
        //: 1 Values in the valid range, including the maximum and minimum
        //:   values for this type, are parsed correctly.
        //:
        //: 2 The function fails and the passed in variable remains unmodified
        //:   if the data is not valid.
        //:
        //: 3 The return code is 0 on success and non-zero on failure.
        //
        // Plan:
        //: 1 Using the table-driven technique, specify a set of distinct
        //:   rows of string value, expected parsed value, and return code.
        //:
        //: 2 For each row in the table of P-1:
        //:
        //:   1 Provide the string value and a variable to be parsed into to
        //:     the 'getValue' function.  Invoke the function three times,
        //:     leaving the variable unset, then assigning a sentinel value of
        //:     type "bdlt::Time' to the variable and finally assigning a
        //:     a sentinel value of type 'bdlt::TimeTz'.
        //:
        //:   2 If the parsing should succeed then verify that the variable
        //:     value matches the expected value.  Otherwise confirm that the
        //:     variable value is unmodified.  (C-1..2)
        //:
        //:   3 Confirm that the return code is 0 on success and non-zero
        //:     otherwise.  (C-3)
        //
        // Testing:
        //   static int getValue(TimeOrTimeTz         *v, bsl::string_view s);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING 'getValue' for 'TimeOrTimeTz'"
                          << "\n=====================================" << endl;
        {
            static const struct {
                int         d_line;      // source line number

                const char *d_input_p;   // input

                int         d_hour;      // hour under test

                int         d_minutes;   // minutes under test

                int         d_seconds;   // seconds under test

                int         d_milliSecs; // milli seconds under test

                int         d_uSecs;     // micro seconds under test

                int         d_offset;    // UTC offset

                bool        d_isValid;   // isValid flag

                bool        d_isTimeTz;  // flag indicating whether the result
                                         // object is expected to contain
                                         // 'TimeTz' or 'Time' object
            } DATA[] = {
  //LINE INPUT
  //---- ---------------------------
  //                          HR    M     S    MS   US   OFF    VALID  TIMETZ
  //                          ---   --    --   ---  ---  -----  ------ ------
  {  L_, "\"00:00:00\"",
                                0,   0,    0,    0,   0,     0,  true, false },
  {  L_, "\"00:00:00.000\"",
                                0,   0,    0,    0,   0,     0,  true, false },
  {  L_, "\"00:00:00.000000\"",
                                0,   0,    0,    0,   0,     0,  true, false },
  {  L_, "\"00:00:00.000001\"",
                                0,   0,    0,    0,   1,     0,  true, false },
  {  L_, "\"00:00:00.000999\"",
                                0,   0,    0,    0, 999,     0,  true, false },
  {  L_, "\"00:00:00.001000\"",
                                0,   0,    0,    1,   0,     0,  true, false },
  {  L_, "\"00:00:00.999\"",
                                0,   0,    0,  999,   0,     0,  true, false },
  {  L_, "\"00:00:00.100001\"",
                                0,   0,    0,  100,   1,     0,  true, false },
  {  L_, "\"00:00:00.999999\"",
                                0,   0,    0,  999, 999,     0,  true, false },

  {  L_, "\"00:00:01\"",
                                0,   0,    1,    0,   0,     0,  true, false },
  {  L_, "\"00:00:01.000\"",
                                0,   0,    1,    0,   0,     0,  true, false },
  {  L_, "\"00:00:59.000\"",
                                0,   0,   59,    0,   0,     0,  true, false },
  {  L_, "\"01:01:01.001\"",
                                1,   1,    1,    1,   0,     0,  true, false },
  {  L_, "\"01:01:01.000001\"",
                                1,   1,    1,    0,   1,     0,  true, false },
  {  L_, "\"01:01:01.001001\"",
                                1,   1,    1,    1,   1,     0,  true, false },

  {  L_, "\"01:23:59\"",
                                1,  23,   59,    0,   0,     0,  true, false },
  {  L_, "\"01:23:59.059\"",
                                1,  23,   59,   59,   0,     0,  true, false },
  {  L_, "\"23:59:59.000\"",
                               23,  59,   59,    0,   0,     0,  true, false },
  {  L_, "\"23:59:59.999\"",
                               23,  59,   59,  999,   0,     0,  true, false },
  {  L_, "\"23:59:59.999000\"",
                               23,  59,   59,  999,   0,     0,  true, false },
  {  L_, "\"23:59:59.999001\"",
                               23,  59,   59,  999,   1,     0,  true, false },
  {  L_, "\"23:59:59.999999\"",
                               23,  59,   59,  999, 999,     0,  true, false },
  {  L_, "\"23:59:59.999Z\"",
                               23,  59,   59,  999,   0,     0,  true,  true },
  {  L_, "\"23:59:59.999z\"",
                               23,  59,   59,  999,   0,     0,  true,  true },

  {  L_, "\"24:00:00\"",
                               24,   0,    0,    0,   0,     0,  true, false },
  {  L_, "\"24:00:00.000\"",
                               24,   0,    0,    0,   0,     0,  true, false },
  {  L_, "\"24:00:00.000000\"",
                               24,   0,    0,    0,   0,     0,  true, false },

  {  L_, "\"00:00:00.000+00:00\"",
                                0,   0,    0,    0,   0,     0,  true,  true },
  {  L_, "\"00:00:00.000Z\"",
                                0,   0,    0,    0,   0,     0,  true,  true },
  {  L_, "\"00:00:00.000z\"",
                                0,   0,    0,    0,   0,     0,  true,  true },
  {  L_, "\"00:00:00.000+00:45\"",
                                0 ,  0,    0,    0,   0,    45,  true,  true },
  {  L_, "\"00:00:00.000+23:59\"",
                                0,   0,    0,    0,   0,  1439,  true,  true },
  {  L_, "\"00:00:00.000-23:59\"",
                                0,   0,    0,    0,   0, -1439,  true,  true },

  {  L_, "\"00:00:00.999+00:00\"",
                                0,   0,    0,  999,   0,     0,  true,  true },
  {  L_, "\"00:00:00.999+23:59\"",
                                0,   0,    0,  999,   0,  1439,  true,  true },
  {  L_, "\"00:00:00.999-23:59\"",
                                0,   0,    0,  999,   0, -1439,  true,  true },

  {  L_, "\"00:00:00.9999+00:00\"",
                                0,   0,    0,  999, 900,     0,  true,  true },
  {  L_, "\"00:00:00.99999+00:00\"",
                                0,   0,    0,  999, 990,     0,  true,  true },
  {  L_, "\"00:00:00.999999+00:00\"",
                                0,   0,    0,  999, 999,     0,  true,  true },

  {  L_, "\"00:00:01.000+00:00\"",
                                0,   0,    1,    0,   0,     0,  true,  true },
  {  L_, "\"00:00:01.000Z\"",
                                0,   0,    1,    0,   0,     0,  true,  true },
  {  L_, "\"00:00:01.000z\"",
                                0,   0,    1,    0,   0,     0,  true,  true },
  {  L_, "\"00:00:01.000+23:59\"",
                                0,   0,    1,    0,   0,  1439,  true,  true },
  {  L_, "\"00:00:01.000-23:59\"",
                                0,   0,    1,    0,   0, -1439,  true,  true },

  {  L_, "\"00:00:59.000+00:00\"",
                                0,   0,   59,    0,   0,     0,  true,  true },
  {  L_, "\"00:00:59.000Z\"",
                                0,   0,   59,    0,   0,     0,  true,  true },
  {  L_, "\"00:00:59.000z\"",
                                0,   0,   59,    0,   0,     0,  true,  true },
  {  L_, "\"00:00:59.000+23:59\"",
                                0,   0,   59,    0,   0,  1439,  true,  true },
  {  L_, "\"00:00:59.000-23:59\"",
                                0,   0,   59,    0,   0, -1439,  true,  true },

  {  L_, "\"01:01:01.001+00:00\"",
                                1,   1,    1,    1,   0,     0,  true,  true },
  {  L_, "\"01:01:01.001+08:20\"",
                                1,   1,    1,    1,   0,   500,  true,  true },

  {  L_, "\"01:23:59.059+00:00\"",
                                1,  23,   59,   59,   0,     0,  true,  true },
  {  L_, "\"01:23:59.059Z\"",
                                1,  23,   59,   59,   0,     0,  true,  true },
  {  L_, "\"01:23:59.059z\"",
                                1,  23,   59,   59,   0,     0,  true,  true },
  {  L_, "\"01:23:59.059+23:59\"",
                                1,  23,   59,   59,   0,  1439,  true,  true },
  {  L_, "\"01:23:59.059-23:59\"",
                                1,  23,   59,   59,   0, -1439,  true,  true },

  {  L_, "\"23:59:59.000+23:59\"",
                               23,  59,   59,    0,   0,  1439,  true,  true },
  {  L_, "\"23:59:59.000-23:59\"",
                               23,  59,   59,    0,   0, -1439,  true,  true },

  {  L_, "\"23:59:59.999+00:00\"",
                               23,  59,   59,  999,   0,     0,  true,  true },
  {  L_, "\"23:59:59.999Z\"",
                               23,  59,   59,  999,   0,     0,  true,  true },
  {  L_, "\"23:59:59.999z\"",
                               23,  59,   59,  999,   0,     0,  true,  true },
  {  L_, "\"23:59:59.999+23:59\"",
                               23,  59,   59,  999,   0,  1439,  true,  true },
  {  L_, "\"23:59:59.999-23:59\"",
                               23,  59,   59,  999,   0, -1439,  true,  true },

  {  L_, "\"24:00:00.000+00:00\"",
                               24,   0,    0,    0,   0,     0,  true,  true },
  {  L_, "\"24:00:00.000Z\"",
                               24,   0,    0,    0,   0,     0,  true,  true },
  {  L_, "\"24:00:00.000z\"",
                               24,   0,    0,    0,   0,     0,  true,  true },

  {  L_, "\"23:59:59.9999999\"",
                                0,   0,    0,    0,   0,     0,  true, false },

  {  L_, "\"GARBAGE\"",
                               24,   0,    0,    0,   0,     0, false, false },
  {  L_, "\".9999\"",
                               24,   0,    0,    0,   0,     0, false, false },
  {  L_, "\"23.9999\"",
                               24,   0,    0,    0,   0,     0, false, false },
  {  L_, "\"23:59.9999\"",
                               24,   0,    0,    0,   0,     0, false, false },

  {  L_, "\"25:00:00.000\"",
                               24,   0,    0,    0,   0,     0, false, false },
  {  L_, "\"00:61:00.000\"",
                               24,   0,    0,    0,   0,     0, false, false },
  {  L_, "\"00:00:61.000\"",
                               24,   0,    0,    0,   0,     0, false, false },

  {  L_, "\"0001-01-01+00:00\"",
                               24,   0,    0,    0,   0,     0, false, false },
  {  L_, "\"0001-01-01-00:00\"",
                               24,   0,    0,    0,   0,     0, false, false },

  {  L_, "\"25:00:00.000+00:00\"",
                               24,   0,    0,    0,   0,     0, false, false },
  {  L_, "\"00:61:00.000+00:00\"",
                               24,   0,    0,    0,   0,     0, false, false },
  {  L_, "\"00:00:61.000+00:00\"",
                               24,   0,    0,    0,   0,     0, false, false },
  {  L_, "\"00:00:00.000+24:00\"",
                               24,   0,    0,    0,   0,     0, false, false },
  {  L_, "\"00:00:00.000+00:61\"",
                               24,   0,    0,    0,   0,     0, false, false },
  {  L_, "\"00:00:00.000-24:00\"",
                               24,   0,    0,    0,   0,     0, false, false },
  {  L_, "\"00:00:00.000-00:61\"",
                               24,   0,    0,    0,   0,     0, false, false },

  {  L_, "\"0001-01-01T00:00:00.000+00:00\"",
                               24,   0,    0,    0,   0,     0, false, false },
        };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int         LINE        = DATA[i].d_line;
                const string      INPUT       = DATA[i].d_input_p;
                const int         HOUR        = DATA[i].d_hour;
                const int         MINUTE      = DATA[i].d_minutes;
                const int         SECOND      = DATA[i].d_seconds;
                const int         MILLISECOND = DATA[i].d_milliSecs;
                const int         MICROSECOND = DATA[i].d_uSecs;
                const int         OFFSET      = DATA[i].d_offset;
                const bool        IS_VALID    = DATA[i].d_isValid;
                const bool        IS_TIMETZ   = DATA[i].d_isTimeTz;


                const bdlt::Time   DEFAULT_TIME;
                const bdlt::Time   EXP_TIME(HOUR,
                                            MINUTE,
                                            SECOND,
                                            MILLISECOND,
                                            MICROSECOND);
                const bdlt::TimeTz DEFAULT_TIMETZ;
                const bdlt::TimeTz EXP_TIMETZ(EXP_TIME, OFFSET);

                if (veryVerbose) { T_ P_(LINE) P(INPUT) }

                const bsl::string_view isb(INPUT);

                // Variant is left unset.

                Util::TimeOrTimeTz value;
                ASSERTV(LINE, value.isUnset());

                int rc = Util::getValue(&value, isb);

                if (IS_VALID) {
                    ASSERTV(LINE, rc, 0 == rc);
                    if (IS_TIMETZ) {
                        ASSERTV(LINE, value.is<bdlt::TimeTz>());
                        ASSERTV(LINE, EXP_TIMETZ, value.the<bdlt::TimeTz>(),
                                EXP_TIMETZ == value.the<bdlt::TimeTz>());
                    }
                    else {
                        ASSERTV(LINE, value.is<bdlt::Time>());
                        ASSERTV(LINE, EXP_TIME, value.the<bdlt::Time>(),
                                EXP_TIME == value.the<bdlt::Time>());
                    }
                }
                else {
                    ASSERTV(LINE, rc, rc);
                    ASSERTV(LINE, value.isUnset());
                }

                // 'bdlt::Time' value is initially assigned.

                value = DEFAULT_TIME;
                ASSERTV(LINE, value.is<bdlt::Time>());
                ASSERTV(LINE, DEFAULT_TIME == value.the<bdlt::Time>());

                rc = Util::getValue(&value, isb);

                if (IS_VALID) {
                    ASSERTV(LINE, rc, 0 == rc);
                    if (IS_TIMETZ) {
                        ASSERTV(LINE, value.is<bdlt::TimeTz>());
                        ASSERTV(LINE, EXP_TIMETZ, value.the<bdlt::TimeTz>(),
                                EXP_TIMETZ == value.the<bdlt::TimeTz>());
                    }
                    else {
                        ASSERTV(LINE, value.is<bdlt::Time>());
                        ASSERTV(LINE, EXP_TIME, value.the<bdlt::Time>(),
                                EXP_TIME == value.the<bdlt::Time>());
                    }
                }
                else {
                    ASSERTV(LINE, rc, rc);
                    ASSERTV(LINE, value.is<bdlt::Time>());
                    ASSERTV(LINE, DEFAULT_TIME == value.the<bdlt::Time>());
                }

                // 'bdlt::TimeTz' value is initially assigned.

                value = DEFAULT_TIMETZ;
                ASSERTV(LINE, value.is<bdlt::TimeTz>());
                ASSERTV(LINE, DEFAULT_TIMETZ == value.the<bdlt::TimeTz>());

                rc = Util::getValue(&value, isb);

                if (IS_VALID) {
                    ASSERTV(LINE, rc, 0 == rc);
                    if (IS_TIMETZ) {
                        ASSERTV(LINE, value.is<bdlt::TimeTz>());
                        ASSERTV(LINE, EXP_TIMETZ, value.the<bdlt::TimeTz>(),
                                EXP_TIMETZ == value.the<bdlt::TimeTz>());
                    }
                    else {
                        ASSERTV(LINE, value.is<bdlt::Time>());
                        ASSERTV(LINE, EXP_TIME, value.the<bdlt::Time>(),
                                EXP_TIME == value.the<bdlt::Time>());
                    }
                }
                else {
                    ASSERTV(LINE, rc, rc);
                    ASSERTV(LINE, value.is<bdlt::TimeTz>());
                    ASSERTV(LINE, DEFAULT_TIMETZ == value.the<bdlt::TimeTz>());
                }
            }
        }
      } break;
      case 25: {
        // --------------------------------------------------------------------
        // DATE AND TIME TYPES ROUND-TRIP
        //
        // Concerns:
        //: 1 Date/time values decode back to the same value.
        //
        // Plan:
        //: 1 Use the table-driven technique:
        //:
        //:   1 Specify a set of valid values.
        //:
        //:   2 Encode, then decode each value and verify the decoded value is
        //:     the same..
        //:
        //: 2 Perform step one for every date/time type.
        //
        // Testing:
        //   DATE AND TIME TYPES ROUND-TRIP
        // --------------------------------------------------------------------

        if (verbose) cout << "\nDATE AND TIME TYPES ROUND-TRIP"
                          << "\n==============================" << endl;

        const struct {
            int d_line;
            int d_year;
            int d_month;
            int d_day;
            int d_hour;
            int d_minute;
            int d_second;
            int d_millisecond;
            int d_microsecond;
            int d_offset;
        } DATA[] = {
            //Line Year   Mon  Day  Hour  Min  Sec     ms   us   offset
            //---- ----   ---  ---  ----  ---  ---     --   --   ------

            // Valid dates and times
            { L_,     1,   1,   1,    0,   0,   0,     0,    0,      0 },
            { L_,  2005,   1,   1,    0,   0,   0,     0,    0,    -90 },
            { L_,   123,   6,  15,   13,  40,  59,     0,    0,   -240 },
            { L_,  1999,  10,  12,   23,   0,   1,     0,    0,   -720 },

            // Vary milliseconds
            { L_,  1999,  10,  12,   23,   0,   1,     0,    0,     90 },
            { L_,  1999,  10,  12,   23,   0,   1,   456,    0,    240 },
            { L_,  1999,  10,  12,   23,   0,   1,   456,  789,    240 },
            { L_,  1999,  10,  12,   23,   0,   1,   999,  789,    720 },
            { L_,  1999,  12,  31,   23,  59,  59,   999,  999,    720 }
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int LINE        = DATA[ti].d_line;
            const int YEAR        = DATA[ti].d_year;
            const int MONTH       = DATA[ti].d_month;
            const int DAY         = DATA[ti].d_day;
            const int HOUR        = DATA[ti].d_hour;
            const int MINUTE      = DATA[ti].d_minute;
            const int SECOND      = DATA[ti].d_second;
            const int MILLISECOND = DATA[ti].d_millisecond;
            const int MICROSECOND = DATA[ti].d_microsecond;
            const int OFFSET      = DATA[ti].d_offset;;

            const bdlt::Date       DATE(YEAR, MONTH, DAY);
            const bdlt::Time       TIME(HOUR, MINUTE, SECOND,
                                         MILLISECOND);
            const bdlt::Datetime   DATETIME(YEAR, MONTH, DAY,
                                            HOUR, MINUTE, SECOND,
                                            MILLISECOND, MICROSECOND);

            const bdlt::DateTz     DATETZ(DATE, OFFSET);
            const bdlt::TimeTz     TIMETZ(TIME, OFFSET);
            const bdlt::DatetimeTz DATETIMETZ(DATETIME, OFFSET);

            // Options necessary to round-trip.
            baljsn::EncoderOptions options;
            options.setDatetimeFractionalSecondPrecision(6);

            if (verbose) cout << "Round-trip 'Date'" << endl;
            {
                bsl::ostringstream oss;
                ASSERTV(LINE, 0 == Print::printValue(oss, DATE, &options));

                const bsl::string result(oss.str());

                bdlt::Date decoded;
                ASSERTV(LINE, 0 == Util::getValue(&decoded, result));
                ASSERTV(LINE, DATE, result, decoded, DATE == decoded);
            }

            if (verbose) cout << "Round-trip 'DateTz'" << endl;
            {
                bsl::ostringstream oss;
                ASSERTV(LINE, 0 == Print::printValue(oss, DATETZ, &options));

                const bsl::string result(oss.str());

                bdlt::DateTz decoded;
                ASSERTV(LINE, 0 == Util::getValue(&decoded, result));
                ASSERTV(LINE, DATETZ, result, decoded, DATETZ == decoded);
            }

            if (verbose) cout << "Round-trip 'Time'" << endl;
            {
                bsl::ostringstream oss;
                ASSERTV(LINE, 0 == Print::printValue(oss, TIME, &options));

                const bsl::string result(oss.str());

                bdlt::Time decoded;
                ASSERTV(LINE, 0 == Util::getValue(&decoded, result));
                ASSERTV(LINE, TIME, result, decoded, TIME == decoded);
            }

            if (verbose) cout << "Round-trip 'TimeTz'" << endl;
            {
                bsl::ostringstream oss;
                ASSERTV(LINE, 0 == Print::printValue(oss, TIMETZ, &options));

                const bsl::string result(oss.str());

                bdlt::TimeTz decoded;
                ASSERTV(LINE, 0 == Util::getValue(&decoded, result));
                ASSERTV(LINE, TIMETZ, result, decoded, TIMETZ == decoded);
            }

            if (verbose) cout << "Round-trip 'Datetime'" << endl;
            {
                bsl::ostringstream oss;
                ASSERTV(LINE, 0 == Print::printValue(oss, DATETIME, &options));

                const bsl::string result(oss.str());

                bdlt::Datetime decoded;
                ASSERTV(LINE, 0 == Util::getValue(&decoded, result));
                ASSERTV(LINE, DATETIME, result, decoded, DATETIME == decoded);
            }

            if (verbose) cout << "Round-trip 'DatetimeTz'" << endl;
            {
                bsl::ostringstream oss;
                ASSERTV(LINE,
                        0 == Print::printValue(oss, DATETIMETZ, &options));

                const bsl::string result(oss.str());

                bdlt::DatetimeTz decoded;
                ASSERTV(LINE, 0 == Util::getValue(&decoded, result));
                ASSERTV(LINE, DATETIMETZ, result, decoded,
                        DATETIMETZ == decoded);
            }
        }
      } break;
      case 24: {
        // --------------------------------------------------------------------
        // NUMBERS ROUND-TRIP
        //
        // Concerns:
        //: 1 Numbers encoded without precisions round trip to the same number.
        //:
        //: 2 'unsigned char' values are treated as numbers.
        //
        // Plan:
        //: 1 Use the table-driven technique:
        //:
        //:   1 Specify a set of valid values, including those that will test
        //:     the precision of the output.
        //:
        //:   2 Encode, then decode each value and verify that the decoded
        //:     value is as expected.
        //
        // Testing:
        //  NUMBERS ROUND-TRIP
        // --------------------------------------------------------------------

        if (verbose) cout << "\nNUMBERS ROUND-TRIP"
                          << "\n==================" << endl;

        if (verbose) cout << "Round-trip 'float'" << endl;
        {
            typedef bsl::numeric_limits<float> Limits;

            const float neg0 = copysignf(0.0f, -1.0f);

            const struct {
                int   d_line;
                float d_value;
            } DATA[] = {
                //LINE        VALUE
                //----  -------------
                { L_,           0.0f },
                { L_,         0.125f },
                { L_,        1.0e-1f },
                { L_,      0.123456f },
                { L_,           1.0f },
                { L_,           1.5f },
                { L_,          10.0f },
                { L_,         1.5e1f },
                { L_,   1.23456e-20f },
                { L_,   0.123456789f },
                { L_,  0.1234567891f },

                { L_,           neg0 },
                { L_,        -0.125f },
                { L_,       -1.0e-1f },
                { L_,     -0.123456f },
                { L_,          -1.0f },
                { L_,          -1.5f },
                { L_,         -10.0f },
                { L_,        -1.5e1f },
                { L_,  -1.23456e-20f },
                { L_,  -0.123456789f },
                { L_, -0.1234567891f },

                // {DRQS 165162213} regression, 2^24 loses precision as float
                { L_, 1.0f * 0xFFFFFF },

                // Full Mantissa Integers
                { L_, 1.0f * 0xFFFFFF },
                { L_, 1.0f * 0xFFFFFF     // this happens to be also
                       * (1ull << 63)     // 'NumLimits::max()'
                       * (1ull << 41) },

                // Boundary Values
                { L_,  Limits::min()        },
                { L_,  Limits::denorm_min() },
                { L_,  Limits::max()        },
                { L_, -Limits::min()        },
                { L_, -Limits::denorm_min() },
                { L_, -Limits::max()        },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int   LINE  = DATA[ti].d_line;
                const float VALUE = DATA[ti].d_value;

                bsl::ostringstream oss;
                ASSERTV(LINE, 0 == Print::printValue(oss, VALUE));

                bsl::string result(oss.str());

                float decoded;
                ASSERTV(LINE, 0 == Util::getValue(&decoded, result));
                ASSERTV(LINE, VALUE, result, decoded, VALUE == decoded);
                // We also use 'memcmp' to ensure that we really get back the
                // same binary IEEE-754.
                ASSERTV(LINE, VALUE, result, decoded,
                        0 == bsl::memcmp(&VALUE, &decoded, sizeof VALUE));

                // Verify that default-options-encoding round-trips as well
                baljsn::EncoderOptions options;
                oss.str("");
                ASSERTV(LINE, 0 == Print::printValue(oss, VALUE, &options));

                result = oss.str();
                ASSERTV(LINE, 0 == Util::getValue(&decoded, result));
                ASSERTV(LINE, VALUE, result, decoded, VALUE == decoded);
                // We also use 'memcmp' to ensure that we really get back the
                // same binary IEEE-754.
                ASSERTV(LINE, VALUE, result, decoded,
                        0 == bsl::memcmp(&VALUE, &decoded, sizeof VALUE));

                // Verify that 'maxFloatPrecision == 0' round-trips as well
                options.setMaxFloatPrecision(0);
                oss.str("");
                ASSERTV(LINE, 0 == Print::printValue(oss, VALUE, &options));

                result = oss.str();
                ASSERTV(LINE, 0 == Util::getValue(&decoded, result));
                ASSERTV(LINE, VALUE, result, decoded, VALUE == decoded);
                // We also use 'memcmp' to ensure that we really get back the
                // same binary IEEE-754.
                ASSERTV(LINE, VALUE, result, decoded,
                        0 == bsl::memcmp(&VALUE, &decoded, sizeof VALUE));
            }
        }

        if (verbose)
            cout << "Round-trip 'float' with 'maxFloatPrecision' option"
                 << endl;
        {
            float neg0 = copysignf(0.0f, -1.0f);

            const float ROUND_TRIPS = bsl::numeric_limits<float>::infinity();
                // We use infinity to indicate that with the specified encoder
                // options the exact same binary numeric value must be parsed
                // back, as 'printValue'/'getValue' do not normally allow it as
                // input.

            const struct {
                int   d_line;
                float d_value;
                int   d_maxFloatPrecision;
                float d_expected;
            } DATA[] = {
                //LINE   VALUE             PRECISION     EXPECTED
                //----   -----             ---------   ----------------

                { L_,     0.0,             1,            ROUND_TRIPS    },
                { L_,     0.0,             2,            ROUND_TRIPS    },
                { L_,    neg0,             1,            ROUND_TRIPS    },
                { L_,    neg0,             7,            ROUND_TRIPS    },
                { L_,     1.0,             1,            ROUND_TRIPS    },
                { L_,     1.0,             3,            ROUND_TRIPS    },
                { L_,    10.0,             2,            ROUND_TRIPS    },
                { L_,    10.0,             3,            ROUND_TRIPS    },
                { L_,    -1.5,             1,          -2.0f            },
                { L_,    -1.5,             2,            ROUND_TRIPS    },
                { L_,     1.0e-1f,         1,            ROUND_TRIPS    },
                { L_,     0.1234567891f,   1,           0.1f            },
                { L_,     0.1234567891f,   4,           0.1235f         },
                { L_,     0.1234567891f,   9,           0.123456791f    },

                { L_,    10.0f,            1,            ROUND_TRIPS    },
                { L_,    -1.5e1f,          1,          -20.0f           },
                { L_,    -1.23456789e-20f, 1,          -1e-20f          },
                { L_,    -1.23456789e-20f, 2,          -1.2e-20f        },
                { L_,    -1.23456789e-20f, 8,          -1.2345679e-20f  },
                { L_,    -1.23456789e-20f, 9,            ROUND_TRIPS    },
                { L_,     1.23456789e-20f, 1,           1e-20f          },
                { L_,     1.23456789e-20f, 9,           1.23456787e-20f },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int   LINE      = DATA[ti].d_line;
                const float VALUE     = DATA[ti].d_value;
                const int   PRECISION = DATA[ti].d_maxFloatPrecision;
                const float EXPECTED  = DATA[ti].d_expected;

                baljsn::EncoderOptions options;
                options.setMaxFloatPrecision(PRECISION);

                bsl::ostringstream oss;
                ASSERTV(LINE, 0 == Print::printValue(oss, VALUE, &options));

                const bsl::string result(oss.str());
                float decoded;
                ASSERTV(LINE, 0 == Util::getValue(&decoded, result));
                if (EXPECTED == ROUND_TRIPS) {
                    ASSERTV(LINE, VALUE, result, decoded, VALUE == decoded);
                    // We also use 'memcmp' to ensure that we really get back
                    // the same binary IEEE-754.
                    ASSERTV(LINE, VALUE, result, decoded,
                            0 == bsl::memcmp(&VALUE, &decoded, sizeof VALUE));
                }
                else {
                    ASSERTV(LINE, EXPECTED, result, decoded,
                            EXPECTED == decoded);
                }
            }
        }

        if (verbose) cout << "Round-trip 'float' Inf and NaN" << endl;
        {
            roundTripTestNonNumericValues<float>();
        }

        if (verbose) cout << "Round-trip 'double'" << endl;
        {
            typedef bsl::numeric_limits<double> Limits;

            double neg0 = copysign(0.0, -1.0);

            const struct {
                int    d_line;
                double d_value;
            } DATA[] = {
                //LINE            VALUE
                //----   ----------------------
                { L_,      0.0                  },
                { L_,      1.0e-1               },
                { L_,      0.125                },
                { L_,      1.0                  },
                { L_,      1.5                  },
                { L_,     10.0                  },
                { L_,      1.5e1                },
                { L_,      9.9e100              },
                { L_,      3.14e300             },
                { L_,      2.23e-308            },
                { L_,      0.12345678912345     },
                { L_,      0.12345678901234567  },
                { L_,      0.123456789012345678 },

                { L_,   neg0                    },
                { L_,     -1.0e-1               },
                { L_,     -0.125                },
                { L_,     -1.0                  },
                { L_,     -1.5                  },
                { L_,    -10.0                  },
                { L_,     -1.5e1                },
                { L_,     -9.9e100              },
                { L_,     -3.14e300             },
                { L_,     -2.23e-308            },
                { L_,     -0.12345678912345     },
                { L_,     -0.12345678901234567  },
                { L_,     -0.123456789012345678 },

                // Small Integers
                { L_,      123456789012345.     },
                { L_,      1234567890123456.    },

                // Full Mantissa Integers
                { L_, 1.0 * 0x1FFFFFFFFFFFFFull },
                { L_, 1.0 * 0x1FFFFFFFFFFFFFull  // This is also limits::max()
                      * (1ull << 63) * (1ull << 63) * (1ull << 63)
                      * (1ull << 63) * (1ull << 63) * (1ull << 63)
                      * (1ull << 63) * (1ull << 63) * (1ull << 63)
                      * (1ull << 63) * (1ull << 63) * (1ull << 63)
                      * (1ull << 63) * (1ull << 63) * (1ull << 63)
                      * (1ull << 26)            },

                // Boundary Values
                { L_,  Limits::min()        },
                { L_,  Limits::denorm_min() },
                { L_,  Limits::max()        },
                { L_, -Limits::min()        },
                { L_, -Limits::denorm_min() },
                { L_, -Limits::max()        },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int    LINE  = DATA[ti].d_line;
                const double VALUE = DATA[ti].d_value;

                bsl::ostringstream oss;
                ASSERTV(LINE, 0 == Print::printValue(oss, VALUE));

                bsl::string result(oss.str());
                double decoded;
                ASSERTV(LINE, 0 == Util::getValue(&decoded, result));
                ASSERTV(LINE, VALUE, result, decoded, VALUE == decoded);
                // We also use 'memcmp' to ensure that we really get back the
                // same binary IEEE-754.
                ASSERTV(LINE, VALUE, result, decoded,
                        0 == bsl::memcmp(&VALUE, &decoded, sizeof VALUE));


                // Verify that default-options-encoding round-trips as well
                baljsn::EncoderOptions options;
                oss.str("");
                ASSERTV(LINE, 0 == Print::printValue(oss, VALUE, &options));

                result = oss.str();
                ASSERTV(LINE, 0 == Util::getValue(&decoded, result));
                ASSERTV(LINE, VALUE, result, decoded, VALUE == decoded);
                // We also use 'memcmp' to ensure that we really get back the
                // same binary IEEE-754.
                ASSERTV(LINE, VALUE, result, decoded,
                        0 == bsl::memcmp(&VALUE, &decoded, sizeof VALUE));

                // Verify that 'maxDoublePrecision == 0' round-trips as well
                options.setMaxDoublePrecision(0);
                oss.str("");
                ASSERTV(LINE, 0 == Print::printValue(oss, VALUE, &options));

                result = oss.str();
                ASSERTV(LINE, 0 == Util::getValue(&decoded, result));
                ASSERTV(LINE, VALUE, result, decoded, VALUE == decoded);
                // We also use 'memcmp' to ensure that we really get back the
                // same binary IEEE-754.
                ASSERTV(LINE, VALUE, result, decoded,
                        0 == bsl::memcmp(&VALUE, &decoded, sizeof VALUE));
            }
        }

        if (verbose)
            cout << "Round-trip 'double' with 'maxDoublePrecision' option"
                 << endl;
        {
            typedef bsl::numeric_limits<double> Limits;

            double neg0 = copysign(0.0, -1.0);

            const double ROUND_TRIPS = Limits::infinity();
                // We use infinity to indicate that with the specified encoder
                // options the exact same binary numeric value must be parsed
                // back, as 'printValue'/'getValue' do not normally allow it as
                // input.

            const struct {
                int    d_line;
                double d_value;
                int    d_maxDoublePrecision;
                double d_expected;
            } DATA[] = {
                //                            PRECISION
                //LINE               VALUE        |         EXPECTED
                //---  -----------------------   -v-  ---------------------

                {L_,                       0.0,   1,             ROUND_TRIPS },
                {L_,                       0.0,   2,             ROUND_TRIPS },
                {L_,                      neg0,   1,             ROUND_TRIPS },
                {L_,                      neg0,  17,             ROUND_TRIPS },
                {L_,                       1.0,   1,             ROUND_TRIPS },
                {L_,                       1.0,   3,             ROUND_TRIPS },
                {L_,                      10.0,   2,             ROUND_TRIPS },
                {L_,                      10.0,   3,             ROUND_TRIPS },
                {L_,                      -1.5,   1,  -2.0                   },
                {L_,                      -1.5,   2,             ROUND_TRIPS },
                {L_,                  -9.9e100,   2,             ROUND_TRIPS },
                {L_,                  -9.9e100,  15,             ROUND_TRIPS },
                {L_,                  -9.9e100,  17,             ROUND_TRIPS },
                {L_,                 -3.14e300,  15,             ROUND_TRIPS },
                {L_,                 -3.14e300,  17,             ROUND_TRIPS },
                {L_,                  3.14e300,   2,   3.1e+300              },
                {L_,                  3.14e300,  17,             ROUND_TRIPS },
                {L_,                    1.0e-1,   1,             ROUND_TRIPS },
                {L_,                 2.23e-308,   2,   2.2e-308              },
                {L_,                 2.23e-308,  17,             ROUND_TRIPS },
                {L_,      0.123456789012345678,   1,   0.1                   },
                {L_,      0.123456789012345678,   2,   0.12                  },
                {L_,      0.123456789012345678,  15,   0.123456789012346     },
                {L_,      0.123456789012345678,  16,   0.1234567890123457    },
                {L_,      0.123456789012345678,  17,             ROUND_TRIPS },

                {L_,                      10.0,   1,   1e+01                 },
                {L_,                    -1.5e1,   1,  -2e+01                 },
                {L_,   -1.2345678901234567e-20,   1,  -1e-20                 },
                {L_,   -1.2345678901234567e-20,   2,  -1.2e-20               },
                {L_,   -1.2345678901234567e-20,  15,  -1.23456789012346e-20  },
                {L_,   -1.2345678901234567e-20,  16,  -1.234567890123457e-20 },
                {L_,   -1.2345678901234567e-20,  17,             ROUND_TRIPS },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int    LINE     = DATA[ti].d_line;
                const double VALUE    = DATA[ti].d_value;
                const int    PREC     = DATA[ti].d_maxDoublePrecision;
                const double EXPECTED = DATA[ti].d_expected;

                baljsn::EncoderOptions options;
                options.setMaxDoublePrecision(PREC);

                bsl::ostringstream oss;
                ASSERTV(LINE, 0 == Print::printValue(oss, VALUE, &options));

                const bsl::string result(oss.str());
                double decoded;
                ASSERTV(LINE, 0 == Util::getValue(&decoded, result));
                if (EXPECTED == ROUND_TRIPS) {
                    ASSERTV(LINE, VALUE, result, decoded, VALUE == decoded);
                    // We also use 'memcmp' to ensure that we really get back
                    // the same binary IEEE-754.
                    ASSERTV(LINE, VALUE, result, decoded,
                            0 == bsl::memcmp(&VALUE, &decoded, sizeof VALUE));
                }
                else {
                    ASSERTV(LINE, EXPECTED, result, decoded,
                            EXPECTED == decoded);
                }
            }
        }

        if (verbose) cout << "Round-trip 'double' Inf and NaN" << endl;
        {
            roundTripTestNonNumericValues<double>();
        }

        if (verbose) cout << "Round-trip 'Decimal64'" << endl;
        {
#define DEC(X) BDLDFP_DECIMAL_DD(X)

            typedef bsl::numeric_limits<bdldfp::Decimal64> Limits;

            const struct {
                int               d_line;
                bdldfp::Decimal64 d_value;
            } DATA[] = {
                //LINE  VALUE
                //----  -----------------------------
                {L_,    DEC( 0.0),                    },
                {L_,    DEC(-0.0),                    },
                {L_,    DEC( 1.13),                   },
                {L_,    DEC(-9.876543210987654e307)   },
                {L_,    DEC(-9.8765432109876548e307)  },
                {L_,    DEC(-9.87654321098765482e307) },

                // Boundary values
                { L_,    Limits::min()        },
                { L_,    Limits::denorm_min() },
                { L_,    Limits::max()        },
                { L_,   -Limits::min()        },
                { L_,   -Limits::denorm_min() },
                { L_,   -Limits::max()        },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int               LINE  = DATA[ti].d_line;
                const bdldfp::Decimal64 VALUE = DATA[ti].d_value;

                bsl::ostringstream oss;
                ASSERTV(LINE, 0 == Print::printValue(oss, VALUE));

                bsl::string result(oss.str());
                bdldfp::Decimal64 decoded;
                ASSERTV(LINE, 0 == Util::getValue(&decoded, result));
                ASSERTV(LINE, VALUE, result, decoded, VALUE == decoded);
                // We also use 'memcmp' to ensure that we really get back the
                // same binary IEEE-754.
                ASSERTV(LINE, VALUE, result, decoded,
                        0 == bsl::memcmp(&VALUE, &decoded, sizeof VALUE));

                // Verify that default options are the same as no options
                //*** NOT done because the default without options is different

                // Verify that 'setEncodeQuotedDecimal64' 'false' behaves the
                // same way as no options in that it round trips.
                baljsn::EncoderOptions options;
                options.setEncodeQuotedDecimal64(false);

                oss.str("");
                ASSERTV(LINE, 0 == Print::printValue(oss, VALUE, &options));

                result = oss.str();
                ASSERTV(LINE, 0 == Util::getValue(&decoded, result));
                ASSERTV(LINE, VALUE, result, decoded, VALUE == decoded);
                // We also use 'memcmp' to ensure that we really get back the
                // same binary IEEE-754.
                ASSERTV(LINE, VALUE, result, decoded,
                        0 == bsl::memcmp(&VALUE, &decoded, sizeof VALUE));

                // Verify that 'setEncodeQuotedDecimal64' 'true' quotes the
                // numbers as if they were strings
                options.setEncodeQuotedDecimal64(true);

                oss.str("");
                ASSERTV(LINE, 0 == Print::printValue(oss, VALUE, &options));

                result = oss.str();
                ASSERTV(LINE, 0 == Util::getValue(&decoded, result));
                ASSERTV(LINE, VALUE, result, decoded, VALUE == decoded);
                // We also use 'memcmp' to ensure that we really get back the
                // same binary IEEE-754.
                ASSERTV(LINE, VALUE, result, decoded,
                        0 == bsl::memcmp(&VALUE, &decoded, sizeof VALUE));
            }
#undef DEC
        }

        if (verbose) cout << "Round-trip 'Decimal64' Inf and NaN" << endl;
        {
            roundTripTestNonNumericValues<bdldfp::Decimal64>();
        }

        if (verbose) cout << "Encode int" << endl;
        {
            testIntegerTypeRoundTrip<char>();
            testIntegerTypeRoundTrip<short>();
            testIntegerTypeRoundTrip<int>();
            testIntegerTypeRoundTrip<Int64>();
            testIntegerTypeRoundTrip<unsigned char>();
            testIntegerTypeRoundTrip<unsigned short>();
            testIntegerTypeRoundTrip<unsigned int>();
            testIntegerTypeRoundTrip<Uint64>();
        }
      } break;
      case 23: {
        // --------------------------------------------------------------------
        // STRINGS ROUND-TRIP
        //
        // Concerns:
        //: 1 Encoded single characters string read back as .
        //:
        //: 2 All escape characters are encoded corrected.
        //:
        //: 3 Control characters round-trip.
        //
        // Plan:
        //: 1 Using the table-driven technique:
        //:
        //:   1 Specify a set of values that include all escaped characters and
        //:     some control characters.
        //:
        //:   2 Encode the value and verify the results.
        //
        // Testing:
        //  static int printValue(bsl::ostream& s, const char             *v);
        //  static int printValue(bsl::ostream& s, const bsl::string&      v);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nSTRINGS ROUND-TRIP"
                          << "\n==================" << endl;

        const struct {
            int         d_line;
            const char *d_value;
        } DATA[] = {
            //LINE   VALUE
            //----   -----
            { L_,    ""     },
            { L_,    " "    },
            { L_,    "~"    },

            // Text
            { L_,    "test" },
            { L_,    "A quick brown fox jump over a lazy dog!" },

            // Escape sequences
            { L_,    "\""   },
            { L_,    "\\"   },
            { L_,    "/"    },
            { L_,    "\b"   },
            { L_,    "\f"   },
            { L_,    "\n"   },
            { L_,    "\r"   },
            { L_,    "\t"   },
            { L_,    "\\/\b\f\n\r\t" },  // Combination of escape sequences

            { L_,    "\xc2\x80"         },
            { L_,    "\xdf\xbf"         },
            { L_,    "\xe0\xa0\x80"     },
            { L_,    "\xef\xbf\xbf"     },
            { L_,    "\xf0\x90\x80\x80" },
            { L_,    "\xf4\x8f\xbf\xbf" },

            { L_,    "\x01" },
            { L_,    "\x02" },
            { L_,    "\x03" },
            { L_,    "\x04" },
            { L_,    "\x05" },
            { L_,    "\x06" },
            { L_,    "\x07" },
            { L_,    "\x08" },  // Backspace
            { L_,    "\x09" },  // Horizontal tab
            { L_,    "\x0A" },  // New line
            { L_,    "\x0B" },  // Vertical tab
            { L_,    "\x0C" },  // Form feed
            { L_,    "\x0D" },  // Carriage return
            { L_,    "\x0E" },
            { L_,    "\x0F" },
            { L_,    "\x10" },
            { L_,    "\x11" },
            { L_,    "\x12" },
            { L_,    "\x13" },
            { L_,    "\x14" },
            { L_,    "\x15" },  // NACK (Negative ACK)
            { L_,    "\x16" },
            { L_,    "\x17" },
            { L_,    "\x18" },  // Cancel
            { L_,    "\x19" },
            { L_,    "\x1A" },
            { L_,    "\x1B" },  // Escape
            { L_,    "\x1C" },
            { L_,    "\x1D" },
            { L_,    "\x1E" },
            { L_,    "\x1F" },
            { L_,    "\x20" }   // Space
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE  = DATA[ti].d_line;
            const char *const VALUE = DATA[ti].d_value;

            bsl::ostringstream oss;
            ASSERTV(LINE, 0 == Print::printValue(oss, VALUE));

            const bsl::string result(oss.str());
            bsl::string decoded;
            ASSERTV(0 == Util::getValue(&decoded, result));
            ASSERTV(result, decoded, VALUE == decoded);
        }
      } break;
      case 22: {
        // --------------------------------------------------------------------
        // BOOLEAN ROUND-TRIP
        //
        // Concerns:
        //: 1 Both 'bool' values read back as the same value when printed.
        //
        // Plan:
        //: 1 Use a brute force approach to test both cases.
        //
        // Testing:
        //   BOOLEAN ROUND-TRIP
        // --------------------------------------------------------------------

        if (verbose) cout << "\nBOOLEAN ROUND-TRIP"
                          << "\n==================" << endl;

        if (verbose) cout << "Round-trip 'true'" << endl;
        {
            bsl::ostringstream oss;
            ASSERTV(0 == Print::printValue(oss, true));

            {
                const bsl::string result = oss.str();

                bool decoded;
                ASSERTV(0 == Util::getValue(&decoded, result));
                ASSERTV(result, decoded, true == decoded);
            }

            // Verify that passing in options to encode round-trips as well
            const baljsn::EncoderOptions defaultOptions;

            oss.str("");
            ASSERT(0 == Print::printValue(oss, true, &defaultOptions));

            {
                const bsl::string result = oss.str();

                bool decoded;
                ASSERTV(0 == Util::getValue(&decoded, result));
                ASSERTV(result, decoded, true == decoded);
            }
        }

        if (verbose) cout << "Encode 'false'" << endl;
        {
            bsl::ostringstream oss;
            ASSERTV(0 == Print::printValue(oss, false));

            {
                const bsl::string result = oss.str();

                bool decoded;
                ASSERTV(0 == Util::getValue(&decoded, result));
                ASSERTV(result, decoded, false == decoded);
            }

            // Verify that passing in options to encode round-trips as well
            const baljsn::EncoderOptions defaultOptions;

            oss.str("");
            ASSERT(0 == Print::printValue(oss, false, &defaultOptions));

            {
                const bsl::string result = oss.str();

                bool decoded;
                ASSERTV(0 == Util::getValue(&decoded, result));
                ASSERTV(result, decoded, false == decoded);
            }
        }
      } break;
      case 21: {
        // --------------------------------------------------------------------
        // TESTING 'getValue' for Decimal64 values
        //
        // Concerns:
        //: 1 Values in the valid range are parsed correctly.  Note that we do
        //:   not need to perform an exhaustive test, because we assume the
        //:   underlying parse function used 'DecimalUtil::parseDecimal64' is
        //:   implemented correctly.
        //:
        //: 2 The passed in variable is unmodified if the data is not valid.
        //:
        //: 3 The return code is 0 on success and non-zero on failure.
        //
        // Plan:
        //: 1 Using the table-driven technique, specify a set of distinct
        //:   rows of string value, expected parsed value, and return code.
        //:
        //: 2 For each row in the table of P-1:
        //:
        //:   1 Provide the string value and a variable  to be parsed into to
        //:     the 'getValue' function.  The variable is assigned a sentinel
        //:     value before invoking the function.
        //:
        //:   2 If the parsing should succeed then verify that the variable
        //:     value matches the expected value.  Otherwise confirm that the
        //:     variable value is unmodified.
        //:
        //:   3 Confirm that the return code is 0 on success and non-zero
        //:     otherwise.
        //
        // Testing:
        //   static int getValue(bdldfp::Decimal64 *v, bsl::string_view s);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING 'getValue' for Decimal64"
                          << "\n================================" << endl;
        {
#define DEC(X) BDLDFP_DECIMAL_DD(X)

            typedef bdldfp::Decimal64 Type;

            const Type NAN_P = bsl::numeric_limits<Type>::quiet_NaN();
            const Type NAN_N = -NAN_P;
            const Type INF_P = bsl::numeric_limits<Type>::infinity();
            const Type INF_N = -INF_P;

            const Type ERROR_VALUE = BDLDFP_DECIMAL_DD(999.0);

            static const struct {
                int         d_line;    // line number
                const char *d_input_p; // input on the stream
                Type        d_exp;     // exp unsigned value
                bool        d_isValid; // isValid flag
            } DATA[] = {
     // line  input                       exp                         isValid
     // ----  -----                       ---                         -------
     {  L_,    "0",                       DEC(0.0),                    true  },
     {  L_,   "-0",                       DEC(0.0),                    true  },
     {  L_,    "0.0",                     DEC(0.0),                    true  },
     {  L_,   "-0.0",                     DEC(0.0),                    true  },
     {  L_,    "1",                       DEC(1.0),                    true  },
     {  L_,   "-1",                       DEC(-1.0),                   true  },
     {  L_,    "1.2",                     DEC(1.2),                    true  },
     {  L_,    "1.23",                    DEC(1.23),                   true  },
     {  L_,    "1.234",                   DEC(1.234),                  true  },
     {  L_,   "12.34",                    DEC(12.34),                  true  },
     {  L_,  "123.4",                     DEC(123.4),                  true  },
     {  L_,   "-1.2",                     DEC(-1.2),                   true  },
     {  L_,   "-1.23",                    DEC(-1.23),                  true  },
     {  L_,   "-1.234",                   DEC(-1.234),                 true  },
     {  L_,  "-12.34",                    DEC(-12.34),                 true  },
     {  L_, "-123.4",                     DEC(-123.4),                 true  },
     {  L_,   "+1.2",                     DEC(1.2),                    true  },
     {  L_,   "+1.23",                    DEC(1.23),                   true  },
     {  L_,   "+1.234",                   DEC(1.234),                  true  },
     {  L_,  "+12.34",                    DEC(12.34),                  true  },
     {  L_, "+123.4",                     DEC(123.4),                  true  },
     {  L_,   "-9.876543210987654e307",   DEC(-9.876543210987654e307), true  },
     {  L_, "\"-0.1\"",                   DEC(-0.1),                   true  },
     {  L_,  "\"0\"",                     DEC(0.0),                    true  },
     {  L_, "\"-0\"",                     DEC(0.0),                    true  },
     {  L_,  "\"0.0\"",                   DEC(0.0),                    true  },
     {  L_, "\"-0.0\"",                   DEC(0.0),                    true  },
     {  L_,  "\"1\"",                     DEC(1.0),                    true  },
     {  L_, "\"-1\"",                     DEC(-1.0),                   true  },
     {  L_,  "\"1.2\"",                   DEC(1.2),                    true  },
     {  L_,  "\"1.23\"",                  DEC(1.23),                   true  },
     {  L_,  "\"1.234\"",                 DEC(1.234),                  true  },
     {  L_, "\"12.34\"",                  DEC(12.34),                  true  },
     {  L_, "\"123.4\"",                  DEC(123.4),                  true  },
     {  L_, "\"-1.2\"",                   DEC(-1.2),                   true  },
     {  L_, "\"-1.23\"",                  DEC(-1.23),                  true  },
     {  L_, "\"-1.234\"",                 DEC(-1.234),                 true  },
     {  L_, "\"-12.34\"",                 DEC(-12.34),                 true  },
     {  L_, "\"-123.4\"",                 DEC(-123.4),                 true  },
     {  L_, "\"+1.2\"",                   DEC(1.2),                    true  },
     {  L_, "\"+1.23\"",                  DEC(1.23),                   true  },
     {  L_, "\"+1.234\"",                 DEC(1.234),                  true  },
     {  L_, "\"+12.34\"",                 DEC(12.34),                  true  },
     {  L_, "\"+123.4\"",                 DEC(123.4),                  true  },
     {  L_, "\"-9.876543210987654e307\"", DEC(-9.876543210987654e307), true  },
     {  L_,   "-0.1",                     DEC(-0.1),                   true  },
     {  L_,  "\"NaN\"",                   NAN_P,                       true  },
     {  L_,  "\"nan\"",                   NAN_P,                       true  },
     {  L_,  "\"NAN\"",                   NAN_P,                       true  },
     {  L_, "\"+NaN\"",                   NAN_P,                       true  },
     {  L_, "\"+nan\"",                   NAN_P,                       true  },
     {  L_, "\"+NAN\"",                   NAN_P,                       true  },
     {  L_, "\"-NaN\"",                   NAN_N,                       true  },
     {  L_, "\"-nan\"",                   NAN_N,                       true  },
     {  L_, "\"-NAN\"",                   NAN_N,                       true  },
     {  L_,  "\"INF\"",                   INF_P,                       true  },
     {  L_,  "\"inf\"",                   INF_P,                       true  },
     {  L_,  "\"infinity\"",              INF_P,                       true  },
     {  L_, "\"+INF\"",                   INF_P,                       true  },
     {  L_, "\"+inf\"",                   INF_P,                       true  },
     {  L_, "\"+infinity\"",              INF_P,                       true  },
     {  L_, "\"-INF\"",                   INF_N,                       true  },
     {  L_, "\"-inf\"",                   INF_N,                       true  },
     {  L_, "\"-infinity\"",              INF_N,                       true  },
     {  L_,         "-",                  ERROR_VALUE,                 false },
     {  L_,       "E-1",                  ERROR_VALUE,                 false },
     {  L_,  "Z34.56e1",                  ERROR_VALUE,                 false },
     {  L_,  "3Z4.56e1",                  ERROR_VALUE,                 false },
     {  L_,      "1.1}",                  ERROR_VALUE,                 false },
     {  L_,     "1.1\n",                  ERROR_VALUE,                 false },
     {  L_,   "1.10xFF",                  ERROR_VALUE,                 false },
     {  L_,  "DEADBEEF",                  ERROR_VALUE,                 false },
     {  L_,      "JUNK",                  ERROR_VALUE,                 false },
     {  L_,     "\"0\"",                  DEC(0.0),                    true  },
     {  L_,       "0\"",                  ERROR_VALUE,                 false },
     {  L_,       "\"0",                  ERROR_VALUE,                 false },
     {  L_,        "\"",                  ERROR_VALUE,                 false },
     {  L_,      "\"\"",                  ERROR_VALUE,                 false },
     {  L_,     "\"X\"",                  ERROR_VALUE,                 false },
     {  L_,  "\" NaN\"",                  ERROR_VALUE,                 false },
            };
            const int NUM_DATA = sizeof(DATA) / sizeof(*DATA);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int    LINE     = DATA[i].d_line;
                const string INPUT    = DATA[i].d_input_p;
                const Type   EXP      = DATA[i].d_exp;
                const bool   IS_VALID = DATA[i].d_isValid;
                      Type   value    = ERROR_VALUE;

                const bsl::string_view isb(INPUT);

                bslma::TestAllocator         da("default", veryVeryVerbose);
                bslma::DefaultAllocatorGuard dag(&da);

                const int rc = Util::getValue(&value, isb);
                if (IS_VALID) {
                    LOOP2_ASSERT(LINE, rc, 0 == rc);
                }
                else {
                    LOOP2_ASSERT(LINE, rc, rc);
                }

                if (IS_VALID && bdldfp::DecimalUtil::isNan(EXP)) {
                    LOOP_ASSERT(LINE, bdldfp::DecimalUtil::isNan(value));
                }
                else {
                    LOOP3_ASSERT(LINE, EXP, value, EXP == value);
                }

                ASSERTV(LINE, da.numBlocksTotal(), 0 == da.numBlocksTotal());
            }
#undef DEC
        }
      } break;
      case 20: {
        // --------------------------------------------------------------------
        // TESTING 'getValue' for vector<char> values
        //
        // Concerns:
        //: 1 Valid base64-encoded values are parsed correctly.
        //:
        //: 2 The passed in variable is unmodified if the data is not valid.
        //:
        //: 3 The return code is 0 on success and non-zero on failure.
        //
        // Plan:
        //: 1 Using the table-driven technique, specify a set of distinct
        //:   rows of string value, expected parsed value, and return code.
        //:
        //: 2 For each row in the table of P-1:
        //:
        //:   1 Provide the string value and a variable  to be parsed into to
        //:     the 'getValue' function.  The variable is assigned a sentinel
        //:     value before invoking the function.
        //:
        //:   2 If the parsing should succeed then verify that the variable
        //:     value matches the expected value.  Otherwise confirm that the
        //:     variable value is unmodified.
        //:
        //:   3 Confirm that the return code is 0 on success and non-zero
        //:     otherwise.
        //
        // Testing:
        //   static int getValue(vector<char>     *v, bsl::string_view s);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING 'getValue' for 'vector<char>'"
                          << "\n======================================"
                          << endl;
        {
            static const struct {
                int         d_line;      // source line number
                const char *d_input_p;   // input
                const char *d_output_p;  // output
                int         d_outputLen; // output length
                bool        d_isValid;   // isValid flag
            } DATA[] = {
          {  L_,  "\"\"",            "",                   0, true  },

          {  L_,  "\"Ug==\"",        "R",                  1, true  },
          {  L_,  "\"QVY=\"",        "AV",                 2, true  },

          {  L_,  "\"AA==\"",        "\x00",               1, true  },
          {  L_,  "\"AQ==\"",        "\x01",               1, true  },
          {  L_,  "\"\\/w==\"",      "\xFF",               1, true  },

          {  L_,  "\"UVE=\"",        "QQ",                 2, true  },

          {  L_,  "\"YQ==\"",        "a",                  1, true  },
          {  L_,  "\"YWI=\"",        "ab",                 2, true  },
          {  L_,  "\"YWJj\"",        "abc",                3, true  },
          {  L_,  "\"YWJjZA==\"",    "abcd",               4, true  },

          {  L_,  "\"Qmxvb21iZXJnTFA=\"", "BloombergLP",  11, true  },

          {  L_,  "",                "",                   0, false },
          {  L_,  "\"Q\"",           "",                   0, false },
          {  L_,  "\"QV\"",          "",                   0, false },
          {  L_,  "\"QVE\"",         "",                   0, false },
          {  L_,  "\"QVE==\"",       "",                   0, false },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int         LINE        = DATA[i].d_line;
                const char       *INPUT       = DATA[i].d_input_p;
                const char       *OUTPUT      = DATA[i].d_output_p;
                const int         LEN         = DATA[i].d_outputLen;
                const bool        IS_VALID    = DATA[i].d_isValid;

                vector<char> exp(OUTPUT, OUTPUT + LEN);
                const vector<char>& EXP = exp;

                vector<char> value;

                bslma::TestAllocator da(veryVeryVerbose);
                bslma::DefaultAllocatorGuard guard(&da);

                const bsl::string_view isb(INPUT);

                const int rc = Util::getValue(&value, isb);
                if (IS_VALID) {
                    LOOP2_ASSERT(LINE, rc, 0 == rc);

                    bool result = EXP == value;
                    LOOP_ASSERT(LINE, result);
                    if (!result) {
                        cout << "EXP: ";
                        bdlb::PrintMethods::print(cout, EXP, 0, -1);
                        cout << endl << "VALUE: ";
                        bdlb::PrintMethods::print(cout, value, 0, -1);
                        cout << endl;
                    }
                }
                else {
                    LOOP2_ASSERT(LINE, rc, rc);
                }

                ASSERT(0 == da.numBlocksTotal());
            }
        }
      } break;
      case 19: {
        // --------------------------------------------------------------------
        // TESTING 'getValue' for bdlt::DatetimeTz values
        //
        // Concerns:
        //: 1 Values in the valid range, including the maximum and minimum
        //:   values for this type, are parsed correctly.
        //:
        //: 2 The passed in variable is unmodified if the data is not valid.
        //:
        //: 3 The return code is 0 on success and non-zero on failure.
        //
        // Plan:
        //: 1 Using the table-driven technique, specify a set of distinct
        //:   rows of string value, expected parsed value, and return code.
        //:
        //: 2 For each row in the table of P-1:
        //:
        //:   1 Provide the string value and a variable  to be parsed into to
        //:     the 'getValue' function.  The variable is assigned a sentinel
        //:     value before invoking the function.
        //:
        //:   2 If the parsing should succeed then verify that the variable
        //:     value matches the expected value.  Otherwise confirm that the
        //:     variable value is unmodified.
        //:
        //:   3 Confirm that the return code is 0 on success and non-zero
        //:     otherwise.
        //
        // Testing:
        //   static int getValue(bdlt::DatetimeTz     *v, bsl::string_view s);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING 'getValue' for 'bdlt::DatetimeTz'"
                          << "\n========================================"
                          << endl;
        {
            static const struct {
                int         d_line;      // source line number
                const char *d_input_p;   // input
                int         d_year;      // year under test
                int         d_month;     // month under test
                int         d_day;       // day under test
                int         d_hour;      // hour under test
                int         d_minutes;   // minutes under test
                int         d_seconds;   // seconds under test
                int         d_milliSecs; // milli seconds under test
                int         d_microSecs; // micro seconds under test
                int         d_tzoffset;  // time zone offset
                bool        d_isValid;   // isValid flag
            } DATA[] = {
//line  input year month day   hour    min   sec    ms    us  offset isValid
//----  ----- ---- ----- ---   ----    ---   ---    --    --  ------ -------
    {   L_, "\"0001-01-01T00:00:00.000+00:00\"",
              1,     1,    1,     0,     0,    0,    0,   0,      0,  true   },
    {   L_, "\"0001-01-01T00:00:00.000+00:45\"",
              1,     1,    1,     0,     0,    0,    0,   0,     45,  true   },
    {   L_, "\"0001-01-01T00:00:00.000-23:59\"",
              1,     1,    1,     0,     0,    0,    0,   0,  -1439,  true   },
    {   L_, "\"0001-01-01T00:00:00.000000+00:00\"",
              1,     1,    1,     0,     0,    0,    0,   0,      0,  true   },
    {   L_, "\"0001-01-01T00:00:00.000000+00:45\"",
              1,     1,    1,     0,     0,    0,    0,   0,     45,  true   },
    {   L_, "\"0001-01-01T00:00:00.000000-23:59\"",
              1,     1,    1,     0,     0,    0,    0,   0,  -1439,  true   },

    {   L_, "\"0001-01-01T01:01:01.001+00:00\"",
              1,     1,    1,     1,     1,    1,    1,   0,      0,  true   },
    {   L_, "\"0001-01-01T01:01:01.001+08:20\"",
              1,     1,    1,     1,     1,    1,    1,   0,    500,  true   },
    {   L_, "\"0001-01-01T01:01:01.001000+00:00\"",
              1,     1,    1,     1,     1,    1,    1,   0,      0,  true   },
    {   L_, "\"0001-01-01T01:01:01.001000+08:20\"",
              1,     1,    1,     1,     1,    1,    1,   0,    500,  true   },
    {   L_, "\"0001-01-01T00:00:00.000000-23:59\"",
              1,     1,    1,     0,     0,    0,    0,   0,  -1439,  true   },
    {   L_, "\"0001-01-01T01:23:59.059059+00:00\"",
              1,     1,    1,     1,    23,   59,   59,  59,      0,  true   },
    {   L_, "\"0001-01-01T01:23:59.059059+23:59\"",
              1,     1,    1,     1,    23,   59,   59,  59,   1439,  true   },
    {   L_, "\"0001-01-01T01:23:59.059059-23:59\"",
              1,     1,    1,     1,    23,   59,   59,  59,  -1439,  true   },
    {   L_, "\"0001-01-02T00:00:00.000000+00:00\"",
              1,     1,    2,     0,     0,    0,    0,   0,      0,  true   },
    {   L_, "\"0001-01-02T00:00:00.000000+23:59\"",
              1,     1,    2,     0,     0,    0,    0,   0,   1439,  true   },
    {   L_, "\"0001-01-02T00:00:00.000000-23:59\"",
              1,     1,    2,     0,     0,    0,    0,   0,  -1439,  true   },
    {   L_, "\"0001-01-02T01:01:01.000001+00:00\"",
              1,     1,    2,     1,     1,    1,    0,   1,      0,  true   },
    {   L_, "\"0001-01-02T01:01:01.000001+08:20\"",
              1,     1,    2,     1,     1,    1,    0,   1,    500,  true   },

    {   L_, "\"0001-01-02T01:23:59.059168+00:00\"",
              1,     1,    2,     1,    23,   59,   59, 168,      0,  true   },
    {   L_, "\"0001-01-02T01:23:59.059168+08:20\"",
              1,     1,    2,     1,    23,   59,   59, 168,    500,  true   },
    {   L_, "\"0001-01-02T01:23:59.059168-08:20\"",
              1,     1,    2,     1,    23,   59,   59, 168,   -500,  true   },
    {   L_, "\"0001-01-10T00:00:00.000000+00:00\"",
              1,     1,   10,     0,     0,    0,    0,   0,      0,  true   },
    {   L_, "\"0001-01-10T01:01:01.001+01:39\"",
              1,     1,   10,     1,     1,    1,    1,   0,     99,  true   },
    {   L_, "\"0001-01-10T01:01:01.000001+01:39\"",
              1,     1,   10,     1,     1,    1,    0,   1,     99,  true   },

    {   L_, "\"0001-01-30T00:00:00.000000+00:00\"",
              1,     1,   30,     0,     0,    0,    0,   0,      0,  true   },
    {   L_, "\"0001-01-31T00:00:00.000000+23:59\"",
              1,     1,   31,     0,     0,    0,    0,   0,   1439,  true   },
    {   L_, "\"0001-01-31T00:00:00.000000-23:59\"",
              1,     1,   31,     0,     0,    0,    0,   0,  -1439,  true   },

    {   L_, "\"0001-02-01T00:00:00.000000+00:00\"",
              1,     2,    1,     0,     0,    0,    0,   0,      0,  true   },
    {   L_, "\"0001-02-01T23:59:59.000000+23:59\"",
              1,     2,    1,    23,    59,   59,    0,   0,   1439,  true   },

    {   L_, "\"0001-12-31T00:00:00.000000+00:00\"",
              1,    12,   31,     0,     0,    0,    0,   0,      0,  true   },
    {   L_, "\"0001-12-31T23:59:59.000000+23:59\"",
              1,    12,   31,    23,    59,   59,    0,   0,   1439,  true   },

    {   L_, "\"0002-01-01T00:00:00.000000+00:00\"",
              2,     1,    1,     0,     0,    0,    0,   0,      0,  true   },
    {   L_, "\"0002-01-01T23:59:59.000000+23:59\"",
              2,     1,    1,    23,    59,   59,    0,   0,   1439,  true   },

    {   L_, "\"0004-01-01T00:00:00.000000+00:00\"",
              4,     1,    1,     0,     0,    0,    0,   0,      0,  true   },
    {   L_, "\"0004-01-01T23:59:59.000000+23:59\"",
              4,     1,    1,    23,    59,   59,    0,   0,   1439,  true   },

    {   L_, "\"0004-02-28T00:00:00.000000+00:00\"",
              4,     2,   28,     0,     0,    0,    0,   0,      0,  true   },
    {   L_, "\"0004-02-28T23:59:59.000000+23:59\"",
              4,     2,   28,    23,    59,   59,    0,   0,   1439,  true   },
    {   L_, "\"0004-02-28T23:59:59.000000-23:59\"",
              4,     2,   28,    23,    59,   59,    0,   0,  -1439,  true   },

    {   L_, "\"0004-02-29T00:00:00.000000+00:00\"",
              4,     2,   29,     0,     0,    0,    0,   0,      0,  true   },
    {   L_, "\"0004-02-29T23:59:59.000000+23:59\"",
              4,     2,   29,    23,    59,   59,    0,   0,   1439,  true   },
    {   L_, "\"0004-02-29T23:59:59.000000-23:59\"",
              4,     2,   29,    23,    59,   59,    0,   0,  -1439,  true   },

    {   L_, "\"0004-03-01T00:00:00.000000+00:00\"",
              4,     3,    1,     0,     0,    0,    0,   0,      0,  true   },
    {   L_, "\"0004-03-01T23:59:59.000000+23:59\"",
              4,     3,    1,    23,    59,   59,    0,   0,   1439,  true   },
    {   L_, "\"0004-03-01T23:59:59.000000-23:59\"",
              4,     3,    1,    23,    59,   59,    0,   0,  -1439,  true   },

    {   L_, "\"0008-02-28T00:00:00.000000+00:00\"",
              8,     2,   28,     0,     0,    0,    0,   0,      0,  true   },
    {   L_, "\"0008-02-28T23:59:59.000000+23:59\"",
              8,     2,   28,    23,    59,   59,    0,   0,   1439,  true   },

    {   L_, "\"0008-02-29T00:00:00.000000+00:00\"",
              8,     2,   29,     0,     0,    0,    0,   0,      0,  true   },
    {   L_, "\"0008-02-29T23:59:59.000000+23:59\"",
              8,     2,   29,    23,    59,   59,    0,   0,   1439,  true   },

    {   L_, "\"0008-03-01T00:00:00.000000+00:00\"",
              8,     3,    1,     0,     0,    0,    0,   0,      0,  true   },
    {   L_, "\"0008-03-01T23:59:59.000000+23:59\"",
              8,     3,    1,    23,    59,   59,    0,   0,   1439,  true   },

    {   L_, "\"0100-02-28T00:00:00.000000+00:00\"",
            100,     2,   28,     0,     0,    0,    0,   0,      0,  true   },
    {   L_, "\"0100-02-28T23:59:59.000000+23:59\"",
            100,     2,   28,    23,    59,   59,    0,   0,   1439,  true   },
    {   L_, "\"0100-02-28T23:59:59.000000-23:59\"",
            100,     2,   28,    23,    59,   59,    0,   0,  -1439,  true   },

    {   L_, "\"0100-03-01T00:00:00.000000+00:00\"",
            100,     3,    1,     0,     0,    0,    0,   0,      0,  true   },
    {   L_, "\"0100-03-01T23:59:59.000000+23:59\"",
            100,     3,    1,    23,    59,   59,    0,   0,   1439,  true   },
    {   L_, "\"0100-03-01T23:59:59.000000-23:59\"",
            100,     3,    1,    23,    59,   59,    0,   0,  -1439,  true   },

    {   L_, "\"0400-02-28T00:00:00.000000+00:00\"",
            400,     2,   28,     0,     0,    0,    0,   0,      0,  true   },
    {   L_, "\"0400-02-28T23:59:59.000000+23:59\"",
            400,     2,   28,    23,    59,   59,    0,   0,   1439,  true   },
    {   L_, "\"0400-02-28T23:59:59.000000-23:59\"",
            400,     2,   28,    23,    59,   59,    0,   0,  -1439,  true   },

    {   L_, "\"0400-02-29T00:00:00.000000+00:00\"",
            400,     2,   29,     0,     0,    0,    0,   0,      0,  true   },
    {   L_, "\"0400-02-29T23:59:59.000000+23:59\"",
            400,     2,   29,    23,    59,   59,    0,   0,   1439,  true   },
    {   L_, "\"0400-02-29T23:59:59.000000-23:59\"",
            400,     2,   29,    23,    59,   59,    0,   0,  -1439,  true   },

    {   L_, "\"0400-03-01T00:00:00.000000+00:00\"",
            400,     3,    1,     0,     0,    0,    0,   0,      0,  true   },
    {   L_, "\"0400-03-01T23:59:59.000000+23:59\"",
            400,     3,    1,    23,    59,   59,    0,   0,   1439,  true   },
    {   L_, "\"0400-03-01T23:59:59.000000-23:59\"",
            400,     3,    1,    23,    59,   59,    0,   0,  -1439,  true   },

    {   L_, "\"0500-02-28T00:00:00.000000+00:00\"",
            500,     2,   28,     0,     0,    0,    0,   0,      0,  true   },
    {   L_, "\"0500-02-28T23:59:59.000000+23:59\"",
            500,     2,   28,    23,    59,   59,    0,   0,   1439,  true   },

    {   L_, "\"0500-03-01T00:00:00.000000+00:00\"",
            500,     3,    1,     0,     0,    0,    0,   0,      0,  true   },
    {   L_, "\"0500-03-01T23:59:59.000000+23:59\"",
            500,     3,    1,    23,    59,   59,    0,   0,   1439,  true   },

    {   L_, "\"0800-02-28T00:00:00.000000+00:00\"",
            800,     2,   28,     0,     0,    0,    0,   0,      0,  true   },
    {   L_, "\"0800-02-28T23:59:59.000000+23:59\"",
            800,     2,   28,    23,    59,   59,    0,   0,   1439,  true   },

    {   L_, "\"0800-02-29T00:00:00.000000+00:00\"",
            800,     2,   29,     0,     0,    0,    0,   0,      0,  true   },
    {   L_, "\"0800-02-29T23:59:59.000000+23:59\"",
            800,     2,   29,    23,    59,   59,    0,   0,   1439,  true   },

    {   L_, "\"0800-03-01T00:00:00.000000+00:00\"",
            800,     3,    1,     0,     0,    0,    0,   0,      0,  true   },
    {   L_, "\"0800-03-01T23:59:59.000000+23:59\"",
            800,     3,    1,    23,    59,   59,    0,   0,   1439,  true   },

    {   L_, "\"1000-02-28T00:00:00.000000+00:00\"",
           1000,     2,   28,     0,     0,    0,    0,   0,      0,  true   },
    {   L_, "\"1000-02-28T23:59:59.000000+23:59\"",
           1000,     2,   28,    23,    59,   59,    0,   0,   1439,  true   },

    {   L_, "\"1000-03-01T00:00:00.000000+00:00\"",
           1000,     3,    1,     0,     0,    0,    0,   0,      0,  true   },
    {   L_, "\"1000-03-01T23:59:59.000000+23:59\"",
           1000,     3,    1,    23,    59,   59,    0,   0,   1439,  true   },

    {   L_, "\"2000-02-28T00:00:00.000000+00:00\"",
           2000,     2,   28,     0,     0,    0,    0,   0,      0,  true   },
    {   L_, "\"2000-02-28T23:59:59.000000+23:59\"",
           2000,     2,   28,    23,    59,   59,    0,   0,   1439,  true   },

    {   L_, "\"2000-02-29T00:00:00.000000+00:00\"",
           2000,     2,   29,     0,     0,    0,    0,   0,      0,  true   },
    {   L_, "\"2000-02-29T23:59:59.000000+23:59\"",
           2000,     2,   29,    23,    59,   59,    0,   0,   1439,  true   },

    {   L_, "\"2000-03-01T00:00:00.000000+00:00\"",
           2000,     3,    1,     0,     0,    0,    0,   0,      0,  true   },
    {   L_, "\"2000-03-01T23:59:59.000000+23:59\"",
           2000,     3,    1,    23,    59,   59,    0,   0,   1439,  true   },

    {   L_, "\"2016-12-31T00:00:00.000000+00:00\"",
           2016,    12,   31,     0,     0,    0,    0,   0,      0,  true   },
    {   L_, "\"2017-12-31T00:00:00.000000+00:00\"",
           2017,    12,   31,     0,     0,    0,    0,   0,      0,  true   },
    {   L_, "\"2018-12-31T00:00:00.000000+00:00\"",
           2018,    12,   31,     0,     0,    0,    0,   0,      0,  true   },
    {   L_, "\"2019-12-31T00:00:00.000000+00:00\"",
           2019,    12,   31,     0,     0,    0,    0,   0,      0,  true   },

    {   L_, "\"2020-01-01T00:00:00.000000+00:00\"",
           2020,     1,    1,     0,     0,    0,    0,   0,      0,  true   },
    {   L_, "\"2020-01-01T00:00:00.000000+23:59\"",
           2020,     1,    1,     0,     0,    0,    0,   0,   1439,  true   },
    {   L_, "\"2020-01-01T00:00:00.000000-23:59\"",
           2020,     1,    1,     0,     0,    0,    0,   0,  -1439,  true   },

    {   L_, "\"2020-01-01T23:59:59.999999+00:00\"",
           2020,     1,    1,    23,    59,   59,  999, 999,      0,  true   },
    {   L_, "\"2020-01-01T23:59:59.999999+23:59\"",
           2020,     1,    1,    23,    59,   59,  999, 999,   1439,  true   },
    {   L_, "\"2020-01-01T23:59:59.999999-23:59\"",
           2020,     1,    1,    23,    59,   59,  999, 999,  -1439,  true   },

    {   L_, "\"2020-01-02T00:00:00.000000+00:00\"",
           2020,     1,    2,     0,     0,    0,    0,   0,      0,  true   },
    {   L_, "\"2020-01-02T00:00:00.000000+23:59\"",
           2020,     1,    2,     0,     0,    0,    0,   0,   1439,  true   },
    {   L_, "\"2020-01-02T00:00:00.000000-23:59\"",
           2020,     1,    2,     0,     0,    0,    0,   0,  -1439,  true   },

    {   L_, "\"2020-02-28T00:00:00.000000+00:00\"",
           2020,     2,   28,     0,     0,    0,    0,   0,      0,  true   },
    {   L_, "\"2020-02-28T23:59:59.000000+23:59\"",
           2020,     2,   28,    23,    59,   59,    0,   0,   1439,  true   },
    {   L_, "\"2020-02-28T23:59:59.000000-23:59\"",
           2020,     2,   28,    23,    59,   59,    0,   0,  -1439,  true   },

    {   L_, "\"2020-02-29T00:00:00.000000+00:00\"",
           2020,     2,   29,     0,     0,    0,    0,   0,      0,  true   },
    {   L_, "\"2020-02-29T23:59:59.000000+23:59\"",
           2020,     2,   29,    23,    59,   59,    0,   0,   1439,  true   },
    {   L_, "\"2020-02-29T23:59:59.000000-23:59\"",
           2020,     2,   29,    23,    59,   59,    0,   0,  -1439,  true   },

    {   L_, "\"2020-03-01T00:00:00.000000+00:00\"",
           2020,     3,    1,     0,     0,    0,    0,   0,      0,  true   },
    {   L_, "\"2020-03-01T23:59:59.000000+23:59\"",
           2020,     3,    1,    23,    59,   59,    0,   0,   1439,  true   },
    {   L_, "\"2020-03-01T23:59:59.000000-23:59\"",
           2020,     3,    1,    23,    59,   59,    0,   0,  -1439,  true   },

    {   L_, "\"2021-01-02T00:00:00.000000+00:00\"",
           2021,     1,    2,     0,     0,    0,    0,   0,      0,  true   },
    {   L_, "\"2022-01-02T00:00:00.000000+00:00\"",
           2022,     1,    2,     0,     0,    0,    0,   0,      0,  true   },

    {   L_, "\"9999-02-28T00:00:00.000000+00:00\"",
           9999,     2,   28,     0,     0,    0,    0,   0,      0,  true   },
    {   L_, "\"9999-02-28T23:59:59.000000+23:59\"",
           9999,     2,   28,    23,    59,   59,    0,   0,   1439,  true   },
    {   L_, "\"9999-02-28T23:59:59.000000-23:59\"",
           9999,     2,   28,    23,    59,   59,    0,   0,  -1439,  true   },

    {   L_, "\"9999-03-01T00:00:00.000000+00:00\"",
           9999,     3,    1,     0,     0,    0,    0,   0,      0,  true   },
    {   L_, "\"9999-03-01T23:59:59.000000+23:59\"",
           9999,     3,    1,    23,    59,   59,    0,   0,   1439,  true   },
    {   L_, "\"9999-03-01T23:59:59.000000-23:59\"",
           9999,     3,    1,    23,    59,   59,    0,   0,  -1439,  true   },

    {   L_, "\"9999-12-30T00:00:00.000000+00:00\"",
           9999,    12,   30,     0,     0,    0,    0,   0,      0,  true   },
    {   L_, "\"9999-12-30T23:59:59.000000+23:59\"",
           9999,    12,   30,    23,    59,   59,    0,   0,   1439,  true   },

    {   L_, "\"9999-12-31T00:00:00.000000+00:00\"",
           9999,    12,   31,     0,     0,    0,    0,   0,      0,  true   },
    {   L_, "\"9999-12-31T23:59:59.000000+23:59\"",
           9999,    12,   31,    23,    59,   59,    0,   0,   1439,  true   },

    {   L_, "\"0000-01-01T00:00:00.000000+00:00\"",
              1,     1,    1,    24,     0,    0,    0,   0,      0,  false  },
    {   L_, "\"0001-00-01T00:00:00.000000+00:00\"",
              1,     1,    1,    24,     0,    0,    0,   0,      0,  false  },
    {   L_, "\"0001-01-00T00:00:00.000000+00:00\"",
              1,     1,    1,    24,     0,    0,    0,   0,      0,  false  },
    {   L_, "\"0000-00-00T00:00:00.000000+00:00\"",
              1,     1,    1,    24,     0,    0,    0,   0,      0,  false  },
    {   L_, "\"0001-13-00T00:00:00.000000+00:00\"",
              1,     1,    1,    24,     0,    0,    0,   0,      0,  false  },
    {   L_, "\"0001-01-32T00:00:00.000000+00:00\"",
              1,     1,    1,    24,     0,    0,    0,   0,      0,  false  },
    {   L_, "\"0001-01-01T25:00:00.000000+00:00\"",
              1,     1,    1,    24,     0,    0,    0,   0,      0,  false  },
    {   L_, "\"0001-01-01T00:61:00.000000+00:00\"",
              1,     1,    1,    24,     0,    0,    0,   0,      0,  false  },
    {   L_, "\"0001-01-01T00:00:61.000000+00:00\"",
              1,     1,    1,    24,     0,    0,    0,   0,      0,  false  },
    {   L_, "\"0001-01-01T00:00:00.000000+24:00\"",
              1,     1,    1,    24,     0,    0,    0,   0,      0,  false  },
    {   L_, "\"0001-01-01T00:00:00.000000+00:61\"",
              1,     1,    1,    24,     0,    0,    0,   0,      0,  false  },
    {   L_, "\"0001-01-01T00:00:00.000000-24:00\"",
              1,     1,    1,    24,     0,    0,    0,   0,      0,  false  },
    {   L_, "\"0001-01-01T00:00:00.000000-00:61\"",
              1,     1,    1,    24,     0,    0,    0,   0,      0,  false  },

    {   L_, "\"GARBAGE\"",
              1,     1,    1,    24,     0,    0,    0,   0,      0,  false  },
    {   L_, "\".9999\"",
              1,     1,    1,    24,     0,    0,    0,   0,      0,  false  },
    {   L_, "\"23:59:59.9999\"",
              1,     1,    1,    24,     0,    0,    0,   0,      0,  false  },
    {   L_, "\"T23:59:59.9999\"",
              1,     1,    1,    24,     0,    0,    0,   0,      0,  false  },
    {   L_, "\"01T23:59:59.9999\"",
              1,     1,    1,    24,     0,    0,    0,   0,      0,  false  },
    {   L_, "\"01-01T23:59:59.9999\"",
              1,     1,    1,    24,     0,    0,    0,   0,      0,  false  },
    {   L_, "\"0001-01-01T00:00.000\"",
              1,     1,    1,    24,     0,    0,    0,   0,      0,  false  },
    {   L_, "\"0001-01-01T00:00.000000\"",
              1,     1,    1,    24,     0,    0,    0,   0,      0,  false  },
    {   L_, "\"01-01-01T23:59:59.9999\"",
              1,     1,    1,    24,     0,    0,    0,   0,      0,  false  },
    {   L_, "\"01-01-01T23:59:59.9999999\"",
              1,     1,    1,    24,     0,    0,    0,   0,      0,  false  },
        };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int         LINE        = DATA[i].d_line;
                const string      INPUT       = DATA[i].d_input_p;
                const int         YEAR        = DATA[i].d_year;
                const int         MONTH       = DATA[i].d_month;
                const int         DAY         = DATA[i].d_day;
                const int         HOUR        = DATA[i].d_hour;
                const int         MINUTE      = DATA[i].d_minutes;
                const int         SECOND      = DATA[i].d_seconds;
                const int         MILLISECOND = DATA[i].d_milliSecs;
                const int         MICROSECOND = DATA[i].d_microSecs;
                const int         OFFSET      = DATA[i].d_tzoffset;
                const bool        IS_VALID    = DATA[i].d_isValid;

                bdlt::Datetime dt(YEAR, MONTH, DAY,
                                  HOUR, MINUTE, SECOND,
                                  MILLISECOND, MICROSECOND);
                const bdlt::Datetime& DT = dt;
                bdlt::DatetimeTz exp(DT, OFFSET);
                const bdlt::DatetimeTz& EXP = exp;

                bdlt::DatetimeTz value;

                const bsl::string_view isb(INPUT);

                const int rc = Util::getValue(&value, isb);
                if (IS_VALID) {
                    LOOP2_ASSERT(LINE, rc, 0 == rc);
                }
                else {
                    LOOP2_ASSERT(LINE, rc, rc);
                }
                LOOP3_ASSERT(LINE, EXP, value,   EXP == value);
            }
        }
      } break;
      case 18: {
        // --------------------------------------------------------------------
        // TESTING 'getValue' for bdlt::Datetime values
        //
        // Concerns:
        //: 1 Values in the valid range, including the maximum and minimum
        //:   values for this type, are parsed correctly.
        //:
        //: 2 The passed in variable is unmodified if the data is not valid.
        //:
        //: 3 The return code is 0 on success and non-zero on failure.
        //
        // Plan:
        //: 1 Using the table-driven technique, specify a set of distinct
        //:   rows of string value, expected parsed value, and return code.
        //:
        //: 2 For each row in the table of P-1:
        //:
        //:   1 Provide the string value and a variable  to be parsed into to
        //:     the 'getValue' function.  The variable is assigned a sentinel
        //:     value before invoking the function.
        //:
        //:   2 If the parsing should succeed then verify that the variable
        //:     value matches the expected value.  Otherwise confirm that the
        //:     variable value is unmodified.
        //:
        //:   3 Confirm that the return code is 0 on success and non-zero
        //:     otherwise.
        //
        // Testing:
        //   static int getValue(bdlt::Datetime       *v, bsl::string_view s);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING 'getValue' for 'bdlt::Datetime'"
                          << "\n======================================"
                          << endl;
        {
            static const struct {
                int         d_line;      // source line number
                const char *d_input_p;   // input
                int         d_year;      // year under test
                int         d_month;     // month under test
                int         d_day;       // day under test
                int         d_hour;      // hour under test
                int         d_minutes;   // minutes under test
                int         d_seconds;   // seconds under test
                int         d_milliSecs; // milli seconds under test
                int         d_microSecs; // micro seconds under test
                bool        d_isValid;   // isValid flag
            } DATA[] = {
  //line    input year month   day   hour    min   sec    ms   us   isValid
  //----    ----- ---- -----   ---   ----    ---   ---    --   --   -------
    {   L_, "\"0001-01-01T00:00:00.000\"",
                   1,     1,    1,     0,     0,    0,    0,   0,   true   },
    {   L_, "\"0001-01-01T00:00:00.000000\"",
                   1,     1,    1,     0,     0,    0,    0,   0,   true   },

    {   L_, "\"0001-01-01T01:01:01.001\"",
                   1,     1,    1,     1,     1,    1,    1,   0,   true   },
    {   L_, "\"0001-01-01T01:01:01.001000\"",
                   1,     1,    1,     1,     1,    1,    1,   0,   true   },
    {   L_, "\"0001-01-01T01:23:59.059\"",
                   1,     1,    1,     1,    23,   59,   59,   0,   true   },
    {   L_, "\"0001-01-01T01:23:59.059059\"",
                   1,     1,    1,     1,    23,   59,   59,  59,   true   },
    {   L_, "\"0001-01-02T00:00:00.000\"",
                   1,     1,    2,     0,     0,    0,    0,   0,   true   },
    {   L_, "\"0001-01-02T00:00:00.000000\"",
                   1,     1,    2,     0,     0,    0,    0,   0,   true   },
    {   L_, "\"0001-01-02T01:01:01.001\"",
                   1,     1,    2,     1,     1,    1,    1,   0,   true   },
    {   L_, "\"0001-01-02T01:01:01.001000\"",
                   1,     1,    2,     1,     1,    1,    1,   0,   true   },

    {   L_, "\"0001-01-02T01:23:59.059\"",
                   1,     1,    2,     1,    23,   59,   59,   0,   true   },
    {   L_, "\"0001-01-02T01:23:59.059501\"",
                   1,     1,    2,     1,    23,   59,   59, 501,   true   },

    {   L_, "\"0001-02-01T23:59:59.000\"",
                   1,     2,    1,    23,    59,   59,    0,   0,   true   },
    {   L_, "\"0001-02-01T23:59:59.000001\"",
                   1,     2,    1,    23,    59,   59,    0,   1,   true   },

    {   L_, "\"0001-12-31T00:00:00.000\"",
                   1,    12,   31,     0,     0,    0,    0,   0,   true   },
    {   L_, "\"0001-12-31T23:59:59.000\"",
                   1,    12,   31,    23,    59,   59,    0,   0,   true   },

    {   L_, "\"0002-01-01T00:00:00.000000\"",
                   2,     1,    1,     0,     0,    0,    0,   0,   true   },
    {   L_, "\"0002-01-01T23:59:59.000000\"",
                   2,     1,    1,    23,    59,   59,    0,   0,   true   },

    {   L_, "\"0004-02-28T00:00:00.000000\"",
                   4,     2,   28,     0,     0,    0,    0,   0,   true   },
    {   L_, "\"0004-02-28T23:59:59.000000\"",
                   4,     2,   28,    23,    59,   59,    0,   0,   true   },
    {   L_, "\"0004-02-28T23:59:59.000000\"",
                   4,     2,   28,    23,    59,   59,    0,   0,   true   },

    {   L_, "\"0004-02-29T00:00:00.000000\"",
                   4,     2,   29,     0,     0,    0,    0,   0,   true   },
    {   L_, "\"0004-02-29T23:59:59.000000\"",
                   4,     2,   29,    23,    59,   59,    0,   0,   true   },

    {   L_, "\"0004-03-01T00:00:00.000000\"",
                   4,     3,    1,     0,     0,    0,    0,   0,   true   },
    {   L_, "\"0004-03-01T23:59:59.000000\"",
                   4,     3,    1,    23,    59,   59,    0,   0,   true   },
    {   L_, "\"0004-03-01T23:59:59.000000\"",
                   4,     3,    1,    23,    59,   59,    0,   0,   true   },

    {   L_, "\"0008-02-28T00:00:00.000000\"",
                   8,     2,   28,     0,     0,    0,    0,   0,   true   },
    {   L_, "\"0008-02-28T23:59:59.000000\"",
                   8,     2,   28,    23,    59,   59,    0,   0,   true   },

    {   L_, "\"0008-02-29T00:00:00.000000\"",
                   8,     2,   29,     0,     0,    0,    0,   0,   true   },
    {   L_, "\"0008-02-29T23:59:59.000000\"",
                   8,     2,   29,    23,    59,   59,    0,   0,   true   },

    {   L_, "\"0008-03-01T00:00:00.000000\"",
                   8,     3,    1,     0,     0,    0,    0,   0,   true   },
    {   L_, "\"0008-03-01T23:59:59.000000\"",
                   8,     3,    1,    23,    59,   59,    0,   0,   true   },

    {   L_, "\"0100-02-28T00:00:00.000000\"",
                 100,     2,   28,     0,     0,    0,    0,   0,   true   },
    {   L_, "\"0100-02-28T23:59:59.000000\"",
                 100,     2,   28,    23,    59,   59,    0,   0,   true   },
    {   L_, "\"0100-02-28T23:59:59.000000\"",
                 100,     2,   28,    23,    59,   59,    0,   0,   true   },

    {   L_, "\"0100-03-01T00:00:00.000000\"",
                 100,     3,    1,     0,     0,    0,    0,   0,   true   },
    {   L_, "\"0100-03-01T23:59:59.000000\"",
                 100,     3,    1,    23,    59,   59,    0,   0,   true   },
    {   L_, "\"0100-03-01T23:59:59.000000\"",
                 100,     3,    1,    23,    59,   59,    0,   0,   true   },

    {   L_, "\"0400-02-28T00:00:00.000000\"",
                 400,     2,   28,     0,     0,    0,    0,   0,   true   },
    {   L_, "\"0400-02-28T23:59:59.000000\"",
                 400,     2,   28,    23,    59,   59,    0,   0,   true   },
    {   L_, "\"0400-02-28T23:59:59.000000\"",
                 400,     2,   28,    23,    59,   59,    0,   0,   true   },

    {   L_, "\"0400-02-29T00:00:00.000000\"",
                 400,     2,   29,     0,     0,    0,    0,   0,   true   },
    {   L_, "\"0400-02-29T23:59:59.000000\"",
                 400,     2,   29,    23,    59,   59,    0,   0,   true   },
    {   L_, "\"0400-02-29T23:59:59.000000\"",
                 400,     2,   29,    23,    59,   59,    0,   0,   true   },

    {   L_, "\"0400-03-01T00:00:00.000000\"",
                 400,     3,    1,     0,     0,    0,    0,   0,   true   },
    {   L_, "\"0400-03-01T23:59:59.000000\"",
                 400,     3,    1,    23,    59,   59,    0,   0,   true   },
    {   L_, "\"0400-03-01T23:59:59.000000\"",
                 400,     3,    1,    23,    59,   59,    0,   0,   true   },

    {   L_, "\"0500-02-28T00:00:00.000000\"",
                 500,     2,   28,     0,     0,    0,    0,   0,   true   },
    {   L_, "\"0500-02-28T23:59:59.000000\"",
                 500,     2,   28,    23,    59,   59,    0,   0,   true   },

    {   L_, "\"0500-03-01T00:00:00.000000\"",
                 500,     3,    1,     0,     0,    0,    0,   0,   true   },
    {   L_, "\"0500-03-01T23:59:59.000000\"",
                 500,     3,    1,    23,    59,   59,    0,   0,   true   },

    {   L_, "\"0800-02-28T00:00:00.000000\"",
                 800,     2,   28,     0,     0,    0,    0,   0,   true   },
    {   L_, "\"0800-02-28T23:59:59.000000\"",
                 800,     2,   28,    23,    59,   59,    0,   0,   true   },

    {   L_, "\"0800-02-29T00:00:00.000000\"",
                 800,     2,   29,     0,     0,    0,    0,   0,   true   },
    {   L_, "\"0800-02-29T23:59:59.000000\"",
                 800,     2,   29,    23,    59,   59,    0,   0,   true   },

    {   L_, "\"0800-03-01T00:00:00.000000\"",
                 800,     3,    1,     0,     0,    0,    0,   0,   true   },
    {   L_, "\"0800-03-01T23:59:59.000000\"",
                 800,     3,    1,    23,    59,   59,    0,   0,   true   },

    {   L_, "\"1000-02-28T00:00:00.000000\"",
                1000,     2,   28,     0,     0,    0,    0,   0,   true   },
    {   L_, "\"1000-02-28T23:59:59.000000\"",
                1000,     2,   28,    23,    59,   59,    0,   0,   true   },

    {   L_, "\"1000-03-01T00:00:00.000000\"",
                1000,     3,    1,     0,     0,    0,    0,   0,   true   },
    {   L_, "\"1000-03-01T23:59:59.000000\"",
                1000,     3,    1,    23,    59,   59,    0,   0,   true   },

    {   L_, "\"2000-02-28T00:00:00.000000\"",
                2000,     2,   28,     0,     0,    0,    0,   0,   true   },
    {   L_, "\"2000-02-28T23:59:59.000000\"",
                2000,     2,   28,    23,    59,   59,    0,   0,   true   },

    {   L_, "\"2000-02-29T00:00:00.000000\"",
                2000,     2,   29,     0,     0,    0,    0,   0,   true   },
    {   L_, "\"2000-02-29T23:59:59.000000\"",
                2000,     2,   29,    23,    59,   59,    0,   0,   true   },

    {   L_, "\"2000-03-01T00:00:00.000000\"",
                2000,     3,    1,     0,     0,    0,    0,   0,   true   },
    {   L_, "\"2000-03-01T23:59:59.000000\"",
                2000,     3,    1,    23,    59,   59,    0,   0,   true   },

    {   L_, "\"2016-12-31T00:00:00.000000\"",
                2016,    12,   31,     0,     0,    0,    0,   0,   true   },
    {   L_, "\"2017-12-31T00:00:00.000000\"",
                2017,    12,   31,     0,     0,    0,    0,   0,   true   },
    {   L_, "\"2018-12-31T00:00:00.000000\"",
                2018,    12,   31,     0,     0,    0,    0,   0,   true   },
    {   L_, "\"2019-12-31T00:00:00.000000\"",
                2019,    12,   31,     0,     0,    0,    0,   0,   true   },

    {   L_, "\"2020-01-01T00:00:00.000000\"",
                2020,     1,    1,     0,     0,    0,    0,   0,   true   },
    {   L_, "\"2020-01-01T00:00:00.000000\"",
                2020,     1,    1,     0,     0,    0,    0,   0,   true   },
    {   L_, "\"2020-01-01T00:00:00.000000\"",
                2020,     1,    1,     0,     0,    0,    0,   0,   true   },

    {   L_, "\"2020-01-01T23:59:59.999999\"",
                2020,     1,    1,    23,    59,   59,  999, 999,   true   },
    {   L_, "\"2020-01-01T23:59:59.999999\"",
                2020,     1,    1,    23,    59,   59,  999, 999,   true   },
    {   L_, "\"2020-01-01T23:59:59.999999\"",
                2020,     1,    1,    23,    59,   59,  999, 999,   true   },

    {   L_, "\"2020-01-02T00:00:00.000000\"",
                2020,     1,    2,     0,     0,    0,    0,   0,   true   },
    {   L_, "\"2020-01-02T00:00:00.000000\"",
                2020,     1,    2,     0,     0,    0,    0,   0,   true   },
    {   L_, "\"2020-01-02T00:00:00.000000\"",
                2020,     1,    2,     0,     0,    0,    0,   0,   true   },

    {   L_, "\"2020-02-28T00:00:00.000000\"",
                2020,     2,   28,     0,     0,    0,    0,   0,   true   },
    {   L_, "\"2020-02-28T23:59:59.000000\"",
                2020,     2,   28,    23,    59,   59,    0,   0,   true   },
    {   L_, "\"2020-02-28T23:59:59.000000\"",
                2020,     2,   28,    23,    59,   59,    0,   0,   true   },

    {   L_, "\"2020-02-29T00:00:00.000000\"",
                2020,     2,   29,     0,     0,    0,    0,   0,   true   },
    {   L_, "\"2020-02-29T23:59:59.000000\"",
                2020,     2,   29,    23,    59,   59,    0,   0,   true   },
    {   L_, "\"2020-02-29T23:59:59.000000\"",
                2020,     2,   29,    23,    59,   59,    0,   0,   true   },

    {   L_, "\"2020-03-01T00:00:00.000000\"",
                2020,     3,    1,     0,     0,    0,    0,   0,   true   },
    {   L_, "\"2020-03-01T23:59:59.000000\"",
                2020,     3,    1,    23,    59,   59,    0,   0,   true   },
    {   L_, "\"2020-03-01T23:59:59.000000\"",
                2020,     3,    1,    23,    59,   59,    0,   0,   true   },

    {   L_, "\"2021-01-02T00:00:00.000000\"",
                2021,     1,    2,     0,     0,    0,    0,   0,   true   },
    {   L_, "\"2022-01-02T00:00:00.000000\"",
                2022,     1,    2,     0,     0,    0,    0,   0,   true   },

    {   L_, "\"9999-02-28T00:00:00.000000\"",
                9999,     2,   28,     0,     0,    0,    0,   0,   true   },
    {   L_, "\"9999-02-28T23:59:59.000000\"",
                9999,     2,   28,    23,    59,   59,    0,   0,   true   },
    {   L_, "\"9999-02-28T23:59:59.000000\"",
                9999,     2,   28,    23,    59,   59,    0,   0,   true   },

    {   L_, "\"9999-03-01T00:00:00.000000\"",
                9999,     3,    1,     0,     0,    0,    0,   0,   true   },
    {   L_, "\"9999-03-01T23:59:59.000000\"",
                9999,     3,    1,    23,    59,   59,    0,   0,   true   },
    {   L_, "\"9999-03-01T23:59:59.000000\"",
                9999,     3,    1,    23,    59,   59,    0,   0,   true   },

    {   L_, "\"9999-12-30T00:00:00.000000\"",
                9999,    12,   30,     0,     0,    0,    0,   0,   true   },
    {   L_, "\"9999-12-30T23:59:59.000000\"",
                9999,    12,   30,    23,    59,   59,    0,   0,   true   },

    {   L_, "\"9999-12-31T00:00:00.000000\"",
                9999,    12,   31,     0,     0,    0,    0,   0,   true   },
    {   L_, "\"9999-12-31T23:59:59.000000\"",
                9999,    12,   31,    23,    59,   59,    0,   0,   true   },

    {   L_, "\"GARBAGE\"",
                   1,     1,    1,    24,     0,    0,    0,   0,   false  },
    {   L_, "\".9999\"",
                   1,     1,    1,    24,     0,    0,    0,   0,   false  },
    {   L_, "\"23:59:59.9999\"",
                   1,     1,    1,    24,     0,    0,    0,   0,   false  },
    {   L_, "\"T23:59:59.9999\"",
                   1,     1,    1,    24,     0,    0,    0,   0,   false  },
    {   L_, "\"01T23:59:59.9999\"",
                   1,     1,    1,    24,     0,    0,    0,   0,   false  },
    {   L_, "\"01-01T23:59:59.9999\"",
                   1,     1,    1,    24,     0,    0,    0,   0,   false  },
    {   L_, "\"0001-01-01T00:00.000\"",
                   1,     1,    1,    24,     0,    0,    0,   0,   false  },

    {   L_, "\"0000-01-01T00:00:00.000+00:00\"",
                   1,     1,    1,    24,     0,    0,    0,   0,   false  },
    {   L_, "\"0000-01-01T00:00:00.000Z\"",
                   1,     1,    1,    24,     0,    0,    0,   0,   false  },
    {   L_, "\"0000-01-01T00:00:00.000z\"",
                   1,     1,    1,    24,     0,    0,    0,   0,   false  },
    {   L_, "\"0000-01-01T00:00:00.000-00:00\"",
                   1,     1,    1,    24,     0,    0,    0,   0,   false  },

    {   L_, "\"0001-00-01T00:00:00.000\"",
                   1,     1,    1,    24,     0,    0,    0,   0,   false  },
    {   L_, "\"0000-01-01T00:00:00.000000\"",
                   1,     1,    1,    24,     0,    0,    0,   0,   false  },
    {   L_, "\"0000-00-00T00:00:00.000\"",
                   1,     1,    1,    24,     0,    0,    0,   0,   false  },
    {   L_, "\"0001-13-00T00:00:00.000\"",
                   1,     1,    1,    24,     0,    0,    0,   0,   false  },
    {   L_, "\"0001-01-32T00:00:00.000\"",
                   1,     1,    1,    24,     0,    0,    0,   0,   false  },
    {   L_, "\"0001-01-01T25:00:00.000\"",
                   1,     1,    1,    24,     0,    0,    0,   0,   false  },
    {   L_, "\"0001-01-01T00:61:00.000\"",
                   1,     1,    1,    24,     0,    0,    0,   0,   false  },
    {   L_, "\"0001-01-01T00:00:61.000\"",
                   1,     1,    1,    24,     0,    0,    0,   0,   false  },

    // DRQS 130122587: datetime parsing should accept offsets

    {   L_, "\"0001-01-01T00:00:00.000Z\"",
                   1,     1,    1,     0,     0,    0,    0,   0,   true   },
    {   L_, "\"0001-01-01T00:00:00.000+00:00\"",
                   1,     1,    1,     0,     0,    0,    0,   0,   true   },
    {   L_, "\"0001-01-01T00:00:00.000+00:45\"",
                   1,     1,    1,    24,     0,    0,    0,   0,   false  },
    {   L_, "\"0001-01-01T00:00:00.000-23:59\"",
                   1,     1,    1,    23,   59,     0,    0,   0,   true   },
    {   L_, "\"0001-01-01T00:00:00.000000Z\"",
                   1,     1,    1,     0,     0,    0,    0,   0,   true   },
    {   L_, "\"0001-01-01T00:00:00.000000+00:00\"",
                   1,     1,    1,     0,     0,    0,    0,   0,   true   },
    {   L_, "\"0001-01-01T00:00:00.000000+00:45\"",
                   1,     1,    1,    24,     0,    0,    0,   0,   false  },
    {   L_, "\"0001-01-01T00:00:00.000000-23:59\"",
                   1,     1,    1,    23,    59,    0,    0,   0,   true   },

    {   L_, "\"0001-01-01T01:01:01.001z\"",
                   1,     1,    1,     1,     1,    1,    1,   0,   true   },
    {   L_, "\"0001-01-01T01:01:01.001+00:00\"",
                   1,     1,    1,     1,     1,    1,    1,   0,   true   },
    {   L_, "\"0001-01-01T01:01:01.001+08:20\"",
                   1,     1,    1,    24,     0,    0,    0,   0,   false  },
    {   L_, "\"0001-01-01T01:01:01.001000z\"",
                   1,     1,    1,     1,     1,    1,    1,   0,   true   },
    {   L_, "\"0001-01-01T01:01:01.001000+00:00\"",
                   1,     1,    1,     1,     1,    1,    1,   0,   true   },
    {   L_, "\"0001-01-01T01:01:01.001000+08:20\"",
                   1,     1,    1,    24,     0,    0,    0,   0,   false  },
    {   L_, "\"0001-01-01T00:00:00.000000-23:59\"",
                   1,     1,    1,    23,    59,    0,    0,   0,   true   },
    {   L_, "\"0001-01-01T01:23:59.059059+00:00\"",
                   1,     1,    1,     1,    23,   59,   59,  59,   true   },
    {   L_, "\"0001-01-01T01:23:59.059059+23:59\"",
                   1,     1,    1,    24,     0,    0,    0,   0,   false  },
    {   L_, "\"0001-01-01T01:23:59.059059-23:59\"",
                   1,     1,    2,     1,    22,   59,   59,  59,   true   },
    {   L_, "\"0001-01-02T00:00:00.000000+00:00\"",
                   1,     1,    2,     0,     0,    0,    0,   0,   true   },
    {   L_, "\"0001-01-02T00:00:00.000000+23:59\"",
                   1,     1,    1,     0,     1,    0,    0,   0,   true   },
    {   L_, "\"0001-01-02T00:00:00.000000-23:59\"",
                   1,     1,    2,    23,    59,    0,    0,   0,   true   },
    {   L_, "\"0001-01-02T01:01:01.000001+00:00\"",
                   1,     1,    2,     1,     1,    1,    0,   1,   true   },
    {   L_, "\"0001-01-02T01:01:01.000001+08:20\"",
                   1,     1,    1,    16,    41,    1,    0,   1,   true   },

    {   L_, "\"0001-01-02T01:23:59.059168Z\"",
                   1,     1,    2,     1,    23,   59,   59, 168,   true   },
    {   L_, "\"0001-01-02T01:23:59.059168+00:00\"",
                   1,     1,    2,     1,    23,   59,   59, 168,   true   },
    {   L_, "\"0001-01-02T01:23:59.059168+08:20\"",
                   1,     1,    1,    17,     3,   59,   59, 168,   true   },
    {   L_, "\"0001-01-02T01:23:59.059168-08:20\"",
                   1,     1,    2,     9,    43,   59,   59, 168,   true   },
    {   L_, "\"0001-01-10T00:00:00.000000z\"",
                   1,     1,   10,     0,     0,    0,    0,   0,   true   },
    {   L_, "\"0001-01-10T00:00:00.000000+00:00\"",
                   1,     1,   10,     0,     0,    0,    0,   0,   true   },
    {   L_, "\"0001-01-10T01:01:01.001+01:39\"",
                   1,     1,    9,    23,    22,    1,    1,   0,   true   },
    {   L_, "\"0001-01-10T01:01:01.000001+01:39\"",
                   1,     1,    9,    23,    22,    1,    0,   1,   true   },

    {   L_, "\"0001-01-30T00:00:00.000000Z\"",
                   1,     1,   30,     0,     0,    0,    0,   0,   true   },
    {   L_, "\"0001-01-30T00:00:00.000000+00:00\"",
                   1,     1,   30,     0,     0,    0,    0,   0,   true   },
    {   L_, "\"0001-01-31T00:00:00.000000+23:59\"",
                   1,     1,   30,     0,     1,    0,    0,   0,   true   },
    {   L_, "\"0001-01-31T00:00:00.000000-23:59\"",
                   1,     1,   31,    23,    59,    0,    0,   0,   true   },

    {   L_, "\"0001-02-01T00:00:00.000000z\"",
                   1,     2,    1,     0,     0,    0,    0,   0,   true   },
    {   L_, "\"0001-02-01T00:00:00.000000+00:00\"",
                   1,     2,    1,     0,     0,    0,    0,   0,   true   },
    {   L_, "\"0001-02-01T23:59:59.000000+23:59\"",
                   1,     2,    1,     0,     0,   59,    0,   0,   true   },

    {   L_, "\"0001-12-31T00:00:00.000000Z\"",
                   1,    12,   31,     0,     0,    0,    0,   0,   true   },
    {   L_, "\"0001-12-31T00:00:00.000000+00:00\"",
                   1,    12,   31,     0,     0,    0,    0,   0,   true   },
    {   L_, "\"0001-12-31T23:59:59.000000+23:59\"",
                   1,    12,   31,     0,     0,   59,    0,   0,   true   },

    {   L_, "\"0002-01-01T00:00:00.000000+00:00\"",
                   2,     1,    1,     0,     0,    0,    0,   0,   true   },
    {   L_, "\"0002-01-01T23:59:59.000000+23:59\"",
                   2,     1,    1,     0,     0,   59,    0,   0,   true   },

    {   L_, "\"0004-01-01T00:00:00.000000+00:00\"",
                   4,     1,    1,     0,     0,    0,    0,   0,   true   },
    {   L_, "\"0004-01-01T23:59:59.000000+23:59\"",
                   4,     1,    1,     0,     0,   59,    0,   0,   true   },

    {   L_, "\"0004-02-28T00:00:00.000000+00:00\"",
                   4,     2,   28,     0,     0,    0,    0,   0,   true   },
    {   L_, "\"0004-02-28T23:59:59.000000+23:59\"",
                   4,     2,   28,     0,     0,   59,    0,   0,   true   },
    {   L_, "\"0004-02-28T23:59:59.000000-23:59\"",
                   4,     2,   29,    23,    58,   59,    0,   0,   true   },

    {   L_, "\"0004-02-29T00:00:00.000000+00:00\"",
                   4,     2,   29,     0,     0,    0,    0,   0,   true   },
    {   L_, "\"0004-02-29T23:59:59.000000+23:59\"",
                   4,     2,   29,     0,     0,   59,    0,   0,   true   },
    {   L_, "\"0004-02-29T23:59:59.000000-23:59\"",
                   4,     3,    1,    23,    58,   59,    0,   0,   true   },

    {   L_, "\"0004-03-01T00:00:00.000000+00:00\"",
                   4,     3,    1,     0,     0,    0,    0,   0,   true   },
    {   L_, "\"0004-03-01T23:59:59.000000+23:59\"",
                   4,     3,    1,     0,     0,   59,    0,   0,   true   },
    {   L_, "\"0004-03-01T23:59:59.000000-23:59\"",
                   4,     3,    2,    23,    58,   59,    0,   0,   true   },

    {   L_, "\"0008-02-28T00:00:00.000000+00:00\"",
                   8,     2,   28,     0,     0,    0,    0,   0,   true   },
    {   L_, "\"0008-02-28T23:59:59.000000+23:59\"",
                   8,     2,   28,     0,     0,   59,    0,   0,   true   },

    {   L_, "\"0008-02-29T00:00:00.000000+00:00\"",
                   8,     2,   29,     0,     0,    0,    0,   0,   true   },
    {   L_, "\"0008-02-29T23:59:59.000000+23:59\"",
                   8,     2,   29,     0,     0,   59,    0,   0,   true   },

    {   L_, "\"0008-03-01T00:00:00.000000+00:00\"",
                   8,     3,    1,     0,     0,    0,    0,   0,   true   },
    {   L_, "\"0008-03-01T23:59:59.000000+23:59\"",
                   8,     3,    1,     0,     0,   59,    0,   0,   true   },

    {   L_, "\"0100-02-28T00:00:00.000000+00:00\"",
                 100,     2,   28,     0,     0,    0,    0,   0,   true   },
    {   L_, "\"0100-02-28T23:59:59.000000+23:59\"",
                 100,     2,   28,     0,     0,   59,    0,   0,   true   },
    {   L_, "\"0100-02-28T23:59:59.000000-23:59\"",
                 100,     2,   29,    23,    58,   59,    0,   0,   true   },

    {   L_, "\"0100-03-01T00:00:00.000000+00:00\"",
                 100,     3,    1,     0,     0,    0,    0,   0,   true   },
    {   L_, "\"0100-03-01T23:59:59.000000+23:59\"",
                 100,     3,    1,     0,     0,   59,    0,   0,   true   },
    {   L_, "\"0100-03-01T23:59:59.000000-23:59\"",
                 100,     3,    2,    23,    58,   59,    0,   0,  true   },

    {   L_, "\"0400-02-28T00:00:00.000000+00:00\"",
                 400,     2,   28,     0,     0,    0,    0,   0,   true   },
    {   L_, "\"0400-02-28T23:59:59.000000+23:59\"",
                 400,     2,   28,     0,     0,   59,    0,   0,   true   },
    {   L_, "\"0400-02-28T23:59:59.000000-23:59\"",
                 400,     2,   29,    23,    58,   59,    0,   0,   true   },

    {   L_, "\"0400-02-29T00:00:00.000000+00:00\"",
                 400,     2,   29,     0,     0,    0,    0,   0,   true   },
    {   L_, "\"0400-02-29T23:59:59.000000+23:59\"",
                 400,     2,   29,     0,     0,   59,    0,   0,   true   },
    {   L_, "\"0400-02-29T23:59:59.000000-23:59\"",
                 400,     3,    1,    23,    58,   59,    0,   0,   true   },

    {   L_, "\"0400-03-01T00:00:00.000000+00:00\"",
                 400,     3,    1,     0,     0,    0,    0,   0,   true   },
    {   L_, "\"0400-03-01T23:59:59.000000+23:59\"",
                 400,     3,    1,     0,     0,   59,    0,   0,   true   },
    {   L_, "\"0400-03-01T23:59:59.000000-23:59\"",
                 400,     3,    2,    23,    58,   59,    0,   0,   true   },

    {   L_, "\"9999-12-31T00:00:00.000000+00:00\"",
                9999,    12,   31,     0,     0,    0,    0,   0,   true   },
    {   L_, "\"9999-12-31T23:59:59.000000+23:59\"",
                9999,    12,   31,     0,     0,   59,    0,   0,   true   },
    {   L_, "\"9999-12-31T23:59:59.000000-23:59\"",
                   1,     1,    1,    24,     0,    0,    0,   0,   false  },

    {   L_, "\"0000-01-01T00:00:00.000000+00:00\"",
                   1,     1,    1,    24,     0,    0,    0,   0,   false  },
    {   L_, "\"0001-00-01T00:00:00.000000+00:00\"",
                   1,     1,    1,    24,     0,    0,    0,   0,   false  },
    {   L_, "\"0001-01-00T00:00:00.000000+00:00\"",
                   1,     1,    1,    24,     0,    0,    0,   0,   false  },
    {   L_, "\"0000-00-00T00:00:00.000000+00:00\"",
                   1,     1,    1,    24,     0,    0,    0,   0,   false  },
    {   L_, "\"0001-13-00T00:00:00.000000+00:00\"",
                   1,     1,    1,    24,     0,    0,    0,   0,   false  },
    {   L_, "\"0001-01-32T00:00:00.000000+00:00\"",
                   1,     1,    1,    24,     0,    0,    0,   0,   false  },
    {   L_, "\"0001-01-01T25:00:00.000000+00:00\"",
                   1,     1,    1,    24,     0,    0,    0,   0,   false  },
    {   L_, "\"0001-01-01T00:61:00.000000+00:00\"",
                   1,     1,    1,    24,     0,    0,    0,   0,   false  },
    {   L_, "\"0001-01-01T00:00:61.000000+00:00\"",
                   1,     1,    1,    24,     0,    0,    0,   0,   false  },
    {   L_, "\"0001-01-01T00:00:00.000000+24:00\"",
                   1,     1,    1,    24,     0,    0,    0,   0,   false  },
    {   L_, "\"0001-01-01T00:00:00.000000+00:61\"",
                   1,     1,    1,    24,     0,    0,    0,   0,   false  },
    {   L_, "\"0001-01-01T00:00:00.000000-24:00\"",
                   1,     1,    1,    24,     0,    0,    0,   0,   false  },
    {   L_, "\"0001-01-01T00:00:00.000000-00:61\"",
                   1,     1,    1,    24,     0,    0,    0,   0,   false  },

    {   L_, "\"GARBAGE\"",
                   1,     1,    1,    24,     0,    0,    0,   0,   false  },
    {   L_, "\".9999\"",
                   1,     1,    1,    24,     0,    0,    0,   0,   false  },
    {   L_, "\"23:59:59.9999\"",
                   1,     1,    1,    24,     0,    0,    0,   0,   false  },
    {   L_, "\"T23:59:59.9999\"",
                   1,     1,    1,    24,     0,    0,    0,   0,   false  },
    {   L_, "\"01T23:59:59.9999\"",
                   1,     1,    1,    24,     0,    0,    0,   0,   false  },
    {   L_, "\"01-01T23:59:59.9999\"",
                   1,     1,    1,    24,     0,    0,    0,   0,   false  },
    {   L_, "\"0001-01-01T00:00.000\"",
                   1,     1,    1,    24,     0,    0,    0,   0,   false  },
    {   L_, "\"0001-01-01T00:00.000000\"",
                   1,     1,    1,    24,     0,    0,    0,   0,   false  },
    {   L_, "\"01-01-01T23:59:59.9999\"",
                   1,     1,    1,    24,     0,    0,    0,   0,   false  },
    {   L_, "\"01-01-01T23:59:59.9999999\"",
                   1,     1,    1,    24,     0,    0,    0,   0,   false  },

    {   L_, "\"0000-01-01T00:00:00.000000+00:00\"",
                   1,     1,    1,    24,     0,    0,    0,   0,   false  },
    {   L_, "\"0000-01-01T00:00:00.000000Z\"",
                   1,     1,    1,    24,     0,    0,    0,   0,   false  },
    {   L_, "\"0000-01-01T00:00:00.000000z\"",
                   1,     1,    1,    24,     0,    0,    0,   0,   false  },
    {   L_, "\"0000-01-01T00:00:00.000000-00:00\"",
                   1,     1,    1,    24,     0,    0,    0,   0,   false  },

        };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int         LINE        = DATA[i].d_line;
                const string      INPUT       = DATA[i].d_input_p;
                const int         YEAR        = DATA[i].d_year;
                const int         MONTH       = DATA[i].d_month;
                const int         DAY         = DATA[i].d_day;
                const int         HOUR        = DATA[i].d_hour;
                const int         MINUTE      = DATA[i].d_minutes;
                const int         SECOND      = DATA[i].d_seconds;
                const int         MILLISECOND = DATA[i].d_milliSecs;
                const int         MICROSECOND = DATA[i].d_microSecs;
                const bool        IS_VALID    = DATA[i].d_isValid;

                bdlt::Datetime exp(YEAR, MONTH, DAY,
                                   HOUR, MINUTE, SECOND,
                                   MILLISECOND, MICROSECOND);
                const bdlt::Datetime& EXP = exp;

                bdlt::Datetime value;

                const bsl::string_view isb(INPUT);

                const int rc = Util::getValue(&value, isb);
                if (IS_VALID) {
                    LOOP2_ASSERT(LINE, rc, 0 == rc);
                }
                else {
                    LOOP2_ASSERT(LINE, rc, rc);
                }
                LOOP3_ASSERT(LINE, EXP, value,   EXP == value);
            }
        }
      } break;
      case 17: {
        // --------------------------------------------------------------------
        // TESTING 'getValue' for bdlt::DateTz values
        //
        // Concerns:
        //: 1 Values in the valid range, including the maximum and minimum
        //:   values for this type, are parsed correctly.
        //:
        //: 2 The passed in variable is unmodified if the data is not valid.
        //:
        //: 3 The return code is 0 on success and non-zero on failure.
        //
        // Plan:
        //: 1 Using the table-driven technique, specify a set of distinct
        //:   rows of string value, expected parsed value, and return code.
        //:
        //: 2 For each row in the table of P-1:
        //:
        //:   1 Provide the string value and a variable  to be parsed into to
        //:     the 'getValue' function.  The variable is assigned a sentinel
        //:     value before invoking the function.
        //:
        //:   2 If the parsing should succeed then verify that the variable
        //:     value matches the expected value.  Otherwise confirm that the
        //:     variable value is unmodified.
        //:
        //:   3 Confirm that the return code is 0 on success and non-zero
        //:     otherwise.
        //
        // Testing:
        //   static int getValue(bdlt::DateTz         *v, bsl::string_view s);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING 'getValue' for 'bdlt::DateTz'"
                          << "\n====================================" << endl;
        {
            static const struct {
                int         d_line;      // source line number
                const char *d_input_p;   // input
                int         d_year;      // year under test
                int         d_month;     // month under test
                int         d_day;       // day under test
                int         d_tzoffset;  // time zone offset
                bool        d_isValid;   // isValid flag
            } DATA[] = {
    //line  input                      year month   day   offset isValid
    //----  -----                      ---- -----   ---   ------ -------
    {   L_, "\"0001-01-01+00:00\"",     1,     1,    1,       0,  true   },
    {   L_, "\"0001-01-01+00:45\"",     1,     1,    1,      45,  true   },
    {   L_, "\"0001-01-01-23:59\"",     1,     1,    1,   -1439,  true   },

    {   L_, "\"0001-01-01+00:00\"",     1,     1,    1,       0,  true   },
    {   L_, "\"0001-01-01+08:20\"",     1,     1,    1,     500,  true   },
    {   L_, "\"0001-01-01-23:59\"",     1,     1,    1,   -1439,  true   },
    {   L_, "\"0001-01-01+00:00\"",     1,     1,    1,       0,  true   },
    {   L_, "\"0001-01-01+23:59\"",     1,     1,    1,    1439,  true   },
    {   L_, "\"0001-01-01-23:59\"",     1,     1,    1,   -1439,  true   },

    {   L_, "\"0001-01-02+00:00\"",     1,     1,    2,       0,  true   },
    {   L_, "\"0001-01-02+23:59\"",     1,     1,    2,    1439,  true   },
    {   L_, "\"0001-01-02-23:59\"",     1,     1,    2,   -1439,  true   },
    {   L_, "\"0001-01-02+00:00\"",     1,     1,    2,       0,  true   },
    {   L_, "\"0001-01-02+08:20\"",     1,     1,    2,     500,  true   },

    {   L_, "\"0001-01-02+00:00\"",     1,     1,    2,       0,  true   },
    {   L_, "\"0001-01-02+08:20\"",     1,     1,    2,     500,  true   },
    {   L_, "\"0001-01-02-08:20\"",     1,     1,    2,    -500,  true   },

    {   L_, "\"0001-01-10+00:00\"",     1,     1,   10,       0,  true   },
    {   L_, "\"0001-01-10+01:39\"",     1,     1,   10,      99,  true   },

    {   L_, "\"0001-01-30+00:00\"",     1,     1,   30,       0,  true   },
    {   L_, "\"0001-01-31+23:59\"",     1,     1,   31,    1439,  true   },
    {   L_, "\"0001-01-31-23:59\"",     1,     1,   31,   -1439,  true   },

    {   L_, "\"0001-02-01+00:00\"",     1,     2,    1,       0,  true   },
    {   L_, "\"0001-02-01+23:59\"",     1,     2,    1,    1439,  true   },

    {   L_, "\"0001-12-31+00:00\"",     1,    12,   31,       0,  true   },
    {   L_, "\"0001-12-31+23:59\"",     1,    12,   31,    1439,  true   },

    {   L_, "\"0002-01-01+00:00\"",     2,     1,    1,       0,  true   },
    {   L_, "\"0002-01-01+23:59\"",     2,     1,    1,    1439,  true   },

    {   L_, "\"0004-01-01+00:00\"",     4,     1,    1,       0,  true   },
    {   L_, "\"0004-01-01+23:59\"",     4,     1,    1,    1439,  true   },

    {   L_, "\"0004-02-28+00:00\"",     4,     2,   28,       0,  true   },
    {   L_, "\"0004-02-28+23:59\"",     4,     2,   28,    1439,  true   },
    {   L_, "\"0004-02-28-23:59\"",     4,     2,   28,   -1439,  true   },

    {   L_, "\"0004-02-29+00:00\"",     4,     2,   29,       0,  true   },
    {   L_, "\"0004-02-29+23:59\"",     4,     2,   29,    1439,  true   },
    {   L_, "\"0004-02-29-23:59\"",     4,     2,   29,   -1439,  true   },

    {   L_, "\"0004-03-01+00:00\"",     4,     3,    1,       0,  true   },
    {   L_, "\"0004-03-01+23:59\"",     4,     3,    1,    1439,  true   },
    {   L_, "\"0004-03-01-23:59\"",     4,     3,    1,   -1439,  true   },

    {   L_, "\"0008-02-28+00:00\"",     8,     2,   28,       0,  true   },
    {   L_, "\"0008-02-28+23:59\"",     8,     2,   28,    1439,  true   },

    {   L_, "\"0008-02-29+00:00\"",     8,     2,   29,       0,  true   },
    {   L_, "\"0008-02-29+23:59\"",     8,     2,   29,    1439,  true   },

    {   L_, "\"0008-03-01+00:00\"",     8,     3,    1,       0,  true   },
    {   L_, "\"0008-03-01+23:59\"",     8,     3,    1,    1439,  true   },

    {   L_, "\"0100-02-28+00:00\"",   100,     2,   28,       0,  true   },
    {   L_, "\"0100-02-28+23:59\"",   100,     2,   28,    1439,  true   },
    {   L_, "\"0100-02-28-23:59\"",   100,     2,   28,   -1439,  true   },

    {   L_, "\"0100-03-01+00:00\"",   100,     3,    1,       0,  true   },
    {   L_, "\"0100-03-01+23:59\"",   100,     3,    1,    1439,  true   },
    {   L_, "\"0100-03-01-23:59\"",   100,     3,    1,   -1439,  true   },

    {   L_, "\"0400-02-28+00:00\"",   400,     2,   28,       0,  true   },
    {   L_, "\"0400-02-28+23:59\"",   400,     2,   28,    1439,  true   },
    {   L_, "\"0400-02-28-23:59\"",   400,     2,   28,   -1439,  true   },

    {   L_, "\"0400-02-29+00:00\"",   400,     2,   29,       0,  true   },
    {   L_, "\"0400-02-29+23:59\"",   400,     2,   29,    1439,  true   },
    {   L_, "\"0400-02-29-23:59\"",   400,     2,   29,   -1439,  true   },

    {   L_, "\"0400-03-01+00:00\"",   400,     3,    1,       0,  true   },
    {   L_, "\"0400-03-01+23:59\"",   400,     3,    1,    1439,  true   },
    {   L_, "\"0400-03-01-23:59\"",   400,     3,    1,   -1439,  true   },

    {   L_, "\"0500-02-28+00:00\"",   500,     2,   28,       0,  true   },
    {   L_, "\"0500-02-28+23:59\"",   500,     2,   28,    1439,  true   },

    {   L_, "\"0500-03-01+00:00\"",   500,     3,    1,       0,  true   },
    {   L_, "\"0500-03-01+23:59\"",   500,     3,    1,    1439,  true   },

    {   L_, "\"0800-02-28+00:00\"",   800,     2,   28,       0,  true   },
    {   L_, "\"0800-02-28+23:59\"",   800,     2,   28,    1439,  true   },

    {   L_, "\"0800-02-29+00:00\"",   800,     2,   29,       0,  true   },
    {   L_, "\"0800-02-29+23:59\"",   800,     2,   29,    1439,  true   },

    {   L_, "\"0800-03-01+00:00\"",   800,     3,    1,       0,  true   },
    {   L_, "\"0800-03-01+23:59\"",   800,     3,    1,    1439,  true   },

    {   L_, "\"1000-02-28+00:00\"",  1000,     2,   28,       0,  true   },
    {   L_, "\"1000-02-28+23:59\"",  1000,     2,   28,    1439,  true   },

    {   L_, "\"1000-03-01+00:00\"",  1000,     3,    1,       0,  true   },
    {   L_, "\"1000-03-01+23:59\"",  1000,     3,    1,    1439,  true   },

    {   L_, "\"2000-02-28+00:00\"",  2000,     2,   28,       0,  true   },
    {   L_, "\"2000-02-28+23:59\"",  2000,     2,   28,    1439,  true   },

    {   L_, "\"2000-02-29+00:00\"",  2000,     2,   29,       0,  true   },
    {   L_, "\"2000-02-29+23:59\"",  2000,     2,   29,    1439,  true   },

    {   L_, "\"2000-03-01+00:00\"",  2000,     3,    1,       0,  true   },
    {   L_, "\"2000-03-01+23:59\"",  2000,     3,    1,    1439,  true   },

    {   L_, "\"2016-12-31+00:00\"",  2016,    12,   31,       0,  true   },
    {   L_, "\"2017-12-31+00:00\"",  2017,    12,   31,       0,  true   },
    {   L_, "\"2018-12-31+00:00\"",  2018,    12,   31,       0,  true   },
    {   L_, "\"2019-12-31+00:00\"",  2019,    12,   31,       0,  true   },

    {   L_, "\"2020-01-01+00:00\"",  2020,     1,    1,       0,  true   },
    {   L_, "\"2020-01-01+23:59\"",  2020,     1,    1,    1439,  true   },
    {   L_, "\"2020-01-01-23:59\"",  2020,     1,    1,   -1439,  true   },

    {   L_, "\"2020-01-01+00:00\"",  2020,     1,    1,       0,  true   },
    {   L_, "\"2020-01-01+23:59\"",  2020,     1,    1,    1439,  true   },
    {   L_, "\"2020-01-01-23:59\"",  2020,     1,    1,   -1439,  true   },

    {   L_, "\"2020-01-02+00:00\"",  2020,     1,    2,       0,  true   },
    {   L_, "\"2020-01-02+23:59\"",  2020,     1,    2,    1439,  true   },
    {   L_, "\"2020-01-02-23:59\"",  2020,     1,    2,   -1439,  true   },

    {   L_, "\"2020-02-28+00:00\"",  2020,     2,   28,       0,  true   },
    {   L_, "\"2020-02-28+23:59\"",  2020,     2,   28,    1439,  true   },
    {   L_, "\"2020-02-28-23:59\"",  2020,     2,   28,   -1439,  true   },

    {   L_, "\"2020-02-29+00:00\"",  2020,     2,   29,       0,  true   },
    {   L_, "\"2020-02-29+23:59\"",  2020,     2,   29,    1439,  true   },
    {   L_, "\"2020-02-29-23:59\"",  2020,     2,   29,   -1439,  true   },

    {   L_, "\"2020-03-01+00:00\"",  2020,     3,    1,       0,  true   },
    {   L_, "\"2020-03-01+23:59\"",  2020,     3,    1,    1439,  true   },
    {   L_, "\"2020-03-01-23:59\"",  2020,     3,    1,   -1439,  true   },

    {   L_, "\"2021-01-02+00:00\"",  2021,     1,    2,       0,  true   },
    {   L_, "\"2022-01-02+00:00\"",  2022,     1,    2,       0,  true   },

    {   L_, "\"9999-02-28+00:00\"",  9999,     2,   28,       0,  true   },
    {   L_, "\"9999-02-28+23:59\"",  9999,     2,   28,    1439,  true   },
    {   L_, "\"9999-02-28-23:59\"",  9999,     2,   28,   -1439,  true   },

    {   L_, "\"9999-03-01+00:00\"",  9999,     3,    1,       0,  true   },
    {   L_, "\"9999-03-01+23:59\"",  9999,     3,    1,    1439,  true   },
    {   L_, "\"9999-03-01-23:59\"",  9999,     3,    1,   -1439,  true   },

    {   L_, "\"9999-12-30+00:00\"",  9999,    12,   30,       0,  true   },
    {   L_, "\"9999-12-30+23:59\"",  9999,    12,   30,    1439,  true   },

    {   L_, "\"9999-12-31+00:00\"",  9999,    12,   31,       0,  true   },
    {   L_, "\"9999-12-31+23:59\"",  9999,    12,   31,    1439,  true   },

    {   L_, "\"GARBAGE\"",              1,     1,    1,       0,  false  },
    {   L_, "\"0000-01-01Z\"",          1,     1,    1,       0,  false  },
    {   L_, "\"0000-01-01z\"",          1,     1,    1,       0,  false  },
    {   L_, "\"0000-01-01+00:00\"",     1,     1,    1,       0,  false  },
    {   L_, "\"0000-01-01-00:00\"",     1,     1,    1,       0,  false  },
    {   L_, "\"9999-00-01+00:00\"",     1,     1,    1,       0,  false  },
    {   L_, "\"9999-00-01-00:00\"",     1,     1,    1,       0,  false  },

    {   L_, "\"0000-01-01+00:00\"",     1,     1,    1,       0,  false  },
    {   L_, "\"0001-00-01+00:00\"",     1,     1,    1,       0,  false  },
    {   L_, "\"0001-01-00+00:00\"",     1,     1,    1,       0,  false  },
    {   L_, "\"0000-00-00+00:00\"",     1,     1,    1,       0,  false  },
    {   L_, "\"0001-13-00+00:00\"",     1,     1,    1,       0,  false  },
    {   L_, "\"0001-01-32+00:00\"",     1,     1,    1,       0,  false  },
    {   L_, "\"0001-01-01+24:00\"",     1,     1,    1,       0,  false  },
    {   L_, "\"0001-01-01+00:61\"",     1,     1,    1,       0,  false  },
    {   L_, "\"0001-01-01-24:00\"",     1,     1,    1,       0,  false  },
    {   L_, "\"0001-01-01-00:61\"",     1,     1,    1,       0,  false  },

    {   L_, "\"0001-01-01T00:00:00.000+00:00\"",
                                        1,     1,    1,       0,  false  },
        };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int         LINE        = DATA[i].d_line;
                const string      INPUT       = DATA[i].d_input_p;
                const int         YEAR        = DATA[i].d_year;
                const int         MONTH       = DATA[i].d_month;
                const int         DAY         = DATA[i].d_day;
                const int         OFFSET      = DATA[i].d_tzoffset;
                const bool        IS_VALID    = DATA[i].d_isValid;

                bdlt::Date d(YEAR, MONTH, DAY);  const bdlt::Date& D = d;
                bdlt::DateTz exp(D, OFFSET);  const bdlt::DateTz& EXP = exp;

                bdlt::DateTz value;

                const bsl::string_view isb(INPUT);

                const int rc = Util::getValue(&value, isb);
                if (IS_VALID) {
                    LOOP2_ASSERT(LINE, rc, 0 == rc);
                }
                else {
                    LOOP2_ASSERT(LINE, rc, rc);
                }
                LOOP3_ASSERT(LINE, EXP, value,   EXP == value);
            }
        }
      } break;
      case 16: {
        // --------------------------------------------------------------------
        // TESTING 'getValue' for bdlt::Date values
        //
        // Concerns:
        //: 1 Values in the valid range, including the maximum and minimum
        //:   values for this type, are parsed correctly.
        //:
        //: 2 The passed in variable is unmodified if the data is not valid.
        //:
        //: 3 The return code is 0 on success and non-zero on failure.
        //
        // Plan:
        //: 1 Using the table-driven technique, specify a set of distinct
        //:   rows of string value, expected parsed value, and return code.
        //:
        //: 2 For each row in the table of P-1:
        //:
        //:   1 Provide the string value and a variable  to be parsed into to
        //:     the 'getValue' function.  The variable is assigned a sentinel
        //:     value before invoking the function.
        //:
        //:   2 If the parsing should succeed then verify that the variable
        //:     value matches the expected value.  Otherwise confirm that the
        //:     variable value is unmodified.
        //:
        //:   3 Confirm that the return code is 0 on success and non-zero
        //:     otherwise.
        //
        // Testing:
        //   static int getValue(bdlt::Date           *v, bsl::string_view s);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING 'getValue' for 'bdlt::Date' types"
                          << "\n========================================"
                          << endl;
        {
            static const struct {
                int         d_line;      // source line number
                const char *d_input_p;   // input
                int         d_year;      // year under test
                int         d_month;     // month under test
                int         d_day;       // day under test
                bool        d_isValid;   // isValid flag
            } DATA[] = {
  //line  input                       year  month  day  isValid
  //----  -----                       ----  -----  ---  -------
  {  L_, "\"0001-01-01\"",             1,     1,    1,   true   },
  {  L_, "\"0001-01-02\"",             1,     1,    2,   true   },
  {  L_, "\"0001-01-10\"",             1,     1,   10,   true   },
  {  L_, "\"0001-01-30\"",             1,     1,   30,   true   },
  {  L_, "\"0001-01-31\"",             1,     1,   31,   true   },

  {  L_, "\"0001-02-01\"",             1,     2,    1,   true   },
  {  L_, "\"0001-12-31\"",             1,    12,   31,   true   },
  {  L_, "\"0002-01-01\"",             2,     1,    1,   true   },
  {  L_, "\"0004-01-01\"",             4,     1,    1,   true   },
  {  L_, "\"0004-02-28\"",             4,     2,   28,   true   },
  {  L_, "\"0004-02-29\"",             4,     2,   29,   true   },
  {  L_, "\"0004-03-01\"",             4,     3,    1,   true   },

  {  L_, "\"0008-02-28\"",             8,     2,   28,   true   },
  {  L_, "\"0008-02-29\"",             8,     2,   29,   true   },
  {  L_, "\"0008-03-01\"",             8,     3,    1,   true   },
  {  L_, "\"0100-02-28\"",           100,     2,   28,   true   },
  {  L_, "\"0100-03-01\"",           100,     3,    1,   true   },
  {  L_, "\"0400-02-28\"",           400,     2,   28,   true   },
  {  L_, "\"0400-02-29\"",           400,     2,   29,   true   },
  {  L_, "\"0400-03-01\"",           400,     3,    1,   true   },
  {  L_, "\"0500-02-28\"",           500,     2,   28,   true   },
  {  L_, "\"0500-03-01\"",           500,     3,    1,   true   },

  {  L_, "\"0800-02-28\"",           800,     2,   28,   true   },
  {  L_, "\"0800-02-29\"",           800,     2,   29,   true   },
  {  L_, "\"0800-03-01\"",           800,     3,    1,   true   },
  {  L_, "\"1000-02-28\"",          1000,     2,   28,   true   },
  {  L_, "\"1000-03-01\"",          1000,     3,    1,   true   },
  {  L_, "\"2000-02-28\"",          2000,     2,   28,   true   },
  {  L_, "\"2000-02-29\"",          2000,     2,   29,   true   },
  {  L_, "\"2000-03-01\"",          2000,     3,    1,   true   },
  {  L_, "\"2016-12-31\"",          2016,    12,   31,   true   },
  {  L_, "\"2017-12-31\"",          2017,    12,   31,   true   },
  {  L_, "\"2018-12-31\"",          2018,    12,   31,   true   },
  {  L_, "\"2019-12-31\"",          2019,    12,   31,   true   },

  {  L_, "\"2020-01-01\"",          2020,     1,    1,   true   },
  {  L_, "\"2020-01-02\"",          2020,     1,    2,   true   },
  {  L_, "\"2020-02-28\"",          2020,     2,   28,   true   },
  {  L_, "\"2020-02-29\"",          2020,     2,   29,   true   },
  {  L_, "\"2020-03-01\"",          2020,     3,    1,   true   },
  {  L_, "\"2022-01-02\"",          2022,     1,    2,   true   },

  {  L_, "\"9999-02-28\"",          9999,     2,   28,   true   },
  {  L_, "\"9999-03-01\"",          9999,     3,    1,   true   },
  {  L_, "\"9999-12-30\"",          9999,    12,   30,   true   },
  {  L_, "\"9999-12-31\"",          9999,    12,   31,   true   },

  // DRQS 130122587: date parsing should ignore offsets

  {  L_, "\"0001-01-01+00:00\"",      1,     1,    1,    true   },
  {  L_, "\"0001-01-01+00:45\"",      1,     1,    1,    true   },
  {  L_, "\"0001-01-01-23:59\"",      1,     1,    1,    true   },

  {  L_, "\"0001-01-02+00:00\"",      1,     1,    2,    true   },
  {  L_, "\"0001-01-02+08:20\"",      1,     1,    2,    true   },
  {  L_, "\"0001-01-02-08:20\"",      1,     1,    2,    true   },

  {  L_, "\"0001-02-01+00:00\"",      1,     2,    1,    true   },
  {  L_, "\"0001-02-01+23:59\"",      1,     2,    1,    true   },

  {  L_, "\"0004-02-29+00:00\"",      4,     2,   29,    true   },
  {  L_, "\"0004-02-29+23:59\"",      4,     2,   29,    true   },
  {  L_, "\"0004-02-29-23:59\"",      4,     2,   29,    true   },

  {  L_, "\"9999-02-28+00:00\"",   9999,     2,   28,    true   },
  {  L_, "\"9999-02-28+23:59\"",   9999,     2,   28,    true   },
  {  L_, "\"9999-02-28-23:59\"",   9999,     2,   28,    true   },

  {  L_, "\"9999-12-31+00:00\"",   9999,    12,   31,    true   },
  {  L_, "\"9999-12-31+23:59\"",   9999,    12,   31,    true   },

  {  L_, "\"0001-01-01Z\"",            1,     1,    1,   true   },
  {  L_, "\"0001-01-01z\"",            1,     1,    1,   true   },
  {  L_, "\"0001-01-01+00:00\"",       1,     1,    1,   true   },
  {  L_, "\"0001-01-01-00:00\"",       1,     1,    1,   true   },
  {  L_, "\"9999-01-01+00:00\"",    9999,     1,    1,   true   },
  {  L_, "\"9999-01-01-00:00\"",    9999,     1,    1,   true   },

  {  L_, "\"GARBAGE\"",                1,     1,    1,   false  },

  {  L_, "\"0000-01-01\"",             1,     1,    1,   false  },
  {  L_, "\"0001-00-01\"",             1,     1,    1,   false  },
  {  L_, "\"0001-01-00\"",             1,     1,    1,   false  },
  {  L_, "\"0000-00-00\"",             1,     1,    1,   false  },
  {  L_, "\"0001-13-00\"",             1,     1,    1,   false  },
  {  L_, "\"0001-01-32\"",             1,     1,    1,   false  },

  {  L_, "\"0001-01-01T00:00:00.000+00:00\"",
                                       1,     1,    1,   false  },
        };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int         LINE        = DATA[i].d_line;
                const string      INPUT       = DATA[i].d_input_p;
                const int         YEAR        = DATA[i].d_year;
                const int         MONTH       = DATA[i].d_month;
                const int         DAY         = DATA[i].d_day;
                const bool        IS_VALID    = DATA[i].d_isValid;

                bdlt::Date exp(YEAR, MONTH, DAY);  const bdlt::Date& EXP = exp;

                bdlt::Date value;

                const bsl::string_view isb(INPUT);

                const int rc = Util::getValue(&value, isb);
                if (IS_VALID) {
                    LOOP2_ASSERT(LINE, rc, 0 == rc);
                }
                else {
                    LOOP2_ASSERT(LINE, rc, rc);
                }
                LOOP3_ASSERT(LINE, EXP, value,   EXP == value);
            }
        }
      } break;
      case 15: {
        // --------------------------------------------------------------------
        // TESTING 'getValue' for bdlt::TimeTz values
        //
        // Concerns:
        //: 1 Values in the valid range, including the maximum and minimum
        //:   values for this type, are parsed correctly.
        //:
        //: 2 The passed in variable is unmodified if the data is not valid.
        //:
        //: 3 The return code is 0 on success and non-zero on failure.
        //
        // Plan:
        //: 1 Using the table-driven technique, specify a set of distinct
        //:   rows of string value, expected parsed value, and return code.
        //:
        //: 2 For each row in the table of P-1:
        //:
        //:   1 Provide the string value and a variable  to be parsed into to
        //:     the 'getValue' function.  The variable is assigned a sentinel
        //:     value before invoking the function.
        //:
        //:   2 If the parsing should succeed then verify that the variable
        //:     value matches the expected value.  Otherwise confirm that the
        //:     variable value is unmodified.
        //:
        //:   3 Confirm that the return code is 0 on success and non-zero
        //:     otherwise.
        //
        // Testing:
        //   static int getValue(bdlt::TimeTz         *v, bsl::string_view s);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING 'getValue' for 'bdlt::TimeTz'"
                          << "\n====================================" << endl;
        {
            static const struct {
                int         d_line;      // source line number
                const char *d_input_p;   // input
                int         d_hour;      // hour under test
                int         d_minutes;   // minutes under test
                int         d_seconds;   // seconds under test
                int         d_milliSecs; // milli seconds under test
                int         d_tzoffset;  // time zone offset
                bool        d_isValid;   // isValid flag
            } DATA[] = {
  //line    input                    hour    min   sec    ms  offset isValid
  //----    -----                    ----    ---   ---    --  ------ -------
  {  L_, "\"00:00:00.000+00:00\"",     0,     0,    0,    0,      0,  true   },
  {  L_, "\"00:00:00.000Z\"",          0,     0,    0,    0,      0,  true   },
  {  L_, "\"00:00:00.000z\"",          0,     0,    0,    0,      0,  true   },
  {  L_, "\"00:00:00.000+00:45\"",     0,     0,    0,    0,     45,  true   },
  {  L_, "\"00:00:00.000+23:59\"",     0,     0,    0,    0,   1439,  true   },
  {  L_, "\"00:00:00.000-23:59\"",     0,     0,    0,    0,  -1439,  true   },

  {  L_, "\"00:00:00.999+00:00\"",     0,     0,    0,  999,      0,  true   },
  {  L_, "\"00:00:00.999+23:59\"",     0,     0,    0,  999,   1439,  true   },
  {  L_, "\"00:00:00.999-23:59\"",     0,     0,    0,  999,  -1439,  true   },

  {  L_, "\"00:00:01.000+00:00\"",     0,     0,    1,    0,      0,  true   },
  {  L_, "\"00:00:01.000Z\"",          0,     0,    1,    0,      0,  true   },
  {  L_, "\"00:00:01.000z\"",          0,     0,    1,    0,      0,  true   },
  {  L_, "\"00:00:01.000+23:59\"",     0,     0,    1,    0,   1439,  true   },
  {  L_, "\"00:00:01.000-23:59\"",     0,     0,    1,    0,  -1439,  true   },

  {  L_, "\"00:00:59.000+00:00\"",     0,     0,   59,    0,      0,  true   },
  {  L_, "\"00:00:59.000Z\"",          0,     0,   59,    0,      0,  true   },
  {  L_, "\"00:00:59.000z\"",          0,     0,   59,    0,      0,  true   },
  {  L_, "\"00:00:59.000+23:59\"",     0,     0,   59,    0,   1439,  true   },
  {  L_, "\"00:00:59.000-23:59\"",     0,     0,   59,    0,  -1439,  true   },

  {  L_, "\"01:01:01.001+00:00\"",     1,     1,    1,    1,      0,  true   },
  {  L_, "\"01:01:01.001+08:20\"",     1,     1,    1,    1,    500,  true   },

  {  L_, "\"01:23:59.059+00:00\"",     1,    23,   59,   59,      0,  true   },
  {  L_, "\"01:23:59.059Z\"",          1,    23,   59,   59,      0,  true   },
  {  L_, "\"01:23:59.059z\"",          1,    23,   59,   59,      0,  true   },
  {  L_, "\"01:23:59.059+23:59\"",     1,    23,   59,   59,   1439,  true   },
  {  L_, "\"01:23:59.059-23:59\"",     1,    23,   59,   59,  -1439,  true   },

  {  L_, "\"23:59:59.000+23:59\"",    23,    59,   59,    0,   1439,  true   },
  {  L_, "\"23:59:59.000-23:59\"",    23,    59,   59,    0,  -1439,  true   },

  {  L_, "\"23:59:59.999+00:00\"",    23,    59,   59,  999,      0,  true   },
  {  L_, "\"23:59:59.999Z\"",         23,    59,   59,  999,      0,  true   },
  {  L_, "\"23:59:59.999z\"",         23,    59,   59,  999,      0,  true   },
  {  L_, "\"23:59:59.999+23:59\"",    23,    59,   59,  999,   1439,  true   },
  {  L_, "\"23:59:59.999-23:59\"",    23,    59,   59,  999,  -1439,  true   },

  {  L_, "\"24:00:00.000+00:00\"",    24,     0,    0,    0,      0,  true   },
  {  L_, "\"24:00:00.000Z\"",         24,     0,    0,    0,      0,  true   },
  {  L_, "\"24:00:00.000z\"",         24,     0,    0,    0,      0,  true   },

  {  L_, "\"GARBAGE\"",               24,     0,    0,    0,      0,  false  },
  {  L_, "\".9999\"",                 24,     0,    0,    0,      0,  false  },
         // ^
  {  L_, "\"23.9999\"",               24,     0,    0,    0,      0,  false  },
           // ^
  {  L_, "\"23:59.9999\"",            24,     0,    0,    0,      0,  false  },
              // ^
  {  L_, "\"23:59.59.9999\"",         24,     0,    0,    0,      0,  false  },
                     // ^

  {  L_, "\"25:00:00.000+00:00\"",    24,     0,    0,    0,      0,  false  },
  {  L_, "\"00:61:00.000+00:00\"",    24,     0,    0,    0,      0,  false  },
  {  L_, "\"00:00:61.000+00:00\"",    24,     0,    0,    0,      0,  false  },
  {  L_, "\"00:00:00.000+24:00\"",    24,     0,    0,    0,      0,  false  },
  {  L_, "\"00:00:00.000+00:61\"",    24,     0,    0,    0,      0,  false  },
  {  L_, "\"00:00:00.000-24:00\"",    24,     0,    0,    0,      0,  false  },
  {  L_, "\"00:00:00.000-00:61\"",    24,     0,    0,    0,      0,  false  },

  {  L_, "\"0001-01-01T00:00:00.000+00:00\"",
                                      24,     0,    0,    0,      0,  false  },
        };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int         LINE        = DATA[i].d_line;
                const string      INPUT       = DATA[i].d_input_p;
                const int         HOUR        = DATA[i].d_hour;
                const int         MINUTE      = DATA[i].d_minutes;
                const int         SECOND      = DATA[i].d_seconds;
                const int         MILLISECOND = DATA[i].d_milliSecs;
                const int         OFFSET      = DATA[i].d_tzoffset;
                const bool        IS_VALID    = DATA[i].d_isValid;

                bdlt::Time t(HOUR, MINUTE, SECOND, MILLISECOND);
                const bdlt::Time& T = t;
                bdlt::TimeTz exp(T, OFFSET); const bdlt::TimeTz& EXP = exp;

                bdlt::TimeTz value;

                const bsl::string_view isb(INPUT);

                const int rc = Util::getValue(&value, isb);
                if (IS_VALID) {
                    LOOP2_ASSERT(LINE, rc, 0 == rc);
                }
                else {
                    LOOP2_ASSERT(LINE, rc, rc);
                }
                LOOP3_ASSERT(LINE, EXP, value,   EXP == value);
            }
        }
      } break;
      case 14: {
        // --------------------------------------------------------------------
        // TESTING 'getValue' for bdlt::Time values
        //
        // Concerns:
        //: 1 Values in the valid range, including the maximum and minimum
        //:   values for this type, are parsed correctly.
        //:
        //: 2 The passed in variable is unmodified if the data is not valid.
        //:
        //: 3 The return code is 0 on success and non-zero on failure.
        //
        // Plan:
        //: 1 Using the table-driven technique, specify a set of distinct
        //:   rows of string value, expected parsed value, and return code.
        //:
        //: 2 For each row in the table of P-1:
        //:
        //:   1 Provide the string value and a variable  to be parsed into to
        //:     the 'getValue' function.  The variable is assigned a sentinel
        //:     value before invoking the function.
        //:
        //:   2 If the parsing should succeed then verify that the variable
        //:     value matches the expected value.  Otherwise confirm that the
        //:     variable value is unmodified.
        //:
        //:   3 Confirm that the return code is 0 on success and non-zero
        //:     otherwise.
        //
        // Testing:
        //   static int getValue(bdlt::Time           *v, bsl::string_view s);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING 'getValue' for 'bdlt::Time'"
                          << "\n==================================" << endl;
        {
            static const struct {
                int         d_line;      // source line number
                const char *d_input_p;   // input
                int         d_hour;      // hour under test
                int         d_minutes;   // minutes under test
                int         d_seconds;   // seconds under test
                int         d_milliSecs; // milli seconds under test
                int         d_uSecs;     // micro seconds under test
                bool        d_isValid;   // isValid flag
            } DATA[] = {
  //line    input                    hr      m     s    ms   us   isValid
  //----    -----                    ---     --    --   ---  ---  -------
  {  L_, "\"00:00:00\"",               0,     0,    0,    0,   0,  true   },
  {  L_, "\"00:00:00.000\"",           0,     0,    0,    0,   0,  true   },
  {  L_, "\"00:00:00.000000\"",        0,     0,    0,    0,   0,  true   },
  {  L_, "\"00:00:00.000001\"",        0,     0,    0,    0,   1,  true   },
  {  L_, "\"00:00:00.000999\"",        0,     0,    0,    0, 999,  true   },
  {  L_, "\"00:00:00.001000\"",        0,     0,    0,    1,   0,  true   },
  {  L_, "\"00:00:00.999\"",           0,     0,    0,  999,   0,  true   },
  {  L_, "\"00:00:00.100001\"",        0,     0,    0,  100,   1,  true   },
  {  L_, "\"00:00:00.999999\"",        0,     0,    0,  999, 999,  true   },

  {  L_, "\"00:00:01\"",               0,     0,    1,    0,   0,  true   },
  {  L_, "\"00:00:01.000\"",           0,     0,    1,    0,   0,  true   },
  {  L_, "\"00:00:59.000\"",           0,     0,   59,    0,   0,  true   },
  {  L_, "\"01:01:01.001\"",           1,     1,    1,    1,   0,  true   },
  {  L_, "\"01:01:01.000001\"",        1,     1,    1,    0,   1,  true   },
  {  L_, "\"01:01:01.001001\"",        1,     1,    1,    1,   1,  true   },

  {  L_, "\"01:23:59\"",               1,    23,   59,    0,   0,  true   },
  {  L_, "\"01:23:59.059\"",           1,    23,   59,   59,   0,  true   },
  {  L_, "\"23:59:59.000\"",          23,    59,   59,    0,   0,  true   },
  {  L_, "\"23:59:59.999\"",          23,    59,   59,  999,   0,  true   },
  {  L_, "\"23:59:59.999000\"",       23,    59,   59,  999,   0,  true   },
  {  L_, "\"23:59:59.999001\"",       23,    59,   59,  999,   1,  true   },
  {  L_, "\"23:59:59.999999\"",       23,    59,   59,  999, 999,  true   },
  {  L_, "\"23:59:59.999Z\"",         23,    59,   59,  999,   0,  true   },
  {  L_, "\"23:59:59.999z\"",         23,    59,   59,  999,   0,  true   },

  {  L_, "\"24:00:00\"",              24,     0,    0,    0,   0,  true   },
  {  L_, "\"24:00:00.000\"",          24,     0,    0,    0,   0,  true   },
  {  L_, "\"24:00:00.000000\"",       24,     0,    0,    0,   0,  true   },

  // DRQS 130122587: time parsing should accept offsets

  {  L_, "\"00:00:00.000+00:00\"",     0,     0,    0,    0,   0,  true   },
  {  L_, "\"00:00:00.000Z\"",          0,     0,    0,    0,   0,  true   },
  {  L_, "\"00:00:00.000z\"",          0,     0,    0,    0,   0,  true   },
  {  L_, "\"00:00:00.000+00:45\"",    23,    15,    0,    0,   0,  true   },
  {  L_, "\"00:00:00.000+23:59\"",     0,     1,    0,    0,   0,  true   },
  {  L_, "\"00:00:00.000-23:59\"",    23,    59,    0,    0,   0,  true   },

  {  L_, "\"00:00:00.999+00:00\"",     0,     0,    0,  999,   0,  true   },
  {  L_, "\"00:00:00.999+23:59\"",     0,     1,    0,  999,   0,  true   },
  {  L_, "\"00:00:00.999-23:59\"",    23,    59,    0,  999,   0,  true   },

  {  L_, "\"00:00:00.9999+00:00\"",    0,     0,    0,  999, 900,  true   },
  {  L_, "\"00:00:00.99999+00:00\"",   0,     0,    0,  999, 990,  true   },
  {  L_, "\"00:00:00.999999+00:00\"",  0,     0,    0,  999, 999,  true   },

  {  L_, "\"00:00:01.000+00:00\"",     0,     0,    1,    0,   0,  true   },
  {  L_, "\"00:00:01.000Z\"",          0,     0,    1,    0,   0,  true   },
  {  L_, "\"00:00:01.000z\"",          0,     0,    1,    0,   0,  true   },
  {  L_, "\"00:00:01.000+23:59\"",     0,     1,    1,    0,   0,  true   },
  {  L_, "\"00:00:01.000-23:59\"",    23,    59,    1,    0,   0,  true   },

  {  L_, "\"00:00:59.000+00:00\"",     0,     0,   59,    0,   0,  true   },
  {  L_, "\"00:00:59.000Z\"",          0,     0,   59,    0,   0,  true   },
  {  L_, "\"00:00:59.000z\"",          0,     0,   59,    0,   0,  true   },
  {  L_, "\"00:00:59.000+23:59\"",     0,     1,   59,    0,   0,  true   },
  {  L_, "\"00:00:59.000-23:59\"",    23,    59,   59,    0,   0,  true   },

  {  L_, "\"01:01:01.001+00:00\"",     1,     1,    1,    1,   0,  true   },
  {  L_, "\"01:01:01.001+08:20\"",    16,    41,    1,    1,   0,  true   },

  {  L_, "\"01:23:59.059+00:00\"",     1,    23,   59,   59,   0,  true   },
  {  L_, "\"01:23:59.059Z\"",          1,    23,   59,   59,   0,  true   },
  {  L_, "\"01:23:59.059z\"",          1,    23,   59,   59,   0,  true   },
  {  L_, "\"01:23:59.059+23:59\"",     1,    24,   59,   59,   0,  true   },
  {  L_, "\"01:23:59.059-23:59\"",     1,    22,   59,   59,   0,  true   },

  {  L_, "\"23:59:59.000+23:59\"",     0,     0,   59,    0,   0,  true   },
  {  L_, "\"23:59:59.000-23:59\"",    23,    58,   59,    0,   0,  true   },

  {  L_, "\"23:59:59.999+00:00\"",    23,    59,   59,  999,   0,  true   },
  {  L_, "\"23:59:59.999Z\"",         23,    59,   59,  999,   0,  true   },
  {  L_, "\"23:59:59.999z\"",         23,    59,   59,  999,   0,  true   },
  {  L_, "\"23:59:59.999+23:59\"",     0,     0,   59,  999,   0,  true   },
  {  L_, "\"23:59:59.999-23:59\"",    23,    58,   59,  999,   0,  true   },

  {  L_, "\"24:00:00.000+00:00\"",    24,     0,    0,    0,   0,  true   },
  {  L_, "\"24:00:00.000Z\"",         24,     0,    0,    0,   0,  true   },
  {  L_, "\"24:00:00.000z\"",         24,     0,    0,    0,   0,  true   },

  {  L_, "\"23:59:59.9999999\"",       0,     0,    0,    0,   0,  true   },

  {  L_, "\"GARBAGE\"",               24,     0,    0,    0,   0,  false  },
  {  L_, "\".9999\"",                 24,     0,    0,    0,   0,  false  },
         // ^
  {  L_, "\"23.9999\"",               24,     0,    0,    0,   0,  false  },
           // ^
  {  L_, "\"23:59.9999\"",            24,     0,    0,    0,   0,  false  },
              // ^

  {  L_, "\"25:00:00.000\"",          24,     0,    0,    0,   0,  false  },
  {  L_, "\"00:61:00.000\"",          24,     0,    0,    0,   0,  false  },
  {  L_, "\"00:00:61.000\"",          24,     0,    0,    0,   0,  false  },

  {  L_, "\"0001-01-01+00:00\"",      24,     0,    0,    0,   0,  false  },
  {  L_, "\"0001-01-01-00:00\"",      24,     0,    0,    0,   0,  false  },

  {  L_, "\"25:00:00.000+00:00\"",    24,     0,    0,    0,   0,  false  },
  {  L_, "\"00:61:00.000+00:00\"",    24,     0,    0,    0,   0,  false  },
  {  L_, "\"00:00:61.000+00:00\"",    24,     0,    0,    0,   0,  false  },
  {  L_, "\"00:00:00.000+24:00\"",    24,     0,    0,    0,   0,  false  },
  {  L_, "\"00:00:00.000+00:61\"",    24,     0,    0,    0,   0,  false  },
  {  L_, "\"00:00:00.000-24:00\"",    24,     0,    0,    0,   0,  false  },
  {  L_, "\"00:00:00.000-00:61\"",    24,     0,    0,    0,   0,  false  },

  {  L_, "\"0001-01-01T00:00:00.000+00:00\"",
                                      24,     0,    0,    0,   0,  false  },
        };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int         LINE        = DATA[i].d_line;
                const string      INPUT       = DATA[i].d_input_p;
                const int         HOUR        = DATA[i].d_hour;
                const int         MINUTE      = DATA[i].d_minutes;
                const int         SECOND      = DATA[i].d_seconds;
                const int         MILLISECOND = DATA[i].d_milliSecs;
                const int         MICROSECOND = DATA[i].d_uSecs;
                const bool        IS_VALID    = DATA[i].d_isValid;

                bdlt::Time exp(HOUR, MINUTE, SECOND, MILLISECOND, MICROSECOND);
                const bdlt::Time& EXP = exp;

                bdlt::Time value;

                const bsl::string_view isb(INPUT);

                const int rc = Util::getValue(&value, isb);
                if (IS_VALID) {
                    LOOP2_ASSERT(LINE, rc, 0 == rc);
                }
                else {
                    LOOP2_ASSERT(LINE, rc, rc);
                }
                LOOP3_ASSERT(LINE, EXP, value,   EXP == value);
            }
        }
      } break;
      case 13: {
        // --------------------------------------------------------------------
        // TESTING 'getQuotedString', 'getUnquotedString', and 'getValue' for
        // string values
        //
        // Concerns:
        //: 1 Values in the valid range, including the maximum and minimum
        //:   values for this type, are parsed correctly, for 'getQuotedString'
        //:   and 'getUnquotedString'.
        //:
        //: 2 'getValue' and 'getQuotedString' are equivalent.
        //:
        //: 3 The passed in variable is unmodified if the data is not valid.
        //:
        //: 4 For 'getUnquotedString', embedded unescaped '"' characters are
        //:   placed in the result.
        //:
        //: 5 The return code is 0 on success and non-zero on failure.
        //
        // Plan:
        //: 1 Using the table-driven technique, specify a set of distinct
        //:   rows of string value, expected parsed value, and return code.
        //:
        //: 2 For each row in the table of P-1:
        //:
        //:   1 Provide the string value and a variable to be parsed into to
        //:     the 'getValue' function.  The variable is assigned a sentinel
        //:     value before invoking the function.
        //:
        //:   2 If the parsing should succeed then verify that the variable
        //:     value matches the expected value.  Otherwise confirm that the
        //:     variable value is unmodified.
        //:
        //:   3 Confirm that the return code is 0 on success and non-zero
        //:     otherwise.
        //:
        //:   4 Call 'getQuotedString', making sure the result is identical in
        //:     return code and/or value.
        //:
        //:   5 Call 'getUnquotedString', making sure the result (if
        //:     successful) includes the extra outer '"'.
        //:
        //: 3 Using the table-driven technique, specify a different set of
        //:   distinct rows of string value, expected parse value, and return
        //:   code for testing 'getUnquotedString'.
        //
        // Testing:
        //   static int getQuotedString(bsl::string *, bsl::string_view);
        //   static int getUnquotedString(bsl::string *, bsl::string_view);
        //   static int getValue(bsl::string *, bsl::string_view);
        // --------------------------------------------------------------------

        if (verbose)
            cout << "\nTESTING 'getQuotedString', 'getUnquotedString', and "
                    "'getValue' for string values"
                    "\n===================================================="
                    "============================" << endl;
        {
            typedef bsl::string Type;

            const Type LDELIM("<<");
            const Type RDELIM(">>");

            const char *ERROR_VALUE = "";

            static const struct Data {
                int         d_line;    // line number
                const char *d_input_p; // input on the stream
                int         d_inputLen;
                const char *d_exp_p;   // expected value
                int         d_expLen;
                bool        d_isValid; // isValid flag
            } DATA[] = {
                //line input             len exp                     isValid
                //---- -----             --- ---                    -------
                {  L_, "\"\"",           -1, "",                      -1, 1  },
                {  L_, "\"ABC\"",        -1, "ABC",                   -1, 1  },
                {  L_, "\"\\\"\"",       -1, "\"",                    -1, 1  },
                {  L_, "\"\\\\\"",       -1, "\\",                    -1, 1  },
                {  L_, "\"\\b\"",        -1, "\b",                    -1, 1  },
                {  L_, "\"\\f\"",        -1, "\f",                    -1, 1  },
                {  L_, "\"\\n\"",        -1, "\n",                    -1, 1  },
                {  L_, "\"\\r\"",        -1, "\r",                    -1, 1  },
                {  L_, "\"\\t\"",        -1, "\t",                    -1, 1  },

                {  L_, "\"u0001\"",      -1, "u0001",                 -1, 1  },
                {  L_, "\"UABCD\"",      -1, "UABCD",                 -1, 1  },

                {  L_, "\"\\u0001\"",    -1, "\x01",                  -1, 1  },
                {  L_, "\"\\u0020\"",    -1, " ",                     -1, 1  },
                {  L_, "\"\\u002E\"",    -1, ".",                     -1, 1  },
                {  L_, "\"\\u0041\"",    -1, "A",                     -1, 1  },

                {  L_, "\"\\U006d\"",    -1, "m",                     -1, 1  },
                {  L_, "\"\\U007E\"",    -1, "~",                     -1, 1  },

                {  L_, "\"\\U007F\"",    -1, "\x7F",                  -1, 1  },
                {  L_, "\"\\U0080\"",    -1, "\xC2\x80",              -1, 1  },

                {  L_, "\"\\U07FF\"",    -1, "\xDF\xBF",              -1, 1  },
                {  L_, "\"\\U0800\"",    -1, "\xE0\xA0\x80",          -1, 1  },

                {  L_, "\"\\UFFFF\"",    -1, "\xEF\xBF\xBF",          -1, 1  },

                {  L_, "\"\\U02f1\"",    -1, "\xCB\xB1",              -1, 1  },
                {  L_, "\"\\U2710\"",    -1, "\xE2\x9C\x90",          -1, 1  },
                {  L_, "\"\\UD7Ff\"",    -1, "\xED\x9F\xBF",          -1, 1  },
                {  L_, "\"\\Ue000\"",    -1, "\xEE\x80\x80",          -1, 1  },

                {  L_, "\"AB\"",         -1, "AB",                    -1, 1  },
                {  L_, "\"A\\u0020B\"",  -1, "A B",                   -1, 1  },
                {  L_, "\"A\\u002eB\"",  -1, "A.B",                   -1, 1  },
                {  L_, "\"A\\u0080G\"",  -1, "A\xC2\x80G",            -1, 1  },

                {  L_, "\"\\U0000\"",    -1, "\0",                     1, 1  },
                {  L_, "\"A\\U0000B\"",  -1, "A\0B",                   3, 1  },
                {  L_, "\"A\\U0000\\u0080G\"",
                                         -1, "A\0\xC2\x80G",           5, 1  },
                {  L_, "\"A\\U0000\\ue000G\"",
                                         -1, "A\0\xEE\x80\x80G",       6, 1  },

                {  L_, "\"\0\"",          3, "\0",                     1, 1  },

                {  L_, "\"\\U02f1\0\"",   9, "\xCB\xB1\0",             3, 1  },
                {  L_, "\"\\U2710\0\"",   9, "\xE2\x9C\x90\0",         4, 1  },
                {  L_, "\"\\UD7Ff\0\"",   9, "\xED\x9F\xBF\0",         4, 1  },
                {  L_, "\"\\Ue000\0\"",   9, "\xEE\x80\x80\0",         4, 1  },

                {  L_, "\"AB\0\"",        5, "AB\0",                   3, 1  },
                {  L_, "\"A\\u0020B\0\"",11, "A B\0",                  4, 1  },
                {  L_, "\"A\\u002eB\0\"",11, "A.B\0",                  4, 1  },
                {  L_, "\"A\\u0080G\0\"",11, "A\xC2\x80G\0",           5, 1  },

                {  L_, "\"\0\\U02f1\"",   9, "\0\xCB\xB1",             3, 1  },
                {  L_, "\"\0\\U2710\"",   9, "\0\xE2\x9C\x90",         4, 1  },
                {  L_, "\"\0\\UD7Ff\"",   9, "\0\xED\x9F\xBF",         4, 1  },
                {  L_, "\"\0\\Ue000\"",   9, "\0\xEE\x80\x80",         4, 1  },

                {  L_, "\"\0AB\"",        5, "\0AB",                   3, 1  },
                {  L_, "\"\0A\\u0020B\"",11, "\0A B",                  4, 1  },
                {  L_, "\"\0A\\u002eB\"",11, "\0A.B",                  4, 1  },
                {  L_, "\"\0A\\u0080G\"",11, "\0A\xC2\x80G",           5, 1  },

                {  L_, "\"\\U000G\"",    -1, ERROR_VALUE,             -1, 0  },
                {  L_, "\"\\U00h0\"",    -1, ERROR_VALUE,             -1, 0  },
                {  L_, "\"\\U0M00\"",    -1, ERROR_VALUE,             -1, 0  },
                {  L_, "\"\\UX000\"",    -1, ERROR_VALUE,             -1, 0  },

                {  L_, "\"\\U7G00\"",    -1, ERROR_VALUE,             -1, 0  },
                {  L_, "\"\\U007G\"",    -1, ERROR_VALUE,             -1, 0  },
                {  L_, "\"\\UXXXX\"",    -1, ERROR_VALUE,             -1, 0  },

                {  L_, "\"A\\U7G00B\"",  -1, "A",                     -1, 0  },
                {  L_, "\"A\\UXXXXB\"",  -1, "A",                     -1, 0  },

                // Do not allow early '"' in quoted strings
                {  L_, "\"\"abcd\"",     -1, "",                      -1, 0  },
                {  L_, "\"ab\"cd\"",     -1, "ab",                    -1, 0  },
                {  L_, "\"abcd\"\"",     -1, "abcd",                  -1, 0  },

                // Values that 'strtol' accepts - '{DRQS 162368243}'.
                {  L_, "\"\\U0xFF\"",    -1, ERROR_VALUE,             -1, 0  },
                {  L_, "\"\\U   4\"",    -1, ERROR_VALUE,             -1, 0  },
                {  L_, "\"\\U  -1\"",    -1, ERROR_VALUE,             -1, 0  },
                {  L_, "\"\\ud83d\\u0xFF\"", -1, ERROR_VALUE,         -1, 0  },
                {  L_, "\"\\ud83d\\u   4\"", -1, ERROR_VALUE,         -1, 0  },
                {  L_, "\"\\ud83d\\u  -1\"", -1, ERROR_VALUE,         -1, 0  },

                // These error strings were copied from
                // 'bdlde_charconvertutf32' test driver.

                // values that are not valid Unicode because they are in the
                // lower UTF-16 bit plane.

                {  L_, "\"\\UD800\"",    -1, ERROR_VALUE,             -1, 0  },
                {  L_, "\"\\uD8ff\"",    -1, ERROR_VALUE,             -1, 0  },
                {  L_, "\"\\ud917\"",    -1, ERROR_VALUE,             -1, 0  },
                {  L_, "\"\\Udaaf\"",    -1, ERROR_VALUE,             -1, 0  },
                {  L_, "\"\\Udb09\"",    -1, ERROR_VALUE,             -1, 0  },
                {  L_, "\"\\UdbFF\"",    -1, ERROR_VALUE,             -1, 0  },

                // values that are not valid Unicode because they are in the
                // upper UTF-16 bit plane.

                {  L_, "\"\\UDc00\"",    -1, ERROR_VALUE,             -1, 0  },
                {  L_, "\"\\UDcFF\"",    -1, ERROR_VALUE,             -1, 0  },
                {  L_, "\"\\UDd80\"",    -1, ERROR_VALUE,             -1, 0  },
                {  L_, "\"\\UDea7\"",    -1, ERROR_VALUE,             -1, 0  },
                {  L_, "\"\\UDF03\"",    -1, ERROR_VALUE,             -1, 0  },
                {  L_, "\"\\UDFFF\"",    -1, ERROR_VALUE,             -1, 0  },

                // Supplementary plane characters.

                {  L_, "\"\\ud800\\udbff\"", -1, ERROR_VALUE,         -1, 0  },
                {  L_, "\"\\udbad\\udbff\"", -1, ERROR_VALUE,         -1, 0  },
                {  L_, "\"\\udbff\\udbff\"", -1, ERROR_VALUE,         -1, 0  },
                {  L_, "\"\\ud800\\udc00\"", -1, "\xF0\x90\x80\x80",  -1, 1  },
                {  L_, "\"\\ud83d\\ude42\"", -1, "\xF0\x9F\x99\x82",  -1, 1  },
                {  L_, "\"\\udbff\\udfff\"", -1, "\xF4\x8F\xBF\xBF",  -1, 1  },
                {  L_, "\"\\ud800\\ue000\"", -1, ERROR_VALUE,         -1, 0  },
                {  L_, "\"\\udbad\\ue000\"", -1, ERROR_VALUE,         -1, 0  },
                {  L_, "\"\\udbff\\ue000\"", -1, ERROR_VALUE,         -1, 0  },
                {  L_, "\"\\ud83d\\ude4\"",  -1, ERROR_VALUE,         -1, 0  },
                {  L_, "\"\\ud83d\\ude4",    -1, ERROR_VALUE,         -1, 0  },
                {  L_, "\"\\ud83d\\ude\"",   -1, ERROR_VALUE,         -1, 0  },
                {  L_, "\"\\ud83d\\ude",     -1, ERROR_VALUE,         -1, 0  },
                {  L_, "\"\\ud83d\\ud\"",    -1, ERROR_VALUE,         -1, 0  },
                {  L_, "\"\\ud83d\\ud",      -1, ERROR_VALUE,         -1, 0  },
                {  L_, "\"\\ud83d\\u\"",     -1, ERROR_VALUE,         -1, 0  },
                {  L_, "\"\\ud83d\\u",       -1, ERROR_VALUE,         -1, 0  },
                {  L_, "\"\\ud83d\\\"",      -1, ERROR_VALUE,         -1, 0  },
                {  L_, "\"\\ud83d\\",        -1, ERROR_VALUE,         -1, 0  },
                {  L_, "\"\\ud83d\\ude4`\"", -1, ERROR_VALUE,         -1, 0  },
                {  L_, "\"\\ud83d\\ude4g\"", -1, ERROR_VALUE,         -1, 0  },
            };
            const int NUM_DATA = sizeof(DATA) / sizeof(*DATA);

            for (int i = 0; i < NUM_DATA; ++i) {
                const Data&        data     = DATA[i];
                const int          LINE     = data.d_line;
                const char        *IN_P     = data.d_input_p;
                const bsl::size_t  IN_LEN   = data.d_inputLen < 0
                                            ? bsl::strlen(IN_P)
                                            : data.d_inputLen;
                const char        *EXP_P    = data.d_exp_p;
                const bsl::size_t  EXP_LEN  = data.d_expLen < 0
                                            ? bsl::strlen(EXP_P)
                                            : data.d_expLen;
                const bool         IS_VALID = data.d_isValid;

                const bsl::string_view isb(IN_P,  IN_LEN);
                const bsl::string_view unq_isb(IN_P + 1, IN_LEN - 2);
                const bsl::string_view EXP(EXP_P, EXP_LEN);

                Type value  = ERROR_VALUE;
                Type value2 = ERROR_VALUE;
                Type value3 = ERROR_VALUE;

                const int rc  = Util::getValue(&value, isb);
                const int rc2 = Util::getQuotedString(&value2, isb);
                const int rc3 = Util::getUnquotedString(&value3, unq_isb);

                ASSERTV(rc, rc2, rc == rc2);
                ASSERTV(value, value2, value == value2);
                ASSERTV(value, value3, value == value3);

                if (IS_VALID) {
                    ASSERTV(LINE, rc,  0 == rc);
                    ASSERTV(LINE, rc3, 0 == rc3);
                }
                else {
                    ASSERTV(LINE, rc, 0 != rc);
                }
                ASSERTV(LINE, EXP, value, EXP == value);
            }

            static const Data DATA2[] = {
                //line   input   len    exp   expLen     isValid
                //----   -----   ---    ---   ------     -------
                {  L_,   "",     -1,    "",       -1,    true   },
                {  L_,   "a",    -1,    "a",       1,    true   },
                {  L_,   "ab",   -1,    "ab",      2,    true   },
            };

            const int NUM_DATA2 = sizeof(DATA2) / sizeof(*DATA2);

            for (int i = 0; i < NUM_DATA2; ++i) {
                const Data&        data     = DATA2[i];
                const int          LINE     = data.d_line;
                const char        *IN_P     = data.d_input_p;
                const bsl::size_t  IN_LEN   = data.d_inputLen < 0
                                            ? bsl::strlen(IN_P)
                                            : data.d_inputLen;
                const char        *EXP_P    = data.d_exp_p;
                const bsl::size_t  EXP_LEN  = data.d_expLen < 0
                                            ? bsl::strlen(EXP_P)
                                            : data.d_expLen;
                const bool         IS_VALID = data.d_isValid;

                const bsl::string_view isb(IN_P,  IN_LEN);
                const bsl::string_view EXP(EXP_P, EXP_LEN);

                Type value = ERROR_VALUE;

                const int rc = Util::getUnquotedString(&value, isb);

                if (IS_VALID) {
                    ASSERTV(LINE, rc, 0 == rc);
                }
                else {
                    ASSERTV(LINE, rc, rc);
                }
                ASSERTV(LINE, EXP, value, EXP == value);
            }
        }
      } break;
      case 12: {
        // --------------------------------------------------------------------
        // TESTING 'getValue' for double values
        //
        // Concerns:
        //: 1 Values in the valid range, including the maximum and minimum
        //:   values for this type, are parsed correctly.
        //:
        //: 2 The passed in variable is unmodified if the data is not valid.
        //:
        //: 3 The return code is 0 on success and non-zero on failure.
        //
        // Plan:
        //: 1 Using the table-driven technique, specify a set of distinct
        //:   rows of string value, expected parsed value, and return code.
        //:
        //: 2 For each row in the table of P-1:
        //:
        //:   1 Provide the string value and a variable  to be parsed into to
        //:     the 'getValue' function.  The variable is assigned a sentinel
        //:     value before invoking the function.
        //:
        //:   2 If the parsing should succeed then verify that the variable
        //:     value matches the expected value.  Otherwise confirm that the
        //:     variable value is unmodified.
        //:
        //:   3 Confirm that the return code is 0 on success and non-zero
        //:     otherwise.
        //
        // Testing:
        //   static int getValue(double              *v, bsl::string_view s);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING 'getValue' for double"
                          << "\n=============================" << endl;
        {
            typedef double Type;

            const Type ERROR_VALUE = 99.99;

            static const struct {
                int         d_line;    // line number
                const char *d_input_p; // input on the stream
                Type        d_exp;     // exp unsigned value
                bool        d_isValid; // isValid flag
            } DATA[] = {
                //line    input                       exp    isValid
                //----    -----                       ---    -------
                {  L_,      "0",                      0.0,     true    },
                {  L_,     "-0",                      0.0,     true    },
                {  L_,     "0.0",                     0.0,     true    },
                {  L_,     "-0.0",                    0.0,     true    },
                {  L_,      "1",                      1.0,     true    },
                {  L_,     "-1",                     -1.0,     true    },
                {  L_,     "0.1",                     0.1,     true    },
                {  L_,     "-0.1",                   -0.1,     true    },
                {  L_,     "0.12345",             0.12345,     true    },
                {  L_,     "-1.234567e-35",  -1.234567e-35,     true    },
                {  L_,     "1234567.0",         1234567.0,     true    },
                {  L_,     "1234567e+35",       1234567e35,     true    },

                {  L_,     "95",                      95,       true    },
                {  L_,    "127",                     127,       true    },
                {  L_,    "128",                     128,       true    },
                {  L_,    "200",                     200,       true    },
                {  L_,    "255",                     255,       true    },
                {  L_,  "32767",                   32767,       true    },
                {  L_,  "32768",                   32768,       true    },
                {  L_,  "65534",                   65534,       true    },
                {  L_,  "65535",                   65535,       true    },

                {  L_,   "8388607",              8388607,       true    },
                {  L_,   "8388608",              8388608,       true    },

                {  L_,   "2147483646",        2147483646,       true    },
                {  L_,   "2147483647",        2147483647,       true    },

                {  L_,   "4294967294",      4294967294LL,       true    },
                {  L_,   "4294967295",      4294967295LL,       true    },

                {  L_,    "1.1",                     1.1,       true    },
                {  L_,    "1.5",                     1.5,       true    },
                {  L_,    "1.9",                     1.9,       true    },

                {  L_,   "100.123",              100.123,      true    },
                {  L_,   "99.5",                    99.5,      true    },
                {  L_,    "0.86",                   0.86,      true    },

                {  L_,    "1e0",                       1,       true    },
                {  L_,    "1E0",                       1,       true    },
                {  L_,    "1e+0",                      1,       true    },
                {  L_,    "1E+0",                      1,       true    },
                {  L_,    "1e-0",                      1,       true    },
                {  L_,    "1E-0",                      1,       true    },

                {  L_,    "1.234e+1",             12.34,       true    },
                {  L_,    "1.987E+1",             19.87,       true    },

                {  L_,    "12.34e-1",              1.234,      true    },
                {  L_,    "29.87E-1",              2.987,      true    },

                {  L_,    "-123.34e-1",          -12.334,      true    },
                {  L_,    "-298.7E-1",            -29.87,      true    },

                {  L_,    "1e1",                      10,       true    },
                {  L_,    "1E1",                      10,       true    },
                {  L_,    "1e+1",                     10,       true    },
                {  L_,    "1E+1",                     10,       true    },
                {  L_,    "1e-1",                    0.1,       true    },
                {  L_,    "1E-1",                    0.1,       true    },

                {  L_,    "\"NaN\"", bsl::numeric_limits<Type>::quiet_NaN(),
                                                                true    },

                {  L_,    "\"nan\"",  bsl::numeric_limits<Type>::quiet_NaN(),
                                                                true    },

                {  L_,    "\"NAN\"",  bsl::numeric_limits<Type>::quiet_NaN(),
                                                                true    },

                {  L_,    "\"+NaN\"", bsl::numeric_limits<Type>::quiet_NaN(),
                                                                true    },

                {  L_,    "\"+nan\"",  bsl::numeric_limits<Type>::quiet_NaN(),
                                                                true    },

                {  L_,    "\"+NAN\"",  bsl::numeric_limits<Type>::quiet_NaN(),
                                                                true    },

                {  L_,    "\"-NaN\"", -bsl::numeric_limits<Type>::quiet_NaN(),
                                                                true    },

                {  L_,    "\"-nan\"", -bsl::numeric_limits<Type>::quiet_NaN(),
                                                                true    },

                {  L_,    "\"-NAN\"", -bsl::numeric_limits<Type>::quiet_NaN(),
                                                                true    },

                {  L_,    "\"INF\"",  bsl::numeric_limits<Type>::infinity(),
                                                                true    },

                {  L_,    "\"inf\"",  bsl::numeric_limits<Type>::infinity(),
                                                                true    },

                {  L_,    "\"infinity\"",
                                      bsl::numeric_limits<Type>::infinity(),
                                                                true    },

                {  L_,    "\"+INF\"", bsl::numeric_limits<Type>::infinity(),
                                                                true    },

                {  L_,    "\"+inf\"", bsl::numeric_limits<Type>::infinity(),
                                                                true    },

                {  L_,    "\"+infinity\"",
                                      bsl::numeric_limits<Type>::infinity(),
                                                                true    },

                {  L_,    "\"-INF\"", -bsl::numeric_limits<Type>::infinity(),
                                                                true    },

                {  L_,    "\"-inf\"", -bsl::numeric_limits<Type>::infinity(),
                                                                true    },

                {  L_,    "\"-infinity\"",
                                      -bsl::numeric_limits<Type>::infinity(),
                                                                true    },

                {  L_,  "-",          ERROR_VALUE,   false   },
                {  L_,  ".5",         ERROR_VALUE,   false   },
                {  L_,  "-.5",        ERROR_VALUE,   false   },
                {  L_,  "e",          ERROR_VALUE,   false   },
                {  L_,  "e1",         ERROR_VALUE,   false   },
                {  L_,  "E",          ERROR_VALUE,   false   },
                {  L_,  "E1",         ERROR_VALUE,   false   },
                {  L_,  "e+",         ERROR_VALUE,   false   },
                {  L_,  "e+1",        ERROR_VALUE,   false   },
                {  L_,  "E+",         ERROR_VALUE,   false   },
                {  L_,  "E+1",        ERROR_VALUE,   false   },
                {  L_,  "e-",         ERROR_VALUE,   false   },
                {  L_,  "e-1",        ERROR_VALUE,   false   },
                {  L_,  "E-",         ERROR_VALUE,   false   },
                {  L_,  "E-1",        ERROR_VALUE,   false   },

                {  L_,  "Z34.56e1",   ERROR_VALUE,   false   },
                {  L_,  "3Z4.56e1",   ERROR_VALUE,   false   },
                {  L_,  "34Z.56e1",   ERROR_VALUE,   false   },
                {  L_,  "34.Z6e1",    ERROR_VALUE,   false   },
                {  L_,  "34.5Ze1",    ERROR_VALUE,   false   },
                {  L_,  "34.56Z1",    ERROR_VALUE,   false   },
                {  L_,  "34.56eZ",    ERROR_VALUE,   false   },

// TBD:
//                 {  L_,    "0x12",     ERROR_VALUE,   false   },
//                 {  L_,    "0x256",    ERROR_VALUE,   false   },
                {  L_,    "1.1}",     ERROR_VALUE,   false   },
                {  L_,    "1.1,",     ERROR_VALUE,   false   },
                {  L_,    "1.1]",     ERROR_VALUE,   false   },
                {  L_,    "1.1a",     ERROR_VALUE,   false   },
                {  L_,    "1.1 ",     ERROR_VALUE,   false   },
                {  L_,    "1.1\n",    ERROR_VALUE,   false   },
                {  L_,    "1.10xFF",  ERROR_VALUE,   false   },
                {  L_,    "DEADBEEF", ERROR_VALUE,   false   },
                {  L_,    "JUNK",     ERROR_VALUE,   false   },
            };
            const int NUM_DATA = sizeof(DATA) / sizeof(*DATA);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int    LINE     = DATA[i].d_line;
                const string INPUT    = DATA[i].d_input_p;
                const Type   EXP      = DATA[i].d_exp;
                const bool   IS_VALID = DATA[i].d_isValid;
                      Type   value    = ERROR_VALUE;

                const bsl::string_view isb(INPUT);

                bslma::TestAllocator         da("default", veryVeryVerbose);
                bslma::DefaultAllocatorGuard dag(&da);

                const int rc = Util::getValue(&value, isb);
                if (IS_VALID) {
                    LOOP2_ASSERT(LINE, rc, 0 == rc);
                }
                else {
                    LOOP2_ASSERT(LINE, rc, rc);
                }

                if (bdlb::Float::isNan(EXP)) {
                    LOOP_ASSERT(LINE, bdlb::Float::isNan(value));
                }
                else {
                    LOOP3_ASSERT(LINE, EXP, value, EXP == value);
                }

                ASSERTV(LINE, da.numBlocksTotal(), 0 == da.numBlocksTotal());
            }
        }
      } break;
      case 11: {
        // --------------------------------------------------------------------
        // TESTING 'getValue' for float values
        //
        // Concerns:
        //: 1 Values in the valid range, including the maximum and minimum
        //:   values for this type, are parsed correctly.
        //:
        //: 2 The passed in variable is unmodified if the data is not valid.
        //:
        //: 3 The return code is 0 on success and non-zero on failure.
        //
        // Plan:
        //: 1 Using the table-driven technique, specify a set of distinct
        //:   rows of string value, expected parsed value, and return code.
        //:
        //: 2 For each row in the table of P-1:
        //:
        //:   1 Provide the string value and a variable  to be parsed into to
        //:     the 'getValue' function.  The variable is assigned a sentinel
        //:     value before invoking the function.
        //:
        //:   2 If the parsing should succeed then verify that the variable
        //:     value matches the expected value.  Otherwise confirm that the
        //:     variable value is unmodified.
        //:
        //:   3 Confirm that the return code is 0 on success and non-zero
        //:     otherwise.
        //
        // Testing:
        //   static int getValue(float               *v, bsl::string_view s);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING 'getValue' for float"
                          << "\n============================" << endl;
        {
            typedef float Type;

            const Type ERROR_VALUE = 99.99f;

            static const struct {
                int         d_line;    // line number
                const char *d_input_p; // input on the stream
                Type        d_exp;     // exp unsigned value
                bool        d_isValid; // isValid flag
            } DATA[] = {
                //line    input                        exp    isValid
                //----    -----                        ---    -------
                {  L_,      "0",                      0.0f,     true    },
                {  L_,     "-0",                      0.0f,     true    },
                {  L_,     "0.0",                     0.0f,     true    },
                {  L_,     "-0.0",                    0.0f,     true    },
                {  L_,      "1",                      1.0f,     true    },
                {  L_,     "-1",                     -1.0f,     true    },
                {  L_,     "0.1",                     0.1f,     true    },
                {  L_,     "-0.1",                   -0.1f,     true    },
                {  L_,     "0.12345",             0.12345f,     true    },
                {  L_,     "-1.234567e-20", -1.234567e-20f,     true    },
                {  L_,     "1234567.0",         1234567.0f,     true    },
                {  L_,     "1234567e+20",      1234567e20f,     true    },

                {  L_,     "95",                      95,       true    },
                {  L_,    "127",                     127,       true    },
                {  L_,    "128",                     128,       true    },
                {  L_,    "200",                     200,       true    },
                {  L_,    "255",                     255,       true    },
                {  L_,  "32767",                   32767,       true    },
                {  L_,  "32768",                   32768,       true    },
                {  L_,  "65534",                   65534,       true    },
                {  L_,  "65535",                   65535,       true    },

                {  L_,   "8388607",              8388607,       true    },
                {  L_,   "8388608",              8388608,       true    },

                {  L_,   "2147483646",     2147483646.0f,       true    },
                {  L_,   "2147483647",     2147483647.0f,       true    },

                {  L_,   "4294967294",     4294967294.0f,       true    },
                {  L_,   "4294967295",     4294967295.0f,       true    },

                {  L_,    "1.1",                    1.1f,       true    },
                {  L_,    "1.5",                    1.5f,       true    },
                {  L_,    "1.9",                    1.9f,       true    },

                {  L_,   "100.123",             100.123f,       true    },
                {  L_,   "99.5",                   99.5f,       true    },
                {  L_,    "0.86",                  0.86f,       true    },

                {  L_,    "1e0",                       1,       true    },
                {  L_,    "1E0",                       1,       true    },
                {  L_,    "1e+0",                      1,       true    },
                {  L_,    "1E+0",                      1,       true    },
                {  L_,    "1e-0",                      1,       true    },
                {  L_,    "1E-0",                      1,       true    },

                {  L_,    "1.234e+1",             12.34f,       true    },
                {  L_,    "1.987E+1",             19.87f,       true    },

                {  L_,    "12.34e-1",             1.234f,       true    },
                {  L_,    "29.87E-1",             2.987f,       true    },

                {  L_,    "-123.34e-1",         -12.334f,       true    },
                {  L_,    "-298.7E-1",           -29.87f,       true    },

                {  L_,    "1e1",                      10,       true    },
                {  L_,    "1E1",                      10,       true    },
                {  L_,    "1e+1",                     10,       true    },
                {  L_,    "1E+1",                     10,       true    },
                {  L_,    "1e-1",                   0.1f,       true    },
                {  L_,    "1E-1",                   0.1f,       true    },

                {  L_,    "\"NaN\"", bsl::numeric_limits<Type>::quiet_NaN(),
                                                                true    },

                {  L_,    "\"nan\"",  bsl::numeric_limits<Type>::quiet_NaN(),
                                                                true    },

                {  L_,    "\"NAN\"",  bsl::numeric_limits<Type>::quiet_NaN(),
                                                                true    },

                {  L_,    "\"+NaN\"", bsl::numeric_limits<Type>::quiet_NaN(),
                                                                true    },

                {  L_,    "\"+nan\"",  bsl::numeric_limits<Type>::quiet_NaN(),
                                                                true    },

                {  L_,    "\"+NAN\"",  bsl::numeric_limits<Type>::quiet_NaN(),
                                                                true    },

                {  L_,    "\"-NaN\"", -bsl::numeric_limits<Type>::quiet_NaN(),
                                                                true    },

                {  L_,    "\"-nan\"", -bsl::numeric_limits<Type>::quiet_NaN(),
                                                                true    },

                {  L_,    "\"-NAN\"", -bsl::numeric_limits<Type>::quiet_NaN(),
                                                                true    },

                {  L_,    "\"INF\"",  bsl::numeric_limits<Type>::infinity(),
                                                                true    },

                {  L_,    "\"inf\"",  bsl::numeric_limits<Type>::infinity(),
                                                                true    },

                {  L_,    "\"infinity\"",
                                      bsl::numeric_limits<Type>::infinity(),
                                                                true    },

                {  L_,    "\"+INF\"", bsl::numeric_limits<Type>::infinity(),
                                                                true    },

                {  L_,    "\"+inf\"", bsl::numeric_limits<Type>::infinity(),
                                                                true    },

                {  L_,    "\"+infinity\"",
                                      bsl::numeric_limits<Type>::infinity(),
                                                                true    },

                {  L_,    "\"-INF\"", -bsl::numeric_limits<Type>::infinity(),
                                                                true    },

                {  L_,    "\"-inf\"", -bsl::numeric_limits<Type>::infinity(),
                                                                true    },

                {  L_,    "\"-infinity\"",
                                      -bsl::numeric_limits<Type>::infinity(),
                                                                true    },

                {  L_,  "-",          ERROR_VALUE,   false   },
                {  L_,  ".5",         ERROR_VALUE,   false   },
                {  L_,  "-.5",        ERROR_VALUE,   false   },
                {  L_,  "e",          ERROR_VALUE,   false   },
                {  L_,  "e1",         ERROR_VALUE,   false   },
                {  L_,  "E",          ERROR_VALUE,   false   },
                {  L_,  "E1",         ERROR_VALUE,   false   },
                {  L_,  "e+",         ERROR_VALUE,   false   },
                {  L_,  "e+1",        ERROR_VALUE,   false   },
                {  L_,  "E+",         ERROR_VALUE,   false   },
                {  L_,  "E+1",        ERROR_VALUE,   false   },
                {  L_,  "e-",         ERROR_VALUE,   false   },
                {  L_,  "e-1",        ERROR_VALUE,   false   },
                {  L_,  "E-",         ERROR_VALUE,   false   },
                {  L_,  "E-1",        ERROR_VALUE,   false   },

                {  L_,  "Z34.56e1",   ERROR_VALUE,   false   },
                {  L_,  "3Z4.56e1",   ERROR_VALUE,   false   },
                {  L_,  "34Z.56e1",   ERROR_VALUE,   false   },
                {  L_,  "34.Z6e1",    ERROR_VALUE,   false   },
                {  L_,  "34.5Ze1",    ERROR_VALUE,   false   },
                {  L_,  "34.56Z1",    ERROR_VALUE,   false   },
                {  L_,  "34.56eZ",    ERROR_VALUE,   false   },

// TBD:
//                 {  L_,    "0x12",     ERROR_VALUE,   false   },
//                 {  L_,    "0x256",    ERROR_VALUE,   false   },
                {  L_,    "DEADBEEF", ERROR_VALUE,   false   },
                {  L_,    "JUNK",     ERROR_VALUE,   false   },
            };
            const int NUM_DATA = sizeof(DATA) / sizeof(*DATA);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int    LINE     = DATA[i].d_line;
                const string INPUT    = DATA[i].d_input_p;
                const Type   EXP      = DATA[i].d_exp;
                const bool   IS_VALID = DATA[i].d_isValid;
                      Type   value    = ERROR_VALUE;

                const bsl::string_view isb(INPUT);

                bslma::TestAllocator         da("default", veryVeryVerbose);
                bslma::DefaultAllocatorGuard dag(&da);

                const int rc = Util::getValue(&value, isb);
                if (IS_VALID) {
                    LOOP2_ASSERT(LINE, rc, 0 == rc);
                }
                else {
                    LOOP2_ASSERT(LINE, rc, rc);
                }

                if (bdlb::Float::isNan(EXP)) {
                    LOOP_ASSERT(LINE, bdlb::Float::isNan(value));
                }
                else {
                    LOOP3_ASSERT(LINE, EXP, value, EXP == value);
                }

                ASSERTV(LINE, da.numBlocksTotal(), 0 == da.numBlocksTotal());
            }
        }
      } break;
      case 10: {
        // --------------------------------------------------------------------
        // TESTING 'getValue' for Uint64 values
        //
        // Concerns:
        //: 1 Values in the valid range, including the maximum and minimum
        //:   values for this type, are parsed correctly.
        //:
        //: 2 The passed in variable is unmodified if the data is not valid.
        //:
        //: 3 The return code is 0 on success and non-zero on failure.
        //
        // Plan:
        //: 1 Using the table-driven technique, specify a set of distinct
        //:   rows of string value, expected parsed value, and return code.
        //:
        //: 2 For each row in the table of P-1:
        //:
        //:   1 Provide the string value and a variable  to be parsed into to
        //:     the 'getValue' function.  The variable is assigned a sentinel
        //:     value before invoking the function.
        //:
        //:   2 If the parsing should succeed then verify that the variable
        //:     value matches the expected value.  Otherwise confirm that the
        //:     variable value is unmodified.
        //:
        //:   3 Confirm that the return code is 0 on success and non-zero
        //:     otherwise.
        //
        // Testing:
        //   static int getValue(bsls::Types::Uint64 *v, bsl::string_view s);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING 'getValue' for Uint64"
                          << "\n=============================" << endl;

        {
            typedef Uint64 Type;

            const Type ERROR_VALUE = 99;
            const Type MAX         = bsl::numeric_limits<Type>::max();
            const Type MIN         = bsl::numeric_limits<Type>::min();

            static const struct {
                int         d_line;    // line number
                const char *d_input_p; // input on the stream
                Type        d_exp;     // exp unsigned value
                bool        d_isValid; // isValid flag
            } DATA[] = {
   //line          input                    exp     isValid
   //----          -----                    ---     -------
    {  L_,           "0",                     0,      true    },
    {  L_,           "1",                     1,      true    },
    {  L_,          "95",                    95,      true    },
    {  L_,         "127",                   127,      true    },
    {  L_,         "128",                   128,      true    },
    {  L_,         "200",                   200,      true    },
    {  L_,         "255",                   255,      true    },
    {  L_,         "256",                   256,      true    },
    {  L_,       "32766",                 32766,      true    },
    {  L_,       "32767",                 32767,      true    },
    {  L_,       "65534",                 65534,      true    },
    {  L_,       "65535",                 65535,      true    },
    {  L_,     "8388607",               8388607,      true    },
    {  L_,     "8388608",               8388608,      true    },
    {  L_,  "2147483646",            2147483646,      true    },
    {  L_,  "2147483647",            2147483647,      true    },
    {  L_,  "4294967294",            4294967294LL,    true    },
    {  L_,  "4294967295",            4294967295LL,    true    },
    {  L_,  "9223372036854775806",   9223372036854775806LL, true },
    {  L_,  "9223372036854775807",   9223372036854775807LL, true },

    {  L_,           "0",                   MIN,      true    },
    {  L_,           "1",               MIN + 1,      true    },
    {  L_, "18446744073709551614",      MAX - 1,      true    },
    {  L_, "18446744073709551615",          MAX,      true    },

    {  L_,           "0.0",                   0,      true    },
    {  L_,           "0.",                    0,      true    },
    {  L_,           "0.0000000000000",       0,      true    },
    {  L_,           "1.0",                   1,      true    },
    {  L_,           "1.",                    1,      true    },
    {  L_,           "1.00000000000",         1,      true    },
    {  L_,         "95.0",                   95,      true    },
    {  L_,         "95.0000000000",          95,      true    },
    {  L_,        "127.00",                 127,      true    },
    {  L_,        "128.00",                 128,      true    },
    {  L_,        "200.00",                 200,      true    },
    {  L_,        "255.00",                 255,      true    },
    {  L_,        "256.00",                 256,      true    },
    {  L_,  "9223372036854775806.0",        9223372036854775806LL, true },
    {  L_,  "9223372036854775806.00000000", 9223372036854775806LL, true },
    {  L_,  "9223372036854775807.0",        9223372036854775807LL, true },
    {  L_,  "9223372036854775807.00000000", 9223372036854775807LL, true },
    {  L_, "18446744073709551614.0",        18446744073709551614ULL, true },
    {  L_, "18446744073709551614.00000000", 18446744073709551614ULL, true },
    {  L_, "18446744073709551615.0",        18446744073709551615ULL, true },
    {  L_, "18446744073709551615.00000000", 18446744073709551615ULL, true },

    {  L_,          "1e0",                    1,      true    },
    {  L_,          "1E0",                    1,      true    },
    {  L_,          "1e+0",                   1,      true    },
    {  L_,          "1E+0",                   1,      true    },
    {  L_,          "1e-0",                   1,      true    },
    {  L_,          "1E-0",                   1,      true    },

    {  L_,          "1e1",                   10,      true    },
    {  L_,          "2E1",                   20,      true    },
    {  L_,          "3e+1",                  30,      true    },
    {  L_,          "4E+1",                  40,      true    },

    {  L_,          "2e2",                  200,      true    },
    {  L_,          "3E2",                  300,      true    },
    {  L_,          "4e+2",                 400,      true    },
    {  L_,          "5E+2",                 500,      true    },

    {  L_,          "0.1e1",                  1,      true    },
    {  L_,          "0.2e2",                 20,      true    },
    {  L_,          "0.30e2",                30,      true    },
    {  L_,          "0.400e3",              400,      true    },

{ L_,               "0.18446744073709551615e20", 18446744073709551615ULL,true},
{ L_,               "1.844674407370955161500e19",18446744073709551615ULL,true},
{ L_,              "18.44674407370955161500e18", 18446744073709551615ULL,true},
{ L_,             "184.4674407370955161500e17",  18446744073709551615ULL,true},
{ L_,            "1844.674407370955161500e16",   18446744073709551615ULL,true},
{ L_,           "18446.74407370955161500e15",    18446744073709551615ULL,true},
{ L_,          "184467.4407370955161500e14",     18446744073709551615ULL,true},
{ L_,         "1844674.407370955161500e13",      18446744073709551615ULL,true},
{ L_,        "18446744.07370955161500e12",       18446744073709551615ULL,true},
{ L_,       "184467440.7370955161500e11",        18446744073709551615ULL,true},
{ L_,      "1844674407.370955161500e10",         18446744073709551615ULL,true},
{ L_,     "18446744073.70955161500e9",           18446744073709551615ULL,true},
{ L_,    "184467440737.0955161500e8",            18446744073709551615ULL,true},
{ L_,   "1844674407370.955161500e7",             18446744073709551615ULL,true},
{ L_,  "18446744073709.55161500e6",              18446744073709551615ULL,true},
{ L_, "184467440737095.5161500e5",               18446744073709551615ULL,true},
{ L_, "1844674407370955.161500e4",               18446744073709551615ULL,true},
{ L_, "18446744073709551.61500e3",               18446744073709551615ULL,true},
{ L_, "184467440737095516.1500e2",               18446744073709551615ULL,true},
{ L_, "1844674407370955161.500e1",               18446744073709551615ULL,true},
{ L_, "18446744073709551615.00e0",               18446744073709551615ULL,true},

 {  L_,  "1844674407370955161500000e-5",       18446744073709551615ULL, true },
 {  L_,  "1844674407370955161500000.00000e-5", 18446744073709551615ULL, true },
 {  L_,   "184467440737095516150000e-4",       18446744073709551615ULL, true },
 {  L_,    "18446744073709551615000e-3",       18446744073709551615ULL, true },
 {  L_,     "1844674407370955161500e-2",       18446744073709551615ULL, true },
 {  L_,      "184467440737095516150e-1",       18446744073709551615ULL, true },
 {  L_,       "18446744073709551615e-0",       18446744073709551615ULL, true },

    {  L_,          "1.0e0",                  1,      true    },
    {  L_,          "2.000e0",                2,      true    },
    {  L_,          "3.0e1",                 30,      true    },
    {  L_,          "4.5e1",                 45,      true    },
    {  L_,          "6.00e1",                60,      true    },
    {  L_,          "7.00e2",               700,      true    },
    {  L_,          "8.0000e2",             800,      true    },
    {  L_,          "9.12e2",               912,      true    },
    {  L_,          "1.1200e2",             112,      true    },

    {  L_,           ".5",          ERROR_VALUE,      false   },
    {  L_,           ".123",        ERROR_VALUE,      false   },

    {  L_,          "1.1",          ERROR_VALUE,      false   },
    {  L_,          "1.5",          ERROR_VALUE,      false   },
    {  L_,          "1.9",          ERROR_VALUE,      false   },

    {  L_,        "100.123",        ERROR_VALUE,      false   },
    {  L_,         "99.5",          ERROR_VALUE,      false   },
    {  L_,          "0.86",         ERROR_VALUE,      false   },

    {  L_,        "255.01",         ERROR_VALUE,      false   },
    {  L_,        "255.99",         ERROR_VALUE,      false   },

    {  L_,          "1.234e+1",     ERROR_VALUE,      false   },
    {  L_,          "1.987E+1",     ERROR_VALUE,      false   },

    {  L_,          "1e1e1",        ERROR_VALUE,      false   },
    {  L_,          "1e1e-1",       ERROR_VALUE,      false   },

    {  L_,         "12.34e-1",      ERROR_VALUE,      false   },
    {  L_,         "29.87E-1",      ERROR_VALUE,      false   },

    {  L_,         "12345e-1",      ERROR_VALUE,      false   },
    {  L_,         "12345e-2",      ERROR_VALUE,      false   },
    {  L_,         "12345e-4",      ERROR_VALUE,      false   },
    {  L_,         "12345e-5",      ERROR_VALUE,      false   },

    {  L_,         "123.45e-1",     ERROR_VALUE,      false   },
    {  L_,         "1234.5e-2",     ERROR_VALUE,      false   },

    {  L_,          "1.0000000001", ERROR_VALUE,      false   },
    {  L_,          "1.00001e0",    ERROR_VALUE,      false   },

    {  L_,   "18446744073709551616", ERROR_VALUE,     false   },

    {  L_,   "18446744073709551615.01",     ERROR_VALUE,  false   },
    {  L_,   "18446744073709551615.0.",     ERROR_VALUE,  false   },
    {  L_,   "18446744073709551615.0.0",    ERROR_VALUE,  false   },
    {  L_,   "18446744073709551615.99",     ERROR_VALUE,  false   },

    {  L_,   "18446744073709551616.01", ERROR_VALUE,  false   },
    {  L_,   "18446744073709551616.99", ERROR_VALUE,  false   },

    {  L_,        ".",              ERROR_VALUE,      false   },
    {  L_,        "-.",             ERROR_VALUE,      false   },
    {  L_,        "-",              ERROR_VALUE,      false   },
    {  L_,        ".5",             ERROR_VALUE,      false   },
    {  L_,        "-.5",            ERROR_VALUE,      false   },
    {  L_,        "-1",             ERROR_VALUE,      false   },
    {  L_,        "-1.",            ERROR_VALUE,      false   },
    {  L_,        "-1.0",           ERROR_VALUE,      false   },
    {  L_,        "e",              ERROR_VALUE,      false   },
    {  L_,        "e1",             ERROR_VALUE,      false   },
    {  L_,        "E",              ERROR_VALUE,      false   },
    {  L_,        "E1",             ERROR_VALUE,      false   },
    {  L_,        "e+",             ERROR_VALUE,      false   },
    {  L_,        "e+1",            ERROR_VALUE,      false   },
    {  L_,        "E+",             ERROR_VALUE,      false   },
    {  L_,        "E+1",            ERROR_VALUE,      false   },
    {  L_,        "e-",             ERROR_VALUE,      false   },
    {  L_,        "e-1",            ERROR_VALUE,      false   },
    {  L_,        "E-",             ERROR_VALUE,      false   },
    {  L_,        "E-1",            ERROR_VALUE,      false   },

    {  L_,        "Z34.56e1",       ERROR_VALUE,      false   },
    {  L_,        "3Z4.56e1",       ERROR_VALUE,      false   },
    {  L_,        "34Z.56e1",       ERROR_VALUE,      false   },
    {  L_,         "34+56e1",       ERROR_VALUE,      false   },
    {  L_,         "34.Z6e1",       ERROR_VALUE,      false   },
    {  L_,         "34.5Ze1",       ERROR_VALUE,      false   },
    {  L_,         "34.56Z1",       ERROR_VALUE,      false   },
    {  L_,         "34.56eZ",       ERROR_VALUE,      false   },
    {  L_,         "34.56e+Z",      ERROR_VALUE,      false   },
    {  L_,         "34.56e-Z",      ERROR_VALUE,      false   },

    {  L_,   "100000000000000000000", ERROR_VALUE,    false   },
    {  L_,   "10000000000e10",        ERROR_VALUE,    false   },

    {  L_,         "0x12",          ERROR_VALUE,      false   },
    {  L_,         "0x256",         ERROR_VALUE,      false   },
    {  L_,         "JUNK",          ERROR_VALUE,      false   },
    {  L_,         "DEADBEEF",      ERROR_VALUE,      false   },

    {  L_,   "1E",                  ERROR_VALUE,      false   },
    {  L_,   "1E+",                 ERROR_VALUE,      false   },
    {  L_,   "1E-",                 ERROR_VALUE,      false   },
    {  L_,   "1E9999999999",        ERROR_VALUE,      false   },
    {  L_,   "1E-9999999999",       ERROR_VALUE,      false   },
            };
            const int NUM_DATA = sizeof(DATA) / sizeof(*DATA);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int    LINE     = DATA[i].d_line;
                const string INPUT    = DATA[i].d_input_p;
                const Type   EXP      = DATA[i].d_exp;
                const bool   IS_VALID = DATA[i].d_isValid;
                      Type   value    = ERROR_VALUE;

                const bsl::string_view isb(INPUT);

                bslma::TestAllocator         da("default", veryVeryVerbose);
                bslma::DefaultAllocatorGuard dag(&da);

                const int rc = Util::getValue(&value, isb);
                if (IS_VALID) {
                    LOOP2_ASSERT(LINE, rc, 0 == rc);
                }
                else {
                    LOOP2_ASSERT(LINE, rc, rc);
                }
                LOOP4_ASSERT(LINE, INPUT, EXP, value, EXP == value);

                ASSERTV(LINE, da.numBlocksTotal(), 0 == da.numBlocksTotal());
            }

            for (Type i = 0; i <= 255; ++i) {
                char                   c        = static_cast<char>(i);
                const bsl::string_view isb(&c, 1);
                bool                   is_valid = '0' <= c && c <= '9';
                Type                   value    = ERROR_VALUE;
                const int              rc       = Util::getValue(&value, isb);

                ASSERTV(rc, i, is_valid, is_valid == (0 == rc));
                ASSERTV(value, is_valid, !is_valid || value == i - '0');
            }
        }
      } break;
      case 9: {
        // --------------------------------------------------------------------
        // TESTING 'getValue' for Int64 values
        //
        // Concerns:
        //: 1 Values in the valid range, including the maximum and minimum
        //:   values for this type, are parsed correctly.
        //:
        //: 2 The passed in variable is unmodified if the data is not valid.
        //:
        //: 3 The return code is 0 on success and non-zero on failure.
        //
        // Plan:
        //: 1 Using the table-driven technique, specify a set of distinct
        //:   rows of string value, expected parsed value, and return code.
        //:
        //: 2 For each row in the table of P-1:
        //:
        //:   1 Provide the string value and a variable  to be parsed into to
        //:     the 'getValue' function.  The variable is assigned a sentinel
        //:     value before invoking the function.
        //:
        //:   2 If the parsing should succeed then verify that the variable
        //:     value matches the expected value.  Otherwise confirm that the
        //:     variable value is unmodified.
        //:
        //:   3 Confirm that the return code is 0 on success and non-zero
        //:     otherwise.
        //
        // Testing:
        //   static int getValue(bsls::Types::Int64  *v, bsl::string_view s);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING 'getValue' for Int64"
                          << "\n============================" << endl;

        {
            typedef Int64 Type;

            const Type ERROR_VALUE = 99;
            const Type MAX         = bsl::numeric_limits<Type>::max();
            const Type MIN         = bsl::numeric_limits<Type>::min();

            static const struct {
                int         d_line;    // line number
                const char *d_input_p; // input on the stream
                Type        d_exp;     // exp unsigned value
                bool        d_isValid; // isValid flag
            } DATA[] = {
   //line          input                    exp     isValid
   //----          -----                    ---     -------
    {  L_,           "0",                     0,      true    },
    {  L_,           "1",                     1,      true    },
    {  L_,          "95",                    95,      true    },
    {  L_,         "127",                   127,      true    },
    {  L_,         "128",                   128,      true    },
    {  L_,         "200",                   200,      true    },
    {  L_,         "255",                   255,      true    },
    {  L_,         "256",                   256,      true    },
    {  L_,       "32766",                 32766,      true    },
    {  L_,       "32767",                 32767,      true    },
    {  L_,       "65534",                 65534,      true    },
    {  L_,       "65535",                 65535,      true    },
    {  L_,     "8388607",               8388607,      true    },
    {  L_,     "8388608",               8388608,      true    },
    {  L_,  "2147483646",            2147483646,      true    },
    {  L_,  "2147483647",            2147483647,      true    },
    {  L_,  "4294967294",            4294967294LL,    true    },
    {  L_,  "4294967295",            4294967295LL,    true    },

    {  L_,          "-1",                    -1,      true    },
    {  L_,        "-128",                  -128,      true    },
    {  L_,        "-129",                  -129,      true    },
    {  L_,        "-255",                  -255,      true    },
    {  L_,        "-256",                  -256,      true    },
    {  L_,      "-32767",                -32767,      true    },
    {  L_,      "-32768",                -32768,      true    },
    {  L_,      "-65535",                -65535,      true    },
    {  L_,      "-65536",                -65536,      true    },
    {  L_, "-2147483647",           -2147483647,      true    },
    {  L_, "-2147483648",           -2147483648LL,    true    },
    {  L_, "-4294967294",           -4294967294LL,    true    },
    {  L_, "-4294967295",           -4294967295LL,    true    },

    {  L_, "-9223372036854775808",          MIN,      true    },
    {  L_, "-9223372036854775807",      MIN + 1,      true    },
    {  L_,  "9223372036854775806",      MAX - 1,      true    },
    {  L_,  "9223372036854775807",          MAX,      true    },

    {  L_,           "0.0",                   0,      true    },
    {  L_,           "0.",                    0,      true    },
    {  L_,           "0.0000000000000",       0,      true    },
    {  L_,           "1.0",                   1,      true    },
    {  L_,           "1.",                    1,      true    },
    {  L_,           "1.00000000000",         1,      true    },
    {  L_,         "95.0",                   95,      true    },
    {  L_,         "95.0000000000",          95,      true    },
    {  L_,        "127.00",                 127,      true    },
    {  L_,        "128.00",                 128,      true    },
    {  L_,        "200.00",                 200,      true    },
    {  L_,        "255.00",                 255,      true    },
    {  L_,        "256.00",                 256,      true    },
    {  L_,  "9223372036854775806.0",        9223372036854775806LL, true },
    {  L_,  "9223372036854775806.00000000", 9223372036854775806LL, true },
    {  L_,  "9223372036854775807.0",        9223372036854775807LL, true },
    {  L_,  "9223372036854775807.00000000", 9223372036854775807LL, true },

    {  L_,         "-1.00",                  -1,      true    },
    {  L_,       "-127.00",                -127,      true    },
    {  L_,       "-127.0000000",           -127,      true    },
    {  L_,       "-128.00",                -128,      true    },
    {  L_,       "-129.00",                -129,      true    },
    {  L_,       "-255.00",                -255,      true    },
    {  L_,       "-256.00",                -256,      true    },
    {  L_, "-9223372036854775807.0",       -9223372036854775807LL, true },
    {  L_, "-9223372036854775807.00000000",-9223372036854775807LL, true },
    {  L_, "-9223372036854775808.0",
               static_cast<long long>(-9223372036854775807LL - 1), true },
    {  L_, "-9223372036854775808.00000000",
               static_cast<long long>(-9223372036854775807LL - 1), true },

    {  L_,          "1e0",                    1,      true    },
    {  L_,          "1E0",                    1,      true    },
    {  L_,          "1e+0",                   1,      true    },
    {  L_,          "1E+0",                   1,      true    },
    {  L_,          "1e-0",                   1,      true    },
    {  L_,          "1E-0",                   1,      true    },

    {  L_,          "1e1",                   10,      true    },
    {  L_,          "2E1",                   20,      true    },
    {  L_,          "3e+1",                  30,      true    },
    {  L_,          "4E+1",                  40,      true    },

    {  L_,          "2e2",                  200,      true    },
    {  L_,          "3E2",                  300,      true    },
    {  L_,          "4e+2",                 400,      true    },
    {  L_,          "5E+2",                 500,      true    },

    {  L_,          "0.1e1",                  1,      true    },
    {  L_,          "0.2e2",                 20,      true    },
    {  L_,          "0.30e2",                30,      true    },
    {  L_,          "0.400e3",              400,      true    },

{ L_,                  "0.9223372036854775807e19", 9223372036854775807LL,true},
{ L_,                  "9.22337203685477580700e18",9223372036854775807LL,true},
{ L_,                 "92.2337203685477580700e17", 9223372036854775807LL,true},
{ L_,                "922.337203685477580700e16",  9223372036854775807LL,true},
{ L_,               "9223.37203685477580700e15",   9223372036854775807LL,true},
{ L_,              "92233.7203685477580700e14",    9223372036854775807LL,true},
{ L_,             "922337.203685477580700e13",     9223372036854775807LL,true},
{ L_,            "9223372.03685477580700e12",      9223372036854775807LL,true},
{ L_,           "92233720.3685477580700e11",       9223372036854775807LL,true},
{ L_,          "922337203.685477580700e10",        9223372036854775807LL,true},
{ L_,         "9223372036.85477580700e9",          9223372036854775807LL,true},
{ L_,        "92233720368.5477580700e8",           9223372036854775807LL,true},
{ L_,       "922337203685.477580700e7",            9223372036854775807LL,true},
{ L_,      "9223372036854.77580700e6",             9223372036854775807LL,true},
{ L_,     "92233720368547.7580700e5",              9223372036854775807LL,true},
{ L_,    "922337203685477.580700e4",               9223372036854775807LL,true},
{ L_,   "9223372036854775.80700e3",                9223372036854775807LL,true},
{ L_,  "92233720368547758.0700e2",                 9223372036854775807LL,true},
{ L_, "922337203685477580.700e1",                  9223372036854775807LL,true},
{ L_,"9223372036854775807.00e0",                   9223372036854775807LL,true},

{L_,                 "-0.9223372036854775808e19",
                      static_cast<long long>(-9223372036854775807LL - 1),true},
{L_,                 "-9.22337203685477580800e18",
                      static_cast<long long>(-9223372036854775807LL - 1),true},
{L_,                "-92.2337203685477580800e17",
                      static_cast<long long>(-9223372036854775807LL - 1),true},
{L_,               "-922.337203685477580800e16",
                      static_cast<long long>(-9223372036854775807LL - 1),true},
{L_,              "-9223.37203685477580800e15",
                      static_cast<long long>(-9223372036854775807LL - 1),true},
{L_,             "-92233.7203685477580800e14",
                      static_cast<long long>(-9223372036854775807LL - 1),true},
{L_,            "-922337.203685477580800e13",
                      static_cast<long long>(-9223372036854775807LL - 1),true},
{L_,           "-9223372.03685477580800e12",
                      static_cast<long long>(-9223372036854775807LL - 1),true},
{L_,          "-92233720.3685477580800e11",
                      static_cast<long long>(-9223372036854775807LL - 1),true},
{L_,         "-922337203.685477580800e10",
                      static_cast<long long>(-9223372036854775807LL - 1),true},
{L_,        "-9223372036.85477580800e9",
                      static_cast<long long>(-9223372036854775807LL - 1),true},
{L_,       "-92233720368.5477580800e8",
                      static_cast<long long>(-9223372036854775807LL - 1),true},
{L_,      "-922337203685.477580800e7",
                      static_cast<long long>(-9223372036854775807LL - 1),true},
{L_,     "-9223372036854.77580800e6",
                      static_cast<long long>(-9223372036854775807LL - 1),true},
{L_,    "-92233720368547.7580800e5",
                      static_cast<long long>(-9223372036854775807LL - 1),true},
{L_,   "-922337203685477.580800e4",
                      static_cast<long long>(-9223372036854775807LL - 1),true},
{L_,  "-9223372036854775.80800e3",
                      static_cast<long long>(-9223372036854775807LL - 1),true},
{L_, "-92233720368547758.0800e2",
                      static_cast<long long>(-9223372036854775807LL - 1),true},
{L_,"-922337203685477580.800e1",
                      static_cast<long long>(-9223372036854775807LL - 1),true},
{L_, "-9223372036854775808.00e0",
                      static_cast<long long>(-9223372036854775807LL - 1),true},

 {  L_,  "922337203685477580700000e-5",        9223372036854775807LL, true },
 {  L_,  "922337203685477580700000.00000e-5",  9223372036854775807LL, true },
 {  L_,   "92233720368547758070000e-4",        9223372036854775807LL, true },
 {  L_,    "9223372036854775807000e-3",        9223372036854775807LL, true },
 {  L_,     "922337203685477580700e-2",        9223372036854775807LL, true },
 {  L_,      "92233720368547758070e-1",        9223372036854775807LL, true },
 {  L_,       "9223372036854775807e-0",        9223372036854775807LL, true },

 {  L_,  "-922337203685477580800000e-5",
                    static_cast<long long>(-9223372036854775807LL - 1), true },
 {  L_,  "-922337203685477580800000.00000e-5",
                    static_cast<long long>(-9223372036854775807LL - 1), true },
 {  L_,   "-92233720368547758080000e-4",
                    static_cast<long long>(-9223372036854775807LL - 1), true },
 {  L_,    "-9223372036854775808000e-3",
                    static_cast<long long>(-9223372036854775807LL - 1), true },
 {  L_,     "-922337203685477580800e-2",
                    static_cast<long long>(-9223372036854775807LL - 1), true },
 {  L_,      "-92233720368547758080e-1",
                    static_cast<long long>(-9223372036854775807LL - 1), true },
 {  L_,       "-9223372036854775808e-0",
                    static_cast<long long>(-9223372036854775807LL - 1), true },

    {  L_,          "1.0e0",                  1,      true    },
    {  L_,          "2.000e0",                2,      true    },
    {  L_,          "3.0e1",                 30,      true    },
    {  L_,          "4.5e1",                 45,      true    },
    {  L_,          "6.00e1",                60,      true    },
    {  L_,          "7.00e2",               700,      true    },
    {  L_,          "8.0000e2",             800,      true    },
    {  L_,          "9.12e2",               912,      true    },
    {  L_,          "1.1200e2",             112,      true    },

    {  L_,           ".5",          ERROR_VALUE,      false   },
    {  L_,           ".123",        ERROR_VALUE,      false   },

    {  L_,          "1.1",          ERROR_VALUE,      false   },
    {  L_,          "1.5",          ERROR_VALUE,      false   },
    {  L_,          "1.9",          ERROR_VALUE,      false   },

    {  L_,        "100.123",        ERROR_VALUE,      false   },
    {  L_,         "99.5",          ERROR_VALUE,      false   },
    {  L_,          "0.86",         ERROR_VALUE,      false   },

    {  L_,        "255.01",         ERROR_VALUE,      false   },
    {  L_,        "255.99",         ERROR_VALUE,      false   },

    {  L_,          "1.234e+1",     ERROR_VALUE,      false   },
    {  L_,          "1.987E+1",     ERROR_VALUE,      false   },

    {  L_,          "1e1e1",        ERROR_VALUE,      false   },
    {  L_,          "1e1e-1",       ERROR_VALUE,      false   },

    {  L_,         "12.34e-1",      ERROR_VALUE,      false   },
    {  L_,         "29.87E-1",      ERROR_VALUE,      false   },

    {  L_,         "12345e-1",      ERROR_VALUE,      false   },
    {  L_,         "12345e-2",      ERROR_VALUE,      false   },
    {  L_,         "12345e-4",      ERROR_VALUE,      false   },
    {  L_,         "12345e-5",      ERROR_VALUE,      false   },

    {  L_,         "-12345e-1",     ERROR_VALUE,      false   },
    {  L_,         "-12345e-2",     ERROR_VALUE,      false   },
    {  L_,         "-12345e-4",     ERROR_VALUE,      false   },
    {  L_,         "-12345e-5",     ERROR_VALUE,      false   },
    {  L_,         "-12345e-6",     ERROR_VALUE,      false   },

    {  L_,         "123.45e-1",     ERROR_VALUE,      false   },
    {  L_,         "1234.5e-2",     ERROR_VALUE,      false   },

    {  L_,          "1.0000000001", ERROR_VALUE,      false   },
    {  L_,          "1.00001e0",    ERROR_VALUE,      false   },

    {  L_,   "9223372036854775808", ERROR_VALUE,      false   },
    {  L_,  "-9223372036854775809", ERROR_VALUE,      false   },

    {  L_, "9223372036854775807.01", ERROR_VALUE,     false   },
    {  L_, "9223372036854775807.99", ERROR_VALUE,     false   },
    {  L_, "9223372036854775808.01", ERROR_VALUE,     false   },
    {  L_, "9223372036854775808.99", ERROR_VALUE,     false   },

    {  L_,"-9223372036854775808.01", ERROR_VALUE,     false   },
    {  L_,"-9223372036854775808.99", ERROR_VALUE,     false   },
    {  L_,"-9223372036854775809.01", ERROR_VALUE,     false   },
    {  L_,"-9223372036854775809.99", ERROR_VALUE,     false   },

    {  L_,  "18446744073709551616", ERROR_VALUE,     false   },

    {  L_,        "-",              ERROR_VALUE,      false   },
    {  L_,        ".5",             ERROR_VALUE,      false   },
    {  L_,        "-.5",            ERROR_VALUE,      false   },
    {  L_,        "e",              ERROR_VALUE,      false   },
    {  L_,        "e1",             ERROR_VALUE,      false   },
    {  L_,        "E",              ERROR_VALUE,      false   },
    {  L_,        "E1",             ERROR_VALUE,      false   },
    {  L_,        "e+",             ERROR_VALUE,      false   },
    {  L_,        "e+1",            ERROR_VALUE,      false   },
    {  L_,        "E+",             ERROR_VALUE,      false   },
    {  L_,        "E+1",            ERROR_VALUE,      false   },
    {  L_,        "e-",             ERROR_VALUE,      false   },
    {  L_,        "e-1",            ERROR_VALUE,      false   },
    {  L_,        "E-",             ERROR_VALUE,      false   },
    {  L_,        "E-1",            ERROR_VALUE,      false   },

    {  L_,        "Z34.56e1",       ERROR_VALUE,      false   },
    {  L_,        "3Z4.56e1",       ERROR_VALUE,      false   },
    {  L_,        "34Z.56e1",       ERROR_VALUE,      false   },
    {  L_,         "34+56e1",       ERROR_VALUE,      false   },
    {  L_,         "34.Z6e1",       ERROR_VALUE,      false   },
    {  L_,         "34.5Ze1",       ERROR_VALUE,      false   },
    {  L_,         "34.56Z1",       ERROR_VALUE,      false   },
    {  L_,         "34.56eZ",       ERROR_VALUE,      false   },
    {  L_,         "34.56e+Z",      ERROR_VALUE,      false   },
    {  L_,         "34.56e-Z",      ERROR_VALUE,      false   },

    {  L_,         "0x12",          ERROR_VALUE,      false   },
    {  L_,         "0x256",         ERROR_VALUE,      false   },
    {  L_,         "JUNK",          ERROR_VALUE,      false   },
    {  L_,         "DEADBEEF",      ERROR_VALUE,      false   },
            };
            const int NUM_DATA = sizeof(DATA) / sizeof(*DATA);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int    LINE     = DATA[i].d_line;
                const string INPUT    = DATA[i].d_input_p;
                const Type   EXP      = DATA[i].d_exp;
                const bool   IS_VALID = DATA[i].d_isValid;
                      Type   value    = ERROR_VALUE;

                const bsl::string_view isb(INPUT);

                bslma::TestAllocator         da("default", veryVeryVerbose);
                bslma::DefaultAllocatorGuard dag(&da);

                const int rc = Util::getValue(&value, isb);
                if (IS_VALID) {
                    LOOP2_ASSERT(LINE, rc, 0 == rc);
                }
                else {
                    LOOP2_ASSERT(LINE, rc, rc);
                }
                LOOP3_ASSERT(LINE, EXP, value, EXP == value);

                ASSERTV(LINE, da.numBlocksTotal(), 0 == da.numBlocksTotal());
            }
        }
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // TESTING 'getValue' for unsigned int values
        //
        // Concerns:
        //: 1 Values in the valid range, including the maximum and minimum
        //:   values for this type, are parsed correctly.
        //:
        //: 2 The passed in variable is unmodified if the data is not valid.
        //:
        //: 3 The return code is 0 on success and non-zero on failure.
        //
        // Plan:
        //: 1 Using the table-driven technique, specify a set of distinct
        //:   rows of string value, expected parsed value, and return code.
        //:
        //: 2 For each row in the table of P-1:
        //:
        //:   1 Provide the string value and a variable  to be parsed into to
        //:     the 'getValue' function.  The variable is assigned a sentinel
        //:     value before invoking the function.
        //:
        //:   2 If the parsing should succeed then verify that the variable
        //:     value matches the expected value.  Otherwise confirm that the
        //:     variable value is unmodified.
        //:
        //:   3 Confirm that the return code is 0 on success and non-zero
        //:     otherwise.
        //
        // Testing:
        //   static int getValue(unsigned int        *v, bsl::string_view s);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING 'getValue' for unsigned int"
                          << "\n===================================" << endl;

        {
            typedef unsigned int Type;

            const Type ERROR_VALUE = 99;
            const Type MAX         = bsl::numeric_limits<Type>::max();
            const Type MIN         = bsl::numeric_limits<Type>::min();

            static const struct {
                int         d_line;    // line number
                const char *d_input_p; // input on the stream
                Type        d_exp;     // exp unsigned value
                bool        d_isValid; // isValid flag
            } DATA[] = {
   //line          input                    exp     isValid
   //----          -----                    ---     -------
    {  L_,           "0",                     0,      true    },
    {  L_,           "1",                     1,      true    },
    {  L_,          "95",                    95,      true    },
    {  L_,         "127",                   127,      true    },
    {  L_,         "128",                   128,      true    },
    {  L_,         "200",                   200,      true    },
    {  L_,         "255",                   255,      true    },
    {  L_,         "256",                   256,      true    },
    {  L_,       "32766",                 32766,      true    },
    {  L_,       "32767",                 32767,      true    },
    {  L_,       "65534",                 65534,      true    },
    {  L_,       "65535",                 65535,      true    },
    {  L_,     "8388607",               8388607,      true    },
    {  L_,     "8388608",               8388608,      true    },
    {  L_,  "2147483646",            2147483646,      true    },
    {  L_,  "2147483647",            2147483647,      true    },

    {  L_,           "0",                   MIN,      true    },
    {  L_,           "1",               MIN + 1,      true    },
    {  L_,  "4294967294",               MAX - 1,      true    },
    {  L_,  "4294967295",                   MAX,      true    },

    {  L_,           "0.0",                   0,      true    },
    {  L_,           "0.0000000000000",       0,      true    },
    {  L_,           "1.0",                   1,      true    },
    {  L_,           "1.00000000000",         1,      true    },
    {  L_,         "95.0",                   95,      true    },
    {  L_,         "95.0000000000",          95,      true    },
    {  L_,        "127.00",                 127,      true    },
    {  L_,        "128.00",                 128,      true    },
    {  L_,        "200.00",                 200,      true    },
    {  L_,        "255.00",                 255,      true    },
    {  L_,        "256.00",                 256,      true    },
    {  L_, "2147483646.0",           2147483646,      true    },
    {  L_, "2147483646.000000000",   2147483646,      true    },
    {  L_, "2147483647.0",           2147483647,      true    },
    {  L_, "2147483647.000000000",   2147483647,      true    },
    {  L_, "4294967294.0",           4294967294U,     true    },
    {  L_, "4294967294.000000000",   4294967294U,     true    },
    {  L_, "4294967295.0",           4294967295U,     true    },
    {  L_, "4294967295.000000000",   4294967295U,     true    },

    {  L_,          "1e0",                    1,      true    },
    {  L_,          "1E0",                    1,      true    },
    {  L_,          "1e+0",                   1,      true    },
    {  L_,          "1E+0",                   1,      true    },
    {  L_,          "1e-0",                   1,      true    },
    {  L_,          "1E-0",                   1,      true    },

    {  L_,          "1e1",                   10,      true    },
    {  L_,          "2E1",                   20,      true    },
    {  L_,          "3e+1",                  30,      true    },
    {  L_,          "4E+1",                  40,      true    },

    {  L_,          "2e2",                  200,      true    },
    {  L_,          "3E2",                  300,      true    },
    {  L_,          "4e+2",                 400,      true    },
    {  L_,          "5E+2",                 500,      true    },

    {  L_,          "0.1e1",                  1,      true    },
    {  L_,          "0.2e2",                 20,      true    },
    {  L_,          "0.30e2",                30,      true    },
    {  L_,          "0.400e3",              400,      true    },

    {  L_,          "0.429496729500e10", 4294967295U, true    },
    {  L_,          "4.29496729500e9",   4294967295U, true    },
    {  L_,         "42.9496729500e8",    4294967295U, true    },
    {  L_,        "429.496729500e7",     4294967295U, true    },
    {  L_,       "4294.96729500e6",      4294967295U, true    },
    {  L_,      "42949.6729500e5",       4294967295U, true    },
    {  L_,     "429496.729500e4",        4294967295U, true    },
    {  L_,    "4294967.29500e3",         4294967295U, true    },
    {  L_,   "42949672.9500e2",          4294967295U, true    },
    {  L_,  "429496729.500e1",           4294967295U, true    },
    {  L_, "4294967295.00e0",            4294967295U, true    },

    {  L_,  "429496729500000e-5",        4294967295U, true    },
    {  L_,  "429496729500000.00000e-5",  4294967295U, true    },
    {  L_,   "42949672950000e-4",        4294967295U, true    },
    {  L_,    "4294967295000e-3",        4294967295U, true    },
    {  L_,     "429496729500e-2",        4294967295U, true    },
    {  L_,      "42949672950e-1",        4294967295U, true    },
    {  L_,       "4294967295e-0",        4294967295U, true    },

    {  L_,          "1.0e0",                  1,      true    },
    {  L_,          "2.000e0",                2,      true    },
    {  L_,          "3.0e1",                 30,      true    },
    {  L_,          "4.5e1",                 45,      true    },
    {  L_,          "6.00e1",                60,      true    },
    {  L_,          "7.00e2",               700,      true    },
    {  L_,          "8.0000e2",             800,      true    },
    {  L_,          "9.12e2",               912,      true    },
    {  L_,          "1.1200e2",             112,      true    },

    {  L_,           ".5",          ERROR_VALUE,      false   },
    {  L_,           ".123",        ERROR_VALUE,      false   },

    {  L_,          "1.1",          ERROR_VALUE,      false   },
    {  L_,          "1.5",          ERROR_VALUE,      false   },
    {  L_,          "1.9",          ERROR_VALUE,      false   },

    {  L_,        "100.123",        ERROR_VALUE,      false   },
    {  L_,         "99.5",          ERROR_VALUE,      false   },
    {  L_,          "0.86",         ERROR_VALUE,      false   },

    {  L_,        "255.01",         ERROR_VALUE,      false   },
    {  L_,        "255.99",         ERROR_VALUE,      false   },

    {  L_,          "1.234e+1",     ERROR_VALUE,      false   },
    {  L_,          "1.987E+1",     ERROR_VALUE,      false   },

    {  L_,          "1e1e1",        ERROR_VALUE,      false   },
    {  L_,          "1e1e-1",       ERROR_VALUE,      false   },

    {  L_,         "12.34e-1",      ERROR_VALUE,      false   },
    {  L_,         "29.87E-1",      ERROR_VALUE,      false   },

    {  L_,         "12345e-1",      ERROR_VALUE,      false   },
    {  L_,         "12345e-2",      ERROR_VALUE,      false   },
    {  L_,         "12345e-4",      ERROR_VALUE,      false   },
    {  L_,         "12345e-5",      ERROR_VALUE,      false   },

    {  L_,         "123.45e-1",     ERROR_VALUE,      false   },
    {  L_,         "1234.5e-2",     ERROR_VALUE,      false   },

    {  L_,          "1.0000000001", ERROR_VALUE,      false   },
    {  L_,          "1.00001e0",    ERROR_VALUE,      false   },

    {  L_,      "4294967296",       ERROR_VALUE,      false   },
    {  L_,      "4294967295.01",    ERROR_VALUE,      false   },
    {  L_,      "4294967295.99",    ERROR_VALUE,      false   },
    {  L_,      "4294967296.01",    ERROR_VALUE,      false   },
    {  L_,      "4294967296.99",    ERROR_VALUE,      false   },

    {  L_,  "18446744073709551616", ERROR_VALUE,     false   },

    {  L_,        "-",              ERROR_VALUE,      false   },
    {  L_,        ".5",             ERROR_VALUE,      false   },
    {  L_,        "-.5",            ERROR_VALUE,      false   },
    {  L_,        "e",              ERROR_VALUE,      false   },
    {  L_,        "e1",             ERROR_VALUE,      false   },
    {  L_,        "E",              ERROR_VALUE,      false   },
    {  L_,        "E1",             ERROR_VALUE,      false   },
    {  L_,        "e+",             ERROR_VALUE,      false   },
    {  L_,        "e+1",            ERROR_VALUE,      false   },
    {  L_,        "E+",             ERROR_VALUE,      false   },
    {  L_,        "E+1",            ERROR_VALUE,      false   },
    {  L_,        "e-",             ERROR_VALUE,      false   },
    {  L_,        "e-1",            ERROR_VALUE,      false   },
    {  L_,        "E-",             ERROR_VALUE,      false   },
    {  L_,        "E-1",            ERROR_VALUE,      false   },

    {  L_,        "Z34.56e1",       ERROR_VALUE,      false   },
    {  L_,        "3Z4.56e1",       ERROR_VALUE,      false   },
    {  L_,        "34Z.56e1",       ERROR_VALUE,      false   },
    {  L_,         "34+56e1",       ERROR_VALUE,      false   },
    {  L_,         "34.Z6e1",       ERROR_VALUE,      false   },
    {  L_,         "34.5Ze1",       ERROR_VALUE,      false   },
    {  L_,         "34.56Z1",       ERROR_VALUE,      false   },
    {  L_,         "34.56eZ",       ERROR_VALUE,      false   },
    {  L_,         "34.56e+Z",      ERROR_VALUE,      false   },
    {  L_,         "34.56e-Z",      ERROR_VALUE,      false   },

    {  L_,         "0x12",          ERROR_VALUE,      false   },
    {  L_,         "0x256",         ERROR_VALUE,      false   },
    {  L_,         "JUNK",          ERROR_VALUE,      false   },
    {  L_,         "DEADBEEF",      ERROR_VALUE,      false   },
            };
            const int NUM_DATA = sizeof(DATA) / sizeof(*DATA);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int    LINE     = DATA[i].d_line;
                const string INPUT    = DATA[i].d_input_p;
                const Type   EXP      = DATA[i].d_exp;
                const bool   IS_VALID = DATA[i].d_isValid;
                      Type   value    = ERROR_VALUE;

                const bsl::string_view isb(INPUT);

                bslma::TestAllocator         da("default", veryVeryVerbose);
                bslma::DefaultAllocatorGuard dag(&da);

                const int rc = Util::getValue(&value, isb);
                if (IS_VALID) {
                    LOOP2_ASSERT(LINE, rc, 0 == rc);
                }
                else {
                    LOOP2_ASSERT(LINE, rc, rc);
                }
                LOOP3_ASSERT(LINE, EXP, value, EXP == value);

                ASSERTV(LINE, da.numBlocksTotal(), 0 == da.numBlocksTotal());
            }
        }
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TESTING 'getValue' for int values
        //
        // Concerns:
        //: 1 Values in the valid range, including the maximum and minimum
        //:   values for this type, are parsed correctly.
        //:
        //: 2 The passed in variable is unmodified if the data is not valid.
        //:
        //: 3 The return code is 0 on success and non-zero on failure.
        //
        // Plan:
        //: 1 Using the table-driven technique, specify a set of distinct
        //:   rows of string value, expected parsed value, and return code.
        //:
        //: 2 For each row in the table of P-1:
        //:
        //:   1 Provide the string value and a variable  to be parsed into to
        //:     the 'getValue' function.  The variable is assigned a sentinel
        //:     value before invoking the function.
        //:
        //:   2 If the parsing should succeed then verify that the variable
        //:     value matches the expected value.  Otherwise confirm that the
        //:     variable value is unmodified.
        //:
        //:   3 Confirm that the return code is 0 on success and non-zero
        //:     otherwise.
        //
        // Testing:
        //   static int getValue(int                 *v, bsl::string_view s);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING 'getValue' for int"
                          << "\n==========================" << endl;
        {
            typedef int Type;

            const Type ERROR_VALUE = 99;
            const Type MAX         = bsl::numeric_limits<Type>::max();
            const Type MIN         = bsl::numeric_limits<Type>::min();

            static const struct {
                int         d_line;    // line number
                const char *d_input_p; // input on the stream
                Type        d_exp;     // exp unsigned value
                bool        d_isValid; // isValid flag
            } DATA[] = {
#define TLN(inp, exp, isvalid) { L_, inp, static_cast<Type>(exp), isvalid }
                //          input                expected       isValid
                //  ---------------------    ----------------   -------
                TLN(           "0",                        0,     true ),
                TLN(           "1",                        1,     true ),
                TLN(          "95",                       95,     true ),
                TLN(         "127",                      127,     true ),
                TLN(         "128",                      128,     true ),
                TLN(         "200",                      200,     true ),
                TLN(         "255",                      255,     true ),
                TLN(         "256",                      256,     true ),
                TLN(       "32766",                    32766,     true ),
                TLN(       "32767",                    32767,     true ),
                TLN(       "65534",                    65534,     true ),
                TLN(       "65535",                    65535,     true ),
                TLN(     "8388607",                  8388607,     true ),
                TLN(     "8388608",                  8388608,     true ),

                TLN(          "-1",                       -1,     true ),
                TLN(        "-128",                     -128,     true ),
                TLN(        "-129",                     -129,     true ),
                TLN(        "-255",                     -255,     true ),
                TLN(        "-256",                     -256,     true ),
                TLN(      "-32767",                   -32767,     true ),
                TLN(      "-32768",                   -32768,     true ),
                TLN(      "-65535",                   -65535,     true ),
                TLN(      "-65536",                   -65536,     true ),

                TLN( "-2147483648",                      MIN,     true ),
                TLN( "-2147483647",                  MIN + 1,     true ),
                TLN(  "2147483646",                  MAX - 1,     true ),
                TLN(  "2147483647",                      MAX,     true ),

                TLN(           "0.0",                      0,     true ),
                TLN(           "0.0000000000000",          0,     true ),
                TLN(           "1.0",                      1,     true ),
                TLN(           "1.00000000000",            1,     true ),
                TLN(         "95.0",                      95,     true ),
                TLN(         "95.0000000000",             95,     true ),
                TLN(        "127.00",                    127,     true ),
                TLN(        "128.00",                    128,     true ),
                TLN(        "200.00",                    200,     true ),
                TLN(        "255.00",                    255,     true ),
                TLN(        "256.00",                    256,     true ),
                TLN( "2147483646.0",              2147483646,     true ),
                TLN( "2147483646.000000000",      2147483646,     true ),
                TLN( "2147483647.0",              2147483647,     true ),
                TLN( "2147483647.000000000",      2147483647,     true ),

                TLN(         "-1.00",                     -1,     true ),
                TLN(       "-127.00",                   -127,     true ),
                TLN(       "-127.0000000",              -127,     true ),
                TLN(       "-128.00",                   -128,     true ),
                TLN(       "-129.00",                   -129,     true ),
                TLN(       "-255.00",                   -255,     true ),
                TLN(       "-256.00",                   -256,     true ),
                TLN("-2147483647.0",             -2147483647,     true ),
                TLN("-2147483647.000000000",     -2147483647,     true ),
                TLN("-2147483648.0",         -2147483647 - 1,     true ),
                TLN("-2147483648.000000000", -2147483647 - 1,     true ),

                TLN(          "1e0",                       1,     true ),
                TLN(          "1E0",                       1,     true ),
                TLN(          "1e+0",                      1,     true ),
                TLN(          "1E+0",                      1,     true ),
                TLN(          "1e-0",                      1,     true ),
                TLN(          "1E-0",                      1,     true ),

                TLN(          "1e1",                      10,     true ),
                TLN(          "2E1",                      20,     true ),
                TLN(          "3e+1",                     30,     true ),
                TLN(          "4E+1",                     40,     true ),

                TLN(          "2e2",                     200,     true ),
                TLN(          "3E2",                     300,     true ),
                TLN(          "4e+2",                    400,     true ),
                TLN(          "5E+2",                    500,     true ),

                TLN(          "0.1e1",                     1,     true ),
                TLN(          "0.2e2",                    20,     true ),
                TLN(          "0.30e2",                   30,     true ),
                TLN(          "0.400e3",                 400,     true ),

                TLN(          "0.214748364700e10",    2147483647, true ),
                TLN(          "2.14748364700e9",      2147483647, true ),
                TLN(         "21.4748364700e8",       2147483647, true ),
                TLN(        "214.748364700e7",        2147483647, true ),
                TLN(       "2147.48364700e6",         2147483647, true ),
                TLN(      "21474.8364700e5",          2147483647, true ),
                TLN(     "214748.364700e4",           2147483647, true ),
                TLN(    "2147483.64700e3",            2147483647, true ),
                TLN(   "21474836.4700e2",             2147483647, true ),
                TLN(  "214748364.700e1",              2147483647, true ),
                TLN( "2147483647.00e0",               2147483647, true ),

                TLN(         "-0.214748364800e10", -2147483647-1, true ),
                TLN(         "-2.14748364800e9",   -2147483647-1, true ),
                TLN(        "-21.4748364800e8",    -2147483647-1, true ),
                TLN(       "-214.748364800e7",     -2147483647-1, true ),
                TLN(      "-2147.48364800e6",      -2147483647-1, true ),
                TLN(     "-21474.8364800e5",       -2147483647-1, true ),
                TLN(    "-214748.364800e4",        -2147483647-1, true ),
                TLN(   "-2147483.64800e3",         -2147483647-1, true ),
                TLN(  "-21474836.4800e2",          -2147483647-1, true ),
                TLN( "-214748364.800e1",           -2147483647-1, true ),
                TLN("-2147483648.00e0",            -2147483647-1, true ),

                TLN(  "214748364700000e-5",           2147483647, true ),
                TLN(  "214748364700000.00000e-5",     2147483647, true ),
                TLN(   "21474836470000e-4",           2147483647, true ),
                TLN(    "2147483647000e-3",           2147483647, true ),
                TLN(     "214748364700e-2",           2147483647, true ),
                TLN(      "21474836470e-1",           2147483647, true ),
                TLN(       "2147483647e-0",           2147483647, true ),

                TLN( "-214748364800000e-5",        -2147483647-1, true ),
                TLN( "-214748364800000.00000e-5",  -2147483647-1, true ),
                TLN(  "-21474836480000e-4",        -2147483647-1, true ),
                TLN(   "-2147483648000e-3",        -2147483647-1, true ),
                TLN(    "-214748364800e-2",        -2147483647-1, true ),
                TLN(     "-21474836480e-1",        -2147483647-1, true ),
                TLN(      "-2147483648e-0",        -2147483647-1, true ),

                TLN(          "1.0e0",                     1,     true ),
                TLN(          "2.000e0",                   2,     true ),
                TLN(          "3.0e1",                    30,     true ),
                TLN(          "4.5e1",                    45,     true ),
                TLN(          "6.00e1",                   60,     true ),
                TLN(          "7.00e2",                  700,     true ),
                TLN(          "8.0000e2",                800,     true ),
                TLN(          "9.12e2",                  912,     true ),
                TLN(          "1.1200e2",                112,     true ),

                TLN(           ".5",             ERROR_VALUE,     false),
                TLN(           ".123",           ERROR_VALUE,     false),

                TLN(          "1.1",             ERROR_VALUE,     false),
                TLN(          "1.5",             ERROR_VALUE,     false),
                TLN(          "1.9",             ERROR_VALUE,     false),

                TLN(        "100.123",           ERROR_VALUE,     false),
                TLN(         "99.5",             ERROR_VALUE,     false),
                TLN(          "0.86",            ERROR_VALUE,     false),

                TLN(        "255.01",            ERROR_VALUE,     false),
                TLN(        "255.99",            ERROR_VALUE,     false),

                TLN(          "1.234e+1",        ERROR_VALUE,     false),
                TLN(          "1.987E+1",        ERROR_VALUE,     false),

                TLN(          "1e1e1",           ERROR_VALUE,     false),
                TLN(          "1e1e-1",          ERROR_VALUE,     false),

                TLN(         "12.34e-1",         ERROR_VALUE,     false),
                TLN(         "29.87E-1",         ERROR_VALUE,     false),

                TLN(         "12345e-1",         ERROR_VALUE,     false),
                TLN(         "12345e-2",         ERROR_VALUE,     false),
                TLN(         "12345e-4",         ERROR_VALUE,     false),
                TLN(         "12345e-5",         ERROR_VALUE,     false),

                TLN(         "-12345e-1",        ERROR_VALUE,     false),
                TLN(         "-12345e-2",        ERROR_VALUE,     false),
                TLN(         "-12345e-4",        ERROR_VALUE,     false),
                TLN(         "-12345e-5",        ERROR_VALUE,     false),
                TLN(         "-12345e-6",        ERROR_VALUE,     false),

                TLN(         "123.45e-1",        ERROR_VALUE,     false),
                TLN(         "1234.5e-2",        ERROR_VALUE,     false),

                TLN(          "1.0000000001",    ERROR_VALUE,     false),
                TLN(          "1.00001e0",       ERROR_VALUE,     false),

                TLN(     "2147483648",           ERROR_VALUE,     false),
                TLN(     "-2147483649",          ERROR_VALUE,     false),

                TLN(      "2147483647.01",       ERROR_VALUE,     false),
                TLN(      "2147483647.99",       ERROR_VALUE,     false),
                TLN(      "2147483648.01",       ERROR_VALUE,     false),
                TLN(      "2147483648.99",       ERROR_VALUE,     false),

                TLN(     "-2147483648.01",       ERROR_VALUE,     false),
                TLN(     "-2147483648.99",       ERROR_VALUE,     false),
                TLN(     "-2147483649.01",       ERROR_VALUE,     false),
                TLN(     "-2147483649.99",       ERROR_VALUE,     false),

                TLN(  "18446744073709551616",    ERROR_VALUE,     false),

                TLN(        "-",                 ERROR_VALUE,     false),
                TLN(        ".5",                ERROR_VALUE,     false),
                TLN(        "-.5",               ERROR_VALUE,     false),
                TLN(        "e",                 ERROR_VALUE,     false),
                TLN(        "e1",                ERROR_VALUE,     false),
                TLN(        "E",                 ERROR_VALUE,     false),
                TLN(        "E1",                ERROR_VALUE,     false),
                TLN(        "e+",                ERROR_VALUE,     false),
                TLN(        "e+1",               ERROR_VALUE,     false),
                TLN(        "E+",                ERROR_VALUE,     false),
                TLN(        "E+1",               ERROR_VALUE,     false),
                TLN(        "e-",                ERROR_VALUE,     false),
                TLN(        "e-1",               ERROR_VALUE,     false),
                TLN(        "E-",                ERROR_VALUE,     false),
                TLN(        "E-1",               ERROR_VALUE,     false),

                TLN(        "Z34.56e1",          ERROR_VALUE,     false),
                TLN(        "3Z4.56e1",          ERROR_VALUE,     false),
                TLN(        "34Z.56e1",          ERROR_VALUE,     false),
                TLN(         "34+56e1",          ERROR_VALUE,     false),
                TLN(         "34.Z6e1",          ERROR_VALUE,     false),
                TLN(         "34.5Ze1",          ERROR_VALUE,     false),
                TLN(         "34.56Z1",          ERROR_VALUE,     false),
                TLN(         "34.56eZ",          ERROR_VALUE,     false),
                TLN(         "34.56e+Z",         ERROR_VALUE,     false),
                TLN(         "34.56e-Z",         ERROR_VALUE,     false),

                TLN(         "0x12",             ERROR_VALUE,     false),
                TLN(         "0x256",            ERROR_VALUE,     false),
                TLN(         "JUNK",             ERROR_VALUE,     false),
                TLN(         "DEADBEEF",         ERROR_VALUE,     false),
#undef TLN
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int    LINE     = DATA[i].d_line;
                const string INPUT    = DATA[i].d_input_p;
                const Type   EXP      = DATA[i].d_exp;
                const bool   IS_VALID = DATA[i].d_isValid;
                      Type   value    = ERROR_VALUE;

                const bsl::string_view isb(INPUT);

                bslma::TestAllocator         da("default", veryVeryVerbose);
                bslma::DefaultAllocatorGuard dag(&da);

                const int rc = Util::getValue(&value, isb);
                if (IS_VALID) {
                    LOOP2_ASSERT(LINE, rc, 0 == rc);
                }
                else {
                    LOOP2_ASSERT(LINE, rc, rc);
                }
                LOOP3_ASSERT(LINE, EXP, value, EXP == value);

                ASSERTV(LINE, da.numBlocksTotal(), 0 == da.numBlocksTotal());
            }
        }
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING 'getValue' for unsigned short values
        //
        // Concerns:
        //: 1 Values in the valid range, including the maximum and minimum
        //:   values for this type, are parsed correctly.
        //:
        //: 2 The passed in variable is unmodified if the data is not valid.
        //:
        //: 3 The return code is 0 on success and non-zero on failure.
        //
        // Plan:
        //: 1 Using the table-driven technique, specify a set of distinct
        //:   rows of string value, expected parsed value, and return code.
        //:
        //: 2 For each row in the table of P-1:
        //:
        //:   1 Provide the string value and a variable  to be parsed into to
        //:     the 'getValue' function.  The variable is assigned a sentinel
        //:     value before invoking the function.
        //:
        //:   2 If the parsing should succeed then verify that the variable
        //:     value matches the expected value.  Otherwise confirm that the
        //:     variable value is unmodified.
        //:
        //:   3 Confirm that the return code is 0 on success and non-zero
        //:     otherwise.
        //
        // Testing:
        //   static int getValue(unsigned short      *v, bsl::string_view s);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING 'getValue' for unsigned short"
                          << "\n=====================================" << endl;

        {
            typedef unsigned short Type;

            const Type ERROR_VALUE = 99;
            const Type MAX         = bsl::numeric_limits<Type>::max();
            const Type MIN         = bsl::numeric_limits<Type>::min();

#ifdef BSLS_PLATFORM_HAS_PRAGMA_GCC_DIAGNOSTIC
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wnarrowing"
#endif

            static const struct {
                int         d_line;    // line number
                const char *d_input_p; // input on the stream
                Type        d_exp;     // exp unsigned value
                bool        d_isValid; // isValid flag
            } DATA[] = {
   //line          input                    exp     isValid
   //----          -----                    ---     -------
    {  L_,           "0",                     0,      true    },
    {  L_,           "1",                     1,      true    },
    {  L_,          "95",                    95,      true    },
    {  L_,         "127",                   127,      true    },
    {  L_,         "128",                   128,      true    },
    {  L_,         "200",                   200,      true    },
    {  L_,         "255",                   255,      true    },
    {  L_,         "256",                   256,      true    },
    {  L_,       "32766",                 32766,      true    },
    {  L_,       "32767",                 32767,      true    },
    {  L_,       "65534",                 65534,      true    },
    {  L_,       "65535",                 65535,      true    },

    {  L_,           "0",                   MIN,      true    },
    {  L_,           "1",               MIN + 1,      true    },
    {  L_,       "65534",               MAX - 1,      true    },
    {  L_,       "65535",                   MAX,      true    },

    {  L_,           "0.0",                   0,      true    },
    {  L_,           "0.0000000000000",       0,      true    },
    {  L_,           "1.0",                   1,      true    },
    {  L_,           "1.00000000000",         1,      true    },
    {  L_,         "95.0",                   95,      true    },
    {  L_,         "95.0000000000",          95,      true    },
    {  L_,        "127.00",                 127,      true    },
    {  L_,        "128.00",                 128,      true    },
    {  L_,        "200.00",                 200,      true    },
    {  L_,        "255.00",                 255,      true    },
    {  L_,        "256.00",                 256,      true    },
    {  L_,      "32766.0",                32766,      true    },
    {  L_,      "32766.00000000",         32766,      true    },
    {  L_,      "32767.0",                32767,      true    },
    {  L_,      "32767.00000000",         32767,      true    },
    {  L_,      "65534.0",                65534,      true    },
    {  L_,      "65534.00000000",         65534,      true    },
    {  L_,      "65535.0",                65535,      true    },
    {  L_,      "65535.00000000",         65535,      true    },

    {  L_,          "1e0",                    1,      true    },
    {  L_,          "1E0",                    1,      true    },
    {  L_,          "1e+0",                   1,      true    },
    {  L_,          "1E+0",                   1,      true    },
    {  L_,          "1e-0",                   1,      true    },
    {  L_,          "1E-0",                   1,      true    },

    {  L_,          "1e1",                   10,      true    },
    {  L_,          "2E1",                   20,      true    },
    {  L_,          "3e+1",                  30,      true    },
    {  L_,          "4E+1",                  40,      true    },

    {  L_,          "2e2",                  200,      true    },
    {  L_,          "3E2",                  300,      true    },
    {  L_,          "4e+2",                 400,      true    },
    {  L_,          "5E+2",                 500,      true    },

    {  L_,          "0.1e1",                  1,      true    },
    {  L_,          "0.2e2",                 20,      true    },
    {  L_,          "0.30e2",                30,      true    },
    {  L_,          "0.400e3",              400,      true    },

    {  L_,          "0.6553500e5",        65535,      true    },
    {  L_,          "6.5535000e4",        65535,      true    },
    {  L_,         "65.5350000e3",        65535,      true    },
    {  L_,        "655.3500000e2",        65535,      true    },
    {  L_,       "6553.5000000e1",        65535,      true    },
    {  L_,      "65535.000000e0",         65535,      true    },

    {  L_,  "6553500000e-5",              65535,      true    },
    {  L_,  "6553500000.00000e-5",        65535,      true    },
    {  L_,   "655350000e-4",              65535,      true    },
    {  L_,    "65535000e-3",              65535,      true    },
    {  L_,     "6553500e-2",              65535,      true    },
    {  L_,      "655350e-1",              65535,      true    },
    {  L_,       "65535e-0",              65535,      true    },

    {  L_,          "1.0e0",                  1,      true    },
    {  L_,          "2.000e0",                2,      true    },
    {  L_,          "3.0e1",                 30,      true    },
    {  L_,          "4.5e1",                 45,      true    },
    {  L_,          "6.00e1",                60,      true    },
    {  L_,          "7.00e2",               700,      true    },
    {  L_,          "8.0000e2",             800,      true    },
    {  L_,          "9.12e2",               912,      true    },
    {  L_,          "1.1200e2",             112,      true    },

    {  L_,           ".5",          ERROR_VALUE,      false   },
    {  L_,           ".123",        ERROR_VALUE,      false   },

    {  L_,          "1.1",          ERROR_VALUE,      false   },
    {  L_,          "1.5",          ERROR_VALUE,      false   },
    {  L_,          "1.9",          ERROR_VALUE,      false   },

    {  L_,        "100.123",        ERROR_VALUE,      false   },
    {  L_,         "99.5",          ERROR_VALUE,      false   },
    {  L_,          "0.86",         ERROR_VALUE,      false   },

    {  L_,        "255.01",         ERROR_VALUE,      false   },
    {  L_,        "255.99",         ERROR_VALUE,      false   },

    {  L_,          "1.234e+1",     ERROR_VALUE,      false   },
    {  L_,          "1.987E+1",     ERROR_VALUE,      false   },

    {  L_,          "1e1e1",        ERROR_VALUE,      false   },
    {  L_,          "1e1e-1",       ERROR_VALUE,      false   },

    {  L_,         "12.34e-1",      ERROR_VALUE,      false   },
    {  L_,         "29.87E-1",      ERROR_VALUE,      false   },

    {  L_,         "12345e-1",      ERROR_VALUE,      false   },
    {  L_,         "12345e-2",      ERROR_VALUE,      false   },
    {  L_,         "12345e-4",      ERROR_VALUE,      false   },
    {  L_,         "12345e-5",      ERROR_VALUE,      false   },

    {  L_,         "123.45e-1",     ERROR_VALUE,      false   },
    {  L_,         "1234.5e-2",     ERROR_VALUE,      false   },

    {  L_,          "1.0000000001", ERROR_VALUE,      false   },
    {  L_,          "1.00001e0",    ERROR_VALUE,      false   },

    {  L_,      "65536",            ERROR_VALUE,      false   },
    {  L_,      "65537",            ERROR_VALUE,      false   },

    {  L_,      "65535.01",         ERROR_VALUE,      false   },
    {  L_,      "65535.99",         ERROR_VALUE,      false   },
    {  L_,      "65536.01",         ERROR_VALUE,      false   },
    {  L_,      "65536.99",         ERROR_VALUE,      false   },

    {  L_,  "18446744073709551616", ERROR_VALUE,     false   },

    {  L_,        "-",              ERROR_VALUE,      false   },
    {  L_,        ".5",             ERROR_VALUE,      false   },
    {  L_,        "-.5",            ERROR_VALUE,      false   },
    {  L_,        "e",              ERROR_VALUE,      false   },
    {  L_,        "e1",             ERROR_VALUE,      false   },
    {  L_,        "E",              ERROR_VALUE,      false   },
    {  L_,        "E1",             ERROR_VALUE,      false   },
    {  L_,        "e+",             ERROR_VALUE,      false   },
    {  L_,        "e+1",            ERROR_VALUE,      false   },
    {  L_,        "E+",             ERROR_VALUE,      false   },
    {  L_,        "E+1",            ERROR_VALUE,      false   },
    {  L_,        "e-",             ERROR_VALUE,      false   },
    {  L_,        "e-1",            ERROR_VALUE,      false   },
    {  L_,        "E-",             ERROR_VALUE,      false   },
    {  L_,        "E-1",            ERROR_VALUE,      false   },

    {  L_,        "Z34.56e1",       ERROR_VALUE,      false   },
    {  L_,        "3Z4.56e1",       ERROR_VALUE,      false   },
    {  L_,        "34Z.56e1",       ERROR_VALUE,      false   },
    {  L_,         "34+56e1",       ERROR_VALUE,      false   },
    {  L_,         "34.Z6e1",       ERROR_VALUE,      false   },
    {  L_,         "34.5Ze1",       ERROR_VALUE,      false   },
    {  L_,         "34.56Z1",       ERROR_VALUE,      false   },
    {  L_,         "34.56eZ",       ERROR_VALUE,      false   },
    {  L_,         "34.56e+Z",      ERROR_VALUE,      false   },
    {  L_,         "34.56e-Z",      ERROR_VALUE,      false   },

    {  L_,         "0x12",          ERROR_VALUE,      false   },
    {  L_,         "0x256",         ERROR_VALUE,      false   },
    {  L_,         "JUNK",          ERROR_VALUE,      false   },
    {  L_,         "DEADBEEF",      ERROR_VALUE,      false   },
            };
            const int NUM_DATA = sizeof(DATA) / sizeof(*DATA);

#ifdef BSLS_PLATFORM_HAS_PRAGMA_GCC_DIAGNOSTIC
#pragma GCC diagnostic pop
#endif

            for (int i = 0; i < NUM_DATA; ++i) {
                const int    LINE     = DATA[i].d_line;
                const string INPUT    = DATA[i].d_input_p;
                const Type   EXP      = DATA[i].d_exp;
                const bool   IS_VALID = DATA[i].d_isValid;
                      Type   value    = ERROR_VALUE;

                const bsl::string_view isb(INPUT);

                bslma::TestAllocator         da("default", veryVeryVerbose);
                bslma::DefaultAllocatorGuard dag(&da);

                const int rc = Util::getValue(&value, isb);
                if (IS_VALID) {
                    LOOP2_ASSERT(LINE, rc, 0 == rc);
                }
                else {
                    LOOP2_ASSERT(LINE, rc, rc);
                }
                LOOP3_ASSERT(LINE, EXP, value, EXP == value);

                ASSERTV(LINE, da.numBlocksTotal(), 0 == da.numBlocksTotal());
            }
        }
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING 'getValue' for short values
        //
        // Concerns:
        //: 1 Values in the valid range, including the maximum and minimum
        //:   values for this type, are parsed correctly.
        //:
        //: 2 The passed in variable is unmodified if the data is not valid.
        //:
        //: 3 The return code is 0 on success and non-zero on failure.
        //
        // Plan:
        //: 1 Using the table-driven technique, specify a set of distinct
        //:   rows of string value, expected parsed value, and return code.
        //:
        //: 2 For each row in the table of P-1:
        //:
        //:   1 Provide the string value and a variable  to be parsed into to
        //:     the 'getValue' function.  The variable is assigned a sentinel
        //:     value before invoking the function.
        //:
        //:   2 If the parsing should succeed then verify that the variable
        //:     value matches the expected value.  Otherwise confirm that the
        //:     variable value is unmodified.
        //:
        //:   3 Confirm that the return code is 0 on success and non-zero
        //:     otherwise.
        //
        // Testing:
        //   static int getValue(short               *v, bsl::string_view s);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING 'getValue' for short"
                          << "\n============================" << endl;
        {
            typedef short Type;

            const Type ERROR_VALUE = 99;
            const Type MAX         = bsl::numeric_limits<Type>::max();
            const Type MIN         = bsl::numeric_limits<Type>::min();

#ifdef BSLS_PLATFORM_HAS_PRAGMA_GCC_DIAGNOSTIC
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wnarrowing"
#endif

            static const struct {
                int         d_line;    // line number
                const char *d_input_p; // input on the stream
                Type        d_exp;     // exp unsigned value
                bool        d_isValid; // isValid flag
            } DATA[] = {
   //line          input                    exp     isValid
   //----          -----                    ---     -------
    {  L_,           "0",                     0,      true    },
    {  L_,           "1",                     1,      true    },
    {  L_,          "95",                    95,      true    },
    {  L_,         "127",                   127,      true    },
    {  L_,         "128",                   128,      true    },
    {  L_,         "200",                   200,      true    },
    {  L_,         "255",                   255,      true    },
    {  L_,         "256",                   256,      true    },

    {  L_,          "-1",                    -1,      true    },
    {  L_,        "-128",                  -128,      true    },
    {  L_,        "-129",                  -129,      true    },
    {  L_,        "-255",                  -255,      true    },
    {  L_,        "-256",                  -256,      true    },

    {  L_,      "-32768",                   MIN,      true    },
    {  L_,      "-32767",               MIN + 1,      true    },
    {  L_,       "32766",               MAX - 1,      true    },
    {  L_,       "32767",                   MAX,      true    },

    {  L_,           "0.0",                   0,      true    },
    {  L_,           "0.0000000000000",       0,      true    },
    {  L_,           "1.0",                   1,      true    },
    {  L_,           "1.00000000000",         1,      true    },
    {  L_,         "95.0",                   95,      true    },
    {  L_,         "95.0000000000",          95,      true    },
    {  L_,        "127.00",                 127,      true    },
    {  L_,        "128.00",                 128,      true    },
    {  L_,        "200.00",                 200,      true    },
    {  L_,        "255.00",                 255,      true    },
    {  L_,        "256.00",                 256,      true    },
    {  L_,      "32766.0",                32766,      true    },
    {  L_,      "32766.00000000",         32766,      true    },
    {  L_,      "32767.0",                32767,      true    },
    {  L_,      "32767.00000000",         32767,      true    },

    {  L_,         "-1.00",                  -1,      true    },
    {  L_,       "-127.00",                -127,      true    },
    {  L_,       "-127.0000000",           -127,      true    },
    {  L_,       "-128.00",                -128,      true    },
    {  L_,       "-129.00",                -129,      true    },
    {  L_,       "-255.00",                -255,      true    },
    {  L_,       "-256.00",                -256,      true    },
    {  L_,     "-32767.0",               -32767,      true    },
    {  L_,     "-32767.00000000",        -32767,      true    },
    {  L_,     "-32768.0",               -32768,      true    },
    {  L_,     "-32768.00000000",        -32768,      true    },

    {  L_,          "1e0",                    1,      true    },
    {  L_,          "1E0",                    1,      true    },
    {  L_,          "1e+0",                   1,      true    },
    {  L_,          "1E+0",                   1,      true    },
    {  L_,          "1e-0",                   1,      true    },
    {  L_,          "1E-0",                   1,      true    },

    {  L_,          "1e1",                   10,      true    },
    {  L_,          "2E1",                   20,      true    },
    {  L_,          "3e+1",                  30,      true    },
    {  L_,          "4E+1",                  40,      true    },

    {  L_,          "2e2",                  200,      true    },
    {  L_,          "3E2",                  300,      true    },
    {  L_,          "4e+2",                 400,      true    },
    {  L_,          "5E+2",                 500,      true    },

    {  L_,          "0.1e1",                  1,      true    },
    {  L_,          "0.2e2",                 20,      true    },
    {  L_,          "0.30e2",                30,      true    },
    {  L_,          "0.400e3",              400,      true    },

    {  L_,          "0.3276700e5",        32767,      true    },
    {  L_,          "3.2767000e4",        32767,      true    },
    {  L_,         "32.7670000e3",        32767,      true    },
    {  L_,        "327.6700000e2",        32767,      true    },
    {  L_,       "3276.7000000e1",        32767,      true    },
    {  L_,       "32767.000000e0",        32767,      true    },

    {  L_,         "-0.3276800e5",       -32768,      true    },
    {  L_,         "-3.2768000e4",       -32768,      true    },
    {  L_,        "-32.7680000e3",       -32768,      true    },
    {  L_,       "-327.6800000e2",       -32768,      true    },
    {  L_,      "-3276.8000000e1",       -32768,      true    },
    {  L_,      "-32768.000000e0",       -32768,      true    },

    {  L_,  "3276700000e-5",              32767,      true    },
    {  L_,  "3276700000.00000e-5",        32767,      true    },
    {  L_,   "327670000e-4",              32767,      true    },
    {  L_,    "32767000e-3",              32767,      true    },
    {  L_,     "3276700e-2",              32767,      true    },
    {  L_,      "327670e-1",              32767,      true    },
    {  L_,       "32767e-0",              32767,      true    },

    {  L_,  "-3276800000e-5",            -32768,      true    },
    {  L_,  "-3276800000.000000e-5",     -32768,      true    },
    {  L_,   "-327680000e-4",            -32768,      true    },
    {  L_,    "-32768000e-3",            -32768,      true    },
    {  L_,     "-3276800e-2",            -32768,      true    },
    {  L_,      "-327680e-1",            -32768,      true    },
    {  L_,       "-32768e-0",            -32768,      true    },

    {  L_,          "1.0e0",                  1,      true    },
    {  L_,          "2.000e0",                2,      true    },
    {  L_,          "3.0e1",                 30,      true    },
    {  L_,          "4.5e1",                 45,      true    },
    {  L_,          "6.00e1",                60,      true    },
    {  L_,          "7.00e2",               700,      true    },
    {  L_,          "8.0000e2",             800,      true    },
    {  L_,          "9.12e2",               912,      true    },
    {  L_,          "1.1200e2",             112,      true    },

    {  L_,           ".5",          ERROR_VALUE,      false   },
    {  L_,           ".123",        ERROR_VALUE,      false   },

    {  L_,          "1.1",          ERROR_VALUE,      false   },
    {  L_,          "1.5",          ERROR_VALUE,      false   },
    {  L_,          "1.9",          ERROR_VALUE,      false   },

    {  L_,        "100.123",        ERROR_VALUE,      false   },
    {  L_,         "99.5",          ERROR_VALUE,      false   },
    {  L_,          "0.86",         ERROR_VALUE,      false   },

    {  L_,        "255.01",         ERROR_VALUE,      false   },
    {  L_,        "255.99",         ERROR_VALUE,      false   },

    {  L_,          "1.234e+1",     ERROR_VALUE,      false   },
    {  L_,          "1.987E+1",     ERROR_VALUE,      false   },

    {  L_,          "1e1e1",        ERROR_VALUE,      false   },
    {  L_,          "1e1e-1",       ERROR_VALUE,      false   },

    {  L_,         "12.34e-1",      ERROR_VALUE,      false   },
    {  L_,         "29.87E-1",      ERROR_VALUE,      false   },

    {  L_,         "12345e-1",      ERROR_VALUE,      false   },
    {  L_,         "12345e-2",      ERROR_VALUE,      false   },
    {  L_,         "12345e-4",      ERROR_VALUE,      false   },
    {  L_,         "12345e-5",      ERROR_VALUE,      false   },

    {  L_,         "-12345e-1",     ERROR_VALUE,      false   },
    {  L_,         "-12345e-2",     ERROR_VALUE,      false   },
    {  L_,         "-12345e-4",     ERROR_VALUE,      false   },
    {  L_,         "-12345e-5",     ERROR_VALUE,      false   },
    {  L_,         "-12345e-6",     ERROR_VALUE,      false   },

    {  L_,         "123.45e-1",     ERROR_VALUE,      false   },
    {  L_,         "1234.5e-2",     ERROR_VALUE,      false   },

    {  L_,          "1.0000000001", ERROR_VALUE,      false   },
    {  L_,          "1.00001e0",    ERROR_VALUE,      false   },

    {  L_,      "32768",            ERROR_VALUE,      false   },
    {  L_,      "65535",            ERROR_VALUE,      false   },

    {  L_,     "-32769",            ERROR_VALUE,      false   },
    {  L_,     "-65535",            ERROR_VALUE,      false   },

    {  L_,      "32767.01",         ERROR_VALUE,      false   },
    {  L_,      "32767.99",         ERROR_VALUE,      false   },
    {  L_,      "32768.01",         ERROR_VALUE,      false   },
    {  L_,      "65535.01",         ERROR_VALUE,      false   },

    {  L_,     "-32768.01",         ERROR_VALUE,      false   },
    {  L_,     "-32768.99",         ERROR_VALUE,      false   },
    {  L_,     "-32769.01",         ERROR_VALUE,      false   },
    {  L_,     "-32769.99",         ERROR_VALUE,      false   },

    {  L_,  "18446744073709551616", ERROR_VALUE,     false   },

    {  L_,        "-",              ERROR_VALUE,      false   },
    {  L_,        ".5",             ERROR_VALUE,      false   },
    {  L_,        "-.5",            ERROR_VALUE,      false   },
    {  L_,        "e",              ERROR_VALUE,      false   },
    {  L_,        "e1",             ERROR_VALUE,      false   },
    {  L_,        "E",              ERROR_VALUE,      false   },
    {  L_,        "E1",             ERROR_VALUE,      false   },
    {  L_,        "e+",             ERROR_VALUE,      false   },
    {  L_,        "e+1",            ERROR_VALUE,      false   },
    {  L_,        "E+",             ERROR_VALUE,      false   },
    {  L_,        "E+1",            ERROR_VALUE,      false   },
    {  L_,        "e-",             ERROR_VALUE,      false   },
    {  L_,        "e-1",            ERROR_VALUE,      false   },
    {  L_,        "E-",             ERROR_VALUE,      false   },
    {  L_,        "E-1",            ERROR_VALUE,      false   },

    {  L_,        "Z34.56e1",       ERROR_VALUE,      false   },
    {  L_,        "3Z4.56e1",       ERROR_VALUE,      false   },
    {  L_,        "34Z.56e1",       ERROR_VALUE,      false   },
    {  L_,         "34+56e1",       ERROR_VALUE,      false   },
    {  L_,         "34.Z6e1",       ERROR_VALUE,      false   },
    {  L_,         "34.5Ze1",       ERROR_VALUE,      false   },
    {  L_,         "34.56Z1",       ERROR_VALUE,      false   },
    {  L_,         "34.56eZ",       ERROR_VALUE,      false   },
    {  L_,         "34.56e+Z",      ERROR_VALUE,      false   },
    {  L_,         "34.56e-Z",      ERROR_VALUE,      false   },

    {  L_,         "0x12",          ERROR_VALUE,      false   },
    {  L_,         "0x256",         ERROR_VALUE,      false   },
    {  L_,         "JUNK",          ERROR_VALUE,      false   },
    {  L_,         "DEADBEEF",      ERROR_VALUE,      false   },
            };
            const int NUM_DATA = sizeof(DATA) / sizeof(*DATA);

#ifdef BSLS_PLATFORM_HAS_PRAGMA_GCC_DIAGNOSTIC
#pragma GCC diagnostic pop
#endif

            for (int i = 0; i < NUM_DATA; ++i) {
                const int    LINE     = DATA[i].d_line;
                const string INPUT    = DATA[i].d_input_p;
                const Type   EXP      = DATA[i].d_exp;
                const int    IS_VALID = DATA[i].d_isValid;
                      Type   value    = ERROR_VALUE;

                const bsl::string_view isb(INPUT);

                bslma::TestAllocator         da("default", veryVeryVerbose);
                bslma::DefaultAllocatorGuard dag(&da);

                const int rc = Util::getValue(&value, isb);
                if (IS_VALID) {
                    LOOP2_ASSERT(LINE, rc, 0 == rc);
                }
                else {
                    LOOP2_ASSERT(LINE, rc, rc);
                }
                LOOP3_ASSERT(LINE, EXP, value, EXP == value);

                ASSERTV(LINE, da.numBlocksTotal(), 0 == da.numBlocksTotal());
            }
        }
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING 'getValue' for unsigned char values
        //
        // Concerns:
        //: 1 Values in the valid range, including the maximum and minimum
        //:   values for this type, are parsed correctly.
        //:
        //: 2 The passed in variable is unmodified if the data is not valid.
        //:
        //: 3 The return code is 0 on success and non-zero on failure.
        //
        // Plan:
        //: 1 Using the table-driven technique, specify a set of distinct
        //:   rows of string value, expected parsed value, and return code.
        //:
        //: 2 For each row in the table of P-1:
        //:
        //:   1 Provide the string value and a variable  to be parsed into to
        //:     the 'getValue' function.  The variable is assigned a sentinel
        //:     value before invoking the function.
        //:
        //:   2 If the parsing should succeed then verify that the variable
        //:     value matches the expected value.  Otherwise confirm that the
        //:     variable value is unmodified.
        //:
        //:   3 Confirm that the return code is 0 on success and non-zero
        //:     otherwise.
        //
        // Testing:
        //   static int getValue(unsigned char       *v, bsl::string_view s);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING 'getValue' for unsigned char"
                          << "\n====================================" << endl;
        {
            typedef unsigned char Type;

            const Type ERROR_VALUE = 99;
            const Type MAX         = bsl::numeric_limits<Type>::max();
            const Type MIN         = bsl::numeric_limits<Type>::min();

#ifdef BSLS_PLATFORM_HAS_PRAGMA_GCC_DIAGNOSTIC
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wnarrowing"
#endif

            static const struct {
                int         d_line;    // line number
                const char *d_input_p; // input on the stream
                Type        d_exp;     // exp unsigned value
                bool        d_isValid; // isValid flag
            } DATA[] = {
   //line          input                    exp     isValid
   //----          -----                    ---     -------
    {  L_,           "1",                     1,      true    },
    {  L_,          "95",                    95,      true    },
    {  L_,         "127",                   127,      true    },
    {  L_,         "128",                   128,      true    },
    {  L_,         "200",                   200,      true    },

    {  L_,           "0",                   MIN,      true    },
    {  L_,           "1",               MIN + 1,      true    },
    {  L_,         "254",               MAX - 1,      true    },
    {  L_,         "255",                   MAX,      true    },

    {  L_,           "0.0",                   0,      true    },
    {  L_,           "0.0000000000000",       0,      true    },
    {  L_,           "1.0",                   1,      true    },
    {  L_,           "1.00000000000",         1,      true    },
    {  L_,         "95.0",                   95,      true    },
    {  L_,         "95.0000000000",          95,      true    },
    {  L_,        "127.00",                 127,      true    },
    {  L_,        "128.00",                 128,      true    },
    {  L_,        "200.00",                 200,      true    },
    {  L_,        "255.00",                 255,      true    },

    {  L_,          "1e0",                    1,      true    },
    {  L_,          "1E0",                    1,      true    },
    {  L_,          "1e+0",                   1,      true    },
    {  L_,          "1E+0",                   1,      true    },
    {  L_,          "1e-0",                   1,      true    },
    {  L_,          "1E-0",                   1,      true    },

    {  L_,          "1e1",                   10,      true    },
    {  L_,          "2E1",                   20,      true    },
    {  L_,          "3e+1",                  30,      true    },
    {  L_,          "4E+1",                  40,      true    },
    {  L_,          "2e2",                  200,      true    },

    {  L_,          "0.1e1",                  1,      true    },
    {  L_,          "0.2e2",                 20,      true    },
    {  L_,          "0.30e2",                30,      true    },

    {  L_,          "0.25500e3",            255,      true    },
    {  L_,          "2.55000e2",            255,      true    },
    {  L_,          "25.5000e1",            255,      true    },
    {  L_,          "255.000e0",            255,      true    },

    {  L_,  "25500000e-5",                  255,      true    },
    {  L_,  "25500000.0000000e-5",          255,      true    },
    {  L_,   "2550000e-4",                  255,      true    },
    {  L_,    "255000e-3",                  255,      true    },
    {  L_,     "25500e-2",                  255,      true    },
    {  L_,      "2550e-1",                  255,      true    },
    {  L_,       "255e-0",                  255,      true    },

    {  L_,          "1.0e0",                  1,      true    },
    {  L_,          "2.000e0",                2,      true    },
    {  L_,          "3.0e1",                 30,      true    },
    {  L_,          "4.5e1",                 45,      true    },
    {  L_,          "6.00e1",                60,      true    },

    {  L_,           ".5",          ERROR_VALUE,      false   },
    {  L_,           ".123",        ERROR_VALUE,      false   },

    {  L_,          "1.1",          ERROR_VALUE,      false   },
    {  L_,          "1.5",          ERROR_VALUE,      false   },
    {  L_,          "1.9",          ERROR_VALUE,      false   },

    {  L_,        "100.123",        ERROR_VALUE,      false   },
    {  L_,         "99.5",          ERROR_VALUE,      false   },
    {  L_,          "0.86",         ERROR_VALUE,      false   },

    {  L_,        "255.01",         ERROR_VALUE,      false   },
    {  L_,        "255.99",         ERROR_VALUE,      false   },

    {  L_,          "1.234e+1",     ERROR_VALUE,      false   },
    {  L_,          "1.987E+1",     ERROR_VALUE,      false   },

    {  L_,         "12.34e-1",      ERROR_VALUE,      false   },
    {  L_,         "29.87E-1",      ERROR_VALUE,      false   },

    {  L_,         "12345e-1",      ERROR_VALUE,      false   },
    {  L_,         "12345e-2",      ERROR_VALUE,      false   },
    {  L_,         "12345e-4",      ERROR_VALUE,      false   },
    {  L_,         "12345e-5",      ERROR_VALUE,      false   },

    {  L_,         "-12345e-1",     ERROR_VALUE,      false   },
    {  L_,         "-12345e-2",     ERROR_VALUE,      false   },
    {  L_,         "-12345e-4",     ERROR_VALUE,      false   },
    {  L_,         "-12345e-5",     ERROR_VALUE,      false   },
    {  L_,         "-12345e-6",     ERROR_VALUE,      false   },

    {  L_,         "123.45e-1",     ERROR_VALUE,      false   },
    {  L_,         "1234.5e-2",     ERROR_VALUE,      false   },

    {  L_,          "1.0000000001", ERROR_VALUE,      false   },
    {  L_,          "1.00001e0",    ERROR_VALUE,      false   },

    {  L_,        "256",            ERROR_VALUE,      false   },

    {  L_,        "255.01",         ERROR_VALUE,      false   },
    {  L_,        "255.99",         ERROR_VALUE,      false   },

    {  L_,  "18446744073709551616", ERROR_VALUE,     false   },

    {  L_,        "-",              ERROR_VALUE,      false   },
    {  L_,        ".5",             ERROR_VALUE,      false   },
    {  L_,        "-.5",            ERROR_VALUE,      false   },
    {  L_,        "e",              ERROR_VALUE,      false   },
    {  L_,        "e1",             ERROR_VALUE,      false   },
    {  L_,        "E",              ERROR_VALUE,      false   },
    {  L_,        "E1",             ERROR_VALUE,      false   },
    {  L_,        "e+",             ERROR_VALUE,      false   },
    {  L_,        "e+1",            ERROR_VALUE,      false   },
    {  L_,        "E+",             ERROR_VALUE,      false   },
    {  L_,        "E+1",            ERROR_VALUE,      false   },
    {  L_,        "e-",             ERROR_VALUE,      false   },
    {  L_,        "e-1",            ERROR_VALUE,      false   },
    {  L_,        "E-",             ERROR_VALUE,      false   },
    {  L_,        "E-1",            ERROR_VALUE,      false   },

    {  L_,         "Z4.56e1",       ERROR_VALUE,      false   },
    {  L_,         "3Z.56e1",       ERROR_VALUE,      false   },
    {  L_,          "3+56e1",       ERROR_VALUE,      false   },
    {  L_,          "3.Z6e1",       ERROR_VALUE,      false   },
    {  L_,          "3.5Ze1",       ERROR_VALUE,      false   },
    {  L_,          "3.56Z1",       ERROR_VALUE,      false   },
    {  L_,          "3.56eZ",       ERROR_VALUE,      false   },
    {  L_,          "3.56e+Z",      ERROR_VALUE,      false   },
    {  L_,          "3.56e-Z",      ERROR_VALUE,      false   },

    {  L_,         "0x12",          ERROR_VALUE,      false   },
    {  L_,         "0x256",         ERROR_VALUE,      false   },
    {  L_,         "JUNK",          ERROR_VALUE,      false   },
    {  L_,         "DEADBEEF",      ERROR_VALUE,      false   },
            };
            const int NUM_DATA = sizeof(DATA) / sizeof(*DATA);

#ifdef BSLS_PLATFORM_HAS_PRAGMA_GCC_DIAGNOSTIC
#pragma GCC diagnostic pop
#endif

            for (int i = 0; i < NUM_DATA; ++i) {
                const int    LINE     = DATA[i].d_line;
                const string INPUT    = DATA[i].d_input_p;
                const Type   EXP      = DATA[i].d_exp;
                const bool   IS_VALID = DATA[i].d_isValid;
                      Type   value    = ERROR_VALUE;

                const bsl::string_view isb(INPUT);

                bslma::TestAllocator         da("default", veryVeryVerbose);
                bslma::DefaultAllocatorGuard dag(&da);

                const int rc = Util::getValue(&value, isb);
                if (IS_VALID) {
                    LOOP2_ASSERT(LINE, rc, 0 == rc);
                }
                else {
                    LOOP2_ASSERT(LINE, rc, rc);
                }
                LOOP3_ASSERT(LINE, EXP, value, EXP == value);

                ASSERTV(LINE, da.numBlocksTotal(), 0 == da.numBlocksTotal());
            }
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING 'getValue' for signed char values
        //
        // Concerns:
        //: 1 Values in the valid range, including the maximum and minimum
        //:   values for this type, are parsed correctly.
        //:
        //: 2 The passed in variable is unmodified if the data is not valid.
        //:
        //: 3 The return code is 0 on success and non-zero on failure.
        //
        // Plan:
        //: 1 Using the table-driven technique, specify a set of distinct
        //:   rows of string value, expected parsed value, and return code.
        //:
        //: 2 For each row in the table of P-1:
        //:
        //:   1 Provide the string value and a variable  to be parsed into to
        //:     the 'getValue' function.  The variable is assigned a sentinel
        //:     value before invoking the function.
        //:
        //:   2 If the parsing should succeed then verify that the variable
        //:     value matches the expected value.  Otherwise confirm that the
        //:     variable value is unmodified.
        //:
        //:   3 Confirm that the return code is 0 on success and non-zero
        //:     otherwise.
        //
        // Testing:
        //   static int getValue(char                *v, bsl::string_view s);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING 'getValue' for signed char"
                          << "\n==================================" << endl;

        {
            const char ERROR_VALUE = 'X';
            const char MAX         = bsl::numeric_limits<signed char>::max();
            const char MIN         = bsl::numeric_limits<signed char>::min();

#ifdef BSLS_PLATFORM_HAS_PRAGMA_GCC_DIAGNOSTIC
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wnarrowing"
#endif

            static const struct {
                int          d_line;    // line number
                const char  *d_input_p; // input on the stream
                signed char  d_exp;     // exp char value
                bool         d_isValid; // isValid flag
            } DATA[] = {
   //line          input                    exp     isValid
   //----          -----                    ---     -------
    {  L_,           "0",                  '\0',      true    },
    {  L_,          "32",                   ' ',      true    },
    {  L_,          "49",                   '1',      true    },
    {  L_,          "65",                   'A',      true    },
    {  L_,         "126",                   '~',      true    },

    {  L_,        "-128",                  MIN,       true    },
    {  L_,        "-127",               MIN + 1,      true    },
    {  L_,         "126",               MAX - 1,      true    },
    {  L_,         "127",                   MAX,      true    },

    {  L_,           "9",                  '\t',      true    },
    {  L_,          "10",                  '\n',      true    },
    {  L_,          "13",                  '\r',      true    },
    {  L_,          "34",                  '\"',      true    },
    {  L_,          "92",                  '\\',      true    },

    {  L_,           "0.0",                '\0',      true    },
    {  L_,           "0.0000000000000",    '\0',      true    },
    {  L_,          "32.0",                 ' ',      true    },
    {  L_,          "32.00000000000",       ' ',      true    },
    {  L_,         "95.0",                  '_',      true    },
    {  L_,         "95.0000000000",         '_',      true    },
    {  L_,        "126.00",                 '~',      true    },

    {  L_,          "0e0",                 '\0',      true    },
    {  L_,          "0E0",                 '\0',      true    },
    {  L_,          "0e+0",                '\0',      true    },
    {  L_,          "0E+0",                '\0',      true    },
    {  L_,          "0e-0",                '\0',      true    },
    {  L_,          "0E-0",                '\0',      true    },

    {  L_,          "1e1",                 '\n',      true    },
    {  L_,          "1E1",                 '\n',      true    },
    {  L_,          "1e+1",                '\n',      true    },
    {  L_,          "1E+1",                '\n',      true    },

    {  L_,          "0.9e1",               '\t',      true    },
    {  L_,          "0.7e2",                'F',      true    },
    {  L_,          "0.70e2",               'F',      true    },
    {  L_,          "0.100e3",              'd',      true    },

    {  L_,          "0.12600e3",            '~',      true    },
    {  L_,          "1.260e2",              '~',      true    },
    {  L_,          "12.60e1",              '~',      true    },
    {  L_,          "126.0e0",              '~',      true    },

    {  L_,  "12600000e-5",                  '~',      true    },
    {  L_,  "12600000.00000e-5",            '~',      true    },
    {  L_,   "1260000e-4",                  '~',      true    },
    {  L_,    "126000e-3",                  '~',      true    },
    {  L_,     "12600e-2",                  '~',      true    },
    {  L_,      "1260e-1",                  '~',      true    },
    {  L_,       "126e-0",                  '~',      true    },

    {  L_,          "9.0e0",               '\t',      true    },
    {  L_,          "9.000e0",             '\t',      true    },
    {  L_,          "3.3e1",                '!',      true    },
    {  L_,          "6.5e1",                'A',      true    },
    {  L_,          "6.50e1",               'A',      true    },

    {  L_,           ".5",          ERROR_VALUE,      false   },
    {  L_,           ".123",        ERROR_VALUE,      false   },

    {  L_,          "1.1",          ERROR_VALUE,      false   },
    {  L_,          "1.5",          ERROR_VALUE,      false   },
    {  L_,          "1.9",          ERROR_VALUE,      false   },

    {  L_,        "100.123",        ERROR_VALUE,      false   },
    {  L_,         "99.5",          ERROR_VALUE,      false   },
    {  L_,          "0.86",         ERROR_VALUE,      false   },

    {  L_,        "127.01",         ERROR_VALUE,      false   },
    {  L_,        "127.99",         ERROR_VALUE,      false   },

    {  L_,  "18446744073709551616", ERROR_VALUE,     false   },

    {  L_,          "1.234e+1",     ERROR_VALUE,      false   },
    {  L_,          "1.987E+1",     ERROR_VALUE,      false   },

    {  L_,          "1e1e1",        ERROR_VALUE,      false   },
    {  L_,          "1e1e-1",       ERROR_VALUE,      false   },

    {  L_,         "12.34e-1",      ERROR_VALUE,      false   },
    {  L_,         "29.87E-1",      ERROR_VALUE,      false   },

    {  L_,         "12345e-1",      ERROR_VALUE,      false   },
    {  L_,         "12345e-2",      ERROR_VALUE,      false   },
    {  L_,         "12345e-4",      ERROR_VALUE,      false   },
    {  L_,         "12345e-5",      ERROR_VALUE,      false   },

    {  L_,         "-12345e-1",     ERROR_VALUE,      false   },
    {  L_,         "-12345e-2",     ERROR_VALUE,      false   },
    {  L_,         "-12345e-4",     ERROR_VALUE,      false   },
    {  L_,         "-12345e-5",     ERROR_VALUE,      false   },
    {  L_,         "-12345e-6",     ERROR_VALUE,      false   },

    {  L_,         "123.45e-1",     ERROR_VALUE,      false   },
    {  L_,         "1234.5e-2",     ERROR_VALUE,      false   },

    {  L_,          "1.0000000001", ERROR_VALUE,      false   },
    {  L_,          "1.00001e0",    ERROR_VALUE,      false   },

    {  L_,        "128",            ERROR_VALUE,      false   },
    {  L_,       "-129",            ERROR_VALUE,      false   },

    {  L_,      "127.01",           ERROR_VALUE,      false   },
    {  L_,      "127.99",           ERROR_VALUE,      false   },
    {  L_,      "128.01",           ERROR_VALUE,      false   },
    {  L_,      "128.01",           ERROR_VALUE,      false   },

    {  L_,     "-128.01",           ERROR_VALUE,      false   },
    {  L_,     "-128.99",           ERROR_VALUE,      false   },
    {  L_,     "-129.01",           ERROR_VALUE,      false   },
    {  L_,     "-129.99",           ERROR_VALUE,      false   },

    {  L_,        "-",              ERROR_VALUE,      false   },
    {  L_,        ".5",             ERROR_VALUE,      false   },
    {  L_,        "-.5",            ERROR_VALUE,      false   },

    {  L_,       "Z2.7e1",          ERROR_VALUE,      false   },
    {  L_,       "1Z.7e1",          ERROR_VALUE,      false   },
    {  L_,       "12+7e1",          ERROR_VALUE,      false   },
    {  L_,       "12.Ze1",          ERROR_VALUE,      false   },
    {  L_,       "12.7Z1",          ERROR_VALUE,      false   },
    {  L_,       "12.7eZ",          ERROR_VALUE,      false   },
    {  L_,       "12.7e+Z",         ERROR_VALUE,      false   },
    {  L_,       "12.7e-Z",         ERROR_VALUE,      false   },

    {  L_,       "0x12",            ERROR_VALUE,      false   },
    {  L_,       "0x256",           ERROR_VALUE,      false   },
    {  L_,       "JUNK",            ERROR_VALUE,      false   },
    {  L_,       "DEADBEEF",        ERROR_VALUE,      false   },

    {  L_,       "\"\"",            ERROR_VALUE,      false   },
    {  L_,       "\"AB\"",          ERROR_VALUE,      false   },
            };
            const int NUM_DATA = sizeof(DATA) / sizeof(*DATA);

#ifdef BSLS_PLATFORM_HAS_PRAGMA_GCC_DIAGNOSTIC
#pragma GCC diagnostic pop
#endif

            for (int i = 0; i < NUM_DATA; ++i) {
                const int            LINE     = DATA[i].d_line;
                const string         INPUT    = DATA[i].d_input_p;
                const char           C        = DATA[i].d_exp;
                const signed char    SC       = (signed char) DATA[i].d_exp;
                const bool           IS_VALID = DATA[i].d_isValid;
                      char           c        = ERROR_VALUE;
                      signed char    sc       = ERROR_VALUE;

                if (veryVerbose) { P(INPUT) P(C) }

                // char values
                {
                    const bsl::string_view isb(INPUT);
                    const int rc = Util::getValue(&c, isb);
                    if (IS_VALID) {
                        LOOP2_ASSERT(LINE, rc, 0 == rc);
                    }
                    else {
                        LOOP2_ASSERT(LINE, rc, rc);
                    }
                    LOOP3_ASSERT(LINE, C, c, C == c);
                }

                // signed char values
                {
                    const bsl::string_view isb(INPUT);
                    const int rc = Util::getValue(&sc, isb);
                    if (IS_VALID) {
                        LOOP2_ASSERT(LINE, rc, 0 == rc);
                    }
                    else {
                        LOOP2_ASSERT(LINE, rc, rc);
                    }
                    LOOP3_ASSERT(LINE, SC, sc, SC == sc);
                }
            }
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING 'getValue' for bool values
        //
        // Concerns:
        //: 1 "true" is decoded into 'true' and "false" is decoded into
        //:   'false'.
        //:
        //: 2 A string that is a subset of 'true' and 'false' returns an error.
        //:
        //: 3 The return code is 0 on success and non-zero on failure.
        //
        // Plan:
        //: 1 Use a brute force approach to test both cases.  Confirm that the
        //:   return value is 0.
        //:
        //: 2 Try to decode an erroneous value and verify that the return
        //:   value is non-zero.
        //
        // Testing:
        //   static int getValue(bool                *v, bsl::string_viewf s);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING 'getValue' for bool"
                          << "\n===========================" << endl;

        {
            typedef bool Type;

            const Type EV = false;

            static const struct {
                int         d_line;    // line number
                const char *d_input_p; // input on the stream
                Type        d_exp;     // exp char value
                bool        d_isValid; // isValid flag
            } DATA[] = {
                //line    input        exp      isValid
                //----    -----        ---      -------

                {   L_,    "",          EV,      false    },

                {   L_,    "T",         EV,      false    },
                {   L_,    "F",         EV,      false    },

                {   L_,    "tru",       EV,      false    },
                {   L_,    "fals",      EV,      false    },

                {   L_,    "true",      true,    true     },
                {   L_,    "false",     false,   true     },

                {   L_,    "True",      EV,      false    },
                {   L_,    "False",     EV,      false    },

                {   L_,    "TRUE",      EV,      false    },
                {   L_,    "FALSE",     EV,      false    },

                {   L_,    "truee",     EV,      false    },
                {   L_,    "falsee",    EV,      false    },

                {   L_,    "truefalse", EV,      false    },
                {   L_,    "falsetrue", EV,      false    },
            };
            const int NUM_DATA = sizeof(DATA) / sizeof(*DATA);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int            LINE     = DATA[i].d_line;
                const string         INPUT    = DATA[i].d_input_p;
                const bool           EXP      = DATA[i].d_exp;
                const bool           IS_VALID = DATA[i].d_isValid;
                      bool           value    = EV;

                if (veryVerbose) { P(LINE) P(INPUT) P(EXP) }

                {
                    const bsl::string_view isb(INPUT);
                    const int rc = Util::getValue(&value, isb);
                    if (IS_VALID) {
                        LOOP2_ASSERT(LINE, rc, 0 == rc);
                    }
                    else {
                        LOOP2_ASSERT(LINE, rc, rc);
                    }
                    LOOP3_ASSERT(LINE, EXP, value, EXP == value);
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
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) cout << "\nBREATHING TEST"
                          << "\n==============" << endl;

        const struct {
            int         d_line;
            const char* d_string;
            bool        d_validFlag;
            double      d_result;
        } DATA[] = {
            //LINE        STRING    FLAG      RESULTS
            //----        ------    ----      -------

            { L_,   "-1.625e308",   true,  -1.625e308 },
            { L_,    "-3.125e38",   true,   -3.125e38 },

            { L_,         "-1.0",   true,        -1.0 },
            { L_,         "-0.0",   true,         0.0 },

            { L_,            "0",   true,         0.0 },
            { L_,          "0.0",   true,         0.0 },
            { L_,          "1.0",   true,         1.0 },
            { L_,     "3.125e38",   true,    3.125e38 },
            { L_,    "1.625e308",   true,   1.625e308 },

            { L_,          "0e0",   true,         0.0 },

            { L_,          "1e0",   true,         1.0 },

            { L_,         "-1.5",   true,        -1.5 },
            { L_,     "-1.25e-3",   true,    -1.25e-3 },
            { L_,     "9.25e+10",   true,     9.25e10 },

            { L_,           "+1",  false,         0.0 },
            { L_,          "--1",  false,         0.0 },
            { L_,           "1.",  false,         0.0 },
            { L_,        "1e+-1",  false,         0.0 }
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int ti = 0; ti < NUM_DATA; ++ ti) {
            const int         LINE   = DATA[ti].d_line;
            const char *const STRING = DATA[ti].d_string;
            const bool        FLAG   = DATA[ti].d_validFlag;
            const double      EXP    = DATA[ti].d_result;

            const bsl::string_view iss(STRING);

            double result;
            const int rc = Util::getValue(&result, iss);
            if (FLAG) {
                ASSERTV(LINE, rc,               0 == rc);
                ASSERTV(LINE, result, EXP, result == EXP);
            }
            else {
                ASSERTV(LINE, rc, rc);
            }
        }
      } break;
      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      }
    }

    // CONCERN: In no case does memory come from the global/default allocator.

    LOOP_ASSERT(globalAllocator.numBlocksTotal(),
                0 == globalAllocator.numBlocksTotal());

    if (testStatus > 0) {
        cerr << "Error, non-zero test status = " << testStatus << "." << endl;
    }

    return testStatus;
}

// ----------------------------------------------------------------------------
// Copyright 2018 Bloomberg Finance L.P.
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
