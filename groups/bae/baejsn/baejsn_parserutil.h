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
//@DESCRIPTION: This component provides utility functions for encoding and
// decoding of primitive JSON constructs into fundamental C++ types.

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

  private:
    // PRIVATE CLASS METHODS
    template <typename TYPE>
    static int getDateAndTimeValue(TYPE              *value,
                                   bslstl::StringRef  data,
                                   int                maxLength);

    template <typename TYPE>
    static int getIntegralValue(TYPE *value, bslstl::StringRef data);

    template <typename TYPE>
    static int getUnsignedIntegralValue(TYPE *value, bslstl::StringRef data);

    static int getUint64(bsls::Types::Uint64 *value, bslstl::StringRef data);

    static int getString(bsl::string *value, bslstl::StringRef data);

    template <typename TYPE>
    static int getDateAndTimeValue(bsl::streambuf *streamBuf,
                                   TYPE           *value,
                                   int             maxLength);
        // DEPRECATED:

    template <typename TYPE>
    static int getIntegralValue(bsl::streambuf *streamBuf, TYPE *value);
        // DEPRECATED:

    static int getUint64(bsl::streambuf      *streamBuf,
                         bsls::Types::Uint64 *value);
        // DEPRECATED:

    static int getDouble(bsl::streambuf *streamBuf, double *value);
        // DEPRECATED:

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

    static void skipSpaces(bsl::streambuf *streamBuf);
        // Read characters from the specified 'streamBuf' until a
        // non-whitespace character is encountered.  Note that the function
        // also returns if the end of file is reached before a non-whitespace
        // character.
        //
        // DEPRECATED: See 'baejsn_decoder' instead.

    static int getString(bsl::streambuf *streamBuf, bsl::string *value);
        // Load into the specified 'value' the sequence of unicode characters
        // enclosed within quotes read from the specified 'streamBuf' ignoring
        // any leading whitespace characters.  Return 0 on success and a
        // non-zero value otherwise.  Note that an error is returned if the end
        // of file is reached before the trailing quote.
        //
        // DEPRECATED: Use 'getValue' instead.

    static int eatToken(bsl::streambuf *streamBuf, const char *token);
        // Read the characters in the specified 'token' from the specified
        // 'streamBuf' and advance the get pointer to the character just after
        // the matched string, with no effect otherwise.  Return 0 if a match
        // is found, and a non-zero value otherwise.
        //
        // DEPRECATED: Use 'getValue' instead.

    static int advancePastWhitespaceAndToken(bsl::streambuf *streamBuf,
                                             char            token);
        // Read characters from the specified 'streamBuf' until the specified
        // 'token' is encountered and skipping any whitespace characters.
        // Return 0 on success or a non-zero value on failure.  Note that an
        // error is returned if either the end of file is encountered or the
        // first non-whitespace character is not 'token'.
        //
        // DEPRECATED: See 'baejsn_decoder' instead.

    static int getValue(bsl::streambuf *streamBuf, bool                *value);
    static int getValue(bsl::streambuf *streamBuf, char                *value);
    static int getValue(bsl::streambuf *streamBuf, unsigned char       *value);
    static int getValue(bsl::streambuf *streamBuf, signed char         *value);
    static int getValue(bsl::streambuf *streamBuf, short               *value);
    static int getValue(bsl::streambuf *streamBuf, unsigned short      *value);
    static int getValue(bsl::streambuf *streamBuf, int                 *value);
    static int getValue(bsl::streambuf *streamBuf, unsigned int        *value);
    static int getValue(bsl::streambuf *streamBuf, bsls::Types::Int64  *value);
    static int getValue(bsl::streambuf *streamBuf, bsls::Types::Uint64 *value);
    static int getValue(bsl::streambuf *streamBuf, float               *value);
    static int getValue(bsl::streambuf *streamBuf, double              *value);
    static int getValue(bsl::streambuf *streamBuf, bsl::string         *value);
    static int getValue(bsl::streambuf *streamBuf, bdet_Date           *value);
    static int getValue(bsl::streambuf *streamBuf, bdet_Datetime       *value);
    static int getValue(bsl::streambuf *streamBuf, bdet_DatetimeTz     *value);
    static int getValue(bsl::streambuf *streamBuf, bdet_DateTz         *value);
    static int getValue(bsl::streambuf *streamBuf, bdet_Time           *value);
    static int getValue(bsl::streambuf *streamBuf, bdet_TimeTz         *value);
        // Load into the specified 'value' the characters read from the
        // specified 'streamBuf'.  Return 0 on success or a non-zero value on
        // failure.
        //
        // DEPRECATED: Use the alternate 'getValue' overload instead.
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
        if (-tmp <
          static_cast<bsls::Types::Uint64>(bsl::numeric_limits<TYPE>::min())) {
            return -1;                                                // RETURN
        }
        tmp *= -1;
    }
    else if (tmp >
          static_cast<bsls::Types::Uint64>(bsl::numeric_limits<TYPE>::max())) {
        return -1;                                                    // RETURN
    }

    *value = static_cast<TYPE>(tmp);
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
    return getIntegralValue(value, data);
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

// DEPRECATED PRIVATE CLASS METHODS
template <typename TYPE>
int baejsn_ParserUtil::getDateAndTimeValue(bsl::streambuf *streamBuf,
                                           TYPE           *value,
                                           int             maxLength)
{
    bsl::string valueString;
    if (0 != getString(streamBuf, &valueString)) {
        return -1;                                                    // RETURN
    }

    return valueString.length() <= static_cast<unsigned int>(maxLength)
         ? bdepu_Iso8601::parse(value,
                                valueString.data(),
                                static_cast<int>(valueString.length()))
         : -1;
}

template <typename TYPE>
int baejsn_ParserUtil::getIntegralValue(bsl::streambuf *streamBuf,
                                        TYPE           *value)
{
    double tmp;
    int rc = getDouble(streamBuf, &tmp);
    if (rc) {
        return -1;                                                    // RETURN
    }

    const double tolerance = 0.99;
    const double low       = tmp + tolerance;    // accept -TYPE_MIN.99
    const double hi        = tmp - tolerance;    // accept  TYPE_MAX.99

    if (low < static_cast<double>(bsl::numeric_limits<TYPE>::min())
     || hi  > static_cast<double>(bsl::numeric_limits<TYPE>::max())) {
        return -1;                                                    // RETURN
    }

    *value = static_cast<TYPE>(tmp);
    return 0;
}

// DEPRECATED CLASS METHODS
inline
int baejsn_ParserUtil::getValue(bsl::streambuf *streamBuf, bool *value)
{
    if (0 == eatToken(streamBuf, "true")) {
        *value = true;
    }
    else if (0 == eatToken(streamBuf, "false")) {
        *value = false;
    }
    else {
        return -1;                                                    // RETURN
    }
    return 0;
}

inline
int baejsn_ParserUtil::getValue(bsl::streambuf *streamBuf,
                                char           *value)
{
    bsl::string valueString;

    if (0 == getString(streamBuf, &valueString) && 1 == valueString.length()) {
        *value = valueString[0];
        return 0;                                                     // RETURN
    }
    return -1;
}

inline
int baejsn_ParserUtil::getValue(bsl::streambuf *streamBuf,
                                unsigned char  *value)
{
    return getIntegralValue(streamBuf, value);
}

inline
int baejsn_ParserUtil::getValue(bsl::streambuf *streamBuf,
                                signed char    *value)
{
    return getValue(streamBuf, (char *) value);
}

inline
int baejsn_ParserUtil::getValue(bsl::streambuf *streamBuf, short *value)
{
    return getIntegralValue(streamBuf, value);
}

inline
int baejsn_ParserUtil::getValue(bsl::streambuf *streamBuf,
                                unsigned short *value)
{
    return getIntegralValue(streamBuf, value);
}

inline
int baejsn_ParserUtil::getValue(bsl::streambuf *streamBuf, int *value)
{
    return getIntegralValue(streamBuf, value);
}

inline
int baejsn_ParserUtil::getValue(bsl::streambuf *streamBuf,
                                unsigned int   *value)
{
    return getIntegralValue(streamBuf, value);
}

inline
int baejsn_ParserUtil::getValue(bsl::streambuf     *streamBuf,
                                bsls::Types::Int64 *value)
{
    skipSpaces(streamBuf);

    int ch = streamBuf->sgetc();

    bool isNegative;
    if ('-' == ch) {
        isNegative = true;
        streamBuf->snextc();
    }
    else {
        isNegative = false;
    }

    bsls::Types::Uint64 tmp = 0;
    if (0 != getUint64(streamBuf, &tmp)) {
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
int baejsn_ParserUtil::getValue(bsl::streambuf      *streamBuf,
                                bsls::Types::Uint64 *value)
{
    bsls::Types::Uint64 tmp = 0;
    const int rc = getUint64(streamBuf, &tmp);
    if (!rc) {
        *value = tmp;
    }
    return rc;
}

inline
int baejsn_ParserUtil::getValue(bsl::streambuf *streamBuf,
                                float          *value)
{
    double tmp;
    const int rc = getDouble(streamBuf, &tmp);
    if (!rc) {
        *value = static_cast<float>(tmp);
    }
    return rc;
}

inline
int baejsn_ParserUtil::getValue(bsl::streambuf *streamBuf,
                                double         *value)
{
    return getDouble(streamBuf, value);
}

inline
int baejsn_ParserUtil::getValue(bsl::streambuf *streamBuf,
                                bsl::string    *value)
{
    return getString(streamBuf, value);
}

inline
int baejsn_ParserUtil::getValue(bsl::streambuf *streamBuf,
                                bdet_Date      *value)
{
    enum { MAX_LENGTH = bdepu_Iso8601::BDEPU_DATE_STRLEN };

    return getDateAndTimeValue(streamBuf, value, MAX_LENGTH);
}

inline
int baejsn_ParserUtil::getValue(bsl::streambuf *streamBuf,
                                bdet_Datetime  *value)
{
    enum { MAX_LENGTH = bdepu_Iso8601::BDEPU_DATETIME_STRLEN };

    return getDateAndTimeValue(streamBuf, value, MAX_LENGTH);
}

inline
int baejsn_ParserUtil::getValue(bsl::streambuf  *streamBuf,
                                bdet_DatetimeTz *value)
{
    enum { MAX_LENGTH = bdepu_Iso8601::BDEPU_DATETIMETZ_STRLEN };

    return getDateAndTimeValue(streamBuf, value, MAX_LENGTH);
}

inline
int baejsn_ParserUtil::getValue(bsl::streambuf *streamBuf,
                                bdet_DateTz    *value)
{
    enum { MAX_LENGTH = bdepu_Iso8601::BDEPU_DATETZ_STRLEN };

    return getDateAndTimeValue(streamBuf, value, MAX_LENGTH);
}

inline
int baejsn_ParserUtil::getValue(bsl::streambuf *streamBuf,
                                bdet_Time      *value)
{
    enum { MAX_LENGTH = bdepu_Iso8601::BDEPU_TIME_STRLEN };

    return getDateAndTimeValue(streamBuf, value, MAX_LENGTH);
}

inline
int baejsn_ParserUtil::getValue(bsl::streambuf *streamBuf,
                                bdet_TimeTz    *value)
{
    enum { MAX_LENGTH = bdepu_Iso8601::BDEPU_TIMETZ_STRLEN };

    return getDateAndTimeValue(streamBuf, value, MAX_LENGTH);
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
