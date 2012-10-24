// baejsn_parserutil.h                                                -*-C++-*-
#ifndef INCLUDED_BAEJSN_PARSERUTIL
#define INCLUDED_BAEJSN_PARSERUTIL

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide functions to encode and decode simple types in JSON format.
//
//@CLASSES:
//
//@SEE_ALSO:
//
//@AUTHOR: Raymond Chiu (schiu49)
//
//@DESCRIPTION: This component provides utility functions for encoding and
// decoding of primitive JSON constructs fundamental C++ types.

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BDEPU_ISO8601
#include <bdepu_iso8601.h>
#endif

#ifndef INCLUDED_BSL_STREAMBUF
#include <bsl_streambuf.h>
#endif

#ifndef INCLUDED_BSLS_TYPES
#include <bsls_types.h>
#endif

namespace BloombergLP {


                            // ========================
                            // struct baejsn_ParserUtil
                            // ========================

struct baejsn_ParserUtil {
        // TBD: We can get overflow in the getValueImp functions.

  private:

    template <typename TYPE>
    static int getDateAndTimeValue(bsl::streambuf *streamBuf, TYPE *value);

    template <typename TYPE>
    static int getNumericalValue(bsl::streambuf *streamBuf, TYPE *value);

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

    static int getInteger(bsl::streambuf      *streamBuf,
                          bsls::Types::Int64  *value);

    static int getDouble(bsl::streambuf *streamBuf,
                         double         *value);

  public:
    static int skipSpaces(bsl::streambuf *streamBuf);

    static int getString(bsl::streambuf *streamBuf, bsl::string *value);

    static int eatToken(bsl::streambuf *streamBuf, const char *token);
        // If the get pointer of specified 'streamBuf' refers to a string that
        // matches the specified 'token', advance the get pointer to the
        // charater just after the matched string, with no effect otherwise.
        // Return 0 if a match is found, and a non-zero value otherwise.

    static int advancePastWhitespaceAndToken(bsl::streambuf *streamBuf,
                                             char            token);
        // TBD

    static int getInteger(bsl::streambuf      *streamBuf,
                          bsls::Types::Uint64 *value);
        // TBD make private

    template <class TYPE>
    static int getNumber(bsl::streambuf *streamBuf, TYPE *value);
        // TBD make private

    template <typename TYPE>
    static int putValue(bsl::streambuf *streamBuf, const TYPE& value);

    template <typename TYPE>
    static int getValue(bsl::streambuf *streamBuf, TYPE *value);
};

// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================

                            // ------------------------
                            // struct baejsn_ParserUtil
                            // ------------------------

// PRIVATE METHODS
template <typename TYPE>
int baejsn_ParserUtil::getDateAndTimeValue(bsl::streambuf *streamBuf,
                                           TYPE           *value)
{
    bsl::string temp;
    if (0 != getString(streamBuf, &temp)) {
        return -1;                                                    // RETURN
    }
    return bdepu_Iso8601::parse(value, temp.data(), temp.length());
}

template <typename TYPE>
inline
int baejsn_ParserUtil::getNumericalValue(bsl::streambuf *streamBuf,
                                         TYPE           *value)
{
    return getNumber(streamBuf, value);
}

inline
int baejsn_ParserUtil::getValueImp(bsl::streambuf *streamBuf,
                                   bool           *value)
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

    if (0 == getString(streamBuf, &valueString)
     && 1 == valueString.length()) {
        *value = valueString[0];
    }
    else {
        return -1;                                                    // RETURN
    }
    return 0;
}

inline
int baejsn_ParserUtil::getValueImp(bsl::streambuf *streamBuf,
                                   unsigned char  *value)
{
    return getNumericalValue(streamBuf, value);
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
    return getNumericalValue(streamBuf, value);
}

inline
int baejsn_ParserUtil::getValueImp(bsl::streambuf *streamBuf,
                                   unsigned short *value)
{
    return getNumericalValue(streamBuf, value);
}

inline
int baejsn_ParserUtil::getValueImp(bsl::streambuf *streamBuf, int *value)
{
    return getNumericalValue(streamBuf, value);
}

inline
int baejsn_ParserUtil::getValueImp(bsl::streambuf *streamBuf,
                                   unsigned int   *value)
{
    return getNumericalValue(streamBuf, value);
}

inline
int baejsn_ParserUtil::getValueImp(bsl::streambuf     *streamBuf,
                                   bsls::Types::Int64 *value)
{
    return getNumericalValue(streamBuf, value);
//     int ch = streamBuf->sgetc();

//     if (ch == bsl::streambuf::traits_type::eof()) {
//         return -1;                                                    // RETURN
//     }

//     bool isNegative;

//     if (ch == '-') {
//         isNegative = true;
//         streamBuf->snextc();
//     }
//     else {
//         isNegative = false;
//     }

//     bsls::Types::Uint64 tmp = 0;
//     if (0 != getInteger(streamBuf, &tmp)) {
//         return -1;                                                    // RETURN
//     }
//     *value = static_cast<bsls::Types::Int64>(tmp);

//     if (isNegative) {
//         *value = *value * -1;
//     }

//     return 0;
}

inline
int baejsn_ParserUtil::getValueImp(bsl::streambuf      *streamBuf,
                                   bsls::Types::Uint64 *value)
{
    return getNumericalValue(streamBuf, value);
}

inline
int baejsn_ParserUtil::getValueImp(bsl::streambuf *streamBuf,
                                   float          *value)
{
    return getNumericalValue(streamBuf, value);
}

inline
int baejsn_ParserUtil::getValueImp(bsl::streambuf *streamBuf,
                                   double         *value)
{
    return getNumericalValue(streamBuf, value);
}

inline
int baejsn_ParserUtil::getValueImp(bsl::streambuf *streamBuf,
                                   bsl::string    *value)
{
    if (0 != getString(streamBuf, value)) {
        return -1;                                                    // RETURN
    }
    return 0;
}

inline
int baejsn_ParserUtil::getValueImp(bsl::streambuf *streamBuf,
                                   bdet_Date      *value)
{
    return getDateAndTimeValue(streamBuf, value);
}

inline
int baejsn_ParserUtil::getValueImp(bsl::streambuf *streamBuf,
                                   bdet_Datetime  *value)
{
    return getDateAndTimeValue(streamBuf, value);
}

inline
int baejsn_ParserUtil::getValueImp(bsl::streambuf  *streamBuf,
                                   bdet_DatetimeTz *value)
{
    return getDateAndTimeValue(streamBuf, value);
}

inline
int baejsn_ParserUtil::getValueImp(bsl::streambuf *streamBuf,
                                   bdet_DateTz    *value)
{
    return getDateAndTimeValue(streamBuf, value);
}

inline
int baejsn_ParserUtil::getValueImp(bsl::streambuf *streamBuf,
                                   bdet_Time      *value)
{
    return getDateAndTimeValue(streamBuf, value);
}

inline
int baejsn_ParserUtil::getValueImp(bsl::streambuf *streamBuf,
                                   bdet_TimeTz    *value)
{
    return getDateAndTimeValue(streamBuf, value);
}

template <class TYPE>
inline
int baejsn_ParserUtil::getNumber(bsl::streambuf *streamBuf,
                                 TYPE           *value)
{
    double temp;
    int rc = getDouble(streamBuf, &temp);
    *value = static_cast<TYPE>(temp);
    return rc;
}

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
