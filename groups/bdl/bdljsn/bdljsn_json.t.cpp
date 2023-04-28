// bdljsn_json.t.cpp                                                  -*-C++-*-
#include <bdljsn_json.h>

#include <bsla_maybeunused.h>

#include <bslalg_constructorproxy.h>

#include <bslim_testutil.h>

#include <bsla_maybeunused.h>

#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>
#include <bslma_testallocatormonitor.h>

#include <bslmt_once.h>

#include <bsls_assert.h>
#include <bsls_asserttest.h>
#include <bsls_compilerfeatures.h>
#include <bsls_review.h>

#include <bsl_climits.h>
#include <bsl_cstdlib.h>
#include <bsl_cstring.h>
#include <bsl_iostream.h>
#include <bsl_list.h>
#include <bsl_ostream.h>
#include <bsl_sstream.h>
#include <bsl_string.h>
#include <bsl_utility.h>
#include <bsl_vector.h>

using namespace BloombergLP;
using namespace BloombergLP::bdljsn;
using bsl::cout;
using bsl::cerr;
using bsl::endl;
using bsl::flush;

// ============================================================================
//                                 TEST PLAN
// ----------------------------------------------------------------------------
// Note that this component is comprised of three different classes, no one of
// which can be described without describing the others.  Classes bdljsn::Json,
// bdljsn::JsonObject, and bdljsn::JsonArray are essentially specializations of
// bdlb::Variant, bsl::unordered_map, and bsl::vector, respectively.
//
//                                 Overview
//                                 --------
//
// The first class to be tested is JsonArray.  The testing methodology will be
// that of a value-semantic type.
//
// Primary Manipulators:
//: o 'JsonArray::pushBack'
//
// Basic Accessors:
//: o 'JsonArray::operator[](size_t i) const'
//
// The input iterator constructor of 'JsonArray' is capable of creating an
// object in any valid state, obviating the primitive generator function, 'gg',
// normally used for this purpose.  The standard 10-case approach to testing
// value-semantic types will be followed, except that we will test the value
// constructor in case 3.
//
// Certain standard value-semantic-tpye test cases are omitted:
//: o [10] -- 'BSLX' streaming is not implemented for this class.
//
// Global Concerns:
//: o The test driver is robust w.r.t. reuse in other, similar components.
//: o ACCESSOR methods are declared 'const'.
//: o CREATOR & MANIPULATOR pointer/reference parameters are declared 'const'.
//: o No memory is ever allocated from the global allocator.
//: o Any allocated memory is always from the object allocator.
//: o An object's value is independent of the allocator used to supply memory.
//: o injected exceptions are safely propagated during memory allocation.
//: o Precondition violations are detected in appropriate build modes.
//
// Global Assumptions:
//: o All explicit memory allocations are presumed to use the global, default,
//:   or object allocator.
//: o ACCESSOR methods are 'const' thread-safe.
//: o Individual attribute types are presumed to be *alias-safe*; hence, only
//:   certain methods require the testing of this property:
//:   o copy-assignment
//:   o swap
// ----------------------------------------------------------------------------
// JSONARRAY CLASS METHODS
//
// CREATORS
// [ 2] JsonArray();
// [ 2] JsonArray(const bslma::allocator *a);
// [ 3] JsonArray(INPUT_ITERATOR first, INPUT_ITERATOR last, *a);
// [13] JsonArray(initializer_list<Json> l, *a);
// [ 7] JsonArray(const JsonArray& o, *a);
// [ 8] JsonArray(MovableRef<JsonArray> o);
// [ 8] JsonArray(MovableRef<JsonArray> o, *a);
// [ 2] ~JsonArray();
//
// MANIPULATORS
// [10] JsonArray::operator=(const JsonArray& rhs);
// [11] JsonArray::operator=(MovableRef<JsonArray> rhs);
// [13] JsonArray::operator=(initializer_list<Json> l);
// [ 2] JsonArray::pushBack(const Json& j);
// [13] JsonArray::pushBack(MovableRef<Json> j);
// [13] Json& JsonArray::operator[](size_t i);
// [13] JsonArray::assign(initializer_list<Json> l);
// [13] JsonArray::assign(INPUT_ITERATOR first, INPUT_ITERATOR last);
// [13] Iterator JsonArray::begin();
// [13] Iterator JsonArray::end();
// [13] Json& JsonArray::front();
// [13] Json& JsonArray::back();
// [13] JsonArray::resize(size_t c);
// [13] JsonArray::resize(size_t c, const Json& j);
// [13] Iter JsonArray::insert(size_t i, const Json& j);
// [13] Iter JsonArray::insert(size_t i, MovableRef<Json> j);
// [13] Iter JsonArray::insert(size_t i, INPUT_ITER f, INPUT_ITER l);
// [13] Iter JsonArray::insert(ConstIter p, const Json& j);
// [13] Iter JsonArray::insert(ConstIter p, MovableRef<Json> j);
// [13] Iter JsonArray::insert(ConstIter p, INPUT_ITER f, INPUT_ITER l);
// [13] Iter JsonArray::erase(ConstIter p);
// [13] Iter JsonArray::erase(ConstIter f, ConstIter l);
// [13] Iter JsonArray::erase(size_t i);
// [13] JsonArray::popBack();
// [13] JsonArray::clear();
//
// [ 9] void JsonArray::swap(JsonArray& other);
//
// ACCESSORS
// [ 4] const Json& JsonArray::front() const;
// [ 4] const Json& JsonArray::begin() const;
// [ 4] const Json& JsonArray::cbegin() const;
// [ 4] const Json& JsonArray::back() const;
// [ 4] const Json& JsonArray::end() const;
// [ 4] const Json& JsonArray::cend() const;
// [ 4] bool JsonArray::empty() const;
// [ 4] size_t JsonArray::size() const;
// [ 4] const Json& JsonArray::operator[](size_t i) const;
// [14] size_t JsonArray::maxSize() const;
//
// [ 4] bslma::Allocator *JsonArray::allocator() const;
// [ 5] ostream& JsonArray::print(os& s, int l = 0, int sPL = 4) const;
//
// FREE OPERATORS
// [ 5] operator<<(ostream&, const JsonArray&);
// [ 6] bool operator==(const bdljsn::JsonArray& lhs, rhs);
// [ 6] bool operator!=(const bdljsn::JsonArray& lhs, rhs);
//
// FREE FUNCTIONS
// [ 9] void swap(JsonArray& a, b);
//
// JSONOBJECT CLASS METHODS
//
// CREATORS
// [15] JsonObject();
// [15] JsonObject(bslma::Allocator *basicAllocator);
// [20] JsonObject(const JsonObject& original, *a);
// [21] JsonObject(MovableRef<JsonObject> o);
// [21] JsonObject(MovableRef<JsonObject> o, *a);
// [16] JsonObject(INPUT_ITER first, INPUT_ITER last, *a);
// [26] JsonObject(initializer_list<Member> members, *a);
// [15] ~JsonObject();
//
// MANIPULATORS
// [23] JsonObject::operator=(const JsonObject& rhs);
// [24] JsonObject::operator=(MovableRef<JsonObject> rhs);
// [26] JsonObject::operator=(initializer_list<Member> members);
// [15] Json& JsonObject::operator[](const string_view& key);
// [26] Iterator JsonObject::begin();
// [26] Iterator JsonObject::end();
// [26] void JsonObject::clear();
// [26] pair<Iterator, bool> JsonObject::insert(const Member& m);
// [26] pair<Iterator, bool> JsonObject::insert(MovableRef<Member> m);
// [26] void JsonObject::insert(INPUT_ITER first, INPUT_ITER last);
// [26] void JsonObject::insert(initializer_list<Member> members);
// [26] pair<Iterator, bool> JsonObject::insert(string_view key, V&& v);
// [26] bsl::size_t JsonObject::erase(const bsl::string_view& key);
// [26] Iterator JsonObject::erase(Iterator position);
// [26] Iterator JsonObject::erase(ConstIterator position);
// [26] Iterator JsonObject::find(const bsl::string_view& key);
//
// [22] void JsonObject::swap(JsonObject& other);
//
// ACCESSORS
// [17] const Json& JsonObject::operator[](bsl::string_view& key) const;
// [27] ConstIterator JsonObject::begin() const;
// [27] ConstIterator JsonObject::cbegin() const;
// [27] ConstIterator JsonObject::end() const;
// [27] ConstIterator JsonObject::cend() const;
// [27] ConstIterator JsonObject::find(string_view& key) const;
// [27] bool JsonObject::contains(const string_view& key) const;
// [27] bool JsonObject::empty() const;
// [27] size_t JsonObject::size() const;
//
// [27] bslma::Allocator *JsonObject::allocator() const;
// [18] ostream& JsonObject::print(os& s, int l = 0, int sPL = 4) const;
//
// FREE OPERATORS
// [18] operator<<(ostream&, const JsonObject&);
// [19] bool operator==(const bdljsn::JsonObject& lhs, rhs);
// [19] bool operator!=(const bdljsn::JsonObject& lhs, rhs);
//
// FREE FUNCTIONS
// [22] void swap(JsonObject& a, b);
//
// JSON CLASS METHODS
//
// CONSTRUCTORS
// [28] Json();
// [28] Json(bslma::Allocator *basicAllocator);
// [33] Json(const Json &original, *a);
// [34] Json(bslmf::MovableRef<Json> original);
// [34] Json(bslmf::MovableRef<Json> original, *a);
// [29] Json(const JsonArray &array, *a);
// [29] Json(bslmf::MovableRef<JsonArray> array, *a);
// [29] Json(bool boolean, *a);
// [29] Json(const JsonNull& null, *a);
// [29] Json(float number, *a);
// [29] Json(double number, *a);
// [29] Json(bdldfp::Decimal64 number, *a);
// [29] Json(int number, *a);
// [29] Json(unsigned int number, *a);
// [29] Json(bsls::Types::Int64 number, *a);
// [29] Json(bsls::Types::Uint64 number, *a);
// [29] Json(const JsonNumber& number, *a);
// [29] Json(bslmf::MovableRef<JsonNumber> number, *a);
// [29] Json(const JsonObject& object, *a);
// [29] Json(bslmf::MovableRef<JsonObject> object, *a);
// [29] Json(const char *string, *a);
// [29] Json(const bsl::string_view& string, *a);
// [29] Json(STRING&& string, *a);
// [28] ~Json();
//
// MANIPULATORS
// [36] Json& Json::operator=(const Json& rhs);
// [37] Json& Json::operator=(bslmf::MovableRef<Json> rhs);
// [39] Json& Json::operator=(float rhs);
// [39] Json& Json::operator=(double rhs);
// [39] Json& Json::operator=(bdldfp::Decimal64 rhs);
// [39] Json& Json::operator=(int rhs);
// [39] Json& Json::operator=(unsigned int rhs);
// [39] Json& Json::operator=(bsls::Types::Int64 rhs);
// [39] Json& Json::operator=(bsls::Types::Uint64 rhs);
// [39] Json& Json::operator=(const JsonNumber& rhs);
// [39] Json& Json::operator=(bslmf::MovableRef<JsonNumber> rhs);
// [39] Json& Json::operator=(const char *rhs);
// [39] Json& Json::operator=(const bsl::string_view& rhs);
// [39] Json& Json::operator=(STRING&& rhs);
// [39] Json& Json::operator=(bool rhs);
// [39] Json& Json::operator=(const JsonObject& rhs);
// [39] Json& Json::operator=(bslmf::MovableRef<JsonObject> rhs);
// [39] Json& Json::operator=(const JsonArray& rhs);
// [39] Json& Json::operator=(bslmf::MovableRef<JsonArray> rhs);
// [39] Json& Json::operator=(const JsonNull& rhs);
// [39] Json& Json::operator=(bslmf::MovableRef<JsonNull> rhs);
// [28] JsonArray& Json::makeArray();
// [39] JsonArray& Json::makeArray(const JsonArray& array);
// [39] JsonArray& Json::makeArray(MovableRef<JsonArray> array);
// [28] bool& Json::makeBoolean();
// [39] bool& Json::makeBoolean(bool boolean);
// [28] void Json::makeNull();
// [28] JsonNumber& Json::makeNumber();
// [39] JsonNumber& Json::makeNumber(const JsonNumber& number);
// [39] JsonNumber& Json::makeNumber(MovableRef<JsonNumber> number);
// [28] JsonObject& Json::makeObject();
// [39] JsonObject& Json::makeObject(const JsonObject& object);
// [39] JsonObject& Json::makeObject(MovableRef<JsonObject> object);
// [28] void Json::makeString(const char *string);
// [39] void Json::makeString(const bsl::string_view& string);
// [39] void Json::makeString(STRING&& string);
// [39] JsonArray& Json::theArray();
// [39] bool& Json::theBoolean();
// [39] JsonNull& Json::theNull();
// [39] JsonNumber& Json::theNumber();
// [39] JsonObject& Json::theObject();
// [39] Json& Json::operator[](const string_view& key);
// [39] Json& Json::operator[](size_t index);
// [39] Json::operator BloombergLP::bdljsn::JsonArray &();
// [39] Json::operator bool &();
// [39] Json::operator BloombergLP::bdljsn::JsonNull &();
// [39] Json::operator BloombergLP::bdljsn::JsonNumber &();
// [39] Json::operator BloombergLP::bdljsn::JsonObject &();
// [39] Json::operator bsl::string &();
//
// [35] void Json::swap(Json& other);
//
// ACCESSORS
// [40] bool Json::isArray() const;
// [40] bool Json::isBoolean() const;
// [40] bool Json::isNull() const;
// [40] bool Json::isNumber() const;
// [40] bool Json::isObject() const;
// [40] bool Json::isString() const;
// [30] const JsonArray& Json::theArray() const;
// [30] const bool& Json::theBoolean() const;
// [30] const JsonNull& Json::theNull() const;
// [30] const JsonNumber& Json::theNumber() const;
// [30] const JsonObject& Json::theObject() const;
// [30] const bsl::string& Json::theString() const;
// [30] JsonType::Enum Json::type() const;
// [40] int Json::asInt(int *r) const;
// [40] int Json::asInt64(Int64 *r) const;
// [40] int Json::asUint(unsigned int *r) const;
// [40] int Json::asUint64(Uint64 *r) const;
// [40] float Json::asFloat() const;
// [40] double Json::asDouble() const;
// [40] Decimal64 Json::asDecimal64() const;
// [40] int Json::asDecimal64Exact(Decimal64 *r) const;
// [40] const Json& Json::operator[](const string_view& key) const;
// [40] const Json& Json::operator[](size_t index) const;
// [40] size_t Json::size() const;
// [40] Json::operator const BloombergLP::bdljsn::JsonArray &() const;
// [40] Json::operator const bool &() const;
// [40] Json::operator const BloombergLP::bdljsn::JsonNull &() const;
// [40] Json::operator const BloombergLP::bdljsn::JsonNumber &() const;
// [40] Json::operator const BloombergLP::bdljsn::JsonObject &() const;
// [40] Json::operator const bsl::basic_string &() const;
//
// [40] bslma::Allocator* Json::allocator() const;
// [31] ostream& Json::print(ostream& stream, int l, int spl) const;
//
// FREE OPERATORS
// [31] ostream& operator<<(ostream&, const Json&);
// [32] bool operator==(const Json& lhs, rhs);
// [32] bool operator!=(const Json& lhs, rhs);
//
// FREE FUNCTIONS
// [35] void swap(Json& lhs, rhs);
// ----------------------------------------------------------------------------

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
//                     NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT_SAFE_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(EXPR)
#define ASSERT_SAFE_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(EXPR)
#define ASSERT_PASS(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS(EXPR)
#define ASSERT_FAIL(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL(EXPR)

// ============================================================================
//                    EXCEPTION TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define EXCEPTION_COUNT bslmaExceptionCounter

// ============================================================================
//                             GLOBAL TEST DATA
// ----------------------------------------------------------------------------

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//                             The "GG" Grammar
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// The "GG" grammar is the 'VALUE' non-terminal of the following pseudo-EBNF:
// TBD: Document this more thoroughly.
//..
//  VALUE  ::= s         <create a string with value '"string"'>
//           | 1         <create a number with value '1'>
//           | t         <create a boolean with value 'true'>
//           | f         <create a boolean with value 'false'>
//           | n         <create a null value>
//           | OBJECT    <create an object value>
//           | ARRAY     <create an array value>
//
//  OBJECT ::= { (MEMBER VALUE)* }
//
//  MEMBER ::= a         <create a member with key '"memberA"'>
//           | b         <create a member with key '"memberB"'>
//
//  ARRAY  ::= [ VALUE* ]
//..

// BDE_VERIFY pragma: -AR01 returning allocator aware by value
// BDE_VERIFY pragma: -FD01 generator functions don't have contracts

Json       g (const char *spec);
JsonObject go(const char *spec);
JsonArray  ga(const char *spec);
Json&      gg(Json *object, const char *spec);
int       ggg(Json *object, const char *spec, int verbose = 1);

Json g(const char *spec)
{
    Json object;
    gg(&object, spec);
    return object;
}

JsonObject go(const char *spec)
{
    Json result = g(spec);
    ASSERT(result.isObject());
    return result.theObject();
}

JsonArray ga(const char *spec)
{
    Json result = g(spec);
    ASSERT(result.isArray());
    return result.theArray();
}

Json& gg(Json *object, const char *spec)
{
    ASSERTV(ggg(object, spec) == 0);
    return *object;
}

enum Context { e_TOP, e_ARRAY, e_OBJECT_EXPECT_MEMBER, e_OBJECT_EXPECT_VALUE };

struct State {
    // PUBLIC DATA
    Context      d_context;
    Json        *d_json_p;
    bsl::string  d_member;  // null unless 'd_context == e_OBJECT_VALUE'.

    // CREATORS
    State(Context                  context,
          Json                    *json,
          const bsl::string_view&  member = bsl::string_view())
    : d_context(context)
    , d_json_p(json)
    , d_member(member)
    {
    }
};

int ggg(Json *object, const char *spec, int verbose)
{
    (void) verbose;
    const bsl::size_t specLength = bsl::strlen(spec);

    const State initialState(e_TOP, object, "");

    bsl::vector<State> stack;
    stack.push_back(initialState);

    for (const char *cIt = spec; cIt != spec + specLength; ++cIt) {
        const char c = *cIt;

        switch (stack.back().d_context) {
          case e_TOP: {
            switch (c) {
              case 's': {
                stack.back().d_json_p->makeString("string");
              } break;
              case '1': {
                stack.back().d_json_p->makeNumber(JsonNumber(1));
              } break;
              case 't': {
                stack.back().d_json_p->makeBoolean(true);
              } break;
              case 'f': {
                stack.back().d_json_p->makeBoolean(false);
              } break;
              case 'n': {
                stack.back().d_json_p->makeNull();
              } break;
              case '[': {
                const State state = stack.back();
                stack.pop_back();
                state.d_json_p->makeArray();
                stack.emplace_back(e_ARRAY, state.d_json_p);
              } break;
              case '{': {
                const State state = stack.back();
                stack.pop_back();
                state.d_json_p->makeObject();
                stack.emplace_back(e_OBJECT_EXPECT_MEMBER, state.d_json_p);
              } break;
              default: {
                ASSERT(!"illegal");
              } break;
            }
          } break;
          case e_ARRAY: {
            switch (c) {
              case 's': {
                stack.back().d_json_p->theArray().pushBack(Json("string"));
              } break;
              case '1': {
                stack.back().d_json_p->theArray().pushBack(Json(1));
              } break;
              case 't': {
                stack.back().d_json_p->theArray().pushBack(Json(true));
              } break;
              case 'f': {
                stack.back().d_json_p->theArray().pushBack(Json(false));
              } break;
              case 'n': {
                stack.back().d_json_p->theArray().pushBack(Json());
              } break;
              case '[': {
                stack.back().d_json_p->theArray().pushBack(Json(JsonArray()));
                stack.emplace_back(e_ARRAY,
                                   &stack.back().d_json_p->theArray().back());
              } break;
              case ']': {
                stack.pop_back();
              } break;
              case '{': {
                stack.back().d_json_p->theArray().pushBack(Json(JsonObject()));
                stack.emplace_back(e_OBJECT_EXPECT_MEMBER,
                                   &stack.back().d_json_p->theArray().back());
              } break;
              default: {
                ASSERT(!"illegal");
              } break;
            }
          } break;
          case e_OBJECT_EXPECT_MEMBER: {
            switch (c) {
              case 'a': {
                stack.back().d_member  = "memberA";
                stack.back().d_context = e_OBJECT_EXPECT_VALUE;
              } break;
              case 'b': {
                stack.back().d_member  = "memberB";
                stack.back().d_context = e_OBJECT_EXPECT_VALUE;
              } break;
              case '}': {
                stack.pop_back();
              } break;
              default: {
                ASSERT(!"illegal");
              } break;
            }
          } break;
          case e_OBJECT_EXPECT_VALUE: {
            switch (c) {
              case 's': {
                stack.back().d_json_p->theObject().insert(
                                                         stack.back().d_member,
                                                         Json("string"));
                stack.back().d_context = e_OBJECT_EXPECT_MEMBER;
              } break;
              case '1': {
                stack.back().d_json_p->theObject().insert(
                                                         stack.back().d_member,
                                                         Json(1));
                stack.back().d_context = e_OBJECT_EXPECT_MEMBER;
              } break;
              case 't': {
                stack.back().d_json_p->theObject().insert(
                                                         stack.back().d_member,
                                                         Json(true));
                stack.back().d_context = e_OBJECT_EXPECT_MEMBER;
              } break;
              case 'f': {
                stack.back().d_json_p->theObject().insert(
                                                         stack.back().d_member,
                                                         Json(false));
                stack.back().d_context = e_OBJECT_EXPECT_MEMBER;
              } break;
              case 'n': {
                stack.back().d_json_p->theObject().insert(
                                                         stack.back().d_member,
                                                         Json());
                stack.back().d_context = e_OBJECT_EXPECT_MEMBER;
              } break;
              case '[': {
                stack.back().d_json_p->theObject().insert(
                                                         stack.back().d_member,
                                                         Json(JsonArray()));
                stack.back().d_context = e_OBJECT_EXPECT_MEMBER;
                stack.emplace_back(
                           e_ARRAY,
                           &stack.back()
                                .d_json_p->theObject()[stack.back().d_member]);
              } break;
              case '{': {
                stack.back().d_json_p->theObject().insert(
                                                         stack.back().d_member,
                                                         Json(JsonObject()));
                stack.back().d_context = e_OBJECT_EXPECT_MEMBER;
                stack.emplace_back(
                           e_OBJECT_EXPECT_MEMBER,
                           &stack.back()
                                .d_json_p->theObject()[stack.back().d_member]);
              } break;
              default: {
                ASSERT(!"illegal");
              } break;
            }
          } break;
          default: {
            ASSERT(!"illegal");
          } break;
        }
    }

    return 0;
}

                          // ========================
                          // struct GenerateJsonArray
                          // ========================

struct GenerateJsonArray {
    // CREATORS
    GenerateJsonArray()
    {
    }

    // ACCESSORS
    Json operator()() const { return Json(JsonArray()); }

    Json operator()(const Json& element) const
    {
        JsonArray result;
        result.pushBack(element);
        return Json(result);
    }

    Json operator()(const Json& element1, const Json& element2) const
    {
        JsonArray result;
        result.pushBack(element1);
        result.pushBack(element2);
        return Json(result);
    }

    Json operator()(const Json& element1,
                    const Json& element2,
                    const Json& element3) const
    {
        JsonArray result;
        result.pushBack(element1);
        result.pushBack(element2);
        result.pushBack(element3);
        return Json(result);
    }
};

                         // =========================
                         // struct GenerateJsonObject
                         // =========================

struct GenerateJsonObject {
    // CREATORS
    GenerateJsonObject()
    {
    }

    // ACCESSORS
    Json operator()() const { return Json(JsonObject()); }

    Json operator()(const bsl::string_view& key, const Json& value) const
    {
        JsonObject result;
        result.insert(key, value);
        return Json(result);
    }

    Json operator()(const bsl::string_view& key1,
                    const Json&             value1,
                    const bsl::string_view& key2,
                    const Json&             value2) const
    {
        JsonObject result;
        result.insert(key1, value1);
        result.insert(key2, value2);
        return Json(result);
    }

    Json operator()(const bsl::string_view& key1,
                    const Json&             value1,
                    const bsl::string_view& key2,
                    const Json&             value2,
                    const bsl::string_view& key3,
                    const Json&             value3) const
    {
        JsonObject result;
        result.insert(key1, value1);
        result.insert(key2, value2);
        result.insert(key3, value3);
        return Json(result);
    }
};

// Define 'bsl::string' value long enough to ensure dynamic memory allocation.

#ifdef BSLS_PLATFORM_CPU_32_BIT
#define SUFFICIENTLY_LONG_STRING "123456789012345678901234567890123"
#else  // 64_BIT
#define SUFFICIENTLY_LONG_STRING "12345678901234567890123456789012" \
                                     "123456789012345678901234567890123"
#endif
BSLMF_ASSERT(sizeof SUFFICIENTLY_LONG_STRING > sizeof(bsl::string));

const char *const LONG_STRING    = "a_"    SUFFICIENTLY_LONG_STRING;
const char *const LONGER_STRING  = "ab_"   SUFFICIENTLY_LONG_STRING;
const char *const LONGEST_STRING = "abc_"  SUFFICIENTLY_LONG_STRING;
const char *const INT_LONG_STRING = "1111" SUFFICIENTLY_LONG_STRING;

// Define DEFAULT DATA used by test cases 3, 7, 8, and 9.

struct DefaultDataRow {
    int         d_line;
    char        d_mem;
    bool        d_bool;
    const char *d_number_p;
    const char *d_string_p;
};

static const DefaultDataRow DEFAULT_DATA[] =
{
    //LINE  MEM   BOOL   NUMBER  STRING
    //----  ---   ----   ------  ------

    // default (must be  first)
    { L_,   'N', false,             "0", ""                               },

    // bool
    { L_,   'N', true,              "0", ""                               },

    // number
    { L_,   'N', false,            "-1", ""                               },
    { L_,   'N', false,             "1", ""                               },
    { L_,   'Y', false, INT_LONG_STRING, ""                               },

    // string
    { L_,   'N', false,             "0", "a"                              },
    { L_,   'N', false,             "0", "AB"                             },
    { L_,   'N', false,             "0", "1234567890"                     },
    { L_,   'Y', false,             "0", LONG_STRING                      },
    { L_,   'Y', false,             "0", LONGER_STRING                    },
    { L_,   'Y', false,             "0", LONGEST_STRING                   }
};
enum { DEFAULT_NUM_DATA = sizeof DEFAULT_DATA / sizeof *DEFAULT_DATA };

char KEY0[] = "zero";
char KEY1[] = "one";
char KEY2[] = "two";
char KEY3[] = "three";

const char* KEYS[] = {KEY0, KEY1, KEY2, KEY3};

template <class TYPE>
class TemplatedJsonObjectInsertionTest {
  public:
      // Tests insertion of templated insert method.  The behavior is undefined
      // unless either TYPE is not allocator aware, or the specified 'value'
      // has the specified 'oa' installed.
    void operator()(const TYPE& value, bslma::TestAllocator *oa);
};

template <class TYPE>
void TemplatedJsonObjectInsertionTest<TYPE>::operator()(
                                                   const TYPE&           value,
                                                   bslma::TestAllocator *oa)
{
    const JsonObject::Member INIT(KEY0, Json(value, oa), oa);
    const JsonObject         EXPECTED_VALUE(&INIT, &INIT + 1, oa);

    {
        JsonObject mX(oa);
        mX.insert(KEY0, value);
        ASSERTV(EXPECTED_VALUE, mX, EXPECTED_VALUE == mX);
    }
    {
        const JsonObject::Member NON_ALLOCATING(KEY1, Json());
        bslma::TestAllocator     ca("compare", false);
        JsonObject               mY(&ca);
        mY.insert(NON_ALLOCATING);

        bslalg::ConstructorProxy<TYPE> proxy(value, oa);
        TYPE&                          moveFrom     = proxy.object();
        bsls::Types::Int64             BlocksBefore = oa->numBlocksTotal();
        bsls::Types::Int64             BytesBefore  = oa->numBytesTotal();

        JsonObject mX(oa);
        mX.insert(KEY0, bslmf::MovableRefUtil::move(moveFrom));
        ASSERTV(value, EXPECTED_VALUE, mX, EXPECTED_VALUE == mX);
        ASSERTV(value,
                moveFrom,
                ca.numBlocksTotal(),
                BlocksBefore,
                oa->numBlocksTotal(),
                ca.numBlocksTotal() == (oa->numBlocksTotal() - BlocksBefore));
        ASSERTV(value,
                moveFrom,
                ca.numBytesTotal(),
                BytesBefore,
                oa->numBytesTotal(),
                ca.numBytesTotal() == (oa->numBytesTotal() - BytesBefore));
    }
}

class ReorderExpectedJsonObjectString {
    typedef bsl::unordered_map<bsl::string, bsl::string> Container;
    bsl::vector<bsl::string> d_orderOfKeys;

    static bsl::vector<bsl::string> extractOrder(const JsonObject& X)
    {
        bsl::vector<bsl::string> keys;
        for (JsonObject::ConstIterator iter = X.cbegin();
             iter != X.cend();
             ++iter) {
            keys.push_back(iter->first);
        }
        return keys;
    }

  public:
    explicit ReorderExpectedJsonObjectString(const JsonObject& object)
    : d_orderOfKeys(extractOrder(object))
    {
    }

    bsl::string operator()(const bsl::string& input) const
    {
        bsl::string            format[5];
        Container              data;
        bsl::string::size_type pos  = 0;
        bsl::string::size_type prev = 0;

        for (int i = 0; i < 4; ++i) {
            pos = input.find('\"', prev = pos);
            if (bsl::string::npos == pos) {
                ASSERTV(i, input, "Quote character number i not found");
                return "";                                            // RETURN
            }
            format[i] = input.substr(prev, pos - prev);

            const char delim = (i == 3 ? ']' : ',');
            pos = input.find(delim, prev = pos);
            if (bsl::string::npos == pos) {
                ASSERTV(delim, i, input, "delimiter not found");
                return "";                                            // RETURN
            }
            if (delim == ']') {
                ++pos;
                if (pos == input.size()) {
                    ASSERTV(i, input, "\']\' character was last in string");
                    return "";                                        // RETURN
                }
            }
            const bsl::string::size_type secondQuote = input.find('\"',
                                                                  prev + 1);
            if (secondQuote == bsl::string::npos) {
                ASSERTV(i, input, "second quote not found");
                return "";                                            // RETURN
            }
            data[input.substr(prev + 1, secondQuote - (prev + 1))] =
                                                input.substr(prev, pos - prev);
        }
        format[4] = input.substr(pos, input.size() - pos);

        bsl::stringstream ss;
        for (int index = 0; index < 4; ++index) {
            ss << format[index] << data[d_orderOfKeys[index]];
        }
        ss << format[4];
        return ss.str();
    }
};

// ============================================================================
//                          EXTENDED BREATHING TEST
// ----------------------------------------------------------------------------

void extendedBreathingTest(BSLA_MAYBE_UNUSED bool verbose,
                           BSLA_MAYBE_UNUSED bool veryVerbose)
{
    // First, briefly exercise the "gg" generator.

    const Json one(1);
    const Json s("string");
    const Json t(true);
    const Json f(false);
    const Json n;

    const bsl::string_view a = "memberA";
    const bsl::string_view b = "memberB";

    const GenerateJsonArray  A;
    const GenerateJsonObject O;

    const struct {
        int         d_line;
        const char *d_spec_p;
        Json        d_expectedJson;
    } DATA[] = {
        //   LINE
        //  .----
        // /    SPEC    EXPECTED 'Json'
        //-- ---------- ---------------
        { L_, "s"      , s            },
        { L_, "1"      , one          },
        { L_, "t"      , t            },
        { L_, "f"      , f            },
        { L_, "n"      , n            },

        { L_, "[]"     , A()          },
        { L_, "[s]"    , A(s)         },
        { L_, "[1]"    , A(one)       },
        { L_, "[t]"    , A(t)         },
        { L_, "[f]"    , A(f)         },
        { L_, "[n]"    , A(n)         },

        { L_, "[ns]"   , A(n,s)       },
        { L_, "[n1]"   , A(n,one)     },
        { L_, "[nt]"   , A(n,t)       },
        { L_, "[nf]"   , A(n,f)       },
        { L_, "[nn]"   , A(n,n)       },

        { L_, "[[]s]"  , A(A(),s)     },
        { L_, "[[]1]"  , A(A(),one)   },
        { L_, "[[]t]"  , A(A(),t)     },
        { L_, "[[]f]"  , A(A(),f)     },
        { L_, "[[]n]"  , A(A(),n)     },

        { L_, "[{}s]"  , A(O(),s)     },
        { L_, "[{}1]"  , A(O(),one)   },
        { L_, "[{}t]"  , A(O(),t)     },
        { L_, "[{}f]"  , A(O(),f)     },
        { L_, "[{}n]"  , A(O(),n)     },

        { L_, "[s[]]"  , A(s,A())     },
        { L_, "[1[]]"  , A(one,A())   },
        { L_, "[t[]]"  , A(t,A())     },
        { L_, "[f[]]"  , A(f,A())     },
        { L_, "[n[]]"  , A(n,A())     },

        { L_, "[s{}]"  , A(s,O())     },
        { L_, "[1{}]"  , A(one,O())   },
        { L_, "[t{}]"  , A(t,O())     },
        { L_, "[f{}]"  , A(f,O())     },
        { L_, "[n{}]"  , A(n,O())     },

        { L_, "{}"     , O()          },
        { L_, "{as}"   , O(a,s)       },
        { L_, "{a1}"   , O(a,one)     },
        { L_, "{at}"   , O(a,t)       },
        { L_, "{af}"   , O(a,f)       },
        { L_, "{an}"   , O(a,n)       },

        { L_, "{a[]}"  , O(a,A())     },

        { L_, "{asbs}" , O(a,s,b,s)   },
        { L_, "{a{}bs}", O(a,O(),b,s) },
    };

    const int NUM_DATA = sizeof(DATA) / sizeof(DATA[0]);

    for (int i = 0; i != NUM_DATA; ++i) {
        const int   LINE = DATA[i].d_line;
        const char *SPEC = DATA[i].d_spec_p;
        const Json  EXPECTED_JSON = DATA[i].d_expectedJson;

        Json json;
        int  rc = ggg(&json, SPEC);
        ASSERTV(LINE, 0 == rc);
        ASSERTV(LINE, json == EXPECTED_JSON);
    }

    if (veryVerbose) {
        bsl::cout << "Testing 'JsonArray' Operations" << bsl::endl
                  << "------------------------------" << bsl::endl;
    }

    // Default Constructor
    {
        bslma::TestAllocator ta;
        ASSERT(JsonArray() == ga("[]"));
        ASSERT(JsonArray(&ta) == ga("[]"));
    }

    // Copy Constructor
    {
        bslma::TestAllocator ta;
        JsonArray            lvalue1 = ga("[1s]");
        JsonArray            lvalue2 = ga("[1s]");

        ASSERT(JsonArray(lvalue1) == ga("[1s]"));
        ASSERT(JsonArray(lvalue1, &ta) == ga("[1s]"));
    }

    // Move Constructor
    {
        bslma::TestAllocator ta;
        JsonArray            lvalue1 = ga("[1s]");
        JsonArray            lvalue2 = ga("[1s]");

        ASSERT(JsonArray(bslmf::MovableRefUtil::move(lvalue1)) ==
               ga("[1s]"));
        ASSERT(JsonArray(bslmf::MovableRefUtil::move(lvalue2), &ta) ==
               ga("[1s]"));
    }

    // Range Constructor
    {
        bslma::TestAllocator ta;
        const Json           values[2] = { g("1"), g("s") };
        ASSERT(JsonArray(values, values + 2) == ga("[1s]"));
        ASSERT(JsonArray(values, values + 2, &ta) == ga("[1s]"));
    }

#if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
    // Initializer-List Constructor
    {
        bslma::TestAllocator ta;
        ASSERT(JsonArray({ g("1"), g("s") }) == ga("[1s]"));
        ASSERT(JsonArray({ g("1"), g("s") }, &ta) == ga("[1s]"));
    }
#endif

    // Copy-Assignment Operator
    {
        JsonArray  mX;
        JsonArray& mXRef = (mX = ga("[1s]"));
        ASSERT(mX == ga("[1s]"));
        ASSERT(&mXRef == &mX);
    }

    // Move-Assignment Operator
    {
        JsonArray  mX;
        JsonArray  mY    = ga("[1s]");
        JsonArray& mXRef = (mX = bslmf::MovableRefUtil::move(mY));
        ASSERT(mX == ga("[1s"));
        ASSERT(&mXRef == &mX);
    }

#if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
    // Initializer-List Assignment Operator
    {
        JsonArray  mX;
        JsonArray& mXRef = (mX = { g("1"), g("s") });
        ASSERT(mX == ga("[1s]"));
        ASSERT(&mXRef == &mX);
    }

    // Intializer-List Assignment
    {
        JsonArray mX;
        mX.assign({g("1"), g("s")});
        ASSERT(mX == ga("[1s]"));
    }
#endif

    // Range Assignment
    {
        Json range[3] = {g("n"), g("n"), g("n")};

        JsonArray mX;
        mX.assign(range, range + 3);
        ASSERT(mX == ga("[nnn]"));
    }

    // Begin and End
    {
        JsonArray           mX    = ga("[1ns]");
        JsonArray::Iterator begin = mX.begin();
        JsonArray::Iterator end   = mX.end();

        ASSERT(begin != end);
        ASSERT(end - begin == 3);

        ASSERT(begin->isNumber());

        ++begin;
        ASSERT(begin->isNull());
        ASSERT(begin != end);

        ++begin;
        ASSERT(begin->isString());
        ASSERT(begin != end);

        ++begin;
        ASSERT(begin == end);
    }

    // Subscript Operator
    {
        JsonArray mX = ga("[1ns]");

        ASSERT(mX[0].isNumber());
        ASSERT(mX[1].isNull());
        ASSERT(mX[2].isString());
    }

    // Front and Back
    {
        JsonArray mX = ga("[1ns]");

        ASSERT(mX.front().isNumber());
        ASSERT(mX.back().isString());
    }

    // Resize
    {
        JsonArray mX = ga("[1n]");

        mX.resize(4, g("s"));

        ASSERT(mX[0].isNumber());
        ASSERT(mX[1].isNull());
        ASSERT(mX[2].isString());
        ASSERT(mX[3].isString());
    }

    // Value Insertion
    {
        JsonArray mX;

        Json        mY = g("s");
        const Json& Y  = mY;

        JsonArray::Iterator insertion1 = mX.insert(static_cast<bsl::size_t>(0),
                                                   Y);
        ASSERT(&*insertion1 == &mX[0]);
        ASSERT(mX[0].isString());

        JsonArray::Iterator insertion2 =
                         mX.insert(1, bslmf::MovableRefUtil::move(mY));
        ASSERT(&*insertion2 == &mX[1]);
        ASSERT(mX[1].isString());

        JsonArray::Iterator insertion3 = mX.insert(static_cast<bsl::size_t>(0),
                                                   g("n"));
        ASSERT(&*insertion3 == &mX[0]);
        ASSERT(mX[0].isNull());
    }

    // Range Insertion
    {
        JsonArray mX = ga("[1]");

        Json range[2] = { g("n"), g("s") };

        JsonArray::Iterator insertion = mX.insert(static_cast<bsl::size_t>(0),
                                                  range,
                                                  range + 2);
        ASSERT(&*insertion == &mX[0]);

        ASSERT(mX[0].isNull());
        ASSERT(mX[1].isString());
        ASSERT(mX[2].isNumber());
    }

    // Value Insertion at an Iterator
    {
        JsonArray mX;

        Json        mY = g("s");
        const Json& Y  = mY;

        JsonArray::Iterator insertion1 = mX.insert(mX.begin(), Y);
        ASSERT(&*insertion1 == &mX[0]);
        ASSERT(mX[0].isString());

        JsonArray::Iterator insertion2 =
                         mX.insert(1, bslmf::MovableRefUtil::move(mY));
        ASSERT(&*insertion2 == &mX[1]);
        ASSERT(mX[1].isString());

        JsonArray::Iterator insertion3 = mX.insert(mX.begin(), g("n"));
        ASSERT(&*insertion3 == &mX[0]);
        ASSERT(mX[0].isNull());
    }

    // Range Insertion at an Iterator
    {
        JsonArray mX = ga("[1]");

        Json range[2] = { g("n"), g("s") };

        JsonArray::Iterator insertion = mX.insert(mX.begin(),
                                                  range,
                                                  range + 2);
        ASSERT(&*insertion == &mX[0]);

        ASSERT(mX[0].isNull());
        ASSERT(mX[1].isString());
        ASSERT(mX[2].isNumber());
    }

    // Erase
    {
        JsonArray mX = ga("[1ns]");

        JsonArray::Iterator erasure = mX.erase(1);
        ASSERT(&*erasure == &mX[1]);

        ASSERT(mX[0].isNumber());
        ASSERT(mX[1].isString());
    }

    // Erasure at an Iterator
    {
        JsonArray mX = ga("[1ns]");

        JsonArray::Iterator erasure = mX.erase(mX.begin() + 1);
        ASSERT(&*erasure == &mX[1]);

        ASSERT(mX[0].isNumber());
        ASSERT(mX[1].isString());
    }

    // Range Erasure
    {
        JsonArray mX = ga("[1nsf]");

        JsonArray::Iterator erasure = mX.erase(mX.begin() + 1,
                                               mX.begin() + 3);
        ASSERT(&*erasure == &mX[1]);

        ASSERT(mX[0].isNumber());
        ASSERT(mX[1].isBoolean());
    }

    // PushBack
    {
        JsonArray mX;

        Json        mY = g("s");
        const Json& Y  = mY;

        mX.pushBack(Y);
        ASSERT(mX[0].isString());

        mX.pushBack(bslmf::MovableRefUtil::move(mY));
        ASSERT(mX[1].isString());
    }

    // PopBack
    {
        JsonArray mX = ga("[1s]");

        mX.popBack();
        ASSERT(mX[0].isNumber());
        ASSERT(mX.size() == 1);

        mX.popBack();
        ASSERT(mX.size() == 0);
    }

    // Clear
    {
        JsonArray mX = ga("[1s]");

        mX.clear();

        ASSERT(mX.size() == 0);
    }

    // (Member) Swap
    {
        JsonArray mX = ga("[1s]");
        JsonArray mY = ga("[n]");

        mX.swap(mY);

        ASSERT(mX[0].isNull());

        ASSERT(mY[0].isNumber());
        ASSERT(mY[1].isString());
    }

    // (Const) Subscript Operator
    {
        JsonArray        mX = ga("[1ns]");
        const JsonArray& X  = mX;

        ASSERT(X[0].isNumber());
        ASSERT(X[1].isNull());
        ASSERT(X[2].isString());
    }

    // (Const) Begin
    {
        JsonArray        mX = ga("[1ns]");
        const JsonArray& X  = mX;

        JsonArray::ConstIterator begin = X.begin();

        ASSERT(&*begin == &X[0]);
        ++begin;
        ASSERT(&*begin == &X[1]);
        ++begin;
        ASSERT(&*begin == &X[2]);

        JsonArray::ConstIterator cbegin = X.cbegin();

        ASSERT(&*cbegin == &X[0]);
        ++cbegin;
        ASSERT(&*cbegin == &X[1]);
        ++cbegin;
        ASSERT(&*cbegin == &X[2]);
    }

    // (Const) End
    {
        JsonArray        mX = ga("[1ns]");
        const JsonArray& X  = mX;

        JsonArray::ConstIterator end = X.end();

        --end;
        ASSERT(&*end == &X[2]);
        --end;
        ASSERT(&*end == &X[1]);
        --end;
        ASSERT(&*end == &X[0]);

        JsonArray::ConstIterator cend = X.cend();

        --cend;
        ASSERT(&*cend == &X[2]);
        --cend;
        ASSERT(&*cend == &X[1]);
        --cend;
        ASSERT(&*cend == &X[0]);
    }

    // (Const) Front and Back
    {
        JsonArray        mX = ga("[1ns]");
        const JsonArray& X  = mX;

        ASSERT(&X.front() == &X[0]);
        ASSERT(&X.back() == &X[2]);
    }

    // Empty
    {
        JsonArray        mX = ga("[1ns]");
        const JsonArray& X  = mX;

        ASSERT(!X.empty());

        JsonArray        mY;
        const JsonArray& Y = mY;

        ASSERT(Y.empty());
    }

    // Size
    {
        JsonArray        mX = ga("[1ns]");
        const JsonArray& X  = mX;

        ASSERT(X.size() == 3);

        JsonArray        mY;
        const JsonArray& Y = mY;

        ASSERT(Y.size() == 0);
    }

    // Allocator
    {
        JsonArray        mX;
        const JsonArray& X = mX;

        ASSERT(X.allocator() == bslma::Default::allocator());

        bslma::TestAllocator ta;
        JsonArray            mY(&ta);
        const JsonArray&     Y = mY;

        ASSERT(Y.allocator() == &ta);
    }

    // Print
    {
        JsonArray        mX = ga("[1[ns]t{}]");
        const JsonArray& X  = mX;

        bsl::ostringstream stream;
        bsl::ostream&      streamRef = X.print(stream);
        ASSERT(&streamRef == &stream);

        const bsl::string&     result         = stream.str();
        const bsl::string_view expectedResult =
            "[\n"
            "    1,\n"
            "    [\n"
            "        null,\n"
            "        \"string\"\n"
            "    ],\n"
            "    true,\n"
            "    {\n"
            "    }\n"
            "]";
        ASSERTV(result, result == expectedResult);
    }

    // Streaming Operator
    {
        JsonArray        mX = ga("[1[ns]t{}]");
        const JsonArray& X  = mX;

        bsl::ostringstream stream;
        bsl::ostream&      streamRef = stream << X;
        ASSERT(&streamRef == &stream);

        const bsl::string&     result         = stream.str();
        const bsl::string_view expectedResult =
            "[1, [null, \"string\"], true, {}]";
        ASSERTV(result, result == expectedResult);
    }

    // Equality and Inequality Operators
    {
        JsonArray        mX = ga("[1s]");
        const JsonArray& X  = mX;

        JsonArray        mY = ga("[1s]");
        const JsonArray& Y  = mY;

        JsonArray        mZ = ga("[1stn]");
        const JsonArray& Z  = mZ;

        ASSERT(  X == Y );
        ASSERT(!(X == Z));
        ASSERT(!(Y == Z));

        ASSERT(!(X != Y));
        ASSERT(  X != Z );
        ASSERT(  Y != Z );
    }

    // HashAppend
    {
        // TBD: How to test?
    }

    // (Free) Swap
    {
        JsonArray mX = ga("[1s]");
        JsonArray mY = ga("[n]");

        using namespace bsl;
        swap(mX, mY);

        ASSERT(mX[0].isNull());

        ASSERT(mY[0].isNumber());
        ASSERT(mY[1].isString());
    }

    if (veryVerbose) {
        bsl::cout << "Testing 'JsonObject' Operations" << bsl::endl
                  << "-------------------------------" << bsl::endl;
    }

    // Default Constructor
    {
        JsonObject        mX;
        const JsonObject& X  = mX;
        ASSERT(X.size() == 0);
        ASSERT(X.cbegin() == X.cend());
        ASSERT(X.allocator() == bslma::Default::allocator());

        bslma::TestAllocator ta;
        JsonObject           mY;
        const JsonObject&    Y = mY;
        ASSERT(Y.size() == 0);
        ASSERT(Y.cbegin() == Y.cend());
        ASSERT(Y.allocator() == bslma::Default::allocator());
    }

    // Copy Constructor
    {
        JsonObject        mX;
        const JsonObject& X = mX;

        mX.insert(JsonObject::Member("a", Json()));

        JsonObject        mY(X);
        const JsonObject& Y = mY;

        ASSERT(X.size() == 1);
        ASSERT(X.cbegin()->first == "a");
        ASSERT(X.cbegin()->second.isNull());
        ASSERT(X.allocator() == bslma::Default::allocator());

        ASSERT(Y.size() == 1);
        ASSERT(Y.cbegin()->first == "a");
        ASSERT(Y.cbegin()->second.isNull());
        ASSERT(Y.allocator() == bslma::Default::allocator());
    }

    // Extended Copy Constructor
    {
        JsonObject        mX;
        const JsonObject& X = mX;

        mX.insert(JsonObject::Member("a", Json()));

        bslma::TestAllocator ta;

        JsonObject        mY(X, &ta);
        const JsonObject& Y = mY;

        ASSERT(X.size() == 1);
        ASSERT(X.cbegin()->first == "a");
        ASSERT(X.cbegin()->second.isNull());
        ASSERT(X.allocator() == bslma::Default::allocator());

        ASSERT(Y.size() == 1);
        ASSERT(Y.cbegin()->first == "a");
        ASSERT(Y.cbegin()->second.isNull());
        ASSERT(Y.allocator() == &ta);
    }

    // Move Constructor
    {
        JsonObject        mX;
        const JsonObject& X = mX;

        mX.insert(JsonObject::Member("a", Json()));

        JsonObject        mY(bslmf::MovableRefUtil::move(mX));
        const JsonObject& Y = mY;

        ASSERT(X.size() == 0);
        ASSERT(X.allocator() == bslma::Default::allocator());

        ASSERT(Y.size() == 1);
        ASSERT(Y.cbegin()->first == "a");
        ASSERT(Y.cbegin()->second.isNull());
        ASSERT(Y.allocator() == bslma::Default::allocator());
    }

    // Extended Move Constructor
    {
        JsonObject        mX;
        const JsonObject& X = mX;

        mX.insert(JsonObject::Member("a", Json()));

        bslma::TestAllocator ta;

        JsonObject        mY(bslmf::MovableRefUtil::move(mX), &ta);
        const JsonObject& Y = mY;

        ASSERT(X.size() == 1);
        ASSERT(X.cbegin()->first == "a");
        ASSERT(X.cbegin()->second.isNull());
        ASSERT(X.allocator() == bslma::Default::allocator());

        ASSERT(Y.size() == 1);
        ASSERT(Y.cbegin()->first == "a");
        ASSERT(Y.cbegin()->second.isNull());
        ASSERT(Y.allocator() == &ta);
    }

    // Range Constructor
    {
        JsonObject::Member members[2] = {
            JsonObject::Member("a", Json()),
            JsonObject::Member("b", Json(""))
        };

        JsonObject        mX(members, members + 2);
        const JsonObject& X = mX;

        ASSERT(X.size() == 2);
        ASSERT(X.cbegin()->first == "a" ||
               X.cbegin()->first == "b");
        ASSERT((++X.cbegin())->first == "a" ||
               (++X.cbegin())->first == "b");
        ASSERT(X.cbegin()->second.isNull() ||
               X.cbegin()->second.isString());
        ASSERT((++X.cbegin())->second.isNull() ||
               (++X.cbegin())->second.isString());
        ASSERT(X.allocator() == bslma::Default::allocator());
    }

    // Extended Range Constructor
    {
        JsonObject::Member members[2] = {
            JsonObject::Member("a", Json()),
            JsonObject::Member("b", Json(""))
        };

        bslma::TestAllocator ta;

        JsonObject        mX(members, members + 2, &ta);
        const JsonObject& X = mX;

        ASSERT(X.size() == 2);
        ASSERT(X.cbegin()->first == "a" ||
               X.cbegin()->first == "b");
        ASSERT((++X.cbegin())->first == "a" ||
               (++X.cbegin())->first == "b");
        ASSERT(X.cbegin()->second.isNull() ||
               X.cbegin()->second.isString());
        ASSERT((++X.cbegin())->second.isNull() ||
               (++X.cbegin())->second.isString());
        ASSERT(X.allocator() == &ta);
    }

#if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
    // Initializer-List Constructor
    {
        JsonObject        mX({{"a", Json()}, {"b", Json("")}});
        const JsonObject& X = mX;

        ASSERT(X.size() == 2);
        ASSERT(X.cbegin()->first == "a" ||
               X.cbegin()->first == "b");
        ASSERT((++X.cbegin())->first == "a" ||
               (++X.cbegin())->first == "b");
        ASSERT(X.cbegin()->second.isNull() ||
               X.cbegin()->second.isString());
        ASSERT((++X.cbegin())->second.isNull() ||
               (++X.cbegin())->second.isString());
        ASSERT(X.allocator() == bslma::Default::allocator());
    }

    // Extended Initializer-List Constructor
    {
        bslma::TestAllocator ta;

        JsonObject        mX({{"a", Json()}, {"b", Json("")}}, &ta);
        const JsonObject& X = mX;

        ASSERT(X.size() == 2);
        ASSERT(X.cbegin()->first == "a" ||
               X.cbegin()->first == "b");
        ASSERT((++X.cbegin())->first == "a" ||
               (++X.cbegin())->first == "b");
        ASSERT(X.cbegin()->second.isNull() ||
               X.cbegin()->second.isString());
        ASSERT((++X.cbegin())->second.isNull() ||
               (++X.cbegin())->second.isString());
        ASSERT(X.allocator() == &ta);
    }
#endif

    // Copy-Assignment Operator
    {
        JsonObject        mX;
        const JsonObject& X = mX;

        bslma::TestAllocator ta;
        JsonObject           mY(&ta);
        const JsonObject&    Y = mY;
        mY.insert(JsonObject::Member("a", Json()));
        mY.insert(JsonObject::Member("b", Json("")));

        JsonObject& mXRef = (mX = Y);
        ASSERT(&mXRef == &mX);

        ASSERT(X.size() == 2);
        ASSERT(X.cbegin()->first == "a" ||
               X.cbegin()->first == "b");
        ASSERT((++X.cbegin())->first == "a" ||
               (++X.cbegin())->first == "b");
        ASSERT(X.cbegin()->second.isNull() ||
               X.cbegin()->second.isString());
        ASSERT((++X.cbegin())->second.isNull() ||
               (++X.cbegin())->second.isString());
        ASSERT(X.allocator() == bslma::Default::allocator());

        ASSERT(Y.size() == 2);
        ASSERT(Y.cbegin()->first == "a" ||
               Y.cbegin()->first == "b");
        ASSERT((++Y.cbegin())->first == "a" ||
               (++Y.cbegin())->first == "b");
        ASSERT(Y.cbegin()->second.isNull() ||
               Y.cbegin()->second.isString());
        ASSERT((++Y.cbegin())->second.isNull() ||
               (++Y.cbegin())->second.isString());
        ASSERT(Y.allocator() == &ta);
    }

    // Move-Assignment Operator
    {
        JsonObject        mX;
        const JsonObject& X = mX;

        bslma::TestAllocator ta;
        JsonObject           mY(&ta);
        const JsonObject&    Y = mY;
        mY.insert(JsonObject::Member("a", Json()));
        mY.insert(JsonObject::Member("b", Json("")));

        JsonObject& mXRef = (mX = bslmf::MovableRefUtil::move(mY));
        ASSERT(&mXRef == &mX);

        ASSERT(X.size() == 2);
        ASSERT(X.cbegin()->first == "a" ||
               X.cbegin()->first == "b");
        ASSERT((++X.cbegin())->first == "a" ||
               (++X.cbegin())->first == "b");
        ASSERT(X.cbegin()->second.isNull() ||
               X.cbegin()->second.isString());
        ASSERT((++X.cbegin())->second.isNull() ||
               (++X.cbegin())->second.isString());
        ASSERT(X.allocator() == bslma::Default::allocator());

        ASSERT(X == Y);
        ASSERT(Y.allocator() == &ta);
    }

    // Subscript Operator
    {
        JsonObject mX;
        mX.insert(JsonObject::Member("a", Json()));
        mX.insert(JsonObject::Member("b", Json("")));

        ASSERT(mX["a"].isNull());
        ASSERT(mX["b"].isString());
    }

    // Begin
    {
        JsonObject mX;
        mX.insert(JsonObject::Member("a", Json()));
        mX.insert(JsonObject::Member("b", Json("")));

        JsonObject::Iterator begin = mX.begin();
        ASSERT(begin->first == "a" || begin->first == "b");
        ASSERT(begin->second.isNull() || begin->second.isString());

        ++begin;
        ASSERT(begin->first == "a" || begin->first == "b");
        ASSERT(begin->second.isNull() || begin->second.isString());
    }

    // End
    {
        JsonObject mX;
        mX.insert(JsonObject::Member("a", Json()));
        mX.insert(JsonObject::Member("b", Json("")));

        JsonObject::Iterator begin = mX.begin();
        JsonObject::Iterator end   = mX.end();

        ASSERT(begin != end);

        ++begin;
        ++begin;

        ASSERT(begin == end);
    }

    // Clear
    {
        JsonObject        mX;
        const JsonObject& X = mX;
        mX.insert(JsonObject::Member("a", Json()));
        mX.insert(JsonObject::Member("b", Json("")));

        mX.clear();

        ASSERT(X.size() == 0);
    }

    // Copy Insertion
    {
        JsonObject        mX;
        const JsonObject& X = mX;

        JsonObject::Member                    member("a", Json());
        bsl::pair<JsonObject::Iterator, bool> insertion = mX.insert(member);
        ASSERT(insertion.first->first == "a");
        ASSERT(insertion.first->second.isNull());
        ASSERT(insertion.second);
        ASSERT(X.size() == 1);

        insertion = mX.insert(member);
        ASSERT(!insertion.second);
    }

    // Move Insertion
    {
        JsonObject        mX;
        const JsonObject& X = mX;

        JsonObject::Member                    member1("a", Json());
        bsl::pair<JsonObject::Iterator, bool> insertion =
                           mX.insert(bslmf::MovableRefUtil::move(member1));
        ASSERT(insertion.first->first == "a");
        ASSERT(insertion.first->second.isNull());
        ASSERT(insertion.second);
        ASSERT(X.size() == 1);

        JsonObject::Member member2("a", Json());
        insertion = mX.insert(bslmf::MovableRefUtil::move(member2));
        ASSERT(!insertion.second);
    }

    // Range Insertion
    {
        bslma::TestAllocator ta;

        JsonObject        mX(&ta);
        const JsonObject& X = mX;

        JsonObject::Member members[] = {
            JsonObject::Member("a", Json()),
            JsonObject::Member("b", Json(""))
        };

        mX.insert(members, members + 2);

        ASSERT(X.size() == 2);
        ASSERT(X.cbegin()->first == "a" ||
               X.cbegin()->first == "b");
        ASSERT((++X.cbegin())->first == "a" ||
               (++X.cbegin())->first == "b");
        ASSERT(X.cbegin()->second.isNull() ||
               X.cbegin()->second.isString());
        ASSERT((++X.cbegin())->second.isNull() ||
               (++X.cbegin())->second.isString());
        ASSERT(X.allocator() == &ta);
    }

#if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
    // Initializer-List Insertion
    {
        bslma::TestAllocator ta;

        JsonObject        mX(&ta);
        const JsonObject& X = mX;

        mX.insert({{"a", Json()}, {"b", Json("")}});

        ASSERT(X.size() == 2);
        ASSERT(X.cbegin()->first == "a" ||
               X.cbegin()->first == "b");
        ASSERT((++X.cbegin())->first == "a" ||
               (++X.cbegin())->first == "b");
        ASSERT(X.cbegin()->second.isNull() ||
               X.cbegin()->second.isString());
        ASSERT((++X.cbegin())->second.isNull() ||
               (++X.cbegin())->second.isString());
        ASSERT(X.allocator() == &ta);
    }
#endif

    // Key-Value Insertion
    {
        bslma::TestAllocator ta;

        JsonObject        mX(&ta);
        const JsonObject& X = mX;

        mX.insert("a", Json());
        mX.insert("b", Json(""));

        ASSERT(X.size() == 2);
        ASSERT(X.cbegin()->first == "a" ||
               X.cbegin()->first == "b");
        ASSERT((++X.cbegin())->first == "a" ||
               (++X.cbegin())->first == "b");
        ASSERT(X.cbegin()->second.isNull() ||
               X.cbegin()->second.isString());
        ASSERT((++X.cbegin())->second.isNull() ||
               (++X.cbegin())->second.isString());
        ASSERT(X.allocator() == &ta);
    }

    // Erase
    {
        bslma::TestAllocator ta;

        JsonObject        mX(&ta);
        const JsonObject& X = mX;

        bsl::size_t numErased = mX.erase("a");
        ASSERT(numErased == 0);
        ASSERT(X.size() == 0);

        mX.insert(JsonObject::Member("a", Json()));
        numErased = mX.erase("a");
        ASSERT(numErased == 1);
        ASSERT(X.size() == 0);
    }

    // Iterator Erasure
    {
        JsonObject        mX;
        const JsonObject& X = mX;

        mX.insert(JsonObject::Member("a", Json()));

        JsonObject::Iterator bIter =
                        mX.insert(JsonObject::Member("b", Json(""))).first;

        JsonObject::Iterator bErasure = mX.erase(bIter);
        ASSERT(bErasure == mX.end() ||
               bErasure->first == "a");
        ASSERT(X.size() == 1);
    }

    // Const-Iterator Erasure
    {
        JsonObject        mX;
        const JsonObject& X = mX;

        mX.insert(JsonObject::Member("a", Json()));

        JsonObject::Iterator bIter =
                        mX.insert(JsonObject::Member("b", Json(""))).first;

        JsonObject::Iterator bErasure =
                   mX.erase(static_cast<JsonObject::ConstIterator>(bIter));
        ASSERT(bErasure == mX.end() ||
               bErasure->first == "a");
        ASSERT(X.size() == 1);
    }

    // (Member) Swap
    {
        JsonObject        mX;
        const JsonObject& X = mX;
        mX.insert(JsonObject::Member("a", Json()));
        mX.insert(JsonObject::Member("b", Json("")));

        JsonObject        mY;
        const JsonObject& Y = mY;
        mY.insert(JsonObject::Member("c", Json(false)));

        mX.swap(mY);

        ASSERT(X.size() == 1);
        ASSERT(X.cbegin()->first == "c");
        ASSERT(X.cbegin()->second.isBoolean());

        ASSERT(Y.size() == 2);
        ASSERT(Y.cbegin()->first == "a" ||
               Y.cbegin()->first == "b");
        ASSERT((++Y.cbegin())->first == "a" ||
               (++Y.cbegin())->first == "b");
        ASSERT(Y.cbegin()->second.isNull() ||
               Y.cbegin()->second.isString());
        ASSERT((++Y.cbegin())->second.isNull() ||
               (++Y.cbegin())->second.isString());
    }

    // (Const) Subscript Operator
    {
        JsonObject        mX;
        const JsonObject& X = mX;
        mX.insert(JsonObject::Member("a", Json()));
        mX.insert(JsonObject::Member("b", Json("")));

        ASSERT(X["a"].isNull());
        ASSERT(X["b"].isString());
    }

    // (Const) Begin
    {
        JsonObject        mX;
        const JsonObject& X = mX;
        mX.insert(JsonObject::Member("a", Json()));
        mX.insert(JsonObject::Member("b", Json("")));

        JsonObject::ConstIterator begin = X.begin();
        ASSERT(begin->first == "a" || begin->first == "b");
        ASSERT(begin->second.isNull() || begin->second.isString());

        ++begin;
        ASSERT(begin->first == "a" || begin->first == "b");
        ASSERT(begin->second.isNull() || begin->second.isString());

        JsonObject::ConstIterator cbegin = X.cbegin();
        ASSERT(cbegin->first == "a" || cbegin->first == "b");
        ASSERT(cbegin->second.isNull() || cbegin->second.isString());

        ++cbegin;
        ASSERT(cbegin->first == "a" || cbegin->first == "b");
        ASSERT(cbegin->second.isNull() || cbegin->second.isString());
    }

    // (Const) End
    {
        JsonObject        mX;
        const JsonObject& X = mX;
        mX.insert(JsonObject::Member("a", Json()));
        mX.insert(JsonObject::Member("b", Json("")));

        JsonObject::ConstIterator begin = X.begin();
        JsonObject::ConstIterator end   = X.end();

        ASSERT(begin != end);

        ++begin;
        ++begin;

        ASSERT(begin == end);

        JsonObject::ConstIterator cbegin = X.cbegin();
        JsonObject::ConstIterator cend   = X.cend();

        ASSERT(cbegin != cend);

        ++cbegin;
        ++cbegin;

        ASSERT(cbegin == cend);
    }

    // Find
    {
        JsonObject        mX;
        const JsonObject& X = mX;
        mX.insert(JsonObject::Member("a", Json()));
        mX.insert(JsonObject::Member("b", Json("")));

        JsonObject::ConstIterator found = X.find("a");
        ASSERT(found != X.end());
        ASSERT(found->first == "a");
        ASSERT(found->second.isNull());

        JsonObject::ConstIterator notFound = X.find("c");
        ASSERT(notFound == X.end());
    }

    // Contains
    {
        JsonObject        mX;
        const JsonObject& X = mX;
        mX.insert(JsonObject::Member("a", Json()));
        mX.insert(JsonObject::Member("b", Json("")));

        ASSERT( X.contains("a"));
        ASSERT( X.contains("b"));
        ASSERT(!X.contains("c"));
    }

    // Empty
    {
        JsonObject        mX;
        const JsonObject& X = mX;
        mX.insert(JsonObject::Member("a", Json()));
        mX.insert(JsonObject::Member("b", Json("")));

        ASSERT(!X.empty());

        JsonObject        mY;
        const JsonObject& Y = mY;

        ASSERT(Y.empty());
    }

    // Size
    {
        JsonObject        mX;
        const JsonObject& X = mX;
        mX.insert(JsonObject::Member("a", Json()));
        mX.insert(JsonObject::Member("b", Json("")));

        ASSERT(X.size() == 2);

        JsonObject        mY;
        const JsonObject& Y = mY;

        ASSERT(Y.size() == 0);
    }

    // Allocator
    {
        JsonObject        mX;
        const JsonObject& X = mX;

        ASSERT(X.allocator() == bslma::Default::allocator());

        bslma::TestAllocator ta;

        JsonObject        mY(&ta);
        const JsonObject& Y = mY;

        ASSERT(Y.allocator() == &ta);
    }

    // Print
    {
        JsonObject        mX = go("{a{b[1f]}}");
        const JsonObject& X  = mX;

        bsl::ostringstream stream;
        bsl::ostream&      streamRef = X.print(stream);
        ASSERT(&streamRef == &stream);

        const bsl::string&     result         = stream.str();
        const bsl::string_view expectedResult =
            "{\n"
            "    \"memberA\": {\n"
            "        \"memberB\": [\n"
            "            1,\n"
            "            false\n"
            "        ]\n"
            "    }\n"
            "}";
        ASSERTV(result, result == expectedResult);
    }

    // Streaming Operator
    {
        JsonObject        mX = go("{a{b[1f]}}");
        const JsonObject& X  = mX;

        bsl::ostringstream stream;
        bsl::ostream&      streamRef = stream << X;
        ASSERT(&streamRef == &stream);

        const bsl::string&     result = stream.str();
        const bsl::string_view expectedResult =
            "{\"memberA\": {\"memberB\": [1, false]}}";
        ASSERTV(result, result == expectedResult);
    }

    // Equality and Inequality Operators
    {
        JsonObject        mX;
        const JsonObject& X = mX;
        mX.insert(JsonObject::Member("a", Json()));
        mX.insert(JsonObject::Member("b", Json("")));

        JsonObject        mY;
        const JsonObject& Y = mY;
        mY.insert(JsonObject::Member("a", Json()));
        mY.insert(JsonObject::Member("b", Json("")));

        JsonObject        mZ;
        const JsonObject& Z = mZ;
        mZ.insert(JsonObject::Member("c", Json(false)));

        ASSERT(  X == Y );
        ASSERT(!(X == Z));
        ASSERT(!(Y == Z));

        ASSERT(!(X != Y));
        ASSERT(  X != Z );
        ASSERT(  Y != Z );
    }

    // HashAppend
    {
        // TBD: How to test?
    }

    // (Free) Swap
    {
        JsonObject mX;
        mX.insert(JsonObject::Member("a", Json()));
        mX.insert(JsonObject::Member("b", Json("")));

        JsonObject mY;
        mY.insert(JsonObject::Member("c", Json(false)));

        using namespace bsl;
        swap(mX, mY);

        ASSERT(mX["c"].isBoolean());

        ASSERT(mY["a"].isNull());
        ASSERT(mY["b"].isString());
    }

    if (verbose) {
        bsl::cout << "Testing 'Json' Operations" << bsl::endl
                  << "-------------------------" << bsl::endl;
    }

    // Default Constructor
    {
        Json        mX;
        const Json& X = mX;

        ASSERT(X.isNull());
        ASSERT(X.allocator() == bslma::Default::allocator());

        bslma::TestAllocator ta;

        Json        mY(&ta);
        const Json& Y = mY;

        ASSERT(Y.isNull());
        ASSERT(Y.allocator() == &ta);
    }

    // Copy Constructor
    {
        Json        mX;
        const Json& X = mX;

        mX.makeString("");

        Json        mY(X);
        const Json& Y = mY;

        ASSERT(X.isString());
        ASSERT(Y.isString());
        ASSERT(Y.allocator() == bslma::Default::allocator());

        bslma::TestAllocator ta;

        Json        mZ(X, &ta);
        const Json& Z = mZ;

        ASSERT(X.isString());
        ASSERT(Z.isString());
        ASSERT(Z.allocator() == &ta);
    }

    // Move Constructor
    {
        Json mX;
        mX.makeString("");

        Json        mY(bslmf::MovableRefUtil::move(mX));
        const Json& Y = mY;

        ASSERT(Y.isString());

        Json        mW;
        const Json& W = mW;

        mW.makeString("");

        bslma::TestAllocator ta;

        Json        mZ(bslmf::MovableRefUtil::move(mW), &ta);
        const Json& Z = mZ;

        ASSERT(W.isString());
        ASSERT(Z.isString());
    }

    // Array Constructor
    {
        JsonArray array1;
        array1.pushBack(Json());
        array1.pushBack(Json(""));

        Json        mW(array1);
        const Json& W = mW;

        ASSERT(W.isArray());
        ASSERT(W.size() == 2);
        ASSERT(W[0].isNull());
        ASSERT(W[1].isString());
        ASSERT(W.allocator() == bslma::Default::allocator());

        bslma::TestAllocator ta;

        Json        mX(array1, &ta);
        const Json& X = mX;

        ASSERT(X.isArray());
        ASSERT(X.size() == 2);
        ASSERT(X[0].isNull());
        ASSERT(X[1].isString());
        ASSERT(X.allocator() == &ta);

        Json        mY(bslmf::MovableRefUtil::move(array1));
        const Json& Y = mY;

        ASSERT(array1.size() == 0);

        ASSERT(Y.isArray());
        ASSERT(Y.size() == 2);
        ASSERT(Y[0].isNull());
        ASSERT(Y[1].isString());
        ASSERT(Y.allocator() == bslma::Default::allocator());

        JsonArray array2;
        array2.pushBack(Json());
        array2.pushBack(Json(""));

        Json        mZ(bslmf::MovableRefUtil::move(array2), &ta);
        const Json& Z = mZ;

        ASSERT(Z.isArray());
        ASSERT(Z.size() == 2);
        ASSERT(Z[0].isNull());
        ASSERT(Z[1].isString());
        ASSERT(Z.allocator() == &ta);
    }

    // Boolean Constructor
    {
        Json        mX(false);
        const Json& X = mX;

        ASSERT(X.isBoolean());
        ASSERT(X.allocator() == bslma::Default::allocator());

        bslma::TestAllocator ta;

        Json        mY(true, &ta);
        const Json& Y = mY;

        ASSERT(Y.isBoolean());
        ASSERT(Y.allocator() == &ta);
    }

    // Null Constructor
    {
        Json        mX((JsonNull()));
        const Json& X = mX;

        ASSERT(X.isNull());
        ASSERT(X.allocator() == bslma::Default::allocator());

        bslma::TestAllocator ta;

        Json mY(JsonNull(), &ta);
    }

    // Arithmetic-Type Constructors
    {
        bslma::TestAllocator ta;

        Json m1(1.5f);
        ASSERT(m1.isNumber());
        ASSERT(m1.asFloat() == 1.5f);
        ASSERT(m1.allocator() == bslma::Default::allocator());

        Json m2(1.5f, &ta);
        ASSERT(m2.isNumber());
        ASSERT(m2.asFloat() == 1.5f);
        ASSERT(m2.allocator() == &ta);

        Json m3(1.5);
        ASSERT(m3.isNumber());
        ASSERT(m3.asDouble() == 1.5);
        ASSERT(m3.allocator() == bslma::Default::allocator());

        Json m4(1.5, &ta);
        ASSERT(m4.isNumber());
        ASSERT(m4.asDouble() == 1.5);
        ASSERT(m4.allocator() == &ta);

        Json m5(bdldfp::Decimal64(1.5));
        ASSERT(m5.isNumber());
        ASSERT(m5.asDecimal64() == bdldfp::Decimal64(1.5));
        ASSERT(m5.allocator() == bslma::Default::allocator());

        Json m6(bdldfp::Decimal64(1.5), &ta);
        ASSERT(m6.isNumber());
        ASSERT(m6.asDecimal64() == bdldfp::Decimal64(1.5));
        ASSERT(m6.allocator() == &ta);

        Json m7(1);
        ASSERT(m7.isNumber());
        int m7Value;
        int rc = m7.asInt(&m7Value);
        ASSERT(rc == 0);
        ASSERT(m7Value == 1);
        ASSERT(m7.allocator() == bslma::Default::allocator());

        Json m8(1, &ta);
        ASSERT(m8.isNumber());
        int m8Value;
        rc = m8.asInt(&m8Value);
        ASSERT(rc == 0);
        ASSERT(m8Value == 1);
        ASSERT(m8.allocator() == &ta);

        Json         m9(1u);
        unsigned int m9Value;
        rc = m9.asUint(&m9Value);
        ASSERT(rc == 0);
        ASSERT(m9Value == 1u);
        ASSERT(m9.allocator() == bslma::Default::allocator());

        Json         m10(1u, &ta);
        unsigned int m10Value;
        rc = m10.asUint(&m10Value);
        ASSERT(rc == 0);
        ASSERT(m10Value == 1u);
        ASSERT(m10.allocator() == &ta);

        Json               m11(static_cast<bsls::Types::Int64>(1));
        bsls::Types::Int64 m11Value;
        rc = m11.asInt64(&m11Value);
        ASSERT(rc == 0);
        ASSERT(m11Value == static_cast<bsls::Types::Int64>(1));
        ASSERT(m11.allocator() == bslma::Default::allocator());

        Json               m12(static_cast<bsls::Types::Int64>(1), &ta);
        bsls::Types::Int64 m12Value;
        rc = m12.asInt64(&m12Value);
        ASSERT(rc == 0);
        ASSERT(m12Value == static_cast<bsls::Types::Int64>(1));
        ASSERT(m12.allocator() == &ta);

        Json                m13(static_cast<bsls::Types::Uint64>(1u));
        bsls::Types::Uint64 m13Value;
        rc = m13.asUint64(&m13Value);
        ASSERT(rc == 0);
        ASSERT(m13Value == static_cast<bsls::Types::Uint64>(1u));
        ASSERT(m13.allocator() == bslma::Default::allocator());

        Json                m14(static_cast<bsls::Types::Uint64>(1u), &ta);
        bsls::Types::Uint64 m14Value;
        rc = m14.asUint64(&m14Value);
        ASSERT(rc == 0);
        ASSERT(m14Value == static_cast<bsls::Types::Uint64>(1u));
        ASSERT(m14.allocator() == &ta);
    }

    // Number Constructor
    {
        JsonNumber number1("1.5");

        Json        mW(number1);
        const Json& W = mW;
        ASSERT(W.isNumber());
        ASSERT(W.theNumber() == number1);
        ASSERT(W.allocator() == bslma::Default::allocator());

        bslma::TestAllocator ta;

        Json        mX(number1, &ta);
        const Json& X = mX;
        ASSERT(X.isNumber());
        ASSERT(X.theNumber() == number1);
        ASSERT(X.allocator() == &ta);

        Json        mY(bslmf::MovableRefUtil::move(number1));
        const Json& Y = mY;
        ASSERT(Y.isNumber());
        ASSERT(Y.theNumber() == JsonNumber("1.5"));
        ASSERT(Y.allocator() == bslma::Default::allocator());

        JsonNumber number2("1.5");

        Json        mZ(bslmf::MovableRefUtil::move(number2), &ta);
        const Json& Z = mZ;
        ASSERT(Z.isNumber());
        ASSERT(Z.theNumber() == JsonNumber("1.5"));
        ASSERT(Z.allocator() == &ta);
    }

    // Object Constructor
    {
        JsonObject object1;
        object1.insert(JsonObject::Member("a", Json()));

        Json        mW(object1);
        const Json& W = mW;
        ASSERT(W.isObject());
        ASSERT(W.size() == 1);
        ASSERT(W["a"].isNull());
        ASSERT(W.allocator() == bslma::Default::allocator());

        bslma::TestAllocator ta;

        Json        mX(object1, &ta);
        const Json& X = mX;
        ASSERT(X.isObject());
        ASSERT(X.size() == 1);
        ASSERT(X["a"].isNull());
        ASSERT(X.allocator() == &ta);

        Json        mY(bslmf::MovableRefUtil::move(object1));
        const Json& Y = mY;
        ASSERT(Y.isObject());
        ASSERT(Y.size() == 1);
        ASSERT(Y["a"].isNull());
        ASSERT(Y.allocator() == bslma::Default::allocator());

        JsonObject object2;
        object2.insert(JsonObject::Member("b", Json()));

        Json        mZ(bslmf::MovableRefUtil::move(object2), &ta);
        const Json& Z = mZ;
        ASSERT(Z.isObject());
        ASSERT(Z.size() == 1);
        ASSERT(Z["b"].isNull());
        ASSERT(Z.allocator() == &ta);
    }

    // String Constructor
    {
        Json        mU("string");
        const Json& U = mU;
        ASSERT(U.isString());
        ASSERT(U.theString() == "string");
        ASSERT(U.allocator() == bslma::Default::allocator());

        bslma::TestAllocator ta;

        Json        mV("string", &ta);
        const Json& V = mV;
        ASSERT(V.isString());
        ASSERT(V.theString() == "string");
        ASSERT(V.allocator() == &ta);

        bsl::string string1("string");

        Json        mW(string1);
        const Json& W = mW;
        ASSERT(W.isString());
        ASSERT(W.theString() == "string");
        ASSERT(W.allocator() == bslma::Default::allocator());

        Json        mX(string1, &ta);
        const Json& X = mX;
        ASSERT(X.isString());
        ASSERT(X.theString() == "string");
        ASSERT(X.allocator() == &ta);

        Json        mY(bslmf::MovableRefUtil::move(string1));
        const Json& Y = mY;
        ASSERT(Y.isString());
        ASSERT(Y.theString() == "string");
        ASSERT(Y.allocator() == bslma::Default::allocator());

        bsl::string string2("string");

        Json        mZ(bslmf::MovableRefUtil::move(string1), &ta);
        const Json& Z = mZ;
        ASSERT(Z.isString());
        ASSERT(Z.theString() == "string");
        ASSERT(Z.allocator() == &ta);
    }

    // Copy-Assignment Operator
    {
        Json        mX("");
        const Json& X = mX;

        Json        mY;
        const Json& Y = mY;

        mY = X;

        ASSERT(X.isString());
        ASSERT(Y.isString());
    }

    // Move-Assignment Operator
    {
        Json mX("");

        Json        mY;
        const Json& Y = mY;

        mY = bslmf::MovableRefUtil::move(mX);

        ASSERT(Y.isString());
    }

    // Arithmetic-Type Assignment Operators
    {
        Json m1;
        ASSERT(&(m1 = 1.5f) == &m1);
        ASSERT(m1.isNumber());
        ASSERT(m1.asFloat() == 1.5f);

        Json m2;
        ASSERT(&(m2 = 1.5) == &m2);
        ASSERT(m2.isNumber());
        ASSERT(m2.asDouble() == 1.5);

        Json m3;
        ASSERT(&(m3 = bdldfp::Decimal64(1.5)) == &m3);
        ASSERT(m3.isNumber());
        ASSERT(m3.asDecimal64() == bdldfp::Decimal64(1.5));

        Json m4;
        ASSERT(&(m4 = 1) == &m4);
        ASSERT(m4.isNumber());
        int m4Value;
        int rc = m4.asInt(&m4Value);
        ASSERT(rc == 0);
        ASSERT(m4Value == 1);

        Json m5;
        ASSERT(&(m5 = static_cast<bsls::Types::Int64>(1)) == &m5);
        ASSERT(m5.isNumber());
        bsls::Types::Int64 m5Value;
        rc = m5.asInt64(&m5Value);
        ASSERT(rc == 0);
        ASSERT(m5Value == static_cast<bsls::Types::Int64>(1));

        Json m6;
        ASSERT(&(m6 = 1u) == &m6);
        ASSERT(m6.isNumber());
        unsigned int m6Value;
        rc = m6.asUint(&m6Value);
        ASSERT(rc == 0);
        ASSERT(m6Value == 1u);

        Json m7;
        ASSERT(&(m7 = static_cast<bsls::Types::Uint64>(1u)) == &m7);
        ASSERT(m7.isNumber());
        bsls::Types::Uint64 m7Value;
        rc = m7.asUint64(&m7Value);
        ASSERT(rc == 0);
        ASSERT(m7Value == static_cast<bsls::Types::Uint64>(1u));
    }

    // Number Assignment Operator
    {
        JsonNumber number("1.5");

        Json        mX;
        const Json& X = mX;
        ASSERT(&(mX = number) == &mX);
        ASSERT(X.isNumber());
        ASSERT(X.theNumber() == JsonNumber("1.5"));

        Json        mY;
        const Json& Y = mY;
        ASSERT(&(mY = bslmf::MovableRefUtil::move(number)) == &mY);
        ASSERT(Y.isNumber());
        ASSERT(Y.theNumber() == JsonNumber("1.5"));
    }

    // String Assignment Operator
    {
        Json        mW;
        const Json& W = mW;
        ASSERT(&(mW = "string") == &mW);
        ASSERT(W.isString());
        ASSERT(W.theString() == "string");

        bsl::string string("string");

        Json        mX;
        const Json& X = mX;
        ASSERT(&(mX = string) == &mX);
        ASSERT(X.isString());
        ASSERT(X.theString() == "string");

        Json        mY;
        const Json& Y = mY;
        ASSERT(&(mY = bslmf::MovableRefUtil::move(string)) == &mY);
        ASSERT(Y.isString());
        ASSERT(Y.theString() == "string");
    }

    // Boolean-Assignment Operator
    {
        Json        mX;
        const Json& X = mX;

        Json& mXRef = (mX = true);

        ASSERT(&mXRef == &mX);
        ASSERT(X.isBoolean());
    }

    // Object-Assignment Operator
    {
        JsonObject object;
        object.insert(JsonObject::Member("a", Json()));

        Json        mX;
        const Json& X = mX;

        Json& mXRef = (mX = object);

        ASSERT(&mXRef == &mX);
        ASSERT(X.isObject());
        ASSERT(X.size() == 1);
        ASSERT(X["a"].isNull());

        Json        mY;
        const Json& Y = mY;

        Json& mYRef = (mY = bslmf::MovableRefUtil::move(object));

        ASSERT(&mYRef == &mY);
        ASSERT(Y.isObject());
        ASSERT(Y.size() == 1);
        ASSERT(Y["a"].isNull());
    }

    // Array-Assignment Operator
    {
        JsonArray array;
        array.pushBack(Json());
        array.pushBack(Json(""));

        Json        mX;
        const Json& X = mX;

        Json& mXRef = (mX = array);

        ASSERT(&mXRef == &mX);
        ASSERT(X.isArray());
        ASSERT(X.size() == 2);
        ASSERT(X[0].isNull());
        ASSERT(X[1].isString());

        Json        mY;
        const Json& Y = mY;

        Json& mYRef = (mY = bslmf::MovableRefUtil::move(array));

        ASSERT(&mYRef == &mY);
        ASSERT(Y.isArray());
        ASSERT(Y.size() == 2);
        ASSERT(Y[0].isNull());
        ASSERT(Y[1].isString());
    }

    // Null-Assignment Operator
    {
        JsonNull null;

        Json        mX;
        const Json& X = mX;

        Json& mXRef = (mX = null);

        ASSERT(&mXRef == &mX);
        ASSERT(X.isNull());

        Json        mY;
        const Json& Y = mY;

        Json& mYRef = (mY = bslmf::MovableRefUtil::move(null));

        ASSERT(&mYRef == &mY);
        ASSERT(Y.isNull());
    }

    // Make Array
    {
        Json        mX;
        const Json& X = mX;

        JsonArray& mXArray = mX.makeArray();

        ASSERT(X.isArray());
        ASSERT(&mXArray == &X.theArray());

        JsonArray array;
        array.pushBack(Json());
        array.pushBack(Json(""));

        Json        mY;
        const Json& Y = mY;

        JsonArray& mYArray = mY.makeArray(array);

        ASSERT(Y.isArray());
        ASSERT(&mYArray == &Y.theArray());
        ASSERT(Y.size() == 2);
        ASSERT(Y[0].isNull());
        ASSERT(Y[1].isString());

        Json        mZ;
        const Json& Z = mZ;

        JsonArray& mZArray =
                          mZ.makeArray(bslmf::MovableRefUtil::move(array));

        ASSERT(Z.isArray());
        ASSERT(&mZArray == &Z.theArray())
        ASSERT(Z.size() == 2);
        ASSERT(Z[0].isNull());
        ASSERT(Z[1].isString());
    }

    // Make Boolean
    {
        Json        mX;
        const Json& X = mX;

        bool& mXBool = mX.makeBoolean();

        ASSERT(X.isBoolean());
        ASSERT(&mXBool == &X.theBoolean());
        ASSERT(X.theBoolean() == false);

        Json        mY;
        const Json& Y = mY;

        bool& mYBool = mY.makeBoolean(true);

        ASSERT(Y.isBoolean());
        ASSERT(&mYBool == &Y.theBoolean());
        ASSERT(Y.theBoolean() == true);
    }

    // Make Null
    {
        Json        mX;
        const Json& X = mX;

        mX.makeNull();

        ASSERT(X.isNull());

        Json        mY("");
        const Json& Y = mY;

        mY.makeNull();

        ASSERT(Y.isNull());
    }

    // Make Number
    {
        Json        mX;
        const Json& X = mX;

        JsonNumber& mXNumber = mX.makeNumber();

        ASSERT(X.isNumber());
        ASSERT(&mXNumber == &X.theNumber());
        ASSERT(X.theNumber() == JsonNumber("0"));

        JsonNumber number("123");

        Json        mY;
        const Json& Y = mY;

        JsonNumber& mYNumber = mY.makeNumber(number);

        ASSERT(Y.isNumber());
        ASSERT(&mYNumber == &Y.theNumber());
        ASSERT(Y.theNumber() == number);

        Json        mZ;
        const Json& Z = mZ;

        JsonNumber& mZNumber =
                        mZ.makeNumber(bslmf::MovableRefUtil::move(number));

        ASSERT(Z.isNumber());
        ASSERT(&mZNumber == &Z.theNumber());
        ASSERT(Z.theNumber() == JsonNumber("123"));
    }

    // Make Object
    {
        Json        mX;
        const Json& X = mX;

        JsonObject& mXObject = mX.makeObject();

        ASSERT(X.isObject());
        ASSERT(&mXObject == &X.theObject());
        ASSERT(X.size() == 0);

        JsonObject object;
        object.insert(JsonObject::Member("a", Json()));

        Json        mY;
        const Json& Y = mY;

        JsonObject& mYObject = mY.makeObject(object);

        ASSERT(Y.isObject());
        ASSERT(&mYObject == &Y.theObject());
        ASSERT(Y.size() == 1);
        ASSERT(Y["a"].isNull());

        Json        mZ;
        const Json& Z = mZ;

        JsonObject& mZObject =
                        mZ.makeObject(bslmf::MovableRefUtil::move(object));

        ASSERT(Z.isObject());
        ASSERT(&mZObject == &Z.theObject());
        ASSERT(Z.size() == 1);
        ASSERT(Z["a"].isNull());
    }

    // Make String
    {
        Json        mX;
        const Json& X = mX;

        mX.makeString("string");

        ASSERT(X.isString());
        ASSERT(X.theString() == "string");

        bsl::string string("string");

        Json        mY;
        const Json& Y = mY;

        mY.makeString(string);

        ASSERT(Y.isString());
        ASSERT(Y.theString() == "string");

        Json        mZ;
        const Json& Z = mZ;

        mZ.makeString(bslmf::MovableRefUtil::move(string));

        ASSERT(Z.isString());
        ASSERT(Z.theString() == "string");
    }

    // (Member) Swap
    {
        Json        mX(false);
        const Json& X = mX;

        Json        mY("");
        const Json& Y = mY;

        mX.swap(mY);

        ASSERT(X.isString());
        ASSERT(Y.isBoolean());
    }

    // "The" Value Accessors
    {
        Json m1;
        m1.makeArray();
        ASSERT(m1.theArray().size() == 0);

        Json m2;
        m2.makeBoolean(true);
        ASSERT(m2.theBoolean() == true);

        Json m3;
        m3.makeNull();
        ASSERT(m3.theNull() == JsonNull());

        Json m4;
        m4.makeNumber(JsonNumber("1.5"));
        ASSERT(m4.theNumber() == JsonNumber("1.5"));

        Json m5;
        m5.makeObject();
        ASSERT(m5.theObject().size() == 0);
    }

#if defined(BSLS_COMPILERFEATURES_SUPPORT_OPERATOR_EXPLICIT)
    // Explicit Conversion Operators
    {
        Json m1;
        m1.makeArray();
        ASSERT(&static_cast<JsonArray&>(m1) == &m1.theArray());

        Json m2;
        m2.makeBoolean(true);
        ASSERT(&static_cast<bool&>(m2) == &m2.theBoolean());

        Json m3;
        m3.makeNull();
        ASSERT(&static_cast<JsonNull&>(m3) == &m3.theNull());

        Json m4;
        m4.makeNumber(JsonNumber("1.5"));
        ASSERT(&static_cast<JsonNumber&>(m4) == &m4.theNumber());

        Json m5;
        m5.makeObject();
        ASSERT(&static_cast<JsonObject&>(m5) == &m5.theObject());
    }
#endif

    // String Subscript Operator
    {
        Json mX;
        mX.makeObject();
        mX.theObject().insert(JsonObject::Member("a", Json()));
        mX.theObject().insert(JsonObject::Member("b", Json("")));

        ASSERT(mX["a"].isNull());
        ASSERT(mX["b"].isString());
    }

    // Size Subscript Operator
    {
        Json mX;
        mX.makeArray();
        mX.theArray().pushBack(Json());
        mX.theArray().pushBack(Json(""));

        ASSERT(mX[0].isNull());
        ASSERT(mX[1].isString());
    }

    // "Is" Value Tests
    {
        Json m1;
        m1.makeArray();
        ASSERT(m1.isArray());

        Json m2;
        m2.makeBoolean();
        ASSERT(m2.isBoolean());

        Json m3;
        m3.makeNull();
        ASSERT(m3.isNull());

        Json m4;
        m4.makeNumber();
        ASSERT(m4.isNumber());

        Json m5;
        m5.makeObject();
        ASSERT(m5.isObject());

        Json m6;
        m6.makeString("");
        ASSERT(m6.isString());
    }

    // (Const) "The" Value Accessors
    {
        Json        m1;
        const Json& M1 = m1;
        m1.makeArray();
        ASSERT(M1.theArray().size() == 0);

        Json        m2;
        const Json& M2 = m2;
        m2.makeBoolean(true);
        ASSERT(M2.theBoolean() == true);

        Json        m3;
        const Json& M3 = m3;
        m3.makeNull();
        ASSERT(M3.theNull() == JsonNull());

        Json        m4;
        const Json& M4 = m4;
        m4.makeNumber(JsonNumber("1.5"));
        ASSERT(M4.theNumber() == JsonNumber("1.5"));

        Json        m5;
        const Json& M5 = m5;
        m5.makeObject();
        ASSERT(M5.theObject().size() == 0);

        Json        m6;
        const Json& M6 = m6;
        m6.makeString("string");
        ASSERT(M6.theString() == "string");
    }

    // Type
    {
        Json        mX;
        const Json& X = mX;

        mX.makeArray();
        ASSERT(X.type() == JsonType::e_ARRAY);

        mX.makeBoolean();
        ASSERT(X.type() == JsonType::e_BOOLEAN);

        mX.makeNull();
        ASSERT(X.type() == JsonType::e_NULL);

        mX.makeNumber();
        ASSERT(X.type() == JsonType::e_NUMBER);

        mX.makeObject();
        ASSERT(X.type() == JsonType::e_OBJECT);

        mX.makeString("");
        ASSERT(X.type() == JsonType::e_STRING);
    }

    // "As" Arithmetic-Type Accessors
    {
        Json        mX;
        const Json& X = mX;

        mX.makeNumber(JsonNumber(1));

        int xInt;
        int rc = X.asInt(&xInt);
        ASSERT(rc == 0);
        ASSERT(xInt == 1);

        bsls::Types::Int64 xInt64;
        rc = X.asInt64(&xInt64);
        ASSERT(rc == 0);
        ASSERT(xInt64 == static_cast<bsls::Types::Int64>(1));

        unsigned int xUint;
        rc = X.asUint(&xUint);
        ASSERT(rc == 0);
        ASSERT(xUint == 1u);

        bsls::Types::Uint64 xUint64;
        rc = X.asUint64(&xUint64);
        ASSERT(rc == 0);
        ASSERT(xUint64 == static_cast<bsls::Types::Uint64>(1u));

        mX.makeNumber(JsonNumber(1.5));

        ASSERT(X.asFloat() == 1.5f);
        ASSERT(X.asDouble() == 1.5);
        ASSERT(X.asDecimal64() == bdldfp::Decimal64(1.5));

        bdldfp::Decimal64 xDecimal64;
        rc = X.asDecimal64Exact(&xDecimal64);
        ASSERT(rc == 0);
        ASSERT(xDecimal64 == bdldfp::Decimal64(1.5));
    }

    // (Const) String Subscript Operator
    {
        Json        mX;
        const Json& X = mX;
        mX.makeObject();
        mX.theObject().insert(JsonObject::Member("a", Json()));
        mX.theObject().insert(JsonObject::Member("b", Json("")));

        ASSERT(X["a"].isNull());
        ASSERT(X["b"].isString());
    }

    // (Const) Size Subscript Operator
    {
        Json        mX;
        const Json& X = mX;
        mX.makeArray();
        mX.theArray().pushBack(Json());
        mX.theArray().pushBack(Json(""));

        ASSERT(X[0].isNull());
        ASSERT(X[1].isString());
    }

#if defined(BSLS_COMPILERFEATURES_SUPPORT_OPERATOR_EXPLICIT)
    // (Const) Explicit Conversion Operators
    {
        Json        m1;
        const Json& M1 = m1;
        m1.makeArray();
        ASSERT(&static_cast<const JsonArray&>(M1) == &M1.theArray());

        Json        m2;
        const Json& M2 = m2;
        m2.makeBoolean(true);
        ASSERT(&static_cast<const bool&>(M2) == &M2.theBoolean());

        Json        m3;
        const Json& M3 = m3;
        m3.makeNull();
        ASSERT(&static_cast<const JsonNull&>(M3) == &M3.theNull());

        Json        m4;
        const Json& M4 = m4;
        m4.makeNumber(JsonNumber("1.5"));
        ASSERT(&static_cast<const JsonNumber&>(M4) == &M4.theNumber());

        Json        m5;
        const Json& M5 = m5;
        m5.makeObject();
        ASSERT(&static_cast<const JsonObject&>(M5) == &M5.theObject());

        Json        m6;
        const Json& M6 = m6;
        m6.makeString("");
        ASSERT(&static_cast<const bsl::string&>(M6) == &M6.theString());
    }
#endif

    // Size
    {
        Json        mX;
        const Json& X = mX;
        mX.makeObject();
        mX.theObject().insert(JsonObject::Member("a", Json()));
        mX.theObject().insert(JsonObject::Member("b", Json("")));

        ASSERT(X.size() == 2);

        Json        mY;
        const Json& Y = mY;
        mY.makeArray();
        mY.theArray().pushBack(Json());
        mY.theArray().pushBack(Json(""));

        ASSERT(Y.size() == 2);
    }

    // Allocator
    {
        Json        mX;
        const Json& X = mX;

        ASSERT(X.allocator() == bslma::Default::allocator());

        bslma::TestAllocator ta;

        Json        mY(&ta);
        const Json& Y = mY;

        ASSERT(Y.allocator() == &ta);
    }

    // Print
    {
        Json        mX;
        const Json& X = mX;
        mX.makeArray();
        mX.theArray().pushBack(Json(false));
        mX.theArray().pushBack(Json(JsonObject()));
        mX.theArray().pushBack(Json(JsonNumber("1.2345")));

        bsl::ostringstream stream;
        bsl::ostream&      streamRef = X.print(stream);
        ASSERT(&streamRef == &stream);

        const bsl::string&     result = stream.str();
        const bsl::string_view expectedResult =
            "[\n"
            "    false,\n"
            "    {\n"
            "    },\n"
            "    1.2345\n"
            "]";
        ASSERTV(result, result == expectedResult);
    }

    // Streaming Operator
    {
        Json        mX;
        const Json& X = mX;
        mX.makeArray();
        mX.theArray().pushBack(Json(false));
        mX.theArray().pushBack(Json(JsonObject()));
        mX.theArray().pushBack(Json(JsonNumber("1.2345")));

        bsl::ostringstream stream;
        bsl::ostream&      streamRef = stream << X;
        ASSERT(&streamRef == &stream);

        const bsl::string&     result = stream.str();
        const bsl::string_view expectedResult =
            "[false, {}, 1.2345]";
        ASSERTV(result, result == expectedResult);
    }

    // Equality and Inequality Operators
    {
        Json        mX;
        const Json& X = mX;
        mX.makeBoolean(false);

        Json        mY;
        const Json& Y = mY;
        mY.makeBoolean(false);

        Json        mZ;
        const Json& Z = mZ;
        mZ.makeArray();
        mZ.theArray().pushBack(Json());
        mZ.theArray().pushBack(Json(""));

        ASSERT(  X == Y );
        ASSERT(!(X == Z));
        ASSERT(!(Y == Z));

        ASSERT(!(X != Y));
        ASSERT(  X != Z );
        ASSERT(  Y != Z );
    }

    // HashAppend
    {
        // TBD: How to test?
    }

    // (Free) Swap
    {
        Json        mX(false);
        const Json& X = mX;

        Json        mY("");
        const Json& Y = mY;

        using namespace bsl;
        swap(mX, mY);

        ASSERT(X.isString());
        ASSERT(Y.isBoolean());
    }
}

// BDE_VERIFY pragma: +FD01 generator functions don't have contracts
// BDE_VERIFY pragma: +AR01 returning allocator aware by value

template <class SPECIALIZATIONS_ONLY>
class JsonValueConstructorHelper;

template <>
class JsonValueConstructorHelper<JsonNull>
{
  public:
    static JsonType::Enum getType()
        // Returns the 'JsonType::Enum' of the specified type
    {
        return JsonType::e_NULL;
    }

    static JsonNull getValue()
        // Returns a 'const' reference to a value of the specified type
    {
        return JsonNull();
    }
};


template <>
class JsonValueConstructorHelper<bool>
{
  public:
    static bool extractFromJson(const Json& json, int , char)
        // Returns the value of specified type from the specified 'json',
        // suitable for comparison to the value used in construction.
    {
        return json.theBoolean();
    }

    static JsonType::Enum getType()
        // Returns the 'JsonType::Enum' of the specified type
    {
        return JsonType::e_BOOLEAN;
    }


    static bool getValue()
        // Returns a 'const' reference to a value of the specified type
    {
        return true;
    }
};

template <>
class JsonValueConstructorHelper<float>
{
  public:
    static float extractFromJson(const Json& json, int , char)
        // Returns the value of specified type from the specified 'json',
        // suitable for comparison to the value used in construction.
    {
        return json.theNumber().asFloat();
    }

    static JsonType::Enum getType()
        // Returns the 'JsonType::Enum' of the specified type
    {
        return JsonType::e_NUMBER;
    }


    static float getValue()
        // Returns a 'const' reference to a value of the specified type
    {
        return -4.5e4f;
    }
};

template <>
class JsonValueConstructorHelper<double>
{
  public:
    static double extractFromJson(const Json& json, int , char)
        // Returns the value of specified type from the specified 'json',
        // suitable for comparison to the value used in construction.
    {
        return json.theNumber().asDouble();
    }

    static JsonType::Enum getType()
        // Returns the 'JsonType::Enum' of the specified type
    {
        return JsonType::e_NUMBER;
    }


    static double getValue()
        // Returns a 'const' reference to a value of the specified type
    {
        return 123.456;
    }
};

template <>
class JsonValueConstructorHelper<JsonArray>
{
  public:
    static const JsonArray& extractFromJson(const Json& json, int , char)
        // Returns the value of specified type from the specified 'json',
        // suitable for comparison to the value used in construction.
    {
        return json.theArray();
    }

    static JsonType::Enum getType()
        // Returns the 'JsonType::Enum' of the specified type
    {
        return JsonType::e_ARRAY;
    }


    static const JsonArray& getValue()
        // Returns a 'const' reference to a value of the specified type
    {
        static bslma::TestAllocator xa("scratch", false);
        static Json                 json(&xa);
        static const JsonArray      value(&json, &json+1, &xa);
        return value;
    }
};

template <>
class JsonValueConstructorHelper<bdldfp::Decimal64>
{
  public:
    static bdldfp::Decimal64 extractFromJson(const Json& json, int , char)
        // Returns the value of specified type from the specified 'json',
        // suitable for comparison to the value used in construction.
    {
        return json.theNumber().asDecimal64();
    }
    static JsonType::Enum getType()
        // Returns the 'JsonType::Enum' of the specified type
    {
        return JsonType::e_NUMBER;
    }

    static bdldfp::Decimal64 getValue()
        // Returns a 'const' reference to a value of the specified type
    {
        return BDLDFP_DECIMAL_DD(345.678);
    }
};

template <>
class JsonValueConstructorHelper<int>
{
  public:
    static int extractFromJson(const Json& json, int ALLOC_CONFIG, char CONFIG)
        // Returns the value of specified type from the specified 'json',
        // suitable for comparison to the value used in construction.  The
        // specified 'ALLOC_CONFIG' and the specified 'CONFIG' will be used to
        // report any failure to convert.
    {
        int value;
        ASSERTV(ALLOC_CONFIG, CONFIG, 0 == json.theNumber().asInt(&value));
        return value;
    }
    static JsonType::Enum getType()
        // Returns the 'JsonType::Enum' of the specified type
    {
        return JsonType::e_NUMBER;
    }

    static int getValue()
        // Returns a 'const' reference to a value of the specified type
    {
        return -54321;
    }
};

template <>
class JsonValueConstructorHelper<unsigned int>
{
  public:
    static unsigned int extractFromJson(const Json& json,
                                        int         ALLOC_CONFIG,
                                        char        CONFIG)
        // Returns the value of specified type from the specified 'json',
        // suitable for comparison to the value used in construction.  The
        // specified 'ALLOC_CONFIG' and the specified 'CONFIG' will be used to
        // report any failure to convert.
    {
        unsigned int value;
        ASSERTV(ALLOC_CONFIG, CONFIG, 0 == json.theNumber().asUint(&value));
        return value;
    }
    static JsonType::Enum getType()
        // Returns the 'JsonType::Enum' of the specified type
    {
        return JsonType::e_NUMBER;
    }

    static unsigned int getValue()
        // Returns a 'const' reference to a value of the specified type
    {
        return 98765;
    }
};

template <>
class JsonValueConstructorHelper<bsls::Types::Int64>
{
  public:
    static bsls::Types::Int64 extractFromJson(const Json& json,
                                              int         ALLOC_CONFIG,
                                              char        CONFIG)
        // Returns the value of specified type from the specified 'json',
        // suitable for comparison to the value used in construction.  The
        // specified 'ALLOC_CONFIG' and the specified 'CONFIG' will be used to
        // report any failure to convert.
    {
        bsls::Types::Int64 value;
        ASSERTV(ALLOC_CONFIG, CONFIG, 0 == json.theNumber().asInt64(&value));
        return value;
    }
    static JsonType::Enum getType()
        // Returns the 'JsonType::Enum' of the specified type
    {
        return JsonType::e_NUMBER;
    }

    static bsls::Types::Int64 getValue()
        // Returns a 'const' reference to a value of the specified type
    {
        return -0x123456789LL;
    }
};

template <>
class JsonValueConstructorHelper<bsls::Types::Uint64>
{
  public:
    static bsls::Types::Uint64 extractFromJson(const Json& json,
                                               int         ALLOC_CONFIG,
                                               char        CONFIG)
        // Returns the value of specified type from the specified 'json',
        // suitable for comparison to the value used in construction.  The
        // specified 'ALLOC_CONFIG' and the specified 'CONFIG' will be used to
        // report any failure to convert.
    {
        bsls::Types::Uint64 value;
        ASSERTV(ALLOC_CONFIG, CONFIG, 0 == json.theNumber().asUint64(&value));
        return value;
    }
    static JsonType::Enum getType()
        // Returns the 'JsonType::Enum' of the specified type
    {
        return JsonType::e_NUMBER;
    }

    static bsls::Types::Uint64 getValue()
        // Returns a 'const' reference to a value of the specified type
    {
        return 0x987654321LL;
    }
};

template <>
class JsonValueConstructorHelper<JsonNumber>
{
  public:
    static const JsonNumber& extractFromJson(const Json& json, int , char)
        // Returns the value of specified type from the specified 'json',
        // suitable for comparison to the value used in construction.
    {
        return json.theNumber();
    }

    static bsl::string_view getString()
        // Return a string used to construct the JsonNumber returned by
        // 'getValue()'.
    {
        return "100000000000000000000000000000000000001";
    }

    static JsonType::Enum getType()
        // Returns the 'JsonType::Enum' of the specified type
    {
        return JsonType::e_NUMBER;
    }

    static const JsonNumber& getValue()
        // Returns a 'const' reference to a value of the specified type
    {
        static bslma::TestAllocator xa("scratch", false);
        static const JsonNumber number(getString(), &xa);
        return number;
    }
};

template <>
class JsonValueConstructorHelper<JsonObject>
{
  public:
    static const JsonObject& extractFromJson(const Json& json, int , char)
        // Returns the value of specified type from the specified 'json',
        // suitable for comparison to the value used in construction.
    {
        return json.theObject();
    }
    static JsonType::Enum getType()
        // Returns the 'JsonType::Enum' of the specified type
    {
        return JsonType::e_OBJECT;
    }

    static const JsonObject& getValue()
        // Returns a 'const' reference to a value of the specified type
    {
        static bslma::TestAllocator xa("scratch", false);
        static JsonObject           object(&xa);

        BSLMT_ONCE_DO {
            object["null"] = JsonNull();
            object["number"] = 1;
        }

        return object;
    }
};

template <>
class JsonValueConstructorHelper<const char*>
{
  public:
    static bsl::string_view extractFromJson(const Json& json, int , char)
        // Returns the value of specified type from the specified 'json',
        // suitable for comparison to the value used in construction.
    {
        // Return a type that can be evaluated for equality with 'char*'.
        return json.theString();
    }
    static JsonType::Enum getType()
        // Returns the 'JsonType::Enum' of the specified type
    {
        return JsonType::e_STRING;
    }

    static const char* getValue()
        // Returns a 'const' reference to a value of the specified type
    {
        return "c-string";
    }
};

template <>
class JsonValueConstructorHelper<bsl::string_view>
{
  public:
    static bsl::string_view extractFromJson(const Json& json, int , char)
        // Returns the value of specified type from the specified 'json',
        // suitable for comparison to the value used in construction.
    {
        return json.theString();
    }
    static JsonType::Enum getType()
        // Returns the 'JsonType::Enum' of the specified type
    {
        return JsonType::e_STRING;
    }

    static bsl::string_view getValue()
        // Returns a 'const' reference to a value of the specified type
    {
        return "string_view";
    }
};

template <>
class JsonValueConstructorHelper<bsl::string>
{
  public:
    static const bsl::string& extractFromJson(const Json& json, int , char)
        // Returns the value of specified type from the specified 'json',
        // suitable for comparison to the value used in construction.
    {
        return json.theString();
    }
    static JsonType::Enum getType()
        // Returns the 'JsonType::Enum' of the specified type
    {
        return JsonType::e_STRING;
    }


    static const bsl::string& getValue()
        // Returns a 'const' reference to a value of the specified type
    {
        const static bsl::string string = "bsl::string";
        return string;
    }
};

class JsonValueEnumeration
{
  public:
    static char maxConfig()
        // Returns the maximum valid configuration.
    {
        return 'k';
    }

    static bool setValue(bdljsn::Json *json, char config)
        // Sets the value of the specified 'json' object to one of a set of
        // values, according to the specified 'config', and returns true if
        // memory allocation took place.  Behavior is undefined unless 'config'
        // is between 'a' and 'maxConfig()'.
    {
        bool allocated;
        switch (config)
        {
          case 'a': {
            json->makeNull();
            allocated = false;
          } break;
          case 'b': {
            json->makeBoolean(true);
            allocated = false;
          } break;
          case 'c': {
            json->makeBoolean(false);
            allocated = false;
          } break;
          case 'd': {
            json->makeString("string");
            allocated = false;
          } break;
          case 'e': {
            json->makeString(SUFFICIENTLY_LONG_STRING);
            allocated = true;
          } break;
          case 'f': {
            json->makeNumber() = 0;
            allocated = false;
          } break;
          case 'g': {
            json->makeNumber() = 1000;
            allocated = false;
          } break;
          case 'h': {
            json->makeArray();
            json->theArray().resize(1);
            json->theArray()[0].makeBoolean(true);
            allocated = true;
          } break;
          case 'i': {
            json->makeArray();
            json->theArray().resize(1);
            json->theArray()[0].makeString(SUFFICIENTLY_LONG_STRING);
            allocated = true;
          } break;
          case 'j': {
            json->makeObject();
            json->theObject()["key one"] = Json("value one");
            allocated = true;
          } break;
          case 'k': {
            json->makeObject();
            json->theObject()["key two"] = Json("value two");
            allocated = true;
          } break;
          default: {
            BSLS_ASSERT_OPT(!"Bad JSON value enumerator config");
            allocated = false;
          } break;
        }
        return allocated;
    }
};
// ============================================================================
//                               MAIN PROGRAM
// ----------------------------------------------------------------------------

using namespace BloombergLP;

typedef bsls::Types::Int64  Int64;
typedef bsls::Types::Uint64 Uint64;

int main(int argc, char *argv[])
{
    using namespace BloombergLP;

    const int test = argc > 1 ? atoi(argv[1]) : 0;

    BSLA_MAYBE_UNUSED const bool             verbose = argc > 2;
    BSLA_MAYBE_UNUSED const bool         veryVerbose = argc > 3;
    BSLA_MAYBE_UNUSED const bool     veryVeryVerbose = argc > 4;
    BSLA_MAYBE_UNUSED const bool veryVeryVeryVerbose = argc > 5;

    bsl::cout << "TEST " << __FILE__ << " CASE " << test << bsl::endl;

    // CONCERN: 'BSLS_REVIEW' failures should lead to test failures.
    bsls::ReviewFailureHandlerGuard reviewGuard(&bsls::Review::failByAbort);

    // CONCERN: In no case does memory come from the global allocator.

    bslma::TestAllocator globalAllocator("global", veryVeryVeryVerbose);
    bslma::Default::setGlobalAllocator(&globalAllocator);

    switch (test) { case 0:
      case 42: {
        // --------------------------------------------------------------------
        // DRQS 171793948
        //   Ensure that SFINAE of bdljsn::JsonObject::insert works correctly,
        //   especially in C++03.
        //
        // Concerns:
        //: 1 The bdljsn::JsonObject::insert overload taking iterators should
        //:   be selected for any kind of applicable iterator.
        //:
        //: 2 The bdljsn::JsonObject::insert overload taking key and value
        //:   types should be selected for any combination of types that are
        //:   convertible to 'bsl::string_view', and convertible to Json,
        //:   respectively.
        //:
        //: 3 Specifically, two parameters of type 'bsl::string' used to cause
        //:   a build error in C++03, because of a limitation of
        //:   'iterator_traits' in that version of the standard.
        //
        // Plan:
        //: 1 Invoke the overload using different types of iterators, including
        //:   pointers to array elements, and vector and list.
        //:
        //: 2 Invoke the overload using different types convertible to string,
        //:   including 'bsl::string', 'const char*', and 'bsl::string_view'.
        //
        // Testing:
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "DRQS 171793948" << endl
                          << "==============" << endl;

        typedef bsl::pair<bsl::string, bdljsn::Json> ModifiableKeyMember;
        {
            bdljsn::JsonObject               obj;
            bsl::vector<ModifiableKeyMember> members(4);

            members[0].first = "zero";
            members[1].first = "one";
            members[2].first = "two";
            members[3].first = "three";

            obj.insert(members.begin(), members.end());
            ASSERTV(obj.size(), 4 == obj.size());
        }
        {
            bdljsn::JsonObject  obj;
            ModifiableKeyMember members[4];

            members[0].first = "zero";
            members[1].first = "one";
            members[2].first = "two";
            members[3].first = "three";

            obj.insert(members,
                       members + sizeof(members) / sizeof(members[0]));
            ASSERTV(obj.size(), 4 == obj.size());
        }
        {
            bdljsn::JsonObject             obj;
            bsl::list<ModifiableKeyMember> members;

            members.push_back(ModifiableKeyMember("zero", Json()));
            members.push_back(ModifiableKeyMember("one", Json()));
            members.push_back(ModifiableKeyMember("two", Json()));
            members.push_back(ModifiableKeyMember("three", Json()));

            obj.insert(members.begin(), members.end());
            ASSERTV(obj.size(), 4 == obj.size());
        }

        const std::string      s0 = "s0";
              std::string      s1 = "s1";
        const bsl::string      s2 = "s2";
              bsl::string      s3 = "s3";
        const bsl::string_view s4 = "s4";
              bsl::string_view s5 = "s5";

        bdljsn::JsonObject obj;

        obj.insert(s0,s0); ASSERTV(obj.size(), 1== obj.size()); obj.clear();
        obj.insert(s0,s1); ASSERTV(obj.size(), 1== obj.size()); obj.clear();
        obj.insert(s0,s2); ASSERTV(obj.size(), 1== obj.size()); obj.clear();
        obj.insert(s0,s3); ASSERTV(obj.size(), 1== obj.size()); obj.clear();
        obj.insert(s0,s4); ASSERTV(obj.size(), 1== obj.size()); obj.clear();
        obj.insert(s0,s5); ASSERTV(obj.size(), 1== obj.size()); obj.clear();

        obj.insert(s1,s0); ASSERTV(obj.size(), 1== obj.size()); obj.clear();
        obj.insert(s1,s1); ASSERTV(obj.size(), 1== obj.size()); obj.clear();
        obj.insert(s1,s2); ASSERTV(obj.size(), 1== obj.size()); obj.clear();
        obj.insert(s1,s3); ASSERTV(obj.size(), 1== obj.size()); obj.clear();
        obj.insert(s1,s4); ASSERTV(obj.size(), 1== obj.size()); obj.clear();
        obj.insert(s1,s5); ASSERTV(obj.size(), 1== obj.size()); obj.clear();

        obj.insert(s2,s0); ASSERTV(obj.size(), 1== obj.size()); obj.clear();
        obj.insert(s2,s1); ASSERTV(obj.size(), 1== obj.size()); obj.clear();
        obj.insert(s2,s2); ASSERTV(obj.size(), 1== obj.size()); obj.clear();
        obj.insert(s2,s3); ASSERTV(obj.size(), 1== obj.size()); obj.clear();
        obj.insert(s2,s4); ASSERTV(obj.size(), 1== obj.size()); obj.clear();
        obj.insert(s2,s5); ASSERTV(obj.size(), 1== obj.size()); obj.clear();

        obj.insert(s3,s0); ASSERTV(obj.size(), 1== obj.size()); obj.clear();
        obj.insert(s3,s1); ASSERTV(obj.size(), 1== obj.size()); obj.clear();
        obj.insert(s3,s2); ASSERTV(obj.size(), 1== obj.size()); obj.clear();
        obj.insert(s3,s3); ASSERTV(obj.size(), 1== obj.size()); obj.clear();
        obj.insert(s3,s4); ASSERTV(obj.size(), 1== obj.size()); obj.clear();
        obj.insert(s3,s5); ASSERTV(obj.size(), 1== obj.size()); obj.clear();

        obj.insert(s4,s0); ASSERTV(obj.size(), 1== obj.size()); obj.clear();
        obj.insert(s4,s1); ASSERTV(obj.size(), 1== obj.size()); obj.clear();
        obj.insert(s4,s2); ASSERTV(obj.size(), 1== obj.size()); obj.clear();
        obj.insert(s4,s3); ASSERTV(obj.size(), 1== obj.size()); obj.clear();
        obj.insert(s4,s4); ASSERTV(obj.size(), 1== obj.size()); obj.clear();
        obj.insert(s4,s5); ASSERTV(obj.size(), 1== obj.size()); obj.clear();

        obj.insert(s5,s0); ASSERTV(obj.size(), 1== obj.size()); obj.clear();
        obj.insert(s5,s1); ASSERTV(obj.size(), 1== obj.size()); obj.clear();
        obj.insert(s5,s2); ASSERTV(obj.size(), 1== obj.size()); obj.clear();
        obj.insert(s5,s3); ASSERTV(obj.size(), 1== obj.size()); obj.clear();
        obj.insert(s5,s4); ASSERTV(obj.size(), 1== obj.size()); obj.clear();
        obj.insert(s5,s5); ASSERTV(obj.size(), 1== obj.size()); obj.clear();

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY
        const std::string_view s6 = "s6";
              std::string_view s7 = "s7";

        obj.insert(s0,s6); ASSERTV(obj.size(), 1== obj.size()); obj.clear();
        obj.insert(s0,s7); ASSERTV(obj.size(), 1== obj.size()); obj.clear();

        obj.insert(s1,s6); ASSERTV(obj.size(), 1== obj.size()); obj.clear();
        obj.insert(s1,s7); ASSERTV(obj.size(), 1== obj.size()); obj.clear();

        obj.insert(s2,s6); ASSERTV(obj.size(), 1== obj.size()); obj.clear();
        obj.insert(s2,s7); ASSERTV(obj.size(), 1== obj.size()); obj.clear();

        obj.insert(s3,s6); ASSERTV(obj.size(), 1== obj.size()); obj.clear();
        obj.insert(s3,s7); ASSERTV(obj.size(), 1== obj.size()); obj.clear();

        obj.insert(s4,s6); ASSERTV(obj.size(), 1== obj.size()); obj.clear();
        obj.insert(s4,s7); ASSERTV(obj.size(), 1== obj.size()); obj.clear();

        obj.insert(s5,s6); ASSERTV(obj.size(), 1== obj.size()); obj.clear();
        obj.insert(s5,s7); ASSERTV(obj.size(), 1== obj.size()); obj.clear();

        obj.insert(s6,s0); ASSERTV(obj.size(), 1== obj.size()); obj.clear();
        obj.insert(s6,s1); ASSERTV(obj.size(), 1== obj.size()); obj.clear();
        obj.insert(s6,s2); ASSERTV(obj.size(), 1== obj.size()); obj.clear();
        obj.insert(s6,s3); ASSERTV(obj.size(), 1== obj.size()); obj.clear();
        obj.insert(s6,s4); ASSERTV(obj.size(), 1== obj.size()); obj.clear();
        obj.insert(s6,s5); ASSERTV(obj.size(), 1== obj.size()); obj.clear();
        obj.insert(s6,s6); ASSERTV(obj.size(), 1== obj.size()); obj.clear();
        obj.insert(s6,s7); ASSERTV(obj.size(), 1== obj.size()); obj.clear();

        obj.insert(s7,s0); ASSERTV(obj.size(), 1== obj.size()); obj.clear();
        obj.insert(s7,s1); ASSERTV(obj.size(), 1== obj.size()); obj.clear();
        obj.insert(s7,s2); ASSERTV(obj.size(), 1== obj.size()); obj.clear();
        obj.insert(s7,s3); ASSERTV(obj.size(), 1== obj.size()); obj.clear();
        obj.insert(s7,s4); ASSERTV(obj.size(), 1== obj.size()); obj.clear();
        obj.insert(s7,s5); ASSERTV(obj.size(), 1== obj.size()); obj.clear();
        obj.insert(s7,s6); ASSERTV(obj.size(), 1== obj.size()); obj.clear();
        obj.insert(s7,s7); ASSERTV(obj.size(), 1== obj.size()); obj.clear();

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
        const std::pmr::string s8 = "s8";
              std::pmr::string s9 = "s9";

        obj.insert(s0,s8); ASSERTV(obj.size(), 1== obj.size()); obj.clear();
        obj.insert(s0,s9); ASSERTV(obj.size(), 1== obj.size()); obj.clear();

        obj.insert(s1,s8); ASSERTV(obj.size(), 1== obj.size()); obj.clear();
        obj.insert(s1,s9); ASSERTV(obj.size(), 1== obj.size()); obj.clear();

        obj.insert(s2,s8); ASSERTV(obj.size(), 1== obj.size()); obj.clear();
        obj.insert(s2,s9); ASSERTV(obj.size(), 1== obj.size()); obj.clear();

        obj.insert(s3,s8); ASSERTV(obj.size(), 1== obj.size()); obj.clear();
        obj.insert(s3,s9); ASSERTV(obj.size(), 1== obj.size()); obj.clear();

        obj.insert(s4,s8); ASSERTV(obj.size(), 1== obj.size()); obj.clear();
        obj.insert(s4,s9); ASSERTV(obj.size(), 1== obj.size()); obj.clear();

        obj.insert(s5,s8); ASSERTV(obj.size(), 1== obj.size()); obj.clear();
        obj.insert(s5,s9); ASSERTV(obj.size(), 1== obj.size()); obj.clear();

        obj.insert(s6,s8); ASSERTV(obj.size(), 1== obj.size()); obj.clear();
        obj.insert(s6,s9); ASSERTV(obj.size(), 1== obj.size()); obj.clear();

        obj.insert(s7,s8); ASSERTV(obj.size(), 1== obj.size()); obj.clear();
        obj.insert(s7,s9); ASSERTV(obj.size(), 1== obj.size()); obj.clear();

        obj.insert(s8,s0); ASSERTV(obj.size(), 1== obj.size()); obj.clear();
        obj.insert(s8,s1); ASSERTV(obj.size(), 1== obj.size()); obj.clear();
        obj.insert(s8,s2); ASSERTV(obj.size(), 1== obj.size()); obj.clear();
        obj.insert(s8,s3); ASSERTV(obj.size(), 1== obj.size()); obj.clear();
        obj.insert(s8,s4); ASSERTV(obj.size(), 1== obj.size()); obj.clear();
        obj.insert(s8,s5); ASSERTV(obj.size(), 1== obj.size()); obj.clear();
        obj.insert(s8,s6); ASSERTV(obj.size(), 1== obj.size()); obj.clear();
        obj.insert(s8,s7); ASSERTV(obj.size(), 1== obj.size()); obj.clear();
        obj.insert(s8,s8); ASSERTV(obj.size(), 1== obj.size()); obj.clear();
        obj.insert(s8,s9); ASSERTV(obj.size(), 1== obj.size()); obj.clear();

        obj.insert(s9,s0); ASSERTV(obj.size(), 1== obj.size()); obj.clear();
        obj.insert(s9,s1); ASSERTV(obj.size(), 1== obj.size()); obj.clear();
        obj.insert(s9,s2); ASSERTV(obj.size(), 1== obj.size()); obj.clear();
        obj.insert(s9,s3); ASSERTV(obj.size(), 1== obj.size()); obj.clear();
        obj.insert(s9,s4); ASSERTV(obj.size(), 1== obj.size()); obj.clear();
        obj.insert(s9,s5); ASSERTV(obj.size(), 1== obj.size()); obj.clear();
        obj.insert(s9,s6); ASSERTV(obj.size(), 1== obj.size()); obj.clear();
        obj.insert(s9,s7); ASSERTV(obj.size(), 1== obj.size()); obj.clear();
        obj.insert(s9,s8); ASSERTV(obj.size(), 1== obj.size()); obj.clear();
        obj.insert(s9,s9); ASSERTV(obj.size(), 1== obj.size()); obj.clear();

#endif  // BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
#endif  // BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY
      } break;
      case 41: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //   Extracted from component header file.
        //
        // Concerns:
        //: 1 The usage example provided in the component header file compiles,
        //:   links, and runs as shown.
        //:
        //: 2 The usage example does what it's described to do.
        //
        // Plan:
        //: 1 Incorporate usage example from header into test driver, remove
        //:   leading comment characters, and replace 'assert' with 'ASSERT'.
        //:   (C-1)
        //:
        //: 2 Run some basic test data. (C-2)
        //
        // Testing:
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "USAGE EXAMPLE" << endl
                          << "=============" << endl;

#if defined(BSLS_COMPILERFEATURES_SUPPORT_USER_DEFINED_LITERALS) &&     \
    defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
        bdljsn::Json example1, example2;
        {

///Usage
///-----
// This section illustrates the intended use of this component.
//
///Example 1: Constructor a Basic 'bdljsn::Json' Object
/// - - - - - - - - - - - - - - - - - - - - - - - - - -
// Most often 'bdljsn::Json' objects will be written and read from
// JSON text using 'bdljsn_jsonutil'.  In this simple example, we demonstrate
// manually creating the document below and then verify the properties of the
// resulting object:
//..
//  {
//      "number": 3.14,
//      "boolean": true,
//      "string": "text",
//      "null": null,
//      "array": [ "2.76", true ],
//      "object": { "boolean": false }
//  }
//..
// First, we use 'bdljsn::Json::makeObject' to configure the top level
// 'bdljsn::Json' object to be a 'bdljsn::Json' object, and use the various
// manipulators of 'bdljsn::JsonObject' to configure its value:
//..
    using namespace bdldfp::DecimalLiterals;

    bdljsn::Json json;

    json.makeObject();
    json["number"]  = 3.14;
    json["boolean"] = true;
    json["string"]  = "text";
    json["array"].makeArray();
    json["array"].theArray().pushBack(bdljsn::Json(2.76_d64));
    json["array"].theArray().pushBack(bdljsn::Json(true));
    json["object"].makeObject()["boolean"] = false;
//..
// Notice that we used 'operator[]' to implicitly create new members of the
// top-level object.  Using 'json.theObject().insert' would be more efficient
// (see example 2).
//
// Finally, we validate the properties of the resulting object:
//..
    ASSERT(3.14     == json["number"].asDouble());
    ASSERT(true     == json["boolean"].theBoolean());
    ASSERT("text"   == json["string"].theString());
    ASSERT(true     == json["null"].isNull());
    ASSERT(2.76_d64 == json["array"][0].asDecimal64());
    ASSERT(false    == json["object"]["boolean"].theBoolean());
//..
            if (verbose) {
                bsl::cout << json << bsl::endl;
            }
            example1 = json;
        }
        {
///Example 2: More Efficiently Creating a 'bdljsn::Json'
///-----------------------------------------------------
// Example 1 used 'operator[]' to implicitly add members to the Objects.  Using
// 'operator[]' is intuitive but not the most efficient method to add new
// members to a 'bdljsn::JsonObject' (similar to using 'operator[]' to add
// elements to an 'unordered_map').  The following code demonstrates a more
// efficient way to create the same 'bdljsn::Json' representation as example 1:
//..
    using namespace bdldfp::DecimalLiterals;

    bdljsn::Json       json;
    bdljsn::JsonArray  subArray;
    bdljsn::JsonObject subObject;

    json.makeObject();
    json.theObject().insert("number", bdljsn::JsonNumber(3.14));
    json.theObject().insert("boolean", true);
    json.theObject().insert("string", "text");
    json.theObject().insert("null", bdljsn::JsonNull());

    subArray.pushBack(bdljsn::Json(2.76_d64));
    subArray.pushBack(bdljsn::Json(true));
    json.theObject().insert("array", bsl::move(subArray));

    subObject.insert("boolean", false);
    json.theObject().insert("object", bsl::move(subObject));
//..
            if (verbose) {
                bsl::cout << json << bsl::endl;
            }
            example2 = json;
        }
        ASSERTV(example1, example2, example1 == example2);
#endif
      } break;
      case 40: {
        // --------------------------------------------------------------------
        // JSON ACCESSORS
        //
        // Concerns:
        //: 1 Each accessor returns the value of the corresponding attribute of
        //:   the object.
        //:
        //: 2 Each accessor method is declared 'const'.
        //:
        //: 3 No accessor allocates memory.
        //:
        //: 4 Accessors for attributes that can allocate memory (i.e., those
        //:   that take an allocator in their constructor) return a 'const'
        //:   reference.
        //
        // Plan:
        //: 1 Create a 'bslma::TestAllocator' object, and install it as the
        //:   current default allocator.
        //:
        //: 2 Use the default constructor to create an object.
        //:
        //: 3 Verify that each of the "isType()" accessors, invoked on a
        //:   'const' reference to 'Json', returns 'false' before setting the
        //:   object to that type, and returns 'true' after.
        //:
        //: 4 Create a table of interesting 'JsonNumber' values, including
        //:   values one greater than or less than maximum and minimum
        //:   (respectively) representable values for various types.
        //:
        //: 5 For each 'JsonNumber' value in the table:
        //:
        //:   1 Construct a 'Json' object from the 'JsonNumber' value.
        //:
        //:   2 Invoke each "asType()" numeric accessor on both the 'Json'
        //:     value and the 'JsonNumber' value, and verify that the results
        //:     are identical.
        //:
        //: 6 For each non-numerical accessor, construct a 'const Json' object
        //:   having the type corresponding to the accessor, and invoke the
        //:   accessor on both the 'Json' object and the underlying object, and
        //:   verify that the result is identical.
        //:
        //: 7 Verify that at no point was any memory allocated from the default
        //:   allocator.
        //
        // Testing:
        //   bool Json::isArray() const;
        //   bool Json::isBoolean() const;
        //   bool Json::isNull() const;
        //   bool Json::isNumber() const;
        //   bool Json::isObject() const;
        //   bool Json::isString() const;
        //   int Json::asInt(int *r) const;
        //   int Json::asInt64(Int64 *r) const;
        //   int Json::asUint(unsigned int *r) const;
        //   int Json::asUint64(Uint64 *r) const;
        //   float Json::asFloat() const;
        //   double Json::asDouble() const;
        //   Decimal64 Json::asDecimal64() const;
        //   int Json::asDecimal64Exact(Decimal64 *r) const;
        //   const Json& Json::operator[](const string_view& key) const;
        //   const Json& Json::operator[](size_t index) const;
        //   size_t Json::size() const;
        //   Json::operator const BloombergLP::bdljsn::JsonArray &() const;
        //   Json::operator const bool &() const;
        //   Json::operator const BloombergLP::bdljsn::JsonNull &() const;
        //   Json::operator const BloombergLP::bdljsn::JsonNumber &() const;
        //   Json::operator const BloombergLP::bdljsn::JsonObject &() const;
        //   Json::operator const bsl::basic_string &() const;
        //
        //   bslma::Allocator* Json::allocator() const;
        // --------------------------------------------------------------------

        typedef bdljsn::Json Obj;

        if (verbose) cout << endl
            << "JSON ACCESSORS" << endl
            << "==============" << endl;

        bslma::TestAllocator         da("default", veryVeryVeryVerbose);
        bslma::DefaultAllocatorGuard dag(&da);

        {
            Obj mX;  const Obj& X = mX;
            if (veryVerbose) cout << "bool Json::isArray() const;\n";
            {
                ASSERT(!X.isArray());
                mX.makeArray();
                ASSERT(X.isArray());
            }
            if (veryVerbose) cout << "bool Json::isBoolean() const;\n";
            {
                ASSERT(!X.isBoolean());
                mX.makeBoolean();
                ASSERT(X.isBoolean());
            }
            if (veryVerbose) cout << "bool Json::isNull() const;\n";
            {
                ASSERT(!X.isNull());
                mX.makeNull();
                ASSERT(X.isNull());
            }
            if (veryVerbose) cout << "bool Json::isNumber() const;\n";
            {
                ASSERT(!X.isNumber());
                mX.makeNumber();
                ASSERT(X.isNumber());
            }
            if (veryVerbose) cout << "bool Json::isObject() const;\n";
            {
                ASSERT(!X.isObject());
                mX.makeObject();
                ASSERT(X.isObject());
            }
            if (veryVerbose) cout << "bool Json::isString() const;\n";
            {
                ASSERT(!X.isString());
                mX.makeString("");
                ASSERT(X.isString());
            }
        }
        {
            bslma::TestAllocator xa("scratch", veryVeryVeryVerbose);

            const JsonNumber NUMBERS[] = {
                JsonNumber(1, &xa),
                JsonNumber(0, &xa),
                JsonNumber(-1, &xa),
                JsonNumber(bsl::numeric_limits<int>::max(), &xa),
                JsonNumber(bsl::numeric_limits<unsigned int>::max(), &xa),
                JsonNumber(bsl::numeric_limits<bsls::Types::Int64>::max(),
                           &xa),
                JsonNumber(bsl::numeric_limits<bsls::Types::Uint64>::max(),
                           &xa),
                JsonNumber(bsl::numeric_limits<int>::min(), &xa),
                JsonNumber(bsl::numeric_limits<bsls::Types::Int64>::min(),
                           &xa),
                JsonNumber(bsl::numeric_limits<int>::max() + 1ll, &xa),
                JsonNumber(bsl::numeric_limits<unsigned int>::max() + 1ll,
                           &xa),
                JsonNumber(bsl::numeric_limits<bsls::Types::Int64>::max()
                           + 1ull,
                           &xa),
                JsonNumber(bsl::numeric_limits<int>::min() - 1ll, &xa),
                JsonNumber(
                    static_cast<double>(
                    bsl::numeric_limits<bsls::Types::Int64>::max()) * 1.00001,
                    &xa),
                JsonNumber(
                    static_cast<double>(
                    bsl::numeric_limits<bsls::Types::Int64>::min()) * 1.00001,
                    &xa),
                JsonNumber(
                   static_cast<double>(
                   bsl::numeric_limits<bsls::Types::Uint64>::max()) * 1.00001,
                   &xa),
                JsonNumber(bsl::numeric_limits<double>::max(), &xa),
                JsonNumber(bsl::numeric_limits<double>::min(), &xa),
                JsonNumber(1e100 * 1e100, &xa),
                JsonNumber(-1e100 * 1e100, &xa)
            };

            for (long unsigned int i = 0;
                 i < (sizeof(NUMBERS)/sizeof(*NUMBERS));
                 ++i) {
                const JsonNumber& N = NUMBERS[i];
                const Obj         X(N, &xa);

                if (veryVerbose) cout << "Value: " << N << endl;

                int jsRC;
                int nRC;

                if (veryVerbose) cout << "int Json::asInt(int *r) const;\n";
                {
                    int jsValue;
                    int nValue;
                    jsRC = X.asInt(&jsValue);
                    nRC = N.asInt(&nValue);
                    ASSERTV(N, X, jsRC, nRC, jsRC == nRC);
                    ASSERTV(N, X, jsValue, nValue, jsValue == nValue);
                }
                if (veryVerbose)
                    cout << "int Json::asInt64(Int64 *r) const;\n";
                {
                    bsls::Types::Int64 jsValue;
                    bsls::Types::Int64 nValue;
                    jsRC = X.asInt64(&jsValue);
                    nRC = N.asInt64(&nValue);
                    ASSERTV(N, X, jsRC, nRC, jsRC == nRC);
                    ASSERTV(N, X, jsValue, nValue, jsValue == nValue);
                }
                if (veryVerbose)
                    cout << "int Json::asUint(unsigned int *r) const;\n";
                {
                    unsigned int jsValue;
                    unsigned int nValue;
                    jsRC = X.asUint(&jsValue);
                    nRC = N.asUint(&nValue);
                    ASSERTV(N, X, jsRC, nRC, jsRC == nRC);
                    ASSERTV(N, X, jsValue, nValue, jsValue == nValue);
                }
                if (veryVerbose)
                    cout << "int Json::asUint64(Uint64 *r) const;\n";
                {
                    bsls::Types::Uint64 jsValue;
                    bsls::Types::Uint64 nValue;
                    jsRC = X.asUint64(&jsValue);
                    nRC = N.asUint64(&nValue);
                    ASSERTV(N, X, jsRC, nRC, jsRC == nRC);
                    ASSERTV(N, X, jsValue, nValue, jsValue == nValue);
                }
                if (veryVerbose)
                    cout << "int Json::asDecimal64Exact(Decimal64 *r) "
                            "const;\n";
                {
                    bdldfp::Decimal64 jsValue;
                    bdldfp::Decimal64 nValue;
                    jsRC = X.asDecimal64Exact(&jsValue);
                    nRC = N.asDecimal64Exact(&nValue);
                    ASSERTV(N, X, jsRC, nRC, jsRC == nRC);
                    ASSERTV(N, X, jsValue, nValue, jsValue == nValue);
                }
                if (veryVerbose) cout << "float Json::asFloat() const;\n";
                {
                    float jsValue;
                    float nValue;
                    jsValue = X.asFloat();
                    nValue = N.asFloat();
                    ASSERTV(N, X, jsValue, nValue, jsValue == nValue);
                }
                if (veryVerbose) cout << "double Json::asDouble() const;\n";
                {
                    double jsValue;
                    double nValue;
                    jsValue = X.asDouble();
                    nValue = N.asDouble();
                    ASSERTV(N, X, jsValue, nValue, jsValue == nValue);
                }
                if (veryVerbose)
                    cout << "Decimal64 Json::asDecimal64() const;\n";
                {
                    bdldfp::Decimal64 jsValue;
                    bdldfp::Decimal64 nValue;
                    jsValue = X.asDecimal64();
                    nValue = N.asDecimal64();
                    ASSERTV(N, X, jsValue, nValue, jsValue == nValue);
                }
            }
        }
        if (veryVerbose)
            cout << "const Json& Json::operator[](const "
                    "string_view& key) const;\n";
        {
            bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);
            bsl::string_view     key = "key";

            Obj mX(&sa);  const Obj& X = mX;
            mX.makeObject().insert(key, 1);

            bslma::TestAllocatorMonitor sam(&sa);
            const Json&                 result   = mX[key];
            const Json&                 expected = X.theObject()[key];

            ASSERTV(&result == &expected);
            ASSERTV(sam.isTotalSame());
        }
        if (veryVerbose)
            cout << "const Json& Json::operator[](size_t index) const;\n";
        {
            bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);

            Obj mX(&sa);  const Obj& X = mX;
            mX.makeArray().pushBack(Json());

            bslma::TestAllocatorMonitor sam(&sa);
            Json&                       result   = mX[0];
            const Json&                 expected = X.theArray().front();

            ASSERTV(&result == &expected);
            ASSERTV(sam.isTotalSame());
        }
        if (veryVerbose) cout << "size_t Json::size() const;\n";
        {
            bsl::string_view     key = "key";
            bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);

            Obj mX(&sa);  const Obj& X = mX;
            mX.makeObject().insert(key, 1);
            {
                bslma::TestAllocatorMonitor sam(&sa);
                const bsl::size_t           s = X.size();
                ASSERTV(s, 1 == s);
                ASSERTV(sam.isTotalSame());
            }

            mX.makeArray().pushBack(Json());
            {
                bslma::TestAllocatorMonitor sam(&sa);
                const bsl::size_t           s = X.size();
                ASSERTV(s, 1 == s);
                ASSERTV(sam.isTotalSame());
            }
        }
#if defined(BSLS_COMPILERFEATURES_SUPPORT_OPERATOR_EXPLICIT)
        if (veryVerbose)
            cout << "Json::operator const "
                    "BloombergLP::bdljsn::JsonArray &() const;\n";
        {
            typedef JsonArray Type;
            const Obj   X((Type()));
            const Type& ref = static_cast<const Type&>(X);
            ASSERT(&ref == &(X.theArray()));
        }
        if (veryVerbose) cout << "Json::operator const bool &() const;\n";
        {
            typedef bool Type;
            const Obj   X((Type()));
            const Type& ref = static_cast<const Type&>(X);
            ASSERT(&ref == &(X.theBoolean()));
        }
        if (veryVerbose)
            cout << "Json::operator const "
                    "BloombergLP::bdljsn::JsonNull &() const;\n";
        {
            typedef JsonNull Type;
            const Obj   X((Type()));
            const Type& ref = static_cast<const Type&>(X);
            ASSERT(&ref == &(X.theNull()));
        }
        if (veryVerbose)
            cout << "Json::operator const "
                    "BloombergLP::bdljsn::JsonNumber &() const;\n";
        {
            typedef JsonNumber Type;
            const Obj   X((Type()));
            const Type& ref = static_cast<const Type&>(X);
            ASSERT(&ref == &(X.theNumber()));
        }
        if (veryVerbose)
            cout << "Json::operator const "
                    "BloombergLP::bdljsn::JsonObject &() const;\n";
        {
            typedef JsonObject Type;
            const Obj   X((Type()));
            const Type& ref = static_cast<const Type&>(X);
            ASSERT(&ref == &(X.theObject()));
        }
        if (veryVerbose)
            cout << "Json::operator const bsl::basic_string &() const;\n";
        {
            typedef bsl::string Type;
            const Obj   X((Type()));
            const Type& ref = static_cast<const Type&>(X);
            ASSERT(&ref == &(X.theString()));
        }
#else
        if (verbose) cout << "Explicit operators unsupported.\n";
#endif
        if (veryVerbose)
            cout << "bslma::Allocator* Json::allocator() const;\n";
        {
            bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);
            const Obj            X(&sa);
            ASSERT(&sa == X.allocator());
        }

        ASSERTV(da.numBlocksTotal(), 0 == da.numBlocksTotal());
      } break;
      case 39: {
        // --------------------------------------------------------------------
        // JSON MANIPULATORS
        //
        // Concerns:
        //: 1 Manipulators modify the value in the expected way.
        //:
        //: 2 Manipulators that return a reference providing modifiable access
        //:   to 'Json' return a reference to the same object on which they are
        //:   invoked.
        //:
        //: 3 Manipulators set the variant type to the expected type when
        //:   accessed through 'type()'.
        //:
        //: 4 In no case is temporary memory allocated.
        //
        // Plan:
        //: 1 Create a 'bslma::TestAllocator' and install it as the default
        //:   allocator.
        //:
        //: 2 For manipulators that modify the value directly without moving:
        //:
        //:   1 Create two 'bslma::TestAllocator' object, naming one "scratch"
        //:     and one "supplied".
        //:
        //:   2 Create a default constructed object with the supplied allocator
        //:     installed.
        //:
        //:   3 Create a 'const' variable of the type of the manipulator's
        //:     parameter, with a conspicuous value different from its default
        //:     (if any), using the scratch allocator if applicable.
        //:
        //:   4 Create a 'const' variable of the variant-type that the
        //:     manipulator is expected to select, constructed from the value
        //:     from 2.3, using the scratch allocator.
        //:
        //:   5 If the object from 2.3 is expected to allocate, verify that it
        //:     did allocate some memory.
        //:
        //:   6 Invoke the manipulator, passing the value from 2.3, and
        //:     capturing the result in a reference.
        //:
        //:   7 If the returned reference is of type 'Json', verify that the
        //:     referenced address is the same as the object from 2.2.  If the
        //:     type is a variant type, ensure the address is the same as that
        //:     returned by the "theType()" accessor.
        //:
        //:   8 Verify that the type of the object from 2.2 (as determined by
        //:     accessor 'type()') is as expected.
        //:
        //:   9 Verify that the value of the selected variant type of the
        //:     object from 2.2 (as accessed by the "theType()" accessor) is
        //:     equal to the value of the object from 2.4.
        //:
        //:   10If the object in 2.3 allocated memory, verify that the supplied
        //:     allocator also allocated memory, and that it did not allocate
        //:     any temporary memory.  Otherwise, verify that the supplied
        //:     allocator did not allocate any memory.
        //:
        //:   11Verify that the default allocator did not allocate.
        //:
        //: 3 For manipulators that modify the value directly without moving:
        //:
        //:   1 Create two 'bslma::TestAllocator' object, naming one "scratch"
        //:     and one "supplied".
        //:
        //:   2 Create a default constructed object with the supplied allocator
        //:     installed.
        //:
        //:   3 Create a 'const' variable of the type of the manipulator's
        //:     parameter, with a conspicuous value different from its default
        //:     (if any), using the supplied allocator.
        //:
        //:   4 Create a 'const' variable of the variant-type that the
        //:     manipulator is expected to select, constructed from the value
        //:     from 3.3, using the scratch allocator.
        //:
        //:   5 Verify that the supplied allocator did allocate some memory.
        //:
        //:   6 Install a 'bslma::TestAllocatorMonitor' on the supplied
        //:     allocator.
        //:
        //:   7 Invoke the manipulator, moving the value from 3.3, and
        //:     capturing the result in a reference.
        //:
        //:   8 If the returned reference is of type 'Json', verify that the
        //:     referenced address is the same as the object from 3.2.  If the
        //:     type is a variant type, ensure the address is the same as that
        //:     returned by the "theType()" accessor.
        //:
        //:   9 Verify that the type of the object from 3.2 (as determined by
        //:     accessor 'type()') is as expected.
        //:
        //:   10Verify that the selected variant type of the object from 3.2
        //:     (as accessed by the "theType()" accessor) is the type of the
        //:     object from 3.4.
        //:
        //:   11Verify that the supplied allocator did not allocate or
        //:     deallocate since the monitor was installed in 3.6.
        //:
        //:   12Verify that the default allocator did not allocate.
        //
        // Testing:
        //   Json& Json::operator=(float rhs);
        //   Json& Json::operator=(double rhs);
        //   Json& Json::operator=(bdldfp::Decimal64 rhs);
        //   Json& Json::operator=(int rhs);
        //   Json& Json::operator=(unsigned int rhs);
        //   Json& Json::operator=(bsls::Types::Int64 rhs);
        //   Json& Json::operator=(bsls::Types::Uint64 rhs);
        //   Json& Json::operator=(const JsonNumber& rhs);
        //   Json& Json::operator=(bslmf::MovableRef<JsonNumber> rhs);
        //   Json& Json::operator=(const char *rhs);
        //   Json& Json::operator=(const bsl::string_view& rhs);
        //   Json& Json::operator=(STRING&& rhs);
        //   Json& Json::operator=(bool rhs);
        //   Json& Json::operator=(const JsonObject& rhs);
        //   Json& Json::operator=(bslmf::MovableRef<JsonObject> rhs);
        //   Json& Json::operator=(const JsonArray& rhs);
        //   Json& Json::operator=(bslmf::MovableRef<JsonArray> rhs);
        //   Json& Json::operator=(const JsonNull& rhs);
        //   Json& Json::operator=(bslmf::MovableRef<JsonNull> rhs);
        //   JsonArray& Json::makeArray(const JsonArray& array);
        //   JsonArray& Json::makeArray(MovableRef<JsonArray> array);
        //   bool& Json::makeBoolean(bool boolean);
        //   JsonNumber& Json::makeNumber(const JsonNumber& number);
        //   JsonNumber& Json::makeNumber(MovableRef<JsonNumber> number);
        //   JsonObject& Json::makeObject(const JsonObject& object);
        //   JsonObject& Json::makeObject(MovableRef<JsonObject> object);
        //   void Json::makeString(const bsl::string_view& string);
        //   void Json::makeString(STRING&& string);
        //   JsonArray& Json::theArray();
        //   bool& Json::theBoolean();
        //   JsonNull& Json::theNull();
        //   JsonNumber& Json::theNumber();
        //   JsonObject& Json::theObject();
        //   Json& Json::operator[](const string_view& key);
        //   Json& Json::operator[](size_t index);
        //   Json::operator BloombergLP::bdljsn::JsonArray &();
        //   Json::operator bool &();
        //   Json::operator BloombergLP::bdljsn::JsonNull &();
        //   Json::operator BloombergLP::bdljsn::JsonNumber &();
        //   Json::operator BloombergLP::bdljsn::JsonObject &();
        //   Json::operator bsl::string &();
        // --------------------------------------------------------------------

        typedef bdljsn::Json Obj;

        if (verbose) cout << endl
            << "JSON MANIPULATORS" << endl
            << "=================" << endl;

        bslma::TestAllocator         da("default", veryVeryVeryVerbose);
        bslma::DefaultAllocatorGuard dag(&da);

        //   Json& Json::operator=(float rhs);
        {
            bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);
            bslma::TestAllocator xa("scratch",  veryVeryVeryVerbose);

            Obj mX(&sa);  const Obj& X = mX;

            const float      v = 1;
            const JsonNumber expected(v, &xa);

            Json& result = (mX = v);

            ASSERTV(&X, &result, &X == &result);
            ASSERTV(X.type(), JsonType::e_NUMBER == X.type());
            ASSERTV(X.theNumber(), expected == X.theNumber());
            ASSERTV(sa.numBytesTotal(), 0 == sa.numBytesTotal());
            ASSERTV(da.numBytesTotal(), 0 == da.numBytesTotal());
        }
        //   Json& Json::operator=(double rhs);
        {
            bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);
            bslma::TestAllocator xa("scratch",  veryVeryVeryVerbose);

            Obj mX(&sa);  const Obj& X = mX;

            const double     v = 1;
            const JsonNumber expected(v, &xa);

            Json& result = (mX = v);

            ASSERTV(&X, &result, &X == &result);
            ASSERTV(X.type(), JsonType::e_NUMBER == X.type());
            ASSERTV(X.theNumber(), expected == X.theNumber());
            ASSERTV(sa.numBytesTotal(), 0 == sa.numBytesTotal());
            ASSERTV(da.numBytesTotal(), 0 == da.numBytesTotal());
        }
        //   Json& Json::operator=(bdldfp::Decimal64 rhs);
        {
            bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);
            bslma::TestAllocator xa("scratch",  veryVeryVeryVerbose);

            Obj mX(&sa);  const Obj& X = mX;

            const bdldfp::Decimal64 v(1);
            const JsonNumber        expected(v, &xa);

            Json& result = (mX = v);

            ASSERTV(&X, &result, &X == &result);
            ASSERTV(X.type(), JsonType::e_NUMBER == X.type());
            ASSERTV(X.theNumber(), expected == X.theNumber());
            ASSERTV(sa.numBytesTotal(), 0 == sa.numBytesTotal());
            ASSERTV(da.numBytesTotal(), 0 == da.numBytesTotal());
        }
        //   Json& Json::operator=(int rhs);
        {
            bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);
            bslma::TestAllocator xa("scratch",  veryVeryVeryVerbose);

            Obj mX(&sa);  const Obj& X = mX;

            const int        v = 1;
            const JsonNumber expected(v, &xa);

            Json& result = (mX = v);

            ASSERTV(&X, &result, &X == &result);
            ASSERTV(X.type(), JsonType::e_NUMBER == X.type());
            ASSERTV(X.theNumber(), expected == X.theNumber());
            ASSERTV(sa.numBytesTotal(), 0 == sa.numBytesTotal());
            ASSERTV(da.numBytesTotal(), 0 == da.numBytesTotal());
        }
        //   Json& Json::operator=(unsigned int rhs);
        {
            bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);
            bslma::TestAllocator xa("scratch",  veryVeryVeryVerbose);

            Obj mX(&sa);  const Obj& X = mX;

            const unsigned int v = 1;
            const JsonNumber   expected(v, &xa);

            Json& result = (mX = v);

            ASSERTV(&X, &result, &X == &result);
            ASSERTV(X.type(), JsonType::e_NUMBER == X.type());
            ASSERTV(X.theNumber(), expected == X.theNumber());
            ASSERTV(sa.numBytesTotal(), 0 == sa.numBytesTotal());
            ASSERTV(da.numBytesTotal(), 0 == da.numBytesTotal());
        }
        //   Json& Json::operator=(bsls::Types::Int64 rhs);
        {
            bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);
            bslma::TestAllocator xa("scratch",  veryVeryVeryVerbose);

            Obj mX(&sa);  const Obj& X = mX;

            const bsls::Types::Int64 v = 1;
            const JsonNumber         expected(v, &xa);

            Json& result = (mX = v);

            ASSERTV(&X, &result, &X == &result);
            ASSERTV(X.type(), JsonType::e_NUMBER == X.type());
            ASSERTV(X.theNumber(), expected == X.theNumber());
            ASSERTV(sa.numBytesTotal(), 0 == sa.numBytesTotal());
            ASSERTV(da.numBytesTotal(), 0 == da.numBytesTotal());
        }
        //   Json& Json::operator=(bsls::Types::Uint64 rhs);
        {
            bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);
            bslma::TestAllocator xa("scratch",  veryVeryVeryVerbose);

            Obj mX(&sa);  const Obj& X = mX;

            const bsls::Types::Uint64 v = 1;
            const JsonNumber          expected(v, &xa);

            Json& result = (mX = v);

            ASSERTV(&X, &result, &X == &result);
            ASSERTV(X.type(), JsonType::e_NUMBER == X.type());
            ASSERTV(X.theNumber(), expected == X.theNumber());
            ASSERTV(sa.numBytesTotal(), 0 == sa.numBytesTotal());
            ASSERTV(da.numBytesTotal(), 0 == da.numBytesTotal());
        }
        //   Json& Json::operator=(const JsonNumber& rhs);
        {
            bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);
            bslma::TestAllocator xa("scratch",  veryVeryVeryVerbose);

            Obj mX(&sa);  const Obj& X = mX;

            const JsonNumber v("100000000000000000000000000000000000001", &sa);
            const JsonNumber expected(v, &xa);

            ASSERTV(xa.numBlocksTotal(), 0 < xa.numBlocksTotal());

            Json& result = (mX = v);

            ASSERTV(&X, &result, &X == &result);
            ASSERTV(X.type(), JsonType::e_NUMBER == X.type());
            ASSERTV(X.theNumber(), expected == X.theNumber());
            ASSERTV(sa.numBlocksInUse(), 0 < sa.numBlocksInUse());
            ASSERTV(sa.numBlocksInUse(),
                    sa.numBlocksTotal(),
                    sa.numBlocksInUse() == sa.numBlocksTotal());
            ASSERTV(da.numBytesTotal(), 0 == da.numBytesTotal());
        }
        //   Json& Json::operator=(bslmf::MovableRef<JsonNumber> rhs);
        {
            bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);
            bslma::TestAllocator xa("scratch",  veryVeryVeryVerbose);

            Obj mX(&sa);  const Obj& X = mX;

            JsonNumber v("100000000000000000000000000000000000000000001", &sa);

            const JsonNumber expected(v, &xa);

            ASSERTV(sa.numBlocksTotal(), 0 < sa.numBlocksTotal());
            bslma::TestAllocatorMonitor sam(&sa);

            Json& result = (mX = bslmf::MovableRefUtil::move(v));

            ASSERTV(&X, &result, &X == &result);
            ASSERTV(X.type(), JsonType::e_NUMBER == X.type());
            ASSERTV(X.theNumber(), expected == X.theNumber());
            ASSERTV(sam.isTotalSame());
            ASSERTV(da.numBytesTotal(), 0 == da.numBytesTotal());
        }
        //   Json& Json::operator=(const char *rhs);
        {
            bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);
            bslma::TestAllocator xa("scratch",  veryVeryVeryVerbose);

            Obj mX(&sa);  const Obj& X = mX;

            const char* const v = SUFFICIENTLY_LONG_STRING;
            const bsl::string expected(v, &xa);

            ASSERTV(xa.numBlocksTotal(), 0 < xa.numBlocksTotal());

            Json& result = (mX = v);

            ASSERTV(&mX, &result, &mX == &result);
            ASSERTV(X.type(), JsonType::e_STRING == X.type());
            ASSERTV(X.theNumber(), expected == X.theString());
            ASSERTV(sa.numBlocksInUse(), 0 < sa.numBlocksInUse());
            ASSERTV(sa.numBlocksInUse(),
                    sa.numBlocksTotal(),
                    sa.numBlocksInUse() == sa.numBlocksTotal());
            ASSERTV(da.numBytesTotal(), 0 == da.numBytesTotal());
        }
        //   Json& Json::operator=(const bsl::string_view& rhs);
        {
            bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);
            bslma::TestAllocator xa("scratch",  veryVeryVeryVerbose);

            Obj mX(&sa);  const Obj& X = mX;

            const bsl::string_view v = SUFFICIENTLY_LONG_STRING;
            const bsl::string      expected(v, &xa);

            ASSERTV(xa.numBlocksTotal(), 0 < xa.numBlocksTotal());

            Json& result = (mX = v);

            ASSERTV(&mX, &result, &mX == &result);
            ASSERTV(X.type(), JsonType::e_STRING == X.type());
            ASSERTV(X.theNumber(), expected == X.theString());
            ASSERTV(sa.numBlocksInUse(), 0 < sa.numBlocksInUse());
            ASSERTV(sa.numBlocksInUse(),
                    sa.numBlocksTotal(),
                    sa.numBlocksInUse() == sa.numBlocksTotal());
            ASSERTV(da.numBytesTotal(), 0 == da.numBytesTotal());
        }
        //   Json& Json::operator=(STRING&& rhs);
        {
            bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);
            bslma::TestAllocator xa("scratch",  veryVeryVeryVerbose);

            Obj mX(&sa);  const Obj& X = mX;

            const bsl::string v(SUFFICIENTLY_LONG_STRING, &xa);
            const bsl::string expected(v, &xa);

            ASSERTV(xa.numBlocksTotal(), 0 < xa.numBlocksTotal());

            Json& result = (mX = v);

            ASSERTV(&mX, &result, &mX == &result);
            ASSERTV(X.type(), JsonType::e_STRING == X.type());
            ASSERTV(X.theString(), expected == X.theString());
            ASSERTV(sa.numBlocksInUse(), 0 < sa.numBlocksInUse());
            ASSERTV(sa.numBlocksInUse(),
                    sa.numBlocksTotal(),
                    sa.numBlocksInUse() == sa.numBlocksTotal());
            ASSERTV(da.numBytesTotal(), 0 == da.numBytesTotal());
        }
        {
            bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);
            bslma::TestAllocator xa("scratch",  veryVeryVeryVerbose);

            Obj mX(&sa);  const Obj& X = mX;

            bsl::string       v(SUFFICIENTLY_LONG_STRING, &sa);
            const bsl::string expected(v, &xa);

            ASSERTV(sa.numBlocksTotal(), 0 < sa.numBlocksTotal());
            bslma::TestAllocatorMonitor sam(&sa);

            Json& result = (mX = bslmf::MovableRefUtil::move(v));

            ASSERTV(&X, &result, &X == &result);
            ASSERTV(X.type(), JsonType::e_STRING == X.type());
            ASSERTV(X.theString(), expected == X.theString());
            ASSERTV(sam.isTotalSame());
            ASSERTV(da.numBytesTotal(), 0 == da.numBytesTotal());
        }
        //   Json& Json::operator=(bool rhs);
        {
            bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);

            Obj mX(&sa);  const Obj& X = mX;

            const bool v = true;
            const bool expected = v;

            Json& result = (mX = v);

            ASSERTV(&X, &result, &X == &result);
            ASSERTV(X.type(), JsonType::e_BOOLEAN == X.type());
            ASSERTV(X.theBoolean(), expected == X.theBoolean());
            ASSERTV(sa.numBlocksTotal(), 0 == sa.numBlocksTotal());
            ASSERTV(da.numBytesTotal(), 0 == da.numBytesTotal());
        }
        //   Json& Json::operator=(const JsonObject& rhs);
        {
            bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);
            bslma::TestAllocator xa("scratch",  veryVeryVeryVerbose);

            Obj mX(&sa);  const Obj& X = mX;

            const JsonObject v(
                            JsonValueConstructorHelper<JsonObject>::getValue(),
                            &xa);
            const JsonObject expected(v, &xa);

            ASSERTV(xa.numBlocksTotal(), 0 < xa.numBlocksTotal());

            Json& result = (mX = v);

            ASSERTV(&X, &result, &X == &result);
            ASSERTV(X.type(), JsonType::e_OBJECT == X.type());
            ASSERTV(X.theObject(), expected == X.theObject());
            ASSERTV(sa.numBlocksInUse(), 0 < sa.numBlocksInUse());
            ASSERTV(sa.numBlocksInUse(),
                    sa.numBlocksTotal(),
                    sa.numBlocksInUse() == sa.numBlocksTotal());
            ASSERTV(da.numBytesTotal(), 0 == da.numBytesTotal());
        }
        //   Json& Json::operator=(bslmf::MovableRef<JsonObject> rhs);
        {
            bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);
            bslma::TestAllocator xa("scratch",  veryVeryVeryVerbose);

            Obj mX(&sa);  const Obj& X = mX;

            JsonObject v(JsonValueConstructorHelper<JsonObject>::getValue(),
                          &sa);

            const JsonObject expected(v, &xa);

            ASSERTV(sa.numBlocksTotal(), 0 < sa.numBlocksTotal());
            bslma::TestAllocatorMonitor sam(&sa);

            Json& result = (mX = bslmf::MovableRefUtil::move(v));

            ASSERTV(&X, &result, &X == &result);
            ASSERTV(X.type(), JsonType::e_OBJECT == X.type());
            ASSERTV(X.theObject(), expected == X.theObject());
            ASSERTV(sam.isTotalSame());
            ASSERTV(da.numBytesTotal(), 0 == da.numBytesTotal());
        }
        //   Json& Json::operator=(const JsonArray& rhs);
        {
            bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);
            bslma::TestAllocator xa("scratch",  veryVeryVeryVerbose);

            Obj mX(&sa);  const Obj& X = mX;

            const JsonArray v(
                            JsonValueConstructorHelper<JsonArray>::getValue(),
                            &xa);
            const JsonArray expected(v, &xa);

            ASSERTV(xa.numBlocksTotal(), 0 < xa.numBlocksTotal());

            Json& result = (mX = v);

            ASSERTV(&X, &result, &X == &result);
            ASSERTV(X.type(), JsonType::e_ARRAY == X.type());
            ASSERTV(X.theArray(), expected == X.theArray());
            ASSERTV(sa.numBlocksInUse(), 0 < sa.numBlocksInUse());
            ASSERTV(sa.numBlocksInUse(),
                    sa.numBlocksTotal(),
                    sa.numBlocksInUse() == sa.numBlocksTotal());
            ASSERTV(da.numBytesTotal(), 0 == da.numBytesTotal());
        }
        //   Json& Json::operator=(bslmf::MovableRef<JsonArray> rhs);
        {
            bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);
            bslma::TestAllocator xa("scratch",  veryVeryVeryVerbose);

            Obj mX(&sa);  const Obj& X = mX;

            JsonArray v(JsonValueConstructorHelper<JsonArray>::getValue(),
                        &sa);

            const JsonArray expected(v, &xa);

            ASSERTV(sa.numBlocksTotal(), 0 < sa.numBlocksTotal());
            bslma::TestAllocatorMonitor sam(&sa);

            Json& result = (mX = bslmf::MovableRefUtil::move(v));

            ASSERTV(&X, &result, &X == &result);
            ASSERTV(X.type(), JsonType::e_ARRAY == X.type());
            ASSERTV(X.theArray(), expected == X.theArray());
            ASSERTV(sam.isTotalSame());
            ASSERTV(da.numBytesTotal(), 0 == da.numBytesTotal());
        }
        //   Json& Json::operator=(const JsonNull& rhs);
        {
            bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);

            Obj mX(&sa);  const Obj& X = mX;

            const JsonNull v;
            const JsonNull expected(v);

            Json& result = (mX = v);

            ASSERTV(&X, &result, &X == &result);
            ASSERTV(X.type(), JsonType::e_NULL == X.type());
            ASSERTV(X.theNull(), expected == X.theNull());
            ASSERTV(sa.numBlocksTotal(), 0 == sa.numBlocksTotal());
            ASSERTV(da.numBytesTotal(), 0 == da.numBytesTotal());
        }
        //   Json& Json::operator=(bslmf::MovableRef<JsonNull> rhs);
        {
            bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);

            Obj mX(&sa);  const Obj& X = mX;

            JsonNull       v;
            const JsonNull expected(v);

            bslma::TestAllocatorMonitor sam(&sa);

            Json& result = (mX = bslmf::MovableRefUtil::move(v));

            ASSERTV(&X, &result, &X == &result);
            ASSERTV(X.type(), JsonType::e_NULL == X.type());
            ASSERTV(X.theNull(), expected == X.theNull());
            ASSERTV(sam.isTotalSame());
            ASSERTV(da.numBytesTotal(), 0 == da.numBytesTotal());
        }
        //   JsonArray& Json::makeArray(const JsonArray& array);
        {
            bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);
            bslma::TestAllocator xa("scratch",  veryVeryVeryVerbose);

            Obj mX(&sa);  const Obj& X = mX;

            const JsonArray v(
                            JsonValueConstructorHelper<JsonArray>::getValue(),
                            &xa);
            const JsonArray expected(v, &xa);

            ASSERTV(xa.numBlocksTotal(), 0 < xa.numBlocksTotal());

            JsonArray& result = mX.makeArray(v);

            ASSERTV(X.type(), JsonType::e_ARRAY == X.type());
            ASSERTV(X.theArray(), expected == X.theArray());
            ASSERTV(&result, &(X.theArray()), &(X.theArray()) == &result);
            ASSERTV(sa.numBlocksInUse(), 0 < sa.numBlocksInUse());
            ASSERTV(sa.numBlocksInUse(),
                    sa.numBlocksTotal(),
                    sa.numBlocksInUse() == sa.numBlocksTotal());
            ASSERTV(da.numBytesTotal(), 0 == da.numBytesTotal());
        }
        //   JsonArray& Json::makeArray(MovableRef<JsonArray> array);
        {
            bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);
            bslma::TestAllocator xa("scratch",  veryVeryVeryVerbose);

            Obj mX(&sa);  const Obj& X = mX;

            JsonArray v(JsonValueConstructorHelper<JsonArray>::getValue(),
                        &sa);

            const JsonArray expected(v, &xa);

            ASSERTV(sa.numBlocksTotal(), 0 < sa.numBlocksTotal());
            bslma::TestAllocatorMonitor sam(&sa);

            JsonArray& result = mX.makeArray(bslmf::MovableRefUtil::move(v));

            ASSERTV(X.type(), JsonType::e_ARRAY == X.type());
            ASSERTV(X.theArray(), expected == X.theArray());
            ASSERTV(&result, &(X.theArray()), &(X.theArray()) == &result);
            ASSERTV(sam.isTotalSame());
            ASSERTV(da.numBytesTotal(), 0 == da.numBytesTotal());
        }
        //   bool& Json::makeBoolean(bool boolean);
        {
            bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);

            Obj mX(&sa);  const Obj& X = mX;

            const bool v = true;

            bool& result = mX.makeBoolean(v);

            ASSERTV(X.type(), JsonType::e_BOOLEAN == X.type());
            ASSERTV(X.theBoolean(), v == X.theBoolean());
            ASSERTV(&result,
                    &(X.theBoolean()),
                    &(X.theBoolean()) == &result);
            ASSERTV(sa.numBytesTotal(), 0 == sa.numBytesTotal());
            ASSERTV(da.numBytesTotal(), 0 == da.numBytesTotal());
        }
        //   JsonNumber& Json::makeNumber(const JsonNumber& number);
        {
            bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);
            bslma::TestAllocator xa("scratch",  veryVeryVeryVerbose);

            Obj mX(&sa);  const Obj& X = mX;

            const JsonNumber v("100000000000000000000000000000000000001", &xa);
            const JsonNumber expected(v, &xa);

            ASSERTV(xa.numBlocksTotal(), 0 < xa.numBlocksTotal());

            JsonNumber& result = mX.makeNumber(v);

            ASSERTV(X.type(), JsonType::e_NUMBER == X.type());
            ASSERTV(X.theNumber(), expected == X.theNumber());
            ASSERTV(&result, &(X.theNumber()), &(X.theNumber()) == &result);
            ASSERTV(sa.numBlocksInUse(), 0 < sa.numBlocksInUse());
            ASSERTV(sa.numBlocksInUse(),
                    sa.numBlocksTotal(),
                    sa.numBlocksInUse() == sa.numBlocksTotal());
            ASSERTV(da.numBytesTotal(), 0 == da.numBytesTotal());
        }
        //   JsonNumber& Json::makeNumber(MovableRef<JsonNumber> number);
        {
            bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);
            bslma::TestAllocator xa("scratch",  veryVeryVeryVerbose);

            Obj mX(&sa);  const Obj& X = mX;

            JsonNumber v("100000000000000000000000000000000000000000001", &sa);

            const JsonNumber expected(v, &xa);

            ASSERTV(sa.numBlocksTotal(), 0 < sa.numBlocksTotal());
            bslma::TestAllocatorMonitor sam(&sa);

            JsonNumber& result = mX.makeNumber(bslmf::MovableRefUtil::move(v));

            ASSERTV(X.type(), JsonType::e_NUMBER == X.type());
            ASSERTV(X.theNumber(), expected == X.theNumber());
            ASSERTV(&result, &(X.theNumber()), &(X.theNumber()) == &result);
            ASSERTV(sam.isTotalSame());
            ASSERTV(da.numBytesTotal(), 0 == da.numBytesTotal());
        }
        //   JsonObject& Json::makeObject(const JsonObject& object);
        {
            bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);
            bslma::TestAllocator xa("scratch",  veryVeryVeryVerbose);

            Obj mX(&sa);  const Obj& X = mX;

            const JsonObject v(
                            JsonValueConstructorHelper<JsonObject>::getValue(),
                            &xa);
            const JsonObject expected(v, &xa);

            ASSERTV(xa.numBlocksTotal(), 0 < xa.numBlocksTotal());

            JsonObject& result = mX.makeObject(v);

            ASSERTV(X.type(), JsonType::e_OBJECT == X.type());
            ASSERTV(X.theObject(), expected == X.theObject());
            ASSERTV(&result, &(X.theObject()), &(X.theObject()) == &result);
            ASSERTV(sa.numBlocksInUse(), 0 < sa.numBlocksInUse());
            ASSERTV(sa.numBlocksInUse(),
                    sa.numBlocksTotal(),
                    sa.numBlocksInUse() == sa.numBlocksTotal());
            ASSERTV(da.numBytesTotal(), 0 == da.numBytesTotal());
        }
        //   JsonObject& Json::makeObject(MovableRef<JsonObject> object);
        {
            bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);
            bslma::TestAllocator xa("scratch",  veryVeryVeryVerbose);

            Obj mX(&sa);  const Obj& X = mX;

            JsonObject v(JsonValueConstructorHelper<JsonObject>::getValue(),
                         &sa);

            const JsonObject expected(v, &xa);

            ASSERTV(sa.numBlocksTotal(), 0 < sa.numBlocksTotal());
            bslma::TestAllocatorMonitor sam(&sa);

            JsonObject& result = mX.makeObject(bslmf::MovableRefUtil::move(v));

            ASSERTV(X.type(), JsonType::e_OBJECT == X.type());
            ASSERTV(X.theObject(), expected == X.theObject());
            ASSERTV(&result, &(X.theObject()), &(X.theObject()) == &result);
            ASSERTV(sam.isTotalSame());
            ASSERTV(da.numBytesTotal(), 0 == da.numBytesTotal());
        }
        //   void Json::makeString(const bsl::string_view& string);
        {
            bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);
            bslma::TestAllocator xa("scratch",  veryVeryVeryVerbose);

            Obj mX(&sa);  const Obj& X = mX;

            const bsl::string_view v = SUFFICIENTLY_LONG_STRING;
            const bsl::string      expected(v, &xa);

            ASSERTV(xa.numBlocksTotal(), 0 < xa.numBlocksTotal());

            mX.makeString(v);

            ASSERTV(X.type(), JsonType::e_STRING == X.type());
            ASSERTV(X.theString(), expected == X.theString());
            ASSERTV(sa.numBlocksInUse(), 0 < sa.numBlocksInUse());
            ASSERTV(sa.numBlocksInUse(),
                    sa.numBlocksTotal(),
                    sa.numBlocksInUse() == sa.numBlocksTotal());
            ASSERTV(da.numBytesTotal(), 0 == da.numBytesTotal());
        }
        //   void Json::makeString(STRING&& string);
        {
            bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);
            bslma::TestAllocator xa("scratch",  veryVeryVeryVerbose);

            Obj mX(&sa);  const Obj& X = mX;

            bsl::string       v(SUFFICIENTLY_LONG_STRING, &sa);
            const bsl::string expected(v, &xa);

            ASSERTV(sa.numBlocksTotal(), 0 < sa.numBlocksTotal());

            bslma::TestAllocatorMonitor sam(&sa);
            mX.makeString(bslmf::MovableRefUtil::move(v));

            ASSERTV(X.type(), JsonType::e_STRING == X.type());
            ASSERTV(X.theString(), expected == X.theString());
            ASSERTV(sam.isTotalSame());
            ASSERTV(da.numBytesTotal(), 0 == da.numBytesTotal());
        }
        //   JsonArray& Json::theArray();
        {
            bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);

            Obj mX(&sa);  const Obj& X = mX;
            mX.makeArray();
            JsonArray& result = mX.theArray();
            ASSERT(&result == &(X.theArray()));
            ASSERTV(sa.numBlocksTotal(), 0 == sa.numBlocksTotal());
        }
        //   bool& Json::theBoolean();
        {
            bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);

            Obj mX(&sa);  const Obj& X = mX;
            mX.makeBoolean();
            bool& result = mX.theBoolean();
            ASSERT(&result == &(X.theBoolean()));
            ASSERTV(sa.numBlocksTotal(), 0 == sa.numBlocksTotal());
        }
        //   JsonNull& Json::theNull();
        {
            bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);

            Obj mX(&sa);  const Obj& X = mX;
            mX.makeNull();
            JsonNull& result = mX.theNull();
            ASSERT(&result == &(X.theNull()));
            ASSERTV(sa.numBlocksTotal(), 0 == sa.numBlocksTotal());
        }
        //   JsonNumber& Json::theNumber();
        {
            bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);

            Obj mX(&sa);  const Obj& X = mX;
            mX.makeNumber();
            JsonNumber& result = mX.theNumber();
            ASSERT(&result == &(X.theNumber()));
            ASSERTV(sa.numBlocksTotal(), 0 == sa.numBlocksTotal());
        }
        //   JsonObject& Json::theObject();
        {
            bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);

            Obj mX(&sa);  const Obj& X = mX;
            mX.makeObject();
            JsonObject& result = mX.theObject();
            ASSERT(&result == &(X.theObject()));
            ASSERTV(sa.numBlocksTotal(), 0 == sa.numBlocksTotal());
        }
        //   Json& Json::operator[](const string_view& key);
        {
            bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);
            bsl::string_view     key = "key";

            Obj mX(&sa);  const Obj& X = mX;
            mX.makeObject().insert(key, 1);

            bslma::TestAllocatorMonitor sam(&sa);
            Json&                       result   = mX[key];
            const Json&                 expected = X.theObject()[key];
            ASSERTV(&result == &expected);
            ASSERTV(sam.isTotalSame());
        }
        //   Json& Json::operator[](size_t index);
        {
            bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);

            Obj mX(&sa);  const Obj& X = mX;
            mX.makeArray().pushBack(Json());

            bslma::TestAllocatorMonitor sam(&sa);
            Json&                       result   = mX[0];
            const Json&                 expected = X.theArray().front();

            ASSERTV(&result == &expected);
            ASSERTV(sam.isTotalSame());
        }
#if defined(BSLS_COMPILERFEATURES_SUPPORT_OPERATOR_EXPLICIT)
        //   Json::operator BloombergLP::bdljsn::JsonArray &();
        {
            bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);

            Obj mX(&sa);  const Obj& X = mX;
            mX.makeArray();
            JsonArray& result = static_cast<JsonArray&>(mX);
            ASSERTV(&result == &(X.theArray()));
            ASSERTV(sa.numBlocksTotal(), 0 == sa.numBlocksTotal());
        }
        //   Json::operator bool &();
        {
            bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);

            Obj mX(&sa);  const Obj& X = mX;
            mX.makeBoolean();
            bool& result = static_cast<bool&>(mX);
            ASSERTV(&result == &(X.theBoolean()));
            ASSERTV(sa.numBlocksTotal(), 0 == sa.numBlocksTotal());
        }
        //   Json::operator BloombergLP::bdljsn::JsonNull &();
        {
            bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);

            Obj mX(&sa);  const Obj& X = mX;
            mX.makeNull();
            JsonNull& result = static_cast<JsonNull&>(mX);
            ASSERTV(&result == &(X.theNull()));
            ASSERTV(sa.numBlocksTotal(), 0 == sa.numBlocksTotal());
        }
        //   Json::operator BloombergLP::bdljsn::JsonNumber &();
        {
            bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);

            Obj mX(&sa);  const Obj& X = mX;
            mX.makeNumber();
            JsonNumber& result = static_cast<JsonNumber&>(mX);
            ASSERTV(&result == &(X.theNumber()));
            ASSERTV(sa.numBlocksTotal(), 0 == sa.numBlocksTotal());
        }
        //   Json::operator BloombergLP::bdljsn::JsonObject &();
        {
            bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);

            Obj mX(&sa);  const Obj& X = mX;
            mX.makeObject();
            JsonObject& result = static_cast<JsonObject&>(mX);
            ASSERTV(&result == &(X.theObject()));
            ASSERTV(sa.numBlocksTotal(), 0 == sa.numBlocksTotal());
        }
#else
        if (verbose) cout << "Explicit operators unsupported.\n";
#endif
      } break;
      case 38: {
        // --------------------------------------------------------------------
        // JSON BSLX STREAMING
        //   Ensure that we can serialize the value of any object of the class,
        //   and then deserialize that value back into any object of the class.
        //
        // Concerns:
        //: 1 N/A
        //
        // Plan:
        //: 1 N/A
        //
        // Testing:
        //   Reserved for 'bslx' streaming.
        // --------------------------------------------------------------------

        BSLA_MAYBE_UNUSED typedef bdljsn::Json Obj;

        if (verbose) cout << endl
            << "JSON BSLX STREAMING" << endl
            << "===================" << endl;

        if (verbose) cout << "Not yet implemented." << endl;

      } break;
      case 37: {
        // --------------------------------------------------------------------
        // JSON MOVE-ASSIGNMENT OPERATOR
        //   Ensure that we can move the value of any object of the class to
        //   any object of the class, such that the target object subsequently
        //   has the source value, and there are no additional allocations if
        //   only one allocator is being used, and the source object is
        //   unchanged if allocators are different.
        //
        // Concerns:
        //: 1 The move assignment operator can change the value of any
        //:   modifiable target object to that of any source object.
        //:
        //: 2 The allocator used by the target object is unchanged.
        //:
        //: 3 Any memory allocation is from the target object's allocator.
        //:
        //: 4 The signature and return type are standard.
        //:
        //: 5 The reference returned is to the target object (i.e., '*this').
        //:
        //: 6 If the allocators are different, the value of the source object
        //:   is not modified.
        //:
        //: 7 If the allocators are the same, no new allocations happen when
        //:   the move assignment happens.
        //:
        //: 8 The allocator used by the source object is unchanged.
        //:
        //: 9 Any memory allocation is exception neutral.
        //:
        //:10 Assigning an object to itself behaves as expected (alias-safety).
        //:
        //:11 Every object releases any allocated memory at destruction.
        //
        // Plan:
        //: 1 Use the address of 'operator=' to initialize a member-function
        //:   pointer having the appropriate signature and return type for the
        //:   copy-assignment operator defined in this component.  (C-4)
        //:
        //: 2 Create a 'bslma::TestAllocator' object, and install it as the
        //:   default allocator (note that a ubiquitous test allocator is
        //:   already installed as the global allocator).  Create a separate
        //:   'bslma::TestAllocator' object for arrays needed to initialize
        //:   objects under test.
        //:
        //: 3 Using the table-driven technique:
        //:
        //:   1 Specify a set of (unique) valid object values (one per row) in
        //:     terms of three different Json types, including values that
        //:     should require allocation where applicable.
        //:
        //:   2 Additionally, provide a (tri-valued) column, 'MEM', indicating
        //:     the expectation of memory allocation for all typical
        //:     implementations of individual attribute types: ('Y') "Yes",
        //:     ('N') "No", or ('?') "implementation-dependent".
        //:
        //: 4 For each row 'R1' (representing a distinct object value, 'V') in
        //:   the table described in P-3:  (C-1..3, 5-6,8-11)
        //:
        //:   1 Use the value constructor and a "scratch" allocator to create
        //:     two 'const' 'Obj', 'Z' and 'ZZ', each having the value 'V'.
        //:
        //:   2 Execute an inner loop that iterates over each row 'R2'
        //:     (representing a distinct object value, 'W') in the table
        //:     described in P-3:
        //:
        //:   3 For each of the iterations (P-4.2):  (C-1..2, 5..8, 11)
        //:
        //:     1 Create a 'bslma::TestAllocator' objects 's1'.
        //:
        //:     2 Use the value constructor and 's1' to create a modifiable
        //:       'Obj', 'mF', having the value 'V'.
        //:
        //:     3 Use the value constructor and 's1' to create a modifiable
        //:       'Obj', 'mX', having the value 'W'.
        //:
        //:     4 Move-assign 'mX' from 'bslmf::MovableRefUtil::move(mF)'.
        //:
        //:     5 Verify that the address of the return value is the same as
        //:       that of 'mX'.  (C-5)
        //:
        //:     6 Use the equality-comparison operator to verify that the
        //:       target object, 'mX', now has the same value as that of 'Z'.
        //:
        //:     7 Use the 'allocator' accessor of both 'mX' and 'mF' to verify
        //:       that the respective allocators used by the target and source
        //:       objects are unchanged.  (C-2, 7)
        //:
        //:     8 Use the appropriate test allocators to verify that no new
        //:       allocations were made by the move assignment operation.
        //:
        //:   4 For each of the iterations (P-4.2):  (C-1..2, 5, 7-9, 11)
        //:
        //:     1 Create two 'bslma::TestAllocator' objects 's1' and 's2'.
        //:
        //:     2 Use the value constructor and 's1' to create a modifiable
        //:       'Obj', 'mF', having the value 'V'.
        //:
        //:     3 Use the value constructor and 's2' to create a modifiable
        //:       'Obj', 'mX', having the value 'W'.
        //:
        //:     4 Move-assign 'mX' from 'bslmf::MovableRefUtil::move(mF)'.
        //:
        //:     5 Verify that the address of the return value is the same as
        //:       that of 'mX'.  (C-5)
        //:
        //:     6 Use the equality-comparison operator to verify that the
        //:       target object, 'mX', now has the same value as that of 'Z'.
        //:
        //:     7 Use the equality-comparison operator to verify that the
        //:       source object, 'mF', now has the same value as that of 'Z'.
        //:
        //:     8 Use the 'allocator' accessor of both 'mX' and 'mF' to verify
        //:       that the respective allocators used by the target and source
        //:       objects are unchanged.  (C-2, 7)
        //:
        //: 5 Repeat steps similar to those described in P-2 except that, this
        //:   time, there is no inner loop (as in P-4.2); instead, the source
        //:   object, 'Z', is a reference to the target object, 'mX', and both
        //:   'mX' and 'ZZ' are initialized to have the value 'V'.  For each
        //:   row (representing a distinct object value, 'V') in the table
        //:   described in P-3:  (C-10)
        //:
        //:   1 Create a 'bslma::TestAllocator' object, 'oa'.
        //:
        //:   2 Use the value constructor and 'oa' to create a modifiable 'Obj'
        //:     'mX'; also use the value constructor and a distinct "scratch"
        //:     allocator to create a 'const' 'Obj' 'ZZ'.
        //:
        //:   3 Let 'Z' be a  reference to 'mX'.
        //:
        //:   4 Assign 'mX' from 'bslmf::MovableRefUtil::move(Z)'.
        //:
        //:   5 Verify that the address of the return value is the same as that
        //:     of 'mX'.
        //:
        //:   6 Use the equality-comparison operator to verify that the
        //:     target object, 'Z', still has the same value as that of 'ZZ'.
        //:     (C-10)
        //:
        //:   7 Use the 'allocator' accessor of 'mX' to verify that it is still
        //:     the object allocator.
        //:
        //:   8 Use the appropriate test allocators to verify that:
        //:
        //:     1 Any memory that is allocated is from the object allocator.
        //:
        //:     2 No additional (e.g., temporary) object memory is allocated
        //:       when assigning an object value that did NOT initially require
        //:       allocated memory.
        //:
        //:     3 All object memory is released when the object is destroyed.
        //:
        //: 6 Use the test allocator from P-2 to verify that no memory is ever
        //:   allocated from the default allocator.  (C-3)
        //
        // Testing:
        //   Json& Json::operator=(bslmf::MovableRef<Json> rhs);
        // --------------------------------------------------------------------

        typedef bdljsn::Json Obj;

        if (verbose) cout << endl
            << "JSON MOVE-ASSIGNMENT OPERATOR" << endl
            << "=============================" << endl;

        {
            typedef Obj& (Obj::*operatorPtr)(bslmf::MovableRef<Obj>);

            // Verify that the signature and return type are standard.

            operatorPtr operatorAssignment = &Obj::operator=;

            (void)operatorAssignment;  // quash potential compiler warning
        }

        if (verbose) cout <<
            "\nCreate a test allocator and install it as the default." << endl;

        bslma::TestAllocator         ia("initializer", veryVeryVeryVerbose);
        bslma::TestAllocator         da("default", veryVeryVeryVerbose);
        bslma::DefaultAllocatorGuard dag(&da);

        if (verbose) cout <<
           "\nUse a table of distinct object values and expected memory usage."
                                                                       << endl;

        bool anyObjectMemoryAllocatedFlag = false;  // We later check that
                                                    // this test allocates
                                                    // some object memory.
        for (char ti = 'a'; ti <= JsonValueEnumeration::maxConfig(); ++ti) {
            const char VALUE_CONFIG1 = ti;

            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

            Obj mZ(&scratch);

            const bool MEMSRC1 = JsonValueEnumeration::setValue(&mZ,
                                                                VALUE_CONFIG1);
            const Obj& Z = mZ;

            Obj mZZ(&scratch);
            JsonValueEnumeration::setValue(&mZZ, VALUE_CONFIG1);
            const Obj& ZZ = mZZ;

            if (veryVerbose) { T_ P_(VALUE_CONFIG1) P_(Z) P(ZZ) }

            // move assignment with the same allocator

            for (char tj = 'a';
                      tj <= JsonValueEnumeration::maxConfig();
                    ++tj) {
                const char VALUE_CONFIG2 = tj;

                bslma::TestAllocator s1("scratch1", veryVeryVeryVerbose);

                {
                    // Test move assignment with same allocator.

                    Obj mF(&s1); const Obj& F=mF;
                    JsonValueEnumeration::setValue(&mF, VALUE_CONFIG1);

                    Obj mX(&s1); const Obj& X=mX;
                    JsonValueEnumeration::setValue(&mX, VALUE_CONFIG2);

                    if (veryVerbose) { T_ P_(VALUE_CONFIG2) P(F) P(X) }

                    ASSERTV(VALUE_CONFIG1,
                            VALUE_CONFIG2,
                            F,
                            X,
                            (F == X) == (VALUE_CONFIG1 == VALUE_CONFIG2));

                    bslma::TestAllocatorMonitor s1m(&s1);

                    Obj *mR = &(mX = bslmf::MovableRefUtil::move(mF));
                    ASSERTV(VALUE_CONFIG1, VALUE_CONFIG2,  Z,   X,  Z == X);
                    ASSERTV(VALUE_CONFIG1, VALUE_CONFIG2, mR, &mX, mR == &mX);

                    ASSERTV(VALUE_CONFIG1, VALUE_CONFIG2, s1m.isTotalSame());

                    ASSERTV(VALUE_CONFIG1, VALUE_CONFIG2, &s1, X.allocator(),
                            &s1 == X.allocator());
                    ASSERTV(VALUE_CONFIG1, VALUE_CONFIG2, &s1, F.allocator(),
                            &s1 == F.allocator());

                    anyObjectMemoryAllocatedFlag |= !!s1.numBlocksInUse();
                }

                // Verify all memory is released on object destruction.

                ASSERTV(VALUE_CONFIG1, VALUE_CONFIG2, s1.numBlocksInUse(),
                        0 == s1.numBlocksInUse());
            }

            // move assignment with different allocators

            for (char tj = 'a';
                      tj <= JsonValueEnumeration::maxConfig();
                    ++tj) {
                const char VALUE_CONFIG2 = tj;

                bslma::TestAllocator s1("scratch1", veryVeryVeryVerbose);
                bslma::TestAllocator s2("scratch2", veryVeryVeryVerbose);

                {
                    // Test move assignment with different allocator

                    Obj mF(&s1); const Obj& F=mF;
                    JsonValueEnumeration::setValue(&mF, VALUE_CONFIG1);

                    Obj mX(&s2); const Obj& X=mX;

                    const bool MEMDST2 = JsonValueEnumeration::setValue(
                                                                &mX,
                                                                VALUE_CONFIG2);

                    if (veryVerbose) { T_ P_(VALUE_CONFIG2) P(F) P(X) }

                    ASSERTV(VALUE_CONFIG1,
                            VALUE_CONFIG2,
                            F,
                            X,
                            (F == X) == (VALUE_CONFIG1 == VALUE_CONFIG2));


                    BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(s2) {
                        if (veryVeryVerbose) { T_ T_ Q(ExceptionTestBody) }

                        Obj *mR = &(mX = bslmf::MovableRefUtil::move(mF));
                        ASSERTV(VALUE_CONFIG1,
                                VALUE_CONFIG2,
                                Z,
                                X,
                                Z == X);

                        ASSERTV(VALUE_CONFIG1,
                                VALUE_CONFIG2,
                                mR,
                                &mX,
                                mR == &mX);

                        ASSERTV(VALUE_CONFIG1,
                                VALUE_CONFIG2,
                                Z,
                                F,
                                Z == F);

                        ASSERTV(VALUE_CONFIG1,
                                VALUE_CONFIG2,
                                &s2,
                                X.allocator(),
                                &s2 == X.allocator());

                        ASSERTV(VALUE_CONFIG1,
                                VALUE_CONFIG2,
                                &s1,
                                F.allocator(),
                                &s1 == F.allocator());


#ifdef BDE_BUILD_TARGET_EXC
                        if (!MEMDST2 && MEMSRC1) {
                            ASSERTV(VALUE_CONFIG1,
                                    VALUE_CONFIG2,
                                    0 < EXCEPTION_COUNT);
                        }
#endif
                    } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                    anyObjectMemoryAllocatedFlag |= !!s1.numBlocksInUse();
                }

                // Verify all memory is released on object destruction.

                ASSERTV(VALUE_CONFIG1,
                        VALUE_CONFIG2,
                        s1.numBlocksInUse(),
                        0 == s1.numBlocksInUse());
                ASSERTV(VALUE_CONFIG1,
                        VALUE_CONFIG2,
                        s2.numBlocksInUse(),
                        0 == s2.numBlocksInUse());
            }

            // self-assignment

            bslma::TestAllocator oa("object", veryVeryVeryVerbose);

            {
                bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

                Obj mX(&oa);
                JsonValueEnumeration::setValue(&mX, VALUE_CONFIG1);

                Obj mZZ(&scratch);  const Obj& ZZ = mZZ;
                JsonValueEnumeration::setValue(&mZZ, VALUE_CONFIG1);

                Obj& Z = mX;

                ASSERTV(VALUE_CONFIG1, ZZ, Z, ZZ == Z);

                bslma::TestAllocatorMonitor oam(&oa);

                Obj *mR = &(mX = bslmf::MovableRefUtil::move(Z));
                ASSERTV(VALUE_CONFIG1, ZZ,   Z, ZZ == Z);
                ASSERTV(VALUE_CONFIG1, mR, &mX, mR == &mX);

                ASSERTV(VALUE_CONFIG1,
                        &oa,
                        Z.allocator(),
                        &oa == Z.allocator());

                ASSERTV(VALUE_CONFIG1, oam.isTotalSame());

                ASSERTV(VALUE_CONFIG1, 0 == da.numBlocksTotal());
            }

            // Verify all object memory is released on destruction.

            ASSERTV(VALUE_CONFIG1,
                    oa.numBlocksInUse(),
                    0 == oa.numBlocksInUse());
        }

        // Double check that some object memory was allocated.

        ASSERT(anyObjectMemoryAllocatedFlag);
      } break;
      case 36: {
        // --------------------------------------------------------------------
        // JSON COPY-ASSIGNMENT OPERATOR
        //   Ensure that we can assign the value of any object of the class to
        //   any object of the class, such that the two objects subsequently
        //   have the same value.
        //
        // Concerns:
        //: 1 The assignment operator can change the value of any modifiable
        //:   target object to that of any source object.
        //:
        //: 2 The allocator used by the target object is unchanged.
        //:
        //: 3 Any memory allocation is from the target object's allocator.
        //:
        //: 4 The signature and return type are standard.
        //:
        //: 5 The reference returned is to the target object (i.e., '*this').
        //:
        //: 6 The value of the source object is not modified.
        //:
        //: 7 The allocator used by the source object is unchanged.
        //:
        //: 8 QoI: Assigning a source object having the default-constructed
        //:   value allocates no memory.
        //:
        //: 9 Any memory allocation is exception neutral.
        //:
        //:10 Assigning an object to itself behaves as expected (alias-safety).
        //:
        //:11 Every object releases any allocated memory at destruction.
        //
        // Plan:
        //: 1 Use the address of 'operator=' to initialize a member-function
        //:   pointer having the appropriate signature and return type for the
        //:   copy-assignment operator defined in this component.  (C-4)
        //:
        //: 2 Create a 'bslma::TestAllocator' object, and install it as the
        //:   default allocator (note that a ubiquitous test allocator is
        //:   already installed as the global allocator).  Create a separate
        //:   'bslma::TestAllocator' object for arrays needed to initialize
        //:   objects under test.
        //:
        //: 3 Using the assignment method technique:
        //:
        //:   1 Create a method 'JsonValueEnumeration::setValue' which assigns
        //:     (unique) valid object values to an input parameter of type
        //:     pointer to 'Json', based on a configuration value of type
        //:     'char' from character "a" to the character returned by
        //:     'JsonValueEnumeration::maxConfig()'.  Values assigned should
        //:     require allocation where applicable.
        //:
        //:   2 Additionally, 'JsonValueEnumeration::setValue' should return a
        //:     boolean value indicating whether the assigned value should have
        //:     allocated memory or not.
        //:
        //: 4 For each configuration 'R1' (representing a distinct object
        //:   value, 'V') assigned by the method described in P-3:  (C-1..2,
        //:   5..8, 11)
        //:
        //:   1 Use the value constructor and a "scratch" allocator to create
        //:     two 'const' 'Obj', 'Z' and 'ZZ', each having the value 'V'.
        //:
        //:   2 Execute an inner loop that iterates over each configuration
        //:     'R2' (representing a distinct object value, 'W') in the method
        //:     described in P-3:
        //:
        //:   3 For each of the iterations (P-4.2):  (C-1..2, 5..8, 11)
        //:
        //:     1 Create a 'bslma::TestAllocator' object, 'oa'.
        //:
        //:     2 Use the value constructor and 'oa' to create a modifiable
        //:       'Obj', 'mX', having the value 'W'.
        //:
        //:     3 Assign 'mX' from 'Z' in the presence of injected exceptions
        //:       (using the 'BSLMA_TESTALLOCATOR_EXCEPTION_TEST_*' macros).
        //:
        //:     4 Verify that the address of the return value is the same as
        //:       that of 'mX'.  (C-5)
        //:
        //:     5 Use the equality-comparison operator to verify that: (C-1, 6)
        //:
        //:       1 The target object, 'mX', now has the same value as that of
        //:         'Z'.  (C-1)
        //:
        //:       2 'Z' still has the same value as that of 'ZZ'.  (C-6)
        //:
        //:     6 Use the 'allocator' accessor of both 'mX' and 'Z' to verify
        //:       that the respective allocators used by the target and source
        //:       objects are unchanged.  (C-2, 7)
        //:
        //:     7 Use the appropriate test allocators to verify that:
        //:       (C-8, 11)
        //:
        //:       1 For an object that (a) is initialized with a value that did
        //:         NOT require memory allocation, and (b) is then assigned a
        //:         value that DID require memory allocation, the target object
        //:         DOES allocate memory from its object allocator only
        //:         (irrespective of the specific number of allocations or the
        //:         total amount of memory allocated); also cross check with
        //:         what is expected for 'mX' and 'Z'.
        //:
        //:       2 An object that is assigned a value that did NOT require
        //:         memory allocation, does NOT allocate memory from its object
        //:         allocator; also cross check with what is expected for 'Z'.
        //:
        //:       3 No additional memory is allocated by the source object.
        //:         (C-8)
        //:
        //:       4 All object memory is released when the object is destroyed.
        //:         (C-11)
        //:
        //: 5 Repeat steps similar to those described in P-2 except that, this
        //:   time, there is no inner loop (as in P-4.2); instead, the source
        //:   object, 'Z', is a reference to the target object, 'mX', and both
        //:   'mX' and 'ZZ' are initialized to have the value 'V'.  For each
        //:   configuration (representing a distinct object value, 'V') in the
        //:   method described in P-3:  (C-9..10)
        //:
        //:   1 Create a 'bslma::TestAllocator' object, 'oa'.
        //:
        //:   2 Use the value constructor and 'oa' to create a modifiable 'Obj'
        //:     'mX'; also use the value constructor and a distinct "scratch"
        //:     allocator to create a 'const' 'Obj' 'ZZ'.
        //:
        //:   3 Let 'Z' be a 'const' reference to 'mX'.
        //:
        //:   4 Assign 'mX' from 'Z' in the presence of injected exceptions
        //:     (using the 'BSLMA_TESTALLOCATOR_EXCEPTION_TEST_*' macros).
        //:     (C-9)
        //:
        //:   5 Verify that the address of the return value is the same as that
        //:     of 'mX'.
        //:
        //:   6 Use the equality-comparison operator to verify that the
        //:     target object, 'Z', still has the same value as that of 'ZZ'.
        //:     (C-10)
        //:
        //:   7 Use the 'allocator' accessor of 'mX' to verify that it is still
        //:     the object allocator.
        //:
        //:   8 Use the appropriate test allocators to verify that:
        //:
        //:     1 Any memory that is allocated is from the object allocator.
        //:
        //:     2 No additional (e.g., temporary) object memory is allocated
        //:       when assigning an object value that did NOT initially require
        //:       allocated memory.
        //:
        //:     3 All object memory is released when the object is destroyed.
        //:
        //: 6 Use the test allocator from P-2 to verify that no memory is ever
        //:   allocated from the default allocator.  (C-3)
        //
        // Testing:
        //   Json& Json::operator=(const Json& rhs);
        //   CONCERN: All memory allocation is from the object's allocator.
        //   CONCERN: All memory allocation is exception neutral.
        //   CONCERN: Object value is independent of the object allocator.
        //   CONCERN: There is no temporary allocation from any allocator.
        // --------------------------------------------------------------------

        typedef bdljsn::Json Obj;

        if (verbose) cout << endl
            << "JSON COPY-ASSIGNMENT OPERATOR" << endl
            << "=============================" << endl;

        if (verbose) cout <<
                 "\nAssign the address of the operator to a variable." << endl;
        {
            typedef Obj& (Obj::*operatorPtr)(const Obj&);

            // Verify that the signature and return type are standard.

            operatorPtr operatorAssignment = &Obj::operator=;

            (void)operatorAssignment;  // quash potential compiler warning
        }

        if (verbose) cout <<
            "\nCreate a test allocator and install it as the default." << endl;

        bslma::TestAllocator         ia("initializer", veryVeryVeryVerbose);
        bslma::TestAllocator         da("default", veryVeryVeryVerbose);
        bslma::DefaultAllocatorGuard dag(&da);

        if (verbose) cout <<
           "\nUse a table of distinct object values and expected memory usage."
                                                                       << endl;

        bool anyObjectMemoryAllocatedFlag = false;  // We later check that
                                                    // this test allocates
                                                    // some object memory.
        for (char ti = 'a'; ti <= JsonValueEnumeration::maxConfig(); ++ti) {
            const char VALUE_CONFIG1   = ti;

            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

            Obj mZ(&scratch);  const Obj& Z = mZ;

            const bool MEM1 = JsonValueEnumeration::setValue(&mZ,
                                                             VALUE_CONFIG1);

            const Obj ZZ(Z, &scratch);

            if (veryVerbose) { T_ P_(VALUE_CONFIG1) P_(Z) P(ZZ) }

            for (char tj = 'a';
                      tj <= JsonValueEnumeration::maxConfig();
                    ++tj) {
                const char VALUE_CONFIG2   = tj;

                bslma::TestAllocator oa("object", veryVeryVeryVerbose);

                {
                    Obj mX(&oa);  const Obj& X = mX;

                    const bool MEM2 = JsonValueEnumeration::setValue(
                                                                &mX,
                                                                VALUE_CONFIG2);

                    if (veryVerbose) { T_ P_(VALUE_CONFIG2) P(X) }

                    ASSERTV(VALUE_CONFIG1,
                            VALUE_CONFIG2,
                            Z,
                            X,
                            (Z == X) == (VALUE_CONFIG1 == VALUE_CONFIG2));

                    bslma::TestAllocatorMonitor oam(&oa), sam(&scratch);

                    BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                        if (veryVeryVerbose) { T_ T_ Q(ExceptionTestBody) }

                        Obj *mR = &(mX = Z);
                        ASSERTV(VALUE_CONFIG1,
                                VALUE_CONFIG2,
                                Z,
                                X,
                                Z == X);
                        ASSERTV(VALUE_CONFIG1,
                                VALUE_CONFIG2,
                                mR,
                                &mX,
                                mR == &mX);


#ifdef BDE_BUILD_TARGET_EXC
                        if (!MEM2 && MEM1) {
                            ASSERTV(VALUE_CONFIG1,
                                    VALUE_CONFIG2,
                                    0 < EXCEPTION_COUNT);
                        }
#endif

                    } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                    ASSERTV(VALUE_CONFIG1, VALUE_CONFIG2, ZZ, Z, ZZ == Z);

                    ASSERTV(VALUE_CONFIG1,
                            VALUE_CONFIG2,
                            &oa,
                            X.allocator(),
                            &oa == X.allocator());
                    ASSERTV(VALUE_CONFIG1,
                            VALUE_CONFIG2,
                            &scratch,
                            Z.allocator(),
                            &scratch == Z.allocator());

                    if (!MEM2 && MEM1) {
                        ASSERTV(VALUE_CONFIG1, VALUE_CONFIG2, oam.isInUseUp());
                    }
                    else if (!MEM1) {
                        ASSERTV(VALUE_CONFIG1,
                                VALUE_CONFIG2,
                                !oam.isInUseUp());
                    }

                    // Record if some object memory was allocated.

                    anyObjectMemoryAllocatedFlag |= !!oa.numBlocksInUse();

                    ASSERTV(VALUE_CONFIG1, VALUE_CONFIG2, sam.isInUseSame());

                    ASSERTV(VALUE_CONFIG1,
                            VALUE_CONFIG2,
                            0 == da.numBlocksTotal());
                }

                // Verify all memory is released on object destruction.

                ASSERTV(VALUE_CONFIG1, VALUE_CONFIG2, oa.numBlocksInUse(),
                        0 == oa.numBlocksInUse());
            }

            // self-assignment

            bslma::TestAllocator oa("object", veryVeryVeryVerbose);

            {
                bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

                Obj       mX(&oa);
                JsonValueEnumeration::setValue(&mX, VALUE_CONFIG1);
                const Obj ZZ(mX, &scratch);

                const Obj& Z = mX;

                ASSERTV(VALUE_CONFIG1, ZZ, Z, ZZ == Z);

                bslma::TestAllocatorMonitor oam(&oa);

                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                    if (veryVeryVerbose) { T_ T_ Q(ExceptionTestBody) }

                    Obj *mR = &(mX = Z);
                    ASSERTV(VALUE_CONFIG1, ZZ,   Z, ZZ == Z);
                    ASSERTV(VALUE_CONFIG1, mR, &mX, mR == &mX);
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                ASSERTV(VALUE_CONFIG1,
                        &oa,
                        Z.allocator(),
                        &oa == Z.allocator());

                ASSERTV(VALUE_CONFIG1, !oam.isInUseUp());

                ASSERTV(VALUE_CONFIG1, 0 == da.numBlocksTotal());
            }

            // Verify all object memory is released on destruction.

            ASSERTV(VALUE_CONFIG1,
                    oa.numBlocksInUse(),
                    0 == oa.numBlocksInUse());
        }

        // Double check that some object memory was allocated.

        ASSERT(anyObjectMemoryAllocatedFlag);
      } break;
      case 35: {
        // --------------------------------------------------------------------
        // JSON SWAP MEMBER AND FREE FUNCTIONS
        //   Ensure that the free 'swap' function is implemented and can
        //   exchange the values of any two objects.  Ensure that member
        //   'swap' is implemented and can exchange the values of any two
        //   objects that use the same allocator.
        //
        // Concerns:
        //: 1 Both functions exchange the values of the (two) supplied objects.
        //:
        //: 2 The common object allocator used by both objects is unchanged.
        //:
        //: 3 The member function does not allocate memory from any allocator;
        //:   nor does the free function when the two objects being swapped use
        //:   the same allocator.
        //:
        //: 4 The free function can be called with two objects that use
        //:   different allocators.
        //:
        //: 5 Both functions have standard signatures and return types.
        //:
        //: 6 Using either function to swap an object with itself does not
        //:   affect the value of the object (alias-safety).
        //:
        //: 7 The free 'swap' function is discoverable through ADL (Argument
        //:   Dependent Lookup).
        //:
        //: 8 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Use the addresses of the 'swap' member and free functions defined
        //:   in this component to initialize, respectively, member-function
        //:   and free-function pointers having the appropriate signatures and
        //:   return types.  (C-5)
        //:
        //: 2 Create a 'bslma::TestAllocator' object, and install it as the
        //:   default allocator (note that a ubiquitous test allocator is
        //:   already installed as the global allocator).  Provide another
        //:   allocator for objects from which objects under test are
        //:   initialized.
        //:
        //: 3 Using the assignment method technique:
        //:
        //:   1 Create a method 'JsonValueEnumeration::setValue' which assigns
        //:     (unique) valid object values to an input parameter of type
        //:     pointer to 'Json', based on a configuration value of type
        //:     'char' from character "a" to the character returned by
        //:     'JsonValueEnumeration::maxConfig()'.  Values assigned should
        //:     require allocation where applicable.
        //:
        //:   2 Additionally, 'JsonValueEnumeration::setValue' should return a
        //:     boolean value indicating whether the assigned value should have
        //:     allocated memory or not.
        //:
        //: 4 For each configuration 'R1' in the method of P-3:  (C1..2, 6)
        //:
        //:   1 Create a 'bslma::TestAllocator' object, 'oa'.
        //:
        //:   2 Use the value constructor and 'oa' to create a modifiable
        //:     'Obj', 'mW', having the value described by 'R1'; also use the
        //:     copy constructor and a "scratch" allocator to create a 'const'
        //:     'Obj' 'XX' from 'mW'.
        //:
        //:   3 Use the member and free 'swap' functions to swap the value of
        //:     'mW' with itself; verify, after each swap, that:  (C-6)
        //:
        //:     1 The value is unchanged.  (C-6)
        //:
        //:     2 The allocator used by the object is unchanged.
        //:
        //:     3 There was no additional object memory allocation.
        //:
        //:   4 For each row 'R2' in the table of P-3:  (C-1..2)
        //:
        //:     1 Use the copy constructor and 'oa' to create a modifiable
        //:       'Obj', 'mX', from 'XX' (P-4.2).
        //:
        //:     2 Use the value constructor and 'oa' to create a modifiable
        //:       'Obj', 'mY', having the value described by 'R2'; also use
        //:       the copy constructor to create, using a "scratch" allocator,
        //:       a 'const' 'Obj', 'YY', from 'Y'.
        //:
        //:     3 Use, in turn, the member and free 'swap' functions to swap
        //:       the values of 'mX' and 'mY'; verify, after each swap, that:
        //:       (C-1..2)
        //:
        //:       1 The values have been exchanged.  (C-1)
        //:
        //:       2 The common object allocator used by 'mX' and 'mY'
        //:         is unchanged in both objects.  (C-2)
        //:
        //:       3 There was no additional object memory allocation.
        //:
        //: 5 Verify that the free 'swap' function is discoverable through ADL:
        //:   (C-7)
        //:
        //:   1 Create a set of contained values, 'A', choosing values that
        //:     allocate memory.
        //:
        //:   2 Create a 'bslma::TestAllocator' object, 'oa'.
        //:
        //:   3 Use the default constructor and 'oa' to create a modifiable
        //:     'Obj' 'mX' (containing no elements); also use the copy
        //:     constructor and a "scratch" allocator to create a 'const' 'Obj'
        //:     'XX' from 'mX'.
        //:
        //:   4 Use the value constructor and 'oa' to create a modifiable 'Obj'
        //:     'mY' having the value described by the 'Ai' attributes; also
        //:     use the copy constructor and a "scratch" allocator to create a
        //:     'const' 'Obj' 'YY' from 'mY'.
        //:
        //:   5 Use the 'bslalg::SwapUtil' helper function template to swap the
        //:     values of 'mX' and 'mY', using the free 'swap' function defined
        //:     in this component, then verify that:  (C-7)
        //:
        //:     1 The values have been exchanged.
        //:
        //:     2 There was no additional object memory allocation.  (C-7)
        //:
        //: 6 Use the test allocator from P-2 to verify that no memory was
        //:   allocated from the default allocator.  (C-3)
        //:
        //: 7 Verify that free 'swap' exchanges the values of any two objects
        //:   that use different allocators.  (C-4)
        //:
        //: 8 Verify that, in appropriate build modes, defensive checks are
        //:   triggered when, using the member 'swap' function, an attempt is
        //:   made to swap objects that do not refer to the same allocator, but
        //:   not when the allocators are the same (using the
        //:   'BSLS_ASSERTTEST_*' macros).  (C-8)
        //
        // Testing:
        //   void Json::swap(Json& other);
        //   void swap(Json& lhs, rhs);
        //   CONCERN: Precondition violations are detected when enabled.
        // --------------------------------------------------------------------

        typedef bdljsn::Json Obj;

        if (verbose) cout << endl
            << "JSON SWAP MEMBER AND FREE FUNCTIONS" << endl
            << "===================================" << endl;

        // The underlying 'bdlb::Variant' swap operation in C++03 performs a
        // copy, which causes allocation.  Certain assertions need to be
        // elided in that case, unless and until the 'Variant' swap is fixed.

#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
        const bool swapNoCopy = true;
        if (verbose) cout <<
            "Tests will assert absense of allocation on swaps of 'Json' "
            "objects with the same allocator.\n";
#else
        const bool swapNoCopy = false;
        if (verbose) cout <<
            "Tests will not assert absense of allocation on swaps of 'Json' "
            "objects with the same allocator.\n";
#endif

        if (verbose) cout <<
                "\nAssign the address of each function to a variable." << endl;
        {
            typedef void (Obj::*funcPtr)(Obj&);
            typedef void (*freeFuncPtr)(Obj&, Obj&);

            // Verify that the signatures and return types are standard.

            funcPtr     memberSwap = &Obj::swap;
            freeFuncPtr freeSwap   = bsl::swap;

            (void)memberSwap;  // quash potential compiler warnings
            (void)freeSwap;
        }

        if (verbose) cout <<
            "\nCreate a test allocator and install it as the default." << endl;

        bslma::TestAllocator         ia("initializer", veryVeryVeryVerbose);
        bslma::TestAllocator         da("default",     veryVeryVeryVerbose);
        bslma::DefaultAllocatorGuard dag(&da);

        if (verbose) cout <<
           "\nUse a table of distinct object values and expected memory usage."
                                                                       << endl;

        bool anyObjectMemoryAllocatedFlag = false;  // We later check that
                                                    // this test allocates
                                                    // some object memory.
        for (char ti = 'a'; ti <= JsonValueEnumeration::maxConfig(); ++ti) {
            const char VALUE_CONFIG1 = ti;

            bslma::TestAllocator      oa("object",  veryVeryVeryVerbose);
            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

            Obj        mW(&oa); const Obj& W = mW;
            const bool allocated = JsonValueEnumeration::setValue(
                                                                &mW,
                                                                VALUE_CONFIG1);

            const Obj XX(W, &scratch);

            if (veryVerbose) { T_ P_(VALUE_CONFIG1) P_(W) P(XX) }

            // member 'swap'
            {
                bslma::TestAllocatorMonitor oam(&oa);

                mW.swap(mW);

                ASSERTV(VALUE_CONFIG1, XX, W, XX == W);
                ASSERTV(VALUE_CONFIG1, &oa == W.allocator());
                ASSERTV(VALUE_CONFIG1, oam.isTotalSame());
            }

            // free function 'swap'
            {
                bslma::TestAllocatorMonitor oam(&oa);

                swap(mW, mW);

                ASSERTV(VALUE_CONFIG1, XX, W, XX == W);
                ASSERTV(VALUE_CONFIG1, &oa == W.allocator());
                if (swapNoCopy) {
                    ASSERTV(VALUE_CONFIG1, oam.isTotalSame());
                }
            }

            // Verify expected ('Y'/'N') object-memory allocations.

            ASSERTV(VALUE_CONFIG1,
                    allocated,
                    oa.numBlocksInUse(),
                    (allocated) == (0 < oa.numBlocksInUse()));

            for (char tj = 'a';
                      tj <= JsonValueEnumeration::maxConfig();
                    ++tj) {
                const char VALUE_CONFIG2 = tj;

                Obj       mX(XX, &oa);   const Obj& X = mX;
                Obj       mY(&oa);       const Obj& Y = mY;
                JsonValueEnumeration::setValue( &mY, VALUE_CONFIG2);
                const Obj YY(Y, &scratch);

                if (veryVerbose) { T_ P_(VALUE_CONFIG2) P_(X) P_(Y) P(YY) }

                // member 'swap'
                {
                    bslma::TestAllocatorMonitor oam(&oa);

                    mX.swap(mY);

                    ASSERTV(VALUE_CONFIG1, VALUE_CONFIG2, YY, X, YY == X);
                    ASSERTV(VALUE_CONFIG1, VALUE_CONFIG2, XX, Y, XX == Y);
                    ASSERTV(VALUE_CONFIG1,
                            VALUE_CONFIG2,
                            &oa == X.allocator());
                    if (swapNoCopy) {
                        ASSERTV(VALUE_CONFIG1,
                                VALUE_CONFIG2,
                                oam.isTotalSame());
                    }
                }

                // free function 'swap', same allocator
                {
                    bslma::TestAllocatorMonitor oam(&oa);

                    swap(mX, mY);

                    ASSERTV(VALUE_CONFIG1, VALUE_CONFIG2, XX, X, XX == X);
                    ASSERTV(VALUE_CONFIG1, VALUE_CONFIG2, YY, Y, YY == Y);
                    ASSERTV(VALUE_CONFIG1,
                            VALUE_CONFIG2,
                            &oa == X.allocator());
                    if (swapNoCopy) {
                        ASSERTV(VALUE_CONFIG1,
                                VALUE_CONFIG2,
                                oam.isTotalSame());
                    }
                }
            }

            // Record if some object memory was allocated.

            anyObjectMemoryAllocatedFlag |= !!oa.numBlocksInUse();
        }

        // Check that some memory was allocated.

        ASSERT(anyObjectMemoryAllocatedFlag);

        if (verbose) cout <<
                "\nInvoke free 'swap' function in a context where ADL is used."
                                                                       << endl;
        {
            bslma::TestAllocator      oa("object",  veryVeryVeryVerbose);
            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

            Obj       mX(&oa);  const Obj& X = mX;
            const Obj XX(X, &scratch);

            Obj       mY(SUFFICIENTLY_LONG_STRING, &oa);
            const Obj& Y = mY;
            const Obj YY(Y, &scratch);

            if (veryVeryVerbose) { T_ P_(X) P(Y) }

            bslma::TestAllocatorMonitor oam(&oa);

            bslalg::SwapUtil::swap(&mX, &mY);

            ASSERTV(YY, X, YY == X);
            ASSERTV(XX, Y, XX == Y);
            if (swapNoCopy) {
                ASSERT(oam.isTotalSame());
            }

            if (veryVeryVerbose) { T_ P_(X) P(Y) }
        }

        // Verify no memory is allocated from the default allocator.

        if (swapNoCopy) {
            ASSERTV(da.numBlocksTotal(), 0 == da.numBlocksTotal());
        }

        if (verbose) cout <<
            "\nFree 'swap' function with different allocators." << endl;
        for (char ti = 'a'; ti <= JsonValueEnumeration::maxConfig(); ++ti) {
            const char VALUE_CONFIG1 = ti;

            bslma::TestAllocator      oa("object",  veryVeryVeryVerbose);
            bslma::TestAllocator     oa2("object2", veryVeryVeryVerbose);
            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

            Obj XX(&scratch);
            JsonValueEnumeration::setValue(&XX, VALUE_CONFIG1);

            if (veryVerbose) { T_ P_(VALUE_CONFIG1) P(XX) }

            for (char tj = 'a';
                      tj <= JsonValueEnumeration::maxConfig();
                    ++tj) {
                const char VALUE_CONFIG2 = tj;

                Obj mX(XX, &oa); const Obj& X = mX;
                Obj mY(&oa2);    const Obj& Y = mY;
                JsonValueEnumeration::setValue(&mY, VALUE_CONFIG2);
                const Obj YY(Y, &scratch);

                if (veryVerbose) { T_ P_(VALUE_CONFIG2) P_(X) P_(Y) P(YY) }

                // free function 'swap', different allocator
                {
                    swap(mX, mY);

                    ASSERTV(VALUE_CONFIG1, VALUE_CONFIG2, YY, X, YY == X);
                    ASSERTV(VALUE_CONFIG1, VALUE_CONFIG2, XX, Y, XX == Y);
                    ASSERTV(VALUE_CONFIG1,
                            VALUE_CONFIG2,
                            &oa  == X.allocator());
                    ASSERTV(VALUE_CONFIG1,
                            VALUE_CONFIG2,
                            &oa2 == Y.allocator());
                }
            }
        }

#ifdef BDE_BUILD_TARGET_EXC
        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bsls::AssertTestHandlerGuard hG;

            if (verbose) cout << "\t'swap' member function" << endl;
            {
                bslma::TestAllocator oa1("object1", veryVeryVeryVerbose);
                bslma::TestAllocator oa2("object2", veryVeryVeryVerbose);

                Obj mA(&oa1);  Obj mB(&oa1);
                Obj mZ(&oa2);

                ASSERT_PASS(mA.swap(mB));
                ASSERT_FAIL(mA.swap(mZ));
            }
        }
#endif
      } break;
      case 34: {
        // --------------------------------------------------------------------
        // JSON MOVE CONSTRUCTOR
        //   Ensure that we can create a distinct object of the class from any
        //   other one, such that the new object has the original value.
        //   Verify that if the same allocator is used there have been no new
        //   allocations, and if a different allocator is used the source
        //   object has the original value.
        //
        // Concerns:
        //: 1 The move constructor (with or without a supplied allocator)
        //:   creates an object having the same value as the original object
        //:   started with.
        //:
        //: 2 If an allocator is NOT supplied, the allocator of the new object
        //:   is the same as the original object, and no new allocations occur.
        //:
        //: 3 If an allocator is supplied that is the same as the original
        //:   object, then no new allocations occur.
        //:
        //: 4 If an allocator is supplied that is different from the original
        //:   object, then the original object's value remains unchanged.
        //:
        //: 5 Supplying a default-constructed allocator explicitly is the same
        //:   as supplying the default allocator.
        //:
        //: 6 Any memory allocation is from the object allocator.
        //:
        //: 7 There is no temporary memory allocation from any allocator.
        //:
        //: 8 Every object releases any allocated memory at destruction.
        //:
        //: 9 The allocator used by the original object is unchanged.
        //:
        //:10 Any memory allocation is exception neutral.
        //
        // Plan:
        //: 1 Using the assignment method technique:
        //:
        //:   1 Create a method 'JsonValueEnumeration::setValue' which assigns
        //:     (unique) valid object values to an input parameter of type
        //:     pointer to 'Json', based on a configuration value of type
        //:     'char' from character "a" to the character returned by
        //:     'JsonValueEnumeration::maxConfig()'.  Values assigned should
        //:     require allocation where applicable.
        //:
        //:   2 Additionally, 'JsonValueEnumeration::setValue' should return a
        //:     boolean value indicating whether the assigned value should have
        //:     allocated memory or not.
        //:
        //: 2 For each row (representing a distinct object value, 'V') in the
        //:   table described in P-1: (C-1..9)
        //:
        //:   1 Use the value constructor and a "scratch" allocator to create
        //:     two 'const' 'Obj', 'Z' and 'ZZ', each having the value 'V'.
        //:
        //:   2 Execute an inner loop that creates an object by
        //:     move-constructing from a newly created object with value V,
        //:     but invokes the move constructor differently in each
        //:     iteration: (a) using the standard single-argument move
        //:     constructor, (b) using the extended move constructor with a
        //:     default-constructed allocator argument (to use the default
        //:     allocator), (c) using the extended move constructor with the
        //:     same allocator as the moved-from object, and (d) using the
        //:     extended move constructor with a different allocator than the
        //:     moved-from object.
        //:
        //: 3 For each of these iterations (P-2.2):
        //:
        //:   1 Create four 'bslma::TestAllocator' objects, and install one as
        //:     the current default allocator (note that a ubiquitous test
        //:     allocator is already installed as the global allocator).
        //:
        //:   2 Dynamically allocate another object 'F" using the 's1'
        //:     allocator having the same value V, using a distinct allocator
        //:     for the object's footprint.
        //:
        //:   3 Dynamically allocate an object 'X' using the appropriate move
        //:     constructor to move from 'F', passing as a second argument
        //:     (a) nothing, (b) 'allocator_type()', (c) '&s1', or (d)
        //:     'allocator_type(&s2)'.
        //:
        //:   4 Record the allocator expected to be used by the new object and
        //:     how much memory it used before the move constructor.
        //:
        //:   5 Verify that space for 2 objects is used in the footprint
        //:     allocator
        //:
        //:   6 Verify that the moved-to object has the expected value 'V' by
        //:     comparing to 'Z'.
        //:
        //:   7 If the allocators of 'F' and 'X' are different verify that the
        //:     value of 'F' is still 'V'
        //:
        //:   8 If the allocators of 'F' and 'X' are the same, verify that no
        //:     extra memory was used by the move constructor.
        //:
        //:   9 Verify that no memory was used by the move constructor as
        //:     temporary memory, and no unused allocators have had any memory
        //:     used.
        //:
        //:  10 Delete both dynamically allocated objects and verify that all
        //:     temporary allocators have had all memory returned to them.
        //:
        //: 3 Test again, using the data of P-1, but this time just for the
        //:   supplied allocator configuration (P-2.2c), and create the object
        //:   as an automatic variable in the presence of injected exceptions
        //:   (using the 'BSLMA_TESTALLOCATOR_EXCEPTION_TEST_*' macros).  Do
        //:   this by creating one object with one test allocator ('s1') and
        //:   then using the move constructor with a separate test allocator
        //:   that is injecting exceptions ('s2').
        //:   (C-10)
        //
        // Testing:
        //   Json(bslmf::MovableRef<Json> original);
        //   Json(bslmf::MovableRef<Json> original, *a);
        // --------------------------------------------------------------------

        typedef bdljsn::Json Obj;

        if (verbose) cout << endl
            << "JSON MOVE CONSTRUCTOR" << endl
            << "=====================" << endl;

        if (verbose) cout <<
             "\nCreate objects with various allocator configurations." << endl;
        {
            bool anyObjectMemoryAllocatedFlag = false;  // We later check that
                                                        // this test allocates
                                                        // some object memory
            for (char ti = 'a';
                      ti <= JsonValueEnumeration::maxConfig();
                    ++ti) {
                const char VALUE_CONFIG = ti;

                bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

                Obj        mZ(&scratch);
                const bool allocated = JsonValueEnumeration::setValue(
                                                                 &mZ,
                                                                 VALUE_CONFIG);
                const Obj& Z = mZ;

                Obj       mZZ(&scratch);
                JsonValueEnumeration::setValue(&mZZ, VALUE_CONFIG);
                const Obj ZZ = mZZ;

                if (veryVerbose) { T_ P_(Z) P(ZZ) }

                for (char cfg = 'a'; cfg <= 'd'; ++cfg) {
                    const char CONFIG = cfg;

                    bslma::TestAllocator da("default",   veryVeryVeryVerbose);
                    bslma::TestAllocator fa("footprint", veryVeryVeryVerbose);
                    bslma::TestAllocator s1("supplied",  veryVeryVeryVerbose);
                    bslma::TestAllocator s2("supplied2", veryVeryVeryVerbose);

                    bslma::DefaultAllocatorGuard dag(&da);

                    Obj *fromPtr = new (fa) Obj(&s1);
                    JsonValueEnumeration::setValue(fromPtr, VALUE_CONFIG);
                    Obj& mF      = *fromPtr;  const Obj& F = mF;

                    bsls::Types::Int64 s1Alloc = s1.numBytesInUse();

                    Obj                  *objPtr = 0;
                    bslma::TestAllocator *objAllocatorPtr = 0;

                    switch (CONFIG) {
                      case 'a': {
                        // normal move constructor
                        objAllocatorPtr = &s1;
                        objPtr = new (fa) Obj(bslmf::MovableRefUtil::move(mF));
                      } break;
                      case 'b': {
                        // allocator move constructor, default allocator
                        objAllocatorPtr = &da;
                        objPtr = new (fa) Obj(bslmf::MovableRefUtil::move(mF),
                                              0);
                      } break;
                      case 'c': {
                        // allocator move constructor, same allocator
                        objAllocatorPtr = &s1;
                        objPtr = new (fa) Obj(bslmf::MovableRefUtil::move(mF),
                                              objAllocatorPtr);
                      } break;
                      case 'd': {
                        // allocator move constructor, same allocator
                        objAllocatorPtr = &s2;
                        objPtr = new (fa) Obj(bslmf::MovableRefUtil::move(mF),
                                              objAllocatorPtr);
                      } break;
                      default: {
                        BSLS_ASSERT_OPT(!"Bad allocator config.");
                      } break;
                    }
                    ASSERTV(VALUE_CONFIG,
                            CONFIG,
                            2*sizeof(Obj) == fa.numBytesInUse());

                    Obj& mX = *objPtr;  const Obj& X = mX;

                    if (veryVerbose) { T_ T_ P_(CONFIG) P(mF) P(X) }

                    bslma::TestAllocator& oa = *objAllocatorPtr;

                    // Verify the value of the object.

                    ASSERTV(VALUE_CONFIG, CONFIG,  Z, X,  Z == X);

                    if (objAllocatorPtr != F.allocator()) {
                        // If the allocators are different, verify that the
                        // value of 'fX' has not changed.

                        ASSERTV(VALUE_CONFIG, CONFIG, Z, F, Z == F);
                    }
                    else {
                        // If the allocators are the same, verify that no new
                        // bytes were allocated by moving.

                        ASSERTV(VALUE_CONFIG,
                                CONFIG,
                                s1Alloc,
                                s1.numBytesInUse(),
                                s1Alloc == s1.numBytesInUse());
                    }

                    // -------------------------------------------------------
                    // Verify any attribute allocators are installed properly.
                    // -------------------------------------------------------

                    if (X.isArray()) {
                        ASSERTV(VALUE_CONFIG,
                                CONFIG,
                                &oa == X.theArray().allocator());
                    }
                    else if (X.isNumber()) {
                        ASSERTV(VALUE_CONFIG,
                                CONFIG,
                                &oa == X.theNumber().allocator());
                    }
                    else if (X.isObject()) {
                        ASSERTV(VALUE_CONFIG,
                                CONFIG,
                                &oa == X.theObject().allocator());
                    }
                    else if (X.isString()) {
                        ASSERTV(VALUE_CONFIG,
                                CONFIG,
                                &oa == X.theString().allocator());
                    }
                    else if (X.isBoolean() || X.isNull()) {
                        // No allocator.
                    }
                    else {
                        BSLS_ASSERT_OPT(!"JSON is no known type");
                    }
                    ASSERTV(VALUE_CONFIG, CONFIG, &oa == X.allocator());
                    ASSERTV(VALUE_CONFIG, CONFIG, &scratch == Z.allocator());

                    // Verify no allocation from the non-object allocators.
                    if (objAllocatorPtr != &da) {
                        ASSERTV(VALUE_CONFIG, CONFIG, da.numBlocksTotal(),
                                0 == da.numBlocksTotal());
                    }

                    if (objAllocatorPtr != &s2) {
                        ASSERTV(VALUE_CONFIG, CONFIG, s2.numBlocksTotal(),
                                0 == s2.numBlocksTotal());
                    }

                    // Verify no temporary allocations from the object
                    // allocator.

                    ASSERTV(VALUE_CONFIG, CONFIG, oa.numBlocksTotal(),
                            oa.numBlocksInUse(),
                            oa.numBlocksTotal() == oa.numBlocksInUse());

                    // Verify expected ('Y'/'N') object-memory allocations.

                    ASSERTV(VALUE_CONFIG,
                            CONFIG,
                            allocated,
                            oa.numBlocksInUse(),
                            (allocated) == (0 < oa.numBlocksInUse()));

                    // Record if some object memory was allocated.

                    anyObjectMemoryAllocatedFlag |= !!oa.numBlocksInUse();

                    // Reclaim dynamically allocated objects under test.

                    fa.deleteObject(fromPtr);
                    fa.deleteObject(objPtr);

                    // Verify all memory is released on object destruction.

                    ASSERTV(VALUE_CONFIG, CONFIG, da.numBlocksInUse(),
                            0 == da.numBlocksInUse());
                    ASSERTV(VALUE_CONFIG, CONFIG, fa.numBlocksInUse(),
                            0 == fa.numBlocksInUse());
                    ASSERTV(VALUE_CONFIG, CONFIG, s1.numBlocksInUse(),
                            0 == s1.numBlocksInUse());
                    ASSERTV(VALUE_CONFIG, CONFIG, s2.numBlocksInUse(),
                            0 == s2.numBlocksInUse());
                }  // end foreach configuration
            }  // end foreach row

            // Double check that some object memory was allocated.

            ASSERT(anyObjectMemoryAllocatedFlag);
        }

        if (verbose) cout << "\nTesting with injected exceptions." << endl;
        {
            for (char ti = 'a';
                      ti <= JsonValueEnumeration::maxConfig();
                    ++ti) {
                const char VALUE_CONFIG = ti;

                bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);
                bslma::TestAllocator da("default",   veryVeryVeryVerbose);
                bslma::TestAllocator s1("supplied",  veryVeryVeryVerbose);
                bslma::TestAllocator s2("supplied2", veryVeryVeryVerbose);

                Obj        mZ (&scratch);
                const bool allocated = JsonValueEnumeration::setValue(
                                                                 &mZ,
                                                                 VALUE_CONFIG);
                const Obj& Z = mZ;

                if (veryVerbose) { T_ P_(allocated) P(Z) }

                bslma::DefaultAllocatorGuard dag(&da);

                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(s2) {
                    if (veryVeryVerbose) { T_ T_ Q(ExceptionTestBody) }

                    Obj from(&s1);
                    JsonValueEnumeration::setValue(&from, VALUE_CONFIG);

                    Obj obj(bslmf::MovableRefUtil::move(from), &s2);
                    ASSERTV(VALUE_CONFIG, Z, obj, Z == obj);

#ifdef BDE_BUILD_TARGET_EXC
                    if (allocated) {
                        ASSERTV(VALUE_CONFIG, 0 < EXCEPTION_COUNT);
                    }
#endif
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
            }
        }
      } break;
      case 33: {
        // --------------------------------------------------------------------
        // JSON COPY CONSTRUCTOR
        //   Ensure that we can create a distinct object of the class from any
        //   other one, such that the two objects have the same value.
        //
        // Concerns:
        //: 1 The copy constructor (with or without a supplied allocator)
        //:   creates an object having the same value as that of the supplied
        //:   original object.
        //:
        //: 2 If an allocator is NOT supplied to the copy constructor, the
        //:   default allocator in effect at the time of construction becomes
        //:   the object allocator for the resulting object (i.e., the
        //:   allocator of the original object is never copied).
        //:
        //: 3 If an allocator IS supplied to the copy constructor, that
        //:   allocator becomes the object allocator for the resulting object.
        //:
        //: 4 Supplying a default-constructed allocator has the same effect as
        //:   not supplying an allocator.
        //:
        //: 5 Supplying an allocator to the copy constructor has no effect on
        //:   subsequent object values.
        //:
        //: 6 Any memory allocation is from the object allocator.
        //:
        //: 7 There is no temporary memory allocation from any allocator.
        //:
        //: 8 Every object releases any allocated memory at destruction.
        //:
        //: 9 The original object is passed as a 'const' reference.
        //:
        //:10 The value of the original object is unchanged.
        //:
        //:11 The allocator used by the original object is unchanged.
        //:
        //:12 QoI: Copying an object having the default-constructed value
        //:   allocates no memory.
        //:
        //:13 Any memory allocation is exception neutral.
        //
        // Plan:
        //: 1 Using the assignment method technique:
        //:
        //:   1 Create a method 'JsonValueEnumeration::setValue' which assigns
        //:     (unique) valid object values to an input parameter of type
        //:     pointer to 'Json', based on a configuration value of type
        //:     'char' from character "a" to the character returned by
        //:     'JsonValueEnumeration::maxConfig()'.  Values assigned should
        //:     require allocation where applicable.
        //:
        //:   2 Additionally, 'JsonValueEnumeration::setValue' should return a
        //:     boolean value indicating whether the assigned value should have
        //:     allocated memory or not.
        //:
        //: 2 For each configuration (representing a distinct object value,
        //:   'V') in the method described in P-1: (C-1..12)
        //:
        //:   1 Use the value constructor and a "scratch" allocator to create
        //:     two 'const' 'Obj', 'Z' and 'ZZ', each having the value 'V'.
        //:
        //:   2 Execute an inner loop that creates an object by
        //:     copy-constructing from value 'Z' from P-2.1, but invokes the
        //:     copy constructor differently in each iteration: (a) without
        //:     passing an allocator, (b) passing a null pointer, and (c)
        //:     passing the address of a test allocator distinct from the
        //:     default.
        //:
        //:   3 For each of these iterations (P-2.2):  (C-1..12)
        //:
        //:     1 Create three 'bslma::TestAllocator' objects, and install one
        //:       as the current default allocator (note that a ubiquitous test
        //:       allocator is already installed as the global allocator).
        //:
        //:     2 Use the copy constructor to dynamically create an object 'X',
        //:       with it's object allocator configured appropriately (see
        //:       P-2.2), supplying it the 'const' object 'Z' (see P-2.1); use
        //:       a distinct test allocator for the object's footprint.  (C-9)
        //:
        //:     3 Use the equality-comparison operator to verify that:
        //:       (C-1, 5, 10)
        //:
        //:       1 The newly constructed object, 'X', has the same value as
        //:         that of 'Z'.  (C-1, 5)
        //:
        //:       2 'Z' still has the same value as that of 'ZZ'  (C-10)
        //:
        //:     4 Use the 'allocator' accessor of each contained object to
        //:       to ensure that its object allocator is properly installed;
        //:       also use the 'allocator' accessor of 'X' to verify that its
        //:       object allocator is properly installed, and use the
        //:       'allocator' accessor of 'Z' to verify that the allocator that
        //:       it uses is unchanged.
        //:
        //:     5 Use the appropriate test allocators to verify that:
        //:       (C-2..4, 7..8, 12)
        //:
        //:       1 An object that IS expected to allocate memory does so from
        //:         the object allocator only (irrespective of the specific
        //:         number of allocations or the total amount of memory
        //:         allocated)..  (C-2, 4)
        //:
        //:       2 An object that is expected NOT to allocate memory doesn't.
        //:         (C-12)
        //:
        //:       3 If an allocator was supplied at construction (P-2.1c), the
        //:         current default allocator doesn't allocate any memory.
        //:         (C-3)
        //:
        //:       4 No temporary memory is allocated from the object allocator.
        //:         (C-7)
        //:
        //:       5 All object memory is released when the object is destroyed.
        //:         (C-8)
        //:
        //: 3 Test again, using the data of P-1, but this time just for the
        //:   supplied allocator configuration (P-2.2c), and create the object
        //:   as an automatic variable in the presence of injected exceptions
        //:   (using the 'BSLMA_TESTALLOCATOR_EXCEPTION_TEST_*' macros).
        //:   (C-13)
        //
        // Testing:
        //   Json(const Json &original, *a);
        // --------------------------------------------------------------------

        typedef bdljsn::Json Obj;

        if (verbose) cout << endl
            << "JSON COPY CONSTRUCTOR" << endl
            << "=====================" << endl;

        if (verbose) cout <<
           "\nUse a table of distinct object values and expected memory usage."
                                                                       << endl;

        if (verbose) cout <<
             "\nCreate objects with various allocator configurations." << endl;
        {
            bool anyObjectMemoryAllocatedFlag = false;  // We later check that
                                                        // this test allocates
                                                        // some object memory

            for (char ti = 'a';
                      ti <= JsonValueEnumeration::maxConfig();
                    ++ti) {
                const char VALUE_CONFIG = ti;

                bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

                Obj        mZ(&scratch);
                const bool allocated = JsonValueEnumeration::setValue(
                                                                 &mZ,
                                                                 VALUE_CONFIG);
                const Obj& Z = mZ;

                Obj mZZ(&scratch);
                JsonValueEnumeration::setValue(&mZZ, VALUE_CONFIG);
                const Obj& ZZ = mZZ;

                if (veryVerbose) { T_ P_(Z) P(ZZ) }

                for (char cfg = 'a'; cfg <= 'c'; ++cfg) {

                    const char CONFIG = cfg;

                    bslma::TestAllocator da("default",   veryVeryVeryVerbose);
                    bslma::TestAllocator fa("footprint", veryVeryVeryVerbose);
                    bslma::TestAllocator sa("supplied",  veryVeryVeryVerbose);

                    bslma::DefaultAllocatorGuard dag(&da);

                    Obj                  *objPtr = 0;
                    bslma::TestAllocator *objAllocatorPtr = 0;

                    switch (CONFIG) {
                      case 'a': {
                        objAllocatorPtr = &da;
                        objPtr = new (fa) Obj(Z);
                      } break;
                      case 'b': {
                        objAllocatorPtr = &da;
                        objPtr = new (fa) Obj(Z, 0);
                      } break;
                      case 'c': {
                        objAllocatorPtr = &sa;
                        objPtr = new (fa) Obj(Z, objAllocatorPtr);
                      } break;
                      default: {
                        BSLS_ASSERT_OPT(!"Bad allocator config");
                      } break;
                    }
                    ASSERTV(VALUE_CONFIG,
                            CONFIG,
                            sizeof(Obj) == fa.numBytesInUse());

                    Obj& mX = *objPtr;  const Obj& X = mX;

                    if (veryVerbose) { T_ T_ P_(CONFIG) P(X) }

                    bslma::TestAllocator&  oa = *objAllocatorPtr;
                    bslma::TestAllocator& noa = (&da == &oa) ? sa : da;

                    // Verify the value of the object.

                    ASSERTV(VALUE_CONFIG, CONFIG, Z, X,  Z == X);

                    // Verify that the value of 'Z' has not changed.

                    ASSERTV(VALUE_CONFIG, CONFIG, Z, X, ZZ == X);

                    // Verify contained object allocators are installed
                    // properly.

                    if (X.isArray()) {
                        ASSERTV(VALUE_CONFIG,
                                CONFIG,
                                &oa == X.theArray().allocator());
                    }
                    else if (X.isNumber()) {
                        ASSERTV(VALUE_CONFIG,
                                CONFIG,
                                &oa == X.theNumber().allocator());
                    }
                    else if (X.isObject()) {
                        ASSERTV(VALUE_CONFIG,
                                CONFIG,
                                &oa == X.theObject().allocator());
                    }
                    else if (X.isString()) {
                        ASSERTV(VALUE_CONFIG,
                                CONFIG,
                                &oa == X.theString().allocator());
                    }
                    else if (X.isBoolean() || X.isNull()) {
                        // No allocator.
                    }
                    else {
                        BSLS_ASSERT_OPT(!"JSON is no known type");
                    }

                    // Also invoke the object's 'allocator' accessor, as well
                    // as that of 'Z'.

                    ASSERTV(VALUE_CONFIG, CONFIG, &oa, X.allocator(),
                            &oa == X.allocator());

                    ASSERTV(VALUE_CONFIG, CONFIG, &scratch, Z.allocator(),
                            &scratch == Z.allocator());

                    // Verify no allocations from the non-object allocator.

                    ASSERTV(VALUE_CONFIG, CONFIG, noa.numBlocksTotal(),
                            0 == noa.numBlocksTotal());

                    // Verify no temporary memory is allocated from the object
                    // allocator.

                    ASSERTV(VALUE_CONFIG, CONFIG,
                            oa.numBlocksTotal(),
                            oa.numBlocksInUse(),
                            oa.numBlocksTotal() == oa.numBlocksInUse());

                    // Verify expected object-memory allocations.

                    ASSERTV(VALUE_CONFIG,
                            CONFIG,
                            allocated,
                            oa.numBlocksInUse(),
                            (allocated) == (0 < oa.numBlocksInUse()));

                    // Record if some object memory was allocated.

                    anyObjectMemoryAllocatedFlag |= !!oa.numBlocksInUse();

                    // Reclaim dynamically allocated object under test.

                    fa.deleteObject(objPtr);

                    // Verify all memory is released on object destruction.

                    ASSERTV(VALUE_CONFIG, CONFIG, da.numBlocksInUse(),
                            0 == da.numBlocksInUse());
                    ASSERTV(VALUE_CONFIG, CONFIG, fa.numBlocksInUse(),
                            0 == fa.numBlocksInUse());
                    ASSERTV(VALUE_CONFIG, CONFIG, sa.numBlocksInUse(),
                            0 == sa.numBlocksInUse());
                }  // end foreach configuration
            }  // end foreach row

            // Double check that some object memory was allocated.

            ASSERT(anyObjectMemoryAllocatedFlag);

        }

        if (verbose) cout << "\nTesting with injected exceptions." << endl;
        {
            for (char ti = 'a';
                      ti <= JsonValueEnumeration::maxConfig();
                    ++ti) {
                const char VALUE_CONFIG = ti;

                bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);
                bslma::TestAllocator da("default",      veryVeryVeryVerbose);
                bslma::TestAllocator sa("supplied",     veryVeryVeryVerbose);

                Obj        mZ(&scratch);
                const bool allocated = JsonValueEnumeration::setValue(
                                                                 &mZ,
                                                                 VALUE_CONFIG);
                const Obj& Z = mZ;

                Obj mZZ;
                JsonValueEnumeration::setValue(&mZZ, VALUE_CONFIG);
                const Obj& ZZ = mZZ;

                bslma::DefaultAllocatorGuard dag(&da);

                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(sa) {
                    if (veryVeryVerbose) { T_ T_ Q(ExcpetionTestBody) }

                    Obj obj(Z, &sa);
                    ASSERTV(VALUE_CONFIG, Z, obj, Z == obj);

#ifdef BDE_BUILD_TARGET_EXC
                    if (allocated) {
                        ASSERTV(VALUE_CONFIG, 0 < EXCEPTION_COUNT);
                    }
#endif
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                ASSERTV(VALUE_CONFIG, ZZ, Z, ZZ == Z);
                ASSERTV(VALUE_CONFIG, &scratch, Z.allocator(),
                        &scratch == Z.allocator());
                ASSERTV(VALUE_CONFIG,
                        da.numBlocksInUse(),
                        0 == da.numBlocksInUse());
                ASSERTV(VALUE_CONFIG,
                        sa.numBlocksInUse(),
                        0 == sa.numBlocksInUse());
            }
        }
      } break;
      case 32: {
        // --------------------------------------------------------------------
        // JSON EQUALITY-COMPARISON OPERATORS
        //   Ensure that '==' and '!=' are the operational definition of value.
        //
        // Concerns:
        //: 1 Two objects, 'X' and 'Y', compare equal if and only if each of
        //:   their corresponding salient attributes respectively compares
        //:   equal.
        //:
        //: 2 All salient attributes participate in the comparison.
        //:
        //: 3 No non-salient attributes (i.e., 'allocator') participate.
        //:
        //: 4 'true  == (X == X)'  (i.e., identity)
        //:
        //: 5 'false == (X != X)'  (i.e., identity)
        //:
        //: 6 'X == Y' if and only if 'Y == X'  (i.e., commutativity)
        //:
        //: 7 'X != Y' if and only if 'Y != X'  (i.e., commutativity)
        //:
        //: 8 'X != Y' if and only if '!(X == Y)'
        //:
        //: 9 Comparison is symmetric with respect to user-defined conversion
        //:   (i.e., both comparison operators are free functions).
        //:
        //:10 Non-modifiable objects can be compared (i.e., 'const' objects and
        //:   'const' references).
        //:
        //:11 No memory allocation occurs as a result of comparison (e.g., the
        //:   arguments are not passed by value).
        //:
        //:12 The equality operator's signature and return type are standard.
        //:
        //:13 The inequality operator's signature and return type are standard.
        //
        // Plan:
        //: 1 Use the respective addresses of 'operator==' and 'operator!=' to
        //:   initialize function pointers having the appropriate signatures
        //:   and return types for the two homogeneous, free equality-
        //:   comparison operators defined in this component.
        //:   (C-9..10, 12..13)
        //:
        //: 2 Create a 'bslma::TestAllocator' object and install it as the
        //:   default allocator (note that a ubiquitous test allocator is
        //:   already installed as the global allocator).
        //:
        //: 3 Using the assignment method technique, specify a set of distinct
        //:   object values (one per configuration value) in terms of their
        //:   individual contents.
        //:
        //: 4 For each of two configurations, 'a' and 'b': (C-1..3, 6..8)
        //:
        //:   1 Create two (object) allocators, 'oax' and 'oay'.
        //:
        //:   2 For each configuration 'R1' in the method of P-3: (C1..8)
        //:
        //:     1 Create object 'x', using 'oax' allocator, having the value
        //:       'R1', and use a 'const' reference 'X' to 'x' to verify the
        //:       reflexive (anti-reflexive) property of equality (inequality)
        //:       in the presence of aliasing.  (C-4..5)
        //:
        //:     2 For each configuration 'R2' in the method of P-3: (C-1..8)
        //:
        //:       1 Record, in 'EXP', whether or not distinct objects created
        //:         from 'R1' and 'R2', respectively, are expected to have the
        //:         same value.
        //:
        //:       2 Create an object 'y', using 'oax' in configuration 'a' and
        //:         'oay' in configuration 'b', having the value 'R2'.  Create
        //:         a 'const' reference to 'y' named 'Y'.
        //:
        //:       3 Verify the commutative property and expected return value
        //:         for both '==' and '!=', while monitoring both 'oax' and
        //:         'oay' to ensure that no object memory is ever allocated by
        //:         either operator.  (C-1..3, 6..8)
        //:
        //: 5 Use the test allocator from P-2 to verify that no memory is ever
        //:   allocated from the default allocator.  (C-11)
        //
        // Testing:
        //   bool operator==(const Json& lhs, rhs);
        //   bool operator!=(const Json& lhs, rhs);
        // --------------------------------------------------------------------

        typedef bdljsn::Json Obj;

        if (verbose) cout << endl
            << "JSON EQUALITY-COMPARISON OPERATORS" << endl
            << "==================================" << endl;

        if (verbose) cout <<
                 "\nAssign the address of each operator to a variable" << endl;
        {
            typedef bool (*operatorPtr)(const Obj&, const Obj&);

            operatorPtr operatorEq = operator==;  (void) operatorEq;
            operatorPtr operatorNe = operator!=;  (void) operatorNe;
        }

        if (verbose) cout <<
            "\nCreate a test allocator and install it as the default." << endl;

        bslma::TestAllocator         da("default", veryVeryVeryVerbose);
        bslma::DefaultAllocatorGuard dag(&da);

        if (verbose) cout <<
            "\nDefine appropriate individual attribute values, 'Ai' and 'Bi'."
                                                                       << endl;

        if (verbose) cout << "\nCompare every value with every value." << endl;

        for (char cfg = 'a'; cfg <= 'b'; ++cfg) {

            const char CONFIG = cfg;

            // Create two distinct test allocators, 'oax' and 'oay'.

            bslma::TestAllocator oax("objectx", veryVeryVeryVerbose);
            bslma::TestAllocator oay("objecty", veryVeryVeryVerbose);

            // Map allocators above to objects 'X' and 'Y' below.

            bslma::TestAllocator& xa = oax;
            bslma::TestAllocator& ya = 'a' == CONFIG ? oax : oay;

            //for (int ti = 0; ti < NUM_DATA; ++ti) {
            for (char ti =  'a';
                      ti <= JsonValueEnumeration::maxConfig();
                    ++ti) {
                const char CONFIG1  = ti;

                Obj x(&xa);
                JsonValueEnumeration::setValue(&x, CONFIG1);

                const Obj&    X = x;

                if (veryVerbose) { T_ T_ P_(CONFIG1) P_(X) }

                // Ensure an object compares correctly with itself (alias
                // test).
                {
                    ASSERTV(CONFIG1, X, X == X);
                    ASSERTV(CONFIG1, X, !(X != X));
                }

                for (char tj =  'a';
                          tj <= JsonValueEnumeration::maxConfig();
                        ++tj) {
                    const char CONFIG2 = tj;

                    Obj y(&ya);
                    JsonValueEnumeration::setValue(&y, CONFIG2);

                    const Obj& Y = y;

                    if (veryVerbose) { T_ T_ P_(CONFIG2) P_(Y) }

                    const bool EXP = (ti == tj);

                    if (veryVerbose) {
                        T_ T_ T_ P_(EXP) P_(CONFIG) P_(X) P_(Y) }

                    // Verify value, commutativity, and no memory allocation

                    bslma::TestAllocatorMonitor oaxm(&oax), oaym(&oay);

                    ASSERTV(CONFIG1, CONFIG2, CONFIG, X, Y, EXP == (X == Y));
                    ASSERTV(CONFIG1, CONFIG2, CONFIG, Y, X, EXP == (Y == X));

                    ASSERTV(CONFIG1, CONFIG2, CONFIG, X, Y, !EXP == (X != Y));
                    ASSERTV(CONFIG1, CONFIG2, CONFIG, Y, X, !EXP == (Y != X));

                    ASSERTV(CONFIG1, CONFIG2, CONFIG, oaxm.isTotalSame());
                    ASSERTV(CONFIG1, CONFIG2, CONFIG, oaym.isTotalSame());
                }
            }
        }

        ASSERTV(da.numBlocksTotal(), 0 == da.numBlocksTotal());
      } break;
      case 31: {
        // --------------------------------------------------------------------
        // JSON PRINT AND OUTPUT OPERATOR
        //   Ensure that the value of the object can be formatted appropriately
        //   on an 'ostream' in some standard, human-readable form.
        //
        // Concerns:
        //: 1 Not implemented at this time.
        //
        // Plan:
        //: 1 Not implemented at this time.
        //
        // Testing:
        //   ostream& Json::print(ostream& stream, int l, int spl) const;
        //   ostream& operator<<(ostream&, const Json&);
        // --------------------------------------------------------------------

        BSLA_MAYBE_UNUSED typedef bdljsn::Json Obj;

        if (verbose) cout << endl
            << "JSON PRINT AND OUTPUT OPERATOR" << endl
            << "==============================" << endl;
      } break;
      case 30: {
        // --------------------------------------------------------------------
        // JSON BASIC ACCESSORS
        //   Ensure each basic accessor properly provides access to contained
        //   objects.
        //
        // Concerns:
        //: 1 Each accessor returns the value of the corresponding contained
        //:   object.
        //:
        //: 2 Each accessor method is declared 'const'.
        //:
        //: 3 No accessor allocates memory.
        //:
        //: 4 Accessors return a 'const' reference.
        //
        // Plan:
        //: 1 Create two 'bslma::TestAllocator' objects, and install one as
        //:   the current default allocator (note that a ubiquitous test
        //:   allocator is already installed as the global allocator).
        //:
        //: 2 Use the default constructor with the allocator from P-1 that was
        //:   not installed as default, to create an object.
        //:
        //: 3 Verify that each basic accessor, invoked on a 'const' reference
        //:   to the object created in P-2, returns the expected value.  (C-2)
        //:
        //: 4 For each variant type: (C-1, 3..4)
        //:
        //:   1 Use the corresponding primary manipulator to put a non-default,
        //:     discriminable object into the container.
        //:
        //:   2 Use the 'type' accessor and the type-dependent accessor to
        //:     verify expected values.  (C-1)
        //:
        //:   3 Monitor the memory allocated from both the default and object
        //:     allocators before and after calling the accessor; verify that
        //:     there is no change in total memory allocation.  (C-3..4)
        //
        // Testing:
        //   const JsonArray& Json::theArray() const;
        //   const bool& Json::theBoolean() const;
        //   const JsonNull& Json::theNull() const;
        //   const JsonNumber& Json::theNumber() const;
        //   const JsonObject& Json::theObject() const;
        //   const bsl::string& Json::theString() const;
        //   JsonType::Enum Json::type() const;
        // --------------------------------------------------------------------

        typedef bdljsn::Json Obj;

        if (verbose) cout << endl << "JSON BASIC ACCESSORS" << endl
                                  << "====================" << endl;

        const JsonNumber  scratchNumber(SUFFICIENTLY_LONG_STRING);
        const bsl::string scratchString(SUFFICIENTLY_LONG_STRING);

        JsonArray scratchArray;
        scratchArray.pushBack(Json(scratchNumber));
        scratchArray.pushBack(Json(scratchString));


        JsonObject scratchObject;
        scratchObject["number"] = Json(scratchNumber);
        scratchObject["string"] = Json(scratchString);


        bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);
        bslma::TestAllocator da("default",  veryVeryVeryVerbose);

        bslma::DefaultAllocatorGuard dag(&da);

        Obj mX(&sa); const Obj& X = mX;
        {
            bslma::TestAllocatorMonitor tam(&sa);
            ASSERTV(X.type(), JsonType::e_NULL == X.type());
            const JsonNull& null = X.theNull(); (void) null;
            ASSERTV(tam.isTotalSame());
        }

        mX.makeArray(scratchArray);
        {
            bslma::TestAllocatorMonitor tam(&sa);
            ASSERTV(X.type(), JsonType::e_ARRAY == X.type());
            const JsonArray& array = X.theArray();
            ASSERTV(array, scratchArray, array == scratchArray);
            ASSERTV(tam.isTotalSame());
        }

        mX.makeBoolean(true);
        {
            bslma::TestAllocatorMonitor tam(&sa);
            ASSERTV(X.type(), JsonType::e_BOOLEAN == X.type());
            const bool& boolean = X.theBoolean();
            ASSERTV(boolean == true);
            ASSERTV(tam.isTotalSame());
        }

        mX.makeNull();
        {
            bslma::TestAllocatorMonitor tam(&sa);
            ASSERTV(X.type(), JsonType::e_NULL == X.type());
            const JsonNull& otherNull = X.theNull(); (void) otherNull;
            ASSERTV(tam.isTotalSame());
        }

        mX.makeNumber(scratchNumber);
        {
            bslma::TestAllocatorMonitor tam(&sa);
            ASSERTV(X.type(), JsonType::e_NUMBER == X.type());
            const JsonNumber& number = X.theNumber();
            ASSERTV(number, scratchNumber, number == scratchNumber);
            ASSERTV(tam.isTotalSame());
        }

        mX.makeObject(scratchObject);
        {
            bslma::TestAllocatorMonitor tam(&sa);
            ASSERTV(X.type(), JsonType::e_OBJECT == X.type());
            const JsonObject& object = X.theObject();
            ASSERTV(object, scratchObject, object == scratchObject);
            ASSERTV(tam.isTotalSame());
        }

        mX.makeString(scratchString);
        {
            bslma::TestAllocatorMonitor tam(&sa);
            ASSERTV(X.type(), JsonType::e_STRING == X.type());
            const bsl::string& string = X.theString();
            ASSERTV(string, scratchString, string == scratchString);
            ASSERTV(tam.isTotalSame());
        }

        ASSERTV(da.numBytesTotal(), 0 == da.numBytesTotal());
      } break;
      case 29: {
        // --------------------------------------------------------------------
        // JSON VALUE CTOR
        //   Ensure that we can put an object into any initial state relevant
        //   for thorough testing.
        //
        // Concerns:
        //: 1 The value constructor (with or without a supplied allocator) can
        //:   create an object having any value that does not violate the
        //:   constructor's documented preconditions.
        //:
        //: 2 Any string arguments can be of type 'char *' or 'string'.
        //:
        //: 3 Any argument can be 'const'.
        //:
        //: 4 If an allocator is NOT supplied to the value constructor, the
        //:   default allocator in effect at the time of construction becomes
        //:   the object allocator for the resulting object.
        //:
        //: 5 If an allocator IS supplied to the value constructor, that
        //:   allocator becomes the object allocator for the resulting object.
        //:
        //: 6 Supplying a null pointer to allocator has the same effect as not
        //:   supplying an allocator.
        //:
        //: 7 Supplying an allocator to the value constructor has no effect
        //:   on subsequent object values.
        //:
        //: 8 Any memory allocation is from the object allocator.
        //:
        //: 9 There is no temporary memory allocation from any allocator.
        //:
        //:10 Every object releases any allocated memory at destruction.
        //:
        //:11 QoI: Creating an object having the default-constructed value
        //:   allocates no memory.
        //:
        //:12 Any memory allocation is exception neutral.
        //:
        //:13 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Using a template-driven technique:
        //:
        //:   1 Define template specializations providing the following for all
        //:     types that a value constructor takes as a parameter (note that
        //:     all value constructors take a single value parameter and an
        //:     optional allocator parameter).
        //:
        //:     1 A method 'getValue()' that returns a value for primitive
        //:       types, and a 'const' reference to a long-lived object for
        //:       object types.
        //:
        //:     2 A method 'getType()' that returns the 'JsonType::Enum' value
        //:       that is expected from the constructor taking the templated
        //:       type.
        //:
        //:     3 A method 'exractFromJson()' that extracts a value from the
        //:       constructed 'Json' object to be compared with the value used
        //:       to construct it.
        //:
        //: 2 For each of three possible allocator configurations (no allocator
        //:   parameter, null pointer, and pointer to explicit allocator):
        //:
        //:   1 For each possible first parameter type of a value constructor:
        //:
        //:     1 Create three 'bslma::TestAllocator' objects, and install one
        //:       as the current default allocator.
        //:
        //:     2 Use the value constructor to dynamically create an object
        //:       having the value from P-1.1.1 ('const' references for object
        //:       types), with it's object allocator defined per P-2.
        //:
        //:     3 For types that are not expected to allocate, use the object
        //:       allocator to verify that no allocation has taken place.
        //:
        //:     4 For value constructors that move from their parameter, first
        //:       create an object of the tested type to be moved from.  The
        //:       object to be moved from must have the same object allocator
        //:       as the object being constructed.  Then create a
        //:       'bslma::TestAllocatorMonitor' attached to the object
        //:       allocator.  After the constructor that moves the value is
        //:       complete, use the 'bslma::TestAllocatorMonitor' to verify
        //:       that all allocations from the object allocator took place
        //:       before the constructor.
        //:
        //:     5 Use the (as yet unproven) salient attribute accessors from
        //:       P-1.1.2 and P-1.1.3 to verify that the object has the
        //:       expected value.
        //:
        //:     6 Verify that the footprint allocator allocated the expected
        //:       number of bytes.
        //:
        //:     7 Verify that the constructed Json object's allocator is as
        //:       expected by calling 'allocator()'.
        //:
        //:     8 Verify that the unused allocator (not the object allocator,
        //:       and not the footprint allocator) allocated no memory.
        //:
        //:     9 Verify that the object allocated did not allocate any
        //:       temporary memory by comparing total block with blocks in use.
        //:
        //:     10Delete the constructed object, and verify that all object
        //:       memory is released.
        //:
        //: 2 Testing with injected exceptions (TODO).
        //
        // Testing:
        //   Json(const JsonArray &array, *a);
        //   Json(bslmf::MovableRef<JsonArray> array, *a);
        //   Json(bool boolean, *a);
        //   Json(const JsonNull& null, *a);
        //   Json(float number, *a);
        //   Json(double number, *a);
        //   Json(bdldfp::Decimal64 number, *a);
        //   Json(int number, *a);
        //   Json(unsigned int number, *a);
        //   Json(bsls::Types::Int64 number, *a);
        //   Json(bsls::Types::Uint64 number, *a);
        //   Json(const JsonNumber& number, *a);
        //   Json(bslmf::MovableRef<JsonNumber> number, *a);
        //   Json(const JsonObject& object, *a);
        //   Json(bslmf::MovableRef<JsonObject> object, *a);
        //   Json(const char *string, *a);
        //   Json(const bsl::string_view& string, *a);
        //   Json(STRING&& string, *a);
        // --------------------------------------------------------------------

        typedef bdljsn::Json Obj;

        if (verbose) cout << endl << "JSON VALUE CTOR" << endl
                                  << "===============" << endl;

        for (int allocCfg = 0; allocCfg < 3; ++allocCfg) {
            for (char cfg = 'a'; cfg <= 's'; ++cfg) {
                const int  ALLOC_CONFIG = allocCfg;
                const char CONFIG       = cfg;

                if (veryVerbose) { T_ T_ P(CONFIG) };

                bslma::TestAllocator da("default",   veryVeryVeryVerbose);
                bslma::TestAllocator fa("footprint", veryVeryVeryVerbose);
                bslma::TestAllocator sa("supplied",  veryVeryVeryVerbose);

                bslma::DefaultAllocatorGuard dag(&da);

                Obj                  *objPtr = 0;
                bslma::TestAllocator *objAllocatorPtr = 0;

                switch (CONFIG) {
                  case 'a': {  //   Json(const JsonArray &array, *a);
                    typedef JsonArray TheType;
                    typedef JsonValueConstructorHelper<TheType> Helper;
                    if (0 == ALLOC_CONFIG) {
                        objAllocatorPtr = &da;
                        objPtr = new (fa) Obj(Helper::getValue());
                    } else if (1 == ALLOC_CONFIG) {
                        objAllocatorPtr = &da;
                        objPtr = new (fa) Obj(Helper::getValue(), 0);
                    } else if (2 == ALLOC_CONFIG) {
                        objAllocatorPtr = &sa;
                        objPtr = new (fa) Obj(Helper::getValue(), &sa);
                    } else {
                        BSLS_ASSERT_OPT(!"Bad allocator config.");
                    }
                    ASSERTV(ALLOC_CONFIG,
                            CONFIG,
                            Helper::getType(),
                            objPtr->type(),
                            Helper::getType() == objPtr->type());
                    ASSERTV(ALLOC_CONFIG,
                            CONFIG,
                            Helper::extractFromJson(*objPtr,
                                                     ALLOC_CONFIG,
                                                     CONFIG),
                            Helper::getValue(),
                            Helper::extractFromJson(*objPtr,
                                                     ALLOC_CONFIG,
                                                     CONFIG)
                            == Helper::getValue());
                  } break;
                  case 'b': {  // Json(bslmf::MovableRef<JsonArray> array, *a);
                    typedef JsonArray TheType;
                    typedef JsonValueConstructorHelper<TheType> Helper;
                    if (0 == ALLOC_CONFIG) {
                        objAllocatorPtr = &da;
                        TheType from(Helper::getValue(), objAllocatorPtr);

                        bslma::TestAllocatorMonitor tam(objAllocatorPtr);
                        objPtr = new (fa) Obj(
                                            bslmf::MovableRefUtil::move(from));
                        ASSERTV(CONFIG, tam.isTotalSame());
                    } else if (1 == ALLOC_CONFIG) {
                        objAllocatorPtr = &da;
                        TheType from(Helper::getValue(), objAllocatorPtr);

                        bslma::TestAllocatorMonitor tam(objAllocatorPtr);
                        objPtr = new (fa) Obj(
                                             bslmf::MovableRefUtil::move(from),
                                             0);
                        ASSERTV(CONFIG, tam.isTotalSame());
                    } else if (2 == ALLOC_CONFIG) {
                        objAllocatorPtr = &sa;
                        TheType from(Helper::getValue(), objAllocatorPtr);

                        bslma::TestAllocatorMonitor tam(objAllocatorPtr);
                        objPtr = new (fa) Obj(
                                             bslmf::MovableRefUtil::move(from),
                                             &sa);
                        ASSERTV(CONFIG, tam.isTotalSame());
                    } else {
                        BSLS_ASSERT_OPT(!"Bad allocator config.");
                    }
                    ASSERTV(ALLOC_CONFIG,
                            CONFIG,
                            Helper::getType(),
                            objPtr->type(),
                            Helper::getType() == objPtr->type());
                    ASSERTV(ALLOC_CONFIG,
                            CONFIG,
                            Helper::extractFromJson(*objPtr,
                                                     ALLOC_CONFIG,
                                                     CONFIG),
                            Helper::getValue(),
                            Helper::extractFromJson(*objPtr,
                                                     ALLOC_CONFIG,
                                                     CONFIG)
                            == Helper::getValue());
                  } break;
                  case 'c': {  //   Json(bool boolean, *a);
                    typedef bool TheType;
                    typedef JsonValueConstructorHelper<TheType> Helper;
                    if (0 == ALLOC_CONFIG) {
                        objAllocatorPtr = &da;
                        objPtr = new (fa) Obj(Helper::getValue());
                    } else if (1 == ALLOC_CONFIG) {
                        objAllocatorPtr = &da;
                        objPtr = new (fa) Obj(Helper::getValue(), 0);
                    } else if (2 == ALLOC_CONFIG) {
                        objAllocatorPtr = &sa;
                        objPtr = new (fa) Obj(Helper::getValue(), &sa);
                    } else {
                        BSLS_ASSERT_OPT(!"Bad allocator config.");
                    }
                    ASSERTV(ALLOC_CONFIG,
                            CONFIG,
                            0 == objAllocatorPtr->numBlocksTotal());
                    ASSERTV(ALLOC_CONFIG,
                            CONFIG,
                            Helper::getType(),
                            objPtr->type(),
                            Helper::getType() == objPtr->type());
                    ASSERTV(ALLOC_CONFIG,
                            CONFIG,
                            Helper::extractFromJson(*objPtr,
                                                     ALLOC_CONFIG,
                                                     CONFIG),
                            Helper::getValue(),
                            Helper::extractFromJson(*objPtr,
                                                     ALLOC_CONFIG,
                                                     CONFIG)
                            == Helper::getValue());
                  } break;
                  case 'd': {  //   Json(const JsonNull& null, *a);
                    typedef JsonNull TheType;
                    typedef JsonValueConstructorHelper<TheType> Helper;
                    if (0 == ALLOC_CONFIG) {
                        objAllocatorPtr = &da;
                        objPtr = new (fa) Obj(Helper::getValue());
                    } else if (1 == ALLOC_CONFIG) {
                        objAllocatorPtr = &da;
                        objPtr = new (fa) Obj(Helper::getValue(), 0);
                    } else if (2 == ALLOC_CONFIG) {
                        objAllocatorPtr = &sa;
                        objPtr = new (fa) Obj(Helper::getValue(), &sa);
                    } else {
                        BSLS_ASSERT_OPT(!"Bad allocator config.");
                    }
                    ASSERTV(ALLOC_CONFIG,
                            CONFIG,
                            0 == objAllocatorPtr->numBlocksTotal());
                    ASSERTV(ALLOC_CONFIG,
                            CONFIG,
                            Helper::getType(),
                            objPtr->type(),
                            Helper::getType() == objPtr->type());
                  } break;
                  case 'e': {  //   Json(float number, *a);
                    typedef float TheType;
                    typedef JsonValueConstructorHelper<TheType> Helper;
                    if (0 == ALLOC_CONFIG) {
                        objAllocatorPtr = &da;
                        objPtr = new (fa) Obj(Helper::getValue());
                    } else if (1 == ALLOC_CONFIG) {
                        objAllocatorPtr = &da;
                        objPtr = new (fa) Obj(Helper::getValue(), 0);
                    } else if (2 == ALLOC_CONFIG) {
                        objAllocatorPtr = &sa;
                        objPtr = new (fa) Obj(Helper::getValue(), &sa);
                    } else {
                        BSLS_ASSERT_OPT(!"Bad allocator config.");
                    }
                    ASSERTV(ALLOC_CONFIG,
                            CONFIG,
                            0 == objAllocatorPtr->numBlocksTotal());
                    ASSERTV(ALLOC_CONFIG,
                            CONFIG,
                            Helper::getType(),
                            objPtr->type(),
                            Helper::getType() == objPtr->type());
                    ASSERTV(ALLOC_CONFIG,
                            CONFIG,
                            Helper::extractFromJson(*objPtr,
                                                     ALLOC_CONFIG,
                                                     CONFIG),
                            Helper::getValue(),
                            Helper::extractFromJson(*objPtr,
                                                     ALLOC_CONFIG,
                                                     CONFIG)
                            == Helper::getValue());
                  } break;
                  case 'f': {  //   Json(double number, *a);
                    typedef double TheType;
                    typedef JsonValueConstructorHelper<TheType> Helper;
                    if (0 == ALLOC_CONFIG) {
                        objAllocatorPtr = &da;
                        objPtr = new (fa) Obj(Helper::getValue());
                    } else if (1 == ALLOC_CONFIG) {
                        objAllocatorPtr = &da;
                        objPtr = new (fa) Obj(Helper::getValue(), 0);
                    } else if (2 == ALLOC_CONFIG) {
                        objAllocatorPtr = &sa;
                        objPtr = new (fa) Obj(Helper::getValue(), &sa);
                    } else {
                        BSLS_ASSERT_OPT(!"Bad allocator config.");
                    }
                    ASSERTV(ALLOC_CONFIG,
                            CONFIG,
                            0 == objAllocatorPtr->numBlocksTotal());
                    ASSERTV(ALLOC_CONFIG,
                            CONFIG,
                            Helper::getType(),
                            objPtr->type(),
                            Helper::getType() == objPtr->type());
                    ASSERTV(ALLOC_CONFIG,
                            CONFIG,
                            Helper::extractFromJson(*objPtr,
                                                     ALLOC_CONFIG,
                                                     CONFIG),
                            Helper::getValue(),
                            Helper::extractFromJson(*objPtr,
                                                     ALLOC_CONFIG,
                                                     CONFIG)
                            == Helper::getValue());
                  } break;
                  case 'g': {  //   Json(bdldfp::Decimal64 number, *a);
                    typedef bdldfp::Decimal64 TheType;
                    typedef JsonValueConstructorHelper<TheType> Helper;
                    if (0 == ALLOC_CONFIG) {
                        objAllocatorPtr = &da;
                        objPtr = new (fa) Obj(Helper::getValue());
                    } else if (1 == ALLOC_CONFIG) {
                        objAllocatorPtr = &da;
                        objPtr = new (fa) Obj(Helper::getValue(), 0);
                    } else if (2 == ALLOC_CONFIG) {
                        objAllocatorPtr = &sa;
                        objPtr = new (fa) Obj(Helper::getValue(), &sa);
                    } else {
                        BSLS_ASSERT_OPT(!"Bad allocator config.");
                    }
                    ASSERTV(ALLOC_CONFIG,
                            CONFIG,
                            0 == objAllocatorPtr->numBlocksTotal());
                    ASSERTV(ALLOC_CONFIG,
                            CONFIG,
                            Helper::getType(),
                            objPtr->type(),
                            Helper::getType() == objPtr->type());
                    ASSERTV(ALLOC_CONFIG,
                            CONFIG,
                            Helper::extractFromJson(*objPtr,
                                                     ALLOC_CONFIG,
                                                     CONFIG),
                            Helper::getValue(),
                            Helper::extractFromJson(*objPtr,
                                                     ALLOC_CONFIG,
                                                     CONFIG)
                            == Helper::getValue());
                  } break;
                  case 'h': {  //   Json(int number, *a);
                    typedef int TheType;
                    typedef JsonValueConstructorHelper<TheType> Helper;
                    if (0 == ALLOC_CONFIG) {
                        objAllocatorPtr = &da;
                        objPtr = new (fa) Obj(Helper::getValue());
                    } else if (1 == ALLOC_CONFIG) {
                        objAllocatorPtr = &da;
                        objPtr = new (fa) Obj(Helper::getValue(), 0);
                    } else if (2 == ALLOC_CONFIG) {
                        objAllocatorPtr = &sa;
                        objPtr = new (fa) Obj(Helper::getValue(), &sa);
                    } else {
                        BSLS_ASSERT_OPT(!"Bad allocator config.");
                    }
                    ASSERTV(ALLOC_CONFIG,
                            CONFIG,
                            0 == objAllocatorPtr->numBlocksTotal());
                    ASSERTV(ALLOC_CONFIG,
                            CONFIG,
                            Helper::getType(),
                            objPtr->type(),
                            Helper::getType() == objPtr->type());
                    ASSERTV(ALLOC_CONFIG,
                            CONFIG,
                            Helper::extractFromJson(*objPtr,
                                                     ALLOC_CONFIG,
                                                     CONFIG),
                            Helper::getValue(),
                            Helper::extractFromJson(*objPtr,
                                                     ALLOC_CONFIG,
                                                     CONFIG)
                            == Helper::getValue());
                  } break;
                  case 'i': {  //   Json(unsigned int number, *a);
                    typedef unsigned int TheType;
                    typedef JsonValueConstructorHelper<TheType> Helper;
                    if (0 == ALLOC_CONFIG) {
                        objAllocatorPtr = &da;
                        objPtr = new (fa) Obj(Helper::getValue());
                    } else if (1 == ALLOC_CONFIG) {
                        objAllocatorPtr = &da;
                        objPtr = new (fa) Obj(Helper::getValue(), 0);
                    } else if (2 == ALLOC_CONFIG) {
                        objAllocatorPtr = &sa;
                        objPtr = new (fa) Obj(Helper::getValue(), &sa);
                    } else {
                        BSLS_ASSERT_OPT(!"Bad allocator config.");
                    }
                    ASSERTV(ALLOC_CONFIG,
                            CONFIG,
                            0 == objAllocatorPtr->numBlocksTotal());
                    ASSERTV(ALLOC_CONFIG,
                            CONFIG,
                            Helper::getType(),
                            objPtr->type(),
                            Helper::getType() == objPtr->type());
                    ASSERTV(ALLOC_CONFIG,
                            CONFIG,
                            Helper::extractFromJson(*objPtr,
                                                     ALLOC_CONFIG,
                                                     CONFIG),
                            Helper::getValue(),
                            Helper::extractFromJson(*objPtr,
                                                     ALLOC_CONFIG,
                                                     CONFIG)
                            == Helper::getValue());
                  } break;
                  case 'j': {  //   Json(bsls::Types::Int64 number, *a);
                    typedef bsls::Types::Int64 TheType;
                    typedef JsonValueConstructorHelper<TheType> Helper;
                    if (0 == ALLOC_CONFIG) {
                        objAllocatorPtr = &da;
                        objPtr = new (fa) Obj(Helper::getValue());
                    } else if (1 == ALLOC_CONFIG) {
                        objAllocatorPtr = &da;
                        objPtr = new (fa) Obj(Helper::getValue(), 0);
                    } else if (2 == ALLOC_CONFIG) {
                        objAllocatorPtr = &sa;
                        objPtr = new (fa) Obj(Helper::getValue(), &sa);
                    } else {
                        BSLS_ASSERT_OPT(!"Bad allocator config.");
                    }
                    ASSERTV(ALLOC_CONFIG,
                            CONFIG,
                            0 == objAllocatorPtr->numBlocksTotal());
                    ASSERTV(ALLOC_CONFIG,
                            CONFIG,
                            Helper::getType(),
                            objPtr->type(),
                            Helper::getType() == objPtr->type());
                    ASSERTV(ALLOC_CONFIG,
                            CONFIG,
                            Helper::extractFromJson(*objPtr,
                                                     ALLOC_CONFIG,
                                                     CONFIG),
                            Helper::getValue(),
                            Helper::extractFromJson(*objPtr,
                                                     ALLOC_CONFIG,
                                                     CONFIG)
                            == Helper::getValue());
                  } break;
                  case 'k': {  //   Json(bsls::Types::Uint64 number, *a);
                    typedef bsls::Types::Uint64 TheType;
                    typedef JsonValueConstructorHelper<TheType> Helper;
                    if (0 == ALLOC_CONFIG) {
                        objAllocatorPtr = &da;
                        objPtr = new (fa) Obj(Helper::getValue());
                    } else if (1 == ALLOC_CONFIG) {
                        objAllocatorPtr = &da;
                        objPtr = new (fa) Obj(Helper::getValue(), 0);
                    } else if (2 == ALLOC_CONFIG) {
                        objAllocatorPtr = &sa;
                        objPtr = new (fa) Obj(Helper::getValue(), &sa);
                    } else {
                        BSLS_ASSERT_OPT(!"Bad allocator config.");
                    }
                    ASSERTV(ALLOC_CONFIG,
                            CONFIG,
                            0 == objAllocatorPtr->numBlocksTotal());
                    ASSERTV(ALLOC_CONFIG,
                            CONFIG,
                            Helper::getType(),
                            objPtr->type(),
                            Helper::getType() == objPtr->type());
                    ASSERTV(ALLOC_CONFIG,
                            CONFIG,
                            Helper::extractFromJson(*objPtr,
                                                     ALLOC_CONFIG,
                                                     CONFIG),
                            Helper::getValue(),
                            Helper::extractFromJson(*objPtr,
                                                     ALLOC_CONFIG,
                                                     CONFIG)
                            == Helper::getValue());
                  } break;
                  case 'l': {  //   Json(const JsonNumber& number, *a);
                    typedef JsonNumber TheType;
                    typedef JsonValueConstructorHelper<TheType> Helper;
                    if (0 == ALLOC_CONFIG) {
                        objAllocatorPtr = &da;
                        objPtr = new (fa) Obj(Helper::getValue());
                    } else if (1 == ALLOC_CONFIG) {
                        objAllocatorPtr = &da;
                        objPtr = new (fa) Obj(Helper::getValue(), 0);
                    } else if (2 == ALLOC_CONFIG) {
                        objAllocatorPtr = &sa;
                        objPtr = new (fa) Obj(Helper::getValue(), &sa);
                    } else {
                        BSLS_ASSERT_OPT(!"Bad allocator config.");
                    }
                    ASSERTV(ALLOC_CONFIG,
                            CONFIG,
                            Helper::getType(),
                            objPtr->type(),
                            Helper::getType() == objPtr->type());
                    ASSERTV(ALLOC_CONFIG,
                            CONFIG,
                            Helper::extractFromJson(*objPtr,
                                                     ALLOC_CONFIG,
                                                     CONFIG),
                            Helper::getValue(),
                            Helper::extractFromJson(*objPtr,
                                                     ALLOC_CONFIG,
                                                     CONFIG)
                            == Helper::getValue());
                    ASSERTV(ALLOC_CONFIG,
                            CONFIG,
                            Helper::extractFromJson(*objPtr,
                                                     ALLOC_CONFIG,
                                                     CONFIG).value(),
                            Helper::getString(),
                            Helper::extractFromJson(*objPtr,
                                                     ALLOC_CONFIG,
                                                     CONFIG).value()
                            == Helper::getString());
                  } break;
                  case 'm': { //Json(bslmf::MovableRef<JsonNumber> number, *a);
                    typedef JsonNumber TheType;
                    typedef JsonValueConstructorHelper<TheType> Helper;
                    if (0 == ALLOC_CONFIG) {
                        objAllocatorPtr = &da;
                        TheType from(Helper::getValue(), objAllocatorPtr);

                        bslma::TestAllocatorMonitor tam(objAllocatorPtr);
                        objPtr = new (fa) Obj(
                                            bslmf::MovableRefUtil::move(from));
                        ASSERTV(CONFIG, tam.isTotalSame());
                    } else if (1 == ALLOC_CONFIG) {
                        objAllocatorPtr = &da;
                        TheType from(Helper::getValue(), objAllocatorPtr);

                        bslma::TestAllocatorMonitor tam(objAllocatorPtr);
                        objPtr = new (fa) Obj(
                                             bslmf::MovableRefUtil::move(from),
                                             0);
                        ASSERTV(CONFIG, tam.isTotalSame());
                    } else if (2 == ALLOC_CONFIG) {
                        objAllocatorPtr = &sa;
                        TheType from(Helper::getValue(), objAllocatorPtr);

                        bslma::TestAllocatorMonitor tam(objAllocatorPtr);
                        objPtr = new (fa) Obj(
                                             bslmf::MovableRefUtil::move(from),
                                             &sa);
                        ASSERTV(CONFIG, tam.isTotalSame());
                    } else {
                        BSLS_ASSERT_OPT(!"Bad allocator config.");
                    }
                    ASSERTV(ALLOC_CONFIG,
                            CONFIG,
                            Helper::getType(),
                            objPtr->type(),
                            Helper::getType() == objPtr->type());
                    ASSERTV(ALLOC_CONFIG,
                            CONFIG,
                            Helper::extractFromJson(*objPtr,
                                                     ALLOC_CONFIG,
                                                     CONFIG),
                            Helper::getValue(),
                            Helper::extractFromJson(*objPtr,
                                                     ALLOC_CONFIG,
                                                     CONFIG)
                            == Helper::getValue());
                    ASSERTV(ALLOC_CONFIG,
                            CONFIG,
                            Helper::extractFromJson(*objPtr,
                                                     ALLOC_CONFIG,
                                                     CONFIG).value(),
                            Helper::getString(),
                            Helper::extractFromJson(*objPtr,
                                                     ALLOC_CONFIG,
                                                     CONFIG).value()
                            == Helper::getString());
                  } break;
                  case 'n': {  //   Json(const JsonObject& object, *a);
                    typedef JsonObject TheType;
                    typedef JsonValueConstructorHelper<TheType> Helper;
                    if (0 == ALLOC_CONFIG) {
                        objAllocatorPtr = &da;
                        objPtr = new (fa) Obj(Helper::getValue());
                    } else if (1 == ALLOC_CONFIG) {
                        objAllocatorPtr = &da;
                        objPtr = new (fa) Obj(Helper::getValue(), 0);
                    } else if (2 == ALLOC_CONFIG) {
                        objAllocatorPtr = &sa;
                        objPtr = new (fa) Obj(Helper::getValue(), &sa);
                    } else {
                        BSLS_ASSERT_OPT(!"Bad allocator config.");
                    }
                    ASSERTV(ALLOC_CONFIG,
                            CONFIG,
                            Helper::getType(),
                            objPtr->type(),
                            Helper::getType() == objPtr->type());
                    ASSERTV(ALLOC_CONFIG,
                            CONFIG,
                            Helper::extractFromJson(*objPtr,
                                                     ALLOC_CONFIG,
                                                     CONFIG),
                            Helper::getValue(),
                            Helper::extractFromJson(*objPtr,
                                                     ALLOC_CONFIG,
                                                     CONFIG)
                            == Helper::getValue());
                  } break;
                  case 'o': { //Json(bslmf::MovableRef<JsonObject> object, *a);
                    typedef JsonObject TheType;
                    typedef JsonValueConstructorHelper<TheType> Helper;
                    if (0 == ALLOC_CONFIG) {
                        objAllocatorPtr = &da;
                        TheType from(Helper::getValue(), objAllocatorPtr);

                        bslma::TestAllocatorMonitor tam(objAllocatorPtr);
                        objPtr = new (fa) Obj(
                                            bslmf::MovableRefUtil::move(from));
                        ASSERTV(CONFIG, tam.isTotalSame());
                    } else if (1 == ALLOC_CONFIG) {
                        objAllocatorPtr = &da;
                        TheType from(Helper::getValue(), objAllocatorPtr);

                        bslma::TestAllocatorMonitor tam(objAllocatorPtr);
                        objPtr = new (fa) Obj(
                                             bslmf::MovableRefUtil::move(from),
                                             0);
                        ASSERTV(CONFIG, tam.isTotalSame());
                    } else if (2 == ALLOC_CONFIG) {
                        objAllocatorPtr = &sa;
                        TheType from(Helper::getValue(), objAllocatorPtr);

                        bslma::TestAllocatorMonitor tam(objAllocatorPtr);
                        objPtr = new (fa) Obj(
                                             bslmf::MovableRefUtil::move(from),
                                             &sa);
                        ASSERTV(CONFIG, tam.isTotalSame());
                    } else {
                        BSLS_ASSERT_OPT(!"Bad allocator config.");
                    }
                    ASSERTV(ALLOC_CONFIG,
                            CONFIG,
                            Helper::getType(),
                            objPtr->type(),
                            Helper::getType() == objPtr->type());
                    ASSERTV(ALLOC_CONFIG,
                            CONFIG,
                            Helper::extractFromJson(*objPtr,
                                                     ALLOC_CONFIG,
                                                     CONFIG),
                            Helper::getValue(),
                            Helper::extractFromJson(*objPtr,
                                                     ALLOC_CONFIG,
                                                     CONFIG)
                            == Helper::getValue());
                  } break;
                  case 'p': {  //   Json(const char *string, *a);
                    typedef const char * TheType;
                    typedef JsonValueConstructorHelper<TheType> Helper;
                    if (0 == ALLOC_CONFIG) {
                        objAllocatorPtr = &da;
                        objPtr = new (fa) Obj(Helper::getValue());
                    } else if (1 == ALLOC_CONFIG) {
                        objAllocatorPtr = &da;
                        objPtr = new (fa) Obj(Helper::getValue(), 0);
                    } else if (2 == ALLOC_CONFIG) {
                        objAllocatorPtr = &sa;
                        objPtr = new (fa) Obj(Helper::getValue(), &sa);
                    } else {
                        BSLS_ASSERT_OPT(!"Bad allocator config.");
                    }
                    ASSERTV(ALLOC_CONFIG,
                            CONFIG,
                            Helper::getType(),
                            objPtr->type(),
                            Helper::getType() == objPtr->type());
                    ASSERTV(ALLOC_CONFIG,
                            CONFIG,
                            Helper::extractFromJson(*objPtr,
                                                     ALLOC_CONFIG,
                                                     CONFIG),
                            Helper::getValue(),
                            Helper::extractFromJson(*objPtr,
                                                     ALLOC_CONFIG,
                                                     CONFIG)
                            == Helper::getValue());
                  } break;
                  case 'q': {  //   Json(const bsl::string_view& string, *a);
                    typedef bsl::string_view TheType;
                    typedef JsonValueConstructorHelper<TheType> Helper;
                    if (0 == ALLOC_CONFIG) {
                        objAllocatorPtr = &da;
                        objPtr = new (fa) Obj(Helper::getValue());
                    } else if (1 == ALLOC_CONFIG) {
                        objAllocatorPtr = &da;
                        objPtr = new (fa) Obj(Helper::getValue(), 0);
                    } else if (2 == ALLOC_CONFIG) {
                        objAllocatorPtr = &sa;
                        objPtr = new (fa) Obj(Helper::getValue(), &sa);
                    } else {
                        BSLS_ASSERT_OPT(!"Bad allocator config.");
                    }
                    ASSERTV(ALLOC_CONFIG,
                            CONFIG,
                            Helper::getType(),
                            objPtr->type(),
                            Helper::getType() == objPtr->type());
                    ASSERTV(ALLOC_CONFIG,
                            CONFIG,
                            Helper::extractFromJson(*objPtr,
                                                     ALLOC_CONFIG,
                                                     CONFIG),
                            Helper::getValue(),
                            Helper::extractFromJson(*objPtr,
                                                     ALLOC_CONFIG,
                                                     CONFIG)
                            == Helper::getValue());
                  } break;
                  case 'r': {  //   Json(STRING&& string, *a); copy
                    typedef bsl::string TheType;
                    typedef JsonValueConstructorHelper<TheType> Helper;
                    if (0 == ALLOC_CONFIG) {
                        objAllocatorPtr = &da;
                        objPtr = new (fa) Obj(Helper::getValue());
                    } else if (1 == ALLOC_CONFIG) {
                        objAllocatorPtr = &da;
                        objPtr = new (fa) Obj(Helper::getValue(), 0);
                    } else if (2 == ALLOC_CONFIG) {
                        objAllocatorPtr = &sa;
                        objPtr = new (fa) Obj(Helper::getValue(), &sa);
                    } else {
                        BSLS_ASSERT_OPT(!"Bad allocator config.");
                    }
                    ASSERTV(ALLOC_CONFIG,
                            CONFIG,
                            Helper::getType(),
                            objPtr->type(),
                            Helper::getType() == objPtr->type());
                    ASSERTV(ALLOC_CONFIG,
                            CONFIG,
                            Helper::extractFromJson(*objPtr,
                                                     ALLOC_CONFIG,
                                                     CONFIG),
                            Helper::getValue(),
                            Helper::extractFromJson(*objPtr,
                                                     ALLOC_CONFIG,
                                                     CONFIG)
                            == Helper::getValue());
                  } break;
                  case 's': {  //   Json(STRING&& string, *a); move
                    typedef bsl::string TheType;
                    typedef JsonValueConstructorHelper<TheType> Helper;
                    if (0 == ALLOC_CONFIG) {
                        objAllocatorPtr = &da;
                        TheType from(Helper::getValue(), objAllocatorPtr);

                        bslma::TestAllocatorMonitor tam(objAllocatorPtr);
                        objPtr = new (fa) Obj(
                                            bslmf::MovableRefUtil::move(from));
                        ASSERTV(CONFIG, tam.isTotalSame());
                    } else if (1 == ALLOC_CONFIG) {
                        objAllocatorPtr = &da;
                        TheType from(Helper::getValue(), objAllocatorPtr);

                        bslma::TestAllocatorMonitor tam(objAllocatorPtr);
                        objPtr = new (fa) Obj(
                                         bslmf::MovableRefUtil::move(from), 0);
                        ASSERTV(CONFIG, tam.isTotalSame());
                    } else if (2 == ALLOC_CONFIG) {
                        objAllocatorPtr = &sa;
                        TheType from(Helper::getValue(), objAllocatorPtr);

                        bslma::TestAllocatorMonitor tam(objAllocatorPtr);
                        objPtr = new (fa) Obj(
                                             bslmf::MovableRefUtil::move(from),
                                             &sa);
                        ASSERTV(CONFIG, tam.isTotalSame());
                    } else {
                        BSLS_ASSERT_OPT(!"Bad allocator config.");
                    }
                    ASSERTV(ALLOC_CONFIG,
                            CONFIG,
                            Helper::getType(),
                            objPtr->type(),
                            Helper::getType() == objPtr->type());
                    ASSERTV(ALLOC_CONFIG,
                            CONFIG,
                            Helper::extractFromJson(*objPtr,
                                                     ALLOC_CONFIG,
                                                     CONFIG),
                            Helper::getValue(),
                            Helper::extractFromJson(*objPtr,
                                                     ALLOC_CONFIG,
                                                     CONFIG)
                            == Helper::getValue());
                  } break;
                  default: {
                    BSLS_ASSERT_OPT(!"Bad constructor config.");
                  } break;
                }

                ASSERTV(CONFIG,
                        sizeof(Obj),
                        fa.numBytesInUse(),
                        sizeof(Obj) == fa.numBytesInUse());
                ASSERTV(CONFIG,
                        sizeof(Obj),
                        fa.numBytesTotal(),
                        sizeof(Obj) == fa.numBytesTotal());

                Obj& mX = *objPtr;  const Obj& X = mX;

                bslma::TestAllocator&  oa = *objAllocatorPtr;
                bslma::TestAllocator& noa = (&da == &oa) ? sa : da;

                ASSERTV(CONFIG, &oa, X.allocator());

                // Verify no allocation from the non-object allocator.
                ASSERTV(CONFIG,
                        noa.numBlocksTotal(),
                        0 == noa.numBlocksTotal());

                // Verify no temporary memory is allocated from the object
                // allocator.
                 ASSERTV(CONFIG,
                         oa.numBlocksTotal(),
                         oa.numBlocksInUse(),
                         oa.numBlocksTotal() == oa.numBlocksInUse());

                 // Reclaim dynamically allocated object under test.

                 fa.deleteObject(objPtr);

                 // Verify all memory is released on object destruction.

                 ASSERTV(CONFIG,
                         da.numBlocksInUse(),
                         0 == da.numBlocksInUse());
                 ASSERTV(CONFIG,
                         fa.numBlocksInUse(),
                         0 == fa.numBlocksInUse());
                 ASSERTV(CONFIG,
                         sa.numBlocksInUse(),
                         0 == sa.numBlocksInUse());
              }
          }
      } break;
      case 28: {
        // --------------------------------------------------------------------
        // JSON DEFAULT CTOR, PRIMARY MANIPULATORS, & DTOR
        //   Ensure that we can use the default constructor of Json to
        //   create an object (having the default constructed value), use the
        //   primary manipulators to put that object into any state relevant
        //   for thorough testing, and use the destructor to destroy it safely.
        //
        // Concerns:
        //: 1 An object created with the default constructor (with or without a
        //:   supplied allocator) has the contractually specified default
        //:   value.
        //:
        //: 2 If an allocator is NOT supplied to the default constructor, the
        //:   default allocator in effect at the time of construction becomes
        //:   the object allocator for the resulting object.
        //:
        //: 3 If an allocator IS supplied to the default constructor, that
        //:   allocator becomes the object allocator for the resulting object.
        //:
        //: 4 Supplying a default-constructed allocator has the same effect as
        //:   not supplying an allocator.
        //:
        //: 5 Supplying an allocator to the default constructor has no effect
        //:   on subsequent object values.
        //:
        //: 6 Any memory allocation is from the object allocator.
        //:
        //: 7 There is no temporary allocation from any allocator.
        //:
        //: 8 Every object releases any allocated memory at destruction.
        //:
        //: 9 QoI: The default constructor allocates no memory.
        //:
        //:10 Any memory allocation is exception neutral.
        //:
        //:11 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Execute a loop that creates an object by default-construction,
        //:   but invokes the default constructor differently in each
        //:   iteration: (a) without passing an allocator, (b) passing a
        //:   null pointer to allocator explicitly, and (c) passing the address
        //:   of an allocator distinct from the default.  For each of these
        //:   iterations: (C-1..10)
        //:
        //:   1 Create three 'bslma::TestAllocator' objects, and install one as
        //:     the default allocator.
        //:
        //:   2 Use the default constructor to dynamically create an object
        //:     'X', with its object allocator configured appropriately.  Use a
        //:     distinct test allocator for the object's footprint.
        //:
        //:   3 Use the appropriate test allocators to verify that no memory is
        //:     allocated by the default constructor.  (C-9)
        //:
        //:   4 Verify that the default constructed object has the expected
        //:     value (the monostate JsonNull type).  (C-1)
        //:
        //:   5 Use each of the primary manipulators to set the underlying type
        //:     of the object to all six possible types.  For allocator-aware
        //:     types, assert that the correct allocator is installed in the
        //:     held type.  For all types, assert that 'type' shows the correct
        //:     type enumerator. (C-2..5)
        //:
        //:   6 For primary manipulator 'makeString' (which allocates) verify
        //:     exception neutrality using
        //:     'BSLMA_TESTALLOCATOR_EXCEPTION_TEST_*' macros.  (C-10)
        //:
        //:   6 Verify that no memory is allocated from any allocator except
        //:     the constructed object's allocator.  (C-6)
        //:
        //:   7 Verify that no temporary memory is allocated from the object
        //:     allocator.  (C-7..8)
        //:
        //:   8 Verify that, in appropriate build modes, defensive checks are
        //:     triggered for invalid values (invalid 'UTF-8' string to
        //:     'makeString').  (C-11)
        //
        // Testing:
        //   Json();
        //   Json(bslma::Allocator *basicAllocator);
        //   ~Json();
        //   JsonArray& Json::makeArray();
        //   bool& Json::makeBoolean();
        //   void Json::makeNull();
        //   JsonNumber& Json::makeNumber();
        //   JsonObject& Json::makeObject();
        //   void Json::makeString(const char *string);
        // --------------------------------------------------------------------

        typedef bdljsn::Json Obj;
        if (verbose) cout << endl
            << "JSON DEFAULT CTOR, PRIMARY MANIPULATORS, & DTOR" << endl
            << "===============================================" << endl;

        for (char cfg = 'a'; cfg <= 'c'; ++cfg) {

            const char CONFIG = cfg;

            bslma::TestAllocator da("default",   veryVeryVeryVerbose);
            bslma::TestAllocator fa("footprint", veryVeryVeryVerbose);
            bslma::TestAllocator sa("supplied",  veryVeryVeryVerbose);

            bslma::DefaultAllocatorGuard dag(&da);

            Obj                  *objPtr = 0;
            bslma::TestAllocator *objAllocatorPtr = 0;

            switch (CONFIG) {
              case 'a': {
                objAllocatorPtr = &da;
                objPtr = new (fa) Obj();
              } break;
              case 'b': {
                objAllocatorPtr = &da;
                objPtr = new (fa) Obj(static_cast<bslma::Allocator*>(0));
              } break;
              case 'c': {
                objAllocatorPtr = &sa;
                objPtr = new (fa) Obj(objAllocatorPtr);
              } break;
              default: {
                BSLS_ASSERT_OPT(!"Bad allocator config.");
              } break;
            }

            Obj&                   mX = *objPtr;  const Obj& X = mX;
            bslma::TestAllocator&  oa = *objAllocatorPtr;
            bslma::TestAllocator& noa = (&da == &oa) ? sa : da;

            // -------------------------------------------------------
            // Verify any attribute allocators are installed properly.
            // -------------------------------------------------------

            ASSERTV(CONFIG, &oa, X.allocator(), &oa == X.allocator());

            // Verify no allocation from the object/non-object allocators.

            ASSERTV(CONFIG,  oa.numBlocksTotal(), 0 ==  oa.numBlocksTotal());
            ASSERTV(CONFIG, noa.numBlocksTotal(), 0 == noa.numBlocksTotal());

            // --------------------------------------------------------
            // Verify the object's default constructed attribute value.
            // --------------------------------------------------------

            ASSERTV(CONFIG,
                    JsonType::e_NULL,
                    X.type(),
                    JsonType::e_NULL == X.type());

            // Verify that all primary manipulators have intended effects.

            {
                bslma::TestAllocatorMonitor tam(&oa);

                {
                    JsonArray& array = mX.makeArray(); (void) array;
                    ASSERTV(CONFIG,
                            JsonType::e_ARRAY,
                            X.type(),
                            JsonType::e_ARRAY == X.type());
                    ASSERTV(CONFIG,
                            &oa,
                            array.allocator(),
                            &oa == array.allocator());
                }

                {
                    bool& boolean = mX.makeBoolean(); (void) boolean;
                    ASSERTV(CONFIG,
                            JsonType::e_BOOLEAN,
                            X.type(),
                            JsonType::e_BOOLEAN == X.type());
                }

                {
                    mX.makeNull();
                    ASSERTV(CONFIG,
                            JsonType::e_NULL,
                            X.type(),
                            JsonType::e_NULL == X.type());
                }

                {
                    JsonNumber& number = mX.makeNumber(); (void) number;
                    ASSERTV(CONFIG,
                            JsonType::e_NUMBER,
                            X.type(),
                            JsonType::e_NUMBER == X.type());
                    ASSERTV(CONFIG,
                            &oa,
                            number.allocator(),
                            &oa == number.allocator());
                }

                {
                    JsonObject& object = mX.makeObject(); (void) object;
                    ASSERTV(CONFIG,
                            JsonType::e_OBJECT,
                            X.type(),
                            JsonType::e_OBJECT == X.type());
                    ASSERTV(CONFIG,
                            &oa,
                            object.allocator(),
                            &oa == object.allocator());
                }

                {
                    BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                        bslma::TestAllocatorMonitor tam(&oa);
                        mX.makeString(SUFFICIENTLY_LONG_STRING);
                        ASSERTV(CONFIG, tam.isInUseUp());
#ifdef BDE_BUILD_TARGET_EXC
                        ASSERT(0 < EXCEPTION_COUNT);
#endif
                    } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                    ASSERTV(CONFIG,
                            JsonType::e_STRING,
                            X.type(),
                            JsonType::e_STRING == X.type());
                    ASSERTV(CONFIG, &oa == X.theString().get_allocator());
                }
            }

            // Reclaim dynamically allocated object under test.

            fa.deleteObject(objPtr);

            // Verify all memory is released on object destruction.

            ASSERTV(fa.numBlocksInUse(),  0 == fa.numBlocksInUse());
            ASSERTV(oa.numBlocksInUse(),  0 == oa.numBlocksInUse());
            ASSERTV(noa.numBlocksTotal(), 0 == noa.numBlocksTotal());
        }

        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bsls::AssertTestHandlerGuard hG;

            Obj obj;

            if (verbose) cout << "\tmakeString" << endl;
            {
                // Invalid utf8 literal stolen from bdlde_utf8util.t.cpp
                ASSERT_FAIL(obj.makeString("\xf0\x80\x80\x80"));
            }
        }
      } break;
      case 27: {
        // --------------------------------------------------------------------
        // JSONOBJECT ACCESSORS
        //
        // Concerns:
        //: 1 Accessor methods with wide contracts return the expected value
        //:   on search failure.
        //:
        //: 2 Accessor methods for present members return the expected member.
        //:
        //: 3 Accessor methods have appropriate signatures.
        //:
        //: 4 There is no allocation from any allocator.
        //
        // Plan:
        //: 1 Create a 'bslma::TestAllocator' object, and install it as the
        //:   default allocator.
        //:
        //: 2 Create an empty 'const' 'JsonObject' object.
        //:
        //: 3 Create a 'const' 'JsonObject' object with some known data and
        //:   invoke all accessors to access a member known to be in the
        //:   object, asserting results.  (C-1)
        //:
        //: 4 For wide contract accessors, invoke to access a member known not
        //:   to be in the object, asserting results.
        //:
        //: 5 For wide contract accessors, invoke to access a member from the
        //:   empty object, asserting results.  (C-2)
        //:
        //: 6 Use the address of each accessor method to initialize a pointer
        //:   to a function having the appropriate signature and return type.
        //:   (C-3)
        //:
        //: 7 Use the test allocator from P-1 to verify that no memory is ever
        //:   allocated from the default allocator. (C-4)
        //
        // Testing:
        //   bslma::Allocator *JsonObject::allocator() const;
        //   ConstIterator JsonObject::begin() const;
        //   ConstIterator JsonObject::cbegin() const;
        //   ConstIterator JsonObject::end() const;
        //   ConstIterator JsonObject::cend() const;
        //   ConstIterator JsonObject::find(string_view& key) const;
        //   bool JsonObject::contains(const string_view& key) const;
        //   bool JsonObject::empty() const;
        //   size_t JsonObject::size() const;
        // --------------------------------------------------------------------

        typedef JsonObject Obj;

        if (verbose) cout << "\n" "JSONOBJECT ACCESSORS" "\n"
                                  "====================" "\n";

        const Obj::Member ARRAY[] = {
            Obj::Member("bool", Json(true)),
            Obj::Member("string", Json("string")),
            Obj::Member("number", Json(1)),
            Obj::Member("null", Json())
        };

        const Obj EMPTY;
        const Obj X(ARRAY, ARRAY + sizeof(ARRAY)/sizeof(*ARRAY));

        bslma::TestAllocator         da("default",  veryVeryVeryVerbose);
        bslma::DefaultAllocatorGuard dag(&da);

        {
            ASSERT(EMPTY.begin() == EMPTY.end());
            ASSERT(EMPTY.cbegin() == EMPTY.cend());

            ASSERT(X.begin() == X.cbegin());
            ASSERT(X.end() == X.cend());

            Obj::ConstIterator iter = X.begin();
            ASSERTV(iter->first, iter->second, X[iter->first] == iter->second);

            typedef JsonObject::ConstIterator
                              (JsonObject::*NoParamReturningConstIter)() const;
            NoParamReturningConstIter funcPtr;
            funcPtr = &Obj::begin;
            funcPtr = &Obj::cbegin;
            funcPtr = &Obj::end;
            funcPtr = &Obj::cend;
            (void) funcPtr;
        }

        {
            Obj::ConstIterator iter = X.find("number");
            ASSERTV(iter->first, iter->second, iter->second == Json(1));

            ASSERT(X.find("not present") == X.end());
            ASSERT(EMPTY.find("not present") == EMPTY.end());

            typedef JsonObject::ConstIterator
                (JsonObject::*ConstIterFromStringView)(const bsl::string_view&)
                const;

            ConstIterFromStringView funcPtr = &Obj::find;
            (void) funcPtr;
        }

        {
            ASSERT(X.contains("number"));
            ASSERT(!X.contains("not present"));
            ASSERT(!EMPTY.contains("not present"));

            typedef bool
                (JsonObject::*BoolFromStringView)(const bsl::string_view&)
                const;

            BoolFromStringView funcPtr = &Obj::contains;
            (void) funcPtr;
        }

        {
            ASSERT(EMPTY.empty());
            ASSERT(!X.empty());

            typedef bool (JsonObject::*BoolNoParam)() const;
            BoolNoParam funcPtr = &Obj::empty;
            (void) funcPtr;
        }

        {
            ASSERTV(X.size(), 4 == X.size());
            ASSERTV(EMPTY.size(), 0 == EMPTY.size());

            typedef bsl::size_t (JsonObject::*SizeTNoParam)() const;
            SizeTNoParam funcPtr = &Obj::size;
            (void) funcPtr;
        }

        ASSERTV(da.numBlocksTotal(), 0 == da.numBlocksTotal());
      } break;
      case 26: {
        // --------------------------------------------------------------------
        // JSONOBJECT MANIPULATORS
        //
        // Concerns:
        //: 1 Manipulators for JsonObject that modify the object directly leave
        //:   the object in the expected state
        //:
        //:   o Initializer list assignment operator
        //:   o Five overloads of 'insert'
        //:   o Three overloads of 'erase'
        //:
        //: 2 Manipulators for JsonObject that return reference or iterator
        //:   offering modifiable access to a contained object allow the
        //:   contained object to be modified.
        //:
        //:   o 'operator[](size_t)'
        //:   o 'find(string_view)'
        //:   o 'begin()'
        //:   o 'end()'
        //:
        //: 3 Manipulators for JsonObject that modify the object directly and
        //:   also return a value return the expected value.
        //:
        //:   o Three overloads of 'erase'
        //:   o Three overloads of 'insert'
        //:
        //:
        //: 4 Manipulator 'clear()' leaves the object equal to a default
        //:   constructed object.
        //:
        //: 5 Manipulators for JsonObject do not allocate temporary memory.
        //:
        //: 6 Manipulators have the correct signature.
        //
        // Plan:
        //: 1 Use the address of each non-templated manipulator to initialize a
        //:   pointer to a member function having the appropriate signature and
        //:   return type.  (C-6)
        //:
        //: 2 Create a 'bslma::TestAllocator' and install it as the default
        //:   allocator.  Create another 'bslma::TestAllocator' dedicated to
        //:   the constant target value.
        //:
        //: 3 Construct a 'const' 'JsonObject' object 'K' with the target value
        //:   that each modification operation will be intended to produce,
        //:   using the dedicated allocator.
        //:
        //: 4 Construct a 'JsonObject' object using an initailizer-list, with
        //:   value the same as the target object 'K'.  Assert that the value
        //:   matches expectation.
        //:
        //: 5 In a for loop:
        //:
        //:   1 Create a 'JsonObject' object that requires one contained object
        //:     to be modified in order to evaluate equal to 'K'.
        //:
        //:   2 Using a switch statement, apply one of each of the manipulators
        //:     that serve to modify contained values in place, assigning the
        //:     contained object's value to that of the object in 'K'.
        //:
        //:   3 Assert that the value of the modified object matches that of
        //:     'K', and that allocations were as expected. (C-1..3)
        //:
        //: 6 In a for loop:
        //:
        //:   1 Create a 'JsonObject' object that requires one contained object
        //:     to be removed to evaluate equal to 'K'.
        //:
        //:   2 Using a switch statement, apply one of each of the manipulators
        //:     that serve to remove objects from the container, remove the
        //:     contained object that is not contained in 'K'.
        //:
        //:   3 Assert that the value of the modified object matches that of
        //:     'K', and that allocations were as expected. (C-1,3)
        //:
        //: 7 In a for loop:
        //:
        //:   1 Create a 'JsonObject' object that requires one contained object
        //:     to be added to evaluate equal to 'K'.
        //:
        //:   2 Using a switch statement, apply one of each of the manipulators
        //:     that serve to add objects to the container, add the object that
        //:     is contained in 'K'.
        //:
        //:   3 Assert that the value of the modified object matches that of
        //:     'K', and that allocations were as expected. (C-1,3)
        //:
        //: 8 Create a 'JsonObject' object that contains some elements.  Invoke
        //:   'clear' on that object, and assert that the object evaluates
        //:    equal to a default constructed object.  (C-4)
        //:
        //: 9 Use the test allocator from P-1 to verify that no memory is ever
        //:   allocated from the default allocator. (C-5)
        //
        // Testing:
        //   JsonObject(initializer_list<Member> members, *a);
        //   JsonObject::operator=(initializer_list<Member> members);
        //   Iterator JsonObject::begin();
        //   Iterator JsonObject::end();
        //   pair<Iterator, bool> JsonObject::insert(const Member& m);
        //   pair<Iterator, bool> JsonObject::insert(MovableRef<Member> m);
        //   void JsonObject::insert(INPUT_ITER first, INPUT_ITER last);
        //   void JsonObject::insert(initializer_list<Member> members);
        //   pair<Iterator, bool> JsonObject::insert(string_view key, V&& v);
        //   void JsonObject::clear();
        //   Iterator JsonObject::erase(Iterator position);
        //   Iterator JsonObject::erase(ConstIterator position);
        //   bsl::size_t JsonObject::erase(const bsl::string_view& key);
        //   Iterator JsonObject::find(const bsl::string_view& key);
        // --------------------------------------------------------------------

        typedef JsonObject Obj;

        if (verbose) cout << "\n" "JSONOBJECT MANIPULATORS" "\n"
                                  "=======================" "\n";

        {
            Json& (JsonObject::*funcPtr)(const bsl::string_view&) =
                &Obj::operator[];
            (void) funcPtr;
        }
        {
            typedef JsonObject::Iterator (JsonObject::*FuncPtr)();
            FuncPtr funcPtr;
            funcPtr = &Obj::begin;
            funcPtr = &Obj::end;
            (void) funcPtr;
        }
        {
            typedef bsl::pair<JsonObject::Iterator, bool>
                (JsonObject::*FuncPtr)(const JsonObject::Member&);

            FuncPtr funcPtr = &JsonObject::insert;
            (void) funcPtr;
        }
        {
            typedef bsl::pair<JsonObject::Iterator, bool>
                (JsonObject::*FuncPtr)(bslmf::MovableRef<JsonObject::Member>);

            FuncPtr funcPtr = &JsonObject::insert;
            (void) funcPtr;
        }
        {
            typedef void (JsonObject::*FuncPtr)();
            FuncPtr funcPtr = &JsonObject::clear;
            (void) funcPtr;
        }
        {
            typedef JsonObject::Iterator
                (JsonObject::*FuncPtr)(JsonObject::Iterator);

            FuncPtr funcPtr = &JsonObject::erase;
            (void) funcPtr;
        }
        {
            typedef JsonObject::Iterator
                (JsonObject::*FuncPtr)(JsonObject::ConstIterator);

            FuncPtr funcPtr = &JsonObject::erase;
            (void) funcPtr;
        }
        {
            typedef bsl::size_t
                (JsonObject::*FuncPtr)(const bsl::string_view&);

            FuncPtr funcPtr = &JsonObject::erase;
            (void) funcPtr;
        }
        {
            typedef JsonObject::Iterator
                (JsonObject::*FuncPtr)(const bsl::string_view&);

            FuncPtr funcPtr = &JsonObject::find;
            (void) funcPtr;
        }

        bslma::TestAllocator         ka("constant", veryVeryVeryVerbose);
        bslma::TestAllocator         da("default",  veryVeryVeryVerbose);
        bslma::DefaultAllocatorGuard dag(&da);

        const Json        GOOD(true);
        const Json        BAD(false);
        const bsl::string KEY = "bool";

        const Obj::Member GOOD_MEMBER(KEY, GOOD);
        const Obj         K(&GOOD_MEMBER, &GOOD_MEMBER + 1, &ka);

#ifdef BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS
        {
            bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);

            const Obj X({{KEY, Json(true)}}, &sa);

            ASSERTV(K, X, K == X);
            ASSERTV(da.numBlocksTotal(), 0 == da.numBlocksTotal());
            ASSERTV(sa.numBlocksTotal(), 0 <  sa.numBlocksTotal());
        }
#endif

        for (char cfg = 'a'; cfg <= 'e'; ++cfg) {
            const char CONFIG = cfg;

            bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);

            const Obj::Member TO_BE_CORRECTED(KEY, BAD);

            Obj        mX(&TO_BE_CORRECTED, &TO_BE_CORRECTED + 1, &sa);
            const Obj& X = mX;

            switch (CONFIG) {
              case 'a': {
#ifdef BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS
                if (veryVerbose) cout <<
                                       "operator=(initializer_list<Json> l)\n";
                Obj& r = (mX = {{KEY, GOOD}});
                ASSERT(&r == &mX);
#else
                if (veryVerbose) cout << "init list assignment elided.\n";
                continue;
#endif
              } break;
              case 'b': {
                if (veryVerbose) cout << "Json& operator[](string_view k);\n";
                mX[KEY] = GOOD;
              } break;
              case 'c': {
                if (veryVerbose) cout << "Iterator begin();\n";
                mX.begin()->second = GOOD;
              } break;
              case 'd': {
                if (veryVerbose) cout << "Iterator end();\n";
                JsonObject::Iterator iter = mX.begin();
                iter->second = GOOD;
                ++iter;
                ASSERT(mX.end() == iter);
              } break;
              case 'e': {
                if (veryVerbose) cout <<
                                     "Iter find(const bsl::string_view& k);\n";
                mX.find(KEY)->second = GOOD;
              } break;
              default: {
                BSLS_ASSERT_OPT(!"Bad test case config.");
              } break;
            }
            ASSERTV(CONFIG, K, X, K == X);
            ASSERTV(da.numBlocksTotal(), 0 == da.numBlocksTotal());
        }
        for (char cfg = 'f'; cfg <= 'h'; ++cfg) {
            const char CONFIG = cfg;

            bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);

            const bsl::string KEY_TO_REMOVE = "bool to remove";

            const Obj::Member ARRAY[2] = { Obj::Member(KEY, GOOD),
                                           Obj::Member(KEY_TO_REMOVE, BAD)};

            Obj mX(ARRAY, ARRAY + 2, &sa);  const Obj& X = mX;

            switch (CONFIG) {
              case 'f': {
                if (veryVerbose) cout << "Iter erase(Iter p);\n";
                const Obj::Iterator iter = mX.find(KEY_TO_REMOVE);
                const Obj::Iterator next = ++Obj::Iterator(iter);
                const Obj::Iterator r    = mX.erase(iter);
                ASSERT(next == r);
              } break;
              case 'g': {
                if (veryVerbose) cout << "Iter erase(ConstIter p);\n";
                const Obj::ConstIterator iter = mX.find(KEY_TO_REMOVE);
                const Obj::ConstIterator next = ++Obj::ConstIterator(iter);
                const Obj::Iterator      r    = mX.erase(iter);
                ASSERT(next == r);
              } break;
              case 'h': {
                if (veryVerbose) cout <<
                                    "Iter erase(const bsl::string_view& i);\n";
                const bsl::size_t r = mX.erase(KEY_TO_REMOVE);
                ASSERT(r);
                const bsl::size_t r2 = mX.erase(KEY_TO_REMOVE);
                ASSERT(!r2);
              } break;
              default: {
                BSLS_ASSERT_OPT(!"Bad test case config.");
              } break;
            }
            ASSERTV(CONFIG, K, X, K == X);
            ASSERTV(da.numBlocksTotal(), 0 == da.numBlocksTotal());
        }
        for (char cfg = 'i'; cfg <= 'n'; ++cfg) {
            const char        CONFIG = cfg;

            bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);

            Obj mX(&sa);  const Obj& X = mX;

            bool elided = false;

            switch (CONFIG) {
              case 'i': {
                if (veryVerbose) cout << "Json& operator[](string_view i);\n";
                mX[KEY] = GOOD;
              } break;
              case 'j': {
                if (veryVerbose) cout <<
                                 "pair<Iter, bool> insert(const Member& m);\n";
                const Obj::IteratorAndStatus r = mX.insert(GOOD_MEMBER);
                ASSERT(r.first == mX.begin());
                ASSERT(r.second == true);

                const Obj::IteratorAndStatus r2 = mX.insert(GOOD_MEMBER);
                ASSERT(r2.first == mX.begin());
                ASSERT(r2.second == false);
              } break;
              case 'k': {
                if (veryVerbose) cout <<
                    "pair<Iter, bool> MovableRef<Member> m);\n";
                Obj::Member                  moveFrom(GOOD_MEMBER);
                const Obj::IteratorAndStatus r = mX.insert(
                    bslmf::MovableRefUtil::move(moveFrom));
                ASSERT(r.first == mX.begin());
                ASSERT(true == r.second);

                Obj::Member                  moveFrom2(GOOD_MEMBER);
                const Obj::IteratorAndStatus r2 = mX.insert(
                    bslmf::MovableRefUtil::move(moveFrom2));
                ASSERT(r2.first == mX.begin());
                ASSERT(false == r2.second);
              } break;
              case 'l': {
                if (veryVerbose) cout << "void insert(Iter f, Iter l);\n";
                mX.insert(&GOOD_MEMBER, &GOOD_MEMBER + 1);
              } break;
              case 'm': {

#ifdef BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS
                if (veryVerbose) cout <<
                                    "void insert(initializer_list<Member> m);";
                mX.insert({{KEY, GOOD}});
#else
                if (veryVerbose) cout << "init list insertion elided.\n";
                elided = true;
#endif
              } break;
              case 'n': {
                if (veryVerbose) cout <<
                          "pair<Iter, bool> insert(string_view k, VALUE v);\n";
                const Obj::IteratorAndStatus r = mX.insert(KEY, GOOD);
                ASSERT(mX.begin() == r.first);
                ASSERT(true == r.second);

                const Obj::IteratorAndStatus r2 = mX.insert(KEY, GOOD);
                ASSERT(mX.begin() == r2.first);
                ASSERT(false == r2.second);
              } break;
              default: {
                BSLS_ASSERT_OPT(!"Bad test case config.");
              } break;
            }
            ASSERTV(CONFIG, K, X, elided, (K == X) || elided);
            ASSERTV(da.numBlocksTotal(), 0 == da.numBlocksTotal());
        }
        {
            bslma::TestAllocator sa("scratch", veryVeryVeryVerbose);

            Obj mX(&GOOD_MEMBER, &GOOD_MEMBER + 1, &sa); const Obj& X = mX;

            const Obj EMPTY;

            if (veryVerbose) cout << "clear();\n";
            mX.clear();

            ASSERT(EMPTY == X);
            ASSERTV(da.numBlocksTotal(), 0 == da.numBlocksTotal());
        }
        {
            bslma::TestAllocator oa("object", veryVeryVeryVerbose);

            TemplatedJsonObjectInsertionTest<const char *>()("const char*",
                                                             &oa);

            TemplatedJsonObjectInsertionTest<bsl::string_view>()("string_view",
                                                                 &oa);

            TemplatedJsonObjectInsertionTest<int>()(0, &oa);
            TemplatedJsonObjectInsertionTest<unsigned int>()(1, &oa);
            TemplatedJsonObjectInsertionTest<double>()(2.0, &oa);
            TemplatedJsonObjectInsertionTest<float>()(3.0, &oa);
            TemplatedJsonObjectInsertionTest<bool>()(true, &oa);

            TemplatedJsonObjectInsertionTest<bsl::string>()(
                                           bsl::string(LONG_STRING, &oa), &oa);

            TemplatedJsonObjectInsertionTest<Json>()(Json(LONG_STRING, &oa),
                                                     &oa);

            TemplatedJsonObjectInsertionTest<Json>()(
                             Json(JsonNumber(INT_LONG_STRING, &oa), &oa), &oa);

            TemplatedJsonObjectInsertionTest<Json>()(Json(true, &oa), &oa);
        }
        ASSERTV(da.numBlocksTotal(), 0 == da.numBlocksTotal());
      } break;
      case 25: {
        // --------------------------------------------------------------------
        // JSONOBJECT BSLX STREAMING
        //   Ensure that we can serialize the value of any object of the class,
        //   and then deserialize that value back into any object of the class.
        //
        // Concerns:
        //: 1 N/A
        //
        // Plan:
        //: 1 N/A
        //
        // Testing:
        //   Reserved for 'bslx' streaming.
        // --------------------------------------------------------------------

        if (verbose) cout << "JSONOBJECT BSLX STREAMING" << endl
                          << "=========================" << endl;

        if (verbose) cout << "Not yet implemented." << endl;

      } break;
      case 24: {
        // --------------------------------------------------------------------
        // JSONOBJECT MOVE-ASSIGNMENT OPERATOR
        //   Ensure that we can move the value of any object of the class to
        //   any object of the class, such that the target object subsequently
        //   has the source value, and there are no additional allocations if
        //   only one allocator is being used, and the source object is
        //   unchanged if allocators are different.
        //
        // Concerns:
        //: 1 The move assignment operator can change the value of any
        //:   modifiable target object to that of any source object.
        //:
        //: 2 The allocator used by the target object is unchanged.
        //:
        //: 3 Any memory allocation is from the target object's allocator.
        //:
        //: 4 The signature and return type are standard.
        //:
        //: 5 The reference returned is to the target object (i.e., '*this').
        //:
        //: 6 If the allocators are different, the value of the source object
        //:   is not modified.
        //:
        //: 7 If the allocators are the same, no new allocations happen when
        //:   the move assignment happens.
        //:
        //: 8 The allocator used by the source object is unchanged.
        //:
        //: 9 Any memory allocation is exception neutral.
        //:
        //:10 Assigning an object to itself behaves as expected (alias-safety).
        //:
        //:11 Every object releases any allocated memory at destruction.
        //
        // Plan:
        //: 1 Use the address of 'operator=' to initialize a member-function
        //:   pointer having the appropriate signature and return type for the
        //:   copy-assignment operator defined in this component.  (C-4)
        //:
        //: 2 Create a 'bslma::TestAllocator' object, and install it as the
        //:   default allocator (note that a ubiquitous test allocator is
        //:   already installed as the global allocator).  Create a separate
        //:   'bslma::TestAllocator' object for arrays needed to initialize
        //:   objects under test.
        //:
        //: 3 Using the table-driven technique:
        //:
        //:   1 Specify a set of (unique) valid object values (one per row) in
        //:     terms of three different Json types, including values that
        //:     should require allocation where applicable.
        //:
        //:   2 Additionally, provide a (tri-valued) column, 'MEM', indicating
        //:     the expectation of memory allocation for all typical
        //:     implementations of individual attribute types: ('Y') "Yes",
        //:     ('N') "No", or ('?') "implementation-dependent".
        //:
        //: 4 For each row 'R1' (representing a distinct object value, 'V') in
        //:   the table described in P-3:  (C-1..3, 5-6,8-11)
        //:
        //:   1 Use the value constructor and a "scratch" allocator to create
        //:     two 'const' 'Obj', 'Z' and 'ZZ', each having the value 'V'.
        //:
        //:   2 Execute an inner loop that iterates over each row 'R2'
        //:     (representing a distinct object value, 'W') in the table
        //:     described in P-3:
        //:
        //:   3 For each of the iterations (P-4.2):  (C-1..2, 5..8, 11)
        //:
        //:     1 Create a 'bslma::TestAllocator' objects 's1'.
        //:
        //:     2 Use the value constructor and 's1' to create a modifiable
        //:       'Obj', 'mF', having the value 'V'.
        //:
        //:     3 Use the value constructor and 's1' to create a modifiable
        //:       'Obj', 'mX', having the value 'W'.
        //:
        //:     4 Move-assign 'mX' from 'bslmf::MovableRefUtil::move(mF)'.
        //:
        //:     5 Verify that the address of the return value is the same as
        //:       that of 'mX'.  (C-5)
        //:
        //:     6 Use the equality-comparison operator to verify that the
        //:       target object, 'mX', now has the same value as that of 'Z'.
        //:
        //:     7 Use the 'allocator' accessor of both 'mX' and 'mF' to verify
        //:       that the respective allocators used by the target and source
        //:       objects are unchanged.  (C-2, 7)
        //:
        //:     8 Use the appropriate test allocators to verify that no new
        //:       allocations were made by the move assignment operation.
        //:
        //:   4 For each of the iterations (P-4.2):  (C-1..2, 5, 7-9, 11)
        //:
        //:     1 Create two 'bslma::TestAllocator' objects 's1' and 's2'.
        //:
        //:     2 Use the value constructor and 's1' to create a modifiable
        //:       'Obj', 'mF', having the value 'V'.
        //:
        //:     3 Use the value constructor and 's2' to create a modifiable
        //:       'Obj', 'mX', having the value 'W'.
        //:
        //:     4 Move-assign 'mX' from 'bslmf::MovableRefUtil::move(mF)'.
        //:
        //:     5 Verify that the address of the return value is the same as
        //:       that of 'mX'.  (C-5)
        //:
        //:     6 Use the equality-comparison operator to verify that the
        //:       target object, 'mX', now has the same value as that of 'Z'.
        //:
        //:     7 Use the equality-comparison operator to verify that the
        //:       source object, 'mF', now has the same value as that of 'Z'.
        //:
        //:     8 Use the 'allocator' accessor of both 'mX' and 'mF' to verify
        //:       that the respective allocators used by the target and source
        //:       objects are unchanged.  (C-2, 7)
        //:
        //: 5 Repeat steps similar to those described in P-2 except that, this
        //:   time, there is no inner loop (as in P-4.2); instead, the source
        //:   object, 'Z', is a reference to the target object, 'mX', and both
        //:   'mX' and 'ZZ' are initialized to have the value 'V'.  For each
        //:   row (representing a distinct object value, 'V') in the table
        //:   described in P-3:  (C-10)
        //:
        //:   1 Create a 'bslma::TestAllocator' object, 'oa'.
        //:
        //:   2 Use the value constructor and 'oa' to create a modifiable 'Obj'
        //:     'mX'; also use the value constructor and a distinct "scratch"
        //:     allocator to create a 'const' 'Obj' 'ZZ'.
        //:
        //:   3 Let 'Z' be a  reference to 'mX'.
        //:
        //:   4 Assign 'mX' from 'bslmf::MovableRefUtil::move(Z)'.
        //:
        //:   5 Verify that the address of the return value is the same as that
        //:     of 'mX'.
        //:
        //:   6 Use the equality-comparison operator to verify that the
        //:     target object, 'Z', still has the same value as that of 'ZZ'.
        //:     (C-10)
        //:
        //:   7 Use the 'allocator' accessor of 'mX' to verify that it is still
        //:     the object allocator.
        //:
        //:   8 Use the appropriate test allocators to verify that:
        //:
        //:     1 Any memory that is allocated is from the object allocator.
        //:
        //:     2 No additional (e.g., temporary) object memory is allocated
        //:       when assigning an object value that did NOT initially require
        //:       allocated memory.
        //:
        //:     3 All object memory is released when the object is destroyed.
        //:
        //: 6 Use the test allocator from P-2 to verify that no memory is ever
        //:   allocated from the default allocator.  (C-3)
        //
        // Testing:
        //   JsonObject::operator=(MovableRef<JsonObject> rhs);
        // --------------------------------------------------------------------

        typedef JsonObject Obj;

        if (verbose) cout << endl
                          << "JSONOBJECT MOVE-ASSIGNMENT OPERATOR" << endl
                          << "===================================" << endl;

        if (verbose) cout <<
                 "\nAssign the address of the operator to a variable." << endl;
        {
            typedef Obj& (Obj::*operatorPtr)(bslmf::MovableRef<Obj>);

            // Verify that the signature and return type are standard.

            operatorPtr operatorAssignment = &Obj::operator=;

            (void)operatorAssignment;  // quash potential compiler warning
        }

        if (verbose) cout <<
            "\nCreate a test allocator and install it as the default." << endl;

        bslma::TestAllocator         ia("initializer", veryVeryVeryVerbose);
        bslma::TestAllocator         da("default", veryVeryVeryVerbose);
        bslma::DefaultAllocatorGuard dag(&da);

        if (verbose) cout <<
           "\nUse a table of distinct object values and expected memory usage."
                                                                       << endl;
        const int              NUM_DATA        = DEFAULT_NUM_DATA;
        const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

        bool anyObjectMemoryAllocatedFlag = false;  // We later check that
                                                    // this test allocates
                                                    // some object memory.
        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int          LINE1   = DATA[ti].d_line;
            const char         MEMSRC1 = DATA[ti].d_mem;
            const bool         BOOL1     = DATA[ti].d_bool;
            const char *const  NUMBER1   = DATA[ti].d_number_p;
            const char *const  STRING1   = DATA[ti].d_string_p;

            const JsonObject::Member ARRAY1[3] = {
                JsonObject::Member(KEY0, Json(BOOL1, &ia), &ia),
                JsonObject::Member(KEY1,
                                   Json(JsonNumber(NUMBER1, &ia), &ia),
                                   &ia),
                JsonObject::Member(KEY2,
                                   Json(bsl::string(STRING1, &ia), &ia),
                                   &ia)};

            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

            const Obj Z (ARRAY1, ARRAY1 + 3, &scratch);
            const Obj ZZ(ARRAY1, ARRAY1 + 3, &scratch);

            if (veryVerbose) { T_ P_(LINE1) P_(Z) P(ZZ) }

            // move assignment with the same allocator

            for (int tj = 0; tj < NUM_DATA; ++tj) {
                const int          LINE2   = DATA[tj].d_line;
                const bool         BOOL2     = DATA[tj].d_bool;
                const char *const  NUMBER2   = DATA[tj].d_number_p;
                const char *const  STRING2   = DATA[tj].d_string_p;

                const JsonObject::Member ARRAY2[3] = {
                    JsonObject::Member(KEY0, Json(BOOL2 , &ia), &ia),
                    JsonObject::Member(KEY1,
                                       Json(JsonNumber(NUMBER2 , &ia),
                                       &ia), &ia),
                    JsonObject::Member(KEY2,
                                       Json(bsl::string(STRING2, &ia),
                                       &ia), &ia)};

                bslma::TestAllocator s1("scratch1", veryVeryVeryVerbose);

                {
                    // Test move assignment with same allocator.

                    Obj mF(ARRAY1, ARRAY1 + 3, &s1); const Obj& F=mF;
                    Obj mX(ARRAY2, ARRAY2 + 3, &s1); const Obj& X=mX;

                    if (veryVerbose) { T_ P_(LINE2) P(F) P(X) }

                    ASSERTV(LINE1, LINE2, F, X, (F == X) == (LINE1 == LINE2));

                    bslma::TestAllocatorMonitor s1m(&s1);

                    Obj *mR = &(mX = bslmf::MovableRefUtil::move(mF));
                    ASSERTV(LINE1, LINE2,  Z,   X,  Z == X);
                    ASSERTV(LINE1, LINE2, mR, &mX, mR == &mX);

                    ASSERTV(LINE1, LINE2, s1m.isTotalSame());

                    ASSERTV(LINE1, LINE2, &s1, X.allocator(),
                            &s1 == X.allocator());
                    ASSERTV(LINE1, LINE2, &s1, F.allocator(),
                            &s1 == F.allocator());

                    anyObjectMemoryAllocatedFlag |= !!s1.numBlocksInUse();
                }

                // Verify all memory is released on object destruction.

                ASSERTV(LINE1, LINE2, s1.numBlocksInUse(),
                        0 == s1.numBlocksInUse());
            }

            // move assignment with different allocators

            for (int tj = 0; tj < NUM_DATA; ++tj) {
                const int         LINE2     = DATA[tj].d_line;
                const char        MEMDST2   = DATA[tj].d_mem;
                const bool        BOOL2     = DATA[tj].d_bool;
                const char *const NUMBER2   = DATA[tj].d_number_p;
                const char *const STRING2   = DATA[tj].d_string_p;

                const Obj::Member ARRAY2[3] = {
                    Obj::Member(KEY0, Json(BOOL2 , &ia), &ia),
                    Obj::Member(KEY1,
                                Json(JsonNumber(NUMBER2 , &ia) , &ia),
                                &ia),
                    Obj::Member(KEY2,
                                Json(bsl::string(STRING2, &ia) , &ia),
                                &ia)};

                bslma::TestAllocator s1("scratch1", veryVeryVeryVerbose);
                bslma::TestAllocator s2("scratch2", veryVeryVeryVerbose);

                {
                    // Test move assignment with different allocator

                    Obj mF(ARRAY1, ARRAY1 + 3, &s1); const Obj& F=mF;
                    Obj mX(ARRAY2, ARRAY2 + 3, &s2); const Obj& X=mX;

                    if (veryVerbose) { T_ P_(LINE2) P(F) P(X) }

                    ASSERTV(LINE1, LINE2, F, X, (F == X) == (LINE1 == LINE2));


                    BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(s2) {
                        if (veryVeryVerbose) { T_ T_ Q(ExceptionTestBody) }

                        Obj *mR = &(mX = bslmf::MovableRefUtil::move(mF));
                        ASSERTV(LINE1, LINE2,  Z,   X,  Z == X);
                        ASSERTV(LINE1, LINE2, mR, &mX, mR == &mX);

                        ASSERTV(LINE1, LINE2,  Z,   F,  Z == F);

                        ASSERTV(LINE1, LINE2, &s2, X.allocator(),
                               &s2 == X.allocator());
                        ASSERTV(LINE1, LINE2, &s1, F.allocator(),
                               &s1 == F.allocator());


#ifdef BDE_BUILD_TARGET_EXC
                        if ('N' == MEMDST2 && 'Y' == MEMSRC1) {
                            ASSERTV(LINE1, LINE2, 0 < EXCEPTION_COUNT);
                        }
#endif
                    } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                    anyObjectMemoryAllocatedFlag |= !!s1.numBlocksInUse();
                }

                // Verify all memory is released on object destruction.

                ASSERTV(LINE1, LINE2, s1.numBlocksInUse(),
                        0 == s1.numBlocksInUse());
                ASSERTV(LINE1, LINE2, s2.numBlocksInUse(),
                        0 == s2.numBlocksInUse());
            }

            // self-assignment

            bslma::TestAllocator oa("object", veryVeryVeryVerbose);

            {
                bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

                Obj       mX(ARRAY1, ARRAY1 + 3, &oa);
                const Obj ZZ(ARRAY1, ARRAY1 + 3, &scratch);

                Obj& Z = mX;

                ASSERTV(LINE1, ZZ, Z, ZZ == Z);

                bslma::TestAllocatorMonitor oam(&oa);

                Obj *mR = &(mX = bslmf::MovableRefUtil::move(Z));
                ASSERTV(LINE1, ZZ,   Z, ZZ == Z);
                ASSERTV(LINE1, mR, &mX, mR == &mX);

                ASSERTV(LINE1, &oa, Z.allocator(), &oa == Z.allocator());

                ASSERTV(LINE1, oam.isTotalSame());

                ASSERTV(LINE1, da.numBlocksTotal(), 0 == da.numBlocksTotal());
            }

            // Verify all object memory is released on destruction.

            ASSERTV(LINE1, oa.numBlocksInUse(), 0 == oa.numBlocksInUse());
        }

        // Double check that some object memory was allocated.

        ASSERT(anyObjectMemoryAllocatedFlag);
      } break;
      case 23: {
        // --------------------------------------------------------------------
        // JSONOBJECT COPY-ASSIGNMENT OPERATOR
        //   Ensure that we can assign the value of any object of the class to
        //   any object of the class, such that the two objects subsequently
        //   have the same value.
        //
        // Concerns:
        //: 1 The assignment operator can change the value of any modifiable
        //:   target object to that of any source object.
        //:
        //: 2 The allocator used by the target object is unchanged.
        //:
        //: 3 Any memory allocation is from the target object's allocator.
        //:
        //: 4 The signature and return type are standard.
        //:
        //: 5 The reference returned is to the target object (i.e., '*this').
        //:
        //: 6 The value of the source object is not modified.
        //:
        //: 7 The allocator used by the source object is unchanged.
        //:
        //: 8 QoI: Assigning a source object having the default-constructed
        //:   value allocates no memory.
        //:
        //: 9 Any memory allocation is exception neutral.
        //:
        //:10 Assigning an object to itself behaves as expected (alias-safety).
        //:
        //:11 Every object releases any allocated memory at destruction.
        //
        // Plan:
        //: 1 Use the address of 'operator=' to initialize a member-function
        //:   pointer having the appropriate signature and return type for the
        //:   copy-assignment operator defined in this component.  (C-4)
        //:
        //: 2 Create a 'bslma::TestAllocator' object, and install it as the
        //:   default allocator (note that a ubiquitous test allocator is
        //:   already installed as the global allocator).  Create a separate
        //:   'bslma::TestAllocator' object for arrays needed to initialize
        //:   objects under test.
        //:
        //: 3 Using the table-driven technique:
        //:
        //:   1 Specify a set of (unique) valid object values (one per row) in
        //:     terms of three different Json types, including values that
        //:     should require allocation where applicable.
        //:
        //:   2 Additionally, provide a (tri-valued) column, 'MEM', indicating
        //:     the expectation of memory allocation for all typical
        //:     implementations of individual attribute types: ('Y') "Yes",
        //:     ('N') "No", or ('?') "implementation-dependent".
        //:
        //: 4 For each row 'R1' (representing a distinct object value, 'V') in
        //:   the table described in P-3:  (C-1..2, 5..8, 11)
        //:
        //:   1 Use the value constructor and a "scratch" allocator to create
        //:     two 'const' 'Obj', 'Z' and 'ZZ', each having the value 'V'.
        //:
        //:   2 Execute an inner loop that iterates over each row 'R2'
        //:     (representing a distinct object value, 'W') in the table
        //:     described in P-3:
        //:
        //:   3 For each of the iterations (P-4.2):  (C-1..2, 5..8, 11)
        //:
        //:     1 Create a 'bslma::TestAllocator' object, 'oa'.
        //:
        //:     2 Use the value constructor and 'oa' to create a modifiable
        //:       'Obj', 'mX', having the value 'W'.
        //:
        //:     3 Assign 'mX' from 'Z' in the presence of injected exceptions
        //:       (using the 'BSLMA_TESTALLOCATOR_EXCEPTION_TEST_*' macros).
        //:
        //:     4 Verify that the address of the return value is the same as
        //:       that of 'mX'.  (C-5)
        //:
        //:     5 Use the equality-comparison operator to verify that: (C-1, 6)
        //:
        //:       1 The target object, 'mX', now has the same value as that of
        //:         'Z'.  (C-1)
        //:
        //:       2 'Z' still has the same value as that of 'ZZ'.  (C-6)
        //:
        //:     6 Use the 'allocator' accessor of both 'mX' and 'Z' to verify
        //:       that the respective allocators used by the target and source
        //:       objects are unchanged.  (C-2, 7)
        //:
        //:     7 Use the appropriate test allocators to verify that:
        //:       (C-8, 11)
        //:
        //:       1 For an object that (a) is initialized with a value that did
        //:         NOT require memory allocation, and (b) is then assigned a
        //:         value that DID require memory allocation, the target object
        //:         DOES allocate memory from its object allocator only
        //:         (irrespective of the specific number of allocations or the
        //:         total amount of memory allocated); also cross check with
        //:         what is expected for 'mX' and 'Z'.
        //:
        //:       2 An object that is assigned a value that did NOT require
        //:         memory allocation, does NOT allocate memory from its object
        //:         allocator; also cross check with what is expected for 'Z'.
        //:
        //:       3 No additional memory is allocated by the source object.
        //:         (C-8)
        //:
        //:       4 All object memory is released when the object is destroyed.
        //:         (C-11)
        //:
        //: 5 Repeat steps similar to those described in P-2 except that, this
        //:   time, there is no inner loop (as in P-4.2); instead, the source
        //:   object, 'Z', is a reference to the target object, 'mX', and both
        //:   'mX' and 'ZZ' are initialized to have the value 'V'.  For each
        //:   row (representing a distinct object value, 'V') in the table
        //:   described in P-3:  (C-9..10)
        //:
        //:   1 Create a 'bslma::TestAllocator' object, 'oa'.
        //:
        //:   2 Use the value constructor and 'oa' to create a modifiable 'Obj'
        //:     'mX'; also use the value constructor and a distinct "scratch"
        //:     allocator to create a 'const' 'Obj' 'ZZ'.
        //:
        //:   3 Let 'Z' be a 'const' reference to 'mX'.
        //:
        //:   4 Assign 'mX' from 'Z' in the presence of injected exceptions
        //:     (using the 'BSLMA_TESTALLOCATOR_EXCEPTION_TEST_*' macros).
        //:     (C-9)
        //:
        //:   5 Verify that the address of the return value is the same as that
        //:     of 'mX'.
        //:
        //:   6 Use the equality-comparison operator to verify that the
        //:     target object, 'Z', still has the same value as that of 'ZZ'.
        //:     (C-10)
        //:
        //:   7 Use the 'allocator' accessor of 'mX' to verify that it is still
        //:     the object allocator.
        //:
        //:   8 Use the appropriate test allocators to verify that:
        //:
        //:     1 Any memory that is allocated is from the object allocator.
        //:
        //:     2 No additional (e.g., temporary) object memory is allocated
        //:       when assigning an object value that did NOT initially require
        //:       allocated memory.
        //:
        //:     3 All object memory is released when the object is destroyed.
        //:
        //: 6 Use the test allocator from P-2 to verify that no memory is ever
        //:   allocated from the default allocator.  (C-3)
        //
        // Testing:
        //   JsonObject::operator=(const JsonObject& rhs);
        //   CONCERN: All memory allocation is from the object's allocator.
        //   CONCERN: All memory allocation is exception neutral.
        //   CONCERN: Object value is independent of the object allocator.
        //   CONCERN: There is no temporary allocation from any allocator.
        // --------------------------------------------------------------------

        typedef JsonObject Obj;

        if (verbose) cout << endl
                          << "JSONOBJECT COPY-ASSIGNMENT OPERATOR" << endl
                          << "===================================" << endl;

        if (verbose) cout <<
                 "\nAssign the address of the operator to a variable." << endl;
        {
            typedef Obj& (Obj::*operatorPtr)(const Obj&);

            // Verify that the signature and return type are standard.

            operatorPtr operatorAssignment = &Obj::operator=;

            (void)operatorAssignment;  // quash potential compiler warning
        }

        if (verbose) cout <<
            "\nCreate a test allocator and install it as the default." << endl;

        bslma::TestAllocator         ia("initializer", veryVeryVeryVerbose);
        bslma::TestAllocator         da("default", veryVeryVeryVerbose);
        bslma::DefaultAllocatorGuard dag(&da);

        if (verbose) cout <<
           "\nUse a table of distinct object values and expected memory usage."
                                                                       << endl;
        const int              NUM_DATA        = DEFAULT_NUM_DATA;
        const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

        bool anyObjectMemoryAllocatedFlag = false;  // We later check that
                                                    // this test allocates
                                                    // some object memory.
        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int          LINE1   = DATA[ti].d_line;
            const char         MEMSRC1 = DATA[ti].d_mem;
            const bool         BOOL1     = DATA[ti].d_bool;
            const char *const  NUMBER1   = DATA[ti].d_number_p;
            const char *const  STRING1   = DATA[ti].d_string_p;

            const JsonObject::Member ARRAY1[3] = {
                JsonObject::Member(KEY0, Json(BOOL1, &ia), &ia),
                JsonObject::Member(KEY1,
                                   Json(JsonNumber(NUMBER1, &ia), &ia),
                                   &ia),
                JsonObject::Member(KEY2,
                                   Json(bsl::string(STRING1, &ia), &ia),
                                   &ia)};

            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

            const Obj Z (ARRAY1, ARRAY1 + 3, &scratch);
            const Obj ZZ(ARRAY1, ARRAY1 + 3, &scratch);

            if (veryVerbose) { T_ P_(LINE1) P_(Z) P(ZZ) }

            for (int tj = 0; tj < NUM_DATA; ++tj) {
                const int          LINE2   = DATA[tj].d_line;
                const char         MEMDST2 = DATA[tj].d_mem;
                const bool         BOOL2     = DATA[tj].d_bool;
                const char *const  NUMBER2   = DATA[tj].d_number_p;
                const char *const  STRING2   = DATA[tj].d_string_p;

                const JsonObject::Member ARRAY2[3] = {
                    JsonObject::Member(KEY0, Json(BOOL2 , &ia), &ia),
                    JsonObject::Member(KEY1,
                                       Json(JsonNumber(NUMBER2, &ia),
                                       &ia), &ia),
                    JsonObject::Member(KEY2,
                                       Json(bsl::string(STRING2, &ia),
                                       &ia), &ia)};

                bslma::TestAllocator oa("object", veryVeryVeryVerbose);

                {
                    Obj mX(ARRAY2, ARRAY2 + 3, &oa);  const Obj& X = mX;

                    if (veryVerbose) { T_ P_(LINE2) P(X) }

                    ASSERTV(LINE1, LINE2, Z, X, (Z == X) == (LINE1 == LINE2));

                    bslma::TestAllocatorMonitor oam(&oa), sam(&scratch);

                    BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                        if (veryVeryVerbose) { T_ T_ Q(ExceptionTestBody) }

                        Obj *mR = &(mX = Z);
                        ASSERTV(LINE1, LINE2,  Z,   X,  Z == X);
                        ASSERTV(LINE1, LINE2, mR, &mX, mR == &mX);

#ifdef BDE_BUILD_TARGET_EXC
                        if ('N' == MEMDST2 && 'Y' == MEMSRC1) {
                            ASSERTV(LINE1, LINE2, 0 < EXCEPTION_COUNT);
                        }
#endif

                    } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                    ASSERTV(LINE1, LINE2, ZZ, Z, ZZ == Z);

                    ASSERTV(LINE1, LINE2, &oa, X.allocator(),
                            &oa == X.allocator());
                    ASSERTV(LINE1, LINE2, &scratch, Z.allocator(),
                            &scratch == Z.allocator());

                    // All objects within X should have allocator 'oa'
                    ASSERTV(LINE1, LINE2, &oa, X[KEY0].allocator(),
                            &oa == X[KEY0].allocator());
                    ASSERTV(LINE1, LINE2, &oa, X[KEY1].allocator(),
                            &oa == X[KEY1].allocator());
                    ASSERTV(LINE1, LINE2, &oa, X[KEY2].allocator(),
                            &oa == X[KEY2].allocator());

                    // Record if some object memory was allocated.

                    anyObjectMemoryAllocatedFlag |= !!oa.numBlocksInUse();

                    ASSERTV(LINE1, LINE2, sam.isInUseSame());

                    ASSERTV(LINE1, LINE2, da.numBlocksTotal(),
                            0 == da.numBlocksTotal());
                }

                // Verify all memory is released on object destruction.

                ASSERTV(LINE1, LINE2, oa.numBlocksInUse(),
                        0 == oa.numBlocksInUse());
            }

            // self-assignment

            bslma::TestAllocator oa("object", veryVeryVeryVerbose);

            {
                bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

                Obj       mX(ARRAY1, ARRAY1 + 3, &oa);
                const Obj ZZ(ARRAY1, ARRAY1 + 3, &scratch);

                const Obj& Z = mX;

                ASSERTV(LINE1, ZZ, Z, ZZ == Z);

                bslma::TestAllocatorMonitor oam(&oa);

                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                    if (veryVeryVerbose) { T_ T_ Q(ExceptionTestBody) }

                    Obj *mR = &(mX = Z);
                    ASSERTV(LINE1, ZZ,   Z, ZZ == Z);
                    ASSERTV(LINE1, mR, &mX, mR == &mX);
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                ASSERTV(LINE1, &oa, Z.allocator(), &oa == Z.allocator());

                ASSERTV(LINE1, !oam.isInUseUp());

                ASSERTV(LINE1, 0 == da.numBlocksTotal());
            }

            // Verify all object memory is released on destruction.

            ASSERTV(LINE1, oa.numBlocksInUse(), 0 == oa.numBlocksInUse());
        }

        // Double check that some object memory was allocated.

        ASSERT(anyObjectMemoryAllocatedFlag);
      } break;
      case 22: {
        // --------------------------------------------------------------------
        // JSONOBJECT SWAP MEMBER AND FREE FUNCTIONS
        //   Ensure that the free 'swap' function is implemented and can
        //   exchange the values of any two objects.  Ensure that member
        //   'swap' is implemented and can exchange the values of any two
        //   objects that use the same allocator.
        //
        // Concerns:
        //: 1 Both functions exchange the values of the (two) supplied objects.
        //:
        //: 2 The common object allocator used by both objects is unchanged.
        //:
        //: 3 The member function does not allocate memory from any allocator;
        //:   nor does the free function when the two objects being swapped use
        //:   the same allocator.
        //:
        //: 4 The free function can be called with two objects that use
        //:   different allocators.
        //:
        //: 5 Both functions have standard signatures and return types.
        //:
        //: 6 Using either function to swap an object with itself does not
        //:   affect the value of the object (alias-safety).
        //:
        //: 7 The free 'swap' function is discoverable through ADL (Argument
        //:   Dependent Lookup).
        //:
        //: 8 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Use the addresses of the 'swap' member and free functions defined
        //:   in this component to initialize, respectively, member-function
        //:   and free-function pointers having the appropriate signatures and
        //:   return types.  (C-5)
        //:
        //: 2 Create a 'bslma::TestAllocator' object, and install it as the
        //:   default allocator (note that a ubiquitous test allocator is
        //:   already installed as the global allocator).  Provide another
        //:   allocator for objects from which objects under test are
        //:   initialized.
        //:
        //: 3 Using the table-driven technique:
        //:
        //:   1 Specify a set of (unique) valid object values (one per row) in
        //:     terms of three different Json types, including values that
        //:     should require allocation where applicable.
        //:
        //:   2 Additionally, provide a (tri-valued) column, 'MEM', indicating
        //:     the expectation of memory allocation for all typical
        //:     implementations of individual attribute types: ('Y') "Yes",
        //:     ('N') "No", or ('?') "implementation-dependent".
        //:
        //: 4 For each row 'R1' in the table of P-3:  (C1..2, 6)
        //:
        //:   1 Create a 'bslma::TestAllocator' object, 'oa'.
        //:
        //:   2 Use the value constructor and 'oa' to create a modifiable
        //:     'Obj', 'mW', having the value described by 'R1'; also use the
        //:     copy constructor and a "scratch" allocator to create a 'const'
        //:     'Obj' 'XX' from 'mW'.
        //:
        //:   3 Use the member and free 'swap' functions to swap the value of
        //:     'mW' with itself; verify, after each swap, that:  (C-6)
        //:
        //:     1 The value is unchanged.  (C-6)
        //:
        //:     2 The allocator used by the object is unchanged.
        //:
        //:     3 There was no additional object memory allocation.
        //:
        //:   4 For each row 'R2' in the table of P-3:  (C-1..2)
        //:
        //:     1 Use the copy constructor and 'oa' to create a modifiable
        //:       'Obj', 'mX', from 'XX' (P-4.2).
        //:
        //:     2 Use the value constructor and 'oa' to create a modifiable
        //:       'Obj', 'mY', having the value described by 'R2'; also use
        //:       the copy constructor to create, using a "scratch" allocator,
        //:       a 'const' 'Obj', 'YY', from 'Y'.
        //:
        //:     3 Use, in turn, the member and free 'swap' functions to swap
        //:       the values of 'mX' and 'mY'; verify, after each swap, that:
        //:       (C-1..2)
        //:
        //:       1 The values have been exchanged.  (C-1)
        //:
        //:       2 The common object allocator used by 'mX' and 'mY'
        //:         is unchanged in both objects.  (C-2)
        //:
        //:       3 There was no additional object memory allocation.
        //:
        //: 5 Verify that the free 'swap' function is discoverable through ADL:
        //:   (C-7)
        //:
        //:   1 Create a set of contained values, 'A', choosing values that
        //:     allocate memory.
        //:
        //:   2 Create a 'bslma::TestAllocator' object, 'oa'.
        //:
        //:   3 Use the default constructor and 'oa' to create a modifiable
        //:     'Obj' 'mX' (containing no elements); also use the copy
        //:     constructor and a "scratch" allocator to create a 'const' 'Obj'
        //:     'XX' from 'mX'.
        //:
        //:   4 Use the value constructor and 'oa' to create a modifiable 'Obj'
        //:     'mY' having the value described by the 'Ai' attributes; also
        //:     use the copy constructor and a "scratch" allocator to create a
        //:     'const' 'Obj' 'YY' from 'mY'.
        //:
        //:   5 Use the 'bslalg::SwapUtil' helper function template to swap the
        //:     values of 'mX' and 'mY', using the free 'swap' function defined
        //:     in this component, then verify that:  (C-7)
        //:
        //:     1 The values have been exchanged.
        //:
        //:     2 There was no additional object memory allocation.  (C-7)
        //:
        //: 6 Use the test allocator from P-2 to verify that no memory was
        //:   allocated from the default allocator.  (C-3)
        //:
        //: 7 Verify that free 'swap' exchanges the values of any two objects
        //:   that use different allocators.  (C-4)
        //:
        //: 8 Verify that, in appropriate build modes, defensive checks are
        //:   triggered when, using the member 'swap' function, an attempt is
        //:   made to swap objects that do not refer to the same allocator, but
        //:   not when the allocators are the same (using the
        //:   'BSLS_ASSERTTEST_*' macros).  (C-8)
        //
        // Testing:
        //   void JsonObject::swap(JsonObject& other);
        //   void swap(JsonObject& a, b);
        //   CONCERN: Precondition violations are detected when enabled.
        // --------------------------------------------------------------------

        typedef JsonObject Obj;

        if (verbose) cout << endl
                        << "JSONOBJECT SWAP MEMBER AND FREE FUNCTIONS" << endl
                        << "=========================================" << endl;

        if (verbose) cout <<
                "\nAssign the address of each function to a variable." << endl;
        {
            typedef void (Obj::*funcPtr)(Obj&);
            typedef void (*freeFuncPtr)(Obj&, Obj&);

            // Verify that the signatures and return types are standard.

            funcPtr     memberSwap = &Obj::swap;
            freeFuncPtr freeSwap   = bsl::swap;

            (void)memberSwap;  // quash potential compiler warnings
            (void)freeSwap;
        }

        if (verbose) cout <<
            "\nCreate a test allocator and install it as the default." << endl;

        bslma::TestAllocator         ia("initializer", veryVeryVeryVerbose);
        bslma::TestAllocator         da("default", veryVeryVeryVerbose);
        bslma::DefaultAllocatorGuard dag(&da);

        if (verbose) cout <<
           "\nUse a table of distinct object values and expected memory usage."
                                                                       << endl;
        const int              NUM_DATA        = DEFAULT_NUM_DATA;
        const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

        bool anyObjectMemoryAllocatedFlag = false;  // We later check that
                                                    // this test allocates
                                                    // some object memory.
        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int          LINE1     = DATA[ti].d_line;
            const bool         BOOL1     = DATA[ti].d_bool;
            const char *const  NUMBER1   = DATA[ti].d_number_p;
            const char *const  STRING1   = DATA[ti].d_string_p;

            const bsl::pair<const bsl::string, Json> ARRAY1[3] = {
                bsl::pair<const bsl::string, Json>(KEY0,
                                                        Json(BOOL1, &ia), &ia),
                bsl::pair<const bsl::string, Json>(
                                                 KEY1,
                                                 Json(JsonNumber(NUMBER1, &ia),
                                                 &ia), &ia),
                bsl::pair<const bsl::string, Json>(
                                                KEY2,
                                                Json(bsl::string(STRING1, &ia),
                                                &ia), &ia)};

            bslma::TestAllocator      oa("object",  veryVeryVeryVerbose);
            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

            Obj       mW(ARRAY1, ARRAY1+3, &oa); const Obj& W = mW;
            const Obj XX(W, &scratch);

            if (veryVerbose) { T_ P_(LINE1) P_(W) P(XX) }

            // member 'swap'
            {
                bslma::TestAllocatorMonitor oam(&oa);

                mW.swap(mW);

                ASSERTV(LINE1, XX, W, XX == W);
                ASSERTV(LINE1, &oa == W.allocator());
                ASSERTV(LINE1, oam.isTotalSame());
            }

            // free function 'swap'
            {
                bslma::TestAllocatorMonitor oam(&oa);

                swap(mW, mW);

                ASSERTV(LINE1, XX, W, XX == W);
                ASSERTV(LINE1, &oa == W.allocator());
                ASSERTV(LINE1, oam.isTotalSame());
            }

            for (int tj = 0; tj < NUM_DATA; ++tj) {
                const int          LINE2     = DATA[tj].d_line;
                const bool         BOOL2     = DATA[tj].d_bool;
                const char *const  NUMBER2   = DATA[tj].d_number_p;
                const char *const  STRING2   = DATA[tj].d_string_p;

                const bsl::pair<const bsl::string, Json> ARRAY2[3] = {
                    bsl::pair<const bsl::string, Json>(KEYS[0],
                                                       Json(BOOL2,
                                                       &ia), &ia),
                    bsl::pair<const bsl::string, Json>(
                                                 KEYS[1],
                                                 Json(JsonNumber(NUMBER2, &ia),
                                                 &ia), &ia),
                    bsl::pair<const bsl::string, Json>(
                                                KEYS[2],
                                                Json(bsl::string(STRING2, &ia),
                                                &ia), &ia)};

                Obj       mX(XX, &oa);                const Obj& X = mX;
                Obj       mY(ARRAY2, ARRAY2+3, &oa);  const Obj& Y = mY;
                const Obj YY(Y, &scratch);

                if (veryVerbose) { T_ P_(LINE2) P_(X) P_(Y) P(YY) }

                // member 'swap'
                {
                    bslma::TestAllocatorMonitor oam(&oa);

                    mX.swap(mY);

                    ASSERTV(LINE1, LINE2, YY, X, YY == X);
                    ASSERTV(LINE1, LINE2, XX, Y, XX == Y);
                    ASSERTV(LINE1, LINE2, &oa == X.allocator());
                    ASSERTV(LINE1, LINE2, oam.isTotalSame());
                }

                // free function 'swap', same allocator
                {
                    bslma::TestAllocatorMonitor oam(&oa);

                    swap(mX, mY);

                    ASSERTV(LINE1, LINE2, XX, X, XX == X);
                    ASSERTV(LINE1, LINE2, YY, Y, YY == Y);
                    ASSERTV(LINE1, LINE2, &oa == X.allocator());
                    ASSERTV(LINE1, LINE2, oam.isTotalSame());
                }
            }

            // Record if some object memory was allocated.

            anyObjectMemoryAllocatedFlag |= !!oa.numBlocksInUse();
        }

        // Check that some memory was allocated.

        ASSERTV(da.numBlocksTotal(), 0 == da.numBlocksTotal());
        ASSERT(anyObjectMemoryAllocatedFlag);

        if (verbose) cout <<
                "\nInvoke free 'swap' function in a context where ADL is used."
                                                                       << endl;
        {
            bslma::TestAllocator      oa("object",  veryVeryVeryVerbose);
            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

            const JsonObject::Member ALLOCATING_DATA[] = {
                bsl::pair<const bsl::string, Json>(
                                KEY0,
                                Json(bsl::string("a_" SUFFICIENTLY_LONG_STRING,
                                                 &scratch),
                                     &scratch),
                                &scratch),
                bsl::pair<const bsl::string, Json>(
                                    KEY1,
                                    Json(JsonNumber(INT_LONG_STRING, &scratch),
                                    &scratch), &scratch)
            };

            Obj       mX(&oa);  const Obj& X = mX;
            const Obj XX(X, &scratch);

            Obj       mY(ALLOCATING_DATA, ALLOCATING_DATA + 2, &oa);
            const Obj& Y = mY;
            const Obj YY(Y, &scratch);

            if (veryVeryVerbose) { T_ P_(X) P(Y) }

            bslma::TestAllocatorMonitor oam(&oa);

            bslalg::SwapUtil::swap(&mX, &mY);

            ASSERTV(YY, X, YY == X);
            ASSERTV(XX, Y, XX == Y);
            ASSERT(oam.isTotalSame());

            if (veryVeryVerbose) { T_ P_(X) P(Y) }
        }

        // Verify no memory is allocated from the default allocator.

        ASSERTV(da.numBlocksTotal(), 0 == da.numBlocksTotal());

        if (verbose) cout <<
            "\nFree 'swap' function with different allocators." << endl;

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int                LINE1     = DATA[ti].d_line;
            const bool               BOOL1     = DATA[ti].d_bool;
            const char *const        NUMBER1   = DATA[ti].d_number_p;
            const char *const        STRING1   = DATA[ti].d_string_p;
            const JsonObject::Member ARRAY1[3] = {
                JsonObject::Member(KEY0, Json(BOOL1)),
                JsonObject::Member(KEY1, Json(JsonNumber(NUMBER1))),
                JsonObject::Member(KEY2, Json(STRING1))};

            bslma::TestAllocator oa     ("object",  veryVeryVeryVerbose);
            bslma::TestAllocator oa2    ("object2", veryVeryVeryVerbose);
            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

            Obj XX(ARRAY1, ARRAY1+3, &scratch);

            if (veryVerbose) { T_ P_(LINE1) P(XX) }

            for (int tj = 0; tj < NUM_DATA; ++tj) {
                const int                LINE2     = DATA[tj].d_line;
                const bool               BOOL2     = DATA[tj].d_bool;
                const char *const        NUMBER2   = DATA[tj].d_number_p;
                const char *const        STRING2   = DATA[tj].d_string_p;
                const JsonObject::Member ARRAY2[3] = {
                    JsonObject::Member(KEY0, Json(BOOL2)),
                    JsonObject::Member(KEY1, Json(JsonNumber(NUMBER2))),
                    JsonObject::Member(KEY2, Json(STRING2))};

                Obj       mX(XX, &oa);                 const Obj& X = mX;
                Obj       mY(ARRAY2, ARRAY2+3, &oa2);  const Obj& Y = mY;
                const Obj YY(Y, &scratch);

                if (veryVerbose) { T_ P_(LINE2) P_(X) P_(Y) P(YY) }

                // free function 'swap', different allocator
                {
                    swap(mX, mY);

                    ASSERTV(LINE1, LINE2, YY, X, YY == X);
                    ASSERTV(LINE1, LINE2, XX, Y, XX == Y);
                    ASSERTV(LINE1, LINE2, &oa  == X.allocator());
                    ASSERTV(LINE1, LINE2, &oa2 == Y.allocator());
                }
            }
        }

#ifdef BDE_BUILD_TARGET_EXC
        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bsls::AssertTestHandlerGuard hG;

            if (verbose) cout << "\t'swap' member function" << endl;
            {
                bslma::TestAllocator oa1("object1", veryVeryVeryVerbose);
                bslma::TestAllocator oa2("object2", veryVeryVeryVerbose);

                Obj mA(&oa1);  Obj mB(&oa1);
                Obj mZ(&oa2);

                ASSERT_PASS(mA.swap(mB));
                ASSERT_FAIL(mA.swap(mZ));
            }
        }
#endif
      } break;
      case 21: {
        // --------------------------------------------------------------------
        // JSONOBJECT MOVE CONSTRUCTOR
        //   Ensure that we can create a distinct object of the class from any
        //   other one, such that the new object has the original value.
        //   Verify that if the same allocator is used there have been no new
        //   allocations, and if a different allocator is used the source
        //   object has the original value.
        //
        // Concerns:
        //: 1 The move constructor (with or without a supplied allocator)
        //:   creates an object having the same value as the original object
        //:   started with.
        //:
        //: 2 If an allocator is NOT supplied, the allocator of the new object
        //:   is the same as the original object, and no new allocations occur.
        //:
        //: 3 If an allocator is supplied that is the same as the original
        //:   object, then no new allocations occur.
        //:
        //: 4 If an allocator is supplied that is different from the original
        //:   object, then the original object's value remains unchanged.
        //:
        //: 5 Supplying a default-constructed allocator explicitly is the same
        //:   as supplying the default allocator.
        //:
        //: 6 Any memory allocation is from the object allocator.
        //:
        //: 7 There is no temporary memory allocation from any allocator.
        //:
        //: 8 Every object releases any allocated memory at destruction.
        //:
        //: 9 The allocator used by the original object is unchanged.
        //:
        //:10 Any memory allocation is exception neutral.
        //
        // Plan:
        //: 1 Using the table-driven technique:
        //:
        //:   1 Specify a set of (unique) valid object values (one per row) in
        //:     terms of three different Json types, including values that
        //:     should require allocation where applicable.
        //:
        //:   2 Additionally, provide a (tri-valued) column, 'MEM', indicating
        //:     the expectation of memory allocation for all typical
        //:     implementations of individual attribute types: ('Y') "Yes",
        //:     ('N') "No", or ('?') "implementation-dependent".
        //:
        //: 2 For each row (representing a distinct object value, 'V') in the
        //:   table described in P-1: (C-1..9)
        //:
        //:   1 Use the value constructor and a "scratch" allocator to create
        //:     two 'const' 'Obj', 'Z' and 'ZZ', each having the value 'V'.
        //:
        //:   2 Execute an inner loop that creates an object by
        //:     move-constructing from a newly created object with value V,
        //:     but invokes the move constructor differently in each
        //:     iteration: (a) using the standard single-argument move
        //:     constructor, (b) using the extended move constructor with a
        //:     default-constructed allocator argument (to use the default
        //:     allocator), (c) using the extended move constructor with the
        //:     same allocator as the moved-from object, and (d) using the
        //:     extended move constructor with a different allocator than the
        //:     moved-from object.
        //:
        //: 3 For each of these iterations (P-2.2):
        //:
        //:   1 Create four 'bslma::TestAllocator' objects, and install one as
        //:     the current default allocator (note that a ubiquitous test
        //:     allocator is already installed as the global allocator).
        //:
        //:   2 Dynamically allocate another object 'F" using the 's1'
        //:     allocator having the same value V, using a distinct allocator
        //:     for the object's footprint.
        //:
        //:   3 Dynamically allocate an object 'X' using the appropriate move
        //:     constructor to move from 'F', passing as a second argument
        //:     (a) nothing, (b) 'allocator_type()', (c) '&s1', or (d)
        //:     'allocator_type(&s2)'.
        //:
        //:   4 Record the allocator expected to be used by the new object and
        //:     how much memory it used before the move constructor.
        //:
        //:   5 Verify that space for 2 objects is used in the footprint
        //:     allocator
        //:
        //:   6 Verify that the moved-to object has the expected value 'V' by
        //:     comparing to 'Z'.
        //:
        //:   7 If the allocators of 'F' and 'X' are different verify that the
        //:     value of 'F' is still 'V'
        //:
        //:   8 If the allocators of 'F' and 'X' are the same, verify that no
        //:     extra memory was used by the move constructor.
        //:
        //:   9 Verify that no memory was used by the move constructor as
        //:     temporary memory, and no unused allocators have had any memory
        //:     used.
        //:
        //:  10 Delete both dynamically allocated objects and verify that all
        //:     temporary allocators have had all memory returned to them.
        //:
        //: 3 Test again, using the data of P-1, but this time just for the
        //:   supplied allocator configuration (P-2.2c), and create the object
        //:   as an automatic variable in the presence of injected exceptions
        //:   (using the 'BSLMA_TESTALLOCATOR_EXCEPTION_TEST_*' macros).  Do
        //:   this by creating one object with one test allocator ('s1') and
        //:   then using the move constructor with a separate test allocator
        //:   that is injecting exceptions ('s2').
        //:   (C-10)
        //
        // Testing:
        //   JsonObject(MovableRef<JsonObject> o);
        //   JsonObject(MovableRef<JsonObject> o, *a);
        // --------------------------------------------------------------------

        typedef JsonObject Obj;

        if (verbose) cout << "\n" "JSONOBJECT MOVE CONSTRUCTOR" "\n"
                                  "===========================" "\n";

        if (verbose) cout <<
          "\nUse a table of distrinct object values and expected memory usage."
                                                                       << endl;

        const int              NUM_DATA        = DEFAULT_NUM_DATA;
        const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

        if (verbose) cout <<
             "\nCreate objects with various allocator configurations." << endl;
        {
            bool anyObjectMemoryAllocatedFlag = false;  // We later check that
                                                        // this test allocates
                                                        // some object memory
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int         LINE     = DATA[ti].d_line;
                const bool        BOOL     = DATA[ti].d_bool;
                const char *const NUMBER   = DATA[ti].d_number_p;
                const char *const STRING   = DATA[ti].d_string_p;

                const bsl::pair<const bsl::string, Json> ARRAY[3] = {
                    bsl::make_pair<const bsl::string, Json>(
                        KEYS[0], Json(BOOL)),
                    bsl::make_pair<const bsl::string, Json>(
                        KEYS[1], Json(JsonNumber(NUMBER))),
                    bsl::make_pair<const bsl::string, Json>(
                        KEYS[2], Json(STRING))
                };

                bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

                const Obj Z (ARRAY, ARRAY+3, &scratch);
                const Obj ZZ(ARRAY, ARRAY+3, &scratch);

                if (veryVerbose) { T_ P_(Z) P(ZZ) }

                for (char cfg = 'a'; cfg <= 'd'; ++cfg) {
                    const char CONFIG = cfg;

                    bslma::TestAllocator da("default",   veryVeryVeryVerbose);
                    bslma::TestAllocator fa("footprint", veryVeryVeryVerbose);
                    bslma::TestAllocator s1("supplied",  veryVeryVeryVerbose);
                    bslma::TestAllocator s2("supplied2", veryVeryVeryVerbose);

                    bslma::DefaultAllocatorGuard dag(&da);

                    Obj *fromPtr = new (fa) Obj(ARRAY, ARRAY+3, &s1);
                    Obj& mF      = *fromPtr;  const Obj& F = mF;

                    bsls::Types::Int64 s1Alloc = s1.numBytesInUse();

                    Obj                  *objPtr = 0;
                    bslma::TestAllocator *objAllocatorPtr = 0;

                    switch (CONFIG) {
                      case 'a': {
                        // normal move constructor
                        objAllocatorPtr = &s1;
                        objPtr = new (fa) Obj(bslmf::MovableRefUtil::move(mF));
                      } break;
                      case 'b': {
                        // allocator move constructor, default allocator
                        objAllocatorPtr = &da;
                        objPtr = new (fa) Obj(bslmf::MovableRefUtil::move(mF),
                                              0);
                      } break;
                      case 'c': {
                        // allocator move constructor, same allocator
                        objAllocatorPtr = &s1;
                        objPtr = new (fa) Obj(bslmf::MovableRefUtil::move(mF),
                                              objAllocatorPtr);
                      } break;
                      case 'd': {
                        // allocator move constructor, unique allocator
                        objAllocatorPtr = &s2;
                        objPtr = new (fa) Obj(bslmf::MovableRefUtil::move(mF),
                                              objAllocatorPtr);
                      } break;
                      default: {
                        BSLS_ASSERT_OPT(!"Bad allocator config.");
                      } break;
                    }
                    ASSERTV(LINE, CONFIG, 2*sizeof(Obj) == fa.numBytesInUse());

                    Obj& mX = *objPtr;  const Obj& X = mX;

                    if (veryVerbose) { T_ T_ P_(CONFIG) P(mF) P(X) }

                    bslma::TestAllocator& oa = *objAllocatorPtr;

                    // Verify the value of the object.

                    ASSERTV(LINE, CONFIG,  Z, X,  Z == X);

                    if (objAllocatorPtr != F.allocator()) {
                        // If the allocators are different, verify that the
                        // value of 'fX' has not changed.

                        ASSERTV(LINE, CONFIG, Z, F, Z == F);
                    }
                    else {
                        // If the allocators are the same, verify that no new
                        // bytes were allocated by moving.

                        ASSERTV(LINE, CONFIG, s1Alloc, s1.numBytesInUse(),
                                s1Alloc == s1.numBytesInUse());
                    }

                    // -------------------------------------------------------
                    // Verify any attribute allocators are installed properly.
                    // -------------------------------------------------------

                    ASSERTV(LINE, CONFIG, &oa == X[KEY0].allocator());
                    ASSERTV(LINE, CONFIG, &oa == X[KEY1].allocator());
                    ASSERTV(LINE, CONFIG, &oa == X[KEY2].allocator());
                    ASSERTV(LINE, CONFIG, &oa == X.allocator());
                    ASSERTV(LINE, CONFIG, &scratch == Z.allocator());

                    // Verify no allocation from the non-object allocators.
                    if (objAllocatorPtr != &da) {
                        ASSERTV(LINE, CONFIG, da.numBlocksTotal(),
                                0 == da.numBlocksTotal());
                    }

                    if (objAllocatorPtr != &s2) {
                        ASSERTV(LINE, CONFIG, s2.numBlocksTotal(),
                                0 == s2.numBlocksTotal());
                    }

                    // Verify no temporary allocations from the object
                    // allocator.

                    ASSERTV(LINE, CONFIG, oa.numBlocksTotal(),
                            oa.numBlocksInUse(),
                            oa.numBlocksTotal() == oa.numBlocksInUse());

                    // Record if some object memory was allocated.

                    anyObjectMemoryAllocatedFlag |= !!oa.numBlocksInUse();

                    // Reclaim dynamically allocated objects under test.

                    fa.deleteObject(fromPtr);
                    fa.deleteObject(objPtr);

                    // Verify all memory is released on object destruction.

                    ASSERTV(LINE, CONFIG, da.numBlocksInUse(),
                            0 == da.numBlocksInUse());
                    ASSERTV(LINE, CONFIG, fa.numBlocksInUse(),
                            0 == fa.numBlocksInUse());
                    ASSERTV(LINE, CONFIG, s1.numBlocksInUse(),
                            0 == s1.numBlocksInUse());
                    ASSERTV(LINE, CONFIG, s2.numBlocksInUse(),
                            0 == s2.numBlocksInUse());
                }  // end foreach configuration
            }  // end foreach row

            // Double check that some object memory was allocated.

            ASSERT(anyObjectMemoryAllocatedFlag);
        }

        if (verbose) cout << "\nTesting with injected exceptions." << endl;
        {
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int          LINE     = DATA[ti].d_line;
                const char         MEM      = DATA[ti].d_mem;
                const bool         BOOL     = DATA[ti].d_bool;
                const char *const  NUMBER   = DATA[ti].d_number_p;
                const char *const  STRING   = DATA[ti].d_string_p;

                const bsl::pair<const bsl::string, Json> ARRAY[3] =
                {bsl::make_pair<const bsl::string, Json>(KEY0, Json(BOOL)),
                 bsl::make_pair<const bsl::string, Json>(
                                                     KEY1,
                                                     Json(JsonNumber(NUMBER))),
                 bsl::make_pair<const bsl::string, Json>(KEY2, Json(STRING))};

                bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);
                bslma::TestAllocator da("default",   veryVeryVeryVerbose);
                bslma::TestAllocator s1("supplied",  veryVeryVeryVerbose);
                bslma::TestAllocator s2("supplied2", veryVeryVeryVerbose);

                const Obj Z (ARRAY, ARRAY+3, &scratch);

                if (veryVerbose) { T_ P_(MEM) P(Z) }

                bslma::DefaultAllocatorGuard dag(&da);

                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(s2) {
                    if (veryVeryVerbose) { T_ T_ Q(ExceptionTestBody) }

                    Obj from(ARRAY, ARRAY + 3, &s1);

                    Obj obj(bslmf::MovableRefUtil::move(from), &s2);
                    ASSERTV(LINE, Z, obj, Z == obj);

#ifdef BDE_BUILD_TARGET_EXC
                    if ('Y' == MEM) {
                        ASSERTV(LINE, 0 < EXCEPTION_COUNT);
                    }
#endif
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
            }
        }
      } break;
      case 20: {
        // --------------------------------------------------------------------
        // JSONOBJECT COPY CONSTRUCTOR
        //   Ensure that we can create a distinct object of the class from any
        //   other one, such that the two objects have the same value.
        //
        // Concerns:
        //: 1 The copy constructor (with or without a supplied allocator)
        //:   creates an object having the same value as that of the supplied
        //:   original object.
        //:
        //: 2 If an allocator is NOT supplied to the copy constructor, the
        //:   default allocator in effect at the time of construction becomes
        //:   the object allocator for the resulting object (i.e., the
        //:   allocator of the original object is never copied).
        //:
        //: 3 If an allocator IS supplied to the copy constructor, that
        //:   allocator becomes the object allocator for the resulting object.
        //:
        //: 4 Supplying a default-constructed allocator has the same effect as
        //:   not supplying an allocator.
        //:
        //: 5 Supplying an allocator to the copy constructor has no effect on
        //:   subsequent object values.
        //:
        //: 6 Any memory allocation is from the object allocator.
        //:
        //: 7 There is no temporary memory allocation from any allocator.
        //:
        //: 8 Every object releases any allocated memory at destruction.
        //:
        //: 9 The original object is passed as a 'const' reference.
        //:
        //:10 The value of the original object is unchanged.
        //:
        //:11 The allocator used by the original object is unchanged.
        //:
        //:12 QoI: Copying an object having the default-constructed value
        //:   allocates no memory.
        //:
        //:13 Any memory allocation is exception neutral.
        //
        // Plan:
        //: 1 Using the table-driven technique:
        //:
        //:   1 Specify a set of (unique) valid object values (one per row) in
        //:     terms of three different Json types, including values that
        //:     should require allocation where applicable.
        //:
        //:   2 Additionally, provide a (tri-valued) column, 'MEM', indicating
        //:     the expectation of memory allocation for all typical
        //:     implementations of individual attribute types: ('Y') "Yes",
        //:     ('N') "No", or ('?') "implementation-dependent".
        //:
        //: 2 For each row (representing a distinct object value, 'V') in the
        //:   table described in P-1: (C-1..12)
        //:
        //:   1 Use the value constructor and a "scratch" allocator to create
        //:     two 'const' 'Obj', 'Z' and 'ZZ', each having the value 'V'.
        //:
        //:   2 Execute an inner loop that creates an object by
        //:     copy-constructing from value 'Z' from P-2.1, but invokes the
        //:     copy constructor differently in each iteration: (a) without
        //:     passing an allocator, (b) passing a null pointer, and (c)
        //:     passing the address of a test allocator distinct from the
        //:     default.
        //:
        //:   3 For each of these iterations (P-2.2):  (C-1..12)
        //:
        //:     1 Create three 'bslma::TestAllocator' objects, and install one
        //:       as the current default allocator (note that a ubiquitous test
        //:       allocator is already installed as the global allocator).
        //:
        //:     2 Use the copy constructor to dynamically create an object 'X',
        //:       with it's object allocator configured appropriately (see
        //:       P-2.2), supplying it the 'const' object 'Z' (see P-2.1); use
        //:       a distinct test allocator for the object's footprint.  (C-9)
        //:
        //:     3 Use the equality-comparison operator to verify that:
        //:       (C-1, 5, 10)
        //:
        //:       1 The newly constructed object, 'X', has the same value as
        //:         that of 'Z'.  (C-1, 5)
        //:
        //:       2 'Z' still has the same value as that of 'ZZ'  (C-10)
        //:
        //:     4 Use the 'allocator' accessor of each contained object to
        //:       to ensure that its object allocator is properly installed;
        //:       also use the 'allocator' accessor of 'X' to verify that its
        //:       object allocator is properly installed, and use the
        //:       'allocator' accessor of 'Z' to verify that the allocator that
        //:       it uses is unchanged.
        //:
        //:     5 Use the appropriate test allocators to verify that:
        //:       (C-2..4, 7..8, 12)
        //:
        //:       1 An object that IS expected to allocate memory does so from
        //:         the object allocator only (irrespective of the specific
        //:         number of allocations or the total amount of memory
        //:         allocated)..  (C-2, 4)
        //:
        //:       2 An object that is expected NOT to allocate memory doesn't.
        //:         (C-12)
        //:
        //:       3 If an allocator was supplied at construction (P-2.1c), the
        //:         current default allocator doesn't allocate any memory.
        //:         (C-3)
        //:
        //:       4 No temporary memory is allocated from the object allocator.
        //:         (C-7)
        //:
        //:       5 All object memory is released when the object is destroyed.
        //:         (C-8)
        //:
        //: 3 Test again, using the data of P-1, but this time just for the
        //:   supplied allocator configuration (P-2.2c), and create the object
        //:   as an automatic variable in the presence of injected exceptions
        //:   (using the 'BSLMA_TESTALLOCATOR_EXCEPTION_TEST_*' macros).
        //:   (C-13)
        //
        // Testing:
        //   JsonObject(const JsonObject& original, *a);
        // --------------------------------------------------------------------

        typedef JsonObject Obj;

        if (verbose) cout << endl
                          << "JSONOBJECT COPY CONSTRUCTOR" << endl
                          << "===========================" << endl;

        if (verbose) cout <<
           "\nUse a table of distinct object values and expected memory usage."
                                                                       << endl;

        const int              NUM_DATA        = DEFAULT_NUM_DATA;
        const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

        if (verbose) cout <<
             "\nCreate objects with various allocator configurations." << endl;
        {
            bool anyObjectMemoryAllocatedFlag = false;  // We later check that
                                                        // this test allocates
                                                        // some object memory
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int          LINE     = DATA[ti].d_line;
                const char         MEM      = DATA[ti].d_mem;
                const bool         BOOL     = DATA[ti].d_bool;
                const char *const  NUMBER   = DATA[ti].d_number_p;
                const char *const  STRING   = DATA[ti].d_string_p;

                const bsl::pair<const bsl::string, Json> ARRAY[3] =
                {bsl::make_pair<const bsl::string, Json>(KEY0, Json(BOOL)),
                 bsl::make_pair<const bsl::string, Json>(
                                                     KEY1,
                                                     Json(JsonNumber(NUMBER))),
                 bsl::make_pair<const bsl::string, Json>(KEY2, Json(STRING))};

                bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

                const Obj Z (ARRAY, ARRAY+3, &scratch);
                const Obj ZZ(ARRAY, ARRAY+3, &scratch);

                if (veryVerbose) { T_ P_(Z) P(ZZ) }

                for (char cfg = 'a'; cfg <= 'c'; ++cfg) {

                    const char CONFIG = cfg;

                    bslma::TestAllocator da("default",   veryVeryVeryVerbose);
                    bslma::TestAllocator fa("footprint", veryVeryVeryVerbose);
                    bslma::TestAllocator sa("supplied",  veryVeryVeryVerbose);

                    bslma::DefaultAllocatorGuard dag(&da);

                    Obj                  *objPtr = 0;
                    bslma::TestAllocator *objAllocatorPtr = 0;

                    switch (CONFIG) {
                      case 'a': {
                        objAllocatorPtr = &da;
                        objPtr = new (fa) Obj(Z);
                      } break;
                      case 'b': {
                        objAllocatorPtr = &da;
                        objPtr = new (fa) Obj(Z, 0);
                      } break;
                      case 'c': {
                        objAllocatorPtr = &sa;
                        objPtr = new (fa) Obj(Z, objAllocatorPtr);
                      } break;
                      default: {
                        BSLS_ASSERT_OPT(!"Bad allocator config");
                      } break;
                    }
                    ASSERTV(LINE, CONFIG, sizeof(Obj) == fa.numBytesInUse());

                    Obj& mX = *objPtr;  const Obj& X = mX;

                    if (veryVerbose) { T_ T_ P_(CONFIG) P(X) }

                    bslma::TestAllocator&  oa = *objAllocatorPtr;
                    bslma::TestAllocator& noa = (&da == &oa) ? sa : da;

                    // Verify the value of the object.

                    ASSERTV(LINE, CONFIG, Z, X,  Z == X);

                    // Verify that the value of 'Z' has not changed.

                    ASSERTV(LINE, CONFIG, Z, X, ZZ == X);

                    // Verify contained object allocators are installed
                    // properly.

                    ASSERTV(LINE, CONFIG, &oa == X[KEY0].allocator());
                    ASSERTV(LINE, CONFIG, &oa == X[KEY1].allocator());
                    ASSERTV(LINE, CONFIG, &oa == X[KEY2].allocator());

                    // Also invoke the object's 'allocator' accessor, as well
                    // as that of 'Z'.

                    ASSERTV(LINE, CONFIG, &oa, X.allocator(),
                            &oa == X.allocator());

                    ASSERTV(LINE, CONFIG, &scratch, Z.allocator(),
                            &scratch == Z.allocator());

                    // Verify no allocations from the non-object allocator.

                    ASSERTV(LINE, CONFIG, noa.numBlocksTotal(),
                            0 == noa.numBlocksTotal());

                    // Verify no temporary memory is allocated from the object
                    // allocator.

                    ASSERTV(LINE, CONFIG,
                            oa.numBlocksTotal(),
                            oa.numBlocksInUse(),
                            oa.numBlocksTotal() == oa.numBlocksInUse());

                    // Verify expected ('Y'/'N') object-memory allocations in
                    // addition to the one vector allocation.

                    if ('?' != MEM) {
                        ASSERTV(LINE, CONFIG, MEM, oa.numBlocksInUse(),
                                ('N' == MEM) == (2 == oa.numBlocksInUse()));
                    }

                    // Record if some object memory was allocated.

                    anyObjectMemoryAllocatedFlag |= !!oa.numBlocksInUse();

                    // Reclaim dynamically allocated object under test.

                    fa.deleteObject(objPtr);

                    // Verify all memory is released on object destruction.

                    ASSERTV(LINE, CONFIG, da.numBlocksInUse(),
                            0 == da.numBlocksInUse());
                    ASSERTV(LINE, CONFIG, fa.numBlocksInUse(),
                            0 == fa.numBlocksInUse());
                    ASSERTV(LINE, CONFIG, sa.numBlocksInUse(),
                            0 == sa.numBlocksInUse());
                }  // end foreach configuration
            }  // end foreach row

            // Double check that some object memory was allocated.

            ASSERT(anyObjectMemoryAllocatedFlag);

        }

        if (verbose) cout << "\nTesting with injected exceptions." << endl;
        {
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int          LINE     = DATA[ti].d_line;
                const char         MEM      = DATA[ti].d_mem;
                const bool         BOOL     = DATA[ti].d_bool;
                const char *const  NUMBER   = DATA[ti].d_number_p;
                const char *const  STRING   = DATA[ti].d_string_p;

                const bsl::pair<const bsl::string, Json> ARRAY[3] =
                {bsl::make_pair<const bsl::string, Json>(KEY0, Json(BOOL)),
                 bsl::make_pair<const bsl::string, Json>(
                                                     KEY1,
                                                     Json(JsonNumber(NUMBER))),
                 bsl::make_pair<const bsl::string, Json>(KEY2, Json(STRING))};

                bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);
                bslma::TestAllocator da("default",      veryVeryVeryVerbose);
                bslma::TestAllocator sa("supplied",     veryVeryVeryVerbose);

                const Obj Z (ARRAY, ARRAY+3, &scratch);
                const Obj ZZ(ARRAY, ARRAY+3, &scratch);

                bslma::DefaultAllocatorGuard dag(&da);

                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(sa) {
                    if (veryVeryVerbose) { T_ T_ Q(ExcpetionTestBody) }

                    Obj obj(Z, &sa);
                    ASSERTV(LINE, Z, obj, Z == obj);

#ifdef BDE_BUILD_TARGET_EXC
                    if ('Y' == MEM) {
                        ASSERTV(LINE, 0 < EXCEPTION_COUNT);
                    }
#endif
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                ASSERTV(LINE, ZZ, Z, ZZ == Z);
                ASSERTV(LINE, &scratch, Z.allocator(),
                        &scratch == Z.allocator());
                ASSERTV(LINE, da.numBlocksInUse(), 0 == da.numBlocksInUse());
                ASSERTV(LINE, sa.numBlocksInUse(), 0 == sa.numBlocksInUse());
            }
        }
      } break;
      case 19: {
        // --------------------------------------------------------------------
        // JSONOBJECT EQUALITY-COMPARISON OPERATORS
        //   Ensure that '==' and '!=' are the operational definition of value.
        //
        // Concerns:
        //: 1 Two objects, 'X' and 'Y', compare equal if and only if each of
        //:   their corresponding salient attributes respectively compares
        //:   equal.
        //:
        //: 2 All salient attributes participate in the comparison.
        //:
        //: 3 No non-salient attributes (i.e., 'allocator') participate.
        //:
        //: 4 'true  == (X == X)'  (i.e., identity)
        //:
        //: 5 'false == (X != X)'  (i.e., identity)
        //:
        //: 6 'X == Y' if and only if 'Y == X'  (i.e., commutativity)
        //:
        //: 7 'X != Y' if and only if 'Y != X'  (i.e., commutativity)
        //:
        //: 8 'X != Y' if and only if '!(X == Y)'
        //:
        //: 9 Comparison is symmetric with respect to user-defined conversion
        //:   (i.e., both comparison operators are free functions).
        //:
        //:10 Non-modifiable objects can be compared (i.e., 'const' objects and
        //:   'const' references).
        //:
        //:11 No memory allocation occurs as a result of comparison (e.g., the
        //:   arguments are not passed by value).
        //:
        //:12 The equality operator's signature and return type are standard.
        //:
        //:13 The inequality operator's signature and return type are standard.
        //
        // Plan:
        //: 1 Use the respective addresses of 'operator==' and 'operator!=' to
        //:   initialize function pointers having the appropriate signatures
        //:   and return types for the two homogeneous, free equality-
        //:   comparison operators defined in this component.
        //:   (C-9..10, 12..13)
        //:
        //: 2 Create a 'bslma::TestAllocator' object and install it as the
        //:   default allocator (note that a ubiquitous test allocator is
        //:   already installed as the global allocator).
        //:
        //: 3 Using the table-driven technique, specify a set of distinct
        //:   object values (one per row) in terms of their individual contents
        //:   such that for each of 3 distinct baseline contained objects,
        //:   there exists a pair of rows that differ in only the object at
        //:   that index.
        //:
        //: 4 For each of two configurations, 'a' and 'b': (C-1..3, 6..8)
        //:
        //:   1 Create two (object) allocators, 'oax' and 'oay'.
        //:
        //:   2 For each row 'R1' in the table of P-3: (C1..8)
        //:
        //:     1 Create object 'x', using 'oax' allocator, having the value
        //:       'R1', and use a 'const' reference 'X' to 'x' to verify the
        //:       reflexive (anti-reflexive) property of equality (inequality)
        //:       in the presence of aliasing.  (C-4..5)
        //:
        //:     2 For each row 'R2' in the table of P-3: (C-1..8)
        //:
        //:       1 Record, in 'EXP', whether or not distinct objects created
        //:         from 'R1' and 'R2', respectively, are expected to have the
        //:         same value.
        //:
        //:       2 Create an object 'y', using 'oax' in configuration 'a' and
        //:         'oay' in configuration 'b', having the value 'R2'.  Create
        //:         a 'const' reference to 'y' named 'Y'.
        //:
        //:       3 Verify the commutative property and expected return value
        //:         for both '==' and '!=', while monitoring both 'oax' and
        //:         'oay' to ensure that no object memory is ever allocated by
        //:         either operator.  (C-1..3, 6..8)
        //:
        //: 5 Use the test allocator from P-2 to verify that no memory is ever
        //:   allocated from the default allocator.  (C-11)
        //
        // Testing:
        //   bool operator==(const bdljsn::JsonObject& lhs, rhs);
        //   bool operator!=(const bdljsn::JsonObject& lhs, rhs);
        // --------------------------------------------------------------------

        typedef JsonObject Obj;

        if (verbose) cout << "\n" "JSONOBJECT EQUALITY-COMPARISON OPERATORS"
                             "\n" "========================================"
                             "\n";

        if (verbose) cout <<
                 "\nAssign the address of each operator to a variable" << endl;
        {
            typedef bool (*operatorPtr)(const Obj&, const Obj&);

            operatorPtr operatorEq = operator==;  (void) operatorEq;
            operatorPtr operatorNe = operator!=;  (void) operatorNe;
        }

        if (verbose) cout <<
            "\nCreate a test allocator and install it as the default." << endl;

        bslma::TestAllocator         da("default", veryVeryVeryVerbose);
        bslma::DefaultAllocatorGuard dag(&da);

        if (verbose) cout <<
            "\nDefine appropriate individual attribute values, 'Ai' and 'Bi'."
                                                                       << endl;

        const int EMPTY = 123456;

        static const struct {
            const int     d_line;                // source line number
            const int     d_value1;
            const int     d_value2;
            const int     d_value3;
            const int     d_value4;
        } DATA[] = {
        { L_,            1,     2,     3, EMPTY },         // baseline
        { L_,            4,     2,     3, EMPTY },
        { L_,            1,     4,     3, EMPTY },
        { L_,            1,     2,     4, EMPTY },
        { L_,            1,     2,     3,     4 },
        { L_,            1,     2, EMPTY, EMPTY },
        { L_,            1, EMPTY, EMPTY, EMPTY },

        };
        enum { NUM_DATA = sizeof DATA / sizeof *DATA };

        if (verbose) cout << "\nCompare every value with every value." << endl;

        for (char cfg = 'a'; cfg <= 'b'; ++cfg) {

            const char CONFIG = cfg;

            // Create two distinct test allocators, 'oax' and 'oay'.

            bslma::TestAllocator oax("objectx", veryVeryVeryVerbose);
            bslma::TestAllocator oay("objecty", veryVeryVeryVerbose);

            // Map allocators above to objects 'X' and 'Y' below.

            bslma::TestAllocator& xa = oax;
            bslma::TestAllocator& ya = 'a' == CONFIG ? oax : oay;

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int LINE1  = DATA[ti].d_line;

                Obj x(&xa);
                if (EMPTY != DATA[ti].d_value1) {
                    x.insert("first", Json(DATA[ti].d_value1));
                }
                if (EMPTY != DATA[ti].d_value2) {
                    x.insert("second", Json(DATA[ti].d_value2));
                }
                if (EMPTY != DATA[ti].d_value3) {
                    x.insert("third", Json(DATA[ti].d_value3));
                }
                if (EMPTY != DATA[ti].d_value4) {
                    x.insert("fourth", Json(DATA[ti].d_value4));
                }

                const Obj&    X = x;

                if (veryVerbose) { T_ T_ P_(LINE1) P_(X) }

                // Ensure an object compares correctly with itself (alias
                // test).
                {
                    ASSERTV(LINE1, X, X == X);
                    ASSERTV(LINE1, X, !(X != X));
                }

                for (int tj = 0; tj < NUM_DATA; ++tj) {
                    const int LINE2 = DATA[tj].d_line;

                    Obj y(&ya);

                    if (EMPTY != DATA[tj].d_value1) {
                        y.insert("first", Json(DATA[tj].d_value1));
                    }
                    if (EMPTY != DATA[tj].d_value2) {
                        y.insert("second", Json(DATA[tj].d_value2));
                    }
                    if (EMPTY != DATA[tj].d_value3) {
                        y.insert("third", Json(DATA[tj].d_value3));
                    }
                    if (EMPTY != DATA[tj].d_value4) {
                        y.insert("fourth", Json(DATA[tj].d_value4));
                    }
                    const Obj& Y = y;

                    if (veryVerbose) { T_ T_ P_(LINE2) P_(Y) }

                    const bool EXP = (ti == tj);

                    if (veryVerbose) {
                        T_ T_ T_ P_(EXP) P_(CONFIG) P_(X) P_(Y) }

                    // Verify value, commutativity, and no memory allocation

                    bslma::TestAllocatorMonitor oaxm(&oax), oaym(&oay);

                    ASSERTV(LINE1, LINE2, CONFIG, X, Y, EXP == (X == Y));
                    ASSERTV(LINE1, LINE2, CONFIG, Y, X, EXP == (Y == X));

                    ASSERTV(LINE1, LINE2, CONFIG, X, Y, !EXP == (X != Y));
                    ASSERTV(LINE1, LINE2, CONFIG, Y, X, !EXP == (Y != X));

                    ASSERTV(LINE1, LINE2, CONFIG, oaxm.isTotalSame());
                    ASSERTV(LINE1, LINE2, CONFIG, oaym.isTotalSame());

                    // Double check that some object memory was allocated.
                    ASSERTV(LINE1, LINE2, CONFIG, 1 <= xa.numBlocksInUse());
                    ASSERTV(LINE1, LINE2, CONFIG, 1 <= ya.numBlocksInUse());
                }
            }
        }

        ASSERTV(da.numBlocksTotal(), 0 == da.numBlocksTotal());
      } break;
      case 18: {
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
        //: 3 The output using 's << obj' is the same as 'obj.print(s, 0, -1)'
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
        //: 2 Using the table-driven technique:  (C1..3, 5..6, 8)
        //:
        //:   1 Define a 'const' 'JsonObject' object containing a variety of
        //:     'Json' objects.
        //:
        //:   2 Define fourteen carefully selected combinations of values for
        //:     the two formatting parameters, along with the expected output.
        //:
        //:   3 For each row in the table defined in P-2.2: (C-1..3, 5..6, 8)
        //:
        //:     1 Supply the 'JsonObject' from P-2.1 and each pair of
        //:       formatting parameters to 'print', omitting the 'level' or
        //:       'spacesPerLevel' parameter if the value of that argument is
        //:       '-8'.  If the parameters are, arbitrarily, (-9, -9), then
        //:       invoke the 'operator<<' instead.
        //
        // Testing
        //   operator<<(ostream&, const JsonObject&);
        //   ostream& JsonObject::print(os& s, int l = 0, int sPL = 4) const;
        // --------------------------------------------------------------------

        typedef JsonObject Obj;

        if (verbose) cout << endl
                          << "PRINT AND OUTPUT OPERATOR" << endl
                          << "=========================" << endl;

        if (verbose) cout << "\nAssign the addresses of 'print' and "
                             "the output 'operator<<' to variables." << endl;
        {
            using bsl::ostream;

            typedef ostream& (Obj::*funcPtr)(ostream&, int, int) const;
            typedef ostream& (*operatorPtr)(ostream&, const Obj&);

            funcPtr     printMember = &Obj::print;  (void) printMember;
            operatorPtr operatorOp  = operator<<;   (void) operatorOp;
        }

        if (verbose) cout <<
            "\nCreate a table of distinct value/format combinations." << endl;

        JsonArray inner;
        inner.pushBack(Json(2));
        inner.pushBack(Json("inner"));

        Obj mX;
        mX.insert("null", Json());
        mX.insert("string", Json("outer"));
        mX.insert("number", Json(1));
        mX.insert("array", Json(inner));

        const Obj& X = mX;

        static const struct {
            int         d_line;
            int         d_level;
            int         d_spacesPerLevel;
            const char *d_expected_p;
        } DATA[] = {
#define NL "\n"
#define SP " "
        { L_,  0,  0, "{"                                       NL
                      "\"null\": null,"                         NL
                      "\"string\": \"outer\","                  NL
                      "\"number\": 1,"                          NL
                      "\"array\": ["                            NL
                      "2,"                                      NL
                      "\"inner\""                               NL
                      "]"                                       NL
                      "}"                                       },
        { L_,  0,  1, "{"                                       NL
                      " \"null\": null,"                        NL
                      " \"number\": 1,"                         NL
                      " \"string\": \"outer\","                 NL
                      " \"array\": ["                           NL
                      "  2,"                                    NL
                      "  \"inner\""                             NL
                      " ]"                                      NL
                      "}"                                       },

        { L_,  0, -1, "{"
                      "\"null\": null,"                         SP
                      "\"number\": 1,"                          SP
                      "\"string\": \"outer\","                  SP
                      "\"array\": ["
                      "2,"                                      SP
                      "\"inner\""
                      "]"
                      "}"                                       },

        { L_,  0, -8, "{"                                       NL
                      "    \"null\": null,"                     NL
                      "    \"number\": 1,"                      NL
                      "    \"string\": \"outer\","              NL
                      "    \"array\": ["                        NL
                      "        2,"                              NL
                      "        \"inner\""                       NL
                      "    ]"                                   NL
                      "}"                                       },

        { L_,  3,  0, "{"                                       NL
                      "\"null\": null,"                         NL
                      "\"number\": 1,"                          NL
                      "\"string\": \"outer\","                  NL
                      "\"array\": ["                            NL
                      "2,"                                      NL
                      "\"inner\""                               NL
                      "]"                                       NL
                      "}"                                       },

        { L_,  3,  2, "      {"                                 NL
                      "        \"null\": null,"                 NL
                      "        \"number\": 1,"                  NL
                      "        \"string\": \"outer\","          NL
                      "        \"array\": ["                    NL
                      "          2,"                            NL
                      "          \"inner\""                     NL
                      "        ]"                               NL
                      "      }"                                 },

        { L_,  3, -2, "      {"
                      "\"null\": null,"                         SP
                      "\"number\": 1,"                          SP
                      "\"string\": \"outer\","                  SP
                      "\"array\": ["
                      "2,"                                      SP
                      "\"inner\""
                      "]"
                      "}"                                       },

        { L_,  3, -8, "            {"                           NL
                      "                \"null\": null,"         NL
                      "                \"number\": 1,"          NL
                      "                \"string\": \"outer\","  NL
                      "                \"array\": ["            NL
                      "                    2,"                  NL
                      "                    \"inner\""           NL
                      "                ]"                       NL
                      "            }"                           },

        { L_, -3,  0, "{"                                       NL
                      "\"null\": null,"                         NL
                      "\"number\": 1,"                          NL
                      "\"string\": \"outer\","                  NL
                      "\"array\": ["                            NL
                      "2,"                                      NL
                      "\"inner\""                               NL
                      "]"                                       NL
                      "}"                                       },

        { L_, -3,  2, "{"                                       NL
                      "        \"null\": null,"                 NL
                      "        \"number\": 1,"                  NL
                      "        \"string\": \"outer\","          NL
                      "        \"array\": ["                    NL
                      "          2,"                            NL
                      "          \"inner\""                     NL
                      "        ]"                               NL
                      "      }"                                 },

        { L_, -3, -2, "{"
                      "\"null\": null,"                         SP
                      "\"number\": 1,"                          SP
                      "\"string\": \"outer\","                  SP
                      "\"array\": ["
                      "2,"                                      SP
                      "\"inner\""
                      "]"
                      "}"                                       },

        { L_, -3, -8, "{"                                       NL
                      "                \"null\": null,"         NL
                      "                \"number\": 1,"          NL
                      "                \"string\": \"outer\","  NL
                      "                \"array\": ["            NL
                      "                    2,"                  NL
                      "                    \"inner\""           NL
                      "                ]"                       NL
                      "            }"                           },

        { L_, -8, -8, "{"                                       NL
                      "    \"null\": null,"                     NL
                      "    \"number\": 1,"                      NL
                      "    \"string\": \"outer\","              NL
                      "    \"array\": ["                        NL
                      "        2,"                              NL
                      "        \"inner\""                       NL
                      "    ]"                                   NL
                      "}"                                       },

        { L_, -9, -9, "{"
                      "\"null\": null,"                         SP
                      "\"number\": 1,"                          SP
                      "\"string\": \"outer\","                  SP
                      "\"array\": ["
                      "2,"                                      SP
                      "\"inner\""
                      "]"
                      "}"                                       },
#undef SP
#undef NL
        };
        enum { NUM_DATA = sizeof DATA / sizeof *DATA };

        const ReorderExpectedJsonObjectString reorder(X);

        if (verbose) cout << "\nTsting with various print specifications."
                          << endl;
        {
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int         LINE = DATA[ti].d_line;
                const int         L    = DATA[ti].d_level;
                const int         SPL  = DATA[ti].d_spacesPerLevel;
                const char *const EXP  = DATA[ti].d_expected_p;

                if (veryVerbose) { T_ P_(L) P_(SPL) }

                if (veryVeryVerbose) {T_ T_ Q(EXPECTED) cout << EXP; }

                bsl::ostringstream os;

                if (-9 == L) {
                    ASSERTV(LINE, -9 == SPL);

                    ASSERTV(LINE, &os == &(os << X));

                    if (veryVeryVerbose) {T_ T_ Q(operator<<) }
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

                if (veryVeryVerbose) { P(os.str()) }

                const bsl::string expected = reorder(EXP);
                ASSERTV(LINE, expected, os.str(), expected == os.str());
            }
        }
      } break;
      case 17: {
        // --------------------------------------------------------------------
        // JSONOBJECT BASIC ACCESSORS
        //   Ensure each basic accessor properly provides access to contained
        //   objects.
        //
        // Concerns:
        //: 1 Each accessor returns the value of the corresponding contained
        //:   object.
        //:
        //: 2 Each accessor method is declared 'const'.
        //:
        //: 3 No accessor allocates memory.
        //:
        //: 4 Accessors return a 'const' reference (because 'Json' objects can
        //:   allocate).
        //
        // Plan:
        //: 1 Create two 'bslma::TestAllocator' objects, and install one as
        //:   the current default allocator (note that a ubiquitous test
        //:   allocator is already installed as the global allocator).
        //:
        //: 2 Use the default constructor with the allocator from P-1 that was
        //:   not installed as default, to create an object.
        //:
        //: 3 Verify that each basic accessor, invoked on a 'const' reference
        //:   to the object created in P-2, returns the expected value.  (C-2)
        //:
        //: 4 For each salient attribute (constributing to value): (C-1, 3..4)
        //:   1 Use the primary manipulator (pushBack) to put three
        //:     non-default, discriminable objects into the container.
        //:
        //:   2 Use all accessors to verify expected values.  (C-1)
        //:
        //:   3 Monitor the memory allocated from both the default and object
        //:     allocators before and after calling the accessor; verify that
        //:     there is no change in total memory allocation.  (C-3..4)
        //
        // Testing:
        //   const Json& JsonObject::operator[](bsl::string_view& key) const;
        // --------------------------------------------------------------------

        typedef JsonObject Obj;

        const bool             D1 = false;
        const int              D2 = 0;
        const bsl::string_view D3;

        const bool             A1 = true;
        const int              A2 = 1;
        const bsl::string_view A3 = LONG_STRING;

        if (verbose) cout << endl
                          << "JSONOBJECT BASIC ACCESSORS" << endl
                          << "==========================" << endl;

        bslma::TestAllocator da("default", veryVeryVeryVerbose);
        bslma::TestAllocator oa("object",  veryVeryVeryVerbose);

        bslma::DefaultAllocatorGuard dag(&da);

        if (verbose) cout << endl
            << "Create an object, passing in the other allocator." << endl;

        Obj mX(&oa);  const Obj& X = mX;

        mX[KEY0] = Json(D1);
        mX[KEY1] = Json(D2);
        mX[KEY2] = Json(D3);

        if (verbose) cout << endl
            << "Verify all basic accessors report expected values." << endl;
        {
            if (verbose) cout << endl << "JsonObject::operator[]" << endl;
            ASSERTV(X[KEY0].type(), X[KEY0].isBoolean());
            ASSERTV(X[KEY0].theBoolean(), D1 == X[KEY0].theBoolean());

            ASSERTV(X[KEY1].type(), X[KEY1].isNumber());
            {
                int value;
                int status = X[KEY1].theNumber().asInt(&value);
                ASSERTV(status, 0 == status);
                ASSERTV(X[KEY1].theNumber(), D2 == value);
            }

            ASSERTV(X[KEY2].type(), X[KEY2].isString());
            ASSERTV(X[KEY2].theString(), D3 == X[KEY2].theString());

            if (verbose) cout << endl
                << "Change data and verify expected values." << endl;

            mX[KEY0] = A1;
            mX[KEY1] = A2;
            mX[KEY2] = A3;

            if (verbose) cout << endl << "JsonObject::operator[]" << endl;
            ASSERTV(X[KEY0].type(), X[KEY0].isBoolean());
            ASSERTV(X[KEY0].theBoolean(), A1 == X[KEY0].theBoolean());

            ASSERTV(X[KEY1].type(), X[KEY1].isNumber());
            {
                int value;
                int status = X[KEY1].theNumber().asInt(&value);
                ASSERTV(status, 0 == status);
                ASSERTV(X[KEY1].theNumber(), A2 == value);
            }

            ASSERTV(X[KEY2].type(), X[KEY2].isString());
            ASSERTV(X[KEY2].theString(), A3 == X[KEY2].theString());
        }

        // Double check that some object memory was allocated.

        ASSERTV(oa.numBlocksTotal(), 1 <= oa.numBlocksTotal());

        ASSERTV(da.numBlocksTotal(), 0 == da.numBlocksTotal());
      } break;
      case 16: {
        // --------------------------------------------------------------------
        // JSONOBJECT VALUE CTOR
        //   Ensure that we can put an object into any initial state relevant
        //   for thorough testing.
        //
        // Concerns:
        //: 1 The value constructor (with or without a supplied allocator) can
        //:   create an object having any value that does not violate the
        //:   constructor's documented preconditions.
        //:
        //: 2 Any string arguments can be of type 'char *' or 'string'.
        //:
        //: 3 Any argument can be 'const'.
        //:
        //: 4 If an allocator is NOT supplied to the value constructor, the
        //:   default allocator in effect at the time of construction becomes
        //:   the object allocator for the resulting object.
        //:
        //: 5 If an allocator IS supplied to the value constructor, that
        //:   allocator becomes the object allocator for the resulting object.
        //:
        //: 6 Supplying a default-constructed allocator has the same effect as
        //:   not supplying al allocator.
        //:
        //: 7 Supplying an allocator to the value constructor has no effect
        //:   on subsequent object values.
        //:
        //: 8 Any memory allocation is from the object allocator.
        //:
        //: 9 There is no temporary memory allocation from any allocator.
        //:
        //:10 Every object releases any allocated memory at destruction.
        //:
        //:11 QoI: Creating an object having the default-constructed value
        //:   allocates no memory.
        //:
        //:12 Any memory allocation is exception neutral.
        //:
        //:13 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Using the table-driven technique:
        //:
        //:   1 Specify a set of (unique) valid object values (one per row) in
        //:     terms of their individual attributes, including (a) first, the
        //:     default value, (b) boundary values corresponding to every range
        //:     of values that each individual attribute can independently
        //:     attain, and (c) values that should require allocation from each
        //:     individual attribute that can independently allocate memory.
        //:   2 Additionally, provide a (tri-valued) column, 'MEM', indicating
        //:     the expectation of memory allocation for all typical
        //:     implementations of individual attribute types: ('Y') "Yes",
        //:     'N') "No", or ('?') "implementation-dependent".
        //:
        //: 2 For each row (representing a distinct object value, 'V') in the
        //:   table described in P-1: (C-1, 3..11)
        //:
        //:   1 Execute an inner loop that creates an object having value 'V',
        //:     but invokes the constructor differently on each iteration: (a)
        //:     without passing an allocator, (b) passing a
        //:     default-constructed allocator explicitly (c) passing the
        //:     address of a test allocator distinct from the default
        //:     allocator, and (d) passing in an allocator constructed from
        //:     the address of a test allocator distinct from the default.
        //:
        //:   2 For each of the iterations in P-2.1:  (C-1, 4..11)
        //:
        //:     1 Create three 'bslma::TestAllocator' objects, and install one
        //:       as the current default allocator (note that a ubiquitous test
        //:       allocator is already installed as the global allocator).
        //:
        //:     2 Use the value constructor to dynamically create an object
        //:       having the value 'V', with its object allocator configured
        //:       appropriately (see P-2.1), supplying all the arguments as
        //:       'const' and representing any string arguments as 'char *';
        //:       use a distinct test allocator for the object's footprint.
        //:
        //:     3 Use the (as yet unproven) salient attribute accessors to
        //:       verify that all of the attributes of each object have their
        //:       expected values.  (C-1, 7)
        //:
        //:     4 Use the 'allocator' accessor of each underlying attribute
        //:       capable of allocating memory to ensure that its object
        //:       allocator is properly installed; also invoke the (as yet
        //:       unproven) 'allocator' accessor of the object under test.
        //:       (C-8)
        //:
        //:     5 Use the appropriate test allocators to verify that:  (C-4..6,
        //:       9..11)
        //:
        //:       1 An object that IS expected to allocate memory does so
        //:         from the object allocator only (irrespective of the
        //:         specific number of allocations or the total amount of
        //:         memory allocated).  (C-4, 6)
        //:
        //:       2 An object that is expected NOT to allocate memory doesn't.
        //:         (C-11)
        //:
        //:       3 If an allocator was supplied at construction (P-2.1c), the
        //:         default allocator doesn't allocate any memory.  (C-5)
        //:
        //:       4 No temporary memory is allocated from the object allocator.
        //:         (C-9)
        //:
        //:       5 All object memory is released when the object is destroyed.
        //:         (C-10)
        //:
        //: 3 Repeat the steps in P-2 for the supplied allocator configuration
        //:   (P-2.1c) on the data of P-1, but this time create the object as
        //:   an automatic variable in the presence of injected exceptions
        //:   (using the 'BSLMA_TESTALLOCATOR_EXCEPTION_TEST_*' macros);
        //:   represent any string arguments in terms of 'string' using a
        //:   "scratch" allocator.  (C-2, 12)
        //:
        //: 4 Verify that, in appropriate build modes, defensive checks are
        //:   triggered for invalid attribute values, but not triggered for
        //:   adjacent valid ones (using the 'BSLS_ASSERTTEST_*' macros).
        //:   (C-13)
        //
        // Testing:
        //   JsonObject(INPUT_ITER first, INPUT_ITER last, *a);
        //   CONCERN: All creator/manipulator ptr./ref. parameters are 'const'.
        //   CONCERN: String arguments can be either 'char *' or 'string'.
        // --------------------------------------------------------------------

        typedef JsonObject Obj;

        if (verbose) cout << "\n" "JSONOBJECT VALUE CTOR" "\n"
                                  "=====================" "\n";

        if (verbose) cout <<
           "\nUse a table of distinct object values and expected memory usage."
                                                                       << endl;

        const int              NUM_DATA        = DEFAULT_NUM_DATA;
        const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

        if (verbose) cout <<
            "\nUse a table of distict object values and expected memory usage."
                                                                       << endl;
        {
            bool anyObjectMemoryAllocatedFlag = false;

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int          LINE     = DATA[ti].d_line;
                const char         MEM      = DATA[ti].d_mem;
                const bool         BOOL     = DATA[ti].d_bool;
                const char *const  NUMBER   = DATA[ti].d_number_p;
                const char *const  STRING   = DATA[ti].d_string_p;

                const bsl::pair<const bsl::string, Json> ARRAY[3] =
                {bsl::make_pair<const bsl::string, Json>(KEY0, Json(BOOL)),
                 bsl::make_pair<const bsl::string, Json>(
                         KEY1, Json(JsonNumber(NUMBER))),
                 bsl::make_pair<const bsl::string, Json>(KEY2, Json(STRING))};

                if (veryVerbose) { T_ P_(MEM) P_(BOOL) P_(NUMBER) P_(STRING) }

                ASSERTV(LINE, MEM, MEM && strchr("YN?", MEM));

                for (char cfg = 'a'; cfg <= 'c'; ++cfg) {

                    const char CONFIG = cfg;

                    if (veryVerbose) { T_ T_ P(CONFIG) }

                    bslma::TestAllocator da("default",   veryVeryVeryVerbose);
                    bslma::TestAllocator fa("footprint", veryVeryVeryVerbose);
                    bslma::TestAllocator sa("supplied",  veryVeryVeryVerbose);

                    bslma::DefaultAllocatorGuard dag(&da);

                    Obj                  *objPtr          = 0;
                    bslma::TestAllocator *objAllocatorPtr = 0;

                    switch (CONFIG) {
                      case 'a': {
                        objAllocatorPtr = &da;
                        objPtr = new (fa) Obj(ARRAY, ARRAY + 3);
                      } break;
                      case 'b': {
                        objAllocatorPtr = &da;
                        objPtr = new (fa) Obj(ARRAY, ARRAY + 3,
                                              objAllocatorPtr);
                      } break;
                      case 'c': {
                        objAllocatorPtr = &sa;
                        objPtr = new (fa) Obj(ARRAY, ARRAY + 3,
                                              objAllocatorPtr);
                      } break;
                      default: {
                        BSLS_ASSERT_OPT(!"Bad allocator config.");
                      } break;
                    }

                    ASSERTV(LINE, CONFIG,
                            sizeof(Obj) == fa.numBytesInUse());

                    Obj& mX = *objPtr;  const Obj& X = mX;

                    if (veryVerbose) { T_ T_ P_(CONFIG) P(X) }

                    bslma::TestAllocator&  oa = *objAllocatorPtr;
                    bslma::TestAllocator& noa = (&da == &oa) ? sa : da;

                    // -------------------------------------
                    // Verify the object's attribute values.
                    // -------------------------------------

                    ASSERTV(LINE, CONFIG, X[KEY0].type(), X[KEY0].isBoolean());
                    ASSERTV(LINE, CONFIG, BOOL, X[KEY0],
                            BOOL == X[KEY0].theBoolean());
                    ASSERTV(LINE, CONFIG, X[KEY1].type(), X[KEY1].isNumber());
                    ASSERTV(LINE, CONFIG, NUMBER, X[KEY1],
                            NUMBER == X[KEY1].theNumber().value());
                    ASSERTV(LINE, CONFIG, X[KEY2].type(), X[KEY2].isString());
                    ASSERTV(LINE, CONFIG, STRING, X[KEY2],
                            STRING == X[KEY2].theString());

                    // ------------------------------------------------------
                    // Verify any attribute allocators are installed properly
                    // ------------------------------------------------------

                    ASSERTV(LINE, CONFIG, &oa == X[KEY1].allocator());
                    ASSERTV(LINE, CONFIG, &oa == X[KEY2].allocator());
                    ASSERTV(LINE, CONFIG, &oa == X.allocator());
                    ASSERTV(CONFIG, noa.numBlocksTotal(),
                            0 == noa.numBlocksTotal());

                    // Verify no temporary memory is allocated from the object
                    // allocator.

                    ASSERTV(LINE, CONFIG,
                            oa.numBlocksTotal(),
                            oa.numBlocksInUse(),
                            oa.numBlocksTotal() == oa.numBlocksInUse());

                    if ('?' != MEM) {
                        ASSERTV(LINE, CONFIG, MEM, oa.numBlocksInUse(),
                                ('N' == MEM) == (3 >= oa.numBlocksInUse()));
                    }

                    // Record if some object memory was allocated.
                    anyObjectMemoryAllocatedFlag |= !!oa.numBlocksInUse();

                    // Reclaim dynamically allocated object under test.
                    fa.deleteObject(objPtr);

                    // Verify all memory is released on object destruction.

                    ASSERTV(LINE, CONFIG, da.numBlocksInUse(),
                            0 == da.numBlocksInUse());
                    ASSERTV(LINE, CONFIG, fa.numBlocksInUse(),
                            0 == fa.numBlocksInUse());
                    ASSERTV(LINE, CONFIG, sa.numBlocksInUse(),
                            0 == sa.numBlocksInUse());
                }  // end foreach configuration

            }   // end foreach row

            // Double check that some object memory was allocated.

            ASSERT(anyObjectMemoryAllocatedFlag);
        }

        if (verbose) cout << "\nTesting with injected exceptions." << endl;
        {
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int          LINE     = DATA[ti].d_line;
                const char         MEM      = DATA[ti].d_mem;
                const bool         BOOL     = DATA[ti].d_bool;
                const char *const  NUMBER   = DATA[ti].d_number_p;
                const char *const  STRING   = DATA[ti].d_string_p;

                const bsl::pair<const bsl::string, Json> ARRAY[3] =
                {bsl::make_pair<const bsl::string, Json>(KEY0, Json(BOOL)),
                 bsl::make_pair<const bsl::string, Json>(
                                                     KEY1,
                                                     Json(JsonNumber(NUMBER))),
                 bsl::make_pair<const bsl::string, Json>(KEY2, Json(STRING))};

                if (veryVerbose) { T_ P_(MEM) P_(BOOL) P_(NUMBER) P_(STRING) }

                bslma::TestAllocator da("default",  veryVeryVeryVerbose);
                bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);

                bslma::DefaultAllocatorGuard dag(&da);

                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(sa) {
                    if (veryVeryVerbose) { T_ T_ Q(ExceptionTestBody) }

                    Obj obj(ARRAY, ARRAY + 3, &sa);
                    ASSERTV(LINE, ti, obj[KEY0].type(), obj[KEY0].isBoolean());
                    ASSERTV(LINE, ti, BOOL, obj[KEY0],
                            BOOL == obj[KEY0].theBoolean());
                    ASSERTV(LINE, ti, obj[KEY1].type(), obj[KEY1].isNumber());
                    ASSERTV(LINE, ti, NUMBER, obj[KEY1],
                            NUMBER == obj[KEY1].theNumber().value());
                    ASSERTV(LINE, ti, obj[KEY2].type(), obj[KEY2].isString());
                    ASSERTV(LINE, ti, STRING, obj[KEY2],
                            STRING == obj[KEY2].theString());

#ifdef BDE_BUILD_TARGET_EXC
                    if ('Y' == MEM) {
                        ASSERTV(LINE, 0 < EXCEPTION_COUNT);
                    }
#endif
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                ASSERTV(LINE, da.numBlocksInUse(),
                        0 == da.numBlocksInUse());
                ASSERTV(LINE, sa.numBlocksInUse(),
                        0 == sa.numBlocksInUse());
            }
        }
      } break;
      case 15: {
        // --------------------------------------------------------------------
        // JSONOBJECT DEFAULT CTOR, PRIMARY MANIPULATORS, & DTOR
        //   Ensure that we can use the default constructor of JsonObject to
        //   create an object (having the default constructed value), use the
        //   primary manipulators to put that object into any state relevant
        //   for thorough testing, and use the destructor to destroy it safely.
        //
        // Concerns:
        //: 1 An object created with the default constructor (with or without a
        //:   supplied allocator) has the contractually specified default
        //:   value.
        //:
        //: 2 If an allocator is NOT supplied to the default constructor, the
        //:   default allocator in effect at the time of construction becomes
        //:   the object allocator for the resulting object.
        //:
        //: 3 If an allocator IS supplied to the default constructor, that
        //:   allocator becomes the object allocator for the resulting object.
        //:
        //: 4 Supplying a default-constructed allocator has the same effect as
        //:   not supplying an allocator.
        //:
        //: 5 Supplying an allocator to the default constructor has no effect
        //:   on subsequent object values.
        //:
        //: 6 Any memory allocation is from the object allocator.
        //:
        //: 7 There is no temporary allocation from any allocator.
        //:
        //: 8 Every object releases any allocated memory at destruction.
        //:
        //: 9 QoI: The default constructor allocates no memory.
        //:
        //:10 Each attribute is modifiable independently.
        //:
        //:11 Each attribute can be set to represent any value that does not
        //:   violate that attribute's documented constraints.
        //:
        //:12 Any string arguments can be of type 'char *' or 'string'.
        //:
        //:13 Any arguments can be 'const'.
        //:
        //:14 Any memory allocation is exception neutral.
        //:
        //:15 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Create three sets of attribute values for the object: ('D')
        //:   values corresponding to the default-constructed object, ('A')
        //:   values that allocate memory if possible, and ('B') other values
        //:   that do not cause additional memory allocation beyond that which
        //:   may be incurred by 'A'.  Both the 'A' and 'B' attribute values
        //:   should be chosen to be boundary values where possible.  If an
        //:   attribute can be supplied via alternate C++ types (e.g., 'string'
        //:   instead of 'char *'), use the alternate type for 'B'.
        //:
        //: 2 Execute an inner loop that creates an object by
        //:   default-construction, but invokes the default constructor
        //:   differently in each iteration: (a) without passing an allocator,
        //:   (b) passing a default-constructed allocator explicitly (c)
        //:   passing the address of a test allocator distinct from the
        //:   default, and (d) passing in an allocator constructed from the
        //:   address of a test allocator distinct from the default.  For each
        //:   of these iterations: (C-1..14)
        //:
        //:   1 Create three 'bslma::TestAllocator' objects, and install one as
        //:     as the current default allocator (note that a ubiquitous test
        //:     allocator is already installed as the global allocator).
        //:
        //:   2 Use the default constructor to dynamically create an object
        //:     'X', with its object allocator configured appropriately (see
        //:     P-2); use a distinct test allocator for the object's footprint.
        //:
        //:   3 Use the 'allocator' accessor of each underlying attribute
        //:     capable of allocating memory to ensure that its object
        //:     allocator is properly installed; also invoke the (as yet
        //:     unproven) 'allocator' accessor of the object under test.
        //:     (C-2..4)
        //:
        //:   4 Use the appropriate test allocators to verify that no memory
        //:     is allocated by the default constructor.  (C-9)
        //:
        //:   5 Use the individual (as yet unproven) salient attribute
        //:     accessors to verify the default-constructed value.  (C-1)
        //:
        //:   6 For each attribute 'i', in turn, create a local block.  Then
        //:     inside the block, using brute force, set that attribute's
        //:     value, passing a 'const' argument representing each of the
        //:     three test values, in turn (see P-1), first to 'Ai', then to
        //:     'Bi', and finally back to 'Di'.  If attribute 'i' can allocate
        //:     memory, verify that it does so on the first value transition
        //:     ('Di' -> 'Ai'), and that the corresponding primary manipulator
        //:     is exception neutral (using the
        //:     'BSLMA_TESTALLOCATOR_EXCEPTION_TEST_*' macros).  In all other
        //:     cases, verify that no memory allocation occurs.  After each
        //:     transition, use the (as yet unproven) basic accessors to verify
        //:     that only the intended attribute value changed.  (C-5..6,
        //:     11..14)
        //:
        //:   7 Corroborate that attributes are modifiable independently by
        //:     first setting all of the attributes to their 'A' values.  Then
        //:     incrementally set each attribute to it's corresponding  'B'
        //:     value and verify after each manipulation that only that
        //:     attribute's value changed.  (C-10)
        //:
        //:   8 Verify that no temporary memory is allocated from the object
        //:     allocator.  (C-7)
        //:
        //:   9 Verify that all object memory is released when the object is
        //:     destroyed.  (C-8)
        //:
        //: 3 Verify that, in appropriate build modes, defensive checks are
        //:   triggered for invalid attribute values, but not triggered for
        //:   adjacent valid ones (using the 'BSLS_ASSERTTEST_*' macros).
        //:   (C-15)
        //
        // Testing:
        //   JsonObject();
        //   JsonObject(bslma::Allocator *basicAllocator);
        //   ~JsonObject();
        //   Json& JsonObject::operator[](const string_view& key);
        // --------------------------------------------------------------------

        typedef JsonObject Obj;

        if (verbose) cout << endl
            << "JSONOBJECT DEFAULT CTOR, PRIMARY MANIPULATORS, & DTOR" << endl
            << "=====================================================" << endl;

        if (verbose) cout << "\nEstablish suitable attribute values." << endl;

        // 'D' values: These are the default-constructed values.

        const int  D1   = 0;  // Json variant JsonNumber
        const char D2[] = ""; // Json variant bsl::string

        // 'A' values: Should cause memory allocation if possible.

        const int  A1   = 1;
        const char A2[] = "a_" SUFFICIENTLY_LONG_STRING;

        // 'B' values: Should NOT cause allocation (use alternate string type).
        const int              B1 = 2;
        const bsl::string_view B2 = "b";

        if (verbose) cout << "\nTesting with various allocator configurations."
                          << endl;

        for (char cfg = 'a'; cfg <= 'c'; ++cfg) {

            const char CONFIG = cfg;

            bslma::TestAllocator da("default",   veryVeryVeryVerbose);
            bslma::TestAllocator fa("footprint", veryVeryVeryVerbose);
            bslma::TestAllocator sa("supplied",  veryVeryVeryVerbose);

            bslma::DefaultAllocatorGuard dag(&da);

            Obj                  *objPtr = 0;
            bslma::TestAllocator *objAllocatorPtr = 0;

            switch (CONFIG) {
              case 'a': {
                objAllocatorPtr = &da;
                objPtr = new (fa) Obj();
              } break;
              case 'b': {
                objAllocatorPtr = &da;
                objPtr = new (fa) Obj(0);
              } break;
              case 'c': {
                objAllocatorPtr = &sa;
                objPtr = new (fa) Obj(objAllocatorPtr);
              } break;
              default: {
                BSLS_ASSERT_OPT(!"Bad allocator config.");
              } break;
            }

            Obj&                   mX = *objPtr;  const Obj& X = mX;
            bslma::TestAllocator&  oa = *objAllocatorPtr;
            bslma::TestAllocator& noa = (&da == &oa) ? sa : da;

            // --------------------------------------
            // Verify object allocator is as expected
            // --------------------------------------

            ASSERTV(CONFIG, &oa, X.allocator(), &oa == X.allocator());

            // Verify no allocation from the object/non-object allocators

            ASSERTV(CONFIG, oa.numBlocksTotal(), 0 == oa.numBlocksTotal());
            ASSERTV(CONFIG, noa.numBlocksTotal(), 0 == noa.numBlocksTotal());

            // ---------------------------------
            // Set attribute values to defaults.
            // ---------------------------------

            mX[KEY0] = bdljsn::Json(D1);
            mX[KEY1] = bdljsn::Json(D2);

            // -------------------------------------
            // Verify the object's attribute values.
            // -------------------------------------

            ASSERTV(CONFIG, X.size(), 2 == X.size());
            ASSERTV(CONFIG, X[KEY0].type(), X[KEY0].isNumber());
            {
                int value;
                int status = X[KEY0].theNumber().asInt(&value);
                ASSERTV(status, 0 == status);
                ASSERTV(CONFIG, D1, X[KEY0].theNumber(), D1 == value);
            }
            ASSERTV(CONFIG, X[KEY1].type(), X[KEY1].isString());
            ASSERTV(CONFIG,
                    D2,
                    X[KEY1].theString(),
                    X[KEY1].theString() == D2);

            // ---------------------------------------------------------------
            // Verify that each attribute is independently settable.
            // ---------------------------------------------------------------

            {
                bslma::TestAllocatorMonitor tam(&oa);

                mX[KEY0] = A1;
                ASSERTV(CONFIG, X[KEY0].type(), X[KEY0].isNumber());
                {
                    int value;
                    int status = X[KEY0].theNumber().asInt(&value);
                    ASSERTV(status, 0 == status);
                    ASSERTV(CONFIG, A1, X[KEY0].theNumber(), A1 == value);
                }
                ASSERTV(CONFIG, X[KEY1].type(), X[KEY1].isString());
                ASSERTV(CONFIG,
                        D2,
                        X[KEY1].theString(),
                        X[KEY1].theString() == D2);

                mX[KEY0] = B1;
                ASSERTV(CONFIG, X[KEY0].type(), X[KEY0].isNumber());
                {
                    int value;
                    int status = X[KEY0].theNumber().asInt(&value);
                    ASSERTV(status, 0 == status);
                    ASSERTV(CONFIG, B1, X[KEY0].theNumber(), B1 == value);
                }
                ASSERTV(CONFIG, X[KEY1].type(), X[KEY1].isString());
                ASSERTV(CONFIG,
                        D2,
                        X[KEY1].theString(),
                        X[KEY1].theString() == D2);

                mX[KEY0] = D1;
                ASSERTV(CONFIG, X[KEY0].type(), X[KEY0].isNumber());
                {
                    int value;
                    int status = X[KEY0].theNumber().asInt(&value);
                    ASSERTV(status, 0 == status);
                    ASSERTV(CONFIG, D1, X[KEY0].theNumber(), D1 == value);
                }
                ASSERTV(CONFIG, X[KEY1].type(), X[KEY1].isString());
                ASSERTV(CONFIG,
                        D2,
                        X[KEY1].theString(),
                        X[KEY1].theString() == D2);

                ASSERTV(CONFIG, tam.isTotalSame());
            }

            {
                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                    if (veryVeryVerbose) { T_ T_ Q(ExceptionTestBody) }

                    bslma::TestAllocatorMonitor tam(&oa);
                    mX[KEY1].makeString(A2);
                    ASSERTV(CONFIG, tam.isInUseUp());

#ifdef BDE_BUILD_TARGET_EXC
                    ASSERTV(CONFIG, 0 < EXCEPTION_COUNT);
#endif
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                ASSERTV(CONFIG, X[KEY0].type(), X[KEY0].isNumber());
                {
                    int value;
                    int status = X[KEY0].theNumber().asInt(&value);
                    ASSERTV(status, 0 == status);
                    ASSERTV(CONFIG, D1, X[KEY0].theNumber(), D1 == value);
                }
                ASSERTV(CONFIG, X[KEY1].type(), X[KEY1].isString());
                ASSERTV(CONFIG,
                        A2,
                        X[KEY1].theString(),
                        X[KEY1].theString() == A2);

                bslma::TestAllocatorMonitor tam(&oa);

                mX[KEY1].makeString(B2);
                ASSERTV(CONFIG, X[KEY0].type(), X[KEY0].isNumber());
                {
                    int value;
                    int status = X[KEY0].theNumber().asInt(&value);
                    ASSERTV(status, 0 == status);
                    ASSERTV(CONFIG, D1, X[KEY0].theNumber(), D1 == value);
                }
                ASSERTV(CONFIG, X[KEY1].type(), X[KEY1].isString());
                ASSERTV(CONFIG,
                        B2,
                        X[KEY1].theString(),
                        X[KEY1].theString() == B2);

                mX[KEY1].makeString(D2);
                ASSERTV(CONFIG, X[KEY0].type(), X[KEY0].isNumber());
                {
                    int value;
                    int status = X[KEY0].theNumber().asInt(&value);
                    ASSERTV(status, 0 == status);
                    ASSERTV(CONFIG, D1, X[KEY0].theNumber(), D1 == value);
                }
                ASSERTV(CONFIG, X[KEY1].type(), X[KEY1].isString());
                ASSERTV(CONFIG,
                        D2,
                        X[KEY1].theString(),
                        X[KEY1].theString() == D2);

                ASSERTV(CONFIG, tam.isTotalSame());
            }
            // Verify no temporary memory is allocated from the object
            // allocator.  Three for map storage, one for string A2.

            ASSERTV(CONFIG, oa.numBlocksMax(), 4 == oa.numBlocksMax());

            // Reclaim dynamically allocated object under test.

            fa.deleteObject(objPtr);

            // Verify all memory is released on object destruction.

            ASSERTV(fa.numBlocksInUse(),  0 ==  fa.numBlocksInUse());
            ASSERTV(oa.numBlocksInUse(),  0 ==  oa.numBlocksInUse());
            ASSERTV(noa.numBlocksTotal(), 0 == noa.numBlocksTotal());

            // Double check that some object memory was allocated.

            ASSERTV(CONFIG, 1 <= oa.numBlocksTotal());

            // Note that memory should be independently allocated for each
            // attribute capable of allocating memory.
        }
      } break;
      case 14: {
        // --------------------------------------------------------------------
        // JSONARRAY ACCESSORS
        //
        // Concerns:
        //: 1 Method 'maxSize' returns an appropriate value.
        //
        // Plan:
        //: 1 Create a 'const' 'JsonArray' object and invoke 'maxSize()' on it,
        //:   asserting a reasonably large value.
        //
        // Testing:
        //   size_t JsonArray::maxSize() const;
        // --------------------------------------------------------------------

        typedef JsonArray Obj;

        if (verbose) cout << "\n" "JSONARRAY ACCESSORS" "\n"
                                  "===================" "\n";

        const Obj X;

        ASSERT(1000 < X.maxSize());
      } break;
      case 13: {
        // --------------------------------------------------------------------
        // JSONARRAY MANIPULATORS
        //
        // Concerns:
        //: 1 Manipulators for JsonArray that modify the object directly leave
        //:   o Initializer list assignment operator
        //:   o Two overloads of 'resize'
        //:   o Two overloads of 'assign'
        //:   o Three overloads of 'erase'
        //:   o Six overloads of 'insert'
        //:   o 'popBack()'
        //:   o 'clear()'
        //:
        //: 2 Manipulators for JsonArray that return reference or iterator
        //:   offering modifiable access to a contained object allow the
        //:   contained object to be modified.
        //:
        //:   o 'operator[](size_t)'
        //:   o 'front()'
        //:   o 'back()'
        //:   o 'begin()'
        //:   o 'end()'
        //:
        //: 3 Manipulators for JsonArray that modify the object directly and
        //:   also return a value return the expected value.
        //:
        //:   o Three overloads of 'erase'
        //:   o Six overloads of 'insert'
        //:
        //: 4 Manipulators for JsonArray do not allocate temporary memory.
        //
        // Plan:
        //: 1 Create a 'bslma::TestAllocator' and install it as the default
        //:   allocator.  Create another 'bslma::TestAllocator' dedicated to
        //:   the constant target value.
        //:
        //: 2 Construct a 'const' 'JsonArray' object 'K' with the target value
        //:   that each modification operation will be intended to produce,
        //:   using the dedicated allocator.
        //:
        //: 3 Construct a 'JsonArray' object using an initializer-list, with
        //:   value the same as the target object 'K'.  Assert that the value
        //:   matches expectation.
        //:
        //: 4 In a for loop:
        //:
        //:   1 Create a 'JsonArray' object that requires one contained object
        //:     to be modified in order to evaluate equal to 'K'.
        //:
        //:   2 Using a switch statement, apply one of each of the manipulators
        //:     that serve to modify contained values in place, assigning the
        //:     contained object's value to that of the object in 'K'.
        //:
        //:   3 Assert that the value of the modified object matches that of
        //:     'K', and that allocations were as expected.
        //:
        //: 5 In a for loop:
        //:
        //:   1 Create a 'JsonArray' object that requires one contained object
        //:     to be removed to evaluate equal to 'K'.
        //:
        //:   2 Using a switch statement, apply one of each of the manipulators
        //:     that serve to remove objects from the container, remove the
        //:     contained object that is not contained in 'K'.
        //:
        //:   3 For overloads of 'erase', capture the returned value and assert
        //:     that it evaluates equal to 'end()'.
        //:
        //:   4 Assert that the value of the modified object matches that of
        //:     'K', and that allocations were as expected.
        //:
        //: 5 In a for loop:
        //:
        //:   1 Create a 'JsonArray' object that requires one contained object
        //:     to be added to evaluate equal to 'K'.
        //:
        //:   2 Using a switch statement, apply one of each of the manipulators
        //:     that serve to add objects to the container, add the object that
        //:     is contained in 'K'.
        //:
        //:   3 For overloads of 'insert', capture the returned value and
        //:     assert that it evaluates equal to 'begin()'.
        //:
        //:   4 Assert that the value of the modified object matches that of
        //:     'K', and that allocations were as expected.
        //:
        //: 6 Create a 'JsonArray' object that contains some elements.  Invoke
        //:   'clear' on that object, and assert that the object is now empty.
        //
        // Testing:
        //   JsonArray(initializer_list<Json> l, *a);
        //   JsonArray::operator=(initializer_list<Json> l);
        //   Json& JsonArray::operator[](size_t i);
        //   JsonArray::assign(initializer_list<Json> l);
        //   JsonArray::assign(INPUT_ITERATOR first, INPUT_ITERATOR last);
        //   Iterator JsonArray::begin();
        //   Iterator JsonArray::end();
        //   Json& JsonArray::front();
        //   Json& JsonArray::back();
        //   JsonArray::resize(size_t c);
        //   Iter JsonArray::erase(ConstIter p);
        //   Iter JsonArray::erase(ConstIter f, ConstIter l);
        //   Iter JsonArray::erase(size_t i);
        //   JsonArray::popBack();
        //   JsonArray::resize(size_t c, const Json& j);
        //   Iter JsonArray::insert(size_t i, const Json& j);
        //   Iter JsonArray::insert(size_t i, INPUT_ITER f, INPUT_ITER l);
        //   Iter JsonArray::insert(size_t i, MovableRef<Json> j);
        //   Iter JsonArray::insert(ConstIter p, const Json& j);
        //   Iter JsonArray::insert(ConstIter p, INPUT_ITER f, INPUT_ITER l);
        //   Iter JsonArray::insert(ConstIter p, MovableRef<Json> j);
        //   JsonArray::pushBack(MovableRef<Json> j);
        //   JsonArray::clear();
        // --------------------------------------------------------------------

        typedef JsonArray Obj;

        if (verbose) cout << "\n" "JSONARRAY MANIPULATORS" "\n"
                                  "======================" "\n";

        bslma::TestAllocator         ka("constant", veryVeryVeryVerbose);
        bslma::TestAllocator         da("default",  veryVeryVeryVerbose);
        bslma::DefaultAllocatorGuard dag(&da);

        Json GOOD(true);
        Json BAD(false);

        const Obj K(&GOOD, &GOOD + 1, &ka);

#ifdef BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS
        {
            bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);

            const Obj X({Json(true)}, &sa);

            ASSERTV(K, X, K == X);
            ASSERTV(da.numBlocksTotal(), 0 == da.numBlocksTotal());
            ASSERTV(sa.numBlocksTotal(), 1 == sa.numBlocksTotal());
        }
#endif

        for (char cfg = 'a'; cfg <= 'h'; ++cfg) {
            const char CONFIG = cfg;

            bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);

            Obj mX(&sa);  const Obj& X = mX;
            mX.pushBack(BAD);

            switch (CONFIG) {
              case 'a': {
                if (veryVerbose) cout << "Json& operator[](size_t i);\n";
                mX[0] = GOOD;
              } break;
              case 'b': {
#ifdef BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS
                if (veryVerbose) cout <<
                                       "operator=(initializer_list<Json> l)\n";
                Obj& r = (mX = {GOOD});
                ASSERT(&r == &mX);
#else
                if (veryVerbose) cout << "init list assignment elided.\n";
                continue;
#endif
              } break;
              case 'c': {
#ifdef BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS
                if (veryVerbose) cout << "assign(initializer_list<Json> l);\n";
                mX.assign({GOOD});
#else
                if (veryVerbose) cout << "init list assign member elided.\n";
                continue;
#endif
              } break;
              case 'd': {
                if (veryVerbose) cout <<
                        "assign(INPUT_ITERATOR first, INPUT_ITERATOR last);\n";
                mX.assign(&GOOD, &GOOD + 1);
              } break;
              case 'e': {
                if (veryVerbose) cout << "Iterator begin();\n";
                *(mX.begin()) = GOOD;
              } break;
              case 'f': {
                if (veryVerbose) cout << "Iterator end();\n";
                *(mX.end() - 1) = GOOD;
              } break;
              case 'g': {
                if (veryVerbose) cout << "Json& front();\n";
                mX.front() = GOOD;
              } break;
              case 'h': {
                if (veryVerbose) cout << "Json& back();\n";
                mX.back() = GOOD;
              } break;
              default: {
                BSLS_ASSERT_OPT(!"Bad test case config.");
              } break;
            }
            ASSERTV(CONFIG, K, X, K == X);
            ASSERTV(da.numBlocksTotal(), 0 == da.numBlocksTotal());
        }
        for (char cfg = 'i'; cfg <= 'm'; ++cfg) {
            const char CONFIG = cfg;

            bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);

            Obj mX(&sa);  const Obj& X = mX;
            mX.pushBack(GOOD);
            mX.pushBack(BAD);

            switch (CONFIG) {
              case 'i': {
                if (veryVerbose) cout << "resize(size_t c);\n";
                mX.resize(1);
              } break;
              case 'j': {
                if (veryVerbose) cout << "Iter erase(ConstIter p);\n";
                Obj::Iterator r = mX.erase(X.begin() + 1);
                ASSERT(r == mX.end());
              } break;
              case 'k': {
                if (veryVerbose) cout <<
                                     "Iter erase(ConstIter f, ConstIter l);\n";
                Obj::Iterator r = mX.erase(X.begin() + 1, X.end());
                ASSERT(r == mX.end());
              } break;
              case 'l': {
                if (veryVerbose) cout << "Iter erase(size_t i);\n";
                Obj::Iterator r = mX.erase(1);
                ASSERT(r == mX.end());
              } break;
              case 'm': {
                if (veryVerbose) cout << "popBack();\n";
                mX.popBack();
              } break;
              default: {
                BSLS_ASSERT_OPT(!"Bad test case config.");
              } break;
            }
            ASSERTV(CONFIG, K, X, K == X);
            ASSERTV(da.numBlocksTotal(), 0 == da.numBlocksTotal());
        }
        for (char cfg = 'n'; cfg <= 't'; ++cfg) {
            const char        CONFIG = cfg;
            const bsl::size_t index = 0;

            bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);

            Obj mX(&sa);  const Obj& X = mX;

            switch (CONFIG) {
              case 'n': {
                if (veryVerbose) cout << "resize(size_t c, const Json& j);\n";
                mX.resize(1, GOOD);
              } break;
              case 'o': {
                if (veryVerbose) cout <<
                                     "Iter insert(size_t i, const Json& j);\n";
                Obj::Iterator r = mX.insert(index, GOOD);
                ASSERT(r == mX.begin());
              } break;
              case 'p': {
                if (veryVerbose) cout <<
                        "Iter insert(size_t i, INPUT_ITER f, INPUT_ITER l);\n";
                Obj::Iterator r = mX.insert(index, &GOOD, &GOOD + 1);
                ASSERT(r == mX.begin());
              } break;
              case 'q': {
                if (veryVerbose) cout <<
                                "Iter insert(size_t i, MovableRef<Json> j);\n";
                Json from = GOOD;

                Obj::Iterator r = mX.insert(index,
                                            bslmf::MovableRefUtil::move(from));
                ASSERT(r == mX.begin());
              } break;
              case 'r': {
                if (veryVerbose) cout <<
                                  "Iter insert(ConstIter p, const Json& j);\n";
                Obj::Iterator r = mX.insert(X.begin(), GOOD);
                ASSERT(r == mX.begin());
              } break;
              case 's': {
                if (veryVerbose) cout <<
                     "Iter insert(ConstIter p, INPUT_ITER f, INPUT_ITER l);\n";
                Obj::Iterator r = mX.insert(X.begin(), &GOOD, &GOOD + 1);
                ASSERT(r == mX.begin());
              } break;
              case 't': {
                if (veryVerbose) cout <<
                             "Iter insert(ConstIter p, MovableRef<Json> j);\n";
                Json from = GOOD;

                Obj::Iterator r = mX.insert(X.begin(),
                                            bslmf::MovableRefUtil::move(from));
                ASSERT(r == mX.begin());
              } break;
              default: {
                BSLS_ASSERT_OPT(!"Bad test case config.");
              } break;
            }
            ASSERTV(CONFIG, K, X, K == X);
            ASSERTV(da.numBlocksTotal(), 0 == da.numBlocksTotal());
        }
        {
            bslma::TestAllocator sa("scratch", veryVeryVeryVerbose);

            Obj mX(&sa);
            mX.pushBack(BAD);

            if (veryVerbose) cout << "clear();\n";
            mX.clear();

            ASSERT(0 == mX.size());
            ASSERTV(da.numBlocksTotal(), 0 == da.numBlocksTotal());
        }
      } break;
      case 12: {
        // --------------------------------------------------------------------
        // JSONARRAY BSLX STREAMING
        //   Ensure that we can serialize the value of any object of the class,
        //   and then deserialize that value back into any object of the class.
        //
        // Concerns:
        //: 1 N/A
        //
        // Plan:
        //: 1 N/A
        //
        // Testing:
        //   Reserved for 'bslx' streaming.
        // --------------------------------------------------------------------

        if (verbose) cout << "JSONARRAY BSLX STREAMING" << endl
                          << "========================" << endl;

        if (verbose) cout << "Not yet implemented." << endl;

      } break;
      case 11: {
        // --------------------------------------------------------------------
        // JSONARRAY MOVE-ASSIGNMENT OPERATOR
        //   Ensure that we can move the value of any object of the class to
        //   any object of the class, such that the target object subsequently
        //   has the source value, and there are no additional allocations if
        //   only one allocator is being used, and the source object is
        //   unchanged if allocators are different.
        //
        // Concerns:
        //: 1 The move assignment operator can change the value of any
        //:   modifiable target object to that of any source object.
        //:
        //: 2 The allocator used by the target object is unchanged.
        //:
        //: 3 Any memory allocation is from the target object's allocator.
        //:
        //: 4 The signature and return type are standard.
        //:
        //: 5 The reference returned is to the target object (i.e., '*this').
        //:
        //: 6 If the allocators are different, the value of the source object
        //:   is not modified.
        //:
        //: 7 If the allocators are the same, no new allocations happen when
        //:   the move assignment happens.
        //:
        //: 8 The allocator used by the source object is unchanged.
        //:
        //: 9 Any memory allocation is exception neutral.
        //:
        //:10 Assigning an object to itself behaves as expected (alias-safety).
        //:
        //:11 Every object releases any allocated memory at destruction.
        //
        // Plan:
        //: 1 Use the address of 'operator=' to initialize a member-function
        //:   pointer having the appropriate signature and return type for the
        //:   copy-assignment operator defined in this component.  (C-4)
        //:
        //: 2 Create a 'bslma::TestAllocator' object, and install it as the
        //:   default allocator (note that a ubiquitous test allocator is
        //:   already installed as the global allocator).  Create a separate
        //:   'bslma::TestAllocator' object for arrays needed to initialize
        //:   objects under test.
        //:
        //: 3 Using the table-driven technique:
        //:
        //:   1 Specify a set of (unique) valid object values (one per row) in
        //:     terms of three different Json types, including values that
        //:     should require allocation where applicable.
        //:
        //:   2 Additionally, provide a (tri-valued) column, 'MEM', indicating
        //:     the expectation of memory allocation for all typical
        //:     implementations of individual attribute types: ('Y') "Yes",
        //:     ('N') "No", or ('?') "implementation-dependent".
        //:
        //: 4 For each row 'R1' (representing a distinct object value, 'V') in
        //:   the table described in P-3:  (C-1..3, 5-6,8-11)
        //:
        //:   1 Use the value constructor and a "scratch" allocator to create
        //:     two 'const' 'Obj', 'Z' and 'ZZ', each having the value 'V'.
        //:
        //:   2 Execute an inner loop that iterates over each row 'R2'
        //:     (representing a distinct object value, 'W') in the table
        //:     described in P-3:
        //:
        //:   3 For each of the iterations (P-4.2):  (C-1..2, 5..8, 11)
        //:
        //:     1 Create a 'bslma::TestAllocator' objects 's1'.
        //:
        //:     2 Use the value constructor and 's1' to create a modifiable
        //:       'Obj', 'mF', having the value 'V'.
        //:
        //:     3 Use the value constructor and 's1' to create a modifiable
        //:       'Obj', 'mX', having the value 'W'.
        //:
        //:     4 Move-assign 'mX' from 'bslmf::MovableRefUtil::move(mF)'.
        //:
        //:     5 Verify that the address of the return value is the same as
        //:       that of 'mX'.  (C-5)
        //:
        //:     6 Use the equality-comparison operator to verify that the
        //:       target object, 'mX', now has the same value as that of 'Z'.
        //:
        //:     7 Use the 'allocator' accessor of both 'mX' and 'mF' to verify
        //:       that the respective allocators used by the target and source
        //:       objects are unchanged.  (C-2, 7)
        //:
        //:     8 Use the appropriate test allocators to verify that no new
        //:       allocations were made by the move assignment operation.
        //:
        //:   4 For each of the iterations (P-4.2):  (C-1..2, 5, 7-9, 11)
        //:
        //:     1 Create two 'bslma::TestAllocator' objects 's1' and 's2'.
        //:
        //:     2 Use the value constructor and 's1' to create a modifiable
        //:       'Obj', 'mF', having the value 'V'.
        //:
        //:     3 Use the value constructor and 's2' to create a modifiable
        //:       'Obj', 'mX', having the value 'W'.
        //:
        //:     4 Move-assign 'mX' from 'bslmf::MovableRefUtil::move(mF)'.
        //:
        //:     5 Verify that the address of the return value is the same as
        //:       that of 'mX'.  (C-5)
        //:
        //:     6 Use the equality-comparison operator to verify that the
        //:       target object, 'mX', now has the same value as that of 'Z'.
        //:
        //:     7 Use the equality-comparison operator to verify that the
        //:       source object, 'mF', now has the same value as that of 'Z'.
        //:
        //:     8 Use the 'allocator' accessor of both 'mX' and 'mF' to verify
        //:       that the respective allocators used by the target and source
        //:       objects are unchanged.  (C-2, 7)
        //:
        //: 5 Repeat steps similar to those described in P-2 except that, this
        //:   time, there is no inner loop (as in P-4.2); instead, the source
        //:   object, 'Z', is a reference to the target object, 'mX', and both
        //:   'mX' and 'ZZ' are initialized to have the value 'V'.  For each
        //:   row (representing a distinct object value, 'V') in the table
        //:   described in P-3:  (C-10)
        //:
        //:   1 Create a 'bslma::TestAllocator' object, 'oa'.
        //:
        //:   2 Use the value constructor and 'oa' to create a modifiable 'Obj'
        //:     'mX'; also use the value constructor and a distinct "scratch"
        //:     allocator to create a 'const' 'Obj' 'ZZ'.
        //:
        //:   3 Let 'Z' be a  reference to 'mX'.
        //:
        //:   4 Assign 'mX' from 'bslmf::MovableRefUtil::move(Z)'.
        //:
        //:   5 Verify that the address of the return value is the same as that
        //:     of 'mX'.
        //:
        //:   6 Use the equality-comparison operator to verify that the
        //:     target object, 'Z', still has the same value as that of 'ZZ'.
        //:     (C-10)
        //:
        //:   7 Use the 'allocator' accessor of 'mX' to verify that it is still
        //:     the object allocator.
        //:
        //:   8 Use the appropriate test allocators to verify that:
        //:
        //:     1 Any memory that is allocated is from the object allocator.
        //:
        //:     2 No additional (e.g., temporary) object memory is allocated
        //:       when assigning an object value that did NOT initially require
        //:       allocated memory.
        //:
        //:     3 All object memory is released when the object is destroyed.
        //:
        //: 6 Use the test allocator from P-2 to verify that no memory is ever
        //:   allocated from the default allocator.  (C-3)
        //
        // Testing:
        //   JsonArray::operator=(MovableRef<JsonArray> rhs);
        // --------------------------------------------------------------------

        typedef bdljsn::JsonArray Obj;

        if (verbose) cout << endl
                          << "JSONARRAY MOVE-ASSIGNMENT OPERATOR" << endl
                          << "==================================" << endl;

        if (verbose) cout <<
                 "\nAssign the address of the operator to a variable." << endl;
        {
            typedef Obj& (Obj::*operatorPtr)(bslmf::MovableRef<Obj>);

            // Verify that the signature and return type are standard.

            operatorPtr operatorAssignment = &Obj::operator=;

            (void)operatorAssignment;  // quash potential compiler warning
        }

        if (verbose) cout <<
            "\nCreate a test allocator and install it as the default." << endl;

        bslma::TestAllocator         ia("initializer", veryVeryVeryVerbose);
        bslma::TestAllocator         da("default", veryVeryVeryVerbose);
        bslma::DefaultAllocatorGuard dag(&da);

        if (verbose) cout <<
           "\nUse a table of distinct object values and expected memory usage."
                                                                       << endl;
        const int              NUM_DATA        = DEFAULT_NUM_DATA;
        const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

        bool anyObjectMemoryAllocatedFlag = false;  // We later check that
                                                    // this test allocates
                                                    // some object memory.
        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int          LINE1   = DATA[ti].d_line;
            const char         MEMSRC1 = DATA[ti].d_mem;
            const bool         BOOL1     = DATA[ti].d_bool;
            const char *const  NUMBER1   = DATA[ti].d_number_p;
            const char *const  STRING1   = DATA[ti].d_string_p;

            const Json ARRAY1[3] = {Json(BOOL1, &ia),
                                    Json(JsonNumber(NUMBER1, &ia), &ia),
                                    Json(bsl::string(STRING1, &ia) , &ia)};

            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

            const Obj Z (ARRAY1, ARRAY1 + 3, &scratch);
            const Obj ZZ(ARRAY1, ARRAY1 + 3, &scratch);

            if (veryVerbose) { T_ P_(LINE1) P_(Z) P(ZZ) }

            // move assignment with the same allocator

            for (int tj = 0; tj < NUM_DATA; ++tj) {
                const int          LINE2   = DATA[tj].d_line;
                const bool         BOOL2     = DATA[tj].d_bool;
                const char *const  NUMBER2   = DATA[tj].d_number_p;
                const char *const  STRING2   = DATA[tj].d_string_p;

                const Json ARRAY2[3] = {Json(BOOL2 , &ia),
                                        Json(JsonNumber(NUMBER2 , &ia) , &ia),
                                        Json(bsl::string(STRING2, &ia) , &ia)};

                bslma::TestAllocator s1("scratch1", veryVeryVeryVerbose);

                {
                    // Test move assignment with same allocator.

                    Obj mF(ARRAY1, ARRAY1 + 3, &s1); const Obj& F=mF;
                    Obj mX(ARRAY2, ARRAY2 + 3, &s1); const Obj& X=mX;

                    if (veryVerbose) { T_ P_(LINE2) P(F) P(X) }

                    ASSERTV(LINE1, LINE2, F, X, (F == X) == (LINE1 == LINE2));

                    bslma::TestAllocatorMonitor s1m(&s1);

                    Obj *mR = &(mX = bslmf::MovableRefUtil::move(mF));
                    ASSERTV(LINE1, LINE2,  Z,   X,  Z == X);
                    ASSERTV(LINE1, LINE2, mR, &mX, mR == &mX);

                    ASSERTV(LINE1, LINE2, s1m.isTotalSame());

                    ASSERTV(LINE1, LINE2, &s1, X.allocator(),
                            &s1 == X.allocator());
                    ASSERTV(LINE1, LINE2, &s1, F.allocator(),
                            &s1 == F.allocator());

                    anyObjectMemoryAllocatedFlag |= !!s1.numBlocksInUse();
                }

                // Verify all memory is released on object destruction.

                ASSERTV(LINE1, LINE2, s1.numBlocksInUse(),
                        0 == s1.numBlocksInUse());
            }

            // move assignment with different allocators

            for (int tj = 0; tj < NUM_DATA; ++tj) {
                const int         LINE2     = DATA[tj].d_line;
                const char        MEMDST2   = DATA[tj].d_mem;
                const bool        BOOL2     = DATA[tj].d_bool;
                const char *const NUMBER2   = DATA[tj].d_number_p;
                const char *const STRING2   = DATA[tj].d_string_p;

                const Json ARRAY2[3] = {Json(BOOL2 , &ia),
                                        Json(JsonNumber(NUMBER2 , &ia) , &ia),
                                        Json(bsl::string(STRING2, &ia) , &ia)};

                bslma::TestAllocator s1("scratch1", veryVeryVeryVerbose);
                bslma::TestAllocator s2("scratch2", veryVeryVeryVerbose);

                {
                    // Test move assignment with different allocator

                    Obj mF(ARRAY1, ARRAY1 + 3, &s1); const Obj& F=mF;
                    Obj mX(ARRAY2, ARRAY2 + 3, &s2); const Obj& X=mX;

                    if (veryVerbose) { T_ P_(LINE2) P(F) P(X) }

                    ASSERTV(LINE1, LINE2, F, X, (F == X) == (LINE1 == LINE2));


                    BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(s2) {
                        if (veryVeryVerbose) { T_ T_ Q(ExceptionTestBody) }

                        Obj *mR = &(mX = bslmf::MovableRefUtil::move(mF));
                        ASSERTV(LINE1, LINE2,  Z,   X,  Z == X);
                        ASSERTV(LINE1, LINE2, mR, &mX, mR == &mX);

                        ASSERTV(LINE1, LINE2,  Z,   F,  Z == F);

                        ASSERTV(LINE1, LINE2, &s2, X.allocator(),
                               &s2 == X.allocator());
                        ASSERTV(LINE1, LINE2, &s1, F.allocator(),
                               &s1 == F.allocator());


#ifdef BDE_BUILD_TARGET_EXC
                        if ('N' == MEMDST2 && 'Y' == MEMSRC1) {
                            ASSERTV(LINE1, LINE2, 0 < EXCEPTION_COUNT);
                        }
#endif
                    } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                    anyObjectMemoryAllocatedFlag |= !!s1.numBlocksInUse();
                }

                // Verify all memory is released on object destruction.

                ASSERTV(LINE1, LINE2, s1.numBlocksInUse(),
                        0 == s1.numBlocksInUse());
                ASSERTV(LINE1, LINE2, s2.numBlocksInUse(),
                        0 == s2.numBlocksInUse());
            }

            // self-assignment

            bslma::TestAllocator oa("object", veryVeryVeryVerbose);

            {
                bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

                Obj       mX(ARRAY1, ARRAY1 + 3, &oa);
                const Obj ZZ(ARRAY1, ARRAY1 + 3, &scratch);

                Obj& Z = mX;

                ASSERTV(LINE1, ZZ, Z, ZZ == Z);

                bslma::TestAllocatorMonitor oam(&oa);

                Obj *mR = &(mX = bslmf::MovableRefUtil::move(Z));
                ASSERTV(LINE1, ZZ,   Z, ZZ == Z);
                ASSERTV(LINE1, mR, &mX, mR == &mX);

                ASSERTV(LINE1, &oa, Z.allocator(), &oa == Z.allocator());

                ASSERTV(LINE1, oam.isTotalSame());

                ASSERTV(LINE1, 0 == da.numBlocksTotal());
            }

            // Verify all object memory is released on destruction.

            ASSERTV(LINE1, oa.numBlocksInUse(), 0 == oa.numBlocksInUse());
        }

        // Double check that some object memory was allocated.

        ASSERT(anyObjectMemoryAllocatedFlag);
      } break;
      case 10: {
        // --------------------------------------------------------------------
        // JSONARRAY COPY-ASSIGNMENT OPERATOR
        //   Ensure that we can assign the value of any object of the class to
        //   any object of the class, such that the two objects subsequently
        //   have the same value.
        //
        // Concerns:
        //: 1 The assignment operator can change the value of any modifiable
        //:   target object to that of any source object.
        //:
        //: 2 The allocator used by the target object is unchanged.
        //:
        //: 3 Any memory allocation is from the target object's allocator.
        //:
        //: 4 The signature and return type are standard.
        //:
        //: 5 The reference returned is to the target object (i.e., '*this').
        //:
        //: 6 The value of the source object is not modified.
        //:
        //: 7 The allocator used by the source object is unchanged.
        //:
        //: 8 QoI: Assigning a source object having the default-constructed
        //:   value allocates no memory.
        //:
        //: 9 Any memory allocation is exception neutral.
        //:
        //:10 Assigning an object to itself behaves as expected (alias-safety).
        //:
        //:11 Every object releases any allocated memory at destruction.
        //
        // Plan:
        //: 1 Use the address of 'operator=' to initialize a member-function
        //:   pointer having the appropriate signature and return type for the
        //:   copy-assignment operator defined in this component.  (C-4)
        //:
        //: 2 Create a 'bslma::TestAllocator' object, and install it as the
        //:   default allocator (note that a ubiquitous test allocator is
        //:   already installed as the global allocator).  Create a separate
        //:   'bslma::TestAllocator' object for arrays needed to initialize
        //:   objects under test.
        //:
        //: 3 Using the table-driven technique:
        //:
        //:   1 Specify a set of (unique) valid object values (one per row) in
        //:     terms of three different Json types, including values that
        //:     should require allocation where applicable.
        //:
        //:   2 Additionally, provide a (tri-valued) column, 'MEM', indicating
        //:     the expectation of memory allocation for all typical
        //:     implementations of individual attribute types: ('Y') "Yes",
        //:     ('N') "No", or ('?') "implementation-dependent".
        //:
        //: 4 For each row 'R1' (representing a distinct object value, 'V') in
        //:   the table described in P-3:  (C-1..2, 5..8, 11)
        //:
        //:   1 Use the value constructor and a "scratch" allocator to create
        //:     two 'const' 'Obj', 'Z' and 'ZZ', each having the value 'V'.
        //:
        //:   2 Execute an inner loop that iterates over each row 'R2'
        //:     (representing a distinct object value, 'W') in the table
        //:     described in P-3:
        //:
        //:   3 For each of the iterations (P-4.2):  (C-1..2, 5..8, 11)
        //:
        //:     1 Create a 'bslma::TestAllocator' object, 'oa'.
        //:
        //:     2 Use the value constructor and 'oa' to create a modifiable
        //:       'Obj', 'mX', having the value 'W'.
        //:
        //:     3 Assign 'mX' from 'Z' in the presence of injected exceptions
        //:       (using the 'BSLMA_TESTALLOCATOR_EXCEPTION_TEST_*' macros).
        //:
        //:     4 Verify that the address of the return value is the same as
        //:       that of 'mX'.  (C-5)
        //:
        //:     5 Use the equality-comparison operator to verify that: (C-1, 6)
        //:
        //:       1 The target object, 'mX', now has the same value as that of
        //:         'Z'.  (C-1)
        //:
        //:       2 'Z' still has the same value as that of 'ZZ'.  (C-6)
        //:
        //:     6 Use the 'allocator' accessor of both 'mX' and 'Z' to verify
        //:       that the respective allocators used by the target and source
        //:       objects are unchanged.  (C-2, 7)
        //:
        //:     7 Use the appropriate test allocators to verify that:
        //:       (C-8, 11)
        //:
        //:       1 For an object that (a) is initialized with a value that did
        //:         NOT require memory allocation, and (b) is then assigned a
        //:         value that DID require memory allocation, the target object
        //:         DOES allocate memory from its object allocator only
        //:         (irrespective of the specific number of allocations or the
        //:         total amount of memory allocated); also cross check with
        //:         what is expected for 'mX' and 'Z'.
        //:
        //:       2 An object that is assigned a value that did NOT require
        //:         memory allocation, does NOT allocate memory from its object
        //:         allocator; also cross check with what is expected for 'Z'.
        //:
        //:       3 No additional memory is allocated by the source object.
        //:         (C-8)
        //:
        //:       4 All object memory is released when the object is destroyed.
        //:         (C-11)
        //:
        //: 5 Repeat steps similar to those described in P-2 except that, this
        //:   time, there is no inner loop (as in P-4.2); instead, the source
        //:   object, 'Z', is a reference to the target object, 'mX', and both
        //:   'mX' and 'ZZ' are initialized to have the value 'V'.  For each
        //:   row (representing a distinct object value, 'V') in the table
        //:   described in P-3:  (C-9..10)
        //:
        //:   1 Create a 'bslma::TestAllocator' object, 'oa'.
        //:
        //:   2 Use the value constructor and 'oa' to create a modifiable 'Obj'
        //:     'mX'; also use the value constructor and a distinct "scratch"
        //:     allocator to create a 'const' 'Obj' 'ZZ'.
        //:
        //:   3 Let 'Z' be a 'const' reference to 'mX'.
        //:
        //:   4 Assign 'mX' from 'Z' in the presence of injected exceptions
        //:     (using the 'BSLMA_TESTALLOCATOR_EXCEPTION_TEST_*' macros).
        //:     (C-9)
        //:
        //:   5 Verify that the address of the return value is the same as that
        //:     of 'mX'.
        //:
        //:   6 Use the equality-comparison operator to verify that the
        //:     target object, 'Z', still has the same value as that of 'ZZ'.
        //:     (C-10)
        //:
        //:   7 Use the 'allocator' accessor of 'mX' to verify that it is still
        //:     the object allocator.
        //:
        //:   8 Use the appropriate test allocators to verify that:
        //:
        //:     1 Any memory that is allocated is from the object allocator.
        //:
        //:     2 No additional (e.g., temporary) object memory is allocated
        //:       when assigning an object value that did NOT initially require
        //:       allocated memory.
        //:
        //:     3 All object memory is released when the object is destroyed.
        //:
        //: 6 Use the test allocator from P-2 to verify that no memory is ever
        //:   allocated from the default allocator.  (C-3)
        //
        // Testing:
        //   JsonArray::operator=(const JsonArray& rhs);
        //   CONCERN: All memory allocation is from the object's allocator.
        //   CONCERN: All memory allocation is exception neutral.
        //   CONCERN: Object value is independent of the object allocator.
        //   CONCERN: There is no temporary allocation from any allocator.
        // --------------------------------------------------------------------

        typedef bdljsn::JsonArray Obj;

        if (verbose) cout << endl
                          << "JSONARRAY COPY-ASSIGNMENT OPERATOR" << endl
                          << "==================================" << endl;

        if (verbose) cout <<
                 "\nAssign the address of the operator to a variable." << endl;
        {
            typedef Obj& (Obj::*operatorPtr)(const Obj&);

            // Verify that the signature and return type are standard.

            operatorPtr operatorAssignment = &Obj::operator=;

            (void)operatorAssignment;  // quash potential compiler warning
        }

        if (verbose) cout <<
            "\nCreate a test allocator and install it as the default." << endl;

        bslma::TestAllocator         ia("initializer", veryVeryVeryVerbose);
        bslma::TestAllocator         da("default", veryVeryVeryVerbose);
        bslma::DefaultAllocatorGuard dag(&da);

        if (verbose) cout <<
           "\nUse a table of distinct object values and expected memory usage."
                                                                       << endl;
        const int              NUM_DATA        = DEFAULT_NUM_DATA;
        const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

        bool anyObjectMemoryAllocatedFlag = false;  // We later check that
                                                    // this test allocates
                                                    // some object memory.
        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int          LINE1   = DATA[ti].d_line;
            const char         MEMSRC1 = DATA[ti].d_mem;
            const bool         BOOL1     = DATA[ti].d_bool;
            const char *const  NUMBER1   = DATA[ti].d_number_p;
            const char *const  STRING1   = DATA[ti].d_string_p;

            const Json ARRAY1[3] = {Json(BOOL1, &ia),
                                    Json(JsonNumber(NUMBER1, &ia), &ia),
                                    Json(bsl::string(STRING1, &ia) , &ia)};

            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

            const Obj Z (ARRAY1, ARRAY1 + 3, &scratch);
            const Obj ZZ(ARRAY1, ARRAY1 + 3, &scratch);

            if (veryVerbose) { T_ P_(LINE1) P_(Z) P(ZZ) }

            for (int tj = 0; tj < NUM_DATA; ++tj) {
                const int          LINE2   = DATA[tj].d_line;
                const char         MEMDST2 = DATA[tj].d_mem;
                const bool         BOOL2     = DATA[tj].d_bool;
                const char *const  NUMBER2   = DATA[tj].d_number_p;
                const char *const  STRING2   = DATA[tj].d_string_p;

                const Json ARRAY2[3] = {Json(BOOL2 , &ia),
                                        Json(JsonNumber(NUMBER2 , &ia) , &ia),
                                        Json(bsl::string(STRING2, &ia) , &ia)};

                bslma::TestAllocator oa("object", veryVeryVeryVerbose);

                {
                    Obj mX(ARRAY2, ARRAY2 + 3, &oa);  const Obj& X = mX;

                    if (veryVerbose) { T_ P_(LINE2) P(X) }

                    ASSERTV(LINE1, LINE2, Z, X, (Z == X) == (LINE1 == LINE2));

                    bslma::TestAllocatorMonitor oam(&oa), sam(&scratch);

                    BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                        if (veryVeryVerbose) { T_ T_ Q(ExceptionTestBody) }

                        Obj *mR = &(mX = Z);
                        ASSERTV(LINE1, LINE2,  Z,   X,  Z == X);
                        ASSERTV(LINE1, LINE2, mR, &mX, mR == &mX);


#ifdef BDE_BUILD_TARGET_EXC
                        if ('N' == MEMDST2 && 'Y' == MEMSRC1) {
                            ASSERTV(LINE1, LINE2, 0 < EXCEPTION_COUNT);
                        }
#endif

                    } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                    ASSERTV(LINE1, LINE2, ZZ, Z, ZZ == Z);

                    ASSERTV(LINE1, LINE2, &oa, X.allocator(),
                            &oa == X.allocator());
                    ASSERTV(LINE1, LINE2, &scratch, Z.allocator(),
                            &scratch == Z.allocator());

                    if ('N' == MEMDST2 && 'Y' == MEMSRC1) {
                        ASSERTV(LINE1, LINE2, oam.isInUseUp());
                    }
                    else if ('N' == MEMSRC1) {
                        ASSERTV(LINE1, LINE2, !oam.isInUseUp());
                    }

                    // Record if some object memory was allocated.

                    anyObjectMemoryAllocatedFlag |= !!oa.numBlocksInUse();

                    ASSERTV(LINE1, LINE2, sam.isInUseSame());

                    ASSERTV(LINE1, LINE2, 0 == da.numBlocksTotal());
                }

                // Verify all memory is released on object destruction.

                ASSERTV(LINE1, LINE2, oa.numBlocksInUse(),
                        0 == oa.numBlocksInUse());
            }

            // self-assignment

            bslma::TestAllocator oa("object", veryVeryVeryVerbose);

            {
                bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

                Obj       mX(ARRAY1, ARRAY1 + 3, &oa);
                const Obj ZZ(ARRAY1, ARRAY1 + 3, &scratch);

                const Obj& Z = mX;

                ASSERTV(LINE1, ZZ, Z, ZZ == Z);

                bslma::TestAllocatorMonitor oam(&oa);

                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                    if (veryVeryVerbose) { T_ T_ Q(ExceptionTestBody) }

                    Obj *mR = &(mX = Z);
                    ASSERTV(LINE1, ZZ,   Z, ZZ == Z);
                    ASSERTV(LINE1, mR, &mX, mR == &mX);
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                ASSERTV(LINE1, &oa, Z.allocator(), &oa == Z.allocator());

                ASSERTV(LINE1, !oam.isInUseUp());

                ASSERTV(LINE1, 0 == da.numBlocksTotal());
            }

            // Verify all object memory is released on destruction.

            ASSERTV(LINE1, oa.numBlocksInUse(), 0 == oa.numBlocksInUse());
        }

        // Double check that some object memory was allocated.

        ASSERT(anyObjectMemoryAllocatedFlag);
      } break;
      case 9: {
        // --------------------------------------------------------------------
        // JSONARRAY SWAP MEMBER AND FREE FUNCTIONS
        //   Ensure that the free 'swap' function is implemented and can
        //   exchange the values of any two objects.  Ensure that member
        //   'swap' is implemented and can exchange the values of any two
        //   objects that use the same allocator.
        //
        // Concerns:
        //: 1 Both functions exchange the values of the (two) supplied objects.
        //:
        //: 2 The common object allocator used by both objects is unchanged.
        //:
        //: 3 The member function does not allocate memory from any allocator;
        //:   nor does the free function when the two objects being swapped use
        //:   the same allocator.
        //:
        //: 4 The free function can be called with two objects that use
        //:   different allocators.
        //:
        //: 5 Both functions have standard signatures and return types.
        //:
        //: 6 Using either function to swap an object with itself does not
        //:   affect the value of the object (alias-safety).
        //:
        //: 7 The free 'swap' function is discoverable through ADL (Argument
        //:   Dependent Lookup).
        //:
        //: 8 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Use the addresses of the 'swap' member and free functions defined
        //:   in this component to initialize, respectively, member-function
        //:   and free-function pointers having the appropriate signatures and
        //:   return types.  (C-5)
        //:
        //: 2 Create a 'bslma::TestAllocator' object, and install it as the
        //:   default allocator (note that a ubiquitous test allocator is
        //:   already installed as the global allocator).  Provide another
        //:   allocator for objects from which objects under test are
        //:   initialized.
        //:
        //: 3 Using the table-driven technique:
        //:
        //:   1 Specify a set of (unique) valid object values (one per row) in
        //:     terms of three different Json types, including values that
        //:     should require allocation where applicable.
        //:
        //:   2 Additionally, provide a (tri-valued) column, 'MEM', indicating
        //:     the expectation of memory allocation for all typical
        //:     implementations of individual attribute types: ('Y') "Yes",
        //:     ('N') "No", or ('?') "implementation-dependent".
        //:
        //: 4 For each row 'R1' in the table of P-3:  (C1..2, 6)
        //:
        //:   1 Create a 'bslma::TestAllocator' object, 'oa'.
        //:
        //:   2 Use the value constructor and 'oa' to create a modifiable
        //:     'Obj', 'mW', having the value described by 'R1'; also use the
        //:     copy constructor and a "scratch" allocator to create a 'const'
        //:     'Obj' 'XX' from 'mW'.
        //:
        //:   3 Use the member and free 'swap' functions to swap the value of
        //:     'mW' with itself; verify, after each swap, that:  (C-6)
        //:
        //:     1 The value is unchanged.  (C-6)
        //:
        //:     2 The allocator used by the object is unchanged.
        //:
        //:     3 There was no additional object memory allocation.
        //:
        //:   4 For each row 'R2' in the table of P-3:  (C-1..2)
        //:
        //:     1 Use the copy constructor and 'oa' to create a modifiable
        //:       'Obj', 'mX', from 'XX' (P-4.2).
        //:
        //:     2 Use the value constructor and 'oa' to create a modifiable
        //:       'Obj', 'mY', having the value described by 'R2'; also use
        //:       the copy constructor to create, using a "scratch" allocator,
        //:       a 'const' 'Obj', 'YY', from 'Y'.
        //:
        //:     3 Use, in turn, the member and free 'swap' functions to swap
        //:       the values of 'mX' and 'mY'; verify, after each swap, that:
        //:       (C-1..2)
        //:
        //:       1 The values have been exchanged.  (C-1)
        //:
        //:       2 The common object allocator used by 'mX' and 'mY'
        //:         is unchanged in both objects.  (C-2)
        //:
        //:       3 There was no additional object memory allocation.
        //:
        //: 5 Verify that the free 'swap' function is discoverable through ADL:
        //:   (C-7)
        //:
        //:   1 Create a set of contained values, 'A', choosing values that
        //:     allocate memory.
        //:
        //:   2 Create a 'bslma::TestAllocator' object, 'oa'.
        //:
        //:   3 Use the default constructor and 'oa' to create a modifiable
        //:     'Obj' 'mX' (containing no elements); also use the copy
        //:     constructor and a "scratch" allocator to create a 'const' 'Obj'
        //:     'XX' from 'mX'.
        //:
        //:   4 Use the value constructor and 'oa' to create a modifiable 'Obj'
        //:     'mY' having the value described by the 'Ai' attributes; also
        //:     use the copy constructor and a "scratch" allocator to create a
        //:     'const' 'Obj' 'YY' from 'mY'.
        //:
        //:   5 Use the 'bslalg::SwapUtil' helper function template to swap the
        //:     values of 'mX' and 'mY', using the free 'swap' function defined
        //:     in this component, then verify that:  (C-7)
        //:
        //:     1 The values have been exchanged.
        //:
        //:     2 There was no additional object memory allocation.  (C-7)
        //:
        //: 6 Use the test allocator from P-2 to verify that no memory was
        //:   allocated from the default allocator.  (C-3)
        //:
        //: 7 Verify that free 'swap' exchanges the values of any two objects
        //:   that use different allocators.  (C-4)
        //:
        //: 8 Verify that, in appropriate build modes, defensive checks are
        //:   triggered when, using the member 'swap' function, an attempt is
        //:   made to swap objects that do not refer to the same allocator, but
        //:   not when the allocators are the same (using the
        //:   'BSLS_ASSERTTEST_*' macros).  (C-8)
        //
        // Testing:
        //   void JsonArray::swap(JsonArray& other);
        //   void swap(JsonArray& a, b);
        //   CONCERN: Precondition violations are detected when enabled.
        // --------------------------------------------------------------------

        typedef bdljsn::JsonArray Obj;

        if (verbose) cout << endl
                         << "JSONARRAY SWAP MEMBER AND FREE FUNCTIONS" << endl
                         << "========================================" << endl;

        if (verbose) cout <<
                "\nAssign the address of each function to a variable." << endl;
        {
            typedef void (Obj::*funcPtr)(Obj&);
            typedef void (*freeFuncPtr)(Obj&, Obj&);

            // Verify that the signatures and return types are standard.

            funcPtr     memberSwap = &Obj::swap;
            freeFuncPtr freeSwap   = bsl::swap;

            (void)memberSwap;  // quash potential compiler warnings
            (void)freeSwap;
        }

        if (verbose) cout <<
            "\nCreate a test allocator and install it as the default." << endl;

        bslma::TestAllocator         ia("initializer", veryVeryVeryVerbose);
        bslma::TestAllocator         da("default", veryVeryVeryVerbose);
        bslma::DefaultAllocatorGuard dag(&da);

        if (verbose) cout <<
           "\nUse a table of distinct object values and expected memory usage."
                                                                       << endl;
        const int              NUM_DATA        = DEFAULT_NUM_DATA;
        const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

        bool anyObjectMemoryAllocatedFlag = false;  // We later check that
                                                    // this test allocates
                                                    // some object memory.
        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int          LINE1     = DATA[ti].d_line;
            const char         MEM1      = DATA[ti].d_mem;
            const bool         BOOL1     = DATA[ti].d_bool;
            const char *const  NUMBER1   = DATA[ti].d_number_p;
            const char *const  STRING1   = DATA[ti].d_string_p;

            const Json ARRAY1[3] = {Json(BOOL1, &ia),
                                    Json(JsonNumber(NUMBER1, &ia), &ia),
                                    Json(bsl::string(STRING1, &ia) , &ia)};

            bslma::TestAllocator      oa("object",  veryVeryVeryVerbose);
            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

            Obj       mW(ARRAY1, ARRAY1+3, &oa); const Obj& W = mW;
            const Obj XX(W, &scratch);

            if (veryVerbose) { T_ P_(LINE1) P_(W) P(XX) }

            // member 'swap'
            {
                bslma::TestAllocatorMonitor oam(&oa);

                mW.swap(mW);

                ASSERTV(LINE1, XX, W, XX == W);
                ASSERTV(LINE1, &oa == W.allocator());
                ASSERTV(LINE1, oam.isTotalSame());
            }

            // free function 'swap'
            {
                bslma::TestAllocatorMonitor oam(&oa);

                swap(mW, mW);

                ASSERTV(LINE1, XX, W, XX == W);
                ASSERTV(LINE1, &oa == W.allocator());
                ASSERTV(LINE1, oam.isTotalSame());
            }

            // Verify expected ('Y'/'N') object-memory allocations.

            if ('?' != MEM1) {
                ASSERTV(LINE1, MEM1, oa.numBlocksInUse(),
                        ('N' == MEM1) == (1 == oa.numBlocksInUse()));
            }

            for (int tj = 0; tj < NUM_DATA; ++tj) {
                const int          LINE2     = DATA[tj].d_line;
                const bool         BOOL2     = DATA[tj].d_bool;
                const char *const  NUMBER2   = DATA[tj].d_number_p;
                const char *const  STRING2   = DATA[tj].d_string_p;

                const Json ARRAY2[3] = {Json(BOOL2 , &ia),
                                        Json(JsonNumber(NUMBER2 , &ia) , &ia),
                                        Json(bsl::string(STRING2, &ia) , &ia)};

                Obj       mX(XX, &oa);                const Obj& X = mX;
                Obj       mY(ARRAY2, ARRAY2+3, &oa);  const Obj& Y = mY;
                const Obj YY(Y, &scratch);

                if (veryVerbose) { T_ P_(LINE2) P_(X) P_(Y) P(YY) }

                // member 'swap'
                {
                    bslma::TestAllocatorMonitor oam(&oa);

                    mX.swap(mY);

                    ASSERTV(LINE1, LINE2, YY, X, YY == X);
                    ASSERTV(LINE1, LINE2, XX, Y, XX == Y);
                    ASSERTV(LINE1, LINE2, &oa == X.allocator());
                    ASSERTV(LINE1, LINE2, oam.isTotalSame());
                }

                // free function 'swap', same allocator
                {
                    bslma::TestAllocatorMonitor oam(&oa);

                    swap(mX, mY);

                    ASSERTV(LINE1, LINE2, XX, X, XX == X);
                    ASSERTV(LINE1, LINE2, YY, Y, YY == Y);
                    ASSERTV(LINE1, LINE2, &oa == X.allocator());
                    ASSERTV(LINE1, LINE2, oam.isTotalSame());
                }
            }

            // Record if some object memory was allocated.

            anyObjectMemoryAllocatedFlag |= !!oa.numBlocksInUse();
        }

        // Check that some memory was allocated.

        ASSERT(anyObjectMemoryAllocatedFlag);

        if (verbose) cout <<
                "\nInvoke free 'swap' function in a context where ADL is used."
                                                                       << endl;
        {
            bslma::TestAllocator      oa("object",  veryVeryVeryVerbose);
            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

            const Json ALLOCATING_DATA[] = {
                Json(bsl::string("a_" SUFFICIENTLY_LONG_STRING, &scratch),
                    &scratch),
                Json(JsonNumber(INT_LONG_STRING, &scratch), &scratch)};

            Obj       mX(&oa);  const Obj& X = mX;
            const Obj XX(X, &scratch);

            Obj       mY(ALLOCATING_DATA, ALLOCATING_DATA + 2, &oa);
            const Obj& Y = mY;
            const Obj YY(Y, &scratch);

            if (veryVeryVerbose) { T_ P_(X) P(Y) }

            bslma::TestAllocatorMonitor oam(&oa);

            bslalg::SwapUtil::swap(&mX, &mY);

            ASSERTV(YY, X, YY == X);
            ASSERTV(XX, Y, XX == Y);
            ASSERT(oam.isTotalSame());

            if (veryVeryVerbose) { T_ P_(X) P(Y) }
        }

        // Verify no memory is allocated from the default allocator.

        ASSERTV(da.numBlocksTotal(), 0 == da.numBlocksTotal());

        if (verbose) cout <<
            "\nFree 'swap' function with different allocators." << endl;
        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int          LINE1     = DATA[ti].d_line;
            const bool         BOOL1     = DATA[ti].d_bool;
            const char *const  NUMBER1   = DATA[ti].d_number_p;
            const char *const  STRING1   = DATA[ti].d_string_p;
            const Json         ARRAY1[3] = {Json(BOOL1),
                                            Json(JsonNumber(NUMBER1)),
                                            Json(STRING1)};

            bslma::TestAllocator      oa("object",  veryVeryVeryVerbose);
            bslma::TestAllocator     oa2("object2", veryVeryVeryVerbose);
            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

            Obj XX(ARRAY1, ARRAY1+3, &scratch);

            if (veryVerbose) { T_ P_(LINE1) P(XX) }

            for (int tj = 0; tj < NUM_DATA; ++tj) {
                const int          LINE2     = DATA[tj].d_line;
                const bool         BOOL2     = DATA[tj].d_bool;
                const char *const  NUMBER2   = DATA[tj].d_number_p;
                const char *const  STRING2   = DATA[tj].d_string_p;
                const Json         ARRAY2[3] = {Json(BOOL2),
                                                Json(JsonNumber(NUMBER2)),
                                                Json(STRING2)};

                Obj       mX(XX, &oa);                 const Obj& X = mX;
                Obj       mY(ARRAY2, ARRAY2+3, &oa2);  const Obj& Y = mY;
                const Obj YY(Y, &scratch);

                if (veryVerbose) { T_ P_(LINE2) P_(X) P_(Y) P(YY) }

                // free function 'swap', different allocator
                {
                    swap(mX, mY);

                    ASSERTV(LINE1, LINE2, YY, X, YY == X);
                    ASSERTV(LINE1, LINE2, XX, Y, XX == Y);
                    ASSERTV(LINE1, LINE2, &oa  == X.allocator());
                    ASSERTV(LINE1, LINE2, &oa2 == Y.allocator());
                }
            }
        }

#ifdef BDE_BUILD_TARGET_EXC
        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bsls::AssertTestHandlerGuard hG;

            if (verbose) cout << "\t'swap' member function" << endl;
            {
                bslma::TestAllocator oa1("object1", veryVeryVeryVerbose);
                bslma::TestAllocator oa2("object2", veryVeryVeryVerbose);

                Obj mA(&oa1);  Obj mB(&oa1);
                Obj mZ(&oa2);

                ASSERT_PASS(mA.swap(mB));
                ASSERT_FAIL(mA.swap(mZ));
            }
        }
#endif
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // JSONARRAY MOVE CONSTRUCTOR
        //   Ensure that we can create a distinct object of the class from any
        //   other one, such that the new object has the original value.
        //   Verify that if the same allocator is used there have been no new
        //   allocations, and if a different allocator is used the source
        //   object has the original value.
        //
        // Concerns:
        //: 1 The move constructor (with or without a supplied allocator)
        //:   creates an object having the same value as the original object
        //:   started with.
        //:
        //: 2 If an allocator is NOT supplied, the allocator of the new object
        //:   is the same as the original object, and no new allocations occur.
        //:
        //: 3 If an allocator is supplied that is the same as the original
        //:   object, then no new allocations occur.
        //:
        //: 4 If an allocator is supplied that is different from the original
        //:   object, then the original object's value remains unchanged.
        //:
        //: 5 Supplying a default-constructed allocator explicitly is the same
        //:   as supplying the default allocator.
        //:
        //: 6 Any memory allocation is from the object allocator.
        //:
        //: 7 There is no temporary memory allocation from any allocator.
        //:
        //: 8 Every object releases any allocated memory at destruction.
        //:
        //: 9 The allocator used by the original object is unchanged.
        //:
        //:10 Any memory allocation is exception neutral.
        //
        // Plan:
        //: 1 Using the table-driven technique:
        //:
        //:   1 Specify a set of (unique) valid object values (one per row) in
        //:     terms of three different Json types, including values that
        //:     should require allocation where applicable.
        //:
        //:   2 Additionally, provide a (tri-valued) column, 'MEM', indicating
        //:     the expectation of memory allocation for all typical
        //:     implementations of individual attribute types: ('Y') "Yes",
        //:     ('N') "No", or ('?') "implementation-dependent".
        //:
        //: 2 For each row (representing a distinct object value, 'V') in the
        //:   table described in P-1: (C-1..9)
        //:
        //:   1 Use the value constructor and a "scratch" allocator to create
        //:     two 'const' 'Obj', 'Z' and 'ZZ', each having the value 'V'.
        //:
        //:   2 Execute an inner loop that creates an object by
        //:     move-constructing from a newly created object with value V,
        //:     but invokes the move constructor differently in each
        //:     iteration: (a) using the standard single-argument move
        //:     constructor, (b) using the extended move constructor with a
        //:     default-constructed allocator argument (to use the default
        //:     allocator), (c) using the extended move constructor with the
        //:     same allocator as the moved-from object, and (d) using the
        //:     extended move constructor with a different allocator than the
        //:     moved-from object.
        //:
        //: 3 For each of these iterations (P-2.2):
        //:
        //:   1 Create four 'bslma::TestAllocator' objects, and install one as
        //:     the current default allocator (note that a ubiquitous test
        //:     allocator is already installed as the global allocator).
        //:
        //:   2 Dynamically allocate another object 'F" using the 's1'
        //:     allocator having the same value V, using a distinct allocator
        //:     for the object's footprint.
        //:
        //:   3 Dynamically allocate an object 'X' using the appropriate move
        //:     constructor to move from 'F', passing as a second argument
        //:     (a) nothing, (b) 'allocator_type()', (c) '&s1', or (d)
        //:     'allocator_type(&s2)'.
        //:
        //:   4 Record the allocator expected to be used by the new object and
        //:     how much memory it used before the move constructor.
        //:
        //:   5 Verify that space for 2 objects is used in the footprint
        //:     allocator
        //:
        //:   6 Verify that the moved-to object has the expected value 'V' by
        //:     comparing to 'Z'.
        //:
        //:   7 If the allocators of 'F' and 'X' are different verify that the
        //:     value of 'F' is still 'V'
        //:
        //:   8 If the allocators of 'F' and 'X' are the same, verify that no
        //:     extra memory was used by the move constructor.
        //:
        //:   9 Verify that no memory was used by the move constructor as
        //:     temporary memory, and no unused allocators have had any memory
        //:     used.
        //:
        //:  10 Delete both dynamically allocated objects and verify that all
        //:     temporary allocators have had all memory returned to them.
        //:
        //: 3 Test again, using the data of P-1, but this time just for the
        //:   supplied allocator configuration (P-2.2c), and create the object
        //:   as an automatic variable in the presence of injected exceptions
        //:   (using the 'BSLMA_TESTALLOCATOR_EXCEPTION_TEST_*' macros).  Do
        //:   this by creating one object with one test allocator ('s1') and
        //:   then using the move constructor with a separate test allocator
        //:   that is injecting exceptions ('s2').
        //:   (C-10)
        //
        // Testing:
        //   JsonArray(MovableRef<JsonArray> o);
        //   JsonArray(MovableRef<JsonArray> o, *a);
        // --------------------------------------------------------------------

        typedef bdljsn::JsonArray Obj;

        if (verbose) cout << "\n" "JSONARRAY MOVE CONSTRUCTOR" "\n"
                                  "==========================" "\n";

        if (verbose) cout <<
          "\nUse a table of distinct object values and expected memory usage."
                                                                       << endl;

        const int              NUM_DATA        = DEFAULT_NUM_DATA;
        const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

        if (verbose) cout <<
             "\nCreate objects with various allocator configurations." << endl;
        {
            bool anyObjectMemoryAllocatedFlag = false;  // We later check that
                                                        // this test allocates
                                                        // some object memory
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int          LINE     = DATA[ti].d_line;
                const char         MEM      = DATA[ti].d_mem;
                const bool         BOOL     = DATA[ti].d_bool;
                const char *const  NUMBER   = DATA[ti].d_number_p;
                const char *const  STRING   = DATA[ti].d_string_p;
                const Json         ARRAY[3] = {Json(BOOL),
                                               Json(JsonNumber(NUMBER)),
                                               Json(STRING)};

                bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

                const Obj Z (ARRAY, ARRAY+3, &scratch);
                const Obj ZZ(ARRAY, ARRAY+3, &scratch);

                if (veryVerbose) { T_ P_(Z) P(ZZ) }

                for (char cfg = 'a'; cfg <= 'd'; ++cfg) {
                    const char CONFIG = cfg;

                    bslma::TestAllocator da("default",   veryVeryVeryVerbose);
                    bslma::TestAllocator fa("footprint", veryVeryVeryVerbose);
                    bslma::TestAllocator s1("supplied",  veryVeryVeryVerbose);
                    bslma::TestAllocator s2("supplied2", veryVeryVeryVerbose);

                    bslma::DefaultAllocatorGuard dag(&da);

                    Obj *fromPtr = new (fa) Obj(ARRAY, ARRAY+3, &s1);
                    Obj& mF      = *fromPtr;  const Obj& F = mF;

                    bsls::Types::Int64 s1Alloc = s1.numBytesInUse();

                    Obj                  *objPtr = 0;
                    bslma::TestAllocator *objAllocatorPtr = 0;

                    switch (CONFIG) {
                      case 'a': {
                        // normal move constructor
                        objAllocatorPtr = &s1;
                        objPtr = new (fa) Obj(bslmf::MovableRefUtil::move(mF));
                      } break;
                      case 'b': {
                        // allocator move constructor, default allocator
                        objAllocatorPtr = &da;
                        objPtr = new (fa) Obj(bslmf::MovableRefUtil::move(mF),
                                              0);
                      } break;
                      case 'c': {
                        // allocator move constructor, same allocator
                        objAllocatorPtr = &s1;
                        objPtr = new (fa) Obj(bslmf::MovableRefUtil::move(mF),
                                              objAllocatorPtr);
                      } break;
                      case 'd': {
                        // allocator move constructor, same allocator
                        objAllocatorPtr = &s2;
                        objPtr = new (fa) Obj(bslmf::MovableRefUtil::move(mF),
                                              objAllocatorPtr);
                      } break;
                      default: {
                        BSLS_ASSERT_OPT(!"Bad allocator config.");
                      } break;
                    }
                    ASSERTV(LINE, CONFIG, 2*sizeof(Obj) == fa.numBytesInUse());

                    Obj& mX = *objPtr;  const Obj& X = mX;

                    if (veryVerbose) { T_ T_ P_(CONFIG) P(mF) P(X) }

                    bslma::TestAllocator& oa = *objAllocatorPtr;

                    // Verify the value of the object.

                    ASSERTV(LINE, CONFIG,  Z, X,  Z == X);

                    if (objAllocatorPtr != F.allocator()) {
                        // If the allocators are different, verify that the
                        // value of 'fX' has not changed.

                        ASSERTV(LINE, CONFIG, Z, F, Z == F);
                    }
                    else {
                        // If the allocators are the same, verify that no new
                        // bytes were allocated by moving.

                        ASSERTV(LINE, CONFIG, s1Alloc, s1.numBytesInUse(),
                                s1Alloc == s1.numBytesInUse());
                    }

                    // -------------------------------------------------------
                    // Verify any attribute allocators are installed properly.
                    // -------------------------------------------------------

                    ASSERTV(LINE, CONFIG, &oa == X[0].allocator());
                    ASSERTV(LINE, CONFIG, &oa == X[1].allocator());
                    ASSERTV(LINE, CONFIG, &oa == X[2].allocator());
                    ASSERTV(LINE, CONFIG, &oa == X.allocator());
                    ASSERTV(LINE, CONFIG, &scratch == Z.allocator());

                    // Verify no allocation from the non-object allocators.
                    if (objAllocatorPtr != &da) {
                        ASSERTV(LINE, CONFIG, da.numBlocksTotal(),
                                0 == da.numBlocksTotal());
                    }

                    if (objAllocatorPtr != &s2) {
                        ASSERTV(LINE, CONFIG, s2.numBlocksTotal(),
                                0 == s2.numBlocksTotal());
                    }

                    // Verify no temporary allocations from the object
                    // allocator.

                    ASSERTV(LINE, CONFIG, oa.numBlocksTotal(),
                            oa.numBlocksInUse(),
                            oa.numBlocksTotal() == oa.numBlocksInUse());

                    // Verify expected ('Y'/'N') object-memory allocations.

                    if ('?' != MEM) {
                        ASSERTV(LINE, CONFIG, MEM, oa.numBlocksInUse(),
                                ('N' == MEM) == (1 == oa.numBlocksInUse()));
                    }

                    // Record if some object memory was allocated.

                    anyObjectMemoryAllocatedFlag |= !!oa.numBlocksInUse();

                    // Reclaim dynamically allocated objects under test.

                    fa.deleteObject(fromPtr);
                    fa.deleteObject(objPtr);

                    // Verify all memory is released on object destruction.

                    ASSERTV(LINE, CONFIG, da.numBlocksInUse(),
                            0 == da.numBlocksInUse());
                    ASSERTV(LINE, CONFIG, fa.numBlocksInUse(),
                            0 == fa.numBlocksInUse());
                    ASSERTV(LINE, CONFIG, s1.numBlocksInUse(),
                            0 == s1.numBlocksInUse());
                    ASSERTV(LINE, CONFIG, s2.numBlocksInUse(),
                            0 == s2.numBlocksInUse());
                }  // end foreach configuration
            }  // end foreach row

            // Double check that some object memory was allocated.

            ASSERT(anyObjectMemoryAllocatedFlag);
        }

        if (verbose) cout << "\nTesting with injected exceptions." << endl;
        {
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int          LINE     = DATA[ti].d_line;
                const char         MEM      = DATA[ti].d_mem;
                const bool         BOOL     = DATA[ti].d_bool;
                const char *const  NUMBER   = DATA[ti].d_number_p;
                const char *const  STRING   = DATA[ti].d_string_p;
                const Json         ARRAY[3] = {Json(BOOL),
                                               Json(JsonNumber(NUMBER)),
                                               Json(STRING)};

                bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);
                bslma::TestAllocator da("default",   veryVeryVeryVerbose);
                bslma::TestAllocator s1("supplied",  veryVeryVeryVerbose);
                bslma::TestAllocator s2("supplied2", veryVeryVeryVerbose);

                const Obj Z (ARRAY, ARRAY+3, &scratch);

                if (veryVerbose) { T_ P_(MEM) P(Z) }

                bslma::DefaultAllocatorGuard dag(&da);

                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(s2) {
                    if (veryVeryVerbose) { T_ T_ Q(ExceptionTestBody) }

                    Obj from(ARRAY, ARRAY + 3, &s1);

                    Obj obj(bslmf::MovableRefUtil::move(from), &s2);
                    ASSERTV(LINE, Z, obj, Z == obj);

#ifdef BDE_BUILD_TARGET_EXC
                    if ('Y' == MEM) {
                        ASSERTV(LINE, 0 < EXCEPTION_COUNT);
                    }
#endif
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
            }
        }
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // JSONARRAY COPY CONSTRUCTOR
        //   Ensure that we can create a distinct object of the class from any
        //   other one, such that the two objects have the same value.
        //
        // Concerns:
        //: 1 The copy constructor (with or without a supplied allocator)
        //:   creates an object having the same value as that of the supplied
        //:   original object.
        //:
        //: 2 If an allocator is NOT supplied to the copy constructor, the
        //:   default allocator in effect at the time of construction becomes
        //:   the object allocator for the resulting object (i.e., the
        //:   allocator of the original object is never copied).
        //:
        //: 3 If an allocator IS supplied to the copy constructor, that
        //:   allocator becomes the object allocator for the resulting object.
        //:
        //: 4 Supplying a default-constructed allocator has the same effect as
        //:   not supplying an allocator.
        //:
        //: 5 Supplying an allocator to the copy constructor has no effect on
        //:   subsequent object values.
        //:
        //: 6 Any memory allocation is from the object allocator.
        //:
        //: 7 There is no temporary memory allocation from any allocator.
        //:
        //: 8 Every object releases any allocated memory at destruction.
        //:
        //: 9 The original object is passed as a 'const' reference.
        //:
        //:10 The value of the original object is unchanged.
        //:
        //:11 The allocator used by the original object is unchanged.
        //:
        //:12 QoI: Copying an object having the default-constructed value
        //:   allocates no memory.
        //:
        //:13 Any memory allocation is exception neutral.
        //
        // Plan:
        //: 1 Using the table-driven technique:
        //:
        //:   1 Specify a set of (unique) valid object values (one per row) in
        //:     terms of three different Json types, including values that
        //:     should require allocation where applicable.
        //:
        //:   2 Additionally, provide a (tri-valued) column, 'MEM', indicating
        //:     the expectation of memory allocation for all typical
        //:     implementations of individual attribute types: ('Y') "Yes",
        //:     ('N') "No", or ('?') "implementation-dependent".
        //:
        //: 2 For each row (representing a distinct object value, 'V') in the
        //:   table described in P-1: (C-1..12)
        //:
        //:   1 Use the value constructor and a "scratch" allocator to create
        //:     two 'const' 'Obj', 'Z' and 'ZZ', each having the value 'V'.
        //:
        //:   2 Execute an inner loop that creates an object by
        //:     copy-constructing from value 'Z' from P-2.1, but invokes the
        //:     copy constructor differently in each iteration: (a) without
        //:     passing an allocator, (b) passing a null pointer, and (c)
        //:     passing the address of a test allocator distinct from the
        //:     default.
        //:
        //:   3 For each of these iterations (P-2.2):  (C-1..12)
        //:
        //:     1 Create three 'bslma::TestAllocator' objects, and install one
        //:       as the current default allocator (note that a ubiquitous test
        //:       allocator is already installed as the global allocator).
        //:
        //:     2 Use the copy constructor to dynamically create an object 'X',
        //:       with it's object allocator configured appropriately (see
        //:       P-2.2), supplying it the 'const' object 'Z' (see P-2.1); use
        //:       a distinct test allocator for the object's footprint.  (C-9)
        //:
        //:     3 Use the equality-comparison operator to verify that:
        //:       (C-1, 5, 10)
        //:
        //:       1 The newly constructed object, 'X', has the same value as
        //:         that of 'Z'.  (C-1, 5)
        //:
        //:       2 'Z' still has the same value as that of 'ZZ'  (C-10)
        //:
        //:     4 Use the 'allocator' accessor of each contained object to
        //:       to ensure that its object allocator is properly installed;
        //:       also use the 'allocator' accessor of 'X' to verify that its
        //:       object allocator is properly installed, and use the
        //:       'allocator' accessor of 'Z' to verify that the allocator that
        //:       it uses is unchanged.
        //:
        //:     5 Use the appropriate test allocators to verify that:
        //:       (C-2..4, 7..8, 12)
        //:
        //:       1 An object that IS expected to allocate memory does so from
        //:         the object allocator only (irrespective of the specific
        //:         number of allocations or the total amount of memory
        //:         allocated)..  (C-2, 4)
        //:
        //:       2 An object that is expected NOT to allocate memory doesn't.
        //:         (C-12)
        //:
        //:       3 If an allocator was supplied at construction (P-2.1c), the
        //:         current default allocator doesn't allocate any memory.
        //:         (C-3)
        //:
        //:       4 No temporary memory is allocated from the object allocator.
        //:         (C-7)
        //:
        //:       5 All object memory is released when the object is destroyed.
        //:         (C-8)
        //:
        //: 3 Test again, using the data of P-1, but this time just for the
        //:   supplied allocator configuration (P-2.2c), and create the object
        //:   as an automatic variable in the presence of injected exceptions
        //:   (using the 'BSLMA_TESTALLOCATOR_EXCEPTION_TEST_*' macros).
        //:   (C-13)
        //
        // Testing:
        //   JsonArray(const JsonArray& o, *a);
        // --------------------------------------------------------------------

        typedef bdljsn::JsonArray Obj;

        if (verbose) cout << endl
                          << "JSONARRAY COPY CONSTRUCTOR" << endl
                          << "==========================" << endl;

        if (verbose) cout <<
           "\nUse a table of distinct object values and expected memory usage."
                                                                       << endl;

        const int              NUM_DATA        = DEFAULT_NUM_DATA;
        const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

        if (verbose) cout <<
             "\nCreate objects with various allocator configurations." << endl;
        {
            bool anyObjectMemoryAllocatedFlag = false;  // We later check that
                                                        // this test allocates
                                                        // some object memory
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int          LINE     = DATA[ti].d_line;
                const char         MEM      = DATA[ti].d_mem;
                const bool         BOOL     = DATA[ti].d_bool;
                const char *const  NUMBER   = DATA[ti].d_number_p;
                const char *const  STRING   = DATA[ti].d_string_p;
                const Json         ARRAY[3] = {Json(BOOL),
                                               Json(JsonNumber(NUMBER)),
                                               Json(STRING)};

                bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

                const Obj Z (ARRAY, ARRAY+3, &scratch);
                const Obj ZZ(ARRAY, ARRAY+3, &scratch);

                if (veryVerbose) { T_ P_(Z) P(ZZ) }

                for (char cfg = 'a'; cfg <= 'c'; ++cfg) {

                    const char CONFIG = cfg;

                    bslma::TestAllocator da("default",   veryVeryVeryVerbose);
                    bslma::TestAllocator fa("footprint", veryVeryVeryVerbose);
                    bslma::TestAllocator sa("supplied",  veryVeryVeryVerbose);

                    bslma::DefaultAllocatorGuard dag(&da);

                    Obj                  *objPtr = 0;
                    bslma::TestAllocator *objAllocatorPtr = 0;

                    switch (CONFIG) {
                      case 'a': {
                        objAllocatorPtr = &da;
                        objPtr = new (fa) Obj(Z);
                      } break;
                      case 'b': {
                        objAllocatorPtr = &da;
                        objPtr = new (fa) Obj(Z, 0);
                      } break;
                      case 'c': {
                        objAllocatorPtr = &sa;
                        objPtr = new (fa) Obj(Z, objAllocatorPtr);
                      } break;
                      default: {
                        BSLS_ASSERT_OPT(!"Bad allocator config");
                      } break;
                    }
                    ASSERTV(LINE, CONFIG, sizeof(Obj) == fa.numBytesInUse());

                    Obj& mX = *objPtr;  const Obj& X = mX;

                    if (veryVerbose) { T_ T_ P_(CONFIG) P(X) }

                    bslma::TestAllocator&  oa = *objAllocatorPtr;
                    bslma::TestAllocator& noa = (&da == &oa) ? sa : da;

                    // Verify the value of the object.

                    ASSERTV(LINE, CONFIG, Z, X,  Z == X);

                    // Verify that the value of 'Z' has not changed.

                    ASSERTV(LINE, CONFIG, Z, X, ZZ == X);

                    // Verify contained object allocators are installed
                    // properly.

                    ASSERTV(LINE, CONFIG, &oa == X[0].allocator());
                    ASSERTV(LINE, CONFIG, &oa == X[1].allocator());
                    ASSERTV(LINE, CONFIG, &oa == X[2].allocator());

                    // Also invoke the object's 'allocator' accessor, as well
                    // as that of 'Z'.

                    ASSERTV(LINE, CONFIG, &oa, X.allocator(),
                            &oa == X.allocator());

                    ASSERTV(LINE, CONFIG, &scratch, Z.allocator(),
                            &scratch == Z.allocator());

                    // Verify no allocations from the non-object allocator.

                    ASSERTV(LINE, CONFIG, noa.numBlocksTotal(),
                            0 == noa.numBlocksTotal());

                    // Verify no temporary memory is allocated from the object
                    // allocator.

                    ASSERTV(LINE, CONFIG,
                            oa.numBlocksTotal(),
                            oa.numBlocksInUse(),
                            oa.numBlocksTotal() == oa.numBlocksInUse());

                    // Verify expected ('Y'/'N') object-memory allocations in
                    // addition to the one vector allocation.

                    if ('?' != MEM) {
                        ASSERTV(LINE, CONFIG, MEM, oa.numBlocksInUse(),
                                ('N' == MEM) == (1 == oa.numBlocksInUse()));
                    }

                    // Record if some object memory was allocated.

                    anyObjectMemoryAllocatedFlag |= !!oa.numBlocksInUse();

                    // Reclaim dynamically allocated object under test.

                    fa.deleteObject(objPtr);

                    // Verify all memory is released on object destruction.

                    ASSERTV(LINE, CONFIG, da.numBlocksInUse(),
                            0 == da.numBlocksInUse());
                    ASSERTV(LINE, CONFIG, fa.numBlocksInUse(),
                            0 == fa.numBlocksInUse());
                    ASSERTV(LINE, CONFIG, sa.numBlocksInUse(),
                            0 == sa.numBlocksInUse());
                }  // end foreach configuration
            }  // end foreach row

            // Double check that some object memory was allocated.

            ASSERT(anyObjectMemoryAllocatedFlag);

        }

        if (verbose) cout << "\nTesting with injected exceptions." << endl;
        {
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int          LINE     = DATA[ti].d_line;
                const char         MEM      = DATA[ti].d_mem;
                const bool         BOOL     = DATA[ti].d_bool;
                const char *const  NUMBER   = DATA[ti].d_number_p;
                const char *const  STRING   = DATA[ti].d_string_p;
                const Json         ARRAY[3] = {Json(BOOL),
                                               Json(JsonNumber(NUMBER)),
                                               Json(STRING)};

                bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);
                bslma::TestAllocator da("default",      veryVeryVeryVerbose);
                bslma::TestAllocator sa("supplied",     veryVeryVeryVerbose);

                const Obj Z (ARRAY, ARRAY+3, &scratch);
                const Obj ZZ(ARRAY, ARRAY+3, &scratch);

                bslma::DefaultAllocatorGuard dag(&da);

                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(sa) {
                    if (veryVeryVerbose) { T_ T_ Q(ExcpetionTestBody) }

                    Obj obj(Z, &sa);
                    ASSERTV(LINE, Z, obj, Z == obj);

#ifdef BDE_BUILD_TARGET_EXC
                    if ('Y' == MEM) {
                        ASSERTV(LINE, 0 < EXCEPTION_COUNT);
                    }
#endif
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                ASSERTV(LINE, ZZ, Z, ZZ == Z);
                ASSERTV(LINE, &scratch, Z.allocator(),
                        &scratch == Z.allocator());
                ASSERTV(LINE, da.numBlocksInUse(), 0 == da.numBlocksInUse());
                ASSERTV(LINE, sa.numBlocksInUse(), 0 == sa.numBlocksInUse());
            }
        }
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // JSONARRAY EQUALITY-COMPARISON OPERATORS
        //   Ensure that '==' and '!=' are the operational definition of value.
        //
        // Concerns:
        //: 1 Two objects, 'X' and 'Y', compare equal if and only if each of
        //:   their corresponding salient attributes respectively compares
        //:   equal.
        //:
        //: 2 All salient attributes participate in the comparison.
        //:
        //: 3 No non-salient attributes (i.e., 'allocator') participate.
        //:
        //: 4 'true  == (X == X)'  (i.e., identity)
        //:
        //: 5 'false == (X != X)'  (i.e., identity)
        //:
        //: 6 'X == Y' if and only if 'Y == X'  (i.e., commutativity)
        //:
        //: 7 'X != Y' if and only if 'Y != X'  (i.e., commutativity)
        //:
        //: 8 'X != Y' if and only if '!(X == Y)'
        //:
        //: 9 Comparison is symmetric with respect to user-defined conversion
        //:   (i.e., both comparison operators are free functions).
        //:
        //:10 Non-modifiable objects can be compared (i.e., 'const' objects and
        //:   'const' references).
        //:
        //:11 No memory allocation occurs as a result of comparison (e.g., the
        //:   arguments are not passed by value).
        //:
        //:12 The equality operator's signature and return type are standard.
        //:
        //:13 The inequality operator's signature and return type are standard.
        //
        // Plan:
        //: 1 Use the respective addresses of 'operator==' and 'operator!=' to
        //:   initialize function pointers having the appropriate signatures
        //:   and return types for the two homogeneous, free equality-
        //:   comparison operators defined in this component.
        //:   (C-9..10, 12..13)
        //:
        //: 2 Create a 'bslma::TestAllocator' object and install it as the
        //:   default allocator (note that a ubiquitous test allocator is
        //:   already installed as the global allocator).
        //:
        //: 3 Using the table-driven technique, specify a set of distinct
        //:   object values (one per row) in terms of their individual contents
        //:   such that for each of 3 distinct baseline contained objects,
        //:   there exists a pair of rows that differ in only the object at
        //:   that index.
        //:
        //: 4 For each of two configurations, 'a' and 'b': (C-1..3, 6..8)
        //:
        //:   1 Create two (object) allocators, 'oax' and 'oay'.
        //:
        //:   2 For each row 'R1' in the table of P-3: (C1..8)
        //:
        //:     1 Create object 'x', using 'oax' allocator, having the value
        //:       'R1', and use a 'const' reference 'X' to 'x' to verify the
        //:       reflexive (anti-reflexive) property of equality (inequality)
        //:       in the presence of aliasing.  (C-4..5)
        //:
        //:     2 For each row 'R2' in the table of P-3: (C-1..8)
        //:
        //:       1 Record, in 'EXP', whether or not distinct objects created
        //:         from 'R1' and 'R2', respectively, are expected to have the
        //:         same value.
        //:
        //:       2 Create an object 'y', using 'oax' in configuration 'a' and
        //:         'oay' in configuration 'b', having the value 'R2'.  Create
        //:         a 'const' reference to 'y' named 'Y'.
        //:
        //:       3 Verify the commutative property and expected return value
        //:         for both '==' and '!=', while monitoring both 'oax' and
        //:         'oay' to ensure that no object memory is ever allocated by
        //:         either operator.  (C-1..3, 6..8)
        //:
        //: 5 Use the test allocator from P-2 to verify that no memory is ever
        //:   allocated from the default allocator.  (C-11)
        //
        // Testing:
        //   bool operator==(const bdljsn::JsonArray& lhs, rhs);
        //   bool operator!=(const bdljsn::JsonArray& lhs, rhs);
        // --------------------------------------------------------------------

        typedef bdljsn::JsonArray Obj;

        if (verbose) cout << "\n" "JSONARRAY EQUALITY-COMPARISON OPERATORS"
                             "\n" "======================================="
                             "\n";

        if (verbose) cout <<
                 "\nAssign the address of each operator to a variable" << endl;
        {
            typedef bool (*operatorPtr)(const Obj&, const Obj&);

            operatorPtr operatorEq = operator==;  (void) operatorEq;
            operatorPtr operatorNe = operator!=;  (void) operatorNe;
        }

        if (verbose) cout <<
            "\nCreate a test allocator and install it as the default." << endl;

        bslma::TestAllocator         da("default", veryVeryVeryVerbose);
        bslma::DefaultAllocatorGuard dag(&da);

        if (verbose) cout <<
            "\nDefine appropriate individual attribute values, 'Ai' and 'Bi'."
                                                                       << endl;

        const int EMPTY = 123456;

        static const struct {
            const int     d_line;                // source line number
            const int     d_value1;
            const int     d_value2;
            const int     d_value3;
            const int     d_value4;
        } DATA[] = {
        { L_,            1,     2,     3, EMPTY },         // baseline
        { L_,            4,     2,     3, EMPTY },
        { L_,            1,     4,     3, EMPTY },
        { L_,            1,     2,     4, EMPTY },
        { L_,            1,     2,     3,     4 },
        { L_,            1,     2, EMPTY, EMPTY },
        { L_,            1, EMPTY, EMPTY, EMPTY },

        };
        enum { NUM_DATA = sizeof DATA / sizeof *DATA };

        if (verbose) cout << "\nCompare every value with every value." << endl;

        for (char cfg = 'a'; cfg <= 'b'; ++cfg) {

            const char CONFIG = cfg;

            // Create two distinct test allocators, 'oax' and 'oay'.

            bslma::TestAllocator oax("objectx", veryVeryVeryVerbose);
            bslma::TestAllocator oay("objecty", veryVeryVeryVerbose);

            // Map allocators above to objects 'X' and 'Y' below.

            bslma::TestAllocator& xa = oax;
            bslma::TestAllocator& ya = 'a' == CONFIG ? oax : oay;

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int LINE1  = DATA[ti].d_line;
                JsonArray x(&xa);
                if (EMPTY != DATA[ti].d_value1) {
                    x.pushBack(Json(DATA[ti].d_value1));
                }
                if (EMPTY != DATA[ti].d_value2) {
                    x.pushBack(Json(DATA[ti].d_value2));
                }
                if (EMPTY != DATA[ti].d_value3) {
                    x.pushBack(Json(DATA[ti].d_value3));
                }
                if (EMPTY != DATA[ti].d_value4) {
                    x.pushBack(Json(DATA[ti].d_value4));
                }

                const JsonArray&    X = x;

                if (veryVerbose) { T_ T_ P_(LINE1) P_(X) }

                // Ensure an object compares correctly with itself (alias
                // test).
                {
                    ASSERTV(LINE1, X, X == X);
                    ASSERTV(LINE1, X, !(X != X));
                }

                for (int tj = 0; tj < NUM_DATA; ++tj) {
                    const int LINE2 = DATA[tj].d_line;
                    JsonArray y(&ya);

                    if (EMPTY != DATA[tj].d_value1) {
                        y.pushBack(Json(DATA[tj].d_value1));
                    }
                    if (EMPTY != DATA[tj].d_value2) {
                        y.pushBack(Json(DATA[tj].d_value2));
                    }
                    if (EMPTY != DATA[tj].d_value3) {
                        y.pushBack(Json(DATA[tj].d_value3));
                    }
                    if (EMPTY != DATA[tj].d_value4) {
                        y.pushBack(Json(DATA[tj].d_value4));
                    }
                    const JsonArray& Y = y;

                    if (veryVerbose) { T_ T_ P_(LINE2) P_(Y) }

                    const bool EXP = (ti == tj);

                    if (veryVerbose) {
                        T_ T_ T_ P_(EXP) P_(CONFIG) P_(X) P_(Y) }

                    // Verify value, commutativity, and no memory allocation

                    bslma::TestAllocatorMonitor oaxm(&oax), oaym(&oay);

                    ASSERTV(LINE1, LINE2, CONFIG, X, Y, EXP == (X == Y));
                    ASSERTV(LINE1, LINE2, CONFIG, Y, X, EXP == (Y == X));

                    ASSERTV(LINE1, LINE2, CONFIG, X, Y, !EXP == (X != Y));
                    ASSERTV(LINE1, LINE2, CONFIG, Y, X, !EXP == (Y != X));

                    ASSERTV(LINE1, LINE2, CONFIG, oaxm.isTotalSame());
                    ASSERTV(LINE1, LINE2, CONFIG, oaym.isTotalSame());

                    // Double check that some object memory was allocated.
                    ASSERTV(LINE1, LINE2, CONFIG, 1 <= xa.numBlocksInUse());
                    ASSERTV(LINE1, LINE2, CONFIG, 1 <= ya.numBlocksInUse());
                }
            }
        }

        ASSERTV(da.numBlocksTotal(), 0 == da.numBlocksTotal());
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
        //: 3 The output using 's << obj' is the same as 'obj.print(s, 0, -1)'
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
        //: 2 Using the table-driven technique:  (C1..3, 5..6, 8)
        //:
        //:   1 Define a 'const' 'JsonArray' object containing a variety of
        //:     'Json' objects.
        //:
        //:   2 Define fourteen carefully selected combinations of values for
        //:     the two formatting parameters, along with the expected output.
        //:
        //:   3 For each row in the table defined in P-2.2: (C-1..3, 5..6, 8)
        //:
        //:     1 Supply the 'JsonArray' from P-2.1 and each pair of formatting
        //:       parameters to 'print', omitting the 'level' or
        //:       'spacesPerLevel' parameter if the value of that argument is
        //:       '-8'.  If the parameters are, arbitrarily, (-9, -9), then
        //:       invoke the 'operator<<' instead.
        //
        // Testing
        //   operator<<(ostream&, const JsonArray&);
        //   ostream& JsonArray::print(os& s, int l = 0, int sPL = 4) const;
        // --------------------------------------------------------------------

        typedef bdljsn::JsonArray Obj;

        if (verbose) cout << endl
                          << "PRINT AND OUTPUT OPERATOR" << endl
                          << "=========================" << endl;

        if (verbose) cout << "\nAssign the addresses of 'print' and "
                             "the output 'operator<<' to variables." << endl;
        {
            using bsl::ostream;

            typedef ostream& (Obj::*funcPtr)(ostream&, int, int) const;
            typedef ostream& (*operatorPtr)(ostream&, const Obj&);

            funcPtr     printMember = &Obj::print;  (void) printMember;
            operatorPtr operatorOp  = operator<<;   (void) operatorOp;
        }

        if (verbose) cout <<
            "\nCreate a table of distinct value/format combinations." << endl;

        JsonArray inner;
        inner.pushBack(Json(2));
        inner.pushBack(Json("inner"));
        JsonArray mX;
        mX.pushBack(Json());
        mX.pushBack(Json(1));
        mX.pushBack(Json(inner));
        mX.pushBack(Json("outer"));
        const JsonArray& X = mX;

        static const struct {
            int         d_line;
            int         d_level;
            int         d_spacesPerLevel;
            const char *d_expected_p;
        } DATA[] = {
#define NL "\n"
#define SP " "
        { L_,  0,  0, "["                                NL
                      "null,"                            NL
                      "1,"                               NL
                      "["                                NL
                      "2,"                               NL
                      "\"inner\""                        NL
                      "],"                               NL
                      "\"outer\""                        NL
                      "]"                                },

        { L_,  0,  1,  "["                               NL
                      " null,"                           NL
                      " 1,"                              NL
                      " ["                               NL
                      "  2,"                             NL
                      "  \"inner\""                      NL
                      " ],"                              NL
                      " \"outer\""                       NL
                      "]"                                },

        { L_,  0, -1, "["
                      "null,"                            SP
                      "1,"                               SP
                      "["
                      "2,"                               SP
                      "\"inner\""
                      "],"                               SP
                      "\"outer\""
                      "]"                                },

        { L_,  0, -8, "["                                NL
                      "    null,"                        NL
                      "    1,"                           NL
                      "    ["                            NL
                      "        2,"                       NL
                      "        \"inner\""                NL
                      "    ],"                           NL
                      "    \"outer\""                    NL
                      "]"                                },

        { L_,  3,  0, "["                                NL
                      "null,"                            NL
                      "1,"                               NL
                      "["                                NL
                      "2,"                               NL
                      "\"inner\""                        NL
                      "],"                               NL
                      "\"outer\""                        NL
                      "]"                                },

        { L_,  3,  2, "      ["                          NL
                      "        null,"                    NL
                      "        1,"                       NL
                      "        ["                        NL
                      "          2,"                     NL
                      "          \"inner\""              NL
                      "        ],"                       NL
                      "        \"outer\""                NL
                      "      ]"                          },

        { L_,  3, -2, "      ["
                      "null,"                            SP
                      "1,"                               SP
                      "["
                      "2,"                               SP
                      "\"inner\""
                      "],"                               SP
                      "\"outer\""
                      "]"                                },

        { L_,  3, -8, "            ["                    NL
                      "                null,"            NL
                      "                1,"               NL
                      "                ["                NL
                      "                    2,"           NL
                      "                    \"inner\""    NL
                      "                ],"               NL
                      "                \"outer\""        NL
                      "            ]"                    },

        { L_, -3,  0, "["                                NL
                      "null,"                            NL
                      "1,"                               NL
                      "["                                NL
                      "2,"                               NL
                      "\"inner\""                        NL
                      "],"                               NL
                      "\"outer\""                        NL
                      "]"                                },

        { L_, -3,  2, "["                                NL
                      "        null,"                    NL
                      "        1,"                       NL
                      "        ["                        NL
                      "          2,"                     NL
                      "          \"inner\""              NL
                      "        ],"                       NL
                      "        \"outer\""                NL
                      "      ]"                          },

        { L_, -3, -2, "["
                      "null,"                            SP
                      "1,"                               SP
                      "["
                      "2,"                               SP
                      "\"inner\""
                      "],"                               SP
                      "\"outer\""
                      "]"                                },

        { L_, -3, -8, "["                                NL
                      "                null,"            NL
                      "                1,"               NL
                      "                ["                NL
                      "                    2,"           NL
                      "                    \"inner\""    NL
                      "                ],"               NL
                      "                \"outer\""        NL
                      "            ]"                    },

        { L_, -8, -8, "["                                NL
                      "    null,"                        NL
                      "    1,"                           NL
                      "    ["                            NL
                      "        2,"                       NL
                      "        \"inner\""                NL
                      "    ],"                           NL
                      "    \"outer\""                    NL
                      "]"                                },

        { L_, -9, -9, "["
                      "null,"                            SP
                      "1,"                               SP
                      "["
                      "2,"                               SP
                      "\"inner\""
                      "],"                               SP
                      "\"outer\""
                      "]"                                },
#undef SP
#undef NL
        };
        enum { NUM_DATA = sizeof DATA / sizeof *DATA };

        if (verbose) cout << "\nTsting with various print specifications."
                          << endl;
        {
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int         LINE = DATA[ti].d_line;
                const int         L    = DATA[ti].d_level;
                const int         SPL  = DATA[ti].d_spacesPerLevel;
                const char *const EXP  = DATA[ti].d_expected_p;

                if (veryVerbose) { T_ P_(L) P_(SPL) }

                if (veryVeryVerbose) {T_ T_ Q(EXPECTED) cout << EXP; }

                bsl::ostringstream os;

                if (-9 == L) {
                    ASSERTV(LINE, -9 == SPL);

                    ASSERTV(LINE, &os == &(os << X));

                    if (veryVeryVerbose) {T_ T_ Q(operator<<) }
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

                if (veryVeryVerbose) { P(os.str()) }

                ASSERTV(LINE, EXP, os.str(), EXP == os.str());
            }
        }
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // JSONARRAY BASIC ACCESSORS
        //   Ensure each basic accessor properly provides access to contained
        //   objects.
        //
        // Concerns:
        //: 1 Each accessor returns the value of the corresponding contained
        //:   object.
        //:
        //: 2 Each accessor method is declared 'const'.
        //:
        //: 3 No accessor allocates memory.
        //:
        //: 4 Accessors return a 'const' reference (because 'Json' objects can
        //:   allocate).
        //
        // Plan:
        //: 1 Create two 'bslma::TestAllocator' objects, and install one as
        //:   the current default allocator (note that a ubiquitous test
        //:   allocator is already installed as the global allocator).
        //:
        //: 2 Use the default constructor with the allocator from P-1 that was
        //:   not installed as default, to create an object.
        //:
        //: 3 Verify that each basic accessor, invoked on a 'const' reference
        //:   to the object created in P-2, returns the expected value.  (C-2)
        //:
        //: 4 For each salient attribute (constributing to value): (C-1, 3..4)
        //:   1 Use the primary manipulator (pushBack) to put three
        //:     non-default, discriminable objects into the container.
        //:
        //:   2 Use all accessors to verify expected values.  (C-1)
        //:
        //:   3 Monitor the memory allocated from both the default and object
        //:     allocators before and after calling the accessor; verify that
        //:     there is no change in total memory allocation.  (C-3..4)
        //
        // Testing:
        //   bslma::Allocator *JsonArray::allocator() const;
        //   const Json& JsonArray::front() const;
        //   const Json& JsonArray::begin() const;
        //   const Json& JsonArray::cbegin() const;
        //   const Json& JsonArray::back() const;
        //   const Json& JsonArray::end() const;
        //   const Json& JsonArray::cend() const;
        //   bool JsonArray::empty() const;
        //   size_t JsonArray::size() const;
        //   const Json& JsonArray::operator[](size_t i) const;
        // --------------------------------------------------------------------

        typedef bdljsn::JsonArray Obj;

        const bool             D1 = false;
        const int              D2 = 0;
        const bsl::string_view D3;

        const bool             A1 = true;
        const int              A2 = 1;
        const bsl::string_view A3 = LONG_STRING;

        if (verbose) cout << endl
                          << "JSONARRAY BASIC ACCESSORS" << endl
                          << "=========================" << endl;

        bslma::TestAllocator da("default", veryVeryVeryVerbose);
        bslma::TestAllocator oa("object",  veryVeryVeryVerbose);

        bslma::DefaultAllocatorGuard dag(&da);

        if (verbose) cout << endl
            << "Create an object, passing in the other allocator." << endl;

        Obj mX(&oa);  const Obj& X = mX;
        mX.pushBack(Json(D1));
        mX.pushBack(Json(D2));
        mX.pushBack(Json(D3));

        if (verbose) cout << endl
            << "Verify all basic accessors report expected values." << endl;
        {
            ASSERT(&oa == mX.allocator());

            if (verbose) cout << endl << "JsonArray::front" << endl;
            ASSERTV(X.front().type(), X.front().isBoolean());
            ASSERTV(X.front(), D1 == X.front().theBoolean());

            if (verbose) cout << endl << "JsonArray::begin" << endl;
            ASSERTV(X.begin()->type(), X.begin()->isBoolean());
            ASSERTV(X.begin()->theBoolean(), D1 == X.begin()->theBoolean());

            if (verbose) cout << endl << "JsonArray::cbegin" << endl;
            ASSERTV(X.cbegin()->type(), X.cbegin()->isBoolean());
            ASSERTV(X.cbegin()->theBoolean(), D1 == X.cbegin()->theBoolean());

            if (verbose) cout << endl << "JsonArray::back" << endl;
            ASSERTV(X.back().type(), X.back().isString());
            ASSERTV(X.back(), D3 == X.back().theString());

            if (verbose) cout << endl << "JsonArray::end" << endl;
            ASSERTV((X.end()-1)->type(), (X.end()-1)->isString());
            ASSERTV((X.end()-1)->theString(), D3 == (X.end()-1)->theString());

            if (verbose) cout << endl << "JsonArray::cend" << endl;
            ASSERTV((X.cend()-1)->type(), (X.cend()-1)->isString());
            ASSERTV((X.cend()-1)->theString(),
                    D3 == (X.cend()-1)->theString());

            if (verbose) cout << endl << "JsonArray::empty" << endl;
            ASSERTV(X.empty(), false == X.empty());

            if (verbose) cout << endl << "JsonArray::size" << endl;
            ASSERTV(X.size(), 3 == X.size());

            if (verbose) cout << endl << "JsonArray::operator[]" << endl;
            ASSERTV(X[0].type(), X[0].isBoolean());
            ASSERTV(X[0].theBoolean(), D1 == X[0].theBoolean());

            ASSERTV(X[1].type(), X[1].isNumber());
            {
                int value;
                int status = X[1].theNumber().asInt(&value);
                ASSERTV(status, 0 == status);
                ASSERTV(X[1].theNumber(), D2 == value);
            }

            ASSERTV(X[2].type(), X[2].isString());
            ASSERTV(X[2].theString(), D3 == X[2].theString());

            if (verbose) cout << endl
                << "Change data and verify expected values." << endl;

            mX[0] = A1;
            mX[1] = A2;
            mX[2] = A3;

            if (verbose) cout << endl << "JsonArray::front" << endl;
            ASSERTV(X.front().type(), X.front().isBoolean());
            ASSERTV(X.front(), A1 == X.front().theBoolean());

            if (verbose) cout << endl << "JsonArray::begin" << endl;
            ASSERTV(X.begin()->type(), X.begin()->isBoolean());
            ASSERTV(X.begin()->theBoolean(), A1 == X.begin()->theBoolean());

            if (verbose) cout << endl << "JsonArray::cbegin" << endl;
            ASSERTV(X.cbegin()->type(), X.cbegin()->isBoolean());
            ASSERTV(X.cbegin()->theBoolean(), A1 == X.cbegin()->theBoolean());

            if (verbose) cout << endl << "JsonArray::back" << endl;
            ASSERTV(X.back().type(), X.back().isString());
            ASSERTV(X.back(), A3 == X.back().theString());

            if (verbose) cout << endl << "JsonArray::end" << endl;
            ASSERTV((X.end()-1)->type(), (X.end()-1)->isString());
            ASSERTV((X.end()-1)->theString(), A3 == (X.end()-1)->theString());

            if (verbose) cout << endl << "JsonArray::cend" << endl;
            ASSERTV((X.cend()-1)->type(), (X.cend()-1)->isString());
            ASSERTV((X.cend()-1)->theString(),
                    A3 == (X.cend()-1)->theString());

            if (verbose) cout << endl << "JsonArray::empty" << endl;
            ASSERTV(X.empty(), false == X.empty());

            if (verbose) cout << endl << "JsonArray::size" << endl;
            ASSERTV(X.size(), 3 == X.size());

            if (verbose) cout << endl << "JsonArray::operator[]" << endl;
            ASSERTV(X[0].type(), X[0].isBoolean());
            ASSERTV(X[0].theBoolean(), A1 == X[0].theBoolean());

            ASSERTV(X[1].type(), X[1].isNumber());
            {
                int value;
                int status = X[1].theNumber().asInt(&value);
                ASSERTV(status, 0 == status);
                ASSERTV(X[1].theNumber(), A2 == value);
            }

            ASSERTV(X[2].type(), X[2].isString());
            ASSERTV(X[2].theString(), A3 == X[2].theString());
        }

        // Double check that some object memory was allocated.

        ASSERTV(oa.numBlocksTotal(), 1 <= oa.numBlocksTotal());

        ASSERTV(da.numBlocksTotal(), 0 == da.numBlocksTotal());
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // JSONARRAY VALUE CTOR
        //   Ensure that we can put an object into any initial state relevant
        //   for thorough testing.
        //
        // Concerns:
        //: 1 The value constructor (with or without a supplied allocator) can
        //:   create an object having any value that does not violate the
        //:   constructor's documented preconditions.
        //:
        //: 2 Any string arguments can be of type 'char *' or 'string'.
        //:
        //: 3 Any argument can be 'const'.
        //:
        //: 4 If an allocator is NOT supplied to the value constructor, the
        //:   default allocator in effect at the time of construction becomes
        //:   the object allocator for the resulting object.
        //:
        //: 5 If an allocator IS supplied to the value constructor, that
        //:   allocator becomes the object allocator for the resulting object.
        //:
        //: 6 Supplying a default-constructed allocator has the same effect as
        //:   not supplying al allocator.
        //:
        //: 7 Supplying an allocator to the value constructor has no effect
        //:   on subsequent object values.
        //:
        //: 8 Any memory allocation is from the object allocator.
        //:
        //: 9 There is no temporary memory allocation from any allocator.
        //:
        //:10 Every object releases any allocated memory at destruction.
        //:
        //:11 QoI: Creating an object having the default-constructed value
        //:   allocates no memory.
        //:
        //:12 Any memory allocation is exception neutral.
        //:
        //:13 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Using the table-driven technique:
        //:
        //:   1 Specify a set of (unique) valid object values (one per row) in
        //:     terms of their individual attributes, including (a) first, the
        //:     default value, (b) boundary values corresponding to every range
        //:     of values that each individual attribute can independently
        //:     attain, and (c) values that should require allocation from each
        //:     individual attribute that can independently allocate memory.
        //:   2 Additionally, provide a (tri-valued) column, 'MEM', indicating
        //:     the expectation of memory allocation for all typical
        //:     implementations of individual attribute types: ('Y') "Yes",
        //:     'N') "No", or ('?') "implementation-dependent".
        //:
        //: 2 For each row (representing a distinct object value, 'V') in the
        //:   table described in P-1: (C-1, 3..11)
        //:
        //:   1 Execute an inner loop that creates an object having value 'V',
        //:     but invokes the constructor differently on each iteration: (a)
        //:     without passing an allocator, (b) passing a
        //:     default-constructed allocator explicitly (c) passing the
        //:     address of a test allocator distinct from the default
        //:     allocator, and (d) passing in an allocator constructed from
        //:     the address of a test allocator distinct from the default.
        //:
        //:   2 For each of the iterations in P-2.1:  (C-1, 4..11)
        //:
        //:     1 Create three 'bslma::TestAllocator' objects, and install one
        //:       as the current default allocator (note that a ubiquitous test
        //:       allocator is already installed as the global allocator).
        //:
        //:     2 Use the value constructor to dynamically create an object
        //:       having the value 'V', with its object allocator configured
        //:       appropriately (see P-2.1), supplying all the arguments as
        //:       'const' and representing any string arguments as 'char *';
        //:       use a distinct test allocator for the object's footprint.
        //:
        //:     3 Use the (as yet unproven) salient attribute accessors to
        //:       verify that all of the attributes of each object have their
        //:       expected values.  (C-1, 7)
        //:
        //:     4 Use the 'allocator' accessor of each underlying attribute
        //:       capable of allocating memory to ensure that its object
        //:       allocator is properly installed; also invoke the (as yet
        //:       unproven) 'allocator' accessor of the object under test.
        //:       (C-8)
        //:
        //:     5 Use the appropriate test allocators to verify that:  (C-4..6,
        //:       9..11)
        //:
        //:       1 An object that IS expected to allocate memory does so
        //:         from the object allocator only (irrespective of the
        //:         specific number of allocations or the total amount of
        //:         memory allocated).  (C-4, 6)
        //:
        //:       2 An object that is expected NOT to allocate memory doesn't.
        //:         (C-11)
        //:
        //:       3 If an allocator was supplied at construction (P-2.1c), the
        //:         default allocator doesn't allocate any memory.  (C-5)
        //:
        //:       4 No temporary memory is allocated from the object allocator.
        //:         (C-9)
        //:
        //:       5 All object memory is released when the object is destroyed.
        //:         (C-10)
        //:
        //: 3 Repeat the steps in P-2 for the supplied allocator configuration
        //:   (P-2.1c) on the data of P-1, but this time create the object as
        //:   an automatic variable in the presence of injected exceptions
        //:   (using the 'BSLMA_TESTALLOCATOR_EXCEPTION_TEST_*' macros);
        //:   represent any string arguments in terms of 'string' using a
        //:   "scratch" allocator.  (C-2, 12)
        //:
        //: 4 Verify that, in appropriate build modes, defensive checks are
        //:   triggered for invalid attribute values, but not triggered for
        //:   adjacent valid ones (using the 'BSLS_ASSERTTEST_*' macros).
        //:   (C-13)
        //
        // Testing:
        //   JsonArray(INPUT_ITERATOR first, INPUT_ITERATOR last, *a);
        //   CONCERN: All creator/manipulator ptr./ref. parameters are 'const'.
        //   CONCERN: String arguments can be either 'char *' or 'string'.
        // --------------------------------------------------------------------

        typedef bdljsn::JsonArray Obj;

        if (verbose) cout << "\n" "JSONARRAY VALUE CTOR" "\n"
                                  "====================" "\n";

        if (verbose) cout <<
           "\nUse a table of distinct object values and expected memory usage."
                                                                       << endl;

        const int              NUM_DATA        = DEFAULT_NUM_DATA;
        const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

        if (verbose) cout <<
            "\nUse a table of distict object values and expected memory usage."
                                                                       << endl;
        {
            bool anyObjectMemoryAllocatedFlag = false;

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int          LINE     = DATA[ti].d_line;
                const char         MEM      = DATA[ti].d_mem;
                const bool         BOOL     = DATA[ti].d_bool;
                const char *const  NUMBER   = DATA[ti].d_number_p;
                const char *const  STRING   = DATA[ti].d_string_p;
                const Json         ARRAY[3] = {Json(BOOL),
                                               Json(JsonNumber(NUMBER)),
                                               Json(STRING)};

                if (veryVerbose) { T_ P_(MEM) P_(BOOL) P_(NUMBER) P_(STRING) }

                ASSERTV(LINE, MEM, MEM && strchr("YN?", MEM));

                for (char cfg = 'a'; cfg <= 'c'; ++cfg) {

                    const char CONFIG = cfg;

                    if (veryVerbose) { T_ T_ P(CONFIG) }

                    bslma::TestAllocator da("default",   veryVeryVeryVerbose);
                    bslma::TestAllocator fa("footprint", veryVeryVeryVerbose);
                    bslma::TestAllocator sa("supplied",  veryVeryVeryVerbose);

                    bslma::DefaultAllocatorGuard dag(&da);

                    Obj                  *objPtr          = 0;
                    bslma::TestAllocator *objAllocatorPtr = 0;

                    switch (CONFIG) {
                      case 'a': {
                        objAllocatorPtr = &da;
                        objPtr = new (fa) Obj(ARRAY, ARRAY + 3);
                      } break;
                      case 'b': {
                        objAllocatorPtr = &da;
                        objPtr = new (fa) Obj(ARRAY, ARRAY + 3,
                                              objAllocatorPtr);
                      } break;
                      case 'c': {
                        objAllocatorPtr = &sa;
                        objPtr = new (fa) Obj(ARRAY, ARRAY + 3,
                                              objAllocatorPtr);
                      } break;
                      default: {
                        BSLS_ASSERT_OPT(!"Bad allocator config.");
                      } break;
                    }

                    ASSERTV(LINE, CONFIG,
                            sizeof(Obj) == fa.numBytesInUse());

                    Obj& mX = *objPtr;  const Obj& X = mX;

                    if (veryVerbose) { T_ T_ P_(CONFIG) P(X) }

                    bslma::TestAllocator&  oa = *objAllocatorPtr;
                    bslma::TestAllocator& noa = (&da == &oa) ? sa : da;

                    // -------------------------------------
                    // Verify the object's attribute values.
                    // -------------------------------------

                    ASSERTV(LINE, CONFIG, X[0].type(), X[0].isBoolean());
                    ASSERTV(LINE, CONFIG, BOOL, X[0],
                            BOOL == X[0].theBoolean());
                    ASSERTV(LINE, CONFIG, X[1].type(), X[1].isNumber());
                    ASSERTV(LINE, CONFIG, NUMBER, X[1],
                            NUMBER == X[1].theNumber().value());
                    ASSERTV(LINE, CONFIG, X[2].type(), X[2].isString());
                    ASSERTV(LINE, CONFIG, STRING, X[2],
                            STRING == X[2].theString());

                    // ------------------------------------------------------
                    // Verify any attribute allocators are installed properly
                    // ------------------------------------------------------

                    ASSERTV(LINE, CONFIG, &oa == X[1].allocator());
                    ASSERTV(LINE, CONFIG, &oa == X[2].allocator());
                    ASSERTV(LINE, CONFIG, &oa == X.allocator());
                    ASSERTV(CONFIG, noa.numBlocksTotal(),
                            0 == noa.numBlocksTotal());

                    // Verify no temporary memory is allocated from the object
                    // allocator.

                    ASSERTV(LINE, CONFIG,
                            oa.numBlocksTotal(),
                            oa.numBlocksInUse(),
                            oa.numBlocksTotal() == oa.numBlocksInUse());

                    if ('?' != MEM) {
                        ASSERTV(LINE, CONFIG, MEM, oa.numBlocksInUse(),
                                ('N' == MEM) == (1 >= oa.numBlocksInUse()));
                    }

                    // Record if some object memory was allocated.
                    anyObjectMemoryAllocatedFlag |= !!oa.numBlocksInUse();

                    // Reclaim dynamically allocated object under test.
                    fa.deleteObject(objPtr);

                    // Verify all memory is released on object destruction.

                    ASSERTV(LINE, CONFIG, da.numBlocksInUse(),
                            0 == da.numBlocksInUse());
                    ASSERTV(LINE, CONFIG, fa.numBlocksInUse(),
                            0 == fa.numBlocksInUse());
                    ASSERTV(LINE, CONFIG, sa.numBlocksInUse(),
                            0 == sa.numBlocksInUse());
                }  // end foreach configuration

            }   // end foreach row

            // Double check that some object memory was allocated.

            ASSERT(anyObjectMemoryAllocatedFlag);
        }

        if (verbose) cout << "\nTesting with injected exceptions." << endl;
        {
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int          LINE     = DATA[ti].d_line;
                const char         MEM      = DATA[ti].d_mem;
                const bool         BOOL     = DATA[ti].d_bool;
                const char *const  NUMBER   = DATA[ti].d_number_p;
                const char *const  STRING   = DATA[ti].d_string_p;
                const Json         ARRAY[3] = {Json(BOOL),
                                               Json(JsonNumber(NUMBER)),
                                               Json(STRING)};

                if (veryVerbose) { T_ P_(MEM) P_(BOOL) P_(NUMBER) P_(STRING) }

                bslma::TestAllocator da("default",  veryVeryVeryVerbose);
                bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);

                bslma::DefaultAllocatorGuard dag(&da);

                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(sa) {
                    if (veryVeryVerbose) { T_ T_ Q(ExceptionTestBody) }

                    Obj obj(ARRAY, ARRAY + 3, &sa);
                    ASSERTV(LINE, ti, obj[0].type(), obj[0].isBoolean());
                    ASSERTV(LINE, ti, BOOL, obj[0],
                            BOOL == obj[0].theBoolean());
                    ASSERTV(LINE, ti, obj[1].type(), obj[1].isNumber());
                    ASSERTV(LINE, ti, NUMBER, obj[1],
                            NUMBER == obj[1].theNumber().value());
                    ASSERTV(LINE, ti, obj[2].type(), obj[2].isString());
                    ASSERTV(LINE, ti, STRING, obj[2],
                            STRING == obj[2].theString());

#ifdef BDE_BUILD_TARGET_EXC
                    if ('Y' == MEM) {
                        ASSERTV(LINE, 0 < EXCEPTION_COUNT);
                    }
#endif
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                ASSERTV(LINE, da.numBlocksInUse(),
                        0 == da.numBlocksInUse());
                ASSERTV(LINE, sa.numBlocksInUse(),
                        0 == sa.numBlocksInUse());
            }
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // JSONARRAY DEFAULT CTOR, PRIMARY MANIPULATORS, & DTOR
        //   Ensure that we can use the default constructor of JsonArray to
        //   create an object (having the default constructed value), use the
        //   primary manipulators to put that object into any state relevant
        //   for thorough testing, and use the destructor to destroy it safely.
        //
        // Concerns:
        //: 1 An object created with the default constructor (with or without a
        //:   supplied allocator) has the contractually specified default
        //:   value.
        //:
        //: 2 If an allocator is NOT supplied to the default constructor, the
        //:   default allocator in effect at the time of construction becomes
        //:   the object allocator for the resulting object.
        //:
        //: 3 If an allocator IS supplied to the default constructor, that
        //:   allocator becomes the object allocator for the resulting object.
        //:
        //: 4 Supplying a default-constructed allocator has the same effect as
        //:   not supplying an allocator.
        //:
        //: 5 Supplying an allocator to the default constructor has no effect
        //:   on subsequent object values.
        //:
        //: 6 Any memory allocation is from the object allocator.
        //:
        //: 7 There is no temporary allocation from any allocator.
        //:
        //: 8 Every object releases any allocated memory at destruction.
        //:
        //: 9 QoI: The default constructor allocates no memory.
        //:
        //:10 Each attribute is modifiable independently.
        //:
        //:11 Each attribute can be set to represent any value that does not
        //:   violate that attribute's documented constraints.
        //:
        //:12 Any string arguments can be of type 'char *' or 'string'.
        //:
        //:13 Any arguments can be 'const'.
        //:
        //:14 Any memory allocation is exception neutral.
        //:
        //:15 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Create three sets of attribute values for the object: ('D')
        //:   values corresponding to the default-constructed object, ('A')
        //:   values that allocate memory if possible, and ('B') other values
        //:   that do not cause additional memory allocation beyond that which
        //:   may be incurred by 'A'.  Both the 'A' and 'B' attribute values
        //:   should be chosen to be boundary values where possible.  If an
        //:   attribute can be supplied via alternate C++ types (e.g., 'string'
        //:   instead of 'char *'), use the alternate type for 'B'.
        //:
        //: 2 Execute an inner loop that creates an object by
        //:   default-construction, but invokes the default constructor
        //:   differently in each iteration: (a) without passing an allocator,
        //:   (b) passing a default-constructed allocator explicitly (c)
        //:   passing the address of a test allocator distinct from the
        //:   default, and (d) passing in an allocator constructed from the
        //:   address of a test allocator distinct from the default.  For each
        //:   of these iterations: (C-1..14)
        //:
        //:   1 Create three 'bslma::TestAllocator' objects, and install one as
        //:     as the current default allocator (note that a ubiquitous test
        //:     allocator is already installed as the global allocator).
        //:
        //:   2 Use the default constructor to dynamically create an object
        //:     'X', with its object allocator configured appropriately (see
        //:     P-2); use a distinct test allocator for the object's footprint.
        //:
        //:   3 Use the 'allocator' accessor of each underlying attribute
        //:     capable of allocating memory to ensure that its object
        //:     allocator is properly installed; also invoke the (as yet
        //:     unproven) 'allocator' accessor of the object under test.
        //:     (C-2..4)
        //:
        //:   4 Use the appropriate test allocators to verify that no memory
        //:     is allocated by the default constructor.  (C-9)
        //:
        //:   5 Use the individual (as yet unproven) salient attribute
        //:     accessors to verify the default-constructed value.  (C-1)
        //:
        //:   6 For each attribute 'i', in turn, create a local block.  Then
        //:     inside the block, using brute force, set that attribute's
        //:     value, passing a 'const' argument representing each of the
        //:     three test values, in turn (see P-1), first to 'Ai', then to
        //:     'Bi', and finally back to 'Di'.  If attribute 'i' can allocate
        //:     memory, verify that it does so on the first value transition
        //:     ('Di' -> 'Ai'), and that the corresponding primary manipulator
        //:     is exception neutral (using the
        //:     'BSLMA_TESTALLOCATOR_EXCEPTION_TEST_*' macros).  In all other
        //:     cases, verify that no memory allocation occurs.  After each
        //:     transition, use the (as yet unproven) basic accessors to verify
        //:     that only the intended attribute value changed.  (C-5..6,
        //:     11..14)
        //:
        //:   7 Corroborate that attributes are modifiable independently by
        //:     first setting all of the attributes to their 'A' values.  Then
        //:     incrementally set each attribute to it's corresponding  'B'
        //:     value and verify after each manipulation that only that
        //:     attribute's value changed.  (C-10)
        //:
        //:   8 Verify that no temporary memory is allocated from the object
        //:     allocator.  (C-7)
        //:
        //:   9 Verify that all object memory is released when the object is
        //:     destroyed.  (C-8)
        //:
        //: 3 Verify that, in appropriate build modes, defensive checks are
        //:   triggered for invalid attribute values, but not triggered for
        //:   adjacent valid ones (using the 'BSLS_ASSERTTEST_*' macros).
        //:   (C-15)
        //
        // Testing:
        //   JsonArray();
        //   JsonArray(const bslma::allocator *a);
        //   ~JsonArray();
        //   JsonArray::pushBack(const Json& j);
        // --------------------------------------------------------------------

        typedef bdljsn::JsonArray Obj;

        if (verbose) cout << endl
             << "JSONARRAY DEFAULT CTOR, PRIMARY MANIPULATORS, & DTOR" << endl
             << "====================================================" << endl;

        if (verbose) cout << "\nEstablish suitable attribute values." << endl;

        // 'D' values: These are the default-constructed values.

        const int  D1   = 0;  // Json variant JsonNumber
        const char D2[] = ""; // Json variant bsl::string

        // 'A' values: Should cause memory allocation if possible.

        const int  A1   = 1;
        const char A2[] = "a_" SUFFICIENTLY_LONG_STRING;

        // 'B' values: Should NOT cause allocation (use alternate string type).
        const int              B1 = 2;
        const bsl::string_view B2 = "b";

        if (verbose) cout << "\nTesting with various allocator configurations."
                          << endl;

        for (char cfg = 'a'; cfg <= 'c'; ++cfg) {

            const char CONFIG = cfg;

            bslma::TestAllocator da("default",   veryVeryVeryVerbose);
            bslma::TestAllocator fa("footprint", veryVeryVeryVerbose);
            bslma::TestAllocator sa("supplied",  veryVeryVeryVerbose);

            bslma::DefaultAllocatorGuard dag(&da);

            Obj                  *objPtr = 0;
            bslma::TestAllocator *objAllocatorPtr = 0;

            switch (CONFIG) {
              case 'a': {
                objAllocatorPtr = &da;
                objPtr = new (fa) Obj();
              } break;
              case 'b': {
                objAllocatorPtr = &da;
                objPtr = new (fa) Obj(0);
              } break;
              case 'c': {
                objAllocatorPtr = &sa;
                objPtr = new (fa) Obj(objAllocatorPtr);
              } break;
              default: {
                BSLS_ASSERT_OPT(!"Bad allocator config.");
              } break;
            }

            Obj&                   mX = *objPtr;  const Obj& X = mX;
            bslma::TestAllocator&  oa = *objAllocatorPtr;
            bslma::TestAllocator& noa = (&da == &oa) ? sa : da;

            // --------------------------------------
            // Verify object allocator is as expected
            // --------------------------------------

            ASSERTV(CONFIG, &oa, X.allocator(), &oa == X.allocator());

            // Verify no allocation from the object/non-object allocators

            ASSERTV(CONFIG, oa.numBlocksTotal(), 0 == oa.numBlocksTotal());
            ASSERTV(CONFIG, noa.numBlocksTotal(), 0 == noa.numBlocksTotal());

            // ---------------------------------
            // Set attribute values to defaults.
            // ---------------------------------

            mX.pushBack(bdljsn::Json(D1));
            mX.pushBack(bdljsn::Json(D2));

            // -------------------------------------
            // Verify the object's attribute values.
            // -------------------------------------

            ASSERTV(CONFIG, X.size(), 2 == X.size());
            ASSERTV(CONFIG, X[0].type(), X[0].isNumber());
            {
                int value;
                int status = X[0].theNumber().asInt(&value);
                ASSERTV(status, 0 == status);
                ASSERTV(CONFIG, D1, X[0].theNumber(), D1 == value);
            }
            ASSERTV(CONFIG, X[1].type(), X[1].isString());
            ASSERTV(CONFIG, D2, X[1].theString(), X[1].theString() == D2);

            // ---------------------------------------------------------------
            // Verify that each attribute is independently settable.
            // ---------------------------------------------------------------

            {
                bslma::TestAllocatorMonitor tam(&oa);

                mX[0] = A1;
                ASSERTV(CONFIG, X[0].type(), X[0].isNumber());
                {
                    int value;
                    int status = X[0].theNumber().asInt(&value);
                    ASSERTV(status, 0 == status);
                    ASSERTV(CONFIG, A1, X[0].theNumber(), A1 == value);
                }
                ASSERTV(CONFIG, X[1].type(), X[1].isString());
                ASSERTV(CONFIG, D2, X[1].theString(), X[1].theString() == D2);

                mX[0] = B1;
                ASSERTV(CONFIG, X[0].type(), X[0].isNumber());
                {
                    int value;
                    int status = X[0].theNumber().asInt(&value);
                    ASSERTV(status, 0 == status);
                    ASSERTV(CONFIG, B1, X[0].theNumber(), B1 == value);
                }
                ASSERTV(CONFIG, X[1].type(), X[1].isString());
                ASSERTV(CONFIG, D2, X[1].theString(), X[1].theString() == D2);

                mX[0] = D1;
                ASSERTV(CONFIG, X[0].type(), X[0].isNumber());
                {
                    int value;
                    int status = X[0].theNumber().asInt(&value);
                    ASSERTV(status, 0 == status);
                    ASSERTV(CONFIG, D1, X[0].theNumber(), D1 == value);
                }
                ASSERTV(CONFIG, X[1].type(), X[1].isString());
                ASSERTV(CONFIG, D2, X[1].theString(), X[1].theString() == D2);

                ASSERTV(CONFIG, tam.isTotalSame());
            }

            {
                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                    if (veryVeryVerbose) { T_ T_ Q(ExceptionTestBody) }

                    bslma::TestAllocatorMonitor tam(&oa);
                    mX[1].makeString(A2);
                    ASSERTV(CONFIG, tam.isInUseUp());

#ifdef BDE_BUILD_TARGET_EXC
                    ASSERTV(CONFIG, 0 < EXCEPTION_COUNT);
#endif
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                ASSERTV(CONFIG, X[0].type(), X[0].isNumber());
                {
                    int value;
                    int status = X[0].theNumber().asInt(&value);
                    ASSERTV(status, 0 == status);
                    ASSERTV(CONFIG, D1, X[0].theNumber(), D1 == value);
                }
                ASSERTV(CONFIG, X[1].type(), X[1].isString());
                ASSERTV(CONFIG, A2, X[1].theString(), X[1].theString() == A2);

                bslma::TestAllocatorMonitor tam(&oa);

                mX[1].makeString(B2);
                ASSERTV(CONFIG, X[0].type(), X[0].isNumber());
                {
                    int value;
                    int status = X[0].theNumber().asInt(&value);
                    ASSERTV(status, 0 == status);
                    ASSERTV(CONFIG, D1, X[0].theNumber(), D1 == value);
                }
                ASSERTV(CONFIG, X[1].type(), X[1].isString());
                ASSERTV(CONFIG, B2, X[1].theString(), X[1].theString() == B2);

                mX[1].makeString(D2);
                ASSERTV(CONFIG, X[0].type(), X[0].isNumber());
                {
                    int value;
                    int status = X[0].theNumber().asInt(&value);
                    ASSERTV(status, 0 == status);
                    ASSERTV(CONFIG, D1, X[0].theNumber(), D1 == value);
                }
                ASSERTV(CONFIG, X[1].type(), X[1].isString());
                ASSERTV(CONFIG, D2, X[1].theString(), X[1].theString() == D2);

                ASSERTV(CONFIG, tam.isTotalSame());
            }
            // Verify no temporary memory is allocated from the object
            // allocator.  One for vector storage, one for string A2.

            ASSERTV(CONFIG, oa.numBlocksMax(), 2 == oa.numBlocksMax());

            // Reclaim dynamically allocated object under test.

            fa.deleteObject(objPtr);

            // Verify all memory is released on object destruction.

            ASSERTV(fa.numBlocksInUse(),  0 ==  fa.numBlocksInUse());
            ASSERTV(oa.numBlocksInUse(),  0 ==  oa.numBlocksInUse());
            ASSERTV(noa.numBlocksTotal(), 0 == noa.numBlocksTotal());

            // Double check that some object memory was allocated.

            ASSERTV(CONFIG, 1 <= oa.numBlocksTotal());

            // Note that memory should be independently allocated for each
            // attribute capable of allocating memory.
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //   This case exercises (but does not fully test) basic functionality.
        //
        // Concerns:
        //: 1 The class is sufficiently functional to enable comprehensive
        //    testing in subsequent test cases.
        //
        // Plan:
        //: 1 Default construct a Json object.
        //: 2 Default construct a JsonObject object.
        //: 3 Default construct a JsonArray object.
        //: 4 Add the Json object to both container objects
        //: 5 Clear both container objects.
        //: 6 Exercise, in a trivial manner, all public-facing member and
        //:   free functions of 'Json', 'JsonArray', and 'JsonObject'.
        //
        // Testing:
        // --------------------------------------------------------------------

        if (verbose)
            cout << endl
                 << "BREATHING TEST" << endl
                 << "==============" << endl;

        bdljsn::Json       json;
        bdljsn::JsonObject object;
        bdljsn::JsonArray  array;

        ASSERT(0 == object.size());
        ASSERT(0 == array.size());

        object["Key1"] = json;
        array.pushBack(json);

        ASSERT(1 == object.size());
        ASSERT(1 == array.size());

        // note that the first type is not 'const'
        bsl::pair<bsl::string, Json> ARRAY[] = {
            bdljsn::JsonObject::Member("a", Json()),
            bdljsn::JsonObject::Member("b", Json(1))};

        object.insert(ARRAY, ARRAY + 2);
        object.insert("bool", true);
        object.insert("int", 1);
        object.insert("double", 3.1415);
        object.insert("cstring", "JsonObject");
        object.insert("string", bsl::string("bsl::string"));


        ASSERT(bdljsn::Json(true) == object["bool"]);
        ASSERT(bdljsn::Json(1) == object["int"]);
        ASSERT(bdljsn::Json(3.1415) == object["double"]);
        ASSERT(bdljsn::Json("JsonObject") == object["cstring"]);
        ASSERT(bdljsn::Json("bsl::string") == object["string"]);
        ASSERT(bdljsn::Json() == object["a"]);
        ASSERT(bdljsn::Json(1) == object["b"]);

        if (veryVerbose)
            cout << endl
                 << "EXTENDED BREATHING TEST" << endl
                 << "-----------------------" << endl;

        extendedBreathingTest(verbose, veryVerbose);

      } break;
      default: {
        bsl::cerr << "WARNING: CASE `" << test << "' NOT FOUND." << bsl::endl;
        testStatus = -1;
      } break;
    }

    // CONCERN: In no case does memory come from the global allocator.

    ASSERTV(globalAllocator.numBlocksTotal(),
            0 == globalAllocator.numBlocksTotal());

    if (testStatus > 0) {
        bsl::cerr << "Error, non-zero test status = "
                  << testStatus
                  << "."
                  << bsl::endl;
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
