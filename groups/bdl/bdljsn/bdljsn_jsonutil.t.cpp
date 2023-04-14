// bdljsn_jsonutil.t.cpp                                              -*-C++-*-
#include <bdljsn_jsonutil.h>

#include <bdljsn_error.h>
#include <bdljsn_readoptions.h>
#include <bdljsn_writeoptions.h>
#include <bdljsn_writestyle.h>

#include <bdlde_utf8util.h>
#include <bdldfp_decimal.h>
#include <bdlsb_fixedmeminstreambuf.h>
#include <bdlsb_fixedmemoutstreambuf.h>
#include <bdlsb_memoutstreambuf.h>

#include <bdlt_date.h>
#include <bdlt_datetime.h>
#include <bdlt_time.h>

#include <bslim_fuzzdataview.h>
#include <bslim_fuzzutil.h>
#include <bslim_testutil.h>

#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>         // to verify that we do not
#include <bslma_testallocatormonitor.h>  // allocate any memory

#include <bslmf_assert.h>

#include <bsls_asserttest.h>
#include <bsls_compilerfeatures.h>
#include <bsls_fuzztest.h>
#include <bsls_platform.h>
#include <bsls_review.h>

#include <bsl_cstdlib.h>
#include <bsl_iostream.h>
#include <bsl_sstream.h>
#include <bsl_string.h>

using namespace BloombergLP;
using bsl::cerr;
using bsl::cout;
using bsl::endl;
using bsl::ends;

// ============================================================================
//                             TEST PLAN
// ----------------------------------------------------------------------------
//                             Overview
//                             --------
// The component under test implements a utility for reading and writing 'Json'
// objects from/to various stream or string-like sources or destinations.
// Since the functions are independent and do not share any state we will test
// them independently.  There is one "main" overload each of 'read' and 'write'
// - these will be tested fully, and then the "delegating" overloads will be
// tested to make sure the arguments are forwarded and/or defaulted correctly.
//
// We use standard table-based approach to testing where we put both input and
// expected output in the same table row and verify that the actual result
// matches the expected value.

// ----------------------------------------------------------------------------
//                              CLASS METHODS
// [ 2] static int read(Json*, Error*, const string_view&, const ReadOptions&);
// [ 3] static int read(Json*, istream&);
// [ 3] static int read(Json*, istream&, const ReadOptions&);
// [ 3] static int read(Json*, streambuf*);
// [ 3] static int read(Json*, streambuf*, const ReadOptions&);
// [ 3] static int read(Json*, const string_view&);
// [ 3] static int read(Json*, const string_view&, const ReadOptions&);
// [ 3] static int read(Json*, Error*, istream&);
// [ 3] static int read(Json*, Error*, istream&, const ReadOptions&);
// [ 3] static int read(Json*, Error*, streambuf*);
// [ 3] static int read(Json*, Error*, streambuf*, const ReadOptions&);
// [ 3] static int read(Json*, Error*, const string_view&);
// [ 4] static int write(bsl::ostream&, const Json&, const WriteOptions&);
// [ 5] static int write(bsl::ostream&, const Json&);
// [ 5] static int write(bsl::streambuf*, const Json&);
// [ 5] static int write(bsl::streambuf*, const Json&, const WriteOptions&);
// [ 5] static int write(bsl::string*, const Json&);
// [ 5] static int write(bsl::string*, const Json&, const WriteOptions&);
// [ 5] static int write(std::pmr::string*, const Json&);
// [ 5] static int write(std::pmr::string*, const Json&, const WriteOptions&);
// [ 5] static int write(std::string*, const Json&);
// [ 5] static int write(std::string*, const Json&, const WriteOptions&);
// [ 6] static ostream& printError(ostream&, streambuf *, const Error&);
// [ 6] static ostream& printError(ostream&, istream&, const Error&);
// [ 6] static ostream& printError(ostream&, string_view&, const Error&);
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 7] USAGE EXAMPLE

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

#define WS "   \t       \n      \v       \f       \r       "

// ============================================================================
//                     NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------
#define ASSERT_PASS(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS(EXPR)
#define ASSERT_FAIL(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL(EXPR)

// ============================================================================
//                   MACROS FOR TESTING WORKAROUNDS
// ----------------------------------------------------------------------------

// ============================================================================
//                   GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------

typedef bdljsn::JsonUtil         Util;
typedef bdljsn::Json             Json;
typedef bdljsn::JsonArray        JsonArray;
typedef bdljsn::JsonNumber       JsonNumber;
typedef bdljsn::JsonObject       JsonObject;
typedef bdljsn::JsonType         JsonType;
typedef bdljsn::Error            Error;
typedef bdljsn::Location         Location;
typedef bdljsn::ReadOptions      ReadOptions;
typedef bdljsn::WriteOptions     WriteOptions;
typedef bdljsn::WriteStyle       WriteStyle;
typedef bdljsn::WriteStyle::Enum Style;

typedef bsls::Types::Uint64      Uint64;
typedef bsls::Types::Int64       Int64;

// Note that the members of the objects in this JSON are deliberately sorted.

const char *OLD_STYLE_SIMPLE_JSON =
"{\n"
"  \"array\": [\n"
"    3.1,\n"
"    11,\n"
"    \"abc\",\n"
"    true\n"
"  ],\n"
"  \"boolean\": true,\n"
"  \"date\": \"1970-01-01\",\n"
"  \"integer\": 10,\n"
"  \"number\": 2.1,\n"
"  \"object\": {\n"
"    \"boolean\": true,\n"
"    \"date\": \"1970-01-01\",\n"
"    \"integer\": 10,\n"
"    \"string\": \"abc\"\n"
"  },\n"
"  \"string\": \"abc\"\n"
"}";

#ifdef BSLS_COMPILERFEATURES_SUPPORT_RAW_STRINGS
const char *SIMPLE_JSON =
R"JSON({
  "array": [
    3.1,
    11,
    "abc",
    true
  ],
  "boolean": true,
  "date": "1970-01-01",
  "integer": 10,
  "number": 2.1,
  "object": {
    "boolean": true,
    "date": "1970-01-01",
    "integer": 10,
    "string": "abc"
  },
  "string": "abc"
})JSON";
// This assertion is validated at runtime in its own test case
// ASSERT(bsl::string(SIMPLE_JSON) == bsl::string(OLD_STYLE_SIMPLE_JSON));
#else
const char *SIMPLE_JSON = OLD_STYLE_SIMPLE_JSON;
#endif // def BSLS_COMPILERFEATURES_SUPPORT_RAW_STRINGS

// These otherwise standard test driver scaffolding variables are global in
// this test driver so they can be accessed in helper functions.
bool verbose;
bool veryVerbose;
bool veryVeryVerbose;
bool veryVeryVeryVerbose;

bool areEqual(const bsl::string_view& lhs, const bsl::string_view& rhs)
    // Return 'true' if the specified 'lhs' and 'rhs' compare equal, and print
    // diagnostics detailing the difference and return 'false' otherwise.
{
    if (lhs == rhs) {
        return true;                                                  // RETURN
    }
    if (lhs.size() != rhs.size()) {
        bsl::cout << "Strings have different lengths.\n";
    }
    else {
        bsl::string_view::const_iterator lhsIt = lhs.begin();
        bsl::string_view::const_iterator rhsIt = rhs.begin();
        for (; lhsIt != lhs.end(); ++lhsIt, ++rhsIt) {
            if (*lhsIt != *rhsIt) {
                bsl::cout << "Strings differ at position "
                          << int(&*lhsIt - lhs.data()) << " lhs='" << *lhsIt
                          << "' rhs='" << *rhsIt << "\n";
                break;
            }
        }
    }

    bsl::cout << "LHS:\n" << lhs << "\nRHS:\n" << rhs << "\n";

    return false;
}

JsonObject *addMemberObject(JsonObject *object, const bsl::string_view& name)
    // Add to the specified 'object' a member with the specified 'name' and a
    // value consisting of an empty 'JsonObject', and return a pointer to the
    // newly added 'JsonObject'.
{
    ASSERT(0 != object);

    bsl::pair<JsonObject::Iterator, bool> result =
                                       object->insert(make_pair(name, Json()));

    ASSERT(result.second);

    JsonObject &newObject=result.first->second.makeObject();

    return &newObject;
}

JsonObject *appendMemberObject(JsonArray *array)
    // Append to the specified 'array' a value consisting of an empty
    // 'JsonObject', and return a pointer to the newly added 'JsonObject'.
{
    ASSERT(0 != array);

    array->pushBack(Json());

    JsonObject &newObject=array->back().makeObject();

    return &newObject;
}

JsonArray *addMemberArray(JsonObject *object, const bsl::string_view& name)
    // Add to the specified 'object' a member with the specified 'name' and a
    // value consisting of an empty 'JsonArray', and return a pointer to the
    // newly added 'JsonArray'.
{
    ASSERT(0 != object);

    bsl::pair<JsonObject::Iterator, bool> result =
                                       object->insert(make_pair(name, Json()));

    ASSERT(result.second);

    JsonArray &newArray=result.first->second.makeArray();

    return &newArray;
}

JsonArray *appendMemberArray(JsonArray *array)
    // Append to the specified 'array' a value consisting of an empty
    // 'JsonArray', and return a pointer to the newly added 'JsonArray'.
{
    ASSERT(0 != array);

    array->pushBack(Json());

    JsonArray &newArray=array->back().makeArray();

    return &newArray;
}

JsonArray *appendNull(JsonArray *array)
    // Append to the specified 'array' a 'JsonNull' value, and return a pointer
    // to 'array'.
{
    ASSERT(0 != array);

    array->pushBack(Json());

    array->back().makeNull();

    return array;
}

JsonObject *appendNull(JsonObject *object, const bsl::string_view& name)
    // Append to the specified 'object' a 'JsonNull' value with the specified
    // 'name', and return a pointer to 'object'.
{
    ASSERT(0 != object);

    bsl::pair<JsonObject::Iterator, bool> result =
                                       object->insert(make_pair(name, Json()));

    ASSERT(result.second);

    result.first->second.makeNull();

    return object;
}

JsonArray *appendBoolean(JsonArray *array, bool value)
    // Append to the specified 'array' the specified boolean 'value', and
    // return a pointer to 'array'.
{
    ASSERT(0 != array);

    array->pushBack(Json());

    array->back().makeBoolean(value);

    return array;
}

JsonObject *appendBoolean(JsonObject              *object,
                          const bsl::string_view&  name,
                          bool                     value)
    // Append to the specified 'object' a the specified boolean 'value' with
    // the specified 'name', and return a pointer to 'object'.
{
    ASSERT(0 != object);

    bsl::pair<JsonObject::Iterator, bool> result =
                                       object->insert(make_pair(name, Json()));

    ASSERT(result.second);

    result.first->second.makeBoolean(value);

    return object;
}

JsonArray *appendNumber(JsonArray *array, const bsl::string_view& value)
    // Append to the specified 'array' the specified numerical 'value', and
    // return a pointer to 'array'.
{
    ASSERT(0 != array);

    array->pushBack(Json());

    array->back().makeNumber(JsonNumber(value));

    return array;
}

JsonObject *appendNumber(JsonObject              *object,
                         const bsl::string_view&  name,
                         const bsl::string_view&  value)
    // Append to the specified 'object' a the specified numerical 'value' with
    // the specified 'name', and return a pointer to 'object'.
{
    ASSERT(0 != object);

    bsl::pair<JsonObject::Iterator, bool> result =
                                       object->insert(make_pair(name, Json()));

    ASSERT(result.second);

    result.first->second.makeNumber(JsonNumber(value));

    return object;
}

JsonArray *appendString(JsonArray *array, const bsl::string_view& value)
    // Append to the specified 'array' the specified string 'value', and
    // return a pointer to 'array'.
{
    ASSERT(0 != array);

    array->pushBack(Json());

    array->back().makeString(value);

    return array;
}

JsonObject *appendString(JsonObject              *object,
                         const bsl::string_view&  name,
                         const bsl::string_view&  value)
    // Append to the specified 'object' a the specified string 'value' with the
    // specified 'name', and return a pointer to 'object'.
{
    ASSERT(0 != object);

    bsl::pair<JsonObject::Iterator, bool> result =
                                       object->insert(make_pair(name, Json()));

    ASSERT(result.second);

    result.first->second.makeString(value);

    return object;
}

void checkResult(int                     line,
                 const bsl::string_view& string,
                 const Json&             value,
                 bool                    isValid)
    // Using the specified 'line' to annotate messages, check that a 'Json'
    // 'read' from the specified 'string' matches the specified 'value' if the
    // specified 'isValid' is 'true', and that 'read' returns a non-0 result
    // otherwise with all correct error-handling behavior.
{
    if (veryVerbose) {
        T_ P_(line) P_(string) P_(value) P(isValid)
    }

    bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

    Json        json(&scratch);
    Error       err(&scratch);
    ReadOptions ro;

    int rc = Util::read(&json, &err, string, ro);

    if (0 == rc) {
        // Verify that success was expected.
        ASSERTV(line, string, isValid, isValid);
        // Verify that 'json.isArray()' matches 'value.isArray()'
        ASSERTV(line,
                json.isArray(),
                value.isArray(),
                value.isArray() == json.isArray());
        // Verify that 'json.isObject()' matches 'value.isObject()'
        ASSERTV(line,
                json.isObject(),
                value.isObject(),
                value.isObject() == json.isObject());
        // Verify that 'err.location()' is unset.
        ASSERTV(line,
                string,
                rc,
                err.location(),
                Location(0) == err.location());
        // Verify that 'err.message()' is unset.
        ASSERTV(line,
                string,
                rc,
                err.message(),
                err.message().length(),
                0 == err.message().length());
        // Verify that 'json' returns the expected 'value'.
        ASSERTV(line,
                string,
                value,
                json,
                value == json);
    }
    else {
        // Verify that failure was expected.
        ASSERTV(line, string, isValid, !isValid);
        // Verify that 'json' is unchanged on failure.
        ASSERTV(line, string, rc, true == json.isNull());
        // Verify that 'err.location()' is set.
        ASSERTV(line,
                string,
                rc,
                err.location(),
                Location(0) != err.location());
        // Verify that 'err.message()' is set.
        ASSERTV(line,
                string,
                rc,
                err.message(),
                err.message().length(),
                0 != err.message().length());
    }
}

// ============================================================================
//                              FUZZ TESTING
// ----------------------------------------------------------------------------
//                              Overview
//                              --------
// The following function, 'LLVMFuzzerTestOneInput', is the entry point for the
// clang fuzz testing facility.  See {http://bburl/BDEFuzzTesting} for details
// on how to build and run with fuzz testing enabled.
// ----------------------------------------------------------------------------

#ifdef BDE_ACTIVATE_FUZZ_TESTING
#define main test_driver_main
#endif

extern "C"
int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
    // Use the specified 'data' array of 'size' bytes as input to methods of
    // this component and return zero.
{
    const int test = 3;

    switch (test) { case 0:  // Zero is always the leading case.
      case 3: {
        static bsl::string randomChars(bslma::Default::globalAllocator());

        bslim::FuzzDataView fdv(data, size);

        // 'INT_MAX' saw stack overflow at 245 recursions of '[' on a Linux
        // machine.  Now set to a saner, but still generous, value.

        const int FUZZ_MAX_NESTED_DEPTH = 200;

        bool allowTrailingTextFlag = bslim::FuzzUtil::consumeBool(&fdv);
        int  maxNestedDepth        =
                           bslim::FuzzUtil::consumeNumberInRange<int>(
                                                        &fdv,
                                                        1,
                                                        FUZZ_MAX_NESTED_DEPTH);

        bdljsn::ReadOptions readOptions;
        readOptions.setAllowTrailingText(allowTrailingTextFlag);
        readOptions.setMaxNestedDepth(maxNestedDepth);

        bslim::FuzzUtil::consumeRandomLengthString(&randomChars,
                                                   &fdv,
                                                   fdv.length());

        bdlsb::FixedMemInStreamBuf input(randomChars.data(),
                                         randomChars.size());

        bdljsn::Json  result;
        bdljsn::Error errorDescription;

        int rc = bdljsn::JsonUtil::read(&result,
                                        &errorDescription,
                                        &input,
                                        readOptions);
        if (0 == rc) {
            ASSERTV(errorDescription, bdljsn::Error() == errorDescription);
        } else {
            ASSERTV(errorDescription, bdljsn::Error() != errorDescription);
        }

        randomChars.clear();  // retain capacity
      } break;
      default: {
      } break;
    }

    if (testStatus > 0) {
        BSLS_ASSERT_INVOKE("FUZZ TEST FAILURES");
    }

    return 0;
}

// ============================================================================
//                                 MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;

    // These otherwise standard test driver scaffolding variables are global in
    // this test driver so they can be accessed in helper functions.
    verbose             = argc > 2;
    veryVerbose         = argc > 3;
    veryVeryVerbose     = argc > 4;
    veryVeryVeryVerbose = argc > 5;

    (void)veryVerbose; (void) veryVeryVerbose; (void) veryVeryVeryVerbose;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    // CONCERN: 'BSLS_REVIEW' failures should lead to test failures.
    bsls::ReviewFailureHandlerGuard reviewGuard(&bsls::Review::failByAbort);

    // CONCERN: In no case does memory come from the global allocator.

    bslma::TestAllocator globalAllocator("global", veryVeryVeryVerbose);
    bslma::Default::setGlobalAllocator(&globalAllocator);

    switch (test) {
      case 0:
      case 7: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //   First usage example extracted from component header file.
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
            cout << "\nUSAGE EXAMPLE"
                 << "\n=============" << endl;
        if (verbose)
            cout << "Testing usage example 1" << endl;
        {
///Example 1: Reading and Writing JSON Data
/// - - - - - - - - - - - - - - - - - - - -
// This component provides methods for reading and writing JSON data to/from
// 'Json' objects.
//
// First, we define the JSON data we plan to read:
//..
   const char *OLD_STYLE_INPUT_JSON =
   "{\n"
   "  \"a boolean\": true,\n"
   "  \"a date\": \"1970-01-01\",\n"
   "  \"a number\": 2.1,\n"
   "  \"an integer\": 10,\n"
   "  \"array of values\": [\n"
   "    -1,\n"
   "    0,\n"
   "    2.718281828459045,\n"
   "    3.1415926535979,\n"
   "    \"abc\",\n"
   "    true\n"
   "  ],\n"
   "  \"event\": {\n"
   "    \"date\": \"1969-07-16\",\n"
   "    \"description\": \"Apollo 11 Moon Landing\",\n"
   "    \"passengers\": [\n"
   "      \"Neil Armstrong\",\n"
   "      \"Buzz Aldrin\"\n"
   "    ],\n"
   "    \"success\": true\n"
   "  }\n"
   "}";

#ifdef BSLS_COMPILERFEATURES_SUPPORT_RAW_STRINGS
    const char *INPUT_JSON = R"JSON({
  "a boolean": true,
  "a date": "1970-01-01",
  "a number": 2.1,
  "an integer": 10,
  "array of values": [
    -1,
    0,
    2.718281828459045,
    3.1415926535979,
    "abc",
    true
  ],
  "event": {
    "date": "1969-07-16",
    "description": "Apollo 11 Moon Landing",
    "passengers": [
      "Neil Armstrong",
      "Buzz Aldrin"
    ],
    "success": true
  }
})JSON";
    ASSERT(areEqual(INPUT_JSON, OLD_STYLE_INPUT_JSON));
#else
    const char *INPUT_JSON = OLD_STYLE_INPUT_JSON;
#endif // def BSLS_COMPILERFEATURES_SUPPORT_RAW_STRINGS

//..
// Next, we read the JSON data into a 'Json' object:
//..
   bdljsn::Json        result;
   bdljsn::Error       error;

   int rc = bdljsn::JsonUtil::read(&result, &error, INPUT_JSON);

   ASSERT(0 == rc);

   if (0 != rc) {
       bsl::cout << "Error message: \"" << error.message() << "\""
                 << bsl::endl;
   }

//..
// Then, we check the values of a few selected fields:
//..
   ASSERT(result.type() == JsonType::e_OBJECT);
   ASSERT(result["array of values"][2].theNumber().asDouble()
          == 2.718281828459045);
   ASSERT(result["event"]["date"].theString() == "1969-07-16");
   ASSERT(result["event"]["passengers"][1].theString() == "Buzz Aldrin");
//..
// Finally, we'll 'write' the 'result' back into another string and make sure
// we got the same value back, by using the correct 'WriteOptions' to match
// the input format:
//..
   bsl::string resultString;

   // Set the WriteOptions to match the initial style:
   WriteOptions writeOptions;
   writeOptions.setStyle(bdljsn::WriteStyle::e_PRETTY);
   writeOptions.setInitialIndentLevel(0);
   writeOptions.setSpacesPerLevel(2);
   writeOptions.setSortMembers(true);

   bdljsn::JsonUtil::write(&resultString, result, writeOptions);

   ASSERT(resultString == INPUT_JSON);
//..
        }
        if (verbose)
            cout << "Testing usage example 2" << endl;
        {
#ifdef BSLS_COMPILERFEATURES_SUPPORT_RAW_STRINGS

///Example 2: The Effect of 'options' on 'write'
///- - - - - - - - - - - - - - - - - - - - - - -
// By populating a 'WriteOptions' object and passing it to 'write', the format
// of the resulting JSON can be controlled.
//
// First, let's populate a 'Json' object named 'json' from an input string
// using 'read', and create an empty 'options' (see 'bdljsn::WriteOptions'):
//..
    const bsl::string JSON = R"JSON(
      {
        "a" : 1,
        "b" : []
      }
    )JSON";

    bdljsn::Json         json;
    bdljsn::WriteOptions options;

    int rc = bdljsn::JsonUtil::read(&json, JSON);

    ASSERT(0 == rc);
//..
// There are 4 options, which can be broken down into 2 unrelated sets.
//
// The first set consists of the 'sortMembers' option, which controls whether
// members of objects are printed in lexicogaphical order.
//
// The second set consists of the 'style', 'initialIndentLevel', and
// 'spacesPerLevel' options - 'style' controls which format is used to render a
// 'Json', and, if 'bdljsn::WriteStyle::e_PRETTY == options.style()', the
// 'spacesPerLevel' and 'initialIndentLevel' options are used to control the
// indentation of the output.  For any other value of 'options.style()', the
// 'spacesPerLevel' and 'initialIndentLevel' options have no effect.
//
///'sortMembers'
///-  -  -  -  -
// If 'sortMembers' is true, then the members of an object output by 'write'
// will be in sorted order.  Otherwise, the elements are written in an
// (implementation defined) order (that may change).
//
// The 'sortMembers' option defaults to 'false' for performance reasons, but
// applications that rely on stable output text should set 'sortMembers' to
// 'true' (e.g., in a test where the resulting JSON text is compared for
// equality) .
//
// Here, we set 'sortMembers' to 'true', and verify the resulting JSON text
// matches the expected text:
//..
    options.setSortMembers(true);
    bsl::string output;

    rc = bdljsn::JsonUtil::write(&output, json, options);

    ASSERT(0 == rc);
    ASSERT(R"JSON({"a":1,"b":[]})JSON" == output);
//..
// Had we not specified 'setSortMembers(true)', the order of the "a" and "b"
// members in the 'output' string would be unpredictable.
//
///'style' And 'style'-related options
/// -  -  -  -  -  -  -  -  -  -  -  -
// There are 3 options for 'style' (see 'bdljsn::WriteStyle'):
//: o bdljsn::WriteStyle::e_COMPACT
//: o bdljsn::WriteStyle::e_ONELINE
//: o bdljsn::WriteStyle::e_PRETTY
//
// Next, we write 'json' using the style 'e_COMPACT' (the default), a single
// line presentation with no added spaces after ':' and ',' elements.
//..
    rc = bdljsn::JsonUtil::write(&output, json, options);

    ASSERT(0 == rc);

    // Using 'e_COMPACT' style:
    ASSERT(R"JSON({"a":1,"b":[]})JSON" == output);
//..
// Next, we write 'json' using the 'e_ONELINE' style, another single line
// format, which adds single ' ' characters after ':' and ',' elements for
// readability.
//..
    options.setStyle(bdljsn::WriteStyle::e_ONELINE);
    rc = bdljsn::JsonUtil::write(&output, json, options);

    ASSERT(0 == rc);

    // Using 'e_ONELINE' style:
    ASSERT(R"JSON({"a": 1, "b": []})JSON" == output);
//..
// Next, we write 'json' using the 'e_PRETTY' style, a multiline format where
// newlines are introduced after each (non-terminal) '{', '[', ',', ']', and
// '}' character.  Furthermore, the indentation of JSON rendered in the
// 'e_PRETTY' style is controlled by the other 2 attributes, 'spacesPerLevel'
// and 'initialIndentLevel'.
//
// 'e_PRETTY' styling does not add a newline to the end of the output.
//
// 'spacesPerLevel' controls the number of spaces added for each successive
// indentation level - e.g., if 'spacesPerLevel' is 2, then each nesting level
// of the rendered JSON is indented 2 spaces.
//
// 'initialIndentLevel' controls how much the entire JSON output is indented.
// It defaults to 0 - if it's a positive value, then the entire JSON is
// indented by 'initialIndentLevel * spacesPerLevel' spaces.
//..
    options.setStyle(bdljsn::WriteStyle::e_PRETTY);
    options.setSpacesPerLevel(4);     // the default
    options.setInitialIndentLevel(0); // the default

    rc = bdljsn::JsonUtil::write(&output, json, options);

    ASSERT(0 == rc);

    // Using 'e_PRETTY' style:
    ASSERT(
 R"JSON({
    "a": 1,
    "b": []
})JSON" == output);
//..
// Finally, if we set 'initialIndentLevel' to 1, then an extra set of 4 spaces
// is prepended to each line, where 4 is the value of 'spacesPerLevel':
//..
    options.setInitialIndentLevel(1);

    rc = bdljsn::JsonUtil::write(&output, json, options);

    ASSERT(0 == rc);

    // Using 'e_PRETTY' style (with 'initialIndentLevel' as 1):
    ASSERT(
R"JSON(    {
        "a": 1,
        "b": []
    })JSON" == output);
//..
#endif //  BSLS_COMPILERFEATURES_SUPPORT_RAW_STRINGS
        }

      } break;
      case 6: {
        // --------------------------------------------------------------------
        // PRINTERROR TESTS
        //   Test 'printError' overloads
        //
        // Concerns:
        //: 1 Verify that printError writes an description of the error
        //:   including line and column number.
        //:
        //: 2 That printError will write a simple description of the
        //:   error if the line and column number can't be determined
        //:
        //: 3 That printError can accept input in any of the available
        //:   overloads.
        //
        // Plan:
        //: 1 Perform a table based test on a example input with a
        //:   variety of locations and error messages.
        //:
        //: 2 Perform a sanity test on an error reported from
        //:   'JsonUtil::read'
        //:
        //: 3 Perform a manual test for each of the non-primary
        //:   overloads.
        //
        // Testing:
        //   static ostream& printError(ostream&, streambuf *, const Error&);
        //   static ostream& printError(ostream&, istream&, const Error&);
        //   static ostream& printError(ostream&, string_view&, const Error&);
        // --------------------------------------------------------------------

        if (verbose)
            cout << "\nPRINTERROR TESTS"
                 << "\n================" << endl;

        bsl::string twoLineMesage("0123456789\n0123456789");

        struct {
            uint64_t    d_location;
            const char *d_message;
            const char *d_expected;
        } DATA[] = {
            {  0, "A", "Error (line 1, col 1): A"},
            {  0, "B", "Error (line 1, col 1): B"},
            {  1, "C", "Error (line 1, col 2): C"},
            { 13, "D", "Error (line 2, col 3): D"},
            { 21, "E", "Error (offset 21): E"},
        };

        const int NUM_DATA = sizeof(DATA) / sizeof(*DATA);
        if (verbose)
            cout << "Table based test" << endl;
        for (int i = 0; i < NUM_DATA; ++i) {
            Location location(DATA[i].d_location);
            Error error(location, DATA[i].d_message);
            const char *EXPECTED = DATA[i].d_expected;

            bsl::stringbuf input(twoLineMesage);
            bsl::ostringstream output;

            bsl::ostream& result = Util::printError(output, &input, error);
            ASSERT(&result == &output);
            ASSERTV(output.str(), EXPECTED, output.str() == EXPECTED);
        }

        if (verbose)
            cout << "Sanity test on real JSON" << endl;
        {
            const char *JSON =
            "{\n"
            "  \"a\": 1,\n"
            "  \"b\": 2,\n"
            "  \"c\": oops,\n"
            "}\n"
            ;
            Json result;
            Error error;

            int rc = Util::read(&result, &error, JSON);
            ASSERTV(rc, 0 != rc);

            const char *EXPECTED =
                                 "Error (line 4, col 12): Invalid JSON Number";

            bsl::stringbuf input(JSON);
            bsl::ostringstream output;

            Util::printError(output, &input, error);
            ASSERTV(output.str(), EXPECTED, output.str() == EXPECTED);
        }

        if (verbose)
            cout << "Test overloads" << endl;
        {
            Error error(Location(0), "A");
            const char *EXPECTED = "Error (line 1, col 1): A";

            {
                bsl::ostringstream output;

                bsl::ostream& result =
                    Util::printError(output, twoLineMesage, error);

                ASSERT(&result == &output)
                ASSERTV(output.str(), EXPECTED, output.str() == EXPECTED);
            }
            {
                bsl::ostringstream output;
                bsl::istringstream input(twoLineMesage);

                bsl::ostream& result = Util::printError(output, input, error);

                ASSERT(&result == &output)
                ASSERTV(output.str(), EXPECTED, output.str() == EXPECTED);
            }
        }
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // WRITE OVERLOAD TESTS
        //   Test 'write' overloads
        //
        // Concerns:
        //: 1 Verify that that the overloads of 'write' delegate correctly to
        //:   'static int write(std::string*, const Json&, const WriteOpts&)'
        //
        // Plan:
        //: 1 Test each overload making sure the arguments are passed through
        //:   correctly using the same set of table-based inputs.
        //:   (C-1)
        //
        // Testing:
        //   static int write(bsl::ostream&, const Json&);
        //   static int write(bsl::streambuf*, const Json&);
        //   static int write(bsl::streambuf*, const Json&,
        //                    const WriteOptions&);
        //   static int write(bsl::string*, const Json&);
        //   static int write(bsl::string*, const Json&, const WriteOptions&);
        //   static int write(std::pmr::string*, const Json&);
        //   static int write(std::pmr::string*, const Json&,
        //                    const WriteOptions&);
        //   static int write(std::string*, const Json&);
        //   static int write(std::string         *output,
        //                    const Json&          json,
        //                    const WriteOptions&  options);
        // --------------------------------------------------------------------

        if (verbose)
            cout << "\nWRITE OVERLOAD TESTS"
                 << "\n====================" << endl;

        const WriteStyle::Enum P = WriteStyle::e_PRETTY;

        const bsl::string_view JSON_COMPACT="[1,2]"; // DEFAULT
        const bsl::string_view JSON_PRETTY_IIL_0_SPL_0="[\n1,\n2\n]";
        const bsl::string_view JSON_PRETTY_IIL_1_SPL_2 =
                                                     "  [\n    1,\n    2\n  ]";

        bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

        Json  json(&scratch);
        Error err(&scratch);

        json.makeArray();
        json.theArray().pushBack(Json(JsonNumber("1")));
        json.theArray().pushBack(Json(JsonNumber("2")));

        const Json& JSON_OBJECT = json;

        WriteOptions OPTIONS_IIL_0_SPL_0;
        OPTIONS_IIL_0_SPL_0.setInitialIndentLevel(0);
        OPTIONS_IIL_0_SPL_0.setSpacesPerLevel(0);
        OPTIONS_IIL_0_SPL_0.setStyle(P);

        WriteOptions OPTIONS_IIL_1_SPL_2;
        OPTIONS_IIL_1_SPL_2.setInitialIndentLevel(1);
        OPTIONS_IIL_1_SPL_2.setSpacesPerLevel(2);
        OPTIONS_IIL_1_SPL_2.setStyle(P);

        if (verbose)
            cout << "Testing 'static int write(bsl::ostream&, const Json&);'"
                 << endl;
        {
            bslma::TestAllocator ta;
            bsl::ostringstream   os(&ta);

            int rc = Util::write(os, JSON_OBJECT);
            ASSERTV(rc, 0 == rc);
            ASSERTV(os.str(), JSON_COMPACT, JSON_COMPACT == os.str());
        }

        if (verbose)
            cout << "Testing 'static int write(bsl::streambuf*, const Json&);'"
                 << endl;
        {
            char                        outputStr[256];
            bdlsb::FixedMemOutStreamBuf sb(outputStr, sizeof(outputStr));

            int rc = Util::write(&sb, JSON_OBJECT);
            sb.sputc('\0');
            ASSERTV(rc, 0 == rc);
            ASSERTV(outputStr, JSON_COMPACT, JSON_COMPACT == outputStr);
        }

        if (verbose)
            cout << "Testing 'static int write(bsl::streambuf*, const Json&, "
                    "const WriteOptions&);'"
                 << endl;
        {
            char                        outputStr[256];
            bdlsb::FixedMemOutStreamBuf sb(outputStr, sizeof(outputStr));

            int rc = Util::write(&sb, JSON_OBJECT, OPTIONS_IIL_0_SPL_0);
            sb.sputc('\0');
            ASSERTV(rc, 0 == rc);
            ASSERTV(outputStr,
                    JSON_PRETTY_IIL_0_SPL_0,
                    JSON_PRETTY_IIL_0_SPL_0 == outputStr);

            sb.pubseekpos(0);
            rc = Util::write(&sb, JSON_OBJECT, OPTIONS_IIL_1_SPL_2);
            sb.sputc('\0');
            ASSERTV(rc, 0 == rc);
            ASSERTV(outputStr,
                    JSON_PRETTY_IIL_1_SPL_2,
                    JSON_PRETTY_IIL_1_SPL_2 == outputStr);
        }

        if (verbose)
            cout << "Testing 'static int write(bsl::string*, const Json&);'"
                 << endl;
        {
            bslma::TestAllocator ta;
            bsl::string          outputStr(&ta);

            int rc = Util::write(&outputStr, JSON_OBJECT);
            ASSERTV(rc, 0 == rc);
            ASSERTV(outputStr, JSON_COMPACT, JSON_COMPACT == outputStr);
        }

        if (verbose)
            cout << "Testing 'static int write(bsl::string*, const Json&, "
                    "const WriteOptions&);'"
                 << endl;
        {
            bslma::TestAllocator ta;
            bsl::string          outputStr(&ta);

            int rc = Util::write(&outputStr, JSON_OBJECT, OPTIONS_IIL_0_SPL_0);
            ASSERTV(rc, 0 == rc);
            ASSERTV(outputStr,
                    JSON_PRETTY_IIL_0_SPL_0,
                    JSON_PRETTY_IIL_0_SPL_0 == outputStr);

            rc = Util::write(&outputStr, JSON_OBJECT, OPTIONS_IIL_1_SPL_2);
            ASSERTV(rc, 0 == rc);
            ASSERTV(outputStr,
                    JSON_PRETTY_IIL_1_SPL_2,
                    JSON_PRETTY_IIL_1_SPL_2 == outputStr);
        }

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP17_PMR)
        if (verbose)
            cout << "Testing 'static int write(std::pmr::string*, const "
                    "Json&);'"
                 << endl;
        {
            std::pmr::string outputStr;

            int rc = Util::write(&outputStr, JSON_OBJECT);
            ASSERTV(rc, 0 == rc);
            ASSERTV(outputStr, JSON_COMPACT, JSON_COMPACT == outputStr);
        }

        if (verbose)
            cout
                << "Testing 'static int write(std::pmr::string*, const Json&, "
                   "const WriteOptions&);'"
                << endl;
        {
            std::pmr::string outputStr;

            int rc = Util::write(&outputStr, JSON_OBJECT, OPTIONS_IIL_0_SPL_0);
            ASSERTV(rc, 0 == rc);
            ASSERTV(outputStr,
                    JSON_PRETTY_IIL_0_SPL_0,
                    JSON_PRETTY_IIL_0_SPL_0 == outputStr);

            rc = Util::write(&outputStr, JSON_OBJECT, OPTIONS_IIL_1_SPL_2);
            ASSERTV(rc, 0 == rc);
            ASSERTV(outputStr,
                    JSON_PRETTY_IIL_1_SPL_2,
                    JSON_PRETTY_IIL_1_SPL_2 == outputStr);
        }
#endif
        if (verbose)
            cout << "Testing 'static int write(std::string*, const "
                    "Json&);'"
                 << endl;
        {
            std::string outputStr;

            int rc = Util::write(&outputStr, JSON_OBJECT);
            ASSERTV(rc, 0 == rc);
            ASSERTV(outputStr, JSON_COMPACT, JSON_COMPACT == outputStr);
        }

        if (verbose)
            cout
                << "Testing 'static int write(std::string*, const Json&, "
                   "const WriteOptions&);'"
                << endl;
        {
            std::string outputStr;

            int rc = Util::write(&outputStr, JSON_OBJECT, OPTIONS_IIL_0_SPL_0);
            ASSERTV(rc, 0 == rc);
            ASSERTV(outputStr,
                    JSON_PRETTY_IIL_0_SPL_0,
                    JSON_PRETTY_IIL_0_SPL_0 == outputStr);

            rc = Util::write(&outputStr, JSON_OBJECT, OPTIONS_IIL_1_SPL_2);
            ASSERTV(rc, 0 == rc);
            ASSERTV(outputStr,
                    JSON_PRETTY_IIL_1_SPL_2,
                    JSON_PRETTY_IIL_1_SPL_2 == outputStr);
        }
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // WRITE IMPLEMENTATION TEST
        //   Test the principal 'write' overload
        //
        // Concerns:
        //: 1 Verify that that the principal overload of 'write' functions
        //:   correctly.
        //
        // Plan:
        //: 1 Use a table-driven approach to exercise all possible 'Json'
        //:   subtypes and make sure they're written out correctly.
        //:   (C-1)
        //
        // Testing:
        //   static int write(bsl::ostream&, const Json&, const WriteOptions&);
        // --------------------------------------------------------------------

        if (verbose)
            cout << "\nWRITE IMPLEMENTATION TEST"
                 << "\n=========================" << endl;

        const WriteStyle::Enum C = WriteStyle::e_COMPACT;
        const WriteStyle::Enum O = WriteStyle::e_ONELINE;
        const WriteStyle::Enum P = WriteStyle::e_PRETTY;

        if (verbose)
            cout << "\tBasic read and write round-trip covering all types"
                 << endl;
        {
            bslma::TestAllocator       ta;
            bdlsb::FixedMemInStreamBuf input(SIMPLE_JSON,
                                             bsl::strlen(SIMPLE_JSON));
            Error                      error(&ta);

            bdljsn::Json        result;
            const bdljsn::Json& RESULT = result;

            int rc = Util::read(&result, &error, &input, ReadOptions());

            ASSERTV(rc, error, 0 == rc);
            ASSERT(RESULT.type() == JsonType::e_OBJECT);
            ASSERT(RESULT["number"].theNumber().asDouble() == 2.1);

            bsls::Types::Uint64 ui10;
            rc = RESULT["integer"].theNumber().asUint64(&ui10);
            ASSERT(0 == rc);
            ASSERT(10 == ui10);

            ASSERT(RESULT["string"].theString() == "abc");
            ASSERT(RESULT["boolean"].theBoolean() == true);
            ASSERT(RESULT["date"].theString() == "1970-01-01");
            ASSERT(RESULT["array"][0].theNumber().asDouble() == 3.1);

            bsls::Types::Uint64 ui11;
            rc = RESULT["array"][1].theNumber().asUint64(&ui11);
            ASSERT(0 == rc);
            ASSERT(11 == ui11);

            ASSERT(RESULT["array"][2].theString() == "abc");
            ASSERT(RESULT["array"][3].theBoolean() == true);

            bsl::ostringstream os;

            WriteOptions options;
            options.setStyle(P);
            options.setInitialIndentLevel(0);
            options.setSpacesPerLevel(2);
            options.setSortMembers(true);
            Util::write(os, RESULT, options);

            ASSERTV(os.str(), SIMPLE_JSON, os.str() == SIMPLE_JSON);
        }

        if (verbose) bsl::cout << "\tTest write styles" << bsl::endl;
        {
            struct WriteTests {
                int            d_line;
                const char    *d_json_p;
                const char    *d_expectedCompact_p;
                const char    *d_expectedOneLine_p;
                const char    *d_expectedPretty_p;
            } DATA[] = {
                // L  JSON                COMPACT        ONELINE       PRETTY
                {L_, "{}",                  "{}",          "{}",         "{}" }
              , {L_, "[]",                  "[]",          "[]",         "[]" }
              , {L_, "null",              "null",        "null",       "null" }
              , {L_, "true",              "true",        "true",       "true" }
              , {L_, "false",            "false",       "false",      "false" }
              , {L_, "1.0e+1",          "1.0e+1",      "1.0e+1",     "1.0e+1" }
              , {L_, "\"ab\"",          "\"ab\"",      "\"ab\"",     "\"ab\"" }
              , {L_, "{\"a\":1, \"b\":2}",
                             "{\"a\":1,\"b\":2}",
                                         "{\"a\": 1, \"b\": 2}",
                                                             "{\n"
                                                             "    \"a\": 1,\n"
                                                             "    \"b\": 2\n"
                                                             "}"}
              , {L_, "{\"a\":1}",    "{\"a\":1}",  "{\"a\": 1}",
                                                               "{\n"
                                                               "    \"a\": 1\n"
                                                               "}"}
              , {L_, "[1]",                "[1]",         "[1]", "[\n"
                                                                 "    1\n"
                                                                 "]"}
              , {L_, "[1,2]",            "[1,2]",      "[1, 2]", "[\n"
                                                                 "    1,\n"
                                                                 "    2\n"
                                                                 "]"}
            };

            const int NUM_DATA = sizeof(DATA) / sizeof(*DATA);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE       = DATA[i].d_line;
                const char *JSON       = DATA[i].d_json_p;
                const char *EXPCOMPACT = DATA[i].d_expectedCompact_p;
                const char *EXPONELINE = DATA[i].d_expectedOneLine_p;
                const char *EXPPRETTY  = DATA[i].d_expectedPretty_p;

                bslma::TestAllocator       ta;
                bdlsb::FixedMemInStreamBuf input(JSON, bsl::strlen(JSON));
                Error                      error(&ta);

                bdljsn::Json json;

                int rc = Util::read(&json, &error, &input, ReadOptions());
                ASSERTV(LINE, JSON, json, rc, error, 0 == rc);

                WriteOptions options;
                options.setSortMembers(true);

                bsl::ostringstream compact(&ta);
                options.setStyle(C);
                options.setSortMembers(true);
                rc = Util::write(compact, json, options);
                ASSERTV(LINE, json, rc, 0 == rc);
                ASSERTV(LINE,
                        json,
                        compact.str(),
                        EXPCOMPACT,
                        EXPCOMPACT == compact.str());

                bsl::ostringstream oneline(&ta);
                options.reset();
                options.setStyle(O);
                options.setSortMembers(true);
                rc = Util::write(oneline, json, options);
                ASSERTV(LINE, json, rc, 0 == rc);
                ASSERTV(LINE,
                        json,
                        oneline.str(),
                        EXPONELINE,
                        EXPONELINE == oneline.str());

                bsl::ostringstream pretty(&ta);
                options.reset();
                options.setStyle(P);
                options.setSortMembers(true);
                rc = Util::write(pretty, json, options);
                ASSERTV(LINE, json, rc, 0 == rc);
                ASSERTV(LINE,
                        json,
                        pretty.str(),
                        EXPPRETTY,
                        EXPPRETTY == pretty.str());
            }
        }

        if (verbose) bsl::cout << "\tTest IIL/SPL" << bsl::endl;
        {
            struct WriteTests {
                int            d_line;
                const char    *d_json_p;
                int            d_IIL;
                int            d_SPL;
                const char    *d_expOneline_p;
                const char    *d_expPretty_p;
            } DATA[] = {
                // L  JSON            IIL       SPL      ONELINE       PRETTY
                // Standalone basic types are affected IIL*SPL for initial
                // indentation
                {L_, "{}",             0,        0,         "{}",        "{}" }
              , {L_, "{}",             1,        4,     "    {}",    "    {}" }
              , {L_, "[]",             0,        0,         "[]",        "[]" }
              , {L_, "[]",             1,        4,     "    []",    "    []" }
              , {L_, "null",           0,        0,       "null",      "null" }
              , {L_, "null",           1,        4,   "    null",  "    null" }
              , {L_, "true",           0,        0,       "true",      "true" }
              , {L_, "true",           1,        4,   "    true",  "    true" }
              , {L_, "1.0e+1",         0,        0,     "1.0e+1",    "1.0e+1" }
              , {L_, "1.0e+1",         1,        4, "    1.0e+1",
                                                                 "    1.0e+1" }
              , {L_, "\"ab\"",         0,        0,     "\"ab\"",    "\"ab\"" }
              , {L_, "\"ab\"",         1,        4, "    \"ab\"",
                                                                 "    \"ab\"" }
                // Complex types are affected by IIL and SPL for all levels
                // of indentation
              , {L_, "{\"a\":1, \"b\":2}",
                                       0,        0, "{\"a\": 1, \"b\": 2}",
                                                                  "{\n"
                                                                  "\"a\": 1,\n"
                                                                  "\"b\": 2\n"
                                                                  "}"         }
              , {L_, "{\"a\":1, \"b\":2}",
                                       1,        4, "    {\"a\": 1, \"b\": 2}",
                                                        "    {\n"
                                                        "        \"a\": 1,\n"
                                                        "        \"b\": 2\n"
                                                        "    }"               }
              , {L_, "{\"a\":1, \"b\":2}",
                                      10,        1,
                                              "          {\"a\": 1, \"b\": 2}",
                                                       "          {\n"
                                                       "           \"a\": 1,\n"
                                                       "           \"b\": 2\n"
                                                       "          }"          }
              , {L_, "{\"a\":1, \"b\":2}",
                                       5,        2,
                                              "          {\"a\": 1, \"b\": 2}",
                                                    "          {\n"
                                                    "            \"a\": 1,\n"
                                                    "            \"b\": 2\n"
                                                    "          }"             }
              , {L_, "[1,2,[3,4]]",    0,        0, "[1, 2, [3, 4]]",
                                                  "[\n1,\n2,\n[\n3,\n4\n]\n]" }
              , {L_, "[1,2,[3,4]]",    1,        4, "    [1, 2, [3, 4]]",
                                                            "    [\n"
                                                            "        1,\n"
                                                            "        2,\n"
                                                            "        [\n"
                                                            "            3,\n"
                                                            "            4\n"
                                                            "        ]\n"
                                                            "    ]"           }
            };

            const int NUM_DATA = sizeof(DATA) / sizeof(*DATA);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int                LINE       = DATA[i].d_line;
                const char              *JSON       = DATA[i].d_json_p;
                const int                IIL        = DATA[i].d_IIL;
                const int                SPL        = DATA[i].d_SPL;
                const bsl::string_view&  EXPONELINE = DATA[i].d_expOneline_p;
                const bsl::string_view&  EXPPRETTY  = DATA[i].d_expPretty_p;

                bslma::TestAllocator       ta;
                bdlsb::FixedMemInStreamBuf input(JSON, bsl::strlen(JSON));
                Error                      error(&ta);

                bdljsn::Json json;

                int rc = Util::read(&json, &error, &input, ReadOptions());
                ASSERTV(LINE, JSON, json, rc, error, 0 == rc);

                WriteOptions options;
                options.setSortMembers(true);

                bsl::ostringstream pretty(&ta);
                options.reset();
                options.setStyle(P);
                options.setInitialIndentLevel(IIL);
                options.setSpacesPerLevel(SPL);
                options.setSortMembers(true);
                rc = Util::write(pretty, json, options);
                ASSERTV(LINE, json, rc, 0 == rc);
                ASSERTV(LINE,
                        json,
                        IIL,
                        SPL,
                        EXPPRETTY.length(),
                        pretty.str().length(),
                        EXPPRETTY,
                        pretty.str(),
                        EXPPRETTY == pretty.str());

                bsl::ostringstream oneline(&ta);
                options.reset();
                options.setStyle(O);
                options.setInitialIndentLevel(IIL);
                options.setSpacesPerLevel(SPL);
                options.setSortMembers(true);
                rc = Util::write(oneline, json, options);
                ASSERTV(LINE, json, rc, 0 == rc);
                ASSERTV(LINE,
                        json,
                        IIL,
                        SPL,
                        EXPONELINE.length(),
                        oneline.str().length(),
                        EXPONELINE,
                        oneline.str(),
                        EXPONELINE == oneline.str());
            }
        }

        if (verbose) bsl::cout << "\tTesting sorting keys" << bsl::endl;
        {
            // There's a slight chance that the unsorted order IS sorted.  Use
            // 26 keys to make those odds 1/26!, which is about 2.48e-27.
            const char *JSON = "{"
                               "\"z1\":26,"
                               "\"y2\":25,"
                               "\"x3\":24,"
                               "\"w4\":23,"
                               "\"v5\":22,"
                               "\"u6\":21,"
                               "\"t7\":20,"
                               "\"s8\":19,"
                               "\"r9\":18,"
                               "\"q10\":17,"
                               "\"p11\":16,"
                               "\"o12\":15,"
                               "\"n13\":14,"
                               "\"m14\":13,"
                               "\"l15\":12,"
                               "\"k16\":11,"
                               "\"j17\":10,"
                               "\"i18\":9,"
                               "\"h19\":8,"
                               "\"g20\":7,"
                               "\"f21\":6,"
                               "\"e22\":5,"
                               "\"d23\":4,"
                               "\"c24\":3,"
                               "\"b25\":2,"
                               "\"a26\":1"
                               "}";
            const char *SORTED_JSON = "{"
                               "\"a26\":1,"
                               "\"b25\":2,"
                               "\"c24\":3,"
                               "\"d23\":4,"
                               "\"e22\":5,"
                               "\"f21\":6,"
                               "\"g20\":7,"
                               "\"h19\":8,"
                               "\"i18\":9,"
                               "\"j17\":10,"
                               "\"k16\":11,"
                               "\"l15\":12,"
                               "\"m14\":13,"
                               "\"n13\":14,"
                               "\"o12\":15,"
                               "\"p11\":16,"
                               "\"q10\":17,"
                               "\"r9\":18,"
                               "\"s8\":19,"
                               "\"t7\":20,"
                               "\"u6\":21,"
                               "\"v5\":22,"
                               "\"w4\":23,"
                               "\"x3\":24,"
                               "\"y2\":25,"
                               "\"z1\":26"
                               "}";

            bslma::TestAllocator ta;
            Error                error(&ta);

            bdljsn::Json json;
            int          rc = Util::read(&json, &error, JSON, ReadOptions());
            ASSERTV(0 == rc);

            bsl::ostringstream  sorted(&ta), unsorted(&ta);
            WriteOptions options;
            options.setSortMembers(true);
            rc = Util::write(sorted, json, options);
            ASSERTV(0 == rc);

            options.setSortMembers(false);
            rc = Util::write(unsorted, json, options);
            ASSERTV(0 == rc);

            ASSERTV(sorted.str(),
                    unsorted.str(),
                    sorted.str() != unsorted.str());

            ASSERTV(sorted.str(),
                    SORTED_JSON,
                    SORTED_JSON == sorted.str());

            bdljsn::Json roundTrip;
            rc = Util::read(&roundTrip, &error, unsorted.str(), ReadOptions());
            ASSERTV(0 == rc);

            ASSERTV(json, roundTrip, json == roundTrip);
        }

      } break;
      case 3: {
        // --------------------------------------------------------------------
        // READ OVERLOAD TESTS
        //   Test 'read' overloads
        //
        // Concerns:
        //: 1 Verify that that overloads of 'read' forward to the principal
        //:   'read' method correctly.
        //:
        //: 2 If 'allowTrailingText' is 'true' a 'streambuf' will refer to
        //:   the character immediately following the JSON document.
        //
        // Plan:
        //: 1 Test each overload making sure the arguments are passed through
        //:   correctly using the same set of table-based inputs.
        //:   (C-1)
        //:
        //: 2 Perform a table based test, with a variety of JSON document with
        //:  'allowTrailingText' as 'true', verify the
        //:   the 'streambuf' refers to the expected character after 'read'.
        //
        // Testing:
        //   static int read(Json*, istream&);
        //   static int read(Json*, istream&, const ReadOptions&);
        //   static int read(Json*, streambuf*);
        //   static int read(Json*, streambuf*, const ReadOptions&);
        //   static int read(Json*, const string_view&);
        //   static int read(Json*, const string_view&, const ReadOptions&);
        //   static int read(Json*, Error*, istream&);
        //   static int read(Json*, Error*, istream&, const ReadOptions&);
        //   static int read(Json*, Error*, streambuf*);
        //   static int read(Json*, Error*, streambuf*, const ReadOptions&);
        //   static int read(Json*, Error*, const string_view&);
        // --------------------------------------------------------------------

        if (verbose)
            cout << "\nREAD OVERLOAD TESTS"
                 << "\n===================" << endl;

        const bsl::string goodJson="true";
        const bsl::string badJson="x";

        if (verbose)
            cout << "\n\tTesting 'static int read(Json*, streambuf*);" << endl;
        {
            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);
            bsl::istringstream   iss(&scratch);

            Json        json(&scratch);

            iss.str(badJson);
            int rc = Util::read(&json, iss);
            ASSERTV(rc, badJson, json, 0 != rc);
            ASSERTV(json.isNull(), true == json.isNull());

            iss.str(goodJson);
            rc = Util::read(&json, iss);
            ASSERTV(rc, goodJson, json, 0 == rc);
            ASSERTV(json.isBoolean(), true == json.isBoolean());
            ASSERTV(json, true == json.theBoolean());
        }

        if (verbose)
            cout << "\n\tTesting 'static int read(Json*, istream&, const "
                    "ReadOptions&);'"
                 << endl;
        {
            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);
            bsl::istringstream   iss(&scratch);

            Json        json(&scratch);
            ReadOptions ro;

            const bsl::string json65DeepArray =
                              "[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[["
                              "[[[[[[[[[[[[[[[[[["
                              "]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]"
                              "]]]]]]]]]]]]]]]]]]";

            iss.str(json65DeepArray);

            int rc = Util::read(&json, iss, ro);
            ASSERTV(rc, json65DeepArray, json, 0 != rc);
            ASSERTV(json.isNull(), true == json.isNull());

            ro.setMaxNestedDepth(65);
            iss.str(json65DeepArray);

            rc = Util::read(&json, iss, ro);
            ASSERTV(rc, json65DeepArray, json, 0 == rc);
            ASSERTV(json.isArray(), true == json.isArray());
        }

        if (verbose)
            cout << "\n\tTesting 'static int read(Json*, streambuf*);" << endl;
        {
            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

            Json        json(&scratch);

            bdlsb::FixedMemInStreamBuf badStream(badJson.data(),
                                                 badJson.size());
            int rc = Util::read(&json, &badStream);
            ASSERTV(rc, badJson, json, 0 != rc);
            ASSERTV(json.isNull(), true == json.isNull());

            bdlsb::FixedMemInStreamBuf goodStream(goodJson.data(),
                                                  goodJson.size());
            rc = Util::read(&json, &goodStream);
            ASSERTV(rc, goodJson, json, 0 == rc);
            ASSERTV(json.isBoolean(), true == json.isBoolean());
            ASSERTV(json, true == json.theBoolean());
        }

        if (verbose)
            cout << "\n\tTesting 'static int read(Json*, streambuf*, const "
                    "ReadOptions&);'"
                 << endl;
        {
            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

            Json        json(&scratch);
            ReadOptions ro;

            const bsl::string_view json65DeepArray =
                              "[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[["
                              "[[[[[[[[[[[[[[[[[["
                              "]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]"
                              "]]]]]]]]]]]]]]]]]]";

            bdlsb::FixedMemInStreamBuf json65DeepStream(
                                                       json65DeepArray.data(),
                                                       json65DeepArray.size());

            int rc = Util::read(&json, &json65DeepStream, ro);
            ASSERTV(rc, json65DeepArray, json, 0 != rc);
            ASSERTV(json.isNull(), true == json.isNull());

            ro.setMaxNestedDepth(65);
            json65DeepStream.pubsetbuf(json65DeepArray.data(),
                                       json65DeepArray.size());

            rc = Util::read(&json, &json65DeepStream, ro);
            ASSERTV(rc, json65DeepArray, json, 0 == rc);
            ASSERTV(json.isArray(), true == json.isArray());
        }

        if (verbose)
            cout << "\n\tTesting 'static int read(Json*, const string_view&);'"
                 << endl;
        {
            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

            Json json(&scratch);

            const bsl::string_view badJsonView(badJson);
            int                    rc = Util::read(&json, badJsonView);
            ASSERTV(rc, badJson, json, 0 != rc);
            ASSERTV(json.isNull(), true == json.isNull());

            const bsl::string_view goodJsonView(goodJson);
            rc = Util::read(&json, goodJsonView);
            ASSERTV(rc, goodJson, json, 0 == rc);
            ASSERTV(json.isBoolean(), true == json.isBoolean());
            ASSERTV(json, true == json.theBoolean());
        }

        if (verbose)
            cout << "\n\tTesting 'static int read(Json*, const string_view&, "
                    "const ReadOptions&);'"
                 << endl;
        {
            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

            Json        json(&scratch);
            ReadOptions ro;

            const bsl::string_view json65DeepArray =
                              "[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[["
                              "[[[[[[[[[[[[[[[[[["
                              "]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]"
                              "]]]]]]]]]]]]]]]]]]";

            int rc = Util::read(&json, json65DeepArray, ro);
            ASSERTV(rc, json65DeepArray, json, 0 != rc);
            ASSERTV(json.isNull(), true == json.isNull());

            ro.setMaxNestedDepth(65);

            rc = Util::read(&json, json65DeepArray, ro);
            ASSERTV(rc, json65DeepArray, json, 0 == rc);
            ASSERTV(json.isArray(), true == json.isArray());
        }

        if (verbose)
            cout << "\n\tTesting 'static int read(Json*, Error*, streambuf*);"
                 << endl;
        {
            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);
            bsl::istringstream   iss(&scratch);

            Json        json(&scratch);
            Error       err(&scratch);

            iss.str(badJson);
            int rc = Util::read(&json, &err, iss);
            ASSERTV(rc, badJson, json, 0 != rc);
            ASSERTV(json.isNull(), true == json.isNull());
            // Verify that 'err.location()' is set.
            ASSERTV(err.location(),
                    Location(0) != err.location());
            // Verify that 'err.message()' is set.
            ASSERTV(err.message(),
                    err.message().length(),
                    0 != err.message().length());

            err.reset();

            iss.str(goodJson);
            rc = Util::read(&json, &err, iss);
            ASSERTV(rc, goodJson, json, 0 == rc);
            ASSERTV(json.isBoolean(), true == json.isBoolean());
            ASSERTV(json, true == json.theBoolean());
            // Verify that 'err.location()' is unset.
            ASSERTV(err.location(), Location(0) == err.location());
            // Verify that 'err.message()' is unset.
            ASSERTV(err.message(),
                    err.message().length(),
                    0 == err.message().length());
        }

        if (verbose)
            cout << "\n\tTesting 'static int read(Json*, Error*, istream&, "
                    "const ReadOptions&);'"
                 << endl;
        {
            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);
            bsl::istringstream   iss(&scratch);

            Json        json(&scratch);
            Error       err(&scratch);
            ReadOptions ro;

            const bsl::string json65DeepArray =
                              "[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[["
                              "[[[[[[[[[[[[[[[[[["
                              "]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]"
                              "]]]]]]]]]]]]]]]]]]";

            iss.str(json65DeepArray);

            int rc = Util::read(&json, &err, iss, ro);
            ASSERTV(rc, json65DeepArray, json, 0 != rc);
            ASSERTV(json.isNull(), true == json.isNull());
            // Verify that 'err.location()' is set.
            ASSERTV(err.location(),
                    Location(0) != err.location());
            // Verify that 'err.message()' is set.
            ASSERTV(err.message(),
                    err.message().length(),
                    0 != err.message().length());

            err.reset();
            ro.setMaxNestedDepth(65);
            iss.str(json65DeepArray);

            rc = Util::read(&json, &err, iss, ro);
            ASSERTV(rc, json65DeepArray, json, 0 == rc);
            ASSERTV(json.isArray(), true == json.isArray());
            // Verify that 'err.location()' is unset.
            ASSERTV(err.location(), Location(0) == err.location());
            // Verify that 'err.message()' is unset.
            ASSERTV(err.message(),
                    err.message().length(),
                    0 == err.message().length());
        }

        if (verbose)
            cout << "\n\tTesting 'static int read(Json*, Error*, streambuf*);"
                 << endl;
        {
            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

            Json        json(&scratch);
            Error       err(&scratch);

            bdlsb::FixedMemInStreamBuf badStream(badJson.data(),
                                                 badJson.size());
            int rc = Util::read(&json, &err, &badStream);
            ASSERTV(rc, badJson, json, 0 != rc);
            ASSERTV(json.isNull(), true == json.isNull());
            // Verify that 'err.location()' is set.
            ASSERTV(err.location(),
                    Location(0) != err.location());
            // Verify that 'err.message()' is set.
            ASSERTV(err.message(),
                    err.message().length(),
                    0 != err.message().length());

            err.reset();
            bdlsb::FixedMemInStreamBuf goodStream(goodJson.data(),
                                                  goodJson.size());
            rc = Util::read(&json, &err, &goodStream);
            ASSERTV(rc, goodJson, json, 0 == rc);
            ASSERTV(json.isBoolean(), true == json.isBoolean());
            ASSERTV(json, true == json.theBoolean());
            // Verify that 'err.location()' is unset.
            ASSERTV(err.location(), Location(0) == err.location());
            // Verify that 'err.message()' is unset.
            ASSERTV(err.message(),
                    err.message().length(),
                    0 == err.message().length());
        }

        if (verbose)
            cout << "\n\tTesting 'static int read(Json*, Error*, streambuf*, "
                    "const ReadOptions&);'"
                 << endl;
        {
            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

            Json        json(&scratch);
            Error       err(&scratch);
            ReadOptions ro;

            const bsl::string_view json65DeepArray =
                              "[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[["
                              "[[[[[[[[[[[[[[[[[["
                              "]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]"
                              "]]]]]]]]]]]]]]]]]]";

            bdlsb::FixedMemInStreamBuf json65DeepStream(
                                                       json65DeepArray.data(),
                                                       json65DeepArray.size());

            int rc = Util::read(&json, &err, &json65DeepStream, ro);
            ASSERTV(rc, json65DeepArray, json, 0 != rc);
            ASSERTV(json.isNull(), true == json.isNull());
            // Verify that 'err.location()' is set.
            ASSERTV(err.location(),
                    Location(0) != err.location());
            // Verify that 'err.message()' is set.
            ASSERTV(err.message(),
                    err.message().length(),
                    0 != err.message().length());

            err.reset();
            ro.setMaxNestedDepth(65);
            json65DeepStream.pubsetbuf(json65DeepArray.data(),
                                       json65DeepArray.size());

            rc = Util::read(&json, &err, &json65DeepStream, ro);
            ASSERTV(rc, json65DeepArray, json, 0 == rc);
            ASSERTV(json.isArray(), true == json.isArray());
            // Verify that 'err.location()' is unset.
            ASSERTV(err.location(), Location(0) == err.location());
            // Verify that 'err.message()' is unset.
            ASSERTV(err.message(),
                    err.message().length(),
                    0 == err.message().length());
        }

        if (verbose)
            cout << "\n\tTesting 'static int read(Json*, Error*, const "
                    "string_view&);'"
                 << endl;
        {
            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

            Json  json(&scratch);
            Error err(&scratch);

            const bsl::string_view badJsonView(badJson);
            int                    rc = Util::read(&json, &err, badJsonView);
            ASSERTV(rc, badJson, json, 0 != rc);
            ASSERTV(json.isNull(), true == json.isNull());
            // Verify that 'err.location()' is set.
            ASSERTV(err.location(), Location(0) != err.location());
            // Verify that 'err.message()' is set.
            ASSERTV(err.message(),
                    err.message().length(),
                    0 != err.message().length());

            err.reset();

            const bsl::string_view goodJsonView(goodJson);
            rc = Util::read(&json, &err, goodJsonView);
            ASSERTV(rc, goodJson, json, 0 == rc);
            ASSERTV(json.isBoolean(), true == json.isBoolean());
            ASSERTV(json, true == json.theBoolean());
            // Verify that 'err.location()' is unset.
            ASSERTV(err.location(), Location(0) == err.location());
            // Verify that 'err.message()' is unset.
            ASSERTV(err.message(),
                    err.message().length(),
                    0 == err.message().length());
        }

        if (verbose)
            cout << "\n\tTesting 'static int read(Json*, Error*, const "
                    "string_view&, const ReadOptions&);'"
                 << endl;
        {
            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

            Json        json(&scratch);
            Error       err(&scratch);
            ReadOptions ro;

            const bsl::string_view json65DeepArray =
                              "[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[["
                              "[[[[[[[[[[[[[[[[[["
                              "]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]"
                              "]]]]]]]]]]]]]]]]]]";

            int rc = Util::read(&json, &err, json65DeepArray, ro);
            ASSERTV(rc, json65DeepArray, json, 0 != rc);
            ASSERTV(json.isNull(), true == json.isNull());
            // Verify that 'err.location()' is set.
            ASSERTV(err.location(),
                    Location(0) != err.location());
            // Verify that 'err.message()' is set.
            ASSERTV(err.message(),
                    err.message().length(),
                    0 != err.message().length());

            err.reset();
            ro.setMaxNestedDepth(65);

            rc = Util::read(&json, &err, json65DeepArray, ro);
            ASSERTV(rc, json65DeepArray, json, 0 == rc);
            ASSERTV(json.isArray(), true == json.isArray());
            // Verify that 'err.location()' is unset.
            ASSERTV(err.location(), Location(0) == err.location());
            // Verify that 'err.message()' is unset.
            ASSERTV(err.message(),
                    err.message().length(),
                    0 == err.message().length());
        }

        if (verbose)
            cout << "\n\tTest next input position after 'read'"
                 << endl;
        {
            const char k_EOF = 0;
            static const struct {
                int         d_line;
                const char *d_text;
                char        d_nextChar;
            } DATA[] = {
              // L  TEXT          NEXT CHAR
              //== =============  =========
              { L_, "null",        k_EOF }
            , { L_, "null ",         ' ' }
            , { L_, "null a",        ' ' }
            , { L_, "null{",         '{' }
            , { L_, "true",        k_EOF }
            , { L_, "true ",         ' ' }
            , { L_, "true a",        ' ' }
            , { L_, "true{",         '{' }
            , { L_, "1e1",         k_EOF }
            , { L_, "1e1 ",          ' ' }
            , { L_, "1e1 e",         ' ' }
            , { L_, "1e1{",          '{' }
            , { L_, "[]",          k_EOF }
            , { L_, "[] ",           ' ' }
            , { L_, "[] a",          ' ' }
            , { L_, "[]a",           'a' }
            , { L_, "[]{",           '{' }
            , { L_, "{}",          k_EOF }
            , { L_, "{} ",           ' ' }
            , { L_, "{} a",          ' ' }
            , { L_, "{}a",           'a' }
            , { L_, "{}{",           '{' }
            , { L_, "\"text\"",    k_EOF }
            , { L_, "\"text\" ",     ' ' }
            , { L_, "\"text\" a",    ' ' }
            , { L_, "\"text\"a",     'a' }
            , { L_, "\"text\"{",     '{' }
            };

            const int NUM_DATA = sizeof(DATA) / sizeof(DATA[0]);

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int   LINE      = DATA[ti].d_line;
                const char *TEXT      = DATA[ti].d_text;
                const char  NEXT_CHAR = DATA[ti].d_nextChar;

                const int NEXT_CHAR_INT =
                           NEXT_CHAR == k_EOF
                               ? bdlsb::FixedMemInStreamBuf::traits_type::eof()
                               : static_cast<int>(NEXT_CHAR);

                bdlsb::FixedMemInStreamBuf input(TEXT, bsl::strlen(TEXT));

                if (veryVerbose) {
                    T_ P_(LINE) P_(TEXT) P(NEXT_CHAR_INT);
                }

                bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

                Json        json(&scratch);
                Error       err(&scratch);
                ReadOptions ro;

                ro.setAllowTrailingText(true);

                int rc = Util::read(&json, &err, &input, ro);

                ASSERTV(LINE, TEXT, err, rc, 0 == rc);

                int nextC = input.sgetc();

                ASSERTV(LINE,
                        TEXT,
                        nextC,
                        NEXT_CHAR_INT,
                        nextC == NEXT_CHAR_INT);
            }
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // READ IMPLEMENTATION TEST
        //   Test the principal 'read' overload
        //
        // Concerns:
        //: 1 The principal overload of 'read' functions correctly for any
        //:   valid 'JSON' inputs.
        //:
        //: 2 All allocations take place from the passed-in
        //:   allocator.
        //:
        //: 3 On error, the 'Error*' argument is populated correctly, the
        //:   return code is non-0, and the 'Json*' object is left unchanged.
        //:
        //: 4 The 'readOptions.maxNestedDepth()' argument is honored.
        //:
        //: 5 If 'allowTrailingText' is 'true', 'read' will succeed if their
        //:   are tokens following a valid JSON document as long as they are
        //:   separated by a delimiter.
        //:
        //: 6 If 'allowTrailingText' is 'false', 'read' will fail if their
        //:   are tokens other than white space after a valid JSON.
        //
        // Plan:
        //: 1 Test each 'simple' subtype ('null', 'boolean', 'number',
        //:   'string') independently using a table driven approach in each
        //:   case.
        //:
        //: 2 Test the 'array' subtype using each 'simple' subtype as the
        //:   element, using a manual approach.
        //:
        //: 3 Test the 'object' subtype using each 'simple' subtype as the
        //:   element, using a manual approach.
        //:
        //: 4 Test nested 'array' and 'object' subtypes using 'simple',
        //:   'array', and 'object' elements as recursively nested types, using
        //:   a manual approach.
        //:
        //: 5 Validate that the 'ReadOptions.maxNestedDepth()' attribute is
        //:   honored by 'array', 'object', and heterogenous nested values,
        //:   using a table driven approach.
        //:
        //: 6 Ensure that the 'Error*' argument is correctly populated when an
        //:   error occurs.
        //:
        //: 7 Perform a table based test for a variety of input with
        //:  'allowTrailingText' both 'true' and 'false'.
        //
        // Testing:
        //   static int read(Json*, Error*, string_view&,const ReadOptions&);
        // --------------------------------------------------------------------

        if (verbose)
            cout << "\nREAD IMPLEMENTATION TEST"
                 << "\n========================" << endl;

        if (verbose)
            cout
                << "\n\tCreate a test allocator and install it as the default."
                << endl;

        bslma::TestAllocator         da("default", veryVeryVeryVerbose);
        bslma::DefaultAllocatorGuard dag(&da);

        if (verbose) cout << "\n\tTest 'read' of 'null' value." << endl;
        {
            static const struct {
                int               d_line;
                bsl::string       d_string;
                bool              d_isValid;
            } DATA[] = {
              // L                                             STRING   VALID?
              //== ================================================== ========
              { L_,                                           "null",    true }
            , { L_,                                    "       null",    true }
            , { L_,                                    "null       ",    true }
            , { L_,                                            "nul",   false }
            , { L_,                                     "       nul",   false }
            , { L_,                                     "nul       ",   false }
            };

            const int NUM_DATA = sizeof(DATA) / sizeof(DATA[0]);

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int              LINE     = DATA[ti].d_line;
                const bsl::string_view STRING   = DATA[ti].d_string;
                const bool             IS_VALID = DATA[ti].d_isValid;

                if (veryVerbose) { T_ P_(LINE) P_(STRING) P(IS_VALID) }

                bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

                Json        json(&scratch);
                Error       err(&scratch);
                ReadOptions ro;

                json.makeBoolean(true);

                int rc = Util::read(&json, &err, STRING, ro);

                if (0 == rc) {
                    // Verify that success was expected.
                    ASSERTV(LINE, STRING, IS_VALID, IS_VALID);
                    // Verify that 'json.isNull()' is 'true' on successful
                    // 'read'.
                    ASSERTV(LINE,
                            STRING,
                            rc,
                            IS_VALID,
                            true == json.isNull());
                }
                else {
                    // Verify that failure was expected.
                    ASSERTV(LINE, STRING, IS_VALID, !IS_VALID);
                    // Verify that 'json' is unchanged on failure.
                    ASSERTV(LINE,
                            STRING,
                            rc,
                            true == json.isBoolean());
                    // Verify that 'err.location()' is set.
                    ASSERTV(LINE,
                            STRING,
                            rc,
                            err.location(),
                            Location(0) != err.location());
                    // Verify that 'err.message()' is set.
                    ASSERTV(LINE,
                            STRING,
                            rc,
                            err.message(),
                            err.message().length(),
                            0 != err.message().length());
                }
            }
        }

        if (verbose) cout << "\n\tTest 'read' of 'boolean' values." << endl;
        {
            static const struct {
                int               d_line;
                bsl::string       d_string;
                bool              d_value;
                bool              d_isValid;
            } DATA[] = {
              // L                                   STRING    VALUE    VALID?
              //== ======================================== ======== =========
               { L_,                                "true",    true,     true }
             , { L_,                         "       true",    true,     true }
             , { L_,                         "true       ",    true,     true }
             , { L_,                               "false",   false,     true }
             , { L_,                         "      false",   false,     true }
             , { L_,                         "false      ",   false,     true }
             , { L_,                                 "tru",   false,    false }
             , { L_,                          "       yes",   false,    false }
             , { L_,                          "no        ",   false,    false }
            };

            const int NUM_DATA = sizeof(DATA) / sizeof(DATA[0]);

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int              LINE     = DATA[ti].d_line;
                const bsl::string_view STRING   = DATA[ti].d_string;
                const bool             VALUE    = DATA[ti].d_value;
                const bool             IS_VALID = DATA[ti].d_isValid;

                if (veryVerbose) {
                    T_ P_(LINE) P_(STRING) P_(VALUE) P(IS_VALID)
                }

                bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

                Json        json(&scratch);
                Error       err(&scratch);
                ReadOptions ro;

                int rc = Util::read(&json, &err, STRING, ro);

                if (0 == rc) {
                    // Verify that success was expected.
                    ASSERTV(LINE, STRING, IS_VALID, IS_VALID);
                    // Verify that 'json.isBoolean()' is 'true' on successful
                    // 'read'.
                    ASSERTV(LINE,
                            json.isBoolean(),
                            true == json.isBoolean());
                    // Verify that 'json.theBoolean()' returns the expected
                    // value.
                    ASSERTV(LINE,
                            STRING,
                            VALUE,
                            json.theBoolean(),
                            VALUE == json.theBoolean());
                }
                else {
                    // Verify that failure was expected.
                    ASSERTV(LINE, STRING, IS_VALID, !IS_VALID);
                    // Verify that 'json' is unchanged on failure.
                    ASSERTV(LINE,
                            STRING,
                            rc,
                            true == json.isNull());
                    // Verify that 'err.location()' is set.
                    ASSERTV(LINE,
                            STRING,
                            rc,
                            err.location(),
                            Location(0) != err.location());
                    // Verify that 'err.message()' is set.
                    ASSERTV(LINE,
                            STRING,
                            rc,
                            err.message(),
                            err.message().length(),
                            0 != err.message().length());
                }
            }
        }

        if (verbose) cout << "\n\tTest 'read' of 'number' values." << endl;
        {
            ///(from the bdljsn_jsonnumber.h header)
            ///JSON String Specification
            ///- - - - - - - - - - - - -
            // JSON numbers are defined by strings that match the grammar given
            // at https://www.rfc-editor.org/rfc/rfc8259#section-6.  The
            // equivalent regular expression is:
            //..
            //  /^-?(0|[1-9][0-9]*)(\.[0-9]+)?([eE][-+]?[0-9]+)?$/
            //..

            typedef JsonNumber JN;
            static const struct {
                int               d_line;
                bsl::string_view  d_string;
                JsonNumber        d_value;
                bool              d_isValid;
            } DATA[] = {
              // L                  STRING                  VALUE    VALID?
              //== ======================= ====================== =========
               { L_,                  "0",               JN("0"),     true }
             , { L_,        "         -1",              JN("-1"),     true }
             , { L_,        "   -1e0    ",            JN("-1e0"),     true }
             , { L_,    "   -1.123e0    ",        JN("-1.123e0"),     true }
             , { L_,        "   -1e+1   ",           JN("-1e+1"),     true }
             , { L_,        "-1.2       ",            JN("-1.2"),     true }
             , { L_,                 "+1",                  JN(),    false }
             , { L_,             "0x1234",                  JN(),    false }
             , { L_,                 "01",                  JN(),    false }

             // imported from bdljsn_numberutil.t.cpp
             , { L_,                  "0",               JN("0"),     true }
             , { L_,                "0.0",             JN("0.0"),     true }
             , { L_,             "0.0000",          JN("0.0000"),     true }
             , { L_,      "0.0000e100000",   JN("0.0000e100000"),     true }
             , { L_,        "0.0e-100000",     JN("0.0e-100000"),     true }
             , { L_,       "0.00e+100000",    JN("0.00e+100000"),     true }
             , { L_,      "0.000e-100000",   JN("0.000e-100000"),     true }
             , { L_,                  "1",               JN("1"),     true }
             , { L_,               "1e-0",            JN("1e-0"),     true }
             , { L_,               "1e+0",            JN("1e+0"),     true }
             , { L_,                 "10",              JN("10"),     true }
             , { L_,                "105",             JN("105"),     true }
             , { L_,                "1e1",             JN("1e1"),     true }
             , { L_,              "1.5e1",           JN("1.5e1"),     true }
             , { L_,              "1.5e1",           JN("1.5e1"),     true }
             , { L_,              "1.5e2",           JN("1.5e2"),     true }
             , { L_,              "1.0e2",           JN("1.0e2"),     true }
             , { L_,           "150.0e-1",        JN("150.0e-1"),     true }
             , { L_,          "-150.0e-1",       JN("-150.0e-1"),     true }
             , { L_,      "1E+0000000001",   JN("1E+0000000001"),     true }
             , { L_,              "-1e10",           JN("-1e10"),     true }
             , { L_,               "-128",            JN("-128"),     true }
             , { L_,                "127",             JN("127"),     true }
             , { L_,                "255",             JN("255"),     true }
             , { L_,             "-32768",          JN("-32768"),     true }
             , { L_,              "32767",           JN("32767"),     true }
             , { L_,              "65535",           JN("65535"),     true }
             , { L_,        "-2147483648",     JN("-2147483648"),     true }
             , { L_,         "2147483647",      JN("2147483647"),     true }
             , { L_,         "4294967295",      JN("4294967295"),     true }
             , { L_,                "0.5",             JN("0.5"),     true }
             , { L_,               "-1.5",            JN("-1.5"),     true }
             , { L_,              "-10.5",           JN("-10.5"),     true }
             , { L_,               "10.5",            JN("10.5"),     true }
             , { L_,              "1e-10",           JN("1e-10"),     true }
             , { L_,             "1.5e-0",          JN("1.5e-0"),     true }
             , { L_,             "1.5e+0",          JN("1.5e+0"),     true }
             , { L_,           "1e-99999",        JN("1e-99999"),     true }
             , { L_,      "1E-0000000001",   JN("1E-0000000001"),     true }
             , { L_,              "1.5e0",           JN("1.5e0"),     true }
             , { L_,          "1000.5e-1",       JN("1000.5e-1"),     true }
            };

            const int NUM_DATA = sizeof(DATA) / sizeof(DATA[0]);

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int              LINE     = DATA[ti].d_line;
                const bsl::string_view STRING   = DATA[ti].d_string;
                const JsonNumber&      VALUE    = DATA[ti].d_value;
                const bool             IS_VALID = DATA[ti].d_isValid;

                if (veryVerbose) {
                    T_ P_(LINE) P_(STRING) P_(VALUE) P(IS_VALID)
                }

                bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

                Json        json(&scratch);
                Error       err(&scratch);
                ReadOptions ro;

                int rc = Util::read(&json, &err, STRING, ro);

                if (0 == rc) {
                    // Verify that success was expected.
                    ASSERTV(LINE, STRING, IS_VALID, IS_VALID);
                    // Verify that 'json.isNumber()' is 'true' on successful
                    // 'read'.
                    ASSERTV(LINE,
                            json.isBoolean(),
                            true == json.isNumber());
                    // Verify that 'json.theNumber()' returns the expected
                    // value.
                    ASSERTV(LINE,
                            STRING,
                            VALUE,
                            json.theNumber(),
                            VALUE == json.theNumber());
                }
                else {
                    // Verify that failure was expected.
                    ASSERTV(LINE, STRING, IS_VALID, !IS_VALID);
                    // Verify that 'json' is unchanged on failure.
                    ASSERTV(LINE,
                            STRING,
                            rc,
                            true == json.isNull());
                    // Verify that 'err.location()' is set.
                    ASSERTV(LINE,
                            STRING,
                            rc,
                            err.location(),
                            Location(0) != err.location());
                    // Verify that 'err.message()' is set.
                    ASSERTV(LINE,
                            STRING,
                            rc,
                            err.message(),
                            err.message().length(),
                            0 != err.message().length());
                }
            }
        }

        if (verbose) cout << "\n\tTest 'read' of 'string' values." << endl;
        {
            /*
               From https://www.rfc-editor.org/rfc/rfc8259#section-7

               string = quotation-mark *char quotation-mark

               char = unescaped /
                 escape (
                     %x22 /          ; "    quotation mark  U+0022
                     %x5C /          ; \    reverse solidus U+005C
                     %x2F /          ; /    solidus         U+002F
                     %x62 /          ; b    backspace       U+0008
                     %x66 /          ; f    form feed       U+000C
                     %x6E /          ; n    line feed       U+000A
                     %x72 /          ; r    carriage return U+000D
                     %x74 /          ; t    tab             U+0009
                     %x75 4HEXDIG )  ; uXXXX                U+XXXX

               escape = %x5C              ; \

               quotation-mark = %x22      ; "

               unescaped = %x20-21 / %x23-5B / %x5D-10FFFF
            */

            const char *ERROR_VALUE = "";
            static const struct {
                int              d_line;
                bsl::string_view d_string;
                bsl::string_view d_value;
                bool             d_isValid;
            } DATA[] = {
              //  L                 STRING                  VALUE    VALID?
              // == ====================== ====================== =========
               { L_,                "\"\"",                    "",     true }
             , { L_,   "\"\\\\\\/\\b\\f\\n\\r\\t\\u1234\\\"\"",
                                    "\\/\b\f\n\r\t\xe1\x88\xb4\"",     true }
             , { L_,  "\"\\uD834\\uDD1E\"",    "\xf0\x9d\x84\x9e",     true }
             , { L_,                 " \"",           ERROR_VALUE,    false }
             , { L_,             "\"\\z\"",           ERROR_VALUE,    false }

             // imported from bdljsn_stringutil.t.cpp
             , { L_,                "\"\"",                    "",     true }
             , { L_,             "\"ABC\"",                 "ABC",     true }
             , { L_,            "\"\\\"\"",                  "\"",     true }
             , { L_,            "\"\\\\\"",                  "\\",     true }
             , { L_,             "\"\\b\"",                  "\b",     true }
             , { L_,             "\"\\f\"",                  "\f",     true }
             , { L_,             "\"\\n\"",                  "\n",     true }
             , { L_,             "\"\\r\"",                  "\r",     true }
             , { L_,             "\"\\t\"",                  "\t",     true }
             , { L_,           "\"u0001\"",               "u0001",     true }
             , { L_,           "\"UABCD\"",               "UABCD",     true }
             , { L_,         "\"\\u0001\"",                "\x01",     true }
             , { L_,         "\"\\u0020\"",                   " ",     true }
             , { L_,         "\"\\u002E\"",                   ".",     true }
             , { L_,         "\"\\u0041\"",                   "A",     true }
             , { L_,              "\"AB\"",                  "AB",     true }
             , { L_,       "\"A\\u0020B\"",                 "A B",     true }
             , { L_,       "\"A\\u002eB\"",                 "A.B",     true }
             , { L_,       "\"A\\u0080G\"",          "A\xC2\x80G",     true }
             , { L_,         "\"\\U000G\"",           ERROR_VALUE,    false }
             , { L_,         "\"\\U00h0\"",           ERROR_VALUE,    false }
             , { L_,         "\"\\U0M00\"",           ERROR_VALUE,    false }
             , { L_,         "\"\\UX000\"",           ERROR_VALUE,    false }
             , { L_,         "\"\\U7G00\"",           ERROR_VALUE,    false }
             , { L_,         "\"\\U007G\"",           ERROR_VALUE,    false }
             , { L_,         "\"\\UXXXX\"",           ERROR_VALUE,    false }
             , { L_,         "\"\\U0xFF\"",           ERROR_VALUE,    false }
             , { L_,         "\"\\U   4\"",           ERROR_VALUE,    false }
             , { L_,         "\"\\U  -1\"",           ERROR_VALUE,    false }
             , { L_,  "\"\\ud83d\\u0xFF\"",           ERROR_VALUE,    false }
             , { L_,  "\"\\ud83d\\u   4\"",           ERROR_VALUE,    false }
             , { L_,  "\"\\ud83d\\u  -1\"",           ERROR_VALUE,    false }
             , { L_,         "\"\\uD8ff\"",           ERROR_VALUE,    false }
             , { L_,         "\"\\ud917\"",           ERROR_VALUE,    false }
             , { L_,  "\"\\ud800\\udbff\"",           ERROR_VALUE,    false }
             , { L_,  "\"\\udbad\\udbff\"",           ERROR_VALUE,    false }
             , { L_,  "\"\\udbff\\udbff\"",           ERROR_VALUE,    false }
             , { L_,  "\"\\ud800\\udc00\"",    "\xF0\x90\x80\x80",     true }
             , { L_,  "\"\\ud83d\\ude42\"",    "\xF0\x9F\x99\x82",     true }
             , { L_,  "\"\\udbff\\udfff\"",    "\xF4\x8F\xBF\xBF",     true }
             , { L_,  "\"\\ud800\\ue000\"",           ERROR_VALUE,    false }
             , { L_,  "\"\\udbad\\ue000\"",           ERROR_VALUE,    false }
             , { L_,  "\"\\udbff\\ue000\"",           ERROR_VALUE,    false }
             , { L_,  "\"\\ud83d\\ude4`\"",           ERROR_VALUE,    false }
             , { L_,  "\"\\ud83d\\ude4g\"",           ERROR_VALUE,    false }
            };

            const int NUM_DATA = sizeof(DATA) / sizeof(DATA[0]);

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int               LINE     = DATA[ti].d_line;
                const bsl::string_view& STRING   = DATA[ti].d_string;
                const bsl::string_view& VALUE    = DATA[ti].d_value;
                const bool              IS_VALID = DATA[ti].d_isValid;

                if (veryVerbose) {
                    T_ P_(LINE) P_(STRING) P_(VALUE) P(IS_VALID)
                }

                bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

                Json        json(&scratch);
                Error       err(&scratch);
                ReadOptions ro;

                int rc = Util::read(&json, &err, STRING, ro);

                if (0 == rc) {
                    // Verify that success was expected.
                    ASSERTV(LINE, STRING, IS_VALID, IS_VALID);
                    // Verify that 'json.isString()' is 'true' on successful
                    // 'read'.
                    ASSERTV(LINE,
                            json.isBoolean(),
                            true == json.isString());
                    // Verify that 'json.theString()' returns the expected
                    // value.
                    ASSERTV(LINE,
                            STRING,
                            VALUE,
                            json.theString(),
                            VALUE == json.theString());
                }
                else {
                    // Verify that failure was expected.
                    ASSERTV(LINE, STRING, IS_VALID, !IS_VALID);
                    // Verify that 'json' is unchanged on failure.
                    ASSERTV(LINE,
                            STRING,
                            rc,
                            true == json.isNull());
                    // Verify that 'err.location()' is set.
                    ASSERTV(LINE,
                            STRING,
                            rc,
                            err.location(),
                            Location(0) != err.location());
                    // Verify that 'err.message()' is set.
                    ASSERTV(LINE,
                            STRING,
                            rc,
                            err.message(),
                            err.message().length(),
                            0 != err.message().length());
                }
            }
        }

        if (verbose)
            cout << "\n\tTest 'read' of simple 'array' values." << endl;
        {
            // Positive tests
            {
               bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

               Json jsonResult(&scratch);

               ASSERTV(jsonResult,
                       jsonResult.isNull(),
                       true == jsonResult.isNull());

               jsonResult.makeArray();

               ASSERTV(jsonResult,
                       jsonResult.isArray(),
                       true == jsonResult.isArray());

               JsonArray &theArray = jsonResult.theArray();

               checkResult(L_, "[]", jsonResult, true);
               checkResult(L_, WS "[]", jsonResult, true);
               checkResult(L_, WS "[" WS "]" WS, jsonResult, true);

               appendNull(&theArray);

               ASSERTV(jsonResult,
                       theArray,
                       theArray.size(),
                       1 == theArray.size());
               ASSERTV(jsonResult,
                       theArray.back(),
                       theArray.back().isNull(),
                       true == theArray.back().isNull());

               checkResult(L_, "[null]", jsonResult, true);

               appendBoolean(&theArray, true);

               ASSERTV(jsonResult,
                       theArray,
                       theArray.size(),
                       2 == theArray.size());
               ASSERTV(jsonResult,
                       theArray.back(),
                       theArray.back().isBoolean(),
                       true == theArray.back().isBoolean());

               checkResult(L_, "[null, true]", jsonResult, true);

               appendNumber(&theArray, "123.45e+1");

               ASSERTV(jsonResult,
                       theArray,
                       theArray.size(),
                       3 == theArray.size());
               ASSERTV(jsonResult,
                       theArray.back(),
                       theArray.back().isNumber(),
                       true == theArray.back().isNumber());
               ASSERTV(jsonResult,
                       theArray.back(),
                       theArray.back().theNumber(),
                       JsonNumber("123.45e+1") == theArray.back().theNumber());

               checkResult(L_, "[null, true, 123.45e+1]", jsonResult, true);

               appendString(&theArray, "Hello, world!");

               ASSERTV(jsonResult,
                       theArray,
                       theArray.size(),
                       4 == theArray.size());
               ASSERTV(jsonResult,
                       theArray.back(),
                       theArray.back().isString(),
                       true == theArray.back().isString());
               ASSERTV(jsonResult,
                       theArray.back(),
                       theArray.back().theString(),
                       "Hello, world!" == theArray.back().theString());

               checkResult(L_,
                           "[null, true, 123.45e+1, \"Hello, world!\"]",
                           jsonResult,
                           true);
               checkResult(L_,
                           WS "[" WS "null" WS "," WS "true" WS "," WS
                              "123.45e+1" WS "," WS "\"Hello, world!\"" WS "]",
                           jsonResult,
                           true);

               appendMemberArray(&theArray);

               ASSERTV(jsonResult,
                       theArray,
                       theArray.size(),
                       5 == theArray.size());
               ASSERTV(jsonResult,
                       theArray.back(),
                       theArray.back().isArray(),
                       true == theArray.back().isArray());
               ASSERTV(jsonResult,
                       theArray.back(),
                       theArray.back().theArray(),
                       theArray.back().theArray().size(),
                       0 == theArray.back().theArray().size());

               checkResult(L_,
                           "[null, true, 123.45e+1, \"Hello, world!\", []]",
                           jsonResult,
                           true);
               checkResult(L_,
                           WS "[" WS "null" WS "," WS "true" WS "," WS
                              "123.45e+1" WS "," WS "\"Hello, world!\"" WS
                              "," WS "[" WS "]" WS "]",
                           jsonResult,
                           true);

               appendMemberObject(&theArray);

               ASSERTV(jsonResult,
                       theArray,
                       theArray.size(),
                       6 == theArray.size());
               ASSERTV(jsonResult,
                       theArray.back(),
                       theArray.back().isObject(),
                       true == theArray.back().isObject());
               ASSERTV(jsonResult,
                       theArray.back(),
                       theArray.back().theObject(),
                       theArray.back().theObject().size(),
                       0 == theArray.back().theObject().size());

               checkResult(
                          L_,
                          "[null, true, 123.45e+1, \"Hello, world!\", [], {}]",
                          jsonResult,
                          true);
               checkResult(L_,
                           WS "[" WS "null" WS "," WS "true" WS "," WS
                              "123.45e+1" WS "," WS "\"Hello, world!\"" WS
                              "," WS "[" WS "]" WS "," WS "{" WS "}" WS "]",
                           jsonResult,
                           true);
            }

            // Negative tests
            {
               bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

               Json jsonResult(&scratch);

               ASSERTV(jsonResult,
                       jsonResult.isNull(),
                       true == jsonResult.isNull());

               checkResult(L_, "[", jsonResult, false);
               checkResult(L_, WS "[", jsonResult, false);
               checkResult(L_, WS "[" WS, jsonResult, false);

               checkResult(L_, " ]", jsonResult, false);
               checkResult(L_, WS "]", jsonResult, false);
               checkResult(L_, WS "]" WS, jsonResult, false);

               checkResult(L_, "[}", jsonResult, false);
               checkResult(L_, WS "[}", jsonResult, false);
               checkResult(L_, WS "[" WS "}" WS, jsonResult, false);

               checkResult(L_, "[nul]", jsonResult, false);
               checkResult(L_, "[nil]", jsonResult, false);
               checkResult(L_, "[no]", jsonResult, false);

               checkResult(L_, "[null, rue]", jsonResult, false);
               checkResult(L_, "[null, tru]", jsonResult, false);
               checkResult(L_, "[null, true,]", jsonResult, false);
               checkResult(L_, "[null, true],", jsonResult, false);

               checkResult(L_, "[null, true, +123.45e+1]", jsonResult, false);

               checkResult(L_,
                           "[null, true, 123.45e+1, \"Hello, world!\"\"]",
                           jsonResult,
                           false);

               checkResult(L_,
                           "[null, true, 123.45e+1, \"\"Hello, world!\"]",
                           jsonResult,
                           false);

               checkResult(L_,
                           "[null, true, 123.45e+1, \"Hello, world!\", {]]",
                           jsonResult,
                           false);

               checkResult(L_,
                           "[null, true, 123.45e+1, \"Hello, world!\", [}]",
                           jsonResult,
                           false);

               checkResult(
                          L_,
                          "[null, true, 123.45e+1, \"Hello, world!\", [], [}]",
                          jsonResult,
                          false);
               checkResult(
                          L_,
                          "[null, true, 123.45e+1, \"Hello, world!\", [], {]]",
                          jsonResult,
                          false);
            }
        }

        if (verbose)
            cout << "\n\tTest 'read' of simple 'object' values." << endl;
        {
            // Positive tests
            {
               bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

               Json jsonResult(&scratch);

               ASSERTV(jsonResult,
                       jsonResult.isNull(),
                       true == jsonResult.isNull());

               jsonResult.makeObject();

               ASSERTV(jsonResult,
                       jsonResult.isObject(),
                       true == jsonResult.isObject());

               JsonObject &theObject = jsonResult.theObject();

               checkResult(L_, "{}", jsonResult, true);
               checkResult(L_, WS "{}", jsonResult, true);
               checkResult(L_, WS "{" WS "}" WS, jsonResult, true);

               appendNull(&theObject, "a");

               ASSERTV(jsonResult,
                       theObject,
                       theObject.size(),
                       1 == theObject.size());
               ASSERTV(jsonResult,
                       theObject["a"],
                       theObject["a"].isNull(),
                       true == theObject["a"].isNull());

               checkResult(L_, "{\"a\":null}", jsonResult, true);
               checkResult(L_,
                           WS "{" WS "\"a\"" WS ":" WS "null" WS "}" WS,
                           jsonResult,
                           true);

               appendBoolean(&theObject, "b", true);

               ASSERTV(jsonResult,
                       theObject,
                       theObject.size(),
                       2 == theObject.size());
               ASSERTV(jsonResult,
                       theObject["b"],
                       theObject["b"].isBoolean(),
                       true == theObject["b"].isBoolean());

               checkResult(L_, "{\"a\":null, \"b\":true}", jsonResult, true);

               appendNumber(&theObject, "c", "123.45e+1");

               ASSERTV(jsonResult,
                       theObject,
                       theObject.size(),
                       3 == theObject.size());
               ASSERTV(jsonResult,
                       theObject["c"],
                       theObject["c"].isNumber(),
                       true == theObject["c"].isNumber());
               ASSERTV(jsonResult,
                       theObject["c"],
                       theObject["c"].theNumber(),
                       JsonNumber("123.45e+1") == theObject["c"].theNumber());

               checkResult(L_,
                           "{\"a\":null, \"b\":true, \"c\":123.45e+1}",
                           jsonResult,
                           true);

               appendString(&theObject, "d", "Hello, world!");

               ASSERTV(jsonResult,
                       theObject,
                       theObject.size(),
                       4 == theObject.size());
               ASSERTV(jsonResult,
                       theObject["d"],
                       theObject["d"].isString(),
                       true == theObject["d"].isString());
               ASSERTV(jsonResult,
                       theObject["d"],
                       theObject["d"].theString(),
                       "Hello, world!" == theObject["d"].theString());

               checkResult(L_,
                           "{\"a\":null, \"b\":true, \"c\":123.45e+1, "
                           "\"d\":\"Hello, world!\"}",
                           jsonResult,
                           true);
               checkResult(L_,
                           WS "{" WS "\"a\"" WS ":" WS "null" WS "," WS
                              "\"b\"" WS ":" WS "true" WS "," WS "\"c\"" WS
                              ":" WS "123.45e+1" WS "," WS "\"d\"" WS ":" WS
                              "\"Hello, world!\"" WS "}",
                           jsonResult,
                           true);
               // Verify that objects are order-independent
               checkResult(L_,
                           "{\"c\":123.45e+1, \"b\":true, "
                           "\"d\":\"Hello, world!\", \"a\":null}",
                           jsonResult,
                           true);

               addMemberArray(&theObject, "e");

               ASSERTV(jsonResult,
                       theObject,
                       theObject.size(),
                       5 == theObject.size());
               ASSERTV(jsonResult,
                       theObject["e"],
                       theObject["e"].isArray(),
                       true == theObject["e"].isArray());
               ASSERTV(jsonResult,
                       theObject["e"],
                       theObject["e"].theArray(),
                       theObject["e"].theArray().size(),
                       0 == theObject["e"].theArray().size());

               checkResult(L_,
                           "{\"a\":null, \"b\":true, \"c\":123.45e+1, "
                           "\"d\":\"Hello, world!\", \"e\":[]}",
                           jsonResult,
                           true);
               checkResult(L_,
                           WS "{" WS "\"a\"" WS ":" WS "null" WS "," WS
                              "\"b\"" WS ":" WS "true" WS "," WS "\"c\"" WS
                              ":" WS "123.45e+1" WS "," WS "\"d\"" WS ":" WS
                              "\"Hello, world!\"" WS "," WS "\"e\"" WS ":" WS
                              "[" WS "]" WS "}",
                           jsonResult,
                           true);

               addMemberObject(&theObject, "f");

               ASSERTV(jsonResult,
                       theObject,
                       theObject.size(),
                       6 == theObject.size());
               ASSERTV(jsonResult,
                       theObject["f"],
                       theObject["f"].isObject(),
                       true == theObject["f"].isObject());
               ASSERTV(jsonResult,
                       theObject["f"],
                       theObject["f"].theObject(),
                       theObject["f"].theObject().size(),
                       0 == theObject["f"].theObject().size());

               checkResult(L_,
                           "{\"a\":null, \"b\":true, \"c\":123.45e+1, "
                           "\"d\":\"Hello, world!\", \"e\":[], \"f\":{}}",
                           jsonResult,
                           true);
               checkResult(L_,
                           WS "{" WS "\"a\"" WS ":" WS "null" WS "," WS
                              "\"b\"" WS ":" WS "true" WS "," WS "\"c\"" WS
                              ":" WS "123.45e+1" WS "," WS "\"d\"" WS ":" WS
                              "\"Hello, world!\"" WS "," WS "\"e\"" WS ":" WS
                              "[" WS "]" WS "," WS "\"f\"" WS ":" WS "{" WS
                              "}" WS "}",
                           jsonResult,
                           true);
            }

            // Negative tests
            {
               bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

               Json jsonResult(&scratch);

               checkResult(L_, "{", jsonResult, false);
               checkResult(L_, WS "{", jsonResult, false);
               checkResult(L_, WS "{" WS, jsonResult, false);

               checkResult(L_, " }", jsonResult, false);
               checkResult(L_, WS "}", jsonResult, false);
               checkResult(L_, WS "}" WS, jsonResult, false);

               checkResult(L_, "{]", jsonResult, false);
               checkResult(L_, WS "{]", jsonResult, false);
               checkResult(L_, WS "{" WS "]" WS, jsonResult, false);

               checkResult(L_, "{\"a\":ull}", jsonResult, false);
               checkResult(L_, "{\"a\",null}", jsonResult, false);
               checkResult(L_, "{\"a\":}", jsonResult, false);
               checkResult(L_, "{:null}", jsonResult, false);
               checkResult(L_, "{:}", jsonResult, false);

               checkResult(L_, "{\"a\":null, \"b\":tre}", jsonResult, false);
               checkResult(L_, "{\"a\":null  \"b\":tre}", jsonResult, false);
               checkResult(L_, "{\"a\":null  \"b\":tre}", jsonResult, false);

               checkResult(L_,
                           "{\"a\":null, \"b\":true, \"c\":+123.45e+1}",
                           jsonResult,
                           false);
            }
        }

        if (verbose)
            cout << "\n\tTest 'read' of complex nested 'array' values."
                 << endl;
        {
            {
               bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

               Json jsonResult(&scratch);

               ASSERTV(jsonResult,
                       jsonResult.isNull(),
                       true == jsonResult.isNull());

               jsonResult.makeArray();

               ASSERTV(jsonResult,
                       jsonResult.isArray(),
                       true == jsonResult.isArray());

               JsonArray *theArray = &(jsonResult.theArray());

               checkResult(L_, "[]", jsonResult, true);
               checkResult(L_, WS "[]", jsonResult, true);
               checkResult(L_, WS "[" WS "]" WS, jsonResult, true);

               appendNull(theArray);

               ASSERTV(jsonResult,
                       *theArray,
                       theArray->size(),
                       1 == theArray->size());
               ASSERTV(jsonResult,
                       theArray->back(),
                       theArray->back().isNull(),
                       true == theArray->back().isNull());

               checkResult(L_, "[null]", jsonResult, true);

               appendMemberArray(theArray);
               ASSERTV(jsonResult,
                       *theArray,
                       theArray->size(),
                       2 == theArray->size());
               ASSERTV(jsonResult,
                       theArray->back(),
                       theArray->back().isArray(),
                       true == theArray->back().isArray());

               theArray = &(theArray->back().theArray());
               ASSERTV(jsonResult,
                       *theArray,
                       theArray->size(),
                       0 == theArray->size());

               appendBoolean(theArray, true);

               ASSERTV(jsonResult,
                       *theArray,
                       theArray->size(),
                       1 == theArray->size());
               ASSERTV(jsonResult,
                       theArray->back(),
                       theArray->back().isBoolean(),
                       true == theArray->back().isBoolean());

               checkResult(L_, "[null, [true]]", jsonResult, true);

               appendNumber(theArray, "123.45e+1");

               ASSERTV(jsonResult,
                       theArray,
                       theArray->size(),
                       2 == theArray->size());
               ASSERTV(jsonResult,
                       theArray->back(),
                       theArray->back().isNumber(),
                       true == theArray->back().isNumber());
               ASSERTV(jsonResult,
                       theArray->back(),
                       theArray->back().theNumber(),
                       JsonNumber("123.45e+1") ==
                                                 theArray->back().theNumber());

               checkResult(L_, "[null, [true, 123.45e+1]]", jsonResult, true);

               appendString(theArray, "Hello, world!");

               ASSERTV(jsonResult,
                       theArray,
                       theArray->size(),
                       3 == theArray->size());
               ASSERTV(jsonResult,
                       theArray->back(),
                       theArray->back().isString(),
                       true == theArray->back().isString());
               ASSERTV(jsonResult,
                       theArray->back(),
                       theArray->back().theString(),
                       "Hello, world!" == theArray->back().theString());

               checkResult(L_,
                           "[null, [true, 123.45e+1, \"Hello, world!\"]]",
                           jsonResult,
                           true);
               checkResult(L_,
                           WS "[" WS "null" WS "," WS "[" WS "true" WS "," WS
                              "123.45e+1" WS "," WS "\"Hello, world!\"" WS
                              "]" WS "]",
                           jsonResult,
                           true);

               appendMemberArray(theArray);

               ASSERTV(jsonResult,
                       theArray,
                       theArray->size(),
                       4 == theArray->size());
               ASSERTV(jsonResult,
                       theArray->back(),
                       theArray->back().isArray(),
                       true == theArray->back().isArray());
               ASSERTV(jsonResult,
                       theArray->back(),
                       theArray->back().theArray(),
                       theArray->back().theArray().size(),
                       0 == theArray->back().theArray().size());

               checkResult(L_,
                           "[null, [true, 123.45e+1, \"Hello, world!\", []]]",
                           jsonResult,
                           true);
               checkResult(L_,
                           WS "[" WS "null" WS "," WS "[" WS "true" WS "," WS
                              "123.45e+1" WS "," WS "\"Hello, world!\"" WS
                              "," WS "[" WS "]" WS "]" WS "]",
                           jsonResult,
                           true);

               appendMemberObject(theArray);

               ASSERTV(jsonResult,
                       theArray,
                       theArray->size(),
                       5 == theArray->size());
               ASSERTV(jsonResult,
                       theArray->back(),
                       theArray->back().isObject(),
                       true == theArray->back().isObject());
               ASSERTV(jsonResult,
                       theArray->back(),
                       theArray->back().theObject(),
                       theArray->back().theObject().size(),
                       0 == theArray->back().theObject().size());

               checkResult(
                        L_,
                        "[null, [true, 123.45e+1, \"Hello, world!\", [], {}]]",
                        jsonResult,
                        true);
               checkResult(L_,
                           WS "[" WS "null" WS "," WS "[" WS "true" WS "," WS
                              "123.45e+1" WS "," WS "\"Hello, world!\"" WS
                              "," WS "[" WS "]" WS "," WS "{" WS "}" WS "]" WS
                              "]",
                           jsonResult,
                           true);
            }
        }

        if (verbose)
            cout << "\n\tTest 'read' of complex nested 'object' values."
                 << endl;
        {
            {
               bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

               Json jsonResult(&scratch);

               ASSERTV(jsonResult,
                       jsonResult.isNull(),
                       true == jsonResult.isNull());

               jsonResult.makeObject();

               ASSERTV(jsonResult,
                       jsonResult.isObject(),
                       true == jsonResult.isObject());

               JsonObject *theObject = &(jsonResult.theObject());

               checkResult(L_, "{}", jsonResult, true);
               checkResult(L_, WS "{}", jsonResult, true);
               checkResult(L_, WS "{" WS "}" WS, jsonResult, true);

               appendNull(theObject, "a");

               ASSERTV(jsonResult,
                       *theObject,
                       theObject->size(),
                       1 == theObject->size());
               ASSERTV(jsonResult,
                       (*theObject)["a"],
                       (*theObject)["a"].isNull(),
                       true == (*theObject)["a"].isNull());

               checkResult(L_, "{\"a\":null}", jsonResult, true);
               checkResult(L_,
                           WS "{" WS "\"a\"" WS ":" WS "null" WS "}" WS,
                           jsonResult,
                           true);

               addMemberObject(theObject, "a1");
               ASSERTV(jsonResult,
                       *theObject,
                       theObject->size(),
                       2 == theObject->size());
               ASSERTV(jsonResult,
                       (*theObject)["a1"],
                       (*theObject)["a1"].isObject(),
                       true == (*theObject)["a1"].isObject());

               theObject=&((*theObject)["a1"].theObject());

               appendBoolean(theObject, "b", true);

               ASSERTV(jsonResult,
                       *theObject,
                       theObject->size(),
                       1 == theObject->size());
               ASSERTV(jsonResult,
                       (*theObject)["b"],
                       (*theObject)["b"].isBoolean(),
                       true == (*theObject)["b"].isBoolean());

               checkResult(L_,
                           "{\"a\":null, \"a1\":{\"b\":true}}",
                           jsonResult,
                           true);

               appendNumber(theObject, "c", "123.45e+1");

               ASSERTV(jsonResult,
                       *theObject,
                       theObject->size(),
                       2 == theObject->size());
               ASSERTV(jsonResult,
                       (*theObject)["c"],
                       (*theObject)["c"].isNumber(),
                       true == (*theObject)["c"].isNumber());
               ASSERTV(
                      jsonResult,
                      (*theObject)["c"],
                      (*theObject)["c"].theNumber(),
                      JsonNumber("123.45e+1") ==
                                                (*theObject)["c"].theNumber());

               checkResult(
                          L_,
                          "{\"a\":null, \"a1\":{\"b\":true, \"c\":123.45e+1}}",
                          jsonResult,
                          true);

               appendString(theObject, "d", "Hello, world!");

               ASSERTV(jsonResult,
                       *theObject,
                       theObject->size(),
                       3 == theObject->size());
               ASSERTV(jsonResult,
                       (*theObject)["d"],
                       (*theObject)["d"].isString(),
                       true == (*theObject)["d"].isString());
               ASSERTV(jsonResult,
                       (*theObject)["d"],
                       (*theObject)["d"].theString(),
                       "Hello, world!" == (*theObject)["d"].theString());

               checkResult(L_,
                           "{\"a\":null, \"a1\":{\"b\":true, \"c\":123.45e+1, "
                           "\"d\":\"Hello, world!\"}}",
                           jsonResult,
                           true);
               // Verify that objects are order-independent
               checkResult(L_,
                           "{\"a1\":{\"c\":123.45e+1, \"b\":true, "
                           "\"d\":\"Hello, world!\"}, \"a\":null}",
                           jsonResult,
                           true);

               addMemberArray(theObject, "e");

               ASSERTV(jsonResult,
                       *theObject,
                       theObject->size(),
                       4 == theObject->size());
               ASSERTV(jsonResult,
                       (*theObject)["e"],
                       (*theObject)["e"].isArray(),
                       true == (*theObject)["e"].isArray());
               ASSERTV(jsonResult,
                       (*theObject)["e"],
                       (*theObject)["e"].theArray(),
                       (*theObject)["e"].theArray().size(),
                       0 == (*theObject)["e"].theArray().size());

               checkResult(L_,
                           "{\"a\":null, \"a1\":{\"b\":true, \"c\":123.45e+1, "
                           "\"d\":\"Hello, world!\", \"e\":[]}}",
                           jsonResult,
                           true);

               addMemberObject(theObject, "f");

               ASSERTV(jsonResult,
                       *theObject,
                       theObject->size(),
                       5 == theObject->size());
               ASSERTV(jsonResult,
                       (*theObject)["f"],
                       (*theObject)["f"].isObject(),
                       true == (*theObject)["f"].isObject());
               ASSERTV(jsonResult,
                       (*theObject)["f"],
                       (*theObject)["f"].theObject(),
                       (*theObject)["f"].theObject().size(),
                       0 == (*theObject)["f"].theObject().size());

               checkResult(L_,
                           "{\"a\":null, \"a1\":{\"b\":true, \"c\":123.45e+1, "
                           "\"d\":\"Hello, world!\", \"e\":[], \"f\":{}}}",
                           jsonResult,
                           true);
               checkResult(L_,
                           WS "{" WS "\"a\"" WS ":" WS "null" WS "," WS
                              "\"a1\"" WS ":" WS "{" WS "\"b\"" WS ":" WS
                              "true" WS "," WS "\"c\"" WS ":" WS "123.45e+1" WS
                              "," WS "\"d\"" WS ":" WS "\"Hello, world!\"" WS
                              "," WS "\"e\"" WS ":" WS "[" WS "]" WS "," WS
                              "\"f\"" WS ":" WS "{" WS "}" WS "}" WS "}",
                           jsonResult,
                           true);
            }
        }

        if (verbose)
            cout << "\n\tTest 'read' handling of 'maxNestedDepth' 'readOption'"
                 << endl;
        {
            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

            Json        json(&scratch);
            Error       err(&scratch);
            ReadOptions ro;

            const char *json65DeepArray =
                              "[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[["
                              "[[[[[[[[[[[[[[[[[["
                              "]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]"
                              "]]]]]]]]]]]]]]]]]]";

            int rc = Util::read(&json, &err, json65DeepArray, ro);
            ASSERTV(rc, 0 != rc);
            ASSERTV(json.isNull(), true == json.isNull());

            ro.setMaxNestedDepth(65);

            rc = Util::read(&json, &err, json65DeepArray, ro);
            ASSERTV(rc, 0 == rc);
            ASSERTV(json.isArray(), true == json.isArray());

            const char *json65DeepObject =
                "{"
                "\"a\":{\"a\":{\"a\":{\"a\":{\"a\":{\"a\":{\"a\":{\"a\":{"
                "\"a\":{\"a\":{\"a\":{\"a\":{\"a\":{\"a\":{\"a\":{\"a\":{"
                "\"a\":{\"a\":{\"a\":{\"a\":{\"a\":{\"a\":{\"a\":{\"a\":{"
                "\"a\":{\"a\":{\"a\":{\"a\":{\"a\":{\"a\":{\"a\":{\"a\":{"
                "\"a\":{\"a\":{\"a\":{\"a\":{\"a\":{\"a\":{\"a\":{\"a\":{"
                "\"a\":{\"a\":{\"a\":{\"a\":{\"a\":{\"a\":{\"a\":{\"a\":{"
                "\"a\":{\"a\":{\"a\":{\"a\":{\"a\":{\"a\":{\"a\":{\"a\":{"
                "\"a\":{\"a\":{\"a\":{\"a\":{\"a\":{\"a\":{\"a\":{\"a\":{"
                "}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}"
                "}}}}";

            // Reset 'json'
            json.makeNull();

            ro.reset();
            rc = Util::read(&json, &err, json65DeepObject, ro);
            ASSERTV(rc, 0 != rc);
            ASSERTV(json.isNull(), true == json.isNull());

            ro.setMaxNestedDepth(65);

            rc = Util::read(&json, &err, json65DeepObject, ro);
            ASSERTV(rc, 0 == rc);
            ASSERTV(json.isObject(), true == json.isObject());
        }
        if (verbose)
            cout << "\n\tTest 'allowTrailingText' option"
                 << endl;
        {
            static const struct {
                int         d_line;
                const char *d_text;
                bool        d_isValidAllowTrailingText;
                bool        d_isValidNoTrailingText;
            } DATA[] = {
              // L  TEXT          VAID_TT VALID_NO_TT
              //== =============  ======= ============
              { L_, "null",       true,     true }
            , { L_, "null ",      true,     true }
            , { L_, "null a",     true,    false }
            , { L_, "nulla",     false,    false }
            , { L_, "null{",      true,    false }
            , { L_, "null}",      true,    false }
            , { L_, "null[",      true,    false }
            , { L_, "null]",      true,    false }
            , { L_, "null,",      true,    false }
            , { L_, "null\"",     true,    false }
            , { L_, "true",       true,     true }
            , { L_, "true ",      true,     true }
            , { L_, "true a",     true,    false }
            , { L_, "truea",     false,    false }
            , { L_, "true{",      true,    false }
            , { L_, "true}",      true,    false }
            , { L_, "true[",      true,    false }
            , { L_, "true]",      true,    false }
            , { L_, "true,",      true,    false }
            , { L_, "true\"",     true,    false }
            , { L_, "1e1",        true,     true }
            , { L_, "1e1 ",       true,     true }
            , { L_, "1e1 e",      true,    false }
            , { L_, "1e1e",      false,    false }
            , { L_, "1e1{",       true,    false }
            , { L_, "1e1}",       true,    false }
            , { L_, "1e1[",       true,    false }
            , { L_, "1e1]",       true,    false }
            , { L_, "1e1,",       true,    false }
            , { L_, "1e1\"",      true,    false }
            , { L_, "[]",         true,     true }
            , { L_, "[] ",        true,     true }
            , { L_, "[] a",       true,    false }
            , { L_, "[]a",        true,    false }
            , { L_, "[]{",        true,    false }
            , { L_, "[]}",        true,    false }
            , { L_, "[][",        true,    false }
            , { L_, "[]]",        true,    false }
            , { L_, "[],",        true,    false }
            , { L_, "[]\"",       true,    false }
            , { L_, "{}",         true,     true }
            , { L_, "{} ",        true,     true }
            , { L_, "{} a",       true,    false }
            , { L_, "{}a",        true,    false }
            , { L_, "{}{",        true,    false }
            , { L_, "{}}",        true,    false }
            , { L_, "{}[",        true,    false }
            , { L_, "{}]",        true,    false }
            , { L_, "{},",        true,    false }
            , { L_, "{}\"",       true,    false }
            , { L_, "\"text\"",   true,     true }
            , { L_, "\"text\" ",  true,     true }
            , { L_, "\"text\" a", true,    false }
            , { L_, "\"text\"a",  true,    false }
            , { L_, "\"text\"{",  true,    false }
            , { L_, "\"text\"}",  true,    false }
            , { L_, "\"text\"[",  true,    false }
            , { L_, "\"text\"]",  true,    false }
            , { L_, "\"text\",",  true,    false }
            , { L_, "\"text\"\"", true,    false }
            };

            const int NUM_DATA = sizeof(DATA) / sizeof(DATA[0]);

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int   LINE        = DATA[ti].d_line;
                const char *TEXT        = DATA[ti].d_text;
                const bool  IS_VALID_TT = DATA[ti].d_isValidAllowTrailingText;
                const bool  IS_VALID_NO_TT = DATA[ti].d_isValidNoTrailingText;

                const bsl::string_view INPUT(TEXT);

                if (veryVerbose) {
                    T_ P_(LINE) P_(INPUT) P_(IS_VALID_TT) P(IS_VALID_NO_TT);
                }

                bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

                {
                    Json        json(&scratch);
                    Error       err(&scratch);
                    ReadOptions ro;

                    ro.setAllowTrailingText(false);

                    int rc = Util::read(&json, &err, INPUT, ro);

                    ASSERTV(LINE,
                            INPUT,
                            IS_VALID_NO_TT,
                            rc,
                            IS_VALID_NO_TT == (0 == rc));
                }

                {
                    Json        json(&scratch);
                    Error       err(&scratch);
                    ReadOptions ro;

                    ro.setAllowTrailingText(true);

                    int rc = Util::read(&json, &err, INPUT, ro);

                    ASSERTV(LINE,
                            INPUT,
                            IS_VALID_TT,
                            rc,
                            IS_VALID_TT == (0 == rc));
                }
            }
        }
        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bsls::AssertTestHandlerGuard hG;

            bdljsn::Json result;
            bsl::string_view input("false");
            Error error;

            if (veryVerbose) cout << "\tread" << endl;
            {
                ASSERT_PASS(Util::read(&result, &error, input));
                ASSERT_FAIL(Util::read(0, &error, input));
                ASSERT_FAIL(Util::read(0, &error, input));
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
        //: 1 Call 'printValue' on each value type.
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose)
            cout << "\nBREATHING TEST"
                 << "\n==============" << endl;

        ASSERT(areEqual(SIMPLE_JSON, OLD_STYLE_SIMPLE_JSON));

        const WriteStyle::Enum C = WriteStyle::e_COMPACT;
        const WriteStyle::Enum O = WriteStyle::e_ONELINE;
        const WriteStyle::Enum P = WriteStyle::e_PRETTY;

        if (verbose)
            cout << "\tBasic read and write round-trip covering all types"
                 << endl;
        {
            bslma::TestAllocator       ta;
            bdlsb::FixedMemInStreamBuf input(SIMPLE_JSON,
                                             bsl::strlen(SIMPLE_JSON));
            Error                      error(&ta);

            bdljsn::Json        result;
            const bdljsn::Json& RESULT = result;

            int rc = Util::read(&result, &error, &input, ReadOptions());

            ASSERTV(rc, error, 0 == rc);
            ASSERT(RESULT.type() == JsonType::e_OBJECT);
            ASSERT(RESULT["number"].theNumber().asDouble() == 2.1);
            ASSERT(RESULT["string"].theString() == "abc");
            ASSERT(RESULT["boolean"].theBoolean() == true);
            ASSERT(RESULT["date"].theString() == "1970-01-01");
            ASSERT(RESULT["array"][0].theNumber().asDouble() == 3.1);

            ASSERT(RESULT["array"][2].theString() == "abc");
            ASSERT(RESULT["array"][3].theBoolean() == true);

            Uint64 uintValue;
            ASSERT(0 == RESULT["integer"].theNumber().asUint64(&uintValue));
            ASSERT(uintValue == 10);

            ASSERT(0 == RESULT["array"][1].theNumber().asUint64(&uintValue));
            ASSERT(uintValue == 11);

            bsl::string resultString(&ta);

            WriteOptions options;
            options.setStyle(P);
            options.setInitialIndentLevel(0);
            options.setSpacesPerLevel(2);
            options.setSortMembers(true);
            Util::write(&resultString, RESULT, options);

            ASSERTV(resultString, SIMPLE_JSON, resultString == SIMPLE_JSON);
        }

        if (verbose) {
            bsl::cout
                << "\tTest handling of misplaced commas and incorrect endings"
                << bsl::endl;
        }
        {
            struct BadCommas {
                int            d_line;
                const char    *d_json_p;
                int            d_expectedRc;
                bsl::uint64_t  d_expectedOffset;
            } DATA[] = {
              { L_, "",                          -1,                 0 },
              { L_, ",",                         -1,                 0 },
              { L_, "{},",                       -1,                 2 },
              { L_, "{}     ,",                  -1,                 7 },
              { L_, "{}{}",                      -1,                 2 },
              { L_, "[",                         -1,                 1 },
              { L_, "{",                         -1,                 1 },
              { L_, "{\"a\":",                   -1,                 5 },
              { L_, "]",                         -1,                 0 },
              { L_, "}",                         -1,                 0 },
              { L_, ",",                         -1,                 0 },
              { L_, "{,}",                       -1,                 1 },
              { L_, "{}",                         0,                 0 },
              { L_, "[,]",                       -1,                 1 },
              { L_, "[],",                       -1,                 2 },
              { L_, "[]",                         0,                 0 },
              { L_, "[,1]",                      -1,                 1 },
              { L_, "[1,]",                      -1,                 3 },
              { L_, "[1],",                      -1,                 3 },
              { L_, "[1]",                        0,                 0 },
              { L_, "[,{}]",                     -1,                 1 },
              { L_, "[{},]",                     -1,                 4 },
              { L_, "[{}],",                     -1,                 4 },
              { L_, "[,{\"a\":1}]",              -1,                 1 },
              { L_, "[{,\"a\":1}]",              -1,                 2 },
              // TBD: why is this one returning -2?
              { L_, "[{\"a\":1,}]",              -2,                 8 },
              { L_, "[{\"a\":1},]",              -1,                 9 },
              { L_, "[{\"a\":1}],",              -1,                 9 },
              { L_, "[,[1]]",                    -1,                 1 },
              { L_, "[[,1]]",                    -1,                 2 },
              { L_, "[[1,]]",                    -1,                 4 },
              { L_, "[[1],]",                    -1,                 5 },
              { L_, "[[1]],",                    -1,                 5 },
              { L_, "true",                       0,                 0 },
              { L_, "false",                      0,                 0 },
              { L_, "null",                       0,                 0 },
              { L_, "\"abce\\\"def\"",            0,                 0 },
              { L_, "3.14159265358979",           0,                 0 },
              { L_, "true,",                     -1,                 4 },
              { L_, "false,",                    -1,                 5 },
              { L_, "null,",                     -1,                 4 },
              { L_, "\"abce\\\"def\",",          -1,                11 },
              { L_, "3.14159265358979,",         -1,                16 },
            };

            const int NUM_DATA = sizeof(DATA) / sizeof(*DATA);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int            LINE           = DATA[i].d_line;
                const char          *JSON           = DATA[i].d_json_p;
                const int            EXPECTEDRC     = DATA[i].d_expectedRc;
                const bsl::uint64_t  EXPECTEDOFFSET = DATA[i].d_expectedOffset;

                bslma::TestAllocator       ta;
                bdlsb::FixedMemInStreamBuf input(JSON, bsl::strlen(JSON));
                Error                      error(&ta);

                bdljsn::Json json;

                int rc = Util::read(&json, &error, &input, ReadOptions());

                ASSERTV(LINE, JSON, rc, EXPECTEDRC, error, EXPECTEDRC == rc);
                if (EXPECTEDRC == rc) {
                    ASSERTV(LINE,
                            JSON,
                            EXPECTEDOFFSET,
                            error.location().offset(),
                            EXPECTEDOFFSET == error.location().offset());
                }
            }
        }

        if (verbose) bsl::cout << "\tTest write styles" << bsl::endl;
        {
            struct WriteTests {
                int            d_line;
                const char    *d_json_p;
                const char    *d_expectedCompact_p;
                const char    *d_expectedOneLine_p;
                const char    *d_expectedPretty_p;
            } DATA[] = {
                // L  JSON                COMPACT        ONELINE  PRETTY
                {L_, "{}", "{}", "{}", "{}"},
                {L_, "{\"a\":1, \"b\":2}",
                             "{\"a\":1,\"b\":2}",
                                         "{\"a\": 1, \"b\": 2}",
                                                             "{\n"
                                                             "    \"a\": 1,\n"
                                                             "    \"b\": 2\n"
                                                             "}"},
                {L_, "{\"a\":1}",    "{\"a\":1}",  "{\"a\": 1}",
                                                               "{\n"
                                                               "    \"a\": 1\n"
                                                               "}"},
                {L_, "[1]",                "[1]",         "[1]", "[\n"
                                                                 "    1\n"
                                                                 "]"},
            };

            const int NUM_DATA = sizeof(DATA) / sizeof(*DATA);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE       = DATA[i].d_line;
                const char *JSON       = DATA[i].d_json_p;
                const char *EXPCOMPACT = DATA[i].d_expectedCompact_p;
                const char *EXPONELINE = DATA[i].d_expectedOneLine_p;
                const char *EXPPRETTY  = DATA[i].d_expectedPretty_p;

                bslma::TestAllocator       ta;
                bdlsb::FixedMemInStreamBuf input(JSON, bsl::strlen(JSON));
                Error                      error(&ta);

                bdljsn::Json json;

                int rc = Util::read(&json, &error, &input, ReadOptions());
                ASSERTV(LINE, JSON, json, rc, error, 0 == rc);

                WriteOptions options;
                options.setSortMembers(true);

                bsl::string compact(&ta);
                options.setStyle(C);
                options.setSortMembers(true);
                rc = Util::write(&compact, json, options);
                ASSERTV(LINE, json, rc, 0 == rc);
                ASSERTV(LINE,
                        json,
                        compact,
                        EXPCOMPACT,
                        EXPCOMPACT == compact);

                bsl::string oneline(&ta);
                options.reset();
                options.setStyle(O);
                options.setSortMembers(true);
                rc = Util::write(&oneline, json, options);
                ASSERTV(LINE, json, rc, 0 == rc);
                ASSERTV(LINE,
                        json,
                        oneline,
                        EXPONELINE,
                        EXPONELINE == oneline);

                bsl::string pretty(&ta);
                options.reset();
                options.setStyle(P);
                options.setSortMembers(true);
                rc = Util::write(&pretty, json, options);
                ASSERTV(LINE, json, rc, 0 == rc);
                ASSERTV(LINE, json, pretty, EXPPRETTY, EXPPRETTY == pretty);
            }
        }

        if (verbose) {
            bsl::cout << "\tTest ReadOptions.maxNestedDepth" << bsl::endl;
        }
        {
            {
                const int   JSON_DEPTH = 5;
                const char *ARRAYS     = "[[[[[]]]]]";
                const char *OBJECTS    = "{\"a\":{\"a\":{\"a\":{\"a\":{}}}}}";
                const char *MIX        = "[{\"a\":[{\"a\":[]}]}]";

                const char *DATA[]   = {ARRAYS, OBJECTS, MIX};
                const int   NUM_DATA = sizeof(DATA) / sizeof(*DATA);

                for (int i = 0; i < NUM_DATA; ++i) {
                    for (int depth = 1; depth < JSON_DEPTH + 2; ++depth) {
                        const char *J = DATA[i];

                        bslma::TestAllocator       ta;
                        bdlsb::FixedMemInStreamBuf input(J, bsl::strlen(J));
                        Error                      error(&ta);

                        bdljsn::Json result;
                        ReadOptions  options;
                        options.setMaxNestedDepth(depth);
                        int rc = Util::read(&result, &error, &input, options);
                        ASSERTV(J,
                                depth,
                                rc,
                                (JSON_DEPTH <= depth) == (0 == rc));
                    }
                }
            }

            // Verify that default options value of 64 is handled correctly.
            {
                const char *ARRAYS_NEST_64 =
                                  "[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[["
                                  "[[[[[[[[[[[[[[[[[[[[[]]]]]]]]]]]]]]]]]]]]]]"
                                  "]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]";
                const char *OBJECTS_NEST_64 =
                    "{\"a\":{\"a\":{\"a\":{\"a\":{\"a\":{\"a\":{\"a\":{\"a\":{"
                    "\"a\":{\"a\":{\"a\":{\"a\":{\"a\":{\"a\":{\"a\":{\"a\":{"
                    "\"a\":{\"a\":{\"a\":{\"a\":{\"a\":{\"a\":{\"a\":{\"a\":{"
                    "\"a\":{\"a\":{\"a\":{\"a\":{\"a\":{\"a\":{\"a\":{\"a\":{"
                    "\"a\":{\"a\":{\"a\":{\"a\":{\"a\":{\"a\":{\"a\":{\"a\":{"
                    "\"a\":{\"a\":{\"a\":{\"a\":{\"a\":{\"a\":{\"a\":{\"a\":{"
                    "\"a\":{\"a\":{\"a\":{\"a\":{\"a\":{\"a\":{\"a\":{\"a\":{"
                    "\"a\":{\"a\":{\"a\":{\"a\":{\"a\":{\"a\":{\"a\":{}}}}}}}}"
                    "}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}";

                const char *DATA[]   = {ARRAYS_NEST_64, OBJECTS_NEST_64};
                const int   NUM_DATA = sizeof(DATA) / sizeof(*DATA);

                for (int i = 0; i < NUM_DATA; ++i) {
                    const char *J = DATA[i];

                    // Works with default 'options.maxNestedDepth()'
                    {
                        bslma::TestAllocator       ta;
                        bdlsb::FixedMemInStreamBuf input(J, bsl::strlen(J));
                        Error                      error(&ta);

                        bdljsn::Json result;
                        ReadOptions  options;
                        ASSERTV(options.maxNestedDepth(),
                                64 == options.maxNestedDepth());
                        int rc = Util::read(&result, &error, &input, options);
                        ASSERTV(J,
                                rc,
                                options.maxNestedDepth(),
                                (0 == rc));
                    }

                    // Fails with 'options.maxNestedDepth() == 63'
                    {
                        bslma::TestAllocator       ta;
                        bdlsb::FixedMemInStreamBuf input(J, bsl::strlen(J));
                        Error                      error(&ta);

                        bdljsn::Json result;
                        ReadOptions  options;
                        options.setMaxNestedDepth(63);
                        int rc = Util::read(&result, &error, &input, options);
                        ASSERTV(J,
                                rc,
                                options.maxNestedDepth(),
                                (0 != rc));
                    }
                }
            }
        }

        if (verbose) {
            cout << "\tRead with/without options overload check" << endl;
        }
        {
            // TBD: repeat this for 'istream', 'streambuf', and 'string_view'.
            if (veryVerbose) cout << "... bsl::string input" << endl;
            {
                bslma::TestAllocator ta;
                const bsl::string    input(SIMPLE_JSON,
                                           bsl::strlen(SIMPLE_JSON),
                                           &ta);
                Error                error(&ta);

                bdljsn::Json        result;
                const bdljsn::Json& RESULT = result;

                if (veryVeryVerbose)
                    cout << "\t... with error argument" << endl;
                {
                    int rc = Util::read(&result, &error, input, ReadOptions());

                    ASSERTV(rc, error, 0 == rc);

                    bdljsn::Json        result2;
                    const bdljsn::Json& RESULT2 = result2;

                    int rc2 = Util::read(&result2, &error, input);

                    ASSERTV(rc2, error, 0 == rc2);
                    ASSERTV(RESULT, RESULT2, RESULT == RESULT2);
                }

                if (veryVeryVerbose)
                    cout << "\t... without error argument" << endl;
                {
                    int rc = Util::read(&result, &error, input, ReadOptions());

                    ASSERTV(rc, 0 == rc);

                    bdljsn::Json        result2;
                    const bdljsn::Json& RESULT2 = result2;

                    int rc2 = Util::read(&result2, input);

                    ASSERTV(rc2, 0 == rc2);
                    ASSERTV(RESULT, RESULT2, RESULT == RESULT2);
                }
            }
        }

        if (verbose) {
            bsl::cout << "\tTesting formatting options" << bsl::endl;
        }
        const char *FMT_INPUT = "{\"a\":{\"b\":1,\"c\":[1,2]}}";
        const char *FMT_RESULT_C = "{\"a\":{\"b\":1,\"c\":[1,2]}}";
        const char *FMT_RESULT_P00 = "{\"a\": {\"b\": 1, \"c\": [1, 2]}}";

        const char *FMT_RESULT_P11 = " {\n"
                                     "  \"a\": {\n"
                                     "   \"b\": 1,\n"
                                     "   \"c\": [\n"
                                     "    1,\n"
                                     "    2\n"
                                     "   ]\n"
                                     "  }\n"
                                     " }";
        const char *FMT_RESULT_P23 = "      {\n"
                                     "         \"a\": {\n"
                                     "            \"b\": 1,\n"
                                     "            \"c\": [\n"
                                     "               1,\n"
                                     "               2\n"
                                     "            ]\n"
                                     "         }\n"
                                     "      }";

        struct WriteInput {
            int               d_line;
            const char       *d_json_p;
            WriteStyle::Enum  d_style;
            int               d_indentation;
            int               d_spacesPerLevel;
            const char       *d_expected_p;
        } DATA[] = {{L_, FMT_INPUT, C, 0, 0, FMT_RESULT_C},
                    {L_, FMT_INPUT, C, 1, 1, FMT_RESULT_C},
                    {L_, FMT_INPUT, O, 0, 0, FMT_RESULT_P00},
                    {L_, FMT_INPUT, P, 1, 1, FMT_RESULT_P11},
                    {L_, FMT_INPUT, P, 2, 3, FMT_RESULT_P23}};

        const int NUM_DATA = sizeof(DATA) / sizeof(*DATA);

        for (int i = 0; i < NUM_DATA; ++i) {
            const int    LINE     = DATA[i].d_line;
            const char  *JSON     = DATA[i].d_json_p;
            const Style  STYLE    = DATA[i].d_style;
            const int    INDENT   = DATA[i].d_indentation;
            const int    SPACES   = DATA[i].d_spacesPerLevel;
            const char  *EXPECTED = DATA[i].d_expected_p;

            bslma::TestAllocator       ta;
            bdlsb::FixedMemInStreamBuf input(JSON, bsl::strlen(JSON));
            Error                      error(&ta);

            bdljsn::Json        json;

            int rc = Util::read(&json, &error, &input, ReadOptions());

            ASSERTV(rc, error, 0 == rc);

            bsl::string resultString(&ta);

            WriteOptions options;
            options.setStyle(STYLE);
            options.setInitialIndentLevel(INDENT);
            options.setSpacesPerLevel(SPACES);
            options.setSortMembers(true);
            Util::write(&resultString, json, options);

            if (veryVerbose) {
                P(LINE); P(JSON); P(EXPECTED); P(resultString);
            }
            ASSERTV(LINE,
                    JSON,
                    STYLE,
                    INDENT,
                    SPACES,
                    EXPECTED,
                    resultString,
                    areEqual(resultString, EXPECTED));
        }

        if (verbose) {
            bsl::cout << "\tTesting basic error reporting" << bsl::endl;
        }
        struct ErrorInput {
            int          d_line;
            const char  *d_json_p;
            bsl::size_t  d_errorLocation;
        } ERROR_DATA[] = {
            {L_, "]", 0},
            {L_, "[", 1},
            {L_, "}", 0},
            {L_, "{", 1},
            {L_, "{\"foo\" \"bar\"}", 7},
            {L_, "{\"foo\", 1234\"1231\"}", 6},
            {L_, "[] 1", 3},
        };
        const int NUM_ERROR_DATA = sizeof(ERROR_DATA)/sizeof(*ERROR_DATA);
        for (int i = 0; i < NUM_ERROR_DATA; ++i) {
            const int          LINE     = ERROR_DATA[i].d_line;
            const char        *JSON     = ERROR_DATA[i].d_json_p;
            const bsl::size_t  location = ERROR_DATA[i].d_errorLocation;

            bdljsn::Json junk;
            Error        result;

            int rc = Util::read(&junk, &result, JSON, ReadOptions());

            ASSERTV(rc, 0 != rc);

            if (veryVerbose) {
                P(JSON); P_(LINE); P(result);
            }
            ASSERTV(LINE, JSON, result, !result.message().empty());
            ASSERTV(LINE,
                    JSON,
                    result,
                    location == result.location().offset());
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
