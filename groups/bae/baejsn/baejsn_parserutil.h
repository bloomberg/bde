// baejsn_parserutil.h                                                -*-C++-*-
#ifndef INCLUDED_BAEJSN_PARSERUTIL
#define INCLUDED_BAEJSN_PARSERUTIL

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a utility for decoding JSON data into simple types.
//
//@CLASSES:
//  baejsn_ParserUtil: utility for parsing JSON data into simple types
//
//@SEE_ALSO: baejsn_decoder, baejsn_printutil
//
//@AUTHOR: Raymond Chiu (schiu49), Rohan Bhindwale (rbhindwa)
//
//@DESCRIPTION: This component provides utility functions for decoding data in
// the JSON format into a 'bdeat' Simple type.  The primary method is
// 'getValue', which decodes into a specified object and is overloaded for all
// 'bdeat' Simple types.  The following table describes the format in which
// various Simple types are decoded.
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
//  bdet_Date            string     ISO 8601 format
//  bdet_DateTz          string     ISO 8601 format
//  bdet_Time            string     ISO 8601 format
//  bdet_TimeTz          string     ISO 8601 format
//  bdet_DatetimeTz      string     ISO 8601 format
//  bdet_DatetimeTz      string     ISO 8601 format
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Decoding into a Simple 'struct' from JSON data
///---------------------------------------------------------
// Suppose we want to deserialize some JSON data into an object.
//
// First, we define a struct, 'Employee', to contain the data:
//..
//  struct Employee {
//      bsl::string d_name;
//      bdet_Date   d_date;
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
//  assert(0 == baejsn_ParserUtil::getValue(&employee.d_name, nameRef));
//  assert(0 == baejsn_ParserUtil::getValue(&employee.d_date, dateRef));
//  assert(0 == baejsn_ParserUtil::getValue(&employee.d_age, ageRef));
//..
// Finally, we will verify that the values are as expected:
//..
//  assert("John Smith"            == employee.d_name);
//  assert(bdet_Date(1985, 06, 24) == employee.d_date);
//  assert(21                      == employee.d_age);
//..

#ifndef INCLUDED_BAESCM_VERSION
#include <baescm_version.h>
#endif

#ifndef INCLUDED_BDEPU_ISO8601
#include <bdepu_iso8601.h>
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

#ifndef INCLUDED_BSLS_TYPES
#include <bsls_types.h>
#endif

namespace BloombergLP {

class bdet_Date;
class bdet_Time;
class bdet_Datetime;
class bdet_DateTz;
class bdet_TimeTz;
class bdet_DatetimeTz;

                            // ========================
                            // struct baejsn_ParserUtil
                            // ========================

struct baejsn_ParserUtil {
    //This class provides utility functions for decoding data in the JSON
    //format into a 'bdeat' Simple type.  The primary method is 'getValue',
    //which decodes into a specified object and is overloaded for all 'bdeat'
    //Simple types.

  private:
    // PRIVATE CLASS METHODS
    template <typename TYPE>
    static int getDateAndTimeValue(TYPE              *value,
                                   bslstl::StringRef  data,
                                   int                maxLength);
        // Load into the specified 'value' the date or time value represented
        // as a string in the ISO 8601 format in the specified 'data' and
        // having a maximum data length of the specified 'maxLength'.  Return 0
        // on success and a non-zero value otherwise.  Note that an error is
        // returned if 'data.length() > maxLength'.

    template <typename TYPE>
    static int getIntegralValue(TYPE *value, bslstl::StringRef data);
        // Load into the specified 'value' the integer value in the specified
        // 'data'.  Return 0 on success and a non-zero value otherwise.

    template <typename TYPE>
    static int getUnsignedIntegralValue(TYPE *value, bslstl::StringRef data);
        // Load into the specified 'value' the unsigned integer value in the
        // specified 'data'.  Return 0 on success and a non-zero value
        // otherwise.

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
    static int getValue(bdet_Date           *value, bslstl::StringRef data);
    static int getValue(bdet_Datetime       *value, bslstl::StringRef data);
    static int getValue(bdet_DatetimeTz     *value, bslstl::StringRef data);
    static int getValue(bdet_DateTz         *value, bslstl::StringRef data);
    static int getValue(bdet_Time           *value, bslstl::StringRef data);
    static int getValue(bdet_TimeTz         *value, bslstl::StringRef data);
        // Load into the specified 'value' the characters read from the
        // specified 'data'.  Return 0 on success or a non-zero value on
        // failure.
};

// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================

                            // ------------------------
                            // struct baejsn_ParserUtil
                            // ------------------------

// CLASS METHODS
template <typename TYPE>
int baejsn_ParserUtil::getUnsignedIntegralValue(TYPE              *value,
                                                bslstl::StringRef  data)
{
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

template <typename TYPE>
int baejsn_ParserUtil::getIntegralValue(TYPE *value, bslstl::StringRef data)
{
    bool isNegative;
    if ('-' == data[0]) {
        isNegative = true;
        data.assign(data.begin() + 1, data.end());
    }
    else {
        isNegative = false;
    }

    bsls::Types::Uint64 tmp;
    int rc = getUint64(&tmp, data);
    if (rc) {
        return -1;                                                    // RETURN
    }

    if (isNegative) {
        bsls::Types::Int64 signedTmp =
                                     static_cast<bsls::Types::Int64>(tmp) * -1;
        if (signedTmp <
           static_cast<bsls::Types::Int64>(bsl::numeric_limits<TYPE>::min())) {
            return -1;                                                // RETURN
        }
        *value = static_cast<TYPE>(signedTmp);
    }
    else {
        if (tmp >
          static_cast<bsls::Types::Uint64>(bsl::numeric_limits<TYPE>::max())) {
            return -1;                                                // RETURN
        }
        *value = static_cast<TYPE>(tmp);
    }

    return 0;
}

template <typename TYPE>
int baejsn_ParserUtil::getDateAndTimeValue(TYPE              *value,
                                           bslstl::StringRef  data,
                                           int                maxLength)
{
    enum { BAEJSN_STRING_LENGTH_WITH_QUOTES = 2 };

    if (data.length()  < BAEJSN_STRING_LENGTH_WITH_QUOTES
     || '"'           != *data.begin()
     || '"'           != *(data.end() - 1)
     || data.length()  > static_cast<unsigned int>(maxLength)
                                          + BAEJSN_STRING_LENGTH_WITH_QUOTES) {
        return -1;                                                    // RETURN
    }

    return bdepu_Iso8601::parse(
           value,
           data.data() + 1,
           static_cast<int>(data.length() - BAEJSN_STRING_LENGTH_WITH_QUOTES));
}

inline
int baejsn_ParserUtil::getValue(bool *value, bslstl::StringRef data)
{
    if (0 == bsl::strncmp("true", data.data(), data.length())) {
        *value = true;
    }
    else if (0 == bsl::strncmp("false", data.data(), data.length())) {
        *value = false;
    }
    else {
        return -1;                                                    // RETURN
    }
    return 0;
}

inline
int baejsn_ParserUtil::getValue(char *value, bslstl::StringRef data)
{
    signed char tmp;  // Note that 'char' is unsigned on IBM.

    const int rc = getIntegralValue(&tmp, data);
    if (!rc) {
        *value = tmp;
    }
    return rc;
}

inline
int baejsn_ParserUtil::getValue(unsigned char *value, bslstl::StringRef data)
{
    return getUnsignedIntegralValue(value, data);
}

inline
int baejsn_ParserUtil::getValue(signed char *value, bslstl::StringRef data)
{
    return getValue((char *) value, data);
}

inline
int baejsn_ParserUtil::getValue(short *value, bslstl::StringRef data)
{
    return getIntegralValue(value, data);
}

inline
int baejsn_ParserUtil::getValue(unsigned short *value, bslstl::StringRef data)
{
    return getUnsignedIntegralValue(value, data);
}

inline
int baejsn_ParserUtil::getValue(int *value, bslstl::StringRef data)
{
    return getIntegralValue(value, data);
}

inline
int baejsn_ParserUtil::getValue(unsigned int *value, bslstl::StringRef data)
{
    return getUnsignedIntegralValue(value, data);
}

inline
int baejsn_ParserUtil::getValue(bsls::Types::Int64 *value, 
                                bslstl::StringRef   data)
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

    bsls::Types::Uint64 tmp = 0;
    if (0 != getUint64(&tmp, data)) {
        return -1;                                                    // RETURN
    }

    if (isNegative && tmp <= static_cast<bsls::Types::Uint64>(
                         bsl::numeric_limits<bsls::Types::Int64>::max() + 1)) {
        *value = static_cast<bsls::Types::Int64>(tmp) * -1;
    }
    else if (tmp <= static_cast<bsls::Types::Uint64>(
                             bsl::numeric_limits<bsls::Types::Int64>::max())) {
        *value = static_cast<bsls::Types::Int64>(tmp);
    }
    else {
        return -1;                                                    // RETURN
    }
    return 0;
}

inline
int baejsn_ParserUtil::getValue(bsls::Types::Uint64 *value,
                                bslstl::StringRef    data)
{
    return getUnsignedIntegralValue(value, data);
}

inline
int baejsn_ParserUtil::getValue(float *value, bslstl::StringRef data)
{
    double tmp;
    const int rc = getValue(&tmp, data);
    if (!rc) {
        *value = static_cast<float>(tmp);
    }
    return rc;
}

inline
int baejsn_ParserUtil::getValue(bsl::string *value, bslstl::StringRef data)
{
    return getString(value, data);
}

inline
int baejsn_ParserUtil::getValue(bdet_Date *value, bslstl::StringRef data)
{
    enum { MAX_LENGTH = bdepu_Iso8601::BDEPU_DATE_STRLEN };

    return getDateAndTimeValue(value, data, MAX_LENGTH);
}

inline
int baejsn_ParserUtil::getValue(bdet_Datetime *value, bslstl::StringRef data)
{
    enum { MAX_LENGTH = bdepu_Iso8601::BDEPU_DATETIME_STRLEN };

    return getDateAndTimeValue(value, data, MAX_LENGTH);
}

inline
int baejsn_ParserUtil::getValue(bdet_DatetimeTz *value, bslstl::StringRef data)
{
    enum { MAX_LENGTH = bdepu_Iso8601::BDEPU_DATETIMETZ_STRLEN };

    return getDateAndTimeValue(value, data, MAX_LENGTH);
}

inline
int baejsn_ParserUtil::getValue(bdet_DateTz *value, bslstl::StringRef data)
{
    enum { MAX_LENGTH = bdepu_Iso8601::BDEPU_DATETZ_STRLEN };

    return getDateAndTimeValue(value, data, MAX_LENGTH);
}

inline
int baejsn_ParserUtil::getValue(bdet_Time *value, bslstl::StringRef data)
{
    enum { MAX_LENGTH = bdepu_Iso8601::BDEPU_TIME_STRLEN };

    return getDateAndTimeValue(value, data, MAX_LENGTH);
}

inline
int baejsn_ParserUtil::getValue(bdet_TimeTz *value, bslstl::StringRef data)
{
    enum { MAX_LENGTH = bdepu_Iso8601::BDEPU_TIMETZ_STRLEN };

    return getDateAndTimeValue(value, data, MAX_LENGTH);
}

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2012
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
