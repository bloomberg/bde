// baljsn_jsonparserutil.h                                            -*-C++-*-
#ifndef INCLUDED_BALJSN_JSONPARSERUTIL
#define INCLUDED_BALJSN_JSONPARSERUTIL

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a utility to get simple types from `bdl::json` objects
//
//@CLASSES:
//  baljsn::JsonParserUtil: utility to get simple types from `bdljsn::Json`s
//
//@SEE_ALSO: baljsn_parserutil
//
//@DESCRIPTION: This component provides a `struct` of utility functions,
// `baljsn::JsonParserUtil`, for extracting data from `bdljsn::Json` objects
// format into a `bdlat` Simple type.  The primary method is `getValue` which
// is is overloaded for all `bdlat` Simple types.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Decoding into a Simple `struct` from `bdljsn::Json` Objects
///----------------------------------------------------------------------
// Suppose we want extract some data from `bdljsn::Json` objects.
//
// First, we define a `struct`, `Employee`, to contain the data:
// ```
//  struct Employee {
//      bsl::string d_name;
//      bdlt::Date  d_date;
//      int         d_age;
//  };
// ```
// Then, we create an `Employee` object:
// ```
//  Employee employee;
// ```
// Next, we create `bdljsn::Json` objects of different (dynameic) types
// having the data of interest.  Note that the string data does *not* have
// embedden double quote deliminters as required in JSON documents.  Also note
// that the date information is represented in ISO 8601 format in a string.
// ```
//  const char *nameStr = "John Smith";  // No double quotes *in* string.
//  const char *dateStr = "1985-06-24";  // No double quotes *in* string.
//
//  bdljsn::Json name; name.makeString(nameStr);
//  bdljsn::Json date; date.makeString(dateStr); // ISO 8601
//  bdljsn::Json age;  age .makeNumber(bdljsn::JsonNumber(21));
// ```
// Now, we use `bdljsn::Json` objects to populate the `employee` object:
// ```
//  assert(0 == baljsn::JsonParserUtil::getValue(&employee.d_name, name));
//  assert(0 == baljsn::JsonParserUtil::getValue(&employee.d_date, date));
//  assert(0 == baljsn::JsonParserUtil::getValue(&employee.d_age,  age ));
// ```
// Finally, we will verify that the values are as expected:
// ```
//  assert("John Smith"             == employee.d_name);
//  assert(bdlt::Date(1985, 06, 24) == employee.d_date);
//  assert(21                       == employee.d_age);
// ```

#include <balscm_version.h>

#include <baljsn_parserutil.h>

#include <bdljsn_json.h>

#include <bdldfp_decimal.h>

#include <bdlt_date.h>
#include <bdlt_datetime.h>
#include <bdlt_datetimetz.h>
#include <bdlt_datetz.h>
#include <bdlt_time.h>
#include <bdlt_timetz.h>

#include <bsla_unreachable.h>

#include <bsls_assert.h>
#include <bsls_libraryfeatures.h>
#include <bsls_types.h>

#include <bsl_limits.h>
#include <bsl_cstring.h>  // `bsl::strncmp`
#include <bsl_string.h>
#include <bsl_string_view.h>
#include <bsl_vector.h>

#include <string>

namespace BloombergLP {
namespace baljsn {

                            // =====================
                            // struct JsonParserUtil
                            // =====================

///This class provides utility functions for decoding data in the JSON
///format into a `bdeat` Simple type.  The primary method is `getValue`,
///which decodes into a specified object and is overloaded for all `bdeat`
///Simple types.
struct JsonParserUtil {

  public:

    /// Load into the specified `value` the characters read from the
    /// specified `data`.  Return 0 on success or a non-zero value on
    /// failure.  The behavior is undefined unless 'json.isBoolean())'
    static int getValue(bool                 *value,
                        const bdljsn::Json&   json);

    /// Load into the specified `value` the characters read from the
    /// specified `data`.  Return 0 on success or a non-zero value on
    /// failure.  The behavior is undefined unless 'json.isNumber())'
    static int getValue(char                 *value,
                        const bdljsn::Json&   json);
    static int getValue(unsigned char        *value,
                        const bdljsn::Json&   json);
    static int getValue(signed char          *value,
                        const bdljsn::Json&   json);
    static int getValue(short                *value,
                        const bdljsn::Json&   json);
    static int getValue(unsigned short       *value,
                        const bdljsn::Json&   json);
    static int getValue(int                  *value,
                        const bdljsn::Json&   json);
    static int getValue(unsigned int         *value,
                        const bdljsn::Json&   json);
    static int getValue(bsls::Types::Int64   *value,
                        const bdljsn::Json&   json);
    static int getValue(bsls::Types::Uint64  *value,
                        const bdljsn::Json&   json);

    /// Load into the specified `value` the characters read from the
    /// specified `data`.  Return 0 on success or a non-zero value on
    /// failure.  The behavior is undefined unless 'json.isNumber())'
    /// or 'json.isString()' if the value is INF, -INF, or NaN.
    static int getValue(float                *value,
                        const bdljsn::Json&   json);
    static int getValue(double               *value,
                        const bdljsn::Json&   json);
    static int getValue(bdldfp::Decimal64    *value,
                        const bdljsn::Json&   json);

    /// Load into the specified `value` the characters read from the
    /// specified `data`.  Return 0 on success or a non-zero value on
    /// failure.  The behavior is undefined unless 'json.isString())'
    static int getValue(bsl::string          *value,
                        const bdljsn::Json&   json);
    static int getValue(std::string          *value,
                        const bdljsn::Json&   json);
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR_STRING
    static int getValue(std::pmr::string     *value,
                        const bdljsn::Json&   json);
#endif
    static int getValue(bdlt::Date           *value,
                        const bdljsn::Json&   json);
    static int getValue(bdlt::Datetime       *value,
                        const bdljsn::Json&   json);
    static int getValue(bdlt::DatetimeTz     *value,
                        const bdljsn::Json&   json);
    static int getValue(bdlt::DateTz         *value,
                        const bdljsn::Json&   json);
    static int getValue(bdlt::Time           *value,
                        const bdljsn::Json&   json);
    static int getValue(bdlt::TimeTz         *value,
                        const bdljsn::Json&   json);

    static int getValue(ParserUtil::DateOrDateTz         *value,
                        const bdljsn::Json&               json);
    static int getValue(ParserUtil::TimeOrTimeTz         *value,
                        const bdljsn::Json&               json);
    static int getValue(ParserUtil::DatetimeOrDatetimeTz *value,
                        const bdljsn::Json&               json);

    static int getValue(bsl::vector<char>    *value,
                        const bdljsn::Json&   data);
};

// ============================================================================
//                             INLINE DEFINITIONS
// ============================================================================

                            // ---------------------
                            // struct JsonParserUtil
                            // ---------------------

inline
int JsonParserUtil::getValue(bool *value, const bdljsn::Json& json)
{
    BSLS_ASSERT(value);
    BSLS_ASSERT(json.isBoolean());

    *value = json.theBoolean();

    return 0;
}

inline
int JsonParserUtil::getValue(char *value, const bdljsn::Json& json)
{
    BSLS_ASSERT(value);
    BSLS_ASSERT(json.isNumber());

    bsl::string_view data = json.theNumber().value();

    return ParserUtil::getValue(value, data);
}

inline
int JsonParserUtil::getValue(unsigned char *value, const bdljsn::Json& json)
{
    BSLS_ASSERT(value);
    BSLS_ASSERT(json.isNumber());

    bsl::string_view data = json.theNumber().value();

    return ParserUtil::getValue(value, data);
}

inline
int JsonParserUtil::getValue(signed char *value, const bdljsn::Json& json)
{
    BSLS_ASSERT(value);
    BSLS_ASSERT(json.isNumber());

    bsl::string_view data = json.theNumber().value();

    return ParserUtil::getValue(value, data);
}

inline
int JsonParserUtil::getValue(short *value, const bdljsn::Json& json)
{
    BSLS_ASSERT(value);
    BSLS_ASSERT(json.isNumber());

    bsl::string_view data = json.theNumber().value();

    return ParserUtil::getValue(value, data);
}

inline
int JsonParserUtil::getValue(unsigned short *value, const bdljsn::Json& json)
{
    BSLS_ASSERT(value);
    BSLS_ASSERT(json.isNumber());

    bsl::string_view data = json.theNumber().value();

    return ParserUtil::getValue(value, data);
}

inline
int JsonParserUtil::getValue(int *value, const bdljsn::Json& json)
{
    BSLS_ASSERT(value);
    BSLS_ASSERT(json.isNumber());

    bsl::string_view data = json.theNumber().value();

    return ParserUtil::getValue(value, data);
}

inline
int JsonParserUtil::getValue(unsigned int *value, const bdljsn::Json& json)
{
    BSLS_ASSERT(value);
    BSLS_ASSERT(json.isNumber());

    bsl::string_view data = json.theNumber().value();

    return ParserUtil::getValue(value, data);
}

inline
int JsonParserUtil::getValue(bsls::Types::Int64  *value,
                             const bdljsn::Json&  json)
{
    BSLS_ASSERT(value);
    BSLS_ASSERT(json.isNumber());

    bsl::string_view data = json.theNumber().value();

    return ParserUtil::getValue(value, data);
}

inline
int JsonParserUtil::getValue(bsls::Types::Uint64 *value,
                             const bdljsn::Json&  json)
{
    BSLS_ASSERT(value);
    BSLS_ASSERT(json.isNumber());

    bsl::string_view data = json.theNumber().value();

    return ParserUtil::getValue(value, data);

}

inline
int JsonParserUtil::getValue(float *value, const bdljsn::Json& json)
{
    BSLS_ASSERT(value);
    BSLS_ASSERT(json.isNumber() || json.isString());

    double    tmp;
    const int rc = getValue(&tmp, json);
    if (!rc) {
        *value = static_cast<float>(tmp);
    }
    return rc;
}

inline
int JsonParserUtil::getValue(double *value, const bdljsn::Json& json)
{
    BSLS_ASSERT(value);
    BSLS_ASSERT(json.isNumber() || json.isString());

           if (json.isNumber()) {
                bsl::string_view data = json.theNumber().value();
                return ParserUtil::getValue(value, data);             // RETURN
    } else if (json.isString()) {
               if (0 == bsl::strncmp("+inf",
                                     json.theString().data(),
                                     json.theString().size())) {
                    *value =  bsl::numeric_limits<double>::infinity();
                    return  0;                                        // RETURN
        } else if (0 == bsl::strncmp("-inf",
                                     json.theString().data(),
                                     json.theString().size())) {
                    *value = -bsl::numeric_limits<double>::infinity();
                    return  0;                                        // RETURN
        } else if (0 == bsl::strncmp("nan",
                                     json.theString().data(),
                                     json.theString().size())) {
                    *value = bsl::numeric_limits<double>::quiet_NaN();
                    return  0;                                        // RETURN
        } else {
                    return -1;                                        // RETURN
        }
    }

    BSLA_UNREACHABLE;
}

inline
int JsonParserUtil::getValue(bdldfp::Decimal64   *value,
                             const bdljsn::Json&  json)
{
    BSLS_ASSERT(value);
    BSLS_ASSERT(json.isNumber() || json.isString());

    bsl::string_view data = json. isNumber()
                          ? json.theNumber().value()
                          : json.theString();

    return ParserUtil::getValue(value, data);
}

inline
int JsonParserUtil::getValue(bsl::string *value, const bdljsn::Json& json)
{
    BSLS_ASSERT(value);
    BSLS_ASSERT(json.isString());

    value->assign(json.theString());  // Note: The 'Json' string already had
                                      // delimiting double quotes stripped
                                      // and special character sequences
                                      // converted.
    return 0;
}

inline
int JsonParserUtil::getValue(std::string *value, const bdljsn::Json& json)
{
    BSLS_ASSERT(value);
    BSLS_ASSERT(json.isString());

    value->assign(json.theString());

    return 0;
}

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR_STRING
inline
int JsonParserUtil::getValue(std::pmr::string *value, const bdljsn::Json& json)
{
    BSLS_ASSERT(value);
    BSLS_ASSERT(json.isString());

    value->assign(json.theString());

    return 0;
}
#endif

inline
int JsonParserUtil::getValue(bdlt::Date *value, const bdljsn::Json& json)
{
    BSLS_ASSERT(value);
    BSLS_ASSERT(json.isString());

    bsl::string_view data = json.theString();

    return  bdlt::Iso8601Util::parse(value,
                                     data.data(),
                    static_cast<int>(data.length()));
}

inline
int JsonParserUtil::getValue(bdlt::Datetime *value, const bdljsn::Json& json)
{
    BSLS_ASSERT(value);
    BSLS_ASSERT(json.isString());

    bsl::string_view data = json.theString();

    return  bdlt::Iso8601Util::parse(value,
                                     data.data(),
                    static_cast<int>(data.length()));
}

inline
int JsonParserUtil::getValue(bdlt::DatetimeTz *value, const bdljsn::Json& json)
{
    BSLS_ASSERT(value);
    BSLS_ASSERT(json.isString());

    bsl::string_view data = json.theString();

    int rc = bdlt::Iso8601Util::parse(value,
                                      data.data(),
                     static_cast<int>(data.length()));
    return rc;
}

inline
int JsonParserUtil::getValue(bdlt::DateTz *value, const bdljsn::Json& json)
{
    BSLS_ASSERT(value);
    BSLS_ASSERT(json.isString());

    bsl::string_view data = json.theString();

    return  bdlt::Iso8601Util::parse(value,
                                     data.data(),
                    static_cast<int>(data.length()));
}

inline
int JsonParserUtil::getValue(bdlt::Time *value, const bdljsn::Json& json)
{
    BSLS_ASSERT(value);
    BSLS_ASSERT(json.isString());

    bsl::string_view data = json.theString();

    return  bdlt::Iso8601Util::parse(value,
                                     data.data(),
                    static_cast<int>(data.length()));
}

inline
int JsonParserUtil::getValue(bdlt::TimeTz *value, const bdljsn::Json& json)
{
    BSLS_ASSERT(value);
    BSLS_ASSERT(json.isString());

    bsl::string_view data = json.theString();

    return  bdlt::Iso8601Util::parse(value,
                                     data.data(),
                    static_cast<int>(data.length()));
}

inline
int JsonParserUtil::getValue(ParserUtil::DateOrDateTz *value,
                             const bdljsn::Json&       json)
{
    BSLS_ASSERT(value);
    BSLS_ASSERT(json.isString());

    bsl::string_view data = json.theString();

    return  bdlt::Iso8601Util::parse(value,
                                     data.data(),
                    static_cast<int>(data.length()));
}

inline
int JsonParserUtil::getValue(ParserUtil::TimeOrTimeTz *value,
                             const bdljsn::Json&       json)
{
    BSLS_ASSERT(value);
    BSLS_ASSERT(json.isString());

    bsl::string_view data = json.theString();

    return  bdlt::Iso8601Util::parse(value,
                                     data.data(),
                    static_cast<int>(data.length()));
}

inline
int JsonParserUtil::getValue(ParserUtil::DatetimeOrDatetimeTz *value,
                             const bdljsn::Json&               json)
{
    BSLS_ASSERT(value);
    BSLS_ASSERT(json.isString());

    bsl::string_view data = json.theString();

    return  bdlt::Iso8601Util::parse(value,
                                     data.data(),
                    static_cast<int>(data.length()));
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
