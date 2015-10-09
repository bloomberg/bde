// baljsn_parserutil.h                                                -*-C++-*-
#ifndef INCLUDED_BALJSN_PARSERUTIL
#define INCLUDED_BALJSN_PARSERUTIL

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
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
// specified object and is overloaded for all 'bdeat' Simple types.  The
// following table describes the format in which various Simple types are
// decoded.
//..
//  Simple Type          JSON Type  Notes
//  -----------          ---------  -----
//  char                 number
//  unsigned char        number
//  int                  number
//  unsigned int         number
//  bsls::Types::Int64   number
//  bsls::Types::Uint64  number
//  float                number
//  double               number
//  char *               string
//  bsl::string          string
//  bdlt::Date            string     ISO 8601 format
//  bdlt::DateTz          string     ISO 8601 format
//  bdlt::Time            string     ISO 8601 format
//  bdlt::TimeTz          string     ISO 8601 format
//  bdlt::DatetimeTz      string     ISO 8601 format
//  bdlt::DatetimeTz      string     ISO 8601 format
//..
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
//  bslstl::StringRef nameRef(name);
//  bslstl::StringRef dateRef(date);
//  bslstl::StringRef ageRef(age);
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

#ifndef INCLUDED_BALSCM_VERSION
#include <balscm_version.h>
#endif

#ifndef INCLUDED_BDLT_ISO8601UTIL
#include <bdlt_iso8601util.h>
#endif

#ifndef INCLUDED_BSL_LIMITS
#include <bsl_limits.h>
#endif

#ifndef INCLUDED_BSL_CSTRING
#include <bsl_cstring.h>
#endif

#ifndef INCLUDED_BSL_STRING
#include <bsl_string.h>
#endif

#ifndef INCLUDED_BSL_VECTOR
#include <bsl_vector.h>
#endif

#ifndef INCLUDED_BSLS_TYPES
#include <bsls_types.h>
#endif

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
    static int getDateAndTimeValue(TYPE              *value,
                                   bslstl::StringRef  data,
                                   int                maxLength);
        // Load into the specified 'value' the date or time value represented
        // as a string in the ISO 8601 format in the specified 'data' and
        // having a maximum data length of the specified 'maxLength'.  Return 0
        // on success and a non-zero value otherwise.  Note that an error is
        // returned if 'data.length() > maxLength'.  Also note that 'TYPE' is
        // expected to be one of 'bdlt::Date', 'bdlt::Time', bdlt::Datetime',
        // 'bdlt::DateTz', 'bdlt::TimeTz', or 'bdlt::DatetimeTz'.

    template <class TYPE>
    static int getIntegralValue(TYPE *value, bslstl::StringRef data);
        // Load into the specified 'value' the integer value in the specified
        // 'data'.  Return 0 on success and a non-zero value otherwise.  Note
        // that 'TYPE' is expected to be a *signed* integral type.

    template <class TYPE>
    static int getUnsignedIntegralValue(TYPE *value, bslstl::StringRef data);
        // Load into the specified 'value' the unsigned integer value in the
        // specified 'data'.  Return 0 on success and a non-zero value
        // otherwise.  Note that 'TYPE' is expected to be a *unsigned* integral
        // type.

    static int getUint64(bsls::Types::Uint64 *value, bslstl::StringRef data);
        // Load into the specified 'value' the value in the specified 'data'.
        // Return 0 on success and a non-zero value otherwise.

    static int getString(bsl::string *value, bslstl::StringRef data);
        // Load into the specified 'value' the string value in the specified
        // 'data'.  Return 0 on success and a non-zero value otherwise.

  public:
    // CLASS METHODS
    static int getValue(bool                *value, bslstl::StringRef data);
    static int getValue(char                *value, bslstl::StringRef data);
    static int getValue(unsigned char       *value, bslstl::StringRef data);
    static int getValue(signed char         *value, bslstl::StringRef data);
    static int getValue(short               *value, bslstl::StringRef data);
    static int getValue(unsigned short      *value, bslstl::StringRef data);
    static int getValue(int                 *value, bslstl::StringRef data);
    static int getValue(unsigned int        *value, bslstl::StringRef data);
    static int getValue(bsls::Types::Int64  *value, bslstl::StringRef data);
    static int getValue(bsls::Types::Uint64 *value, bslstl::StringRef data);
    static int getValue(float               *value, bslstl::StringRef data);
    static int getValue(double              *value, bslstl::StringRef data);
    static int getValue(bsl::string         *value, bslstl::StringRef data);
    static int getValue(bdlt::Date           *value, bslstl::StringRef data);
    static int getValue(bdlt::Datetime       *value, bslstl::StringRef data);
    static int getValue(bdlt::DatetimeTz     *value, bslstl::StringRef data);
    static int getValue(bdlt::DateTz         *value, bslstl::StringRef data);
    static int getValue(bdlt::Time           *value, bslstl::StringRef data);
    static int getValue(bdlt::TimeTz         *value, bslstl::StringRef data);
    static int getValue(bsl::vector<char>   *value, bslstl::StringRef data);
        // Load into the specified 'value' the characters read from the
        // specified 'data'.  Return 0 on success or a non-zero value on
        // failure.
};

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

                             // -----------------
                             // struct ParserUtil
                             // -----------------

// CLASS METHODS
template <class TYPE>
int ParserUtil::getUnsignedIntegralValue(TYPE *value, bslstl::StringRef data)
{
    if (0 == data.length()) {
        return -1;                                                    // RETURN
    }

    bsls::Types::Uint64 tmp;
    int rc = getUint64(&tmp, data);
    if (rc) {
        return -1;                                                    // RETURN
    }

    if (tmp >
          static_cast<bsls::Types::Uint64>(bsl::numeric_limits<TYPE>::max())) {
        return -1;                                                    // RETURN
    }

    *value = static_cast<TYPE>(tmp);
    return 0;
}

template <class TYPE>
int ParserUtil::getIntegralValue(TYPE *value, bslstl::StringRef data)
{
    if (0 == data.length()) {
        return -1;                                                    // RETURN
    }

    bool isNegative;
    if ('-' == data[0]) {
        isNegative = true;
        data.assign(data.begin() + 1, data.end());
    }
    else {
        isNegative = false;
    }

    bsls::Types::Uint64 tmp;
    const int rc = getUint64(&tmp, data);
    if (rc) {
        return -1;                                                    // RETURN
    }

    bsls::Types::Uint64 maxValue = static_cast<bsls::Types::Uint64>(
                                             bsl::numeric_limits<TYPE>::max());

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
int ParserUtil::getDateAndTimeValue(TYPE              *value,
                                    bslstl::StringRef  data,
                                    int                maxLength)
{
    enum { k_STRING_LENGTH_WITH_QUOTES = 2 };

    if (data.length()  < k_STRING_LENGTH_WITH_QUOTES
     || '"'           != *data.begin()
     || '"'           != *(data.end() - 1)
     || data.length()  > static_cast<unsigned int>(maxLength)
                                          + k_STRING_LENGTH_WITH_QUOTES) {
        return -1;                                                    // RETURN
    }

    return bdlt::Iso8601Util::parse(
           value,
           data.data() + 1,
           static_cast<int>(data.length() - k_STRING_LENGTH_WITH_QUOTES));
}

inline
int ParserUtil::getValue(char *value, bslstl::StringRef data)
{
    signed char tmp;  // Note that 'char' is unsigned on IBM.

    const int rc = getIntegralValue(&tmp, data);
    if (!rc) {
        *value = tmp;
    }
    return rc;
}

inline
int ParserUtil::getValue(unsigned char *value, bslstl::StringRef data)
{
    return getUnsignedIntegralValue(value, data);
}

inline
int ParserUtil::getValue(signed char *value, bslstl::StringRef data)
{
    return getValue((char *) value, data);
}

inline
int ParserUtil::getValue(short *value, bslstl::StringRef data)
{
    return getIntegralValue(value, data);
}

inline
int ParserUtil::getValue(unsigned short *value, bslstl::StringRef data)
{
    return getUnsignedIntegralValue(value, data);
}

inline
int ParserUtil::getValue(int *value, bslstl::StringRef data)
{
    return getIntegralValue(value, data);
}

inline
int ParserUtil::getValue(unsigned int *value, bslstl::StringRef data)
{
    return getUnsignedIntegralValue(value, data);
}

inline
int ParserUtil::getValue(bsls::Types::Int64 *value, bslstl::StringRef data)
{
    return getIntegralValue(value, data);
}

inline
int ParserUtil::getValue(bsls::Types::Uint64 *value, bslstl::StringRef data)
{
    return getUnsignedIntegralValue(value, data);
}

inline
int ParserUtil::getValue(float *value, bslstl::StringRef data)
{
    double tmp;
    const int rc = getValue(&tmp, data);
    if (!rc) {
        *value = static_cast<float>(tmp);
    }
    return rc;
}

inline
int ParserUtil::getValue(bsl::string *value, bslstl::StringRef data)
{
    return getString(value, data);
}

inline
int ParserUtil::getValue(bdlt::Date *value, bslstl::StringRef data)
{
    return getDateAndTimeValue(value, data, bdlt::Iso8601Util::k_DATE_STRLEN);
}

inline
int ParserUtil::getValue(bdlt::Datetime *value, bslstl::StringRef data)
{
    return getDateAndTimeValue(value,
                               data,
                               bdlt::Iso8601Util::k_DATETIME_STRLEN);
}

inline
int ParserUtil::getValue(bdlt::DatetimeTz *value, bslstl::StringRef data)
{
    return getDateAndTimeValue(value,
                               data,
                               bdlt::Iso8601Util::k_DATETIMETZ_STRLEN);
}

inline
int ParserUtil::getValue(bdlt::DateTz *value, bslstl::StringRef data)
{
    return getDateAndTimeValue(value,
                               data,
                               bdlt::Iso8601Util::k_DATETZ_STRLEN);
}

inline
int ParserUtil::getValue(bdlt::Time *value, bslstl::StringRef data)
{
    return getDateAndTimeValue(value, data, bdlt::Iso8601Util::k_TIME_STRLEN);
}

inline
int ParserUtil::getValue(bdlt::TimeTz *value, bslstl::StringRef data)
{
    return getDateAndTimeValue(value,
                               data,
                               bdlt::Iso8601Util::k_TIMETZ_STRLEN);
}
}  // close package namespace

}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2015 Bloomberg Finance L.P.
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
