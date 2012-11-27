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
//@AUTHOR: Raymond Chiu (schiu49)
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

    static int getUint64(bsls::Types::Uint64 *value, bslstl::StringRef data);

    static int getDouble(double *value, bslstl::StringRef data);

    static int getString(bsl::string *value, bslstl::StringRef data);

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
        // specified 'streamBuf'.  Return 0 on success or a non-zero value on
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
int baejsn_ParserUtil::getIntegralValue(TYPE *value, bslstl::StringRef data)
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
int baejsn_ParserUtil::getDateAndTimeValue(TYPE              *value,
                                           bslstl::StringRef  data,
                                           int                maxLength)
{
    return data.length() <= static_cast<unsigned int>(maxLength)
         ? bdepu_Iso8601::parse(value,
                                data.data(),
                                static_cast<int>(data.length()))
         : -1;
}

inline
int baejsn_ParserUtil::getValue(bool *value, bslstl::StringRef data)
{
    if (0 == bsl::strcmp(data.data(), "true")) {
        *value = true;
    }
    else if (0 == bsl::strcmp(data.data(), "false")) {
        *value = false;
    }
    else {
        return -1;                                                    // RETURN
    }
    return 0;
}

inline
int baejsn_ParserUtil::getValue(unsigned char *value, bslstl::StringRef data)
{
    return getIntegralValue(value, data);
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
    return getIntegralValue(value, data);
}

inline
int baejsn_ParserUtil::getValue(int *value, bslstl::StringRef data)
{
    return getIntegralValue(value, data);
}

inline
int baejsn_ParserUtil::getValue(unsigned int *value, bslstl::StringRef data)
{
    return getIntegralValue(value, data);
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
    return getIntegralValue(value, data);
}

inline
int baejsn_ParserUtil::getValue(float *value, bslstl::StringRef data)
{
    double tmp;
    const int rc = getDouble(&tmp, data);
    if (!rc) {
        *value = static_cast<float>(tmp);
    }
    return rc;
}

inline
int baejsn_ParserUtil::getValue(double *value, bslstl::StringRef data)
{
    return getDouble(value, data);
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
