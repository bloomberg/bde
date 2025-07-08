// baljsn_jsonformatter.h                                             -*-C++-*-
#ifndef INCLUDED_BALJSN_JSONFORMATTER
#define INCLUDED_BALJSN_JSONFORMATTER

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a formatter for converting `bdlat` object to `Json` analog.
//
//@CLASSES:
// baljsn::JsonFormatter: mechanism for setting the value of a `bdljsn::Json`
//
//@SEE_ALSO: baljsn_encoder, baljsn_formatter, bdljsn_json
//
//@DESCRIPTION: This component provides a stateful mechanism,
// `baljsn::JsonFormatter`, that sets the value of a `bdljsn::Json` object
// according to the invocations of the formatter`s various manipulator methods.
//
// The target `bdljsn::Json` object can be set to either a JSON scalar
// (i.e., `isNull()`, `isBoolean()`, `isNumber()`, `isString()`), or an a JSON
// array (`isArray()`), or a JSON object (`isObject()`).  Recall that JSON
// arrays consist of a list of elements that can each be either a scalar or an
// object or an array.  Also recall that each object is a name/value pair
// where the value portion can be either a scalar or an object or an array.
//
// The `bdljsn::Json` object must be built up in top-down order.  The proper
// order of manipulator calls must, in effect, follow a pre-order tranversal
// of the object being created.  For details of adding arrays and adding
// objects see [](#Assembling an Array) and [](#Assembling an Object).
// Calling these mainipulators in other orders can cause undefined behavior.
//
// The provided manipulators do *not* allow for any editing once a value has
// been set.  Once a `bdljsn::Json` target object has been given a scalar value
// or started as an array or as an object, any attempt to undo that action
// results in undefined behavior.
//
// Note that two of the manipulators, `addArrayElementSeparators` and
// `nestingDepth`, are not relevant to assembling `bdljsn::Json` objects
// and are defined as "no-ops".  These manipulators are provided so
// `baljsn::JsonFormatter` can be used in the same context as
// `baljsn::Formatter`.
//
///Assembling an Array
///-------------------
// A `bdljsn::Json` object of array type is assembled using the following
// series of manipulator calls.
//
// # `openArray` starts an array (the "current" array).
//
// # `putValue`/`putNullValue` calls each adds to the current array an
//    element having the appropriate scalar type.
//
// # `openObject` adds an array element that is a name/value pair (an Json
//    object).
//
// # `openArray` starts a new array within the current array.  Subsequent
//   operations assemble this new array until `closeArray` is called.
//
// # Repeat the addition of new elements to the current array by calls
//   to `putValue`/`putNullValue`/`openObject`/`openArray` as appropriate.
//
// # `closeArray` indicates that there are no more elements to be added
//   to the current array.
//
///Assembling an Object
///--------------------
// A `bdljsn::Json` object of object type (i.e., a name/value) is assembled
// using the following series of manipulator calls in the order shown
// below.
//
// # `openObject` starts a new object (the "current" object).
//
// # `openMember` starts a new member of the object and provides the name
//   portion of the name/value pair.
//
// # `putValue`/`putNullValue` sets the provided value as the "value" portion
//   of the name/value pair.
//
// # `openArray` starts an array as the value portion of the name/value pair.
//
// # `openObject` starts a new object as the value portion of the
//   name/value pair.  Subsequent calls define this object until `closeMember`
//   is called.
//
// # `closeMember` indicates that the member has been completed.
//
// # Repeat the addition of new members as appropriate.
//
// # `closeObject` indicates that there are no more members to be added to the
//   current object.
//
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
//  { "name"        : "Bob",
//    "homeAddress" :
//       { "street" : "Lexington Ave",
//         "city"   : "New York City",
//         "state"  : "New York"
//       },
//    "age"        :  21
//  }
// ```
// First, we create a `bdljsn::Json` object and directly use the manipulators
// provided by that class.
// ```
//  bdljsn::Json json1;
//  json1.makeObject();
//  json1["name"];
//  json1["homeAddress"];
//  json1["age"];
//
//  bdljsn::Json address;
//  address.makeObject();
//  address["street"] = "Lexington Ave";
//  address["city"]   = "New York City";
//  address["state"]  = "New York";
//
//  json1["name"]        = "Bob";
//  json1["homeAddress"] = address;
//  json1["age"]         = 21;
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
// the `baljsn::JsonFormatter` mechanism.
// ```
//  bdljsn::Json          json2;
//  baljsn::JsonFormatter formatter(&json2);
//
//  formatter.openObject();
//
//      formatter.openMember("name");
//      formatter.putValue("Bob");
//      formatter.closeMember();
//
//      formatter.openMember("homeAddress");
//      formatter.openObject();  // The "value" of the "homeAddress" member.
//
//          formatter.openMember("street");
//          formatter.putValue("Lexington Ave");
//          formatter.closeMember();
//
//          formatter.openMember("city");
//          formatter.putValue("New York City");
//          formatter.closeMember();
//
//          formatter.openMember("state");
//          formatter.putValue("New York");
//          formatter.closeMember();
//
//      formatter.closeObject();
//      formatter.closeMember(); // "homeAddress"
//
//      formatter.openMember("age");
//      formatter.putValue(21);
//      formatter.closeMember();
//
//     formatter.closeObject();
// ```
// Finally, we confirm that the two assembled objects are equal.
//
// ```
//  assert(json1 == json2);
// ```

#include <balscm_version.h>

#include <baljsn_printutil.h>

#include <bdljsn_json.h>

#include <bdlt_date.h>
#include <bdlt_datetime.h>
#include <bdlt_datetimetz.h>
#include <bdlt_datetz.h>
#include <bdlt_time.h>
#include <bdlt_timetz.h>
#include <bdlt_iso8601util.h>
#include <bdlt_iso8601utilconfiguration.h>

#include <bdlb_float.h>

#include <bslma_aatypeutil.h>
#include <bslma_allocatorutil.h>
#include <bslma_bslallocator.h>

#include <bslmf_nestedtraitdeclaration.h>

#include <bsla_fallthrough.h>
#include <bsla_maybeunused.h>

#include <bsls_assert.h>

#include <bsl_cstddef.h>    // `bsl::size_t`
#include <bsl_ostream.h>
#include <bsl_stack.h>
#include <bsl_string.h>
#include <bsl_string_view.h>
#include <bsl_utility.h>    // `bsl::pair`
#include <bsl_sstream.h>    // `bsl::ostringstream`

namespace BloombergLP {
namespace baljsn {

class EncoderOptions;

                          // =========================
                          // class JsonFormatter_State
                          // =========================

/// This `struct` defines an enumeration of the internal states that a
/// a `JsonFormatter` can reach as it processes user requests to `openArray`,
/// `putValue`(s), `closeArray`, etc.  These states influence the formatter
/// and, in some build modes, detect errors in call sequences.
struct JsonFormatter_State {

    // TYPES
    enum Enum {
      e_AT_START = 0
    , e_IN_ARRAY
    , e_IN_OBJECT_EXPECTING_MEMBER_NAME
    , e_IN_OBJECT_EXPECTING_MEMBER_VALUE
    , e_IN_OBJECT_EXPECTING_MEMBER_CLOSE
    };

    // CLASS METHODS
    static bsl::ostream& print(bsl::ostream& stream,
                               Enum          value,
                               int           level          = 0,
                               int           spacesPerLevel = 4);
    static const char *toAscii(Enum value);
};

bsl::ostream& operator<<(bsl::ostream&             stream,
                         JsonFormatter_State::Enum value);

                          // -------------------------
                          // class JsonFormatter_State
                          // -------------------------

// FREE OPERATORS
inline
bsl::ostream& operator<<(bsl::ostream&             stream,
                         JsonFormatter_State::Enum value)
{
    return JsonFormatter_State::print(stream, value, 0, -1);
}

                        // ========================
                        // class JsonFormatter_Util
                        // ========================

struct JsonFormatter_Util {

#ifdef BDE_BUILD_TARGET_SAFE
    template <class TYPE>
    static int expectedReturnPutValue(const TYPE&           value,
                                      const EncoderOptions *options);

    static int expectedReturnPutValue(const char           *value,
                                      const EncoderOptions *options);

    template <class TYPE>
    static int expectedReturnFloatingPoint(const TYPE&           value,
                                           const EncoderOptions *options);
#endif

    template <class TYPE>
    static int setFloatingPointToJson(bdljsn::Json         *json,
                                      const TYPE&           value,
                                      const EncoderOptions *options);

    template <class TYPE>
    static int setTemporalValueToJson(bdljsn::Json         *json,
                                      const TYPE&           value,
                                      const EncoderOptions *options);

    template <class TYPE>
    static int valueAsJson(bdljsn::Json         *json,
                           const TYPE&           value,
                           const EncoderOptions *options);

    static int valueAsJson(bdljsn::Json         *json,
                           const char           *value,
                           const EncoderOptions *options);

};

                          // ===================
                          // class JsonFormatter
                          // ===================

/// This class implements a formatter mechanism that assembles a (potentially
/// complicated) `bdljsn::Json` object according to the invocations of the
/// formatter's manipulators.
class JsonFormatter {

    // PRIVATE TYPES

    typedef JsonFormatter_State                    State;
    typedef bsl::pair<bdljsn::Json *, State::Enum> Status;

  public:

    // TYPES

    typedef bsl::allocator<char> allocator_type;

    // DATA

    allocator_type      d_allocator;
    bsl::stack<Status>  d_stack;

    // PRIVATE MANIPULATORS

    /// Print the internal stack of `Status` objects to standard output in lifo
    /// order.  Each line of output consists of:
    ///
    /// * The stack depth
    /// * The `bdljsn::JsonType` of the referenced `bdljsn::Json` object.
    /// * The internal state, a `JsonFormatter_State::Enum` value.
    ///
    /// Note that this function is provided for debugging only.
    void dumpStack(); // Restores stack state to state when called.

  public:
    // CREATORS

    /// Create a `JsonFormatter` object that modifies the specified `json`
    /// object in accordance with subsequent calls the formatter manipulators.
    /// Optionally specify an `allocator` (e.g., the address of a
    /// `bslma::Allocator` object) to supply memory; otherwise, the default
    /// allocator is used.  The behavior is undefined unless `json->isNull()`.
    JsonFormatter(bdljsn::Json          *json,
                  const allocator_type&  allocator = allocator_type());

    /// Destroy this object.
    //! ~JsonFormatter() = default;

    // MANIPULATORS

    /// Create a new object (set of name/value pairs also know as "members").
    /// Note subsequently created members will land in this object until
    /// `closeObject` is called.
    void openObject();

    /// Indicate that there are no more members to be added to the current
    /// object.
    void closeObject();

    /// Create a new array.  Subsequently created elements will land in this
    /// array (in order of creation) until `closeArray` is called.  The
    /// elements can be created by
    /// `putValue`/`putNullValue`/`openObject`/`openArray`.  The specified
    /// `formatAsEmptyArray` is ignored; arrays are unconditionally formatted.
    void openArray(bool formatAsEmptyArray = false);

    /// Indicate that there are no more elements to be added to the current
    /// array.  The specified `formatAsEmptyArray` is ignored; arrays are
    /// unconditionally formatted.
    void closeArray(bool formatAsEmptyArray = false);

    /// Create a new object member having the specified `name`.  Return 0 on
    /// success and a non-zero value otherwise.  An error is returned if `name`
    /// contains a non-UTF8 character.
    int openMember(const bsl::string_view& name);

    /// Set the value of the current array element or object value -- depending
    /// on the context -- to `bdljsn::JsonNull`.  Return 0 on success and a
    /// non-zero value otherwise.  An error is returned when `putNullValue`
    /// is called in an unexpected context.
    int putNullValue();

    /// Set the value of the current scalar, array element, or object value --
    /// depending on the context -- to the specified `value` of (template
    /// parameter) `TYPE`.  The specified `options` is ignored except:
    ///    * `encodeInfAndNaNAsStrings` when `TYPE` is a floating point type.
    ///    * `datetimeFractionalSecondPrecision` when `TYPE` features a
    ///       datetime or time attribute.
    /// Return 0 on success and a non-zero value otherwise.  Errors are
    /// returned if `value` is disallowed for `bdljsn::Json` objects:
    ///    * `value` is a string type and contains a non-UTF8 character.
    ///    * `value` is a floating point type, is INF or NaN, and
    ///      `false == encodeInfAndNanAsStrings`.
    template <class TYPE>
    int putValue(const TYPE& value, const EncoderOptions *options = 0);

    /// Indicate that there are no more members to be added to the current
    /// object.
    void closeMember();

    /// No-op.
    void addArrayElementSeparator();

    // ACCESSORS

    /// No-op. Return 0.
    int nestingDepth() const;

                                  // Aspects

    /// Return the allocator used by this object to supply memory.  Note
    /// that if no allocator was supplied at construction the default
    /// allocator in effect at construction is used.
    allocator_type get_allocator() const;
};

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

                        // ------------------------
                        // class JsonFormatter_Util
                        // ------------------------

#ifdef BDE_BUILD_TARGET_SAFE
template <class TYPE>
int JsonFormatter_Util::expectedReturnPutValue(BSLA_MAYBE_UNUSED
                                               const TYPE&           value,
                                               BSLA_MAYBE_UNUSED
                                               const EncoderOptions *options)
{
    return 0;
}

template <class TYPE>
inline
int JsonFormatter_Util::expectedReturnFloatingPoint(
                                                 const TYPE&           value,
                                                 const EncoderOptions *options)
{
    switch(bdlb::Float::classify(value)) {
      case bdlb::Float::k_INFINITE:                           BSLA_FALLTHROUGH;
      case bdlb::Float::k_NAN: {
        return options && options->encodeInfAndNaNAsStrings() ?  0 : -1;
                                                                      // RETURN
     } break;
      default: {
        return 0;                                                     // RETURN
      } break;
    }
}

template <>
inline
int JsonFormatter_Util::expectedReturnPutValue(const float&          value,
                                               const EncoderOptions *options)
{
    return expectedReturnFloatingPoint(value, options);
}

template <>
inline
int JsonFormatter_Util::expectedReturnPutValue(const double&         value,
                                               const EncoderOptions *options)
{
    return expectedReturnFloatingPoint(value, options);
}

inline
int JsonFormatter_Util::expectedReturnPutValue(const char           *value,
                                               BSLA_MAYBE_UNUSED
                                               const EncoderOptions *options)
{
    BSLS_ASSERT(value);
    return bdlde::Utf8Util::isValid(value) ? 0 : -1;
}

template <>
inline
int JsonFormatter_Util::expectedReturnPutValue(const bsl::string&    value,
                                               BSLA_MAYBE_UNUSED
                                               const EncoderOptions *options)
{
    return bdlde::Utf8Util::isValid(value.data(), value.length()) ? 0 : -1;
}
#endif // BDE_BUILD_TARGET_SAFE

template <class TYPE>
int JsonFormatter_Util::setFloatingPointToJson(bdljsn::Json         *json,
                                               const TYPE&           value,
                                               const EncoderOptions *options)
{
    BSLS_ASSERT_SAFE(json);

    bdlb::Float::FineClassification classification =
                                              bdlb::Float::classifyFine(value);
    switch (classification) {
      case bdlb::Float::k_POSITIVE_INFINITY:                  BSLA_FALLTHROUGH;
      case bdlb::Float::k_NEGATIVE_INFINITY:                  BSLA_FALLTHROUGH;
      case bdlb::Float::k_QNAN:                               BSLA_FALLTHROUGH;
      case bdlb::Float::k_SNAN: {
        if (options && options->encodeInfAndNaNAsStrings()) {
            const char *asString = PrintUtil::floatingPointSpecialAsString(
                                                               classification);
            BSLS_ASSERT_SAFE(asString);
            json->makeString(asString);
        }
        else {
            return -1;                                                // RETURN
        }
      } break;
      case bdlb::Float::k_NEGATIVE:                           BSLA_FALLTHROUGH;
      case bdlb::Float::k_SIGNALING:                          BSLA_FALLTHROUGH;
      case bdlb::Float::k_POSITIVE_NORMAL:                    BSLA_FALLTHROUGH;
      case bdlb::Float::k_NEGATIVE_NORMAL:                    BSLA_FALLTHROUGH;
      case bdlb::Float::k_POSITIVE_SUBNORMAL:                 BSLA_FALLTHROUGH;
      case bdlb::Float::k_NEGATIVE_SUBNORMAL:                 BSLA_FALLTHROUGH;
      case bdlb::Float::k_POSITIVE_ZERO:                      BSLA_FALLTHROUGH;
      case bdlb::Float::k_NEGATIVE_ZERO: {
        *json = value;
      } break;
      default: {
        BSLS_ASSERT_INVOKE_NORETURN("Should not reach.");
      } break;
    }
    return 0;
}

template <class TYPE>
int JsonFormatter_Util::setTemporalValueToJson(bdljsn::Json         *json,
                                               const TYPE&           value,
                                               const EncoderOptions *options)
{
    BSLS_ASSERT_SAFE(json);

    bdlt::Iso8601UtilConfiguration config;
    if (options) {
        config.setFractionalSecondPrecision(
                                 options->datetimeFractionalSecondPrecision());
    }
    else {
        config.setFractionalSecondPrecision(3);
    }

    const bsl::size_t bufferSize = bdlt::Iso8601Util::k_MAX_STRLEN + 1;
    char              buffer[bufferSize];
    bdlt::Iso8601Util::generate(buffer, bufferSize, value, config);
    json->makeString(buffer);
    return 0;
}

template <class TYPE>
int JsonFormatter_Util::valueAsJson(bdljsn::Json         *json,
                                    const TYPE&           value,
                                    BSLA_MAYBE_UNUSED
                                    const EncoderOptions *options)
{
    BSLS_ASSERT_SAFE(json);
    *json = value;
    return 0;
}

inline
int JsonFormatter_Util::valueAsJson(bdljsn::Json         *json,
                                    const char           *value,
                                    BSLA_MAYBE_UNUSED
                                    const EncoderOptions *options)
{
    BSLS_ASSERT_SAFE(json);
    BSLS_ASSERT_SAFE(value);

    if (bdlde::Utf8Util::isValid(value)) {
        *json = value;
        return  0;
    } else {
        return -1;
    }
}

template <>
inline
int JsonFormatter_Util::valueAsJson(bdljsn::Json         *json,
                                    const bsl::string&    value,
                                    BSLA_MAYBE_UNUSED
                                    const EncoderOptions *options)
{
    BSLS_ASSERT_SAFE(json);
    if (bdlde::Utf8Util::isValid(value.data(), value.length())) {
        *json = value;
        return  0;
    } else {
        return -1;
    }
}

template <>
inline
int JsonFormatter_Util::valueAsJson(bdljsn::Json         *json,
                                    const bdlt::Date&     value,
                                    const EncoderOptions *options)
{
    BSLS_ASSERT_SAFE(json);
    return setTemporalValueToJson(json, value, options);
}

template <>
inline
int JsonFormatter_Util::valueAsJson(bdljsn::Json         *json,
                                    const bdlt::Time&     value,
                                    const EncoderOptions *options)
{
    BSLS_ASSERT_SAFE(json);
    return setTemporalValueToJson(json, value, options);
}

template <>
inline
int JsonFormatter_Util::valueAsJson(bdljsn::Json          *json,
                                    const bdlt::Datetime&  value,
                                    const EncoderOptions  *options)
{
    BSLS_ASSERT_SAFE(json);
    return setTemporalValueToJson(json, value, options);
}

template <>
inline
int JsonFormatter_Util::valueAsJson(bdljsn::Json         *json,
                                    const bdlt::DateTz&   value,
                                    const EncoderOptions *options)
{
    BSLS_ASSERT_SAFE(json);
    return setTemporalValueToJson(json, value, options);
}

template <>
inline
int JsonFormatter_Util::valueAsJson(bdljsn::Json         *json,
                                    const bdlt::TimeTz&   value,
                                    const EncoderOptions *options)
{
    BSLS_ASSERT_SAFE(json);
    return setTemporalValueToJson(json, value, options);
}

template <>
inline
int JsonFormatter_Util::valueAsJson(bdljsn::Json            *json,
                                    const bdlt::DatetimeTz&  value,
                                    const EncoderOptions    *options)
{
    BSLS_ASSERT_SAFE(json);
    return setTemporalValueToJson(json, value, options);
}

template <>
inline
int JsonFormatter_Util::valueAsJson(bdljsn::Json         *json,
                                    const float&          value,
                                    const EncoderOptions *options)
{
    BSLS_ASSERT_SAFE(json);
    return setFloatingPointToJson(json, value, options);
}

template <>
inline
int JsonFormatter_Util::valueAsJson(bdljsn::Json         *json,
                                    const double&         value,
                                    const EncoderOptions *options)
{
    BSLS_ASSERT_SAFE(json);
    return setFloatingPointToJson(json, value, options);
}

                        // -------------------
                        // class JsonFormatter
                        // -------------------

// CREATORS

inline
JsonFormatter::JsonFormatter(bdljsn::Json          *json,
                             const allocator_type&  allocator)
: d_allocator(allocator)
, d_stack(bslma::AllocatorUtil::adapt(allocator))
{
    BSLS_ASSERT(json);
    BSLS_ASSERT(json->isNull());

    d_stack.push(bsl::make_pair(json, State::e_AT_START));
}

// MANIPULATORS

template <class TYPE>
int JsonFormatter::putValue(const TYPE& value, const EncoderOptions *options)
{
    BSLS_ASSERT_OPT(0 < d_stack.size());

    bdljsn::Json& top   = *d_stack.top().first;
    State::Enum   state =  d_stack.top().second;

#ifdef BDE_BUILD_TARGET_SAFE
    const int expRc = JsonFormatter_Util::expectedReturnPutValue(value,
                                                                 options);
#endif
    BSLA_MAYBE_UNUSED int rc;

    switch (state) {
      case State::e_AT_START: {
        BSLS_ASSERT_SAFE(1 == d_stack.size());
        BSLS_ASSERT_SAFE(top.isNull());
        rc = JsonFormatter_Util::valueAsJson(&top, value, options);
        BSLS_ASSERT_SAFE(expRc == rc);
        d_stack.pop();
      } break;
      case State::e_IN_ARRAY: {
        BSLS_ASSERT_SAFE(top.isArray());
        top.theArray().pushBack(bdljsn::Json(
                                    bslma::AllocatorUtil::adapt(d_allocator)));
        rc = JsonFormatter_Util::valueAsJson(&top.theArray().back(),
                                             value,
                                             options);
        BSLS_ASSERT_SAFE(expRc == rc);
      } break;
      case State::e_IN_OBJECT_EXPECTING_MEMBER_VALUE: {
        BSLS_ASSERT_SAFE(top.isNull());
        rc = JsonFormatter_Util::valueAsJson(&top, value, options);
        BSLS_ASSERT_SAFE(expRc == rc);
        d_stack.top().second = State::e_IN_OBJECT_EXPECTING_MEMBER_CLOSE;
        BSLS_ASSERT_SAFE(!top.isNull());  // `null` is set in `putNullValue`,
                                          // not here in `putValue`.
      } break;
      default : {
        BSLS_ASSERT_INVOKE_NORETURN(0); // Should not reach.
      } break;
    }

    return rc;
}

inline
void JsonFormatter::addArrayElementSeparator()
{
}

// ACCESSORS
inline
int JsonFormatter::nestingDepth() const
{
    return 0;
}

inline
JsonFormatter::allocator_type JsonFormatter::get_allocator() const
{
    return d_allocator;
}

}  // close package namespace
}  // close enterprise namespace

#endif

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
