// baljsn_parserutil.h                                                -*-C++-*-
#ifndef INCLUDED_BALJSN_PARSERUTIL
#define INCLUDED_BALJSN_PARSERUTIL

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a utility for decoding JSON data into simple types.
//
//@CLASSES:
//  baljsn::ParserUtil: utility for parsing JSON data into simple types
//
//@SEE_ALSO: baljsn_decoder, baljsn_printutil
//
//@DESCRIPTION: This component provides a 'struct' of utility functions,
// 'baljsn::ParserUtil', for decoding data in the JSON format into a 'bdeat'
// Simple type.  The primary method is 'getValue', which decodes into a
// specified object and is overloaded for all 'bdeat' Simple types.
//
// Refer to the details of the JSON encoding format supported by this utility
// in the package documentation file (doc/baljsn.txt).
//
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
//  struct Employee {
//      bsl::string d_name;
//      bdlt::Date   d_date;
//      int         d_age;
//  };
//..
// Then, we create an 'Employee' object:
//..
//  Employee employee;
//..
// Next, we specify the string values in JSON format used to represent the
// object data.  Note that the birth date is specified in the ISO 8601 format:
//..
//  const char *name = "\"John Smith\"";
//  const char *date = "\"1985-06-24\"";
//  const char *age  = "21";
//
//  const bsl::string_view nameRef(name);
//  const bsl::string_view dateRef(date);
//  const bsl::string_view ageRef(age);
//..
// Now, we use the created string refs to populate the employee object:
//..
//  assert(0 == baljsn::ParserUtil::getValue(&employee.d_name, nameRef));
//  assert(0 == baljsn::ParserUtil::getValue(&employee.d_date, dateRef));
//  assert(0 == baljsn::ParserUtil::getValue(&employee.d_age, ageRef));
//..
// Finally, we will verify that the values are as expected:
//..
//  assert("John Smith"            == employee.d_name);
//  assert(bdlt::Date(1985, 06, 24) == employee.d_date);
//  assert(21                      == employee.d_age);
//..

#include <balscm_version.h>

#include <bdljsn_stringutil.h>

#include <bdlb_variant.h>

#include <bdldfp_decimal.h>

#include <bdlt_iso8601util.h>

#include <bsls_assert.h>
#include <bsls_types.h>

#include <bsl_limits.h>
#include <bsl_cstring.h>
#include <bsl_string.h>
#include <bsl_string_view.h>
#include <bsl_vector.h>

#include <string>
#include <vector>

namespace BloombergLP {
namespace baljsn {

                            // =================
                            // struct ParserUtil
                            // =================

struct ParserUtil {
    //This class provides utility functions for decoding data in the JSON
    //format into a 'bdeat' Simple type.  The primary method is 'getValue',
    //which decodes into a specified object and is overloaded for all 'bdeat'
    //Simple types.

  private:
    // PRIVATE CLASS METHODS
    template <class TYPE>
    static int getDateAndTimeValue(TYPE                    *value,
                                   const bsl::string_view&  data);
        // Load into the specified 'value' the date or time value represented
        // as a string in the ISO 8601 format in the specified 'data'.  Return
        // 0 on success and a non-zero value otherwise.  Note that 'TYPE' is
        // expected to be one of 'bdlt::Date', 'bdlt::Time', bdlt::Datetime',
        // 'bdlt::DateTz', 'bdlt::TimeTz', 'bdlt::DatetimeTz',
        // 'bdlb::Variant2<bdlt::Date, bdlt::DateTz>',
        // 'bdlb::Variant2<bdlt::Time, bdlt::TimeTz>' or
        // 'bdlb::Variant2<bdlt::Datetime, bdlt::DatetimeTz>'.

    template <class TYPE>
    static int getIntegralValue(TYPE *value, bsl::string_view data);
        // Load into the specified 'value' the integer value in the specified
        // 'data'.  Note that this operation follows the more permissive syntax
        // specified for 'bdlb::NumericParseUtil', allowing things like leading
        // '0' digits which the JSON spec does not permit.  Return 0 on success
        // and a non-zero value otherwise.  The behavior is undefined unless
        // 'true == is_integral<TYPE>::value && is_signed<TYPE>::value', e.g,
        // 'TYPE' is expected to be a *signed* integral type.  Note that
        // although 'data' is passed by value instead of 'const'-reference,
        // there is no effect on usage.

    template <class TYPE>
    static int getUnsignedIntegralValue(TYPE                    *value,
                                        const bsl::string_view&  data);
        // Load into the specified 'value' the unsigned integer value in the
        // specified 'data'.  Note that this operation follows the more
        // permissive syntax specified for 'bdlb::NumericParseUtil', allowing
        // things like leading '0' digits which the JSON spec does not permit.
        // Return 0 on success and a non-zero value otherwise.  The behavior is
        // undefined unless
        // 'true == is_integral<TYPE>::value && !is_signed<TYPE>::value', e.g,
        // 'TYPE' is expected to be an *unsigned* integral type.

    static int getUint64(bsls::Types::Uint64     *value,
                         const bsl::string_view&  data);
        // Load into the specified 'value' the value in the specified 'data'.
        // Note that this operation follows the more permissive syntax
        // specified for 'bdlb::NumericParseUtil', allowing things like leading
        // '0' digits which the JSON spec does not permit.  Return 0 on success
        // and a non-zero value otherwise.

  public:
    // TYPES
    typedef bdlb::Variant2<bdlt::Date, bdlt::DateTz>      DateOrDateTz;
        // 'DateOrDateTz' is a convenient alias for
        // 'bdlb::Variant2<Date, DateTz>'.

    typedef bdlb::Variant2<bdlt::Time, bdlt::TimeTz>      TimeOrTimeTz;
        // 'TimeOrTimeTz' is a convenient alias for
        // 'bdlb::Variant2<Time, TimeTz>'.

    typedef bdlb::Variant2<bdlt::Datetime, bdlt::DatetimeTz>
                                                          DatetimeOrDatetimeTz;
        // 'DatetimeOrDatetimeTz' is a convenient alias for
        // 'bdlb::Variant2<Datetime, DatetimeTz>'.

    // CLASS METHODS
    static int getQuotedString(bsl::string             *value,
                               const bsl::string_view&  data);
        // Load into the specified 'value' the string value in the specified
        // 'data'.  The string must be begin and end in '"' characters which
        // are not part of the resulting 'value'.  Return 0 on success and a
        // non-zero value otherwise.

    static int getUnquotedString(bsl::string             *value,
                               const bsl::string_view&  data);
        // Load into the specified 'value' the string value in the specified
        // 'data'.  Return 0 on success and a non-zero value otherwise.

    static int getValue(bool                    *value,
                        const bsl::string_view&  data);
    static int getValue(char                    *value,
                        const bsl::string_view&  data);
    static int getValue(unsigned char           *value,
                        const bsl::string_view&  data);
    static int getValue(signed char             *value,
                        const bsl::string_view&  data);
    static int getValue(short                   *value,
                        const bsl::string_view&  data);
    static int getValue(unsigned short          *value,
                        const bsl::string_view&  data);
    static int getValue(int                     *value,
                        const bsl::string_view&  data);
    static int getValue(unsigned int            *value,
                        const bsl::string_view&  data);
    static int getValue(bsls::Types::Int64      *value,
                        const bsl::string_view&  data);
    static int getValue(bsls::Types::Uint64     *value,
                        const bsl::string_view&  data);
    static int getValue(float                   *value,
                        const bsl::string_view&  data);
    static int getValue(double                  *value,
                        const bsl::string_view&  data);
    static int getValue(bdldfp::Decimal64       *value,
                        const bsl::string_view&  data);
    static int getValue(bdlt::Date              *value,
                        const bsl::string_view&  data);
    static int getValue(bdlt::Datetime          *value,
                        const bsl::string_view&  data);
    static int getValue(bdlt::DatetimeTz        *value,
                        const bsl::string_view&  data);
    static int getValue(bdlt::DateTz            *value,
                        const bsl::string_view&  data);
    static int getValue(bdlt::Time              *value,
                        const bsl::string_view&  data);
    static int getValue(bdlt::TimeTz            *value,
                        const bsl::string_view&  data);
    static int getValue(DateOrDateTz            *value,
                        const bsl::string_view&  data);
    static int getValue(TimeOrTimeTz            *value,
                        const bsl::string_view&  data);
    static int getValue(DatetimeOrDatetimeTz    *value,
                        const bsl::string_view&  data);
    static int getValue(bsl::vector<char>       *value,
                        const bsl::string_view&  data);
        // Load into the specified 'value' the characters read from the
        // specified 'data'.  Return 0 on success or a non-zero value on
        // failure.

    static int getValue(bsl::string             *value,
                        const bsl::string_view&  data);
        // Load into the specified 'value' the string value in the specified
        // 'data'.  The string must be begin and end in '"' characters which
        // are not part of the resulting 'value'.  Return 0 on success and a
        // non-zero value otherwise.
};

// ============================================================================
//                             INLINE DEFINITIONS
// ============================================================================

                            // -----------------
                            // struct ParserUtil
                            // -----------------

// CLASS METHODS
inline
int ParserUtil::getQuotedString(bsl::string             *value,
                                const bsl::string_view&  data)
{
    return bdljsn::StringUtil::readString(
                          value,
                          data,
                          bdljsn::StringUtil::e_ACCEPT_CAPITAL_UNICODE_ESCAPE);
}

template <class TYPE>
int ParserUtil::getUnsignedIntegralValue(TYPE                    *value,
                                         const bsl::string_view&  data)
{
    BSLS_ASSERT(value);

    if (0 == data.length()) {
        return -1;                                                    // RETURN
    }

    bsls::Types::Uint64 tmp;

    int rc = getUint64(&tmp, data);
    if (rc) {
        return -1;                                                    // RETURN
    }

    if (tmp >
        static_cast<bsls::Types::Uint64>((bsl::numeric_limits<TYPE>::max)())) {
        return -1;                                                    // RETURN
    }

    *value = static_cast<TYPE>(tmp);
    return 0;
}

template <class TYPE>
int ParserUtil::getIntegralValue(TYPE *value, bsl::string_view data)
{
    // Note that we take 'data' by value because we may want to modify it.

    if (0 == data.length()) {
        return -1;                                                    // RETURN
    }

    bool isNegative;
    if ('-' == data[0]) {
        isNegative = true;
        data.remove_prefix(1);
    }
    else {
        isNegative = false;
    }

    bsls::Types::Uint64 tmp;

    const int rc = getUint64(&tmp, data);
    if (rc) {
        return -1;                                                    // RETURN
    }

    bsls::Types::Uint64 maxValue =
        static_cast<bsls::Types::Uint64>((bsl::numeric_limits<TYPE>::max)());

    if (isNegative && tmp <= maxValue + 1) {
        *value = static_cast<TYPE>(tmp * -1);
    }
    else if (tmp <= maxValue) {
        *value = static_cast<TYPE>(tmp);
    }
    else {
        return -1;                                                    // RETURN
    }

    return 0;
}

template <class TYPE>
int ParserUtil::getDateAndTimeValue(TYPE                    *value,
                                    const bsl::string_view&  data)
{
    enum { k_STRING_LENGTH_WITH_QUOTES = 2 };

    if (data.length()  < k_STRING_LENGTH_WITH_QUOTES
     || '"'           != *data.begin()
     || '"'           != *(data.end() - 1)) {
        return -1;                                                    // RETURN
    }

    return bdlt::Iso8601Util::parse(
           value,
           data.data() + 1,
           static_cast<int>(data.length() - k_STRING_LENGTH_WITH_QUOTES));
}

inline
int ParserUtil::getValue(char *value, const bsl::string_view& data)
{
    signed char tmp;  // Note that 'char' is unsigned on IBM.
    const int   rc = getIntegralValue(&tmp, data);
    if (!rc) {
        *value = tmp;
    }
    return rc;
}

inline
int ParserUtil::getValue(unsigned char *value, const bsl::string_view& data)
{
    return getUnsignedIntegralValue(value, data);
}

inline
int ParserUtil::getValue(signed char *value, const bsl::string_view& data)
{
    return getValue((char *) value, data);
}

inline
int ParserUtil::getValue(short *value, const bsl::string_view& data)
{
    return getIntegralValue(value, data);
}

inline
int ParserUtil::getValue(unsigned short *value, const bsl::string_view& data)
{
    return getUnsignedIntegralValue(value, data);
}

inline
int ParserUtil::getValue(int *value, const bsl::string_view& data)
{
    return getIntegralValue(value, data);
}

inline
int ParserUtil::getValue(unsigned int *value, const bsl::string_view& data)
{
    return getUnsignedIntegralValue(value, data);
}

inline
int ParserUtil::getValue(bsls::Types::Int64      *value,
                         const bsl::string_view&  data)
{
    return getIntegralValue(value, data);
}

inline
int ParserUtil::getValue(bsls::Types::Uint64     *value,
                         const bsl::string_view&  data)
{
    return getUnsignedIntegralValue(value, data);
}

inline
int ParserUtil::getValue(float *value, const bsl::string_view& data)
{
    double    tmp;
    const int rc = getValue(&tmp, data);
    if (!rc) {
        *value = static_cast<float>(tmp);
    }
    return rc;
}

inline
int ParserUtil::getValue(bsl::string *value, const bsl::string_view& data)
{
    return bdljsn::StringUtil::readString(
                          value,
                          data,
                          bdljsn::StringUtil::e_ACCEPT_CAPITAL_UNICODE_ESCAPE);
}

inline
int ParserUtil::getValue(bdlt::Date *value, const bsl::string_view& data)
{
    return getDateAndTimeValue(value, data);
}

inline
int ParserUtil::getValue(bdlt::Datetime *value, const bsl::string_view& data)
{
    return getDateAndTimeValue(value, data);
}

inline
int ParserUtil::getValue(bdlt::DatetimeTz *value, const bsl::string_view& data)
{
    return getDateAndTimeValue(value, data);
}

inline
int ParserUtil::getValue(bdlt::DateTz *value, const bsl::string_view& data)
{
    return getDateAndTimeValue(value, data);
}

inline
int ParserUtil::getValue(bdlt::Time *value, const bsl::string_view& data)
{
    return getDateAndTimeValue(value, data);
}

inline
int ParserUtil::getValue(bdlt::TimeTz *value, const bsl::string_view& data)
{
    return getDateAndTimeValue(value, data);
}

inline
int ParserUtil::getValue(DateOrDateTz *value, const bsl::string_view& data)
{
    return getDateAndTimeValue(value, data);
}

inline
int ParserUtil::getValue(TimeOrTimeTz *value, const bsl::string_view& data)
{
    return getDateAndTimeValue(value, data);
}

inline
int ParserUtil::getValue(DatetimeOrDatetimeTz    *value,
                         const bsl::string_view&  data)
{
    return getDateAndTimeValue(value, data);
}
}  // close package namespace

}  // close enterprise namespace

#endif

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
