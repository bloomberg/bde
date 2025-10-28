// baljsn_jsontokenizer.t.cpp                                         -*-C++-*-
#include <baljsn_jsontokenizer.h>

#include <bdljsn_jsonutil.h>
#include <bdljsn_numberutil.h>
#include <bdljsn_stringutil.h>

#include <bdlde_utf8util.h>

#include <bdlsb_fixedmeminstreambuf.h>
#include <bdlsb_fixedmemoutstreambuf.h>
#include <bdlsb_memoutstreambuf.h>

#include <bsla_maybeunused.h>
#include <bsla_fallthrough.h>

#include <bslim_printer.h>
#include <bslim_testutil.h>

#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>
#include <bslma_testallocatormonitor.h>

#include <bsls_assert.h>
#include <bsls_asserttest.h>
#include <bsls_keyword.h>
#include <bsls_review.h>

#include <bsl_algorithm.h>
#include <bsl_cfloat.h>
#include <bsl_climits.h>
#include <bsl_cstddef.h>    // `bsl::size_t`
#include <bsl_cstdlib.h>
#include <bsl_cstring.h>
#include <bsl_iostream.h>
#include <bsl_limits.h>
#include <bsl_list.h>
#include <bsl_ostream.h>
#include <bsl_set.h>
#include <bsl_sstream.h>
#include <bsl_streambuf.h>
#include <bsl_string.h>
#include <bsl_vector.h>

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
// The component under test implements a tokenizer for traversing a stream
// filled with JSON data and allows populating an in-memory structure with
// almost no memory allocations.  The implementation works as a finite state
// machine moving from one token to another when the `advanceToNextToken`
// function is called.  The majority of this test driver tests that function by
// starting at a particular token, calling that function, and ensuring that
// after the advance the next token and the data value is as expected.
// ----------------------------------------------------------------------------
// CREATORS
// [ 2] JsonTokenizer(bslma::Allocator       *basicAllocator = 0);
// [ 2] JsonTokenizer(const bdljsn::Json     *json, *bA = 0);
// [ 2] ~JsonTokenizer();
//
// MANIPULATORS
// [**] int advanceToNextToken();
// [ 6] int reset(const bdljsn::Json *json);
//
// ACCESSORS
// [**] TokenType tokenType() const;
// [**] int value(bsl::string_view *data) const;
// [**] int value(const bdljsn::Json **data) const;
//
// [ 2] allocator_type get_allocator() const;
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 7] USAGE EXAMPLE
// [ 3] CONCERN: ARRAY/OBJECT OF SCALARS
// [ 4] CONCERN: ARRAY  CONTAINING ARRAYS/OBJECTS
// [ 5] CONCERN: OBJECT CONTAINING ARRAYS/OBJECTS

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
#define ASSERTV BSLIM_TESTUTIL_ASSERTV

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

// ============================================================================
//                   GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------

typedef baljsn::JsonTokenizer            Obj;
typedef baljsn::JsonTokenizer::TokenType TokenType;
typedef bdljsn::Json                     Json;
typedef bdljsn::JsonNumber               JsonNumber;
typedef bdljsn::JsonObject               Object;

// ============================================================================
//                               MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int                 test = argc > 1 ? atoi(argv[1]) : 0;
    bool             verbose = argc > 2;
    bool         veryVerbose = argc > 3;
    bool     veryVeryVerbose = argc > 4;
    bool veryVeryVeryVerbose = argc > 5;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    bslma::TestAllocator globalAllocator("global", veryVeryVeryVerbose);
    bslma::Default::setGlobalAllocator(&globalAllocator);

    switch (test) { case 0:  // Zero is always the leading case.
      case 7: {
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

        if (verbose) cout << endl
                          << "USAGE EXAMPLE" << endl
                          << "=============" << endl;

    const char *const JsonDoc =
                     "{ \"name\"        : \"Bob\"                          \n"
                     ", \"homeAddress\" : { \"street\" : \"Lexington Ave\" \n"
                     "                  , \"city\"   : \"New York City\"   \n"
                     "                  , \"state\"  : \"New York\"        \n"
                     "                    }                                \n"
                     ", \"age\"         : 21                               \n"
                     "}                                                    \n";

///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Basic Usage
///----------------------
// The `baljsn::JsonTokenizer` allows the user to iteratively examine each of
// the piece-parts of a `bdljsn::Json` object having arbitrary complexity
// --- containing subordinate `JsonArray`s/`JsonObject`s --- as well as the
// scalar `Json` types.  Suppose one must visualize a `bdljsn::Json` object
// having modest complexity:
// ```
// { "name"        : "Bob",
//   "homeAddress" : { "street" : "Lexington Ave",
//                     "city"   : "New York City",
//                     "state"  : "New York"
//                     },
//   "age"         : 21
// }
// ```
// First, create `JsonDoc`, an ASCII string an representation of the above JSON
// document (having required escape sequences and quoting) -- elided.
// ```
// Then, create `json`, the programmatic representation of the JSON document.
// ```
    bdljsn::Json json;
    int          rc = bdljsn::JsonUtil::read(&json, JsonDoc);
    ASSERT(0 == rc);
// ```
// Next, define two macros and an array of indentation strings to simplify the
// formatting code.
// ```
    const char *const indents[] = { ""   // Four spaces per level.
                                  , "    "
                                  , "        "
                                  , "                "
                                  };

    #define PRINT(L, X) bsl::cout << indents[L] << (X) << bsl::endl;

    #define PRINTV(L, X, V) bsl::cout            \
                            << indents[L]        \
                            << (X) << ": "       \
                            << (V) << bsl::endl;

    int level = 0; // indentation level
// ```
// Then, create `tokenizer`, a baljsn::JsonTokenizer`, that will generate
// a sequence of tokens (and sometimes values as well) for the previously
// initialized `json` object.
// ```
    baljsn::JsonTokenizer tokenizer(&json);

    while (0 == tokenizer.advanceToNextToken()) {
// ```
// Now, for each token in the sequence, dispatch to the appropriate action.
// In each case, we output the value of the token at the current level of
// indentation.
//
// For tokens that indicate the start of an `JsonArray` or `JsonObject`, we
// increase the level of indentation.  That level is decreased when the end of
// the `JsonArray` or `JsonObject` is reported.
//
// We expect two tokens to be generated for each member of a `JsonObject`: the
// element name and an associated value.  In such cases we use the `value`
// method of extract the name (provided via `bsl::string_view`) and the value
// (itself a `bdljsn::Json` object).
//
// Be aware that a `e_ELEMENT_VALUE` is also returned for each scalar element
// of a `JsonArray` -- not featured in this example.
// ```
        baljsn::JsonTokenizer::TokenType tokenType = tokenizer.tokenType();

        switch(tokenType) {
            case baljsn::JsonTokenizer::e_START_ARRAY:   {
                PRINT(level, tokenType);
                ++level;
            } break;
            case baljsn::JsonTokenizer::e_END_ARRAY:     {
                --level;
                PRINT(level, tokenType);
            } break;
            case baljsn::JsonTokenizer::e_START_OBJECT:  {
                PRINT(level, tokenType);
                ++level;
            } break;
            case baljsn::JsonTokenizer::e_END_OBJECT:    {
                --level;
                PRINT(level, tokenType);
            } break;
            case baljsn::JsonTokenizer::e_ELEMENT_NAME:  {
                bsl::string_view sv;
                int              rc = tokenizer.value(&sv);
                ASSERT(0 == rc);

                PRINTV(level, tokenType, sv);
            } break;
            case baljsn::JsonTokenizer::e_ELEMENT_VALUE: {
                const bdljsn::Json *jp;
                int                 rc = tokenizer.value(&jp);
                ASSERT(0 == rc);
                ASSERT( jp);
                ASSERT(!jp->isArray());
                ASSERT(!jp->isObject());

                PRINTV(level, tokenType, *jp);
            } break;
            case baljsn::JsonTokenizer::e_ERROR:         {
                PRINT(level, tokenType);
            } break;
            default: {
                ASSERT(false && "reachable");
            }
        }
    }
// ```
// Notice, that the `e_ELEMENT_VALUE` is always associated with a scalar
// `bdljsn::Json` object.  If the value portion of a member is itself an
// `JsonArray` (or `JsonObject`) then expect `e_START_ARRAY` (or
// `e_START_OBJECT`) at that point.
//
// Finally, we can inspect the output for this `bdljsn::Json` object:
// ```
//  START_OBJECT
//      ELEMENT_NAME: homeAddress
//      START_OBJECT
//          ELEMENT_NAME: state
//          ELEMENT_VALUE: "New York"
//          ELEMENT_NAME: city
//          ELEMENT_VALUE: "New York City"
//          ELEMENT_NAME: street
//          ELEMENT_VALUE: "Lexington Ave"
//      END_OBJECT
//      ELEMENT_NAME: age
//      ELEMENT_VALUE: 21
//      ELEMENT_NAME: name
//      ELEMENT_VALUE: "Bob"
//  END_OBJECT
// ```
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TEST `reset`
        //
        // Concern:
        // 1. The `reset` method can set a tokenizer to process a `Json`
        //    object in the following contexts:
        //
        //    1. A default constructed tokenizer.
        //
        //    2. A tokenizer that has completed the tokenization of a `Json`
        //       object.
        //
        //    3. A tokenizer that has started but not yet completed the
        //       tokenization of a `Json` object.
        //
        // 2. `reset` does not leak memory.
        //
        // 3. QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        // 1. A series of ad hoc tests that setup each of the three contexts
        //    and confirms the expected results.
        //
        // 2. Each of the tokenizers used in P-1 is configured to use the same
        //    `bslma::TestAllocator`.  After the three tests the test allocator
        //    is checked to see that memory has been used and also all has
        //    been deallocated.
        //
        // 3. Negative testing using the `BSLS_ASSERT_TEST_*` macros to confirm
        //    checking of preconditions.
        //
        // Testing:
        //   int reset(const bdljsn::Json *json);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TEST `reset`" << endl
                          << "============" << endl;
        if (veryVerbose)
                     cout << "`reset` a default constructed tokenizer" << endl;

        Json jsonEmptyArray;  jsonEmptyArray .makeArray();
        Json jsonEmptyObject; jsonEmptyObject.makeObject();

        bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);

        if (veryVerbose)
                     cout << "`reset` a default constructed tokenizer" << endl;
        {
            Obj obj(&sa);
            obj.reset(&jsonEmptyObject);                                // TEST

            for (int i = 0; 0 == obj.advanceToNextToken(); ++i) {
                TokenType tokenType = obj.tokenType();

                if (veryVerbose) {
                    T_; P_(i); P(tokenType);
                }

                       if (0 == i) {
                        ASSERTV(tokenType, Obj::e_START_OBJECT   == tokenType);
                } else if (1 == i) {
                        ASSERTV(tokenType, Obj::e_END_OBJECT     == tokenType);
                } else {
                        ASSERT(false && "reachable");
                }
            }
        }

        if (veryVerbose) cout << "`reset` tokenizer in end state" << endl;
        {
            Obj obj(&jsonEmptyObject, &sa);

            for (int i = 0; 0 == obj.advanceToNextToken(); ++i) {
                if (veryVerbose) {
                    T_; P(i);
                }
            }

            obj.reset(&jsonEmptyArray);                                 // TEST

            for (int i = 0; 0 == obj.advanceToNextToken(); ++i) {
                TokenType tokenType = obj.tokenType();

                if (veryVerbose) {
                    T_; P_(i); P(tokenType);
                }

                       if (0 == i) {
                        ASSERTV(tokenType, Obj::e_START_ARRAY    == tokenType);
                } else if (1 == i) {
                        ASSERTV(tokenType, Obj::e_END_ARRAY      == tokenType);
                } else {
                        ASSERT(false && "reachable");
                }
            }
        }

        if (veryVerbose) cout << "`reset` tokenizer in mid state" << endl;
        {
            Obj obj(&jsonEmptyObject, &sa);

            for (int i = 0; i < 1 && 0 == obj.advanceToNextToken(); ++i) {
                         // ^^^^^
                if (veryVerbose) {
                    T_; P(i);
                }
                ASSERTV(i, i < 1);
            }

            obj.reset(&jsonEmptyArray);                                 // TEST

            for (int i = 0; 0 == obj.advanceToNextToken(); ++i) {
                TokenType tokenType = obj.tokenType();

                if (veryVerbose) {
                    T_; P_(i); P(tokenType);
                }

                       if (0 == i) {
                        ASSERTV(tokenType, Obj::e_START_ARRAY    == tokenType);
                } else if (1 == i) {
                        ASSERTV(tokenType, Obj::e_END_ARRAY      == tokenType);
                } else {
                        ASSERT(false && "reachable");
                }
            }
        }

        ASSERTV(sa.numBlocksTotal(), 0 <  sa.numBlocksTotal());
        ASSERTV(sa.numBlocksInUse(), 0 == sa.numBlocksInUse());

        if (veryVerbose) cout << "Negative tests" << endl;
        {
            bsls::AssertTestHandlerGuard hg;

            Obj  obj;
            Json json;

            json.makeString("");
            ASSERT_FAIL((obj.reset(&json)));

            json.makeNumber();
            ASSERT_FAIL((obj.reset(&json)));

            json.makeBoolean();
            ASSERT_FAIL((obj.reset(&json)));

            json.makeNull();
            ASSERT_FAIL((obj.reset(&json)));

            json.makeArray();
            ASSERT_PASS((obj.reset(&json)));

            json.makeObject();
            ASSERT_PASS((obj.reset(&json)));
        }
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // OBJECT CONTAINING ARRAYS/OBJECTS
        //
        // Concerns:
        // 1. The tokenizer can process an empty object.
        //
        // 2. The tokenizer can process an object having members whose
        //    value portion consits of an array (an object).
        //
        // Plan:
        // 1. Create an empty object and use `advanceToNextToken` to obtain the
        //    tokens.  Confirm that they are in the expected order.
        //
        // 2. Create an object having four members whose values are each an
        //    array (an object).  Confirm that tokenizer produces tokens
        //    having the expected order, type, and value.
        //
        //    1. Note that when examing the tokens from the objects, the
        //       order of the members is not specified.
        //
        // Testing:
        //   CONCERN: OBJECT CONTAINING ARRAYS/OBJECTS
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "OBJECT CONTAINING ARRAYS/OBJECTS" << endl
                          << "================================" << endl;

        if (veryVerbose) cout << "Empty Object" << endl;
        {
            Json json; json.makeObject();
            Obj  obj(&json);

            for (int i = 0; 0 == obj.advanceToNextToken(); ++i) {
                TokenType tokenType = obj.tokenType();

                if (veryVerbose) {
                    T_; P_(i); P(tokenType);
                }

                       if (0 == i) {
                        ASSERTV(tokenType, Obj::e_START_OBJECT   == tokenType);
                } else if (1 == i) {
                        ASSERTV(tokenType, Obj::e_END_OBJECT     == tokenType);
                } else {
                        ASSERT(false && "reachable");
                }
            }
        }

        if (veryVerbose) cout << "Object Containing Array" << endl;
        {
            Json array1; array1.makeArray();
            array1.theArray().pushBack(Json("a"));

            Json json; json.makeObject();
            json["array1"] = array1;

            ASSERTV(json.size(), 1 == json.size());

            Obj obj(&json);

            for (int i = 0; 0 == obj.advanceToNextToken(); ++i) {

                TokenType tokenType = obj.tokenType();

                if (veryVerbose) {
                    T_; P_(i); P(tokenType);
                }

                switch (i) {
                  // ----------------------------------------------------------
                  case  0: { // 'json': start
                        ASSERTV(tokenType, Obj::e_START_OBJECT   == tokenType);
                  } break;
                  // ----------------------------------------------------------
                  case  1: {     // 'json': member 1 of 1: name
                        ASSERTV(tokenType, Obj::e_ELEMENT_NAME   == tokenType);
                        bsl::string_view memberName;
                        int rc = obj.value(&memberName);
                        ASSERTV(rc, 0 == rc);
                        ASSERTV(            memberName,
                                "array1" == memberName);
                  } break;
                  // ----------------------------------------------------------
                  case  2: {   // 'array1': start
                        ASSERTV(tokenType, Obj::e_START_ARRAY    == tokenType);
                  } break;
                  case  3: {     // 'array1': element 1 of 1
                        ASSERTV(tokenType, Obj::e_ELEMENT_VALUE  == tokenType);
                        const Json *element;
                        int rc = obj.value(&element);
                        ASSERTV(rc, 0 == rc);
                        ASSERTV(       element->theString(),
                                "a" == element->theString());
                  } break;
                  case  4: {   // 'array1': end
                        ASSERTV(tokenType, Obj::e_END_ARRAY      == tokenType);
                  } break;
                  // ----------------------------------------------------------
                  case  5: { // 'json': end
                        ASSERTV(tokenType, Obj::e_END_OBJECT     == tokenType);
                  } break;
                  default: {
                        ASSERT(false && "reachable");
                  } break;
                }
            }
        }

        if (veryVerbose) cout << "Object Containing Object" << endl;
        {
            Json object1; object1.makeObject();
            object1["A"] = "a";

            if (veryVeryVerbose) {
                P(object1);
            }

            Json json; json.makeObject();
            json["object1"] = object1;

            ASSERTV(json.size(), 1 == json.size());

            Obj obj(&json);

            if (veryVeryVerbose) {
                P(json);
            }

            for (int i = 0; 0 == obj.advanceToNextToken(); ++i) {

                TokenType tokenType = obj.tokenType();

                if (veryVerbose) {
                    T_; P_(i); P(tokenType);
                }

                switch (i) {
                  // ----------------------------------------------------------
                  case  0: { // 'json': start
                        ASSERTV(tokenType, Obj::e_START_OBJECT   == tokenType);
                  } break;
                  // ----------------------------------------------------------
                  case  1: { // 'json': member 1 of 1: name
                        ASSERTV(tokenType, Obj::e_ELEMENT_NAME   == tokenType);
                        bsl::string_view memberName;
                        int              rc = obj.value(&memberName);
                        ASSERTV(rc, 0 == rc);
                        ASSERTV(             memberName,
                                "object1" == memberName);
                  } break;
                  case  2: {   // 'object1': start
                        ASSERTV(tokenType, Obj::e_START_OBJECT   == tokenType);
                  } break;
                  // ..........................................................
                  case  3: {     // 'object1': member 1 of 1: name
                        ASSERTV(tokenType, Obj::e_ELEMENT_NAME   == tokenType);
                        bsl::string_view memberName;
                        int              rc = obj.value(&memberName);
                        ASSERTV(rc, 0 == rc);
                        ASSERTV(       memberName,
                                "A" == memberName);
                  } break;
                  case  4: {     // 'object1': member 1 of 1: value
                        ASSERTV(tokenType, Obj::e_ELEMENT_VALUE  == tokenType);
                        const Json *memberValue;
                        int         rc = obj.value(&memberValue);
                        ASSERTV(rc, 0 == rc);
                        ASSERTV(       memberValue->theString(),
                                "a" == memberValue->theString());
                  } break;
                  // ..........................................................
                  case  5: {   // 'object1': end
                        ASSERTV(tokenType, Obj::e_END_OBJECT     == tokenType);
                  } break;
                  // ----------------------------------------------------------
                  case  6: { // 'json': end
                        ASSERTV(tokenType, Obj::e_END_OBJECT     == tokenType);
                  } break;
                  // ----------------------------------------------------------
                  default: {
                        ASSERT(false && "reachable");
                  } break;
                }
            }
        }
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // ARRAY CONTAINING ARRAYS/OBJECTS
        //
        // Concerns:
        // 1. The tokenizer can process an empty array.
        //
        // 2. The tokenizer can process an array whose elements are themselves
        //    arrays (objects).
        //
        // Plan:
        // 1. Create an empty array and use `advanceToNextToken` to obtain the
        //    tokens.  Confirm that they are in the expected order.
        //
        // 2. Create an array having four elements that are each an array
        //    (object).  The elements should have sizes of 0, 1, 2, and 3
        //    elements (members).  Confirm that tokenizer produces tokens
        //    having the expected order, type, and value.
        //
        //    1. Note that when examing the tokens from the objects, the
        //       order of the members is not specified.
        //
        // Testing:
        //   CONCERN: ARRAY CONTAINING ARRAYS/OBJECTS
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "ARRAY CONTAINING ARRAYS/OBJECTS" << endl
                          << "===============================" << endl;

        if (veryVerbose) cout << "Empty Array" << endl;
        {
            Json json; json.makeArray();
            Obj  obj(&json);

            for (int i = 0; 0 == obj.advanceToNextToken(); ++i) {
                TokenType tokenType = obj.tokenType();

                if (veryVerbose) {
                    T_; P_(i); P(tokenType);
                }

                       if (0 == i) {
                        ASSERTV(tokenType, Obj::e_START_ARRAY   == tokenType);
                } else if (1 == i) {
                        ASSERTV(tokenType, Obj::e_END_ARRAY     == tokenType);
                } else {
                        ASSERT(false && "reachable");
                }
            }
        }

        if (veryVerbose) cout << "Array Containing Arrays" << endl;
        {
            Json array0; array0.makeArray();
            Json array1; array1.makeArray();
            Json array2; array2.makeArray();
            Json array3; array3.makeArray();

            array1.theArray().pushBack(Json("a"));

            array2.theArray().pushBack(Json("a"));
            array2.theArray().pushBack(Json("b"));

            array3.theArray().pushBack(Json("a"));
            array3.theArray().pushBack(Json("b"));
            array3.theArray().pushBack(Json("c"));

            Json json; json.makeArray();
            json.theArray().pushBack(array0);
            json.theArray().pushBack(array1);
            json.theArray().pushBack(array2);
            json.theArray().pushBack(array3);

            Obj obj(&json);

            for (int i = 0; 0 == obj.advanceToNextToken(); ++i) {

                TokenType tokenType = obj.tokenType();

                if (veryVerbose) {
                    T_; P_(i); P(tokenType);
                }

                switch (i) {
                  // ----------------------------------------------------------
                  case  0: { // 'json': start
                        ASSERTV(tokenType, Obj::e_START_ARRAY    == tokenType);
                  } break;
                  // ----------------------------------------------------------
                  case  1: {   // 'array0': start
                        ASSERTV(tokenType, Obj::e_START_ARRAY    == tokenType);
                  } break;
                  case  2: {   // 'array0': end
                        ASSERTV(tokenType, Obj::e_END_ARRAY      == tokenType);
                  } break;
                  // ----------------------------------------------------------
                  case  3: {   // 'array1' start
                        ASSERTV(tokenType, Obj::e_START_ARRAY    == tokenType);
                  } break;
                  case  4: {     // 'array1': element 1 of 1
                        ASSERTV(tokenType, Obj::e_ELEMENT_VALUE  == tokenType);
                        const Json *element;
                        int rc = obj.value(&element);
                        ASSERTV(rc, 0 == rc);
                        ASSERTV(       element->theString(),
                                "a" == element->theString());
                  } break;
                  case  5: {   // 'array1' end
                        ASSERTV(tokenType, Obj::e_END_ARRAY      == tokenType);
                  } break;
                  // ----------------------------------------------------------
                  case  6: {   // 'array2' start
                        ASSERTV(tokenType, Obj::e_START_ARRAY    == tokenType);
                  } break;
                  case  7: {     // 'array2': element 1 of 2
                        ASSERTV(tokenType, Obj::e_ELEMENT_VALUE  == tokenType);
                        const Json *element;
                        int rc = obj.value(&element);
                        ASSERTV(rc, 0 == rc);
                        ASSERTV(       element->theString(),
                                "a" == element->theString());
                  } break;
                  case  8: {     // 'array2': element 2 of 2
                        ASSERTV(tokenType, Obj::e_ELEMENT_VALUE  == tokenType);
                        const Json *element;
                        int rc = obj.value(&element);
                        ASSERTV(rc, 0 == rc);
                        ASSERTV(       element->theString(),
                                "b" == element->theString());
                  } break;
                  case  9: {   // array2: end
                        ASSERTV(tokenType, Obj::e_END_ARRAY      == tokenType);
                  } break;
                  // ----------------------------------------------------------
                  case 10: {   // 'array3': start
                        ASSERTV(tokenType, Obj::e_START_ARRAY    == tokenType);
                  } break;
                  case 11: {     // 'array3': element 1 of 3
                        ASSERTV(tokenType, Obj::e_ELEMENT_VALUE  == tokenType);
                        const Json *element;
                        int rc = obj.value(&element);
                        ASSERTV(rc, 0 == rc);
                        ASSERTV(       element->theString(),
                                "a" == element->theString());
                  } break;
                  case 12: {     // 'array3': element 2 of 3
                        ASSERTV(tokenType, Obj::e_ELEMENT_VALUE  == tokenType);
                        const Json *element;
                        int rc = obj.value(&element);
                        ASSERTV(rc, 0 == rc);
                        ASSERTV(       element->theString(),
                                "b" == element->theString());
                  } break;
                  case 13: {     // 'array3': element 3 of 3
                        ASSERTV(tokenType, Obj::e_ELEMENT_VALUE  == tokenType);
                        const Json *element;
                        int rc = obj.value(&element);
                        ASSERTV(rc, 0 == rc);
                        ASSERTV(       element->theString(),
                                "c" == element->theString());
                  } break;
                  case 14: {   // 'array3': end
                        ASSERTV(tokenType, Obj::e_END_ARRAY      == tokenType);
                  } break;
                  // ----------------------------------------------------------
                  case 15: { // 'json': end
                        ASSERTV(tokenType, Obj::e_END_ARRAY      == tokenType);
                  } break;
                  // ----------------------------------------------------------
                  default: {
                        ASSERT(false && "reachable");
                  } break;
                }
            }
        }

        if (veryVerbose) cout << "Array Containing Objects" << endl;
        {
            Json object0; object0.makeObject();
            Json object1; object1.makeObject();
            Json object2; object2.makeObject();
            Json object3; object3.makeObject();

            object1["A"] = "a";

            object2["A"] = "a";
            object2["B"] = "b";

            object3["A"] = "a";
            object3["B"] = "b";
            object3["C"] = "c";

            if (veryVeryVerbose) {
                P(object0);
                P(object1);
                P(object2);
                P(object3);
            }

            Json json; json.makeArray();
            json.theArray().pushBack(object0);
            json.theArray().pushBack(object1);
            json.theArray().pushBack(object2);
            json.theArray().pushBack(object3);

            if (veryVeryVerbose) {
                P(json);
            }

            Obj              obj(&json);
            bsl::string_view savedMemberName;

            for (int i = 0; i <= 12 && 0 == obj.advanceToNextToken(); ++i) {

                TokenType tokenType = obj.tokenType();

                if (veryVerbose) {
                    T_; P_(i); P(tokenType);
                }

                switch (i) {
                  // ----------------------------------------------------------
                  case  0: { // 'json': start
                        ASSERTV(tokenType, Obj::e_START_ARRAY    == tokenType);
                  } break;
                  // ----------------------------------------------------------
                  case  1: {   // 'object0': start
                        ASSERTV(tokenType, Obj::e_START_OBJECT   == tokenType);
                  } break;
                  case  2: {   // 'object0': end
                        ASSERTV(tokenType, Obj::e_END_OBJECT     == tokenType);
                  } break;
                  // ----------------------------------------------------------
                  case  3: {   // 'object1' start
                        ASSERTV(tokenType, Obj::e_START_OBJECT   == tokenType);
                  } break;
                  // ..........................................................
                  case  4: {     // 'object1': member 1 of 1: name
                        ASSERTV(tokenType, Obj::e_ELEMENT_NAME   == tokenType);
                        bsl::string_view memberName;
                        int rc = obj.value(&memberName);
                        ASSERTV(rc, 0 == rc);
                        ASSERTV(       memberName,
                                "A" == memberName);
                  } break;
                  case  5: {     // 'object1': member 1 of 1: value
                        ASSERTV(tokenType, Obj::e_ELEMENT_VALUE  == tokenType);
                        const Json *memberValue;
                        int rc = obj.value(&memberValue);
                        ASSERTV(rc, 0 == rc);
                        ASSERTV(       memberValue->theString(),
                                "a" == memberValue->theString());
                  } break;
                  // ..........................................................
                  case  6: {   // 'object2' end
                        ASSERTV(tokenType, Obj::e_END_OBJECT     == tokenType);
                  } break;
                  // ----------------------------------------------------------
                  case  7: {   // 'object2' start
                        ASSERTV(tokenType, Obj::e_START_OBJECT   == tokenType);
                  } break;
                  // ..........................................................
                  case  8:       // 'object2': member x of 2: name
                  case 10: {     // 'object2': member y of 2: name
                        ASSERTV(tokenType, Obj::e_ELEMENT_NAME   == tokenType);
                        bsl::string_view memberName;
                        int              rc = obj.value(&memberName);
                        ASSERTV(rc, 0 == rc);

                        Object::ConstIterator citr = object2.theObject()
                                                            .find(memberName);
                        ASSERT(object2.theObject().cend() != citr);

                        savedMemberName = memberName;
                  } break;
                  case  9:       // 'object2': member x of 2: value
                  case 11: {     // 'object2': member y of 2: value

                        ASSERTV(tokenType, Obj::e_ELEMENT_VALUE  == tokenType);
                        const Json *memberValue;
                        int         rc = obj.value(&memberValue);
                        ASSERTV(rc, 0 == rc);

                        ASSERTV(object2[savedMemberName],   *memberValue,
                                object2[savedMemberName] == *memberValue);
                  } break;
                  // ..........................................................
                  case 12: {   // 'object2' end
                        ASSERTV(tokenType, Obj::e_END_OBJECT     == tokenType);
                  } break;
                  // ----------------------------------------------------------
                  case 13: {   // 'object3' start
                        ASSERTV(tokenType, Obj::e_START_OBJECT   == tokenType);
                  } break;
                  // ..........................................................
                  case 14:       // 'object3': member x of 3: name
                  case 16:       // 'object3': member y of 3: name
                  case 18: {     // 'object3': member z of 3: name
                        ASSERTV(tokenType, Obj::e_ELEMENT_NAME   == tokenType);
                        bsl::string_view memberName;
                        int              rc = obj.value(&memberName);
                        ASSERTV(rc, 0 == rc);

                        Object::ConstIterator citr = object3.theObject()
                                                            .find(memberName);
                        ASSERT(object3.theObject().cend() != citr);

                        savedMemberName = memberName;
                  } break;
                  case 15:       // 'object3': member x of 3: value
                  case 17:       // 'object3': member y of 3: value
                  case 19: {     // 'object3': member z of 3: value

                        ASSERTV(tokenType, Obj::e_ELEMENT_VALUE  == tokenType);
                        const Json *memberValue;
                        int         rc = obj.value(&memberValue);
                        ASSERTV(rc, 0 == rc);

                        ASSERTV(object3[savedMemberName],   *memberValue,
                                object3[savedMemberName] == *memberValue);
                  } break;
                  // ..........................................................
                  case 20: {   // 'object3' end
                        ASSERTV(tokenType, Obj::e_END_OBJECT     == tokenType);
                  } break;
                  // ----------------------------------------------------------
                  case 21: { // 'json': end
                        ASSERTV(tokenType, Obj::e_END_ARRAY      == tokenType);
                  } break;
                  // ----------------------------------------------------------
                  default: {
                        ASSERT(false && "reachable");
                  } break;
                }
            }
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // ARRAY/OBJECT OF SCALARS
        //
        // Concerns:
        // 1. One can create an array (an object) that holds any of the
        //    JSON scalar types (string, number, boolean, null) as an
        //    element (as a member value).
        //
        // 2. The address obtained from `value(const Json **data)` is an
        //    address in the `Json` object specified at construction (by
        //    `reset).  I.e., the data from the `Json` object is not copied
        //    by the tokenizer.
        //
        // 3. The `advancetoNextToken` method returns a non-zero value once
        //    all tokens have been visited, and zero otherwise.
        //
        // Plan:
        // 1. Create an array (an object) holding one element (member) of
        //    each of the four scalar JSON types.  Use `advanceToNextToken`
        //    to examine each of the tokens and confirm that each has the
        //    expected value in the expected order (depth first).
        //
        // Testing:
        //   CONCERN: ARRAY/OBJECT OF SCALARS
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "ARRAY/OBJECT OF SCALARS" << endl
                          << "=======================" << endl;

        Json jsonString;  jsonString .makeString ("hello");
        Json jsonNumber;  jsonNumber .makeNumber (JsonNumber(123));
        Json jsonBoolean; jsonBoolean.makeBoolean(true);
        Json jsonNull;    jsonNull   .makeNull();

        if (veryVerbose) cout << "Array of Scalars" << endl;
        {
            Json json;  json.makeArray();

            json.theArray().pushBack(jsonString);
            json.theArray().pushBack(jsonNumber);
            json.theArray().pushBack(jsonBoolean);
            json.theArray().pushBack(jsonNull);

            ASSERTV(4 == json.theArray().size());

            Obj obj(&json);

            for (bsl::size_t i = 0; i < 4 + 2; ++i) { // 4 elements
                                                      // + start token
                                                      // + end token
                int rc = obj.advanceToNextToken();
                ASSERTV(rc, 0 == rc);

                TokenType tokenType = obj.tokenType();

                       if (0 == i) {
                        ASSERTV(tokenType, Obj::e_START_ARRAY   == tokenType);
                } else if (1 <= i && i <= 4) {
                        ASSERTV(tokenType, Obj::e_ELEMENT_VALUE == tokenType);
                } else if (5 == i) {
                        ASSERTV(tokenType, Obj::e_END_ARRAY     == tokenType);
                } else {
                        ASSERT(false && "reachable");
                }

                if (1 <= i && i <= 4) {

                    bsl::size_t index = i - 1;

                    const bdljsn::Json *jp;
                    int                 rc = obj.value(&jp);
                    ASSERTV(rc, 0 == rc);

                    ASSERTV(json[index],   *jp,
                           &json[index] ==  jp);
                }
            }

            int rc = obj.advanceToNextToken();
            ASSERTV(rc, 0 != rc); // We have visited all elements
        }

        if (veryVerbose) cout << "Object of Scalars" << endl;
        {
            Json json;  json.makeObject();

            json["jsonString" ] = jsonString;
            json["jsonNumber" ] = jsonNumber;
            json["jsonBoolean"] = jsonBoolean;
            json["jsonNull"   ] = jsonNull;

            ASSERTV(4 == json.theObject().size());

            Obj              obj(&json);
            bsl::string_view memberName;

            for (bsl::size_t i = 0; i < 4 * 2 + 2; ++i) { // 4 members,
                                                          // each a name/value,
                                                          // + start token
                                                          // + end token
                int rc = obj.advanceToNextToken();
                ASSERTV(rc, 0 == rc);

                TokenType tokenType = obj.tokenType();
                switch (i) {
                  case 0: {
                        ASSERTV(tokenType, Obj::e_START_OBJECT   == tokenType);
                  } break;
                  // ..........................................................
                  case 1: {
                        ASSERTV(tokenType, Obj::e_ELEMENT_NAME   == tokenType);
                  } break;
                  case 2: {
                        ASSERTV(tokenType, Obj::e_ELEMENT_VALUE  == tokenType);
                  } break;
                  // ..........................................................
                  case 3: {
                        ASSERTV(tokenType, Obj::e_ELEMENT_NAME   == tokenType);
                  } break;
                  case 4: {
                        ASSERTV(tokenType, Obj::e_ELEMENT_VALUE  == tokenType);
                  } break;
                  // ..........................................................
                  case 5: {
                        ASSERTV(tokenType, Obj::e_ELEMENT_NAME   == tokenType);
                  } break;
                  case 6: {
                        ASSERTV(tokenType, Obj::e_ELEMENT_VALUE  == tokenType);
                  } break;
                  // ..........................................................
                  case 7: {
                        ASSERTV(tokenType, Obj::e_ELEMENT_NAME   == tokenType);
                  } break;

                  case 8: {
                        ASSERTV(tokenType, Obj::e_ELEMENT_VALUE  == tokenType);
                  } break;
                  // ..........................................................
                  case 9: {
                        ASSERTV(tokenType, Obj::e_END_OBJECT     == tokenType);
                  } break;
                }

                if (Obj::e_ELEMENT_NAME == tokenType) {
                    int rc = obj.value(&memberName);
                    ASSERTV(rc, 0 == rc);
                }

                if (Obj::e_ELEMENT_VALUE == tokenType) {
                    const bdljsn::Json *jp;
                    int                 rc = obj.value(&jp);
                    ASSERTV(rc, 0 == rc);

                    ASSERTV(json[memberName],   *jp,
                           &json[memberName] ==  jp);
                }
            }

            int rc = obj.advanceToNextToken();
            ASSERTV(rc, 0 != rc); // We have visited all members.
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // CONSTRUCTOR AND DESTRUCTOR
        //
        // Concerns:
        // 1. Each constructor creates a tokenizer object configured to
        //    use the specified allocator.
        //
        // 2. The `allocator` accessor reports the configured allocator.
        //
        // 2. QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        // 1. Use the "footprint" idiom to create tokenizers and then
        //    confirm that the tokenizer allocates from the configured
        //    allocator and no other.
        //
        // 2. For each configuration confirm that `allocator` accessor
        //    reports the expected allocators.
        //
        // 3. Negative testing using the `BSLS_ASSERT_TEST_*` macros to confirm
        //    checking of preconditions.
        //
        // Testing:
        //   JsonTokenizer(bslma::Allocator       *basicAllocator = 0);
        //   JsonTokenizer(const bdljsn::Json     *json, *bA = 0);
        //   ~JsonTokenizer();
        //   allocator_type get_allocator() const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "CONTRUCTOR AND DESTRUCTOR" << endl
                          << "=========================" << endl;

        if (veryVerbose) cout << "Allocator configuration" << endl;
        {
            for (char cfg = 'a'; cfg <= 'h'; ++cfg) {
                const char CONFIG = cfg;

                if (veryVerbose) {
                    P(CONFIG);
                }

                bslma::TestAllocator fa("footprint", veryVeryVeryVerbose);

                bslma::TestAllocator sa("supplied",  veryVeryVeryVerbose);
                bslma::TestAllocator da("default",   veryVeryVeryVerbose);

                bslma::DefaultAllocatorGuard dag(&da);

                Json json; json.makeArray();

                Obj                  *objPtr          = 0;
                bslma::TestAllocator *objAllocatorPtr = 0;

                switch (CONFIG) {
                  case 'a': {
                    objAllocatorPtr = &da;
                    objPtr = new (fa) Obj();
                  } break;
                  case 'b': {
                    objAllocatorPtr = &da;
                    objPtr = new (fa) Obj(Obj::allocator_type());
                  } break;
                  case 'c': {
                    objAllocatorPtr = &sa;
                    objPtr = new (fa) Obj(objAllocatorPtr);
                  } break;
                  case 'd': {
                    objAllocatorPtr = &sa;
                    Obj::allocator_type alloc(objAllocatorPtr);
                    objPtr = new (fa) Obj(alloc);
                  } break;
                  case 'e': {
                    objAllocatorPtr = &da;
                    objPtr = new (fa) Obj(&json);
                  } break;
                  case 'f': {
                    objAllocatorPtr = &da;
                    objPtr = new (fa) Obj(&json, Obj::allocator_type());
                  } break;
                  case 'g': {
                    objAllocatorPtr = &sa;
                    objPtr = new (fa) Obj(&json, objAllocatorPtr);
                  } break;
                  case 'h': {
                    objAllocatorPtr = &sa;
                    Obj::allocator_type alloc(objAllocatorPtr);
                    objPtr = new (fa) Obj(&json, alloc);
                  } break;
                  default: {
                    BSLS_ASSERT_OPT(0 == "Bad allocator config.");
                  } break;
                }

                ASSERT(objAllocatorPtr);

                Obj&                   mX = *objPtr; const Obj& X = mX;
                bslma::TestAllocator&  oa = *objAllocatorPtr;
                bslma::TestAllocator& noa = (&da == &oa) ? sa : da;

                ASSERT(&oa == X.get_allocator());

                ASSERTV(CONFIG,    oa.numBlocksTotal(),
                        4      ==  oa.numBlocksTotal());

                ASSERTV(CONFIG,   noa.numBlocksTotal(),
                        0      == noa.numBlocksTotal());

                fa.deleteObject(objPtr);
            }
        }

        if (veryVerbose) cout << "Negative tests" << endl;
        {
            bsls::AssertTestHandlerGuard hg;

            Json json;

            json.makeString("");
            ASSERT_FAIL((Obj(&json)));

            json.makeNumber();
            ASSERT_FAIL((Obj(&json)));

            json.makeBoolean();
            ASSERT_FAIL((Obj(&json)));

            json.makeNull();
            ASSERT_FAIL((Obj(&json)));

            json.makeArray();
            ASSERT_PASS((Obj(&json)));

            json.makeObject();
            ASSERT_PASS((Obj(&json)));
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //   This case exercises (but does not fully test) basic functionality.
        //
        // Concerns:
        // 1. The class is sufficiently functional to enable comprehensive
        //    testing in subsequent test cases.
        //
        // Plan:
        // 1. A series of ad hoc tests.
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BREATHING TEST" << endl
                          << "==============" << endl;
        {
            Json mX; mX.makeArray();  const Json& X = mX;
            mX.theArray().pushBack(Json("Tom"));
            mX.theArray().pushBack(Json("Dick"));
            mX.theArray().pushBack(Json("Harry"));

            Json stooges; stooges.makeObject();
            stooges["Moe"]   =  "Moses Horwitz";
            stooges["Larry"] =  "Louis Feinberg";
            stooges["Shemp"] = "Samuel Horwitz";
            stooges["Curly"] = "Jerome Horwitz";

            mX.theArray().pushBack(stooges);

            Json emptyObject; emptyObject.makeObject();
            mX.theArray().pushBack(emptyObject);

            if (veryVerbose) {
                P(X);
            }

            Obj obj(&X);
            for (int i = 0; 0 == obj.advanceToNextToken(); ++i) {
                if (veryVerbose) {
                    T_; P_(i); P(obj.tokenType());
                }

                if (Obj::e_ELEMENT_NAME == obj.tokenType()) {
                    bsl::string_view dataView;
                    int rc = obj.value(&dataView);
                    P_(rc); P(dataView);
                }
                if (Obj::e_ELEMENT_VALUE == obj.tokenType()) {
                    const bdljsn::Json *json;
                    int rc = obj.value(&json);
                    P_(rc); P(*json);
                }
            }

            Json mY; mY.makeObject(); const Json& Y = mY;
            mY["1"] =  1;
            mY["2"] =  2;
            mY["3"] =  3;
            mY["4"] =  5;
            mY["5"] =  8;
            mY["6"] = 13;

            obj.reset(&Y);

            for (int i = 0; 0 == obj.advanceToNextToken(); ++i) {
                if (veryVerbose) {
                    T_; P_(i); P(obj.tokenType());
                }

                if (Obj::e_ELEMENT_NAME == obj.tokenType()) {
                    bsl::string_view dataView;
                    int rc = obj.value(&dataView);
                    P_(rc); P(dataView);
                }
                if (Obj::e_ELEMENT_VALUE == obj.tokenType()) {
                    const bdljsn::Json *json;
                    int rc = obj.value(&json);
                    P_(rc); P(*json);
                }
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
