// bdepu_iso8601.h                                                    -*-C++-*-
#ifndef INCLUDED_BDEPU_ISO8601
#define INCLUDED_BDEPU_ISO8601

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a utility for converting date/time objects to/from ISO8601.
//
//@CLASSES:
//     bdepu_Iso8601: namespace for ISO8601 date/time conversion functions
//
//@SEE_ALSO: bdet_date, bdet_datetime, bdet_datetimetz, bdet_datetz, bdet_time,
//           bdet_timetz, baexml_encoder, baexml_decoder
//
//@AUTHOR: Shezan Baig (sbaig)
//
//@CONTACT: Rohan Bhindwale (rbhindwa)
//
//@DESCRIPTION: This component provides a namespace, 'bdepu_Iso8601'
// containing functions that convert 'bdet' date and time objects to and from
// a string representation.  The string representation generated and parsed by
// this component adheres to the ISO8601 standard for dates and time and is
// suitable for use in XML generation or parsing.
//
// Each 'generate' function takes a 'bsl::ostream' and a 'bdet' date or time
// object and writes the ISO8601 representation to the stream.  Each 'parse'
// function takes a pointer to a 'bdet' date or time object, a character
// string, and a length and parses the character string into the date or time
// object, returning a non-zero status on error.
//
///Subset of ISO 8601 Supported 'bdepu_iso8601'
///--------------------------------------------
// This component provides support for a subset of the formats defined by the
// ISO 8601 standard, particularly those dealing with the representation of
// date, time, and date-time values.  The standard can be found at:
//
//: o http://dotat.at/tmp/ISO_8601-2004_E.pdf
//
///Supported Formats
///- - - - - - - - -
// The 'bdepu_iso8601' supports the the following formats defined in the
// ISO 8601 standard:
//
//: o <FRAC> := .d+
//: o <TZ>   := ((+|-)hh:mm|Z|z)
//: o <Date> := YYYY-MM-DD
//: o <Time> := hh:mm:ss{<FRAC>}
//: o <Datetime> := <Date>T<Time>
//: o <DateTz> := <Date>{<TZ>}
//: o <TimeTz> := <Time>{<TZ>}
//: o <DatetimeTz> := <Datetime>{<TZ>}
//
//: o 'bdet_Date', 'bdet_DateTz' both accept input of the form '<DateTz>'.
//: o 'bdet_Time', 'bdet_TimeTz' both accept input of the form '<TimeTz>'.
//: o 'bdet_Datetime', 'bdet_DatetimeTz' both accept input of the form
//:   '<DatetimeTz>'.
//
//: o Where '<FRAC>' is optional.
//:   o 'd' in 'FRAC' is any decimal digit.
//:   o If present, 'FRAC' must consist of a '.' followed by at least one digit
//:     d, and is interpreted as a fraction of a second.  Digits after the
//:     first 4 are ignored.  If more than 3 digits, it is rounded to the
//:     nearest 3 digits, possibly resulting in a value of 1.0, which will
//:     carry into the higher fields of the time and possibly date.
//: o '<TZ>' is always optional.
//:   o '<TZ>' is always exactly 1 or 6 characters long.  If it is 6 chars, it
//:     is either '+hh:mm' or '-hh:mm', if it is 1 char, it is 'Z' or 'z' which
//:     has the same meaning as '+00:00' (UTC).
//:   o In 'bdet_Date', '<TZ>' is ignored if present.
//:   o In 'bdet_Time' and 'bdet_Datetime', which have no time zone fields,
//:     '<TZ>' is converted to minutes and subtracted from the result.
//:   o In 'bdet_DateTz', 'bdet_TimeTz', and 'bdet_DatetimeTz' it sets the
//:     timezone field.  If absent while parsing an object with a timezone
//:     field, the timezone field is set to '+00:00' (UTC).
//:   o In <TZ>, 'hh' is in the range '00 - 23', 'mm' is in the range
//:     '00 - 59'.  A value of '24' for 'hh' is not allowed.
//: o In types containing '<Date>':
//:   o 'YYYY' is always 4 digits long, 'MM' and 'DD' are always 2 decimal
//:     digits long.
//:   o 'YYYY' is in the range '0001' - '9999'.
//:   o 'MM' is in the range '01' - '12'
//:   o 'DD' is in the range '01' - '31', also subject to constraints depending
//:     on year and month.
//:   o 'YYYY-MM-DD' must be a valid year, month, day combination according to
//:     'bdet_Date'.
//: o In types containing '<Time>':
//:   o 'hh', 'mm', and 'ss' are always two decimal digits long.
//:   o 'hh' must be in the range '00 - '24'.  If 'hh' is '24', then 'mm', and
//:     'ss' must be zero, as must '<FRAC>' and '<TZ>' if present.
//:   o 'mm' must be in the range '00 - '59'.
//:   o 'ss' must be in the range '00 - '59'.
//:   o 'hh:mm:ss' must be a valid hour, minute, second combination according
//:     to 'bdet_Time'.
//: o Where 'T' in '<Datetime>' literally is the uppercase letter 'T'.  It must
//:   be present.
//
///Subsets of ISO8601 Format Not Accepted by These Parsers
///- - - - - - - - - - - - - - - - - - - - - - - - - - - -
// The ISO8601 standard allows for the following formats which this component
// does not support:
//
//: o Ordinal Dates: Dates represented by year and day of year: 2.1.10
//: o Week Dates: Dates represented by year, ordinal week within the year, and
//:   ordinal day within the week: 2.1.11, 4.1.3.2
//: o Recurring Time Intervals: 2.1.17
//: o Leap Seconds: 2.2.2
//: o Calendar Week Numbers: 2.2.10, 4.1.4.
//: o Basic Formats, i.e.  Dates represented as 'YYYYMMDD', times represented
//:   as 'hhmmss', etc: 2.3.3.  This component supports complete formats only,
//:   that is, only formats with separators such as '-' in dates and ':' in
//:   times, and only formats that where 'YYYY' is 4 digits and 'MM', 'DD',
//:   'hh', 'mm', and 'ss' are always 2 digits.
//: o Extended Formats: 2.3.4 Many of the 'Extended Formats' described in the
//:   document are in fact complete, and are thus supported, but many extended
//:   formats, that have an inappropriate number of digits representing their
//:   fields, are not supported.
//: o Representation with Reduced Accuracy: 2.3.7, 4.2.2.3, 4.2.4, 4.3.3.
//:   Abbreviation of representations by omission of lower order components of
//:   dates or years are not supported, except that fractions of a second are
//:   optional.
//: o Expanded Representation: 2.3.8, 4.1.3.3.  Years outside the range
//:   '0000 - 9999'.  In fact, only the range '0001 - 9999' is supported.
//: o Fractions are only permissible demarked by a '.', commas ',' are not
//:   allowed 4.2.2.4.  Fractions of hours or minutes are not allowed; only
//:   fractions of seconds are allowed.
//: o The time designator 'T' is only allowed within 'Datetime's and
//:   'DatetimeTz's to delimit the end of the date from the beginning of the
//:   time.  4.2.2.5.
//: o 4.3.2 says that the 'T' delimiting the end of date and beginning of time
//:   in a 'Datetime' or 'DatetimeTz' is optional.  In this implementation the
//:   'T' is mandatory.
//: o We do not support Time Intervals, Durations: 4.4.
//: o We do not support Recurring Time Intervals: 4.5.
//
///Usage
///-----
// The following example illustrates how to generate an ISO 8601-compliant
// string from a 'bdet_DatetimeTz' value:
//..
//  const bdet_DatetimeTz theDatetime(bdet_Datetime(2005, 1, 31,
//                                                  8, 59, 59, 123), 240);
//  bsl::stringstream ss;
//  bdepu_Iso8601::generate(ss, theDatetime);
//  assert(ss.str() == "2005-01-31T08:59:59.123+04:00");
//..
// The following example illustrates how to parse an ISO 8601-compliant string
// into a 'bdet_DatetimeTz' object:
//..
//  bdet_DatetimeTz dateTime;
//  const char dtStr[] = "2005-01-31T08:59:59.123+04:00";
//  int ret = bdepu_Iso8601::parse(&dateTime, dtStr, bsl::strlen(dtStr));
//  assert(0 == ret);
//  assert(2005 == dateTime.localDatetime().year());
//  assert(   1 == dateTime.localDatetime().month());
//  assert(  31 == dateTime.localDatetime().day());
//  assert(   8 == dateTime.localDatetime().hour());
//  assert(  59 == dateTime.localDatetime().minute());
//  assert(  59 == dateTime.localDatetime().second());
//  assert( 123 == dateTime.localDatetime().millisecond());
//  assert( 240 == dateTime.offset());
//..
// Note that fractions of a second was rounded up to 123 milliseconds and that
// the offset from UTC was converted to minutes.

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSL_OSTREAM
#include <bsl_ostream.h>
#endif

namespace BloombergLP {

class bdet_Date;
class bdet_Datetime;
class bdet_DatetimeTz;
class bdet_DateTz;
class bdet_Time;
class bdet_TimeTz;

                            // ====================
                            // struct bdepu_Iso8601
                            // ====================

struct bdepu_Iso8601 {
    // namespace for ISO8601 date/time conversion functions

    // TYPES
    enum {
        // Fixed lengths for iso8601-formatted date and time values:
        BDEPU_DATE_STRLEN         = 10,
        BDEPU_DATETIME_STRLEN     = 23,
        BDEPU_DATETIMETZ_STRLEN   = 29,
        BDEPU_DATETZ_STRLEN       = 16,
        BDEPU_TIME_STRLEN         = 12,
        BDEPU_TIMETZ_STRLEN       = 18,
        BDEPU_MAX_DATETIME_STRLEN = BDEPU_DATETIMETZ_STRLEN

#ifndef BDE_OMIT_INTERNAL_DEPRECATED
      , DATE_STRLEN         = BDEPU_DATE_STRLEN
      , DATETIME_STRLEN     = BDEPU_DATETIME_STRLEN
      , DATETIMETZ_STRLEN   = BDEPU_DATETIMETZ_STRLEN
      , DATETZ_STRLEN       = BDEPU_DATETZ_STRLEN
      , TIME_STRLEN         = BDEPU_TIME_STRLEN
      , TIMETZ_STRLEN       = BDEPU_TIMETZ_STRLEN
      , MAX_DATETIME_STRLEN = BDEPU_MAX_DATETIME_STRLEN
#endif // BDE_OMIT_INTERNAL_DEPRECATED
    };

    // CLASS METHODS
    static int generate(char             *buffer,
                        const bdet_Date&  object,
                        int               bufferLength);
    static int generate(char                 *buffer,
                        const bdet_Datetime&  object,
                        int                   bufferLength);
    static int generate(char                   *buffer,
                        const bdet_DatetimeTz&  object,
                        int                     bufferLength);
    static int generate(char               *buffer,
                        const bdet_DateTz&  object,
                        int                 bufferLength);
    static int generate(char             *buffer,
                        const bdet_Time&  object,
                        int               bufferLength);
    static int generate(char               *buffer,
                        const bdet_TimeTz&  object,
                        int                 bufferLength);
        // Write the ISO8601 representation of the specified 'object' to
        // specified 'buffer' truncating (if necessary) to the specified
        // 'bufferLength', and return the length of the formatted string
        // before truncation (not including a null terminator).  If
        // 'bufferLength' is larger than necessary to contain the string
        // representation of 'object', then a null terminator is appended to
        // the output.  The behavior is undefined unless '0 <= bufferLength'.
        // Note that, for each type of 'object', the return value is always the
        // same (as enumerated in the '_STRLEN' constants).  Also note that a
        // buffer of size 'BDEPU_MAX_DATETIME_STRLEN + 1' is large enough to
        // hold any formatted string, including the null terminator.  Also
        // note that the output from a 'generate' routine can always be parsed
        // by the corresponding 'parse' routine without loss of accuracy,
        // provided 'bufferLength' is sufficiently large.

    static int generateRaw(char             *buffer,
                           const bdet_Date&  object);
    static int generateRaw(char                 *buffer,
                           const bdet_Datetime&  object);
    static int generateRaw(char                   *buffer,
                           const bdet_DatetimeTz&  object);
    static int generateRaw(char               *buffer,
                           const bdet_DateTz&  object);
    static int generateRaw(char             *buffer,
                           const bdet_Time&  object);
    static int generateRaw(char               *buffer,
                           const bdet_TimeTz&  object);
        // Write the ISO8601 representation of the specified 'object' to
        // specified 'buffer' and return the length of the formatted string
        // (not including a null terminator).  The behavior is undefined unless
        // 'buffer' holds enough characters.  Note that, for each type of
        // 'object', the return value is always the same (as enumerated in the
        // '_STRLEN' constants).  Also note that a buffer of size
        // 'BDEPU_MAX_DATETIME_STRLEN + 1' is large enough to hold any
        // formatted string, including any null terminator.  It is unspecified
        // whether the 'buffer' is null terminated on output or not.

    template <typename DATE_TYPE>
    static bsl::ostream& generate(bsl::ostream&    stream,
                                  const DATE_TYPE& object);
        // Write the ISO8601 representation of the specified 'object' to
        // specified 'stream'.  Return a reference to the modifiable
        // 'stream'.

    static int parse(bdet_Date  *result,
                     const char *input,
                     int         inputLength);
        // Attempt to parse the specified 'inputLength' characters of the
        // specified 'input' as a date, and if successful load this date into
        // the specified 'result'.  Parsing is successful if the input is of
        // the format "YYYY-MM-DD{((+|-)hh:mm|Z|z)}" where 'YYYY', 'MM', and
        // 'DD' are strings representing positive integers, '-' is literally a
        // dash character, 'YYYY' is 4 chars long, and 'MM' and 'DD' are both 2
        // chars long.  'YYYY' must be in the range '[ 0001, 9999 ]', 'MM' must
        // be in the range [ 01, 12 ], and 'DD' must be in the range [ 01, 31
        // ], such that 'YYYY-MM-DD' represents a valid date.  Optional time
        // zone information may be provided, in which case it is parsed and
        // ignored.  Do not modify '*result' on failure.  Return 0 if parsing
        // is successful, and return a non-zero value (and do not modify
        // 'result') otherwise.  Note that this function parses *exactly*
        // 'inputLength' characters; parsing will fail if a subset of the
        // passed string matches the specified format and is then followed by
        // trailing characters.

    static int parse(bdet_Time  *result,
                     const char *input,
                     int         inputLength);
        // Parse a time, represented in the "hh:mm:ss{.d+}{((+|-)hh:mm|Z|z)}"
        // format, from the specified 'input' of the specified 'inputLength'
        // and load it into the specified '*result'.  In the "hh:mm:ss{.d+}"
        // format accepted by this function, 'hh', 'mm', 'ss' are all 2 digit
        // integers (left padded with 0's if necessary) denoting hours,
        // minutes, and seconds, ':' is literally a colon character, and {.d+}
        // is the optional fraction of a second, consisting of a '.' followed
        // by one or more decimal digits.  'hh' must be in the range
        // '[ 00, 24 )', 'mm' must be in the range '[ 00, 60 )', and 'ss' must
        // be in the range '[ 00, 60 )'.  If '{.d+}' contains more than 3
        // digits, the value will be rounded to the nearest value in
        // milliseconds, possibly rounding '*result' up a full second.
        // Optional time zone information may be provided, in which case
        // '*result' is converted to the equivalent GMT time.  An exceptional
        // time value of '24:00:00' may be provided, in which case the fraction
        // of a second must be 0 and the time zone must be absent or GMT.  Do
        // not modify '*result' on failure.  Return 0 on success, and a
        // non-zero otherwise.  Note that if 'inputLength' is longer than the
        // length of the parsed data, parsing will fail.

    static int parse(bdet_Datetime *result,
                     const char    *input,
                     int            inputLength);
        // Parse a date time, represented in the
        // "YYYY-MM-DDThh:mm:ss{.d+}{((+|-)hh:mm|Z|z)}" format, from the
        // specified 'input' of the specified 'inputLength' and load it into
        // the specified '*result'.  In the "YYYY-MM-DD" format accepted by
        // this function, 'YYYY', 'MM', and 'DD' are strings representing
        // positive integers, '-' is literally a dash character, 'YYYY' is 4
        // chars long, and 'MM' and 'DD' are both 2 chars long.  'YYYY' must be
        // in the range '[ 0001, 9999 ]', 'MM' must be in the range [ 01, 12 ],
        // and 'DD' must be in the range [ 01, 31 ], such that 'YYYY-MM-DD'
        // represents a valid date.  'T' literally represents the 'T'
        // character.  In the "hh:mm:ss{.d+}" format, 'hh', 'mm', 'ss' are all
        // 2 digit integers (left padded with 0's if necessary) denoting hours,
        // minutes, and seconds, ':' is literally a colon character, and {.d+}
        // is the optional fraction of a second, consisting of a '.' followed
        // by one or more decimal digits.  'hh' must be in the range
        // '[ 00, 24 )', 'mm' must be in the range '[ 00, 60 )', and 'ss' must
        // be in the range '[ 00, 60 )'.  If '{.d+}' contains more than 3
        // digits, the value will be rounded to the nearest value in
        // milliseconds, possibly resulting in time being rounded up a full
        // second.  Optional time zone information may be provided, in which
        // case '*result' is converted to the equivalent GMT time.  An
        // exceptional case is that a time value of '24:00:00' is allowed, but
        // only if the fraction of a second is 0 and the time zone is absent or
        // GMT.  Do not modify '*result' on failure.  Return 0 on success, and
        // a non-zero value otherwise.  Note that if 'inputLength' is longer
        // than the length of the parsed data, parsing will fail.

    static int parse(bdet_DateTz *result,
                     const char  *input,
                     int          inputLength);
        // Parse a date, represented in the "YYYY-MM-DD{((+|-)hh:mm|Z|z)}"
        // format, from the specified 'input' of the specified 'inputLength'
        // and load it into the specified '*result'.  In the "YYYY-MM-DD"
        // format accepted by this function, 'YYYY', 'MM', and 'DD' are strings
        // representing positive integers, '-' is literally a dash character,
        // 'YYYY' is 4 chars long, and 'MM' and 'DD' are both 2 chars long.
        // 'YYYY' must be in the range '[ 0001, 9999 ]', 'MM' must be in the
        // range [ 01, 12 ], and 'DD' must be in the range [ 01, 31 ], such
        // that 'YYYY-MM-DD' represents a valid date.  Optional time zone
        // information may be provided in the "Shh:mm" format accepted by this
        // function, 'hh' and 'mm' are 2 digit integers (left padded with '0's
        // if necessary).  'hh' must be in the range '[ 00, 24 )' and 'mm' must
        // be in the range '[ 00, 60 )'.  An alternate form of the
        // representation for the time zone is 'Z' or 'z', signifying GMT.  If
        // no time zone is provided, GMT is assumed.  Do not modify '*result'
        // on failure.  Return 0 on success, and a non-zero value otherwise.
        // Note that if 'inputLength' is longer than the length of the parsed
        // data, parsing will fail.

    static int parse(bdet_TimeTz *result,
                     const char  *input,
                     int          inputLength);
        // Parse a time, represented in the "hh:mm:ss{.d+}{((+|-)hh:mm|Z|z)}"
        // format, from the specified 'input' of the specified 'inputLength'
        // and load it into the specified '*result'.  In the "hh:mm:ss{.d+}"
        // format accepted by this function, 'hh', 'mm', 'ss' are all 2 digit
        // integers (left padded with 0's if necessary) denoting hours,
        // minutes, and seconds, ':' is literally a colon character, and {.d+}
        // is the optional fraction of a second, consisting of a '.' followed
        // by one or more decimal digits.  'hh' must be in the range
        // '[ 00, 24 )', 'mm' must be in the range '[ 00, 60 )', and 'ss' must
        // be in the range '[ 00, 60 )'.  If '{.d+}' contains more than 3
        // digits, the value will be rounded to the nearest value in
        // milliseconds, possibly rounding '*result' up a full second.
        // Optional time zone information may be provided in the "Shh:mm"
        // format accepted by this function, 'hh' and 'mm' are 2 digit integers
        // (left padded with '0's if necessary).  'hh' must be in the range
        // '[ 00, 24 )' and 'mm' must be in the range '[ 0, 60 )'.  An
        // alternate form of the representation for the time zone is 'Z' or
        // 'z', signifying GMT.  If time zone information is not provided, GMT
        // is assumed.  An exceptional time value of '24:00:00' may be
        // provided, in which case the fraction of a second must be 0 and the
        // time zone, if present, must be GMT.  Do not modify '*result' on
        // failure.  Return 0 on success, and a non-zero value otherwise.  Note
        // that if 'inputLength' is longer than the length of the parsed data,
        // parsing will fail.

    static int parse(bdet_DatetimeTz *result,
                     const char      *input,
                     int              inputLength);
        // Parse a date time, represented in the
        // "YYYY-MM-DDThh:mm:ss{.d+}{((+|-)hh:mm|Z|z)}" format, from the
        // specified 'input' of the specified 'inputLength' and load it into
        // the specified '*result'.  In the "YYYY-MM-DD" format accepted by
        // this function, 'YYYY', 'MM', and 'DD' are strings representing
        // positive integers, '-' is literally a dash character, 'YYYY' is 4
        // chars long, and 'MM' and 'DD' are both 2 chars long.  'YYYY' must be
        // in the range '[ 0001, 9999 ]', 'MM' must be in the range [ 01, 12 ],
        // and 'DD' must be in the range [ 01, 31 ], such that 'YYYY-MM-DD'
        // represents a valid date.  'T' literally represents the 'T'
        // character.  In the "hh:mm:ss{.d+}" format, 'hh', 'mm', 'ss' are all
        // 2 digit integers (left padded with 0's if necessary) denoting hours,
        // minutes, and seconds, ':' is literally a colon character, and {.d+}
        // is the optional fraction of a second, consisting of a '.' followed
        // by one or more decimal digits.  If '{.d+}' contains more than 3
        // digits, the value will be rounded to the nearest value in
        // milliseconds, possibly rounding '*result' up by a full second.  'hh'
        // must be in the range '[ 00, 24 )', 'mm' must be in the range
        // '[ 00, 60 )', and 'ss' must be in the range '[ 00, 60 )'.  The time
        // zone information is optional but if it is provided then it must be
        // in the "Shh:mm" format, 'hh' and 'mm' are 2 digit integers (left
        // padded with '0's if necessary).  'hh' must be in the range
        // '[ 00, 24 )' and 'mm' must be in the range '[ 00, 60 )'.  An
        // alternate form of representing the time zone is 'Z' or 'z',
        // signifying GMT.  If the time zone is not provided, GMT is assumed.
        // Do not modify '*result' on failure.  An exceptional time value of
        // '24:00;00' may be provided, but if so the fraction of a second must
        // be 0 and time zone, if any, must be GMT.  Return 0 on success, and a
        // non-zero value otherwise.  Note that if 'inputLength' is longer than
        // the length of the parsed data, parsing will fail.
};

// ===========================================================================
//                      INLINE FUNCTION DEFINITIONS
// ===========================================================================

template <typename DATE_TYPE>
inline
bsl::ostream& bdepu_Iso8601::generate(bsl::ostream&    stream,
                                      const DATE_TYPE& object)
{
    char buffer[BDEPU_MAX_DATETIME_STRLEN + 1];
    int len = generate(buffer, object, BDEPU_MAX_DATETIME_STRLEN);
    BSLS_ASSERT_SAFE(BDEPU_MAX_DATETIME_STRLEN >= len);
    stream.write(buffer, len);
    return stream;
}

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2005
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
