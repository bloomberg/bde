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

#ifndef INCLUDED_BSL_STREAMBUF
#include <bsl_streambuf.h>
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
    static int getDateAndTimeValue(bsl::streambuf *streamBuf,
                                   TYPE           *value,
                                   int             maxLength);

    template <typename TYPE>
    static int getIntegralValue(bsl::streambuf *streamBuf, TYPE *value);

    static int getUint64(bsl::streambuf      *streamBuf,
                         bsls::Types::Uint64 *value);

    static int getValueImp(bsl::streambuf *streamBuf, bool            *value);
    static int getValueImp(bsl::streambuf *streamBuf, char            *value);
    static int getValueImp(bsl::streambuf *streamBuf, unsigned char   *value);
    static int getValueImp(bsl::streambuf *streamBuf, signed char     *value);
    static int getValueImp(bsl::streambuf *streamBuf, short           *value);
    static int getValueImp(bsl::streambuf *streamBuf, unsigned short  *value);
    static int getValueImp(bsl::streambuf *streamBuf, int             *value);
    static int getValueImp(bsl::streambuf *streamBuf, unsigned int    *value);
    static int getValueImp(bsl::streambuf     *streamBuf,
                           bsls::Types::Int64 *value);
    static int getValueImp(bsl::streambuf      *streamBuf,
                           bsls::Types::Uint64 *value);
    static int getValueImp(bsl::streambuf *streamBuf, float           *value);
    static int getValueImp(bsl::streambuf *streamBuf, double          *value);
    static int getValueImp(bsl::streambuf *streamBuf, bsl::string     *value);
    static int getValueImp(bsl::streambuf *streamBuf, bdet_Date       *value);
    static int getValueImp(bsl::streambuf *streamBuf, bdet_Datetime   *value);
    static int getValueImp(bsl::streambuf *streamBuf, bdet_DatetimeTz *value);
    static int getValueImp(bsl::streambuf *streamBuf, bdet_DateTz     *value);
    static int getValueImp(bsl::streambuf *streamBuf, bdet_Time       *value);
    static int getValueImp(bsl::streambuf *streamBuf, bdet_TimeTz     *value);

    static int getDouble(bsl::streambuf *streamBuf, double *value);

  public:
    // CLASS METHODS
    static void skipSpaces(bsl::streambuf *streamBuf);
        // Read characters from the specified 'streamBuf' until a
        // non-whitespace character is encountered.  Note that the function
        // also returns if the end of file is reached before a non-whitespace
        // character.

    static int getString(bsl::streambuf *streamBuf, bsl::string *value);
        // Load into the specified 'value' the sequence of non-whitespace
        // unicode characters enclosed within quotes read from the specified
        // 'streamBuf'.  Return 0 on success and a non-zero value otherwise.
        // Note that an error is returned if the end of file is reached before
        // the trailing quote.

    static int eatToken(bsl::streambuf *streamBuf, const char *token);
        // Read the characters in the specified 'token' from the specified
        // 'streamBuf' and advance the get pointer to the character just after
        // the matched string, with no effect otherwise.  Return 0 if a match
        // is found, and a non-zero value otherwise.

    static int advancePastWhitespaceAndToken(bsl::streambuf *streamBuf,
                                             char            token);
        // Read characters from the specified 'streamBuf' until the specified
        // 'token' is encountered and skipping any whitespace characters.
        // Return 0 on success or a non-zero value on failure.  Note that an
        // error is returned if either the end of file is encountered or the
        // first non-whitespace character is not 'token'.

    template <typename TYPE>
    static int getValue(bsl::streambuf *streamBuf, TYPE *value);
        // Load into the specified 'value' the characters read from the
        // specified 'streamBuf'.
};

// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================

                            // ------------------------
                            // struct baejsn_ParserUtil
                            // ------------------------

// PRIVATE CLASS METHODS
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
                                valueString.length())
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

inline
int baejsn_ParserUtil::getValueImp(bsl::streambuf *streamBuf, bool *value)
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
int baejsn_ParserUtil::getValueImp(bsl::streambuf *streamBuf,
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
int baejsn_ParserUtil::getValueImp(bsl::streambuf *streamBuf,
                                   unsigned char  *value)
{
    return getIntegralValue(streamBuf, value);
}

inline
int baejsn_ParserUtil::getValueImp(bsl::streambuf *streamBuf,
                                   signed char    *value)
{
    return getValueImp(streamBuf, (char *) value);
}

inline
int baejsn_ParserUtil::getValueImp(bsl::streambuf *streamBuf, short *value)
{
    return getIntegralValue(streamBuf, value);
}

inline
int baejsn_ParserUtil::getValueImp(bsl::streambuf *streamBuf,
                                   unsigned short *value)
{
    return getIntegralValue(streamBuf, value);
}

inline
int baejsn_ParserUtil::getValueImp(bsl::streambuf *streamBuf, int *value)
{
    return getIntegralValue(streamBuf, value);
}

inline
int baejsn_ParserUtil::getValueImp(bsl::streambuf *streamBuf,
                                   unsigned int   *value)
{
    return getIntegralValue(streamBuf, value);
}

inline
int baejsn_ParserUtil::getValueImp(bsl::streambuf     *streamBuf,
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
int baejsn_ParserUtil::getValueImp(bsl::streambuf      *streamBuf,
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
int baejsn_ParserUtil::getValueImp(bsl::streambuf *streamBuf,
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
int baejsn_ParserUtil::getValueImp(bsl::streambuf *streamBuf,
                                   double         *value)
{
    return getDouble(streamBuf, value);
}

inline
int baejsn_ParserUtil::getValueImp(bsl::streambuf *streamBuf,
                                   bsl::string    *value)
{
    return getString(streamBuf, value);
}

inline
int baejsn_ParserUtil::getValueImp(bsl::streambuf *streamBuf,
                                   bdet_Date      *value)
{
    enum { MAX_LENGTH = bdepu_Iso8601::BDEPU_DATE_STRLEN };

    return getDateAndTimeValue(streamBuf, value, MAX_LENGTH);
}

inline
int baejsn_ParserUtil::getValueImp(bsl::streambuf *streamBuf,
                                   bdet_Datetime  *value)
{
    enum { MAX_LENGTH = bdepu_Iso8601::BDEPU_DATETIME_STRLEN };

    return getDateAndTimeValue(streamBuf, value, MAX_LENGTH);
}

inline
int baejsn_ParserUtil::getValueImp(bsl::streambuf  *streamBuf,
                                   bdet_DatetimeTz *value)
{
    enum { MAX_LENGTH = bdepu_Iso8601::BDEPU_DATETIMETZ_STRLEN };

    return getDateAndTimeValue(streamBuf, value, MAX_LENGTH);
}

inline
int baejsn_ParserUtil::getValueImp(bsl::streambuf *streamBuf,
                                   bdet_DateTz    *value)
{
    enum { MAX_LENGTH = bdepu_Iso8601::BDEPU_DATETZ_STRLEN };

    return getDateAndTimeValue(streamBuf, value, MAX_LENGTH);
}

inline
int baejsn_ParserUtil::getValueImp(bsl::streambuf *streamBuf,
                                   bdet_Time      *value)
{
    enum { MAX_LENGTH = bdepu_Iso8601::BDEPU_TIME_STRLEN };

    return getDateAndTimeValue(streamBuf, value, MAX_LENGTH);
}

inline
int baejsn_ParserUtil::getValueImp(bsl::streambuf *streamBuf,
                                   bdet_TimeTz    *value)
{
    enum { MAX_LENGTH = bdepu_Iso8601::BDEPU_TIMETZ_STRLEN };

    return getDateAndTimeValue(streamBuf, value, MAX_LENGTH);
}

// CLASS METHODS
template <class TYPE>
inline
int baejsn_ParserUtil::getValue(bsl::streambuf *streamBuf, TYPE *value)
{
    return getValueImp(streamBuf, value);
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
