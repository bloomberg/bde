// baljsn_jsonformatter.t.cpp                                         -*-C++-*-

#include <baljsn_jsonformatter.h>
#include <baljsn_encoderoptions.h>
#include <baljsn_parserutil.h>

#include <bslim_testutil.h>

#include <bdljsn_json.h>
#include <bdljsn_jsonnull.h>
#include <bdljsn_jsontype.h>

#include <bdlde_utf8util.h>

#include <bdlt_date.h>
#include <bdlt_datetz.h>
#include <bdlt_datetime.h>
#include <bdlt_datetimetz.h>
#include <bdlt_time.h>
#include <bdlt_timetz.h>

#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>
#include <bslma_testallocatormonitor.h>

#include <bslmf_assert.h>

#include <bsla_unreachable.h>

#include <bsls_asserttest.h>
#include <bsls_nameof.h>

#include <bsl_climits.h>
#include <bsl_cstddef.h>  // `bsl::size_t`
#include <bsl_cstdlib.h>
#include <bsl_iostream.h>
#include <bsl_limits.h>
#include <bsl_memory.h>
#include <bsl_sstream.h>
#include <bsl_string.h>
#include <bsl_utility.h>

using namespace BloombergLP;
using bsl::cout;
using bsl::cerr;
using bsl::endl;
using bsl::flush;

// ============================================================================
//                             TEST PLAN
// ----------------------------------------------------------------------------
//                             Overview
//                             --------
// The component under test implements a formatter mechanism for outputting
// `bdeat`-compatible objects as `bdljsn::Json` objects.
//
// Many of the tests create assorted `bdljsn::Json` objects both by using
// native methods (the oracle) and agin by using the formatter methods.  Then
// the two objects are compared for equality.
// ----------------------------------------------------------------------------
// CREATORS
// [ 1] Formatter(bdljsn::Json *json, bslma::Allocator *bA = 0);
// [ 1] ~Formatter();
//
// MANIPULATORS
// [ 4] void openObject();
// [ 4] void closeObject();
// [ 3] void openArray();
// [ 3] void closeArray();
// [ 4] int openMember();
// [ 2] int putValue(const TYPE& value, const EncoderOptions *options);
// [ 5] int putNullValue();
// [ 4] void closeMember();
// [ 1] void addArrayElementSeparator();
//
// ACCESSORS
// [ 1] int nestingDepth() const;
// [ 1] allocator_type get_allocator() const;
// ----------------------------------------------------------------------------
// [ 6] USAGE EXAMPLE
// [ 2] CONCERN: `putValue` IN SCALAR CONTEXT
// [ 2] CONCERN: `putValue` IN ARRAY  CONTEXT
// [ 4] CONCERN: `putValue` IN OBJECT CONTEXT
// [ 3] CONCERN: NESTED ARRAYS
// [ 3] CONCERN: OBJECT IN ARRAY
// [ 4] CONCERN: OBJECT AS MEMBER VALUE
// [ 4] CONCERN: ARRAY  AS MEMBER VALUE

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
#define P_           BSLIM_TESTUTIL_P_  // P(X) without `\n`.
#define T_           BSLIM_TESTUTIL_T_  // Print a tab (w/o newline).
#define L_           BSLIM_TESTUTIL_L_  // current Line number

// ============================================================================
//                     NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT_SAFE_FAIL(expr) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(expr)
#define ASSERT_SAFE_PASS(expr) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(expr)

#define ASSERT_FAIL(expr)      BSLS_ASSERTTEST_ASSERT_FAIL(expr)
#define ASSERT_PASS(expr)      BSLS_ASSERTTEST_ASSERT_PASS(expr)

#define ASSERT_OPT_FAIL(expr)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL(expr)
#define ASSERT_OPT_PASS(expr)  BSLS_ASSERTTEST_ASSERT_OPT_PASS(expr)

// ============================================================================
//                   GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------

typedef baljsn::JsonFormatter  Obj;
typedef baljsn::EncoderOptions Options;

typedef bdljsn::Json           Json;
typedef bdljsn::JsonType       JT;
typedef bdljsn::JsonType::Enum Enum;

bool g_veryVeryVerbose;

const bool                A = true;
const char                B = 'A';
const signed char         C = '\"';
const unsigned char       D = bsl::numeric_limits<unsigned char      >::max();
const short               E = -1;
const unsigned short      F = bsl::numeric_limits<unsigned short     >::max();
const int                 G = -10;
const unsigned int        H = bsl::numeric_limits<unsigned int       >::max();
const bsls::Types::Int64  I = -100;
const bsls::Types::Uint64 J = bsl::numeric_limits<bsls::Types::Uint64>::max();
const float               K = -1.5;
const double              L = 10.5;
const char               *M = "one";
const bsl::string         N = "one";
const bdldfp::Decimal64   O = BDLDFP_DECIMAL_DD(1.13);

const bdlt::Date          PA(2000,  1,  1);
const bdlt::Time          QA(0, 1, 2, 3, 4);
const bdlt::Datetime      R(PA, QA);
const bdlt::DateTz        S(PA, -5);
const bdlt::TimeTz        T(QA, -5);
const bdlt::DatetimeTz    U(R,  -5);

const int roundoffMicroseconds = -4; // Default options is 3 decimal places
                                     // so the microseconds of `QA` are lost.

const float               INV1 = bsl::numeric_limits<float >::infinity();
const double              INV2 = bsl::numeric_limits<double>::infinity();
const char               *INV3 = "\x80";
const bsl::string         INV4 = "\xc2\x00";

// ============================================================================
//                                `putValue` HELPERS
// ----------------------------------------------------------------------------

template <class ValueTYPE>
struct VT2JT {  // `ValueType` to `JsonType::Enum`
};

template<> struct VT2JT<bool                 >{ static const Enum value; };
template<> struct VT2JT<char                 >{ static const Enum value; };
template<> struct VT2JT<signed char          >{ static const Enum value; };
template<> struct VT2JT<unsigned char        >{ static const Enum value; };
template<> struct VT2JT<short                >{ static const Enum value; };
template<> struct VT2JT<unsigned short       >{ static const Enum value; };
template<> struct VT2JT<int                  >{ static const Enum value; };
template<> struct VT2JT<unsigned int         >{ static const Enum value; };
template<> struct VT2JT<bsls::Types::Int64   >{ static const Enum value; };
template<> struct VT2JT<bsls::Types::Uint64  >{ static const Enum value; };
template<> struct VT2JT<float                >{ static const Enum value; };
template<> struct VT2JT<double               >{ static const Enum value; };
template<> struct VT2JT<const char          *>{ static const Enum value; };
template<> struct VT2JT<bsl::string          >{ static const Enum value; };
template<> struct VT2JT<bdldfp::Decimal64    >{ static const Enum value; };

template<> struct VT2JT<bdlt::Date           >{ static const Enum value; };
template<> struct VT2JT<bdlt::Time           >{ static const Enum value; };
template<> struct VT2JT<bdlt::Datetime       >{ static const Enum value; };
template<> struct VT2JT<bdlt::DateTz         >{ static const Enum value; };
template<> struct VT2JT<bdlt::TimeTz         >{ static const Enum value; };
template<> struct VT2JT<bdlt::DatetimeTz     >{ static const Enum value; };

const Enum VT2JT<bool                 >::value = JT::e_BOOLEAN;
const Enum VT2JT<char                 >::value = JT::e_NUMBER;
const Enum VT2JT<signed char          >::value = JT::e_NUMBER;
const Enum VT2JT<unsigned char        >::value = JT::e_NUMBER;
const Enum VT2JT<short                >::value = JT::e_NUMBER;
const Enum VT2JT<unsigned short       >::value = JT::e_NUMBER;
const Enum VT2JT<int                  >::value = JT::e_NUMBER;
const Enum VT2JT<unsigned int         >::value = JT::e_NUMBER;
const Enum VT2JT<bsls::Types::Int64   >::value = JT::e_NUMBER;
const Enum VT2JT<bsls::Types::Uint64  >::value = JT::e_NUMBER;
const Enum VT2JT<float                >::value = JT::e_NUMBER;
const Enum VT2JT<double               >::value = JT::e_NUMBER;
const Enum VT2JT<const char          *>::value = JT::e_STRING;
const Enum VT2JT<bsl::string          >::value = JT::e_STRING;
const Enum VT2JT<bdldfp::Decimal64    >::value = JT::e_NUMBER;

const Enum VT2JT<bdlt::Date           >::value = JT::e_STRING;
const Enum VT2JT<bdlt::Time           >::value = JT::e_STRING;
const Enum VT2JT<bdlt::Datetime       >::value = JT::e_STRING;
const Enum VT2JT<bdlt::DateTz         >::value = JT::e_STRING;
const Enum VT2JT<bdlt::TimeTz         >::value = JT::e_STRING;
const Enum VT2JT<bdlt::DatetimeTz     >::value = JT::e_STRING;

template <Enum JsonType>
struct AreEqualUtil {

    template <class TYPE>
    static bool areEqualTemporalValues(const Json& json,
                                       const TYPE& value);

    template <class TYPE>
    static bool areEqualFloatingPointValues(const Json& json,
                                            const TYPE& value);
    template <class TYPE>
    static bool areEqual(const Json& json,
                         const TYPE& value,
                         int         roundoffMicroseconds = 0);
};

template <Enum JsonType>
template <class TYPE>
bool AreEqualUtil<JsonType>::areEqualTemporalValues(const Json& json,
                                                    const TYPE& value)
{
    ASSERTV(JT::e_STRING,   json.type(),
            JT::e_STRING == json.type());

    TYPE jsonValue;

    bsl::string valueAsString(json.theString());

    ASSERTV(valueAsString, 2   <= valueAsString.size());
    ASSERTV(valueAsString, '"' != valueAsString.front());
    ASSERTV(valueAsString, '"' != valueAsString.back());

    valueAsString.insert(static_cast<bsl::size_t>(0), 1, '"');
    valueAsString.insert(valueAsString.size()       , 1, '"');

    const int  rc = baljsn::ParserUtil::getValue(&jsonValue, valueAsString);
    if (0 != rc) {
        return false;                                                 // RETURN
    }
    return jsonValue == value;
}

template <>
template <class TYPE>
bool AreEqualUtil<JT::e_BOOLEAN>::areEqual(const Json& json,
                                           const TYPE& value,
                                           int         )
{
    ASSERTV(JT::e_BOOLEAN,   json.type(),
            JT::e_BOOLEAN == json.type());
    return json.theBoolean() == value;
}

template <>
template <class TYPE>
bool AreEqualUtil<JT::e_NUMBER>::areEqual(const Json& json,
                                          const TYPE& value,
                                          int         )
{
    switch (json.type()) {
      case JT::e_NUMBER: {
        TYPE      jsonValue;
        const int rc = baljsn::ParserUtil::getValue(&jsonValue,
                                                    json.theNumber().value());
        if (0 != rc) {
            return false;                                             // RETURN
        }
        return jsonValue == value;                                    // RETURN
      } break;
      case JT::e_STRING: {
        return AreEqualUtil<JT::e_STRING>::areEqualFloatingPointValues(json,
                                                                       value);
                                                                      // RETURN
      } break;
      default: {
        ASSERTV(json.type(), false && "reached");
      } break;
    }
    BSLA_UNREACHABLE;
}

template <>
template <class TYPE>
bool AreEqualUtil<JT::e_STRING>::areEqual(const Json& json,
                                          const TYPE& value,
                                          int         )
{
    ASSERTV(JT::e_STRING,   json.type(),
            JT::e_STRING == json.type());
    return json.theString() == value;
}

template <>
template <class TYPE>
bool AreEqualUtil<JT::e_STRING>::areEqualFloatingPointValues(const Json& json,
                                                             const TYPE& value)
{
    ASSERTV(JT::e_STRING,   json.type(),
            JT::e_STRING == json.type());

    bsl::string valueAsString(json.theString());

    ASSERTV(valueAsString, 2   <= valueAsString.size());
    ASSERTV(valueAsString, '"' != valueAsString.front());
    ASSERTV(valueAsString, '"' != valueAsString.back());

    valueAsString.insert(static_cast<bsl::size_t>(0), 1, '"');
    valueAsString.insert(valueAsString.size()       , 1, '"');

    TYPE jsonValue;
    const int  rc = baljsn::ParserUtil::getValue(&jsonValue, valueAsString);
    ASSERTV(rc, 0 == rc);
    if (0 != rc) {

        return false;                                                 // RETURN
    }
    return jsonValue == value;
}

template <>
template <>
bool AreEqualUtil<JT::e_STRING>::areEqual<bdlt::Date>(const Json&       json,
                                                      const bdlt::Date& value,
                                                      int               )
{
    return areEqualTemporalValues(json, value);
}

template <>
template <>
bool AreEqualUtil<JT::e_STRING>::areEqual<bdlt::Time>(
                                        const Json&       json,
                                        const bdlt::Time& value,
                                        int               roundoffMicroseconds)
{
    bdlt::Time adjustedValue(value);
    adjustedValue.addMicroseconds(roundoffMicroseconds);
    return areEqualTemporalValues(json, adjustedValue);
}

template <>
template <>
bool AreEqualUtil<JT::e_STRING>::areEqual<bdlt::Datetime>(
                                    const Json&           json,
                                    const bdlt::Datetime& value,
                                    int                   roundoffMicroseconds)
{
    bdlt::Datetime adjustedValue(value);
    adjustedValue.addMicroseconds(roundoffMicroseconds);
    return areEqualTemporalValues(json, adjustedValue);
}

template <>
template <>
bool AreEqualUtil<JT::e_STRING>::areEqual<bdlt::DateTz>(
                                                     const Json&         json,
                                                     const bdlt::DateTz& value,
                                                     int                 )
{
    return areEqualTemporalValues(json, value);
}

template <>
template <>
bool AreEqualUtil<JT::e_STRING>::areEqual<bdlt::TimeTz>(
                                      const Json&         json,
                                      const bdlt::TimeTz& value,
                                      int                 roundoffMicroseconds)
{
    bdlt::Time   adjustedTime(value.localTime());
    adjustedTime.addMicroseconds(roundoffMicroseconds);
    bdlt::TimeTz adjustedValue(adjustedTime, value.offset());
    return areEqualTemporalValues(json, adjustedValue);
}

template <>
template <>
bool AreEqualUtil<JT::e_STRING>::areEqual<bdlt::DatetimeTz>(
                                  const Json&             json,
                                  const bdlt::DatetimeTz& value,
                                  int                     roundoffMicroseconds)
{
    bdlt::DatetimeTz adjustedValue(value.localDatetime().addMicroseconds(
                                                         roundoffMicroseconds),
                                   value.offset());
    return areEqualTemporalValues(json, adjustedValue);
}

template <class TYPE>
void testPutValue(int            line,
                  const TYPE&    value,
                  const Options *options,
                  bool           isValid,
                  int            roundoffMicroseconds = 0)
{
    if (g_veryVeryVerbose) {
        Q(testPutValue: enter)
        P(line);
        P(bsls::NameOf<TYPE>());
        P(value);
        P(isValid);
    }

    Json json;
    Obj mX(&json);

    if (isValid) {
        const int rc = mX.putValue(value, options);
        ASSERTV(rc, 0 == rc);

        if (g_veryVeryVerbose) {
            P(json.type());
        }

        ASSERTV(                           json, value, roundoffMicroseconds,
        AreEqualUtil<VT2JT<TYPE>::value>::
                                  areEqual(json, value, roundoffMicroseconds));

        if (g_veryVeryVerbose) {
            P(json.type());
            P(json);
        }

        if (g_veryVeryVerbose) cout << "Negative Testing." << endl;
        {
            bsls::AssertTestHandlerGuard hG;
            ASSERT_OPT_FAIL(mX.putValue(value, options));
        }

    } else {
        Enum expectedType = VT2JT<TYPE>::value;

        switch (expectedType) {
          case JT::e_NUMBER: {
            // Handle the two invalid data points for `float` and `double`.

            ASSERTV(options->encodeInfAndNaNAsStrings(),
                    options->encodeInfAndNaNAsStrings() == false);

            const int rc = mX.putValue(value, options);
            ASSERTV(rc, 0 != rc);
          } break;
          case JT::e_STRING: {
            // Handle the two invalid data points for `const char *` and
            // `bsl::string`.

            const int rc = mX.putValue(value, options);
            ASSERTV(rc, 0 != rc);
          } break;
          default: {
            ASSERTV(expectedType, false && "reached");
          } break;
        }
    }
}

// ============================================================================
//                               MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int                 test = argc > 1 ? atoi(argv[1]) : 0;
    bool             verbose = argc > 2;
    bool         veryVerbose = argc > 3;
    bool     veryVeryVerbose = argc > 4; g_veryVeryVerbose = veryVeryVerbose;
    bool veryVeryVeryVerbose = argc > 5;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    // CONCERN: `BSLS_REVIEW` failures should lead to test failures.
    bsls::ReviewFailureHandlerGuard reviewGuard(&bsls::Review::failByAbort);

    switch (test) { case 0:
      case 6: {
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

        if (verbose) cout << endl << "USAGE EXAMPLE" << endl
                                  << "=============" << endl;
///Usage
///-----
// This section illustrates intended use of this component.
//
///Basic Syntax
/// - - - - - -
// Let us say that we have a JSON document describing some (hypothetical)
// employee data that we wish to convert to a `bdljsn::Json` object so we can
// examine and manipulate that data programmatically.
//
// ```
//  { "name"        : "Bob"
//  , "homeAddress" : { "street" : "Lexington Ave"
//                    , "city"   : "New York City"
//                    , "state"  : "New York"
//                    }
//  , "age"        :  21
//  }
// ```
// First, we create a `bdljsn::Json` object and directly use the manipulators
// provided by that class.
// ```
    bdljsn::Json json1;
    json1.makeObject();
    json1["name"];
    json1["homeAddress"];
    json1["age"];

    bdljsn::Json address;
    address.makeObject();
    address["street"] = "Lexington Ave";
    address["city"]   = "New York City";
    address["state"]  = "New York";

    json1["name"]        = "Bob";
    json1["homeAddress"] = address;
    json1["age"]         = 21;
// ```
//  Notice that, since we have the freedom to do so, we choose to assemble
//  `json1` in a breadth-first order:
//
//  1. First we enter the top-level members by name, `"name"`,
//     `"homeAddress`", and `"age"`.
//  2. Then we assign values to each of those members.  In the case of
//     `"homeAddress"`, we use a separately assembled object.
//
// Now, we confirm that we can also assemble an equivalent object using
// the `baljsn::JsonFormatter` mechanism.  Note that this time our order
// of assembly is (must be) depth first.
// ```
    bdljsn::Json          json2;
    baljsn::JsonFormatter formatter(&json2);

    formatter.openObject();

        formatter.openMember("name");
        formatter.putValue("Bob");
        formatter.closeMember();

        formatter.openMember("homeAddress");
        formatter.openObject();  // The "value" of the "homeAddress" member.

            formatter.openMember("street");
            formatter.putValue("Lexington Ave");
            formatter.closeMember();

            formatter.openMember("city");
            formatter.putValue("New York City");
            formatter.closeMember();

            formatter.openMember("state");
            formatter.putValue("New York");
            formatter.closeMember();

        formatter.closeObject();
        formatter.closeMember(); // "homeAddress"

        formatter.openMember("age");
        formatter.putValue(21);
        formatter.closeMember();

       formatter.closeObject();
// ```
// Finally, we confirm that the two assembled objects are equal.
//
// ```
    ASSERT(json1 == json2);
// ```
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING `putNullValue`
        //
        // Concerns:
        // 1. The `putNullValue` method outputs leaves a the `bdljsn::Json`
        //    object having type `bdljsn::JsonType::e_NULL` (the default
        //    state),
        //
        // 2. The `putNullValue` method can be used in the scalar, array, and
        //    object contexts.
        //
        // 3. QoI: Asserted precondition violations are detected when enabled.
        //
        //    * In particular, `putValue` method cannot be used to reset
        //      a value in the scalar or object contexts.  In array context,
        //
        // Plan:
        // 1. The concerns are addressed in a short sequence of ad hoc tests
        //    that compare the results of using `putNullValue` with an
        //    "expected" `bdljsn::Json` object created using its native
        //    methods.
        //
        // Tests:
        //   void putNullValue();
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING `putNullValue`" << endl
                          << "======================" << endl;

        if (veryVerbose)
                   cout << "Testing `putNullValue` in scalar context." << endl;
        {
            Json mJson, mJsonExpected;
            Obj mX(&mJson);
            mX.putNullValue();
            ASSERTV(mJson,   mJsonExpected,
                    mJson == mJsonExpected);

            if (veryVeryVerbose) cout << "Negative Testing: scalar." << endl;
            {
                bsls::AssertTestHandlerGuard hG;
                ASSERT_OPT_FAIL(mX.putNullValue());
            }
        }

        if (veryVerbose)
                    cout << "Testing `putNullValue` in array context." << endl;
        {
            Json mJsonExpected;
            mJsonExpected.makeArray();
            mJsonExpected.theArray().pushBack(bdljsn::Json());

            Json mJson;
            Obj mX(&mJson);
            mX.openArray();
            mX.putNullValue();
            mX.closeArray();

            ASSERTV(mJson,   mJsonExpected,
                    mJson == mJsonExpected);

            if (veryVeryVerbose) cout << "Negative Testing: array" << endl;
            {
                bsls::AssertTestHandlerGuard hG;
                Json mJson;  const Json& cJson = mJson;
                Obj  mX(&mJson);
                mX.openArray();
                mX.putNullValue();
                ASSERT_SAFE_PASS(mX.putNullValue()); // OK.  Adds 2nd element.
                ASSERTV(     cJson.theArray().size(),
                        2 == cJson.theArray().size());
            }
        }

        if (veryVerbose)
                   cout << "Testing `putNullValue` in object context." << endl;
        {
            Json mJsonExpected;
            mJsonExpected.makeObject();
            mJsonExpected["Here is a null value."] = bdljsn::JsonNull();

            Json mJson;
            Obj  mX(&mJson);
            mX.openObject();
                mX.openMember("Here is a null value.");
                mX.putNullValue();
                mX.closeMember();
            mX.closeObject();

            ASSERTV(mJson,   mJsonExpected,
                    mJson == mJsonExpected);

            if (veryVeryVerbose) cout << "Negative Testing: object" << endl;
            {
                bsls::AssertTestHandlerGuard hG;
                Json mJson;
                Obj  mX(&mJson);
                mX.openObject();
                mX.openMember("someName");
                mX.putNullValue();
                // Now, try to set the value again.
                ASSERT_OPT_FAIL(mX.putNullValue());
            }
        }
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING OJBECT METHODS
        //
        // Concerns:
        // 1. The "value" portion of a member can be:
        //    1. Any supported scalar type.
        //    2. An other object.
        //    3. An array.
        //
        // 2. `openMember` returns a non-zero error code when `name` has a
        //    non-UTF8 character.
        //
        // Plan:
        // 1. Construct `bdljsn::Json` objects using native methods and
        //    using the methods of `baljsn::JsonFormatter`.  Compare for
        //    equality.
        //
        //    * Several types (e.g., `bdlt::Date, bdlt::Time`) are excluded
        //      from this test since those have no native support.
        //
        // Tests:
        //   int openMember();
        //   int closeMember();
        //   void openObject();
        //   void closeObject();
        //   CONCERN: `putValue` IN OBJECT CONTEXT
        //   CONCERN: OBJECT AS MEMBER VALUE
        //   CONCERN: ARRAY  AS MEMBER VALUE
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING OBJECT METHODS" << endl
                          << "======================" << endl;

        if (veryVerbose) cout << "`putValue` IN OBJECT CONTEXT" << endl;
        {
            Json expectedObject;
            expectedObject.makeObject();
            expectedObject.theObject()["A"]  = Json(A);
            expectedObject.theObject()["B"]  = Json(B);
            expectedObject.theObject()["C"]  = Json(C);
            expectedObject.theObject()["D"]  = Json(D);
            expectedObject.theObject()["E"]  = Json(E);
            expectedObject.theObject()["F"]  = Json(F);
            expectedObject.theObject()["G"]  = Json(G);
            expectedObject.theObject()["H"]  = Json(H);
            expectedObject.theObject()["I"]  = Json(I);
            expectedObject.theObject()["J"]  = Json(J);
            expectedObject.theObject()["K"]  = Json(K);
            expectedObject.theObject()["L"]  = Json(L);
            expectedObject.theObject()["M"]  = Json(M);
            expectedObject.theObject()["N"]  = Json(N);
            expectedObject.theObject()["O"]  = Json(O);
        //  expectedObject.theObject()["PA"] = Json(PA);
        //  expectedObject.theObject()["QA"] = Json(QA);
        //  expectedObject.theObject()["R"]  = Json(R);
        //  expectedObject.theObject()["S"]  = Json(S);
        //  expectedObject.theObject()["T"]  = Json(T);
        //  expectedObject.theObject()["U"]  = Json(U);

            Json json;
            Obj  obj(&json);
            obj.openObject();
            obj.openMember("A");  obj.putValue(A);  obj.closeMember();
            obj.openMember("B");  obj.putValue(B);  obj.closeMember();
            obj.openMember("C");  obj.putValue(C);  obj.closeMember();
            obj.openMember("D");  obj.putValue(D);  obj.closeMember();
            obj.openMember("E");  obj.putValue(E);  obj.closeMember();
            obj.openMember("F");  obj.putValue(F);  obj.closeMember();
            obj.openMember("G");  obj.putValue(G);  obj.closeMember();
            obj.openMember("H");  obj.putValue(H);  obj.closeMember();
            obj.openMember("I");  obj.putValue(I);  obj.closeMember();
            obj.openMember("J");  obj.putValue(J);  obj.closeMember();
            obj.openMember("K");  obj.putValue(K);  obj.closeMember();
            obj.openMember("L");  obj.putValue(L);  obj.closeMember();
            obj.openMember("M");  obj.putValue(M);  obj.closeMember();
            obj.openMember("N");  obj.putValue(N);  obj.closeMember();
            obj.openMember("O");  obj.putValue(O);  obj.closeMember();
        //  obj.openMember("PA"); obj.putValue(PA); obj.closeMember();
        //  obj.openMember("QA"); obj.putValue(QA); obj.closeMember();
        //  obj.openMember("R");  obj.putValue(R);  obj.closeMember();
        //  obj.openMember("S");  obj.putValue(S);  obj.closeMember();
        //  obj.openMember("T");  obj.putValue(T);  obj.closeMember();
        //  obj.openMember("U");  obj.putValue(U);  obj.closeMember();
            obj.closeObject();

            ASSERTV(expectedObject,   json,
                    expectedObject == json);
        }

        if (veryVerbose) cout << "OBJECT AS OBJECT VALUE" << endl;
        {
            // Create { "objectOfObject": {} }
            Json emptyObject;       emptyObject.makeObject();
            Json objectOfObject; objectOfObject.makeObject();
            objectOfObject.theObject()["objectOfObject"] = emptyObject;

            // TEST { "objectOfObject": {} }
            Json json;
            Obj  obj(&json);
            obj.openObject();
                obj.openMember("objectOfObject");
                obj.openObject();
                obj.closeObject();
                obj.closeMember();
            obj.closeObject();

            ASSERTV(objectOfObject,   json,
                    objectOfObject == json);
        }

        if (veryVerbose) cout << "ARRAY AS OBJECT VALUE" << endl;
        {
            // Create { "objectOfArray": [] }
            Json emptyArray;       emptyArray.makeArray();
            Json objectOfArray; objectOfArray.makeObject();
            objectOfArray.theObject()["objectOfArray"] = emptyArray;

            // TEST { "objectOfArray": {} }
            Json json;
            Obj  obj(&json);
            obj.openObject();
                obj.openMember("objectOfArray");
                obj.openArray();
                obj.closeArray();
                obj.closeMember();
            obj.closeObject();

            ASSERTV(objectOfArray,   json,
                    objectOfArray == json);
        }

        if (veryVerbose) cout << "TEST `openMember` RETURN VALUE" << endl;
        {
            Json json;
            Obj  obj(&json);

            obj.openObject();

                int rcOK = obj.openMember("hello");
                ASSERTV(rcOK, 0 == rcOK);

                obj.putValue("world");
                obj.closeMember();

                int rcNG = obj.openMember(INV4); // Has non-UTF8 characters
                ASSERTV(rcNG, 0 != rcNG);

           obj.closeObject();

           ASSERTV(json.size(), 1 == json.size());
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING ARRAY METHODS: `openArray` AND ` closeArray`
        //
        // Concerns:
        // 1. All of the supported types can be set as array elements using
        //    `putValue`.
        //
        // 2. `openArray` and `closeArray` can create array elements that are
        //    themselves arrays.
        //
        // Plan:
        // 1. Construct `bdljsn::Json` objects using native methods and
        //    using the methods of `baljsn::JsonFormatter`.  Compare for
        //    equality.
        //
        //    * Several types (e.g., `bdlt::Date, bdlt::Time`) are excluded
        //      from this test since those have no native support.
        //
        // Testing:
        //   void openArray();
        //   void closeArray();
        //   CONCERN: `putValue` IN ARRAY  CONTEXT
        //   CONCERN: NESTED ARRAYS
        //   CONCERN: OBJECT IN ARRAY
        // --------------------------------------------------------------------

        if (verbose) cout
              << endl
              << "TESTING ARRAY METHODS: `openArray` AND `closeArray`" << endl
              << "===================================================" << endl;

        if (veryVerbose) cout << "`putValue` IN ARRAY CONTEXT" << endl;
        {
            Json expectedArray;
            expectedArray.makeArray();
            expectedArray.theArray().pushBack(Json(A));
            expectedArray.theArray().pushBack(Json(B));
            expectedArray.theArray().pushBack(Json(C));
            expectedArray.theArray().pushBack(Json(D));
            expectedArray.theArray().pushBack(Json(E));
            expectedArray.theArray().pushBack(Json(F));
            expectedArray.theArray().pushBack(Json(G));
            expectedArray.theArray().pushBack(Json(H));
            expectedArray.theArray().pushBack(Json(I));
            expectedArray.theArray().pushBack(Json(J));
            expectedArray.theArray().pushBack(Json(K));
            expectedArray.theArray().pushBack(Json(L));
            expectedArray.theArray().pushBack(Json(M));
            expectedArray.theArray().pushBack(Json(N));
            expectedArray.theArray().pushBack(Json(O));
        //  expectedArray.theArray().pushBack(Json(QA));
        //  expectedArray.theArray().pushBack(Json(R));
        //  expectedArray.theArray().pushBack(Json(S));
        //  expectedArray.theArray().pushBack(Json(T));
        //  expectedArray.theArray().pushBack(Json(U));

            Json json;
            Obj  obj(&json);
            obj.openArray();
            obj.putValue(A);
            obj.putValue(B);
            obj.putValue(C);
            obj.putValue(D);
            obj.putValue(E);
            obj.putValue(F);
            obj.putValue(G);
            obj.putValue(H);
            obj.putValue(I);
            obj.putValue(J);
            obj.putValue(K);
            obj.putValue(L);
            obj.putValue(M);
            obj.putValue(N);
            obj.putValue(O);
        //  obj.putValue(PA);
        //  obj.putValue(QA);
        //  obj.putValue(R);
        //  obj.putValue(S);
        //  obj.putValue(T);
        //  obj.putValue(U);
            obj.closeArray();

            ASSERTV(expectedArray,   json,
                    expectedArray == json);
        }

        if (veryVerbose) cout << "CONCERN: NESTED ARRAYS" << endl;
        {
            Json emptyArray;
            emptyArray.makeArray();

            // Create []
            Json array1 = emptyArray;
            ASSERTV(     array1.theArray().size(),
                    0 == array1.theArray().size());

            // Create [[]]
            Json array2 = array1;
            array2.theArray().pushBack(emptyArray);

            ASSERTV(     array2.theArray()   .size(),
                    1 == array2.theArray()   .size());
            ASSERTV(     array2.theArray()[0].size(),
                    0 == array2.theArray()[0].size());

            // Create [[]]
            Json array3 = array2;
            array3.theArray()[0].theArray().pushBack(emptyArray);

            ASSERTV(     array3.theArray()                 .size(),
                    1 == array3.theArray()                 .size());
            ASSERTV(     array3.theArray()[0]              .size(),
                    1 == array3.theArray()[0]              .size());
            ASSERTV(     array3.theArray()[0].theArray()[0].size(),
                    0 == array3.theArray()[0].theArray()[0].size());

            // Test []
            Json json1;
            Obj  obj1(&json1);

            obj1.openArray();
            obj1.closeArray();

            ASSERTV(array1,   json1,
                    array1 == json1);

            // Test [[]]
            Json json2;
            Obj  obj2(&json2);

            obj2.openArray();
            obj2.openArray();
            obj2.closeArray();
            obj2.closeArray();

            ASSERTV(array2,   json2,
                    array2 == json2);

            // Test [[[]]]
            Json json3;
            Obj  obj3(&json3);

            obj3.openArray();
            obj3.openArray();
            obj3.openArray();
            obj3.closeArray();
            obj3.closeArray();
            obj3.closeArray();

            ASSERTV(array3,   json3,
                    array3 == json3);
        }

        if (veryVerbose) cout << "CONCERN: OBJECT IN ARRAY" << endl;
        {
            // Create [{}]
            Json arrayOfObject;
            Json element; element.makeObject();

            arrayOfObject.makeArray();
            arrayOfObject.theArray().pushBack(element);

            // Test [{}]
            Json json;
            Obj  obj(&json);
            obj.openArray();
            obj.openObject();
            obj.closeObject();
            obj.closeArray();

            ASSERTV(arrayOfObject,   json,
                    arrayOfObject == json);
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING `putValue` IN SCALAR CONTEXT
        //
        // Concerns:
        // 1. The `putValue` method can set to a `bdljsn::Json` object in a
        //    scalar context each of the 21 types that can be expected from a
        //    `bdlat` object.
        //
        // 2. The `putValue` method leaves the `bdljsn::Json` object in the
        //    having the expected JSON type (e.g., JSON string, JSON number)
        //    and value.
        //
        // 3. Setting a `float or `double` holding either INF or NAN value
        //    results in an error unless the `setEncodeInfAndNaNAsStrings`
        //    option is `true`.  In that case, the value is represented as
        //    a JSON string, not a JSON number.
        //
        // 4. Datetime and time types are represented as JSON strings and have
        //    a number of decimal digits corresponding to the
        //    `setDatetimeFractionalSecondPrecision` option.
        //
        // 5. QoI: Asserted precondition violations are detected when enabled.
        //
        //    * In particular, attempts to call `putValue` more than once for a
        //      JSON scalar are disallowed.
        //
        // Plan:
        // 1. For all the supported data types create atleast one valid value
        //    and an invalid value (if an invalid value exists) and invoke
        //    `putValue` on them.
        //
        // 2. Confirm that `putValue` returns 0 and correctly encodes the valid
        //    values and returns a non-zero values for invalid values.
        //
        // Testing:
        //   int putValue(const TYPE& value, const EncoderOptions *options);
        //   `putValue` IN SCALAR CONTEXT.
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING `putValue` IN SCALAR CONTEXT" << endl
                          << "====================================" << endl;

        const Options DO;  const Options *DP = &DO;

        testPutValue(L_, A,    DP,   true);
        testPutValue(L_, B,    DP,   true);
        testPutValue(L_, C,    DP,   true);
        testPutValue(L_, D,    DP,   true);
        testPutValue(L_, E,    DP,   true);
        testPutValue(L_, F,    DP,   true);
        testPutValue(L_, G,    DP,   true);
        testPutValue(L_, H,    DP,   true);
        testPutValue(L_, I,    DP,   true);
        testPutValue(L_, J,    DP,   true);
        testPutValue(L_, K,    DP,   true);
        testPutValue(L_, L,    DP,   true);
        testPutValue(L_, M,    DP,   true);
        testPutValue(L_, N,    DP,   true);
        testPutValue(L_, O,    DP,   true);
        testPutValue(L_, PA,   DP,   true);
        testPutValue(L_, QA,   DP,   true, roundoffMicroseconds); // Time
        testPutValue(L_, R,    DP,   true, roundoffMicroseconds); // Datetime
        testPutValue(L_, S,    DP,   true);
        testPutValue(L_, T,    DP,   true, roundoffMicroseconds); // TimeTz
        testPutValue(L_, U,    DP,   true, roundoffMicroseconds); // DatetimeTz

        testPutValue(L_, INV1, DP,   false);
        testPutValue(L_, INV2, DP,   false);
        testPutValue(L_, INV3, DP,   false);
        testPutValue(L_, INV4, DP,   false);

        Options opts;  const Options *OPTS = &opts;

        opts.setEncodeInfAndNaNAsStrings(true);
        testPutValue(L_, INV1, OPTS, true);
        testPutValue(L_, INV2, OPTS, true);

        opts.setDatetimeFractionalSecondPrecision(6);
        testPutValue(L_, PA,   OPTS, true);
        testPutValue(L_, QA,   OPTS, true);
        testPutValue(L_, R,    OPTS, true);
        testPutValue(L_, S,    OPTS, true);
        testPutValue(L_, T,    OPTS, true);
        testPutValue(L_, U,    OPTS, true);

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // TESTING CREATORS AND NO-OP METHODS
        //
        // Concerns:
        // 1. Creation of a `JsonFormatter` does not change the state of the
        //    target `bldjsn::Json` object.
        //
        // 2. The created `JsonFormatter` object uses the expected allocator.
        //
        // 3. The two "no-op" methods, `addArrayElementSeparator` and
        //    `nextinDepth`, do not modify the target object.
        //
        // Plan:
        // 1. Use the "footprint" idiom to confirm that each of the three
        //    constructors creates and object that uses the expected allocator.
        //
        // 2. Invoke each of the "no-ops" methods and confirm that the
        //    `bdljsn::Json` object held by the object under test remains in
        //    the default state.
        //
        // Testing:
        //  Formatter(bdljsn::Json *json, bslma::Allocator *bA = 0);
        //  ~Formatter();
        //  void addArrayElementSeparator();
        //  int nestingDepth() const;
        //  allocator_type get_allocator() const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING CREATORS AND NO-OP METHODS" << endl
                          << "==================================" << endl;

            for (char cfg = 'a'; cfg <= 'd'; ++cfg) {
                const char CONFIG = cfg;  // How we specify the allocator.

                if (veryVerbose) {
                    P(CONFIG);
                }

                bslma::TestAllocator fa("footprint", veryVeryVeryVerbose);

                bslma::TestAllocator sa("supplied",  veryVeryVeryVerbose);
                bslma::TestAllocator da("default",   veryVeryVeryVerbose);

                bslma::DefaultAllocatorGuard dag(&da);

                Json target;

                Obj *objPtr =
               'a' == CONFIG ? new (fa) Obj(&target)                          :
               'b' == CONFIG ? new (fa) Obj(&target, Obj::allocator_type())   :
               'c' == CONFIG ? new (fa) Obj(&target, &sa)                     :
               'd' == CONFIG ? new (fa) Obj(&target, Obj::allocator_type(&sa)):
               /* default */   0                                              ;

                ASSERT(objPtr);

                bslma::TestAllocator *objAllocatorPtr = 'a' == CONFIG ? &da :
                                                        'b' == CONFIG ? &da :
                                                        'c' == CONFIG ? &sa :
                                                        'd' == CONFIG ? &sa :
                                                        /* default */   0   ;
                ASSERT(objAllocatorPtr);

                Obj&                   mX = *objPtr;  const Obj& X = mX;
                bslma::TestAllocator&  oa = *objAllocatorPtr;
                bslma::TestAllocator& noa = (&da == &oa) ? sa : da;

                // -----------------------------------------------------------

                ASSERTV(CONFIG,
                        &oa ==   X.get_allocator());

                ASSERTV(CONFIG,  oa.numBlocksTotal(),
                        2   ==   oa.numBlocksTotal());

                ASSERTV(CONFIG, noa.numBlocksTotal(),
                        0   ==  noa.numBlocksTotal());

                ASSERTV(CONFIG, Json() == target);

                mX.addArrayElementSeparator();                          // TEST

                ASSERTV(CONFIG, Json() == target);

                ASSERTV(CONFIG, X.nestingDepth(),
                        0    == X.nestingDepth());                      // TEST

                ASSERTV(CONFIG, Json() == target);

                // -----------------------------------------------------------

                fa.deleteObject(objPtr);
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
// Copyright 2025 Bloomberg Finance L.P.
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
